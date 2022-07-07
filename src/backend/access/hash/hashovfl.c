/*-------------------------------------------------------------------------
 *
 * hashovfl.c
 *	  Overflow page management code for the Postgres hash access method
 *
 * Portions Copyright (c) 1996-2017, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/access/hash/hashovfl.c
 *
 * NOTES
 *	  Overflow pages look like ordinary relation pages.
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/hash.h"
#include "utils/rel.h"


static Buffer _hash_getovflpage(Relation rel, Buffer metabuf);
static uint32 _hash_firstfreebit(uint32 map);


/*
 * Convert overflow page bit number (its index in the free-page bitmaps)
 * to block number within the index.
 */
static BlockNumber
bitno_to_blkno(HashMetaPage metap, uint32 ovflbitnum)
{
	uint32		splitnum = metap->hashm_ovflpoint;
	uint32		i;

	/* Convert zero-based bitnumber to 1-based page number */
	ovflbitnum += 1;

	/* Determine the split number for this page (must be >= 1) */
	for (i = 1;
		 i < splitnum && ovflbitnum > metap->hashm_spares[i];
		 i++)
		 /* loop */ ;

	/*
	 * Convert to absolute page number by adding the number of bucket pages
	 * that exist before this split point.
	 */
	return (BlockNumber) ((1 << i) + ovflbitnum);
}

/*
 * _hash_ovflblkno_to_bitno
 *
 * Convert overflow page block number to bit number for free-page bitmap.
 */
uint32
_hash_ovflblkno_to_bitno(HashMetaPage metap, BlockNumber ovflblkno)
{
	uint32		splitnum = metap->hashm_ovflpoint;
	uint32		i;
	uint32		bitnum;

	/* Determine the split number containing this page */
	for (i = 1; i <= splitnum; i++)
	{
		if (ovflblkno <= (BlockNumber) (1 << i))
			break;				/* oops */
		bitnum = ovflblkno - (1 << i);
		if (bitnum <= metap->hashm_spares[i])
			return bitnum - 1;	/* -1 to convert 1-based to 0-based */
	}

	elog(ERROR, "invalid overflow block number %u", ovflblkno);
	return 0;					/* keep compiler quiet */
}

/*
 *	_hash_addovflpage
 *
 *	Add an overflow page to the bucket whose last page is pointed to by 'buf'.
 *
 *	On entry, the caller must hold a pin but no lock on 'buf'.  The pin is
 *	dropped before exiting (we assume the caller is not interested in 'buf'
 *	anymore) if not asked to retain.  The pin will be retained only for the
 *	primary bucket.  The returned overflow page will be pinned and
 *	write-locked; it is guaranteed to be empty.
 *
 *	The caller must hold a pin, but no lock, on the metapage buffer.
 *	That buffer is returned in the same state.
 *
 * NB: since this could be executed concurrently by multiple processes,
 * one should not assume that the returned overflow page will be the
 * immediate successor of the originally passed 'buf'.  Additional overflow
 * pages might have been added to the bucket chain in between.
 */
Buffer
_hash_addovflpage(Relation rel, Buffer metabuf, Buffer buf, bool retain_pin)
{
	Buffer		ovflbuf;
	Page		page;
	Page		ovflpage;
	HashPageOpaque pageopaque;
	HashPageOpaque ovflopaque;

	/* allocate and lock an empty overflow page */
	ovflbuf = _hash_getovflpage(rel, metabuf);

	/*
	 * Write-lock the tail page.  It is okay to hold two buffer locks here
	 * since there cannot be anyone else contending for access to ovflbuf.
	 */
	LockBuffer(buf, BUFFER_LOCK_EXCLUSIVE);

	/* probably redundant... */
	_hash_checkpage(rel, buf, LH_BUCKET_PAGE | LH_OVERFLOW_PAGE);

	/* loop to find current tail page, in case someone else inserted too */
	for (;;)
	{
		BlockNumber nextblkno;

		page = BufferGetPage(buf);
		pageopaque = (HashPageOpaque) PageGetSpecialPointer(page);
		nextblkno = pageopaque->hasho_nextblkno;

		if (!BlockNumberIsValid(nextblkno))
			break;

		/* we assume we do not need to write the unmodified page */
		if (retain_pin)
		{
			/* pin will be retained only for the primary bucket page */
			Assert(pageopaque->hasho_flag & LH_BUCKET_PAGE);
			LockBuffer(buf, BUFFER_LOCK_UNLOCK);
		}
		else
			_hash_relbuf(rel, buf);

		retain_pin = false;

		buf = _hash_getbuf(rel, nextblkno, HASH_WRITE, LH_OVERFLOW_PAGE);
	}

	/* now that we have correct backlink, initialize new overflow page */
	ovflpage = BufferGetPage(ovflbuf);
	ovflopaque = (HashPageOpaque) PageGetSpecialPointer(ovflpage);
	ovflopaque->hasho_prevblkno = BufferGetBlockNumber(buf);
	ovflopaque->hasho_nextblkno = InvalidBlockNumber;
	ovflopaque->hasho_bucket = pageopaque->hasho_bucket;
	ovflopaque->hasho_flag = LH_OVERFLOW_PAGE;
	ovflopaque->hasho_page_id = HASHO_PAGE_ID;

	MarkBufferDirty(ovflbuf);

	/* logically chain overflow page to previous page */
	pageopaque->hasho_nextblkno = BufferGetBlockNumber(ovflbuf);
	MarkBufferDirty(buf);
	if (retain_pin)
	{
		/* pin will be retained only for the primary bucket page */
		Assert(pageopaque->hasho_flag & LH_BUCKET_PAGE);
		LockBuffer(buf, BUFFER_LOCK_UNLOCK);
	}
	else
		_hash_relbuf(rel, buf);

	return ovflbuf;
}

/*
 *	_hash_getovflpage()
 *
 *	Find an available overflow page and return it.  The returned buffer
 *	is pinned and write-locked, and has had _hash_pageinit() applied,
 *	but it is caller's responsibility to fill the special space.
 *
 * The caller must hold a pin, but no lock, on the metapage buffer.
 * That buffer is left in the same state at exit.
 */
static Buffer
_hash_getovflpage(Relation rel, Buffer metabuf)
{
	HashMetaPage metap;
	Buffer		mapbuf = 0;
	Buffer		newbuf;
	BlockNumber blkno;
	uint32		orig_firstfree;
	uint32		splitnum;
	uint32	   *freep = NULL;
	uint32		max_ovflpg;
	uint32		bit;
	uint32		first_page;
	uint32		last_bit;
	uint32		last_page;
	uint32		i,
				j;

	/* Get exclusive lock on the meta page */
	LockBuffer(metabuf, BUFFER_LOCK_EXCLUSIVE);

	_hash_checkpage(rel, metabuf, LH_META_PAGE);
	metap = HashPageGetMeta(BufferGetPage(metabuf));

	/* start search at hashm_firstfree */
	orig_firstfree = metap->hashm_firstfree;
	first_page = orig_firstfree >> BMPG_SHIFT(metap);
	bit = orig_firstfree & BMPG_MASK(metap);
	i = first_page;
	j = bit / BITS_PER_MAP;
	bit &= ~(BITS_PER_MAP - 1);

	/* outer loop iterates once per bitmap page */
	for (;;)
	{
		BlockNumber mapblkno;
		Page		mappage;
		uint32		last_inpage;

		/* want to end search with the last existing overflow page */
		splitnum = metap->hashm_ovflpoint;
		max_ovflpg = metap->hashm_spares[splitnum] - 1;
		last_page = max_ovflpg >> BMPG_SHIFT(metap);
		last_bit = max_ovflpg & BMPG_MASK(metap);

		if (i > last_page)
			break;

		Assert(i < metap->hashm_nmaps);
		mapblkno = metap->hashm_mapp[i];

		if (i == last_page)
			last_inpage = last_bit;
		else
			last_inpage = BMPGSZ_BIT(metap) - 1;

		/* Release exclusive lock on metapage while reading bitmap page */
		LockBuffer(metabuf, BUFFER_LOCK_UNLOCK);

		mapbuf = _hash_getbuf(rel, mapblkno, HASH_WRITE, LH_BITMAP_PAGE);
		mappage = BufferGetPage(mapbuf);
		freep = HashPageGetBitmap(mappage);

		for (; bit <= last_inpage; j++, bit += BITS_PER_MAP)
		{
			if (freep[j] != ALL_SET)
				goto found;
		}

		/* No free space here, try to advance to next map page */
		_hash_relbuf(rel, mapbuf);
		i++;
		j = 0;					/* scan from start of next map page */
		bit = 0;

		/* Reacquire exclusive lock on the meta page */
		LockBuffer(metabuf, BUFFER_LOCK_EXCLUSIVE);
	}

	/*
	 * No free pages --- have to extend the relation to add an overflow page.
	 * First, check to see if we have to add a new bitmap page too.
	 */
	if (last_bit == (uint32) (BMPGSZ_BIT(metap) - 1))
	{
		/*
		 * We create the new bitmap page with all pages marked "in use".
		 * Actually two pages in the new bitmap's range will exist
		 * immediately: the bitmap page itself, and the following page which
		 * is the one we return to the caller.  Both of these are correctly
		 * marked "in use".  Subsequent pages do not exist yet, but it is
		 * convenient to pre-mark them as "in use" too.
		 */
		bit = metap->hashm_spares[splitnum];
		_hash_initbitmap(rel, metap, bitno_to_blkno(metap, bit), MAIN_FORKNUM);
		metap->hashm_spares[splitnum]++;
	}
	else
	{
		/*
		 * Nothing to do here; since the page will be past the last used page,
		 * we know its bitmap bit was preinitialized to "in use".
		 */
	}

	/* Calculate address of the new overflow page */
	bit = metap->hashm_spares[splitnum];
	blkno = bitno_to_blkno(metap, bit);

	/*
	 * Fetch the page with _hash_getnewbuf to ensure smgr's idea of the
	 * relation length stays in sync with ours.  XXX It's annoying to do this
	 * with metapage write lock held; would be better to use a lock that
	 * doesn't block incoming searches.
	 */
	newbuf = _hash_getnewbuf(rel, blkno, MAIN_FORKNUM);

	metap->hashm_spares[splitnum]++;

	/*
	 * Adjust hashm_firstfree to avoid redundant searches.  But don't risk
	 * changing it if someone moved it while we were searching bitmap pages.
	 */
	if (metap->hashm_firstfree == orig_firstfree)
		metap->hashm_firstfree = bit + 1;

	/* Write updated metapage and release lock, but not pin */
	MarkBufferDirty(metabuf);
	LockBuffer(metabuf, BUFFER_LOCK_UNLOCK);

	return newbuf;

found:
	/* convert bit to bit number within page */
	bit += _hash_firstfreebit(freep[j]);

	/* mark page "in use" in the bitmap */
	SETBIT(freep, bit);
	MarkBufferDirty(mapbuf);
	_hash_relbuf(rel, mapbuf);

	/* Reacquire exclusive lock on the meta page */
	LockBuffer(metabuf, BUFFER_LOCK_EXCLUSIVE);

	/* convert bit to absolute bit number */
	bit += (i << BMPG_SHIFT(metap));

	/* Calculate address of the recycled overflow page */
	blkno = bitno_to_blkno(metap, bit);

	/*
	 * Adjust hashm_firstfree to avoid redundant searches.  But don't risk
	 * changing it if someone moved it while we were searching bitmap pages.
	 */
	if (metap->hashm_firstfree == orig_firstfree)
	{
		metap->hashm_firstfree = bit + 1;

		/* Write updated metapage and release lock, but not pin */
		MarkBufferDirty(metabuf);
		LockBuffer(metabuf, BUFFER_LOCK_UNLOCK);
	}
	else
	{
		/* We didn't change the metapage, so no need to write */
		LockBuffer(metabuf, BUFFER_LOCK_UNLOCK);
	}

	/* Fetch, init, and return the recycled page */
	return _hash_getinitbuf(rel, blkno);
}

/*
 *	_hash_firstfreebit()
 *
 *	Return the number of the first bit that is not set in the word 'map'.
 */
static uint32
_hash_firstfreebit(uint32 map)
{
	uint32		i,
				mask;

	mask = 0x1;
	for (i = 0; i < BITS_PER_MAP; i++)
	{
		if (!(mask & map))
			return i;
		mask <<= 1;
	}

	elog(ERROR, "firstfreebit found no free bit");

	return 0;					/* keep compiler quiet */
}

/*
 *	_hash_freeovflpage() -
 *
 *	Remove this overflow page from its bucket's chain, and mark the page as
 *	free.  On entry, ovflbuf is write-locked; it is released before exiting.
 *
 *	Since this function is invoked in VACUUM, we provide an access strategy
 *	parameter that controls fetches of the bucket pages.
 *
 *	Returns the block number of the page that followed the given page
 *	in the bucket, or InvalidBlockNumber if no following page.
 *
 *	NB: caller must not hold lock on metapage, nor on page, that's next to
 *	ovflbuf in the bucket chain.  We don't acquire the lock on page that's
 *	prior to ovflbuf in chain if it is same as wbuf because the caller already
 *	has a lock on same.
 */
BlockNumber
_hash_freeovflpage(Relation rel, Buffer ovflbuf, Buffer wbuf,
				   BufferAccessStrategy bstrategy)
{
	HashMetaPage metap;
	Buffer		metabuf;
	Buffer		mapbuf;
	Buffer		prevbuf = InvalidBuffer;
	BlockNumber ovflblkno;
	BlockNumber prevblkno;
	BlockNumber blkno;
	BlockNumber nextblkno;
	BlockNumber writeblkno;
	HashPageOpaque ovflopaque;
	Page		ovflpage;
	Page		mappage;
	uint32	   *freep;
	uint32		ovflbitno;
	int32		bitmappage,
				bitmapbit;
	Bucket bucket PG_USED_FOR_ASSERTS_ONLY;

	/* Get information from the doomed page */
	_hash_checkpage(rel, ovflbuf, LH_OVERFLOW_PAGE);
	ovflblkno = BufferGetBlockNumber(ovflbuf);
	ovflpage = BufferGetPage(ovflbuf);
	ovflopaque = (HashPageOpaque) PageGetSpecialPointer(ovflpage);
	nextblkno = ovflopaque->hasho_nextblkno;
	prevblkno = ovflopaque->hasho_prevblkno;
	writeblkno = BufferGetBlockNumber(wbuf);
	bucket = ovflopaque->hasho_bucket;

	/*
	 * Zero the page for debugging's sake; then write and release it. (Note:
	 * if we failed to zero the page here, we'd have problems with the Assert
	 * in _hash_pageinit() when the page is reused.)
	 */
	MemSet(ovflpage, 0, BufferGetPageSize(ovflbuf));
	MarkBufferDirty(ovflbuf);
	_hash_relbuf(rel, ovflbuf);

	/*
	 * Fix up the bucket chain.  this is a doubly-linked list, so we must fix
	 * up the bucket chain members behind and ahead of the overflow page being
	 * deleted.  Concurrency issues are avoided by using lock chaining as
	 * described atop hashbucketcleanup.
	 */
	if (BlockNumberIsValid(prevblkno))
	{
		Page		prevpage;
		HashPageOpaque prevopaque;

		if (prevblkno == writeblkno)
			prevbuf = wbuf;
		else
			prevbuf = _hash_getbuf_with_strategy(rel,
												 prevblkno,
												 HASH_WRITE,
										   LH_BUCKET_PAGE | LH_OVERFLOW_PAGE,
												 bstrategy);

		prevpage = BufferGetPage(prevbuf);
		prevopaque = (HashPageOpaque) PageGetSpecialPointer(prevpage);

		Assert(prevopaque->hasho_bucket == bucket);
		prevopaque->hasho_nextblkno = nextblkno;

		MarkBufferDirty(prevbuf);
		if (prevblkno != writeblkno)
			_hash_relbuf(rel, prevbuf);
	}
	if (BlockNumberIsValid(nextblkno))
	{
		Buffer		nextbuf = _hash_getbuf_with_strategy(rel,
														 nextblkno,
														 HASH_WRITE,
														 LH_OVERFLOW_PAGE,
														 bstrategy);
		Page		nextpage = BufferGetPage(nextbuf);
		HashPageOpaque nextopaque = (HashPageOpaque) PageGetSpecialPointer(nextpage);

		Assert(nextopaque->hasho_bucket == bucket);
		nextopaque->hasho_prevblkno = prevblkno;
		MarkBufferDirty(nextbuf);
		_hash_relbuf(rel, nextbuf);
	}

	/* Note: bstrategy is intentionally not used for metapage and bitmap */

	/* Read the metapage so we can determine which bitmap page to use */
	metabuf = _hash_getbuf(rel, HASH_METAPAGE, HASH_READ, LH_META_PAGE);
	metap = HashPageGetMeta(BufferGetPage(metabuf));

	/* Identify which bit to set */
	ovflbitno = _hash_ovflblkno_to_bitno(metap, ovflblkno);

	bitmappage = ovflbitno >> BMPG_SHIFT(metap);
	bitmapbit = ovflbitno & BMPG_MASK(metap);

	if (bitmappage >= metap->hashm_nmaps)
		elog(ERROR, "invalid overflow bit number %u", ovflbitno);
	blkno = metap->hashm_mapp[bitmappage];

	/* Release metapage lock while we access the bitmap page */
	LockBuffer(metabuf, BUFFER_LOCK_UNLOCK);

	/* Clear the bitmap bit to indicate that this overflow page is free */
	mapbuf = _hash_getbuf(rel, blkno, HASH_WRITE, LH_BITMAP_PAGE);
	mappage = BufferGetPage(mapbuf);
	freep = HashPageGetBitmap(mappage);
	Assert(ISSET(freep, bitmapbit));
	CLRBIT(freep, bitmapbit);
	MarkBufferDirty(mapbuf);
	_hash_relbuf(rel, mapbuf);

	/* Get write-lock on metapage to update firstfree */
	LockBuffer(metabuf, BUFFER_LOCK_EXCLUSIVE);

	/* if this is now the first free page, update hashm_firstfree */
	if (ovflbitno < metap->hashm_firstfree)
	{
		metap->hashm_firstfree = ovflbitno;
		MarkBufferDirty(metabuf);
	}
	_hash_relbuf(rel, metabuf);

	return nextblkno;
}


/*
 *	_hash_initbitmap()
 *
 *	 Initialize a new bitmap page.  The metapage has a write-lock upon
 *	 entering the function, and must be written by caller after return.
 *
 * 'blkno' is the block number of the new bitmap page.
 *
 * All bits in the new bitmap page are set to "1", indicating "in use".
 */
void
_hash_initbitmap(Relation rel, HashMetaPage metap, BlockNumber blkno,
				 ForkNumber forkNum)
{
	Buffer		buf;
	Page		pg;
	HashPageOpaque op;
	uint32	   *freep;

	/*
	 * It is okay to write-lock the new bitmap page while holding metapage
	 * write lock, because no one else could be contending for the new page.
	 * Also, the metapage lock makes it safe to extend the index using
	 * _hash_getnewbuf.
	 *
	 * There is some loss of concurrency in possibly doing I/O for the new
	 * page while holding the metapage lock, but this path is taken so seldom
	 * that it's not worth worrying about.
	 */
	buf = _hash_getnewbuf(rel, blkno, forkNum);
	pg = BufferGetPage(buf);

	/* initialize the page's special space */
	op = (HashPageOpaque) PageGetSpecialPointer(pg);
	op->hasho_prevblkno = InvalidBlockNumber;
	op->hasho_nextblkno = InvalidBlockNumber;
	op->hasho_bucket = -1;
	op->hasho_flag = LH_BITMAP_PAGE;
	op->hasho_page_id = HASHO_PAGE_ID;

	/* set all of the bits to 1 */
	freep = HashPageGetBitmap(pg);
	MemSet(freep, 0xFF, BMPGSZ_BYTE(metap));

	/* dirty the new bitmap page, and release write lock and pin */
	MarkBufferDirty(buf);
	_hash_relbuf(rel, buf);

	/* add the new bitmap page to the metapage's list of bitmaps */
	/* metapage already has a write lock */
	if (metap->hashm_nmaps >= HASH_MAX_BITMAPS)
		ereport(ERROR,
				(errcode(ERRCODE_PROGRAM_LIMIT_EXCEEDED),
				 errmsg("out of overflow pages in hash index \"%s\"",
						RelationGetRelationName(rel))));

	metap->hashm_mapp[metap->hashm_nmaps] = blkno;

	metap->hashm_nmaps++;
}


/*
 *	_hash_squeezebucket(rel, bucket)
 *
 *	Try to squeeze the tuples onto pages occurring earlier in the
 *	bucket chain in an attempt to free overflow pages. When we start
 *	the "squeezing", the page from which we start taking tuples (the
 *	"read" page) is the last bucket in the bucket chain and the page
 *	onto which we start squeezing tuples (the "write" page) is the
 *	first page in the bucket chain.  The read page works backward and
 *	the write page works forward; the procedure terminates when the
 *	read page and write page are the same page.
 *
 *	At completion of this procedure, it is guaranteed that all pages in
 *	the bucket are nonempty, unless the bucket is totally empty (in
 *	which case all overflow pages will be freed).  The original implementation
 *	required that to be true on entry as well, but it's a lot easier for
 *	callers to leave empty overflow pages and let this guy clean it up.
 *
 *	Caller must acquire cleanup lock on the primary page of the target
 *	bucket to exclude any scans that are in progress, which could easily
 *	be confused into returning the same tuple more than once or some tuples
 *	not at all by the rearrangement we are performing here.  To prevent
 *	any concurrent scan to cross the squeeze scan we use lock chaining
 *	similar to hasbucketcleanup.  Refer comments atop hashbucketcleanup.
 *
 *	We need to retain a pin on the primary bucket to ensure that no concurrent
 *	split can start.
 *
 *	Since this function is invoked in VACUUM, we provide an access strategy
 *	parameter that controls fetches of the bucket pages.
 */
void
_hash_squeezebucket(Relation rel,
					Bucket bucket,
					BlockNumber bucket_blkno,
					Buffer bucket_buf,
					BufferAccessStrategy bstrategy)
{
	BlockNumber wblkno;
	BlockNumber rblkno;
	Buffer		wbuf;
	Buffer		rbuf;
	Page		wpage;
	Page		rpage;
	HashPageOpaque wopaque;
	HashPageOpaque ropaque;
	bool		wbuf_dirty;

	/*
	 * start squeezing into the primary bucket page.
	 */
	wblkno = bucket_blkno;
	wbuf = bucket_buf;
	wpage = BufferGetPage(wbuf);
	wopaque = (HashPageOpaque) PageGetSpecialPointer(wpage);

	/*
	 * if there aren't any overflow pages, there's nothing to squeeze. caller
	 * is responsible for releasing the pin on primary bucket page.
	 */
	if (!BlockNumberIsValid(wopaque->hasho_nextblkno))
	{
		LockBuffer(wbuf, BUFFER_LOCK_UNLOCK);
		return;
	}

	/*
	 * Find the last page in the bucket chain by starting at the base bucket
	 * page and working forward.  Note: we assume that a hash bucket chain is
	 * usually smaller than the buffer ring being used by VACUUM, else using
	 * the access strategy here would be counterproductive.
	 */
	rbuf = InvalidBuffer;
	ropaque = wopaque;
	do
	{
		rblkno = ropaque->hasho_nextblkno;
		if (rbuf != InvalidBuffer)
			_hash_relbuf(rel, rbuf);
		rbuf = _hash_getbuf_with_strategy(rel,
										  rblkno,
										  HASH_WRITE,
										  LH_OVERFLOW_PAGE,
										  bstrategy);
		rpage = BufferGetPage(rbuf);
		ropaque = (HashPageOpaque) PageGetSpecialPointer(rpage);
		Assert(ropaque->hasho_bucket == bucket);
	} while (BlockNumberIsValid(ropaque->hasho_nextblkno));

	/*
	 * squeeze the tuples.
	 */
	wbuf_dirty = false;
	for (;;)
	{
		OffsetNumber roffnum;
		OffsetNumber maxroffnum;
		OffsetNumber deletable[MaxOffsetNumber];
		int			ndeletable = 0;
		bool		retain_pin = false;

		/* Scan each tuple in "read" page */
		maxroffnum = PageGetMaxOffsetNumber(rpage);
		for (roffnum = FirstOffsetNumber;
			 roffnum <= maxroffnum;
			 roffnum = OffsetNumberNext(roffnum))
		{
			IndexTuple	itup;
			Size		itemsz;

			/* skip dead tuples */
			if (ItemIdIsDead(PageGetItemId(rpage, roffnum)))
				continue;

			itup = (IndexTuple) PageGetItem(rpage,
											PageGetItemId(rpage, roffnum));
			itemsz = IndexTupleDSize(*itup);
			itemsz = MAXALIGN(itemsz);

			/*
			 * Walk up the bucket chain, looking for a page big enough for
			 * this item.  Exit if we reach the read page.
			 */
			while (PageGetFreeSpace(wpage) < itemsz)
			{
				Buffer		next_wbuf = InvalidBuffer;

				Assert(!PageIsEmpty(wpage));

				if (wblkno == bucket_blkno)
					retain_pin = true;

				wblkno = wopaque->hasho_nextblkno;
				Assert(BlockNumberIsValid(wblkno));

				/* don't need to move to next page if we reached the read page */
				if (wblkno != rblkno)
					next_wbuf = _hash_getbuf_with_strategy(rel,
														   wblkno,
														   HASH_WRITE,
														   LH_OVERFLOW_PAGE,
														   bstrategy);

				/*
				 * release the lock on previous page after acquiring the lock
				 * on next page
				 */
				if (wbuf_dirty)
					MarkBufferDirty(wbuf);
				if (retain_pin)
					LockBuffer(wbuf, BUFFER_LOCK_UNLOCK);
				else
					_hash_relbuf(rel, wbuf);

				/* nothing more to do if we reached the read page */
				if (rblkno == wblkno)
				{
					if (ndeletable > 0)
					{
						/* Delete tuples we already moved off read page */
						PageIndexMultiDelete(rpage, deletable, ndeletable);
						MarkBufferDirty(rbuf);
					}
					_hash_relbuf(rel, rbuf);
					return;
				}

				wbuf = next_wbuf;
				wpage = BufferGetPage(wbuf);
				wopaque = (HashPageOpaque) PageGetSpecialPointer(wpage);
				Assert(wopaque->hasho_bucket == bucket);
				wbuf_dirty = false;
				retain_pin = false;
			}

			/*
			 * we have found room so insert on the "write" page, being careful
			 * to preserve hashkey ordering.  (If we insert many tuples into
			 * the same "write" page it would be worth qsort'ing instead of
			 * doing repeated _hash_pgaddtup.)
			 */
			(void) _hash_pgaddtup(rel, wbuf, itemsz, itup);
			wbuf_dirty = true;

			/* remember tuple for deletion from "read" page */
			deletable[ndeletable++] = roffnum;
		}

		/*
		 * If we reach here, there are no live tuples on the "read" page ---
		 * it was empty when we got to it, or we moved them all.  So we can
		 * just free the page without bothering with deleting tuples
		 * individually.  Then advance to the previous "read" page.
		 *
		 * Tricky point here: if our read and write pages are adjacent in the
		 * bucket chain, our write lock on wbuf will conflict with
		 * _hash_freeovflpage's attempt to update the sibling links of the
		 * removed page.  In that case, we don't need to lock it again.
		 */
		rblkno = ropaque->hasho_prevblkno;
		Assert(BlockNumberIsValid(rblkno));

		/* free this overflow page (releases rbuf) */
		_hash_freeovflpage(rel, rbuf, wbuf, bstrategy);

		if (wbuf_dirty)
			MarkBufferDirty(wbuf);

		/* are we freeing the page adjacent to wbuf? */
		if (rblkno == wblkno)
		{
			/* retain the pin on primary bucket page till end of bucket scan */
			if (wblkno == bucket_blkno)
				LockBuffer(wbuf, BUFFER_LOCK_UNLOCK);
			else
				_hash_relbuf(rel, wbuf);
			return;
		}

		rbuf = _hash_getbuf_with_strategy(rel,
										  rblkno,
										  HASH_WRITE,
										  LH_OVERFLOW_PAGE,
										  bstrategy);
		rpage = BufferGetPage(rbuf);
		ropaque = (HashPageOpaque) PageGetSpecialPointer(rpage);
		Assert(ropaque->hasho_bucket == bucket);
	}

	/* NOTREACHED */
}
