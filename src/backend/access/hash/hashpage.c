/*-------------------------------------------------------------------------
 *
 * hashpage.c
 *	  Hash table page management code for the Postgres hash access method
 *
 * Portions Copyright (c) 1996-2017, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/access/hash/hashpage.c
 *
 * NOTES
 *	  Postgres hash pages look like ordinary relation pages.  The opaque
 *	  data at high addresses includes information about the page including
 *	  whether a page is an overflow page or a true bucket, the bucket
 *	  number, and the block numbers of the preceding and following pages
 *	  in the same bucket.
 *
 *	  The first page in a hash relation, page zero, is special -- it stores
 *	  information describing the hash table; it is referred to as the
 *	  "meta page." Pages one and higher store the actual data.
 *
 *	  There are also bitmap pages, which are not manipulated here;
 *	  see hashovfl.c.
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/hash.h"
#include "miscadmin.h"
#include "storage/lmgr.h"
#include "storage/smgr.h"


static bool _hash_alloc_buckets(Relation rel, BlockNumber firstblock,
					uint32 nblocks);
static void _hash_splitbucket(Relation rel, Buffer metabuf,
				  Bucket obucket, Bucket nbucket,
				  Buffer obuf,
				  Buffer nbuf,
				  uint32 maxbucket,
				  uint32 highmask, uint32 lowmask);
static void _hash_splitbucket_guts(Relation rel, Buffer metabuf,
					   Bucket obucket, Bucket nbucket, Buffer obuf,
					   Buffer nbuf, HTAB *htab, uint32 maxbucket,
					   uint32 highmask, uint32 lowmask);


/*
 * We use high-concurrency locking on hash indexes (see README for an overview
 * of the locking rules).  However, we can skip taking lmgr locks when the
 * index is local to the current backend (ie, either temp or new in the
 * current transaction).  No one else can see it, so there's no reason to
 * take locks.  We still take buffer-level locks, but not lmgr locks.
 */
#define USELOCKING(rel)		(!RELATION_IS_LOCAL(rel))


/*
 *	_hash_getbuf() -- Get a buffer by block number for read or write.
 *
 *		'access' must be HASH_READ, HASH_WRITE, or HASH_NOLOCK.
 *		'flags' is a bitwise OR of the allowed page types.
 *
 *		This must be used only to fetch pages that are expected to be valid
 *		already.  _hash_checkpage() is applied using the given flags.
 *
 *		When this routine returns, the appropriate lock is set on the
 *		requested buffer and its reference count has been incremented
 *		(ie, the buffer is "locked and pinned").
 *
 *		P_NEW is disallowed because this routine can only be used
 *		to access pages that are known to be before the filesystem EOF.
 *		Extending the index should be done with _hash_getnewbuf.
 */
Buffer
_hash_getbuf(Relation rel, BlockNumber blkno, int access, int flags)
{
	Buffer		buf;

	if (blkno == P_NEW)
		elog(ERROR, "hash AM does not use P_NEW");

	buf = ReadBuffer(rel, blkno);

	if (access != HASH_NOLOCK)
		LockBuffer(buf, access);

	/* ref count and lock type are correct */

	_hash_checkpage(rel, buf, flags);

	return buf;
}

/*
 * _hash_getbuf_with_condlock_cleanup() -- Try to get a buffer for cleanup.
 *
 *		We read the page and try to acquire a cleanup lock.  If we get it,
 *		we return the buffer; otherwise, we return InvalidBuffer.
 */
Buffer
_hash_getbuf_with_condlock_cleanup(Relation rel, BlockNumber blkno, int flags)
{
	Buffer		buf;

	if (blkno == P_NEW)
		elog(ERROR, "hash AM does not use P_NEW");

	buf = ReadBuffer(rel, blkno);

	if (!ConditionalLockBufferForCleanup(buf))
	{
		ReleaseBuffer(buf);
		return InvalidBuffer;
	}

	/* ref count and lock type are correct */

	_hash_checkpage(rel, buf, flags);

	return buf;
}

/*
 *	_hash_getinitbuf() -- Get and initialize a buffer by block number.
 *
 *		This must be used only to fetch pages that are known to be before
 *		the index's filesystem EOF, but are to be filled from scratch.
 *		_hash_pageinit() is applied automatically.  Otherwise it has
 *		effects similar to _hash_getbuf() with access = HASH_WRITE.
 *
 *		When this routine returns, a write lock is set on the
 *		requested buffer and its reference count has been incremented
 *		(ie, the buffer is "locked and pinned").
 *
 *		P_NEW is disallowed because this routine can only be used
 *		to access pages that are known to be before the filesystem EOF.
 *		Extending the index should be done with _hash_getnewbuf.
 */
Buffer
_hash_getinitbuf(Relation rel, BlockNumber blkno)
{
	Buffer		buf;

	if (blkno == P_NEW)
		elog(ERROR, "hash AM does not use P_NEW");

	buf = ReadBufferExtended(rel, MAIN_FORKNUM, blkno, RBM_ZERO_AND_LOCK,
							 NULL);

	/* ref count and lock type are correct */

	/* initialize the page */
	_hash_pageinit(BufferGetPage(buf), BufferGetPageSize(buf));

	return buf;
}

/*
 *	_hash_getnewbuf() -- Get a new page at the end of the index.
 *
 *		This has the same API as _hash_getinitbuf, except that we are adding
 *		a page to the index, and hence expect the page to be past the
 *		logical EOF.  (However, we have to support the case where it isn't,
 *		since a prior try might have crashed after extending the filesystem
 *		EOF but before updating the metapage to reflect the added page.)
 *
 *		It is caller's responsibility to ensure that only one process can
 *		extend the index at a time.  In practice, this function is called
 *		only while holding write lock on the metapage, because adding a page
 *		is always associated with an update of metapage data.
 */
Buffer
_hash_getnewbuf(Relation rel, BlockNumber blkno, ForkNumber forkNum)
{
	BlockNumber nblocks = RelationGetNumberOfBlocksInFork(rel, forkNum);
	Buffer		buf;

	if (blkno == P_NEW)
		elog(ERROR, "hash AM does not use P_NEW");
	if (blkno > nblocks)
		elog(ERROR, "access to noncontiguous page in hash index \"%s\"",
			 RelationGetRelationName(rel));

	/* smgr insists we use P_NEW to extend the relation */
	if (blkno == nblocks)
	{
		buf = ReadBufferExtended(rel, forkNum, P_NEW, RBM_NORMAL, NULL);
		if (BufferGetBlockNumber(buf) != blkno)
			elog(ERROR, "unexpected hash relation size: %u, should be %u",
				 BufferGetBlockNumber(buf), blkno);
		LockBuffer(buf, HASH_WRITE);
	}
	else
	{
		buf = ReadBufferExtended(rel, forkNum, blkno, RBM_ZERO_AND_LOCK,
								 NULL);
	}

	/* ref count and lock type are correct */

	/* initialize the page */
	_hash_pageinit(BufferGetPage(buf), BufferGetPageSize(buf));

	return buf;
}

/*
 *	_hash_getbuf_with_strategy() -- Get a buffer with nondefault strategy.
 *
 *		This is identical to _hash_getbuf() but also allows a buffer access
 *		strategy to be specified.  We use this for VACUUM operations.
 */
Buffer
_hash_getbuf_with_strategy(Relation rel, BlockNumber blkno,
						   int access, int flags,
						   BufferAccessStrategy bstrategy)
{
	Buffer		buf;

	if (blkno == P_NEW)
		elog(ERROR, "hash AM does not use P_NEW");

	buf = ReadBufferExtended(rel, MAIN_FORKNUM, blkno, RBM_NORMAL, bstrategy);

	if (access != HASH_NOLOCK)
		LockBuffer(buf, access);

	/* ref count and lock type are correct */

	_hash_checkpage(rel, buf, flags);

	return buf;
}

/*
 *	_hash_relbuf() -- release a locked buffer.
 *
 * Lock and pin (refcount) are both dropped.
 */
void
_hash_relbuf(Relation rel, Buffer buf)
{
	UnlockReleaseBuffer(buf);
}

/*
 *	_hash_dropbuf() -- release an unlocked buffer.
 *
 * This is used to unpin a buffer on which we hold no lock.
 */
void
_hash_dropbuf(Relation rel, Buffer buf)
{
	ReleaseBuffer(buf);
}

/*
 *	_hash_dropscanbuf() -- release buffers used in scan.
 *
 * This routine unpins the buffers used during scan on which we
 * hold no lock.
 */
void
_hash_dropscanbuf(Relation rel, HashScanOpaque so)
{
	/* release pin we hold on primary bucket page */
	if (BufferIsValid(so->hashso_bucket_buf) &&
		so->hashso_bucket_buf != so->hashso_curbuf)
		_hash_dropbuf(rel, so->hashso_bucket_buf);
	so->hashso_bucket_buf = InvalidBuffer;

	/* release pin we hold on primary bucket page  of bucket being split */
	if (BufferIsValid(so->hashso_split_bucket_buf) &&
		so->hashso_split_bucket_buf != so->hashso_curbuf)
		_hash_dropbuf(rel, so->hashso_split_bucket_buf);
	so->hashso_split_bucket_buf = InvalidBuffer;

	/* release any pin we still hold */
	if (BufferIsValid(so->hashso_curbuf))
		_hash_dropbuf(rel, so->hashso_curbuf);
	so->hashso_curbuf = InvalidBuffer;

	/* reset split scan */
	so->hashso_buc_populated = false;
	so->hashso_buc_split = false;
}


/*
 *	_hash_metapinit() -- Initialize the metadata page of a hash index,
 *				the initial buckets, and the initial bitmap page.
 *
 * The initial number of buckets is dependent on num_tuples, an estimate
 * of the number of tuples to be loaded into the index initially.  The
 * chosen number of buckets is returned.
 *
 * We are fairly cavalier about locking here, since we know that no one else
 * could be accessing this index.  In particular the rule about not holding
 * multiple buffer locks is ignored.
 */
uint32
_hash_metapinit(Relation rel, double num_tuples, ForkNumber forkNum)
{
	HashMetaPage metap;
	HashPageOpaque pageopaque;
	Buffer		metabuf;
	Buffer		buf;
	Page		pg;
	int32		data_width;
	int32		item_width;
	int32		ffactor;
	double		dnumbuckets;
	uint32		num_buckets;
	uint32		log2_num_buckets;
	uint32		i;

	/* safety check */
	if (RelationGetNumberOfBlocksInFork(rel, forkNum) != 0)
		elog(ERROR, "cannot initialize non-empty hash index \"%s\"",
			 RelationGetRelationName(rel));

	/*
	 * Determine the target fill factor (in tuples per bucket) for this index.
	 * The idea is to make the fill factor correspond to pages about as full
	 * as the user-settable fillfactor parameter says.  We can compute it
	 * exactly since the index datatype (i.e. uint32 hash key) is fixed-width.
	 */
	data_width = sizeof(uint32);
	item_width = MAXALIGN(sizeof(IndexTupleData)) + MAXALIGN(data_width) +
		sizeof(ItemIdData);		/* include the line pointer */
	ffactor = RelationGetTargetPageUsage(rel, HASH_DEFAULT_FILLFACTOR) / item_width;
	/* keep to a sane range */
	if (ffactor < 10)
		ffactor = 10;

	/*
	 * Choose the number of initial bucket pages to match the fill factor
	 * given the estimated number of tuples.  We round up the result to the
	 * next power of 2, however, and always force at least 2 bucket pages. The
	 * upper limit is determined by considerations explained in
	 * _hash_expandtable().
	 */
	dnumbuckets = num_tuples / ffactor;
	if (dnumbuckets <= 2.0)
		num_buckets = 2;
	else if (dnumbuckets >= (double) 0x40000000)
		num_buckets = 0x40000000;
	else
		num_buckets = ((uint32) 1) << _hash_log2((uint32) dnumbuckets);

	log2_num_buckets = _hash_log2(num_buckets);
	Assert(num_buckets == (((uint32) 1) << log2_num_buckets));
	Assert(log2_num_buckets < HASH_MAX_SPLITPOINTS);

	/*
	 * We initialize the metapage, the first N bucket pages, and the first
	 * bitmap page in sequence, using _hash_getnewbuf to cause smgrextend()
	 * calls to occur.  This ensures that the smgr level has the right idea of
	 * the physical index length.
	 */
	metabuf = _hash_getnewbuf(rel, HASH_METAPAGE, forkNum);
	pg = BufferGetPage(metabuf);

	pageopaque = (HashPageOpaque) PageGetSpecialPointer(pg);
	pageopaque->hasho_prevblkno = InvalidBlockNumber;
	pageopaque->hasho_nextblkno = InvalidBlockNumber;
	pageopaque->hasho_bucket = -1;
	pageopaque->hasho_flag = LH_META_PAGE;
	pageopaque->hasho_page_id = HASHO_PAGE_ID;

	metap = HashPageGetMeta(pg);

	metap->hashm_magic = HASH_MAGIC;
	metap->hashm_version = HASH_VERSION;
	metap->hashm_ntuples = 0;
	metap->hashm_nmaps = 0;
	metap->hashm_ffactor = ffactor;
	metap->hashm_bsize = HashGetMaxBitmapSize(pg);
	/* find largest bitmap array size that will fit in page size */
	for (i = _hash_log2(metap->hashm_bsize); i > 0; --i)
	{
		if ((1 << i) <= metap->hashm_bsize)
			break;
	}
	Assert(i > 0);
	metap->hashm_bmsize = 1 << i;
	metap->hashm_bmshift = i + BYTE_TO_BIT;
	Assert((1 << BMPG_SHIFT(metap)) == (BMPG_MASK(metap) + 1));

	/*
	 * Label the index with its primary hash support function's OID.  This is
	 * pretty useless for normal operation (in fact, hashm_procid is not used
	 * anywhere), but it might be handy for forensic purposes so we keep it.
	 */
	metap->hashm_procid = index_getprocid(rel, 1, HASHPROC);

	/*
	 * We initialize the index with N buckets, 0 .. N-1, occupying physical
	 * blocks 1 to N.  The first freespace bitmap page is in block N+1. Since
	 * N is a power of 2, we can set the masks this way:
	 */
	metap->hashm_maxbucket = metap->hashm_lowmask = num_buckets - 1;
	metap->hashm_highmask = (num_buckets << 1) - 1;

	MemSet(metap->hashm_spares, 0, sizeof(metap->hashm_spares));
	MemSet(metap->hashm_mapp, 0, sizeof(metap->hashm_mapp));

	/* Set up mapping for one spare page after the initial splitpoints */
	metap->hashm_spares[log2_num_buckets] = 1;
	metap->hashm_ovflpoint = log2_num_buckets;
	metap->hashm_firstfree = 0;

	/*
	 * Release buffer lock on the metapage while we initialize buckets.
	 * Otherwise, we'll be in interrupt holdoff and the CHECK_FOR_INTERRUPTS
	 * won't accomplish anything.  It's a bad idea to hold buffer locks for
	 * long intervals in any case, since that can block the bgwriter.
	 */
	MarkBufferDirty(metabuf);
	LockBuffer(metabuf, BUFFER_LOCK_UNLOCK);

	/*
	 * Initialize the first N buckets
	 */
	for (i = 0; i < num_buckets; i++)
	{
		/* Allow interrupts, in case N is huge */
		CHECK_FOR_INTERRUPTS();

		buf = _hash_getnewbuf(rel, BUCKET_TO_BLKNO(metap, i), forkNum);
		pg = BufferGetPage(buf);
		pageopaque = (HashPageOpaque) PageGetSpecialPointer(pg);
		pageopaque->hasho_prevblkno = InvalidBlockNumber;
		pageopaque->hasho_nextblkno = InvalidBlockNumber;
		pageopaque->hasho_bucket = i;
		pageopaque->hasho_flag = LH_BUCKET_PAGE;
		pageopaque->hasho_page_id = HASHO_PAGE_ID;
		MarkBufferDirty(buf);
		_hash_relbuf(rel, buf);
	}

	/* Now reacquire buffer lock on metapage */
	LockBuffer(metabuf, BUFFER_LOCK_EXCLUSIVE);

	/*
	 * Initialize first bitmap page
	 */
	_hash_initbitmap(rel, metap, num_buckets + 1, forkNum);

	/* all done */
	MarkBufferDirty(metabuf);
	_hash_relbuf(rel, metabuf);

	return num_buckets;
}

/*
 *	_hash_pageinit() -- Initialize a new hash index page.
 */
void
_hash_pageinit(Page page, Size size)
{
	Assert(PageIsNew(page));
	PageInit(page, size, sizeof(HashPageOpaqueData));
}

/*
 * Attempt to expand the hash table by creating one new bucket.
 *
 * This will silently do nothing if we don't get cleanup lock on old or
 * new bucket.
 *
 * Complete the pending splits and remove the tuples from old bucket,
 * if there are any left over from the previous split.
 *
 * The caller must hold a pin, but no lock, on the metapage buffer.
 * The buffer is returned in the same state.
 */
void
_hash_expandtable(Relation rel, Buffer metabuf)
{
	HashMetaPage metap;
	Bucket		old_bucket;
	Bucket		new_bucket;
	uint32		spare_ndx;
	BlockNumber start_oblkno;
	BlockNumber start_nblkno;
	Buffer		buf_nblkno;
	Buffer		buf_oblkno;
	Page		opage;
	HashPageOpaque oopaque;
	uint32		maxbucket;
	uint32		highmask;
	uint32		lowmask;

restart_expand:

	/*
	 * Write-lock the meta page.  It used to be necessary to acquire a
	 * heavyweight lock to begin a split, but that is no longer required.
	 */
	LockBuffer(metabuf, BUFFER_LOCK_EXCLUSIVE);

	_hash_checkpage(rel, metabuf, LH_META_PAGE);
	metap = HashPageGetMeta(BufferGetPage(metabuf));

	/*
	 * Check to see if split is still needed; someone else might have already
	 * done one while we waited for the lock.
	 *
	 * Make sure this stays in sync with _hash_doinsert()
	 */
	if (metap->hashm_ntuples <=
		(double) metap->hashm_ffactor * (metap->hashm_maxbucket + 1))
		goto fail;

	/*
	 * Can't split anymore if maxbucket has reached its maximum possible
	 * value.
	 *
	 * Ideally we'd allow bucket numbers up to UINT_MAX-1 (no higher because
	 * the calculation maxbucket+1 mustn't overflow).  Currently we restrict
	 * to half that because of overflow looping in _hash_log2() and
	 * insufficient space in hashm_spares[].  It's moot anyway because an
	 * index with 2^32 buckets would certainly overflow BlockNumber and hence
	 * _hash_alloc_buckets() would fail, but if we supported buckets smaller
	 * than a disk block then this would be an independent constraint.
	 *
	 * If you change this, see also the maximum initial number of buckets in
	 * _hash_metapinit().
	 */
	if (metap->hashm_maxbucket >= (uint32) 0x7FFFFFFE)
		goto fail;

	/*
	 * Determine which bucket is to be split, and attempt to take cleanup lock
	 * on the old bucket.  If we can't get the lock, give up.
	 *
	 * The cleanup lock protects us not only against other backends, but
	 * against our own backend as well.
	 *
	 * The cleanup lock is mainly to protect the split from concurrent
	 * inserts. See src/backend/access/hash/README, Lock Definitions for
	 * further details.  Due to this locking restriction, if there is any
	 * pending scan, the split will give up which is not good, but harmless.
	 */
	new_bucket = metap->hashm_maxbucket + 1;

	old_bucket = (new_bucket & metap->hashm_lowmask);

	start_oblkno = BUCKET_TO_BLKNO(metap, old_bucket);

	buf_oblkno = _hash_getbuf_with_condlock_cleanup(rel, start_oblkno, LH_BUCKET_PAGE);
	if (!buf_oblkno)
		goto fail;

	opage = BufferGetPage(buf_oblkno);
	oopaque = (HashPageOpaque) PageGetSpecialPointer(opage);

	/*
	 * We want to finish the split from a bucket as there is no apparent
	 * benefit by not doing so and it will make the code complicated to finish
	 * the split that involves multiple buckets considering the case where new
	 * split also fails.  We don't need to consider the new bucket for
	 * completing the split here as it is not possible that a re-split of new
	 * bucket starts when there is still a pending split from old bucket.
	 */
	if (H_BUCKET_BEING_SPLIT(oopaque))
	{
		/*
		 * Copy bucket mapping info now; refer the comment in code below where
		 * we copy this information before calling _hash_splitbucket to see
		 * why this is okay.
		 */
		maxbucket = metap->hashm_maxbucket;
		highmask = metap->hashm_highmask;
		lowmask = metap->hashm_lowmask;

		/*
		 * Release the lock on metapage and old_bucket, before completing the
		 * split.
		 */
		LockBuffer(metabuf, BUFFER_LOCK_UNLOCK);
		LockBuffer(buf_oblkno, BUFFER_LOCK_UNLOCK);

		_hash_finish_split(rel, metabuf, buf_oblkno, old_bucket, maxbucket,
						   highmask, lowmask);

		/* release the pin on old buffer and retry for expand. */
		_hash_dropbuf(rel, buf_oblkno);

		goto restart_expand;
	}

	/*
	 * Clean the tuples remained from the previous split.  This operation
	 * requires cleanup lock and we already have one on the old bucket, so
	 * let's do it. We also don't want to allow further splits from the bucket
	 * till the garbage of previous split is cleaned.  This has two
	 * advantages; first, it helps in avoiding the bloat due to garbage and
	 * second is, during cleanup of bucket, we are always sure that the
	 * garbage tuples belong to most recently split bucket.  On the contrary,
	 * if we allow cleanup of bucket after meta page is updated to indicate
	 * the new split and before the actual split, the cleanup operation won't
	 * be able to decide whether the tuple has been moved to the newly created
	 * bucket and ended up deleting such tuples.
	 */
	if (H_NEEDS_SPLIT_CLEANUP(oopaque))
	{
		/*
		 * Copy bucket mapping info now; refer to the comment in code below
		 * where we copy this information before calling _hash_splitbucket
		 * to see why this is okay.
		 */
		maxbucket = metap->hashm_maxbucket;
		highmask = metap->hashm_highmask;
		lowmask = metap->hashm_lowmask;

		/* Release the metapage lock. */
		LockBuffer(metabuf, BUFFER_LOCK_UNLOCK);

		hashbucketcleanup(rel, old_bucket, buf_oblkno, start_oblkno, NULL,
						  maxbucket, highmask, lowmask, NULL, NULL, true,
						  NULL, NULL);

		_hash_dropbuf(rel, buf_oblkno);

		goto restart_expand;
	}

	/*
	 * There shouldn't be any active scan on new bucket.
	 *
	 * Note: it is safe to compute the new bucket's blkno here, even though we
	 * may still need to update the BUCKET_TO_BLKNO mapping.  This is because
	 * the current value of hashm_spares[hashm_ovflpoint] correctly shows
	 * where we are going to put a new splitpoint's worth of buckets.
	 */
	start_nblkno = BUCKET_TO_BLKNO(metap, new_bucket);

	/*
	 * If the split point is increasing (hashm_maxbucket's log base 2
	 * increases), we need to allocate a new batch of bucket pages.
	 */
	spare_ndx = _hash_log2(new_bucket + 1);
	if (spare_ndx > metap->hashm_ovflpoint)
	{
		Assert(spare_ndx == metap->hashm_ovflpoint + 1);

		/*
		 * The number of buckets in the new splitpoint is equal to the total
		 * number already in existence, i.e. new_bucket.  Currently this maps
		 * one-to-one to blocks required, but someday we may need a more
		 * complicated calculation here.
		 */
		if (!_hash_alloc_buckets(rel, start_nblkno, new_bucket))
		{
			/* can't split due to BlockNumber overflow */
			_hash_relbuf(rel, buf_oblkno);
			goto fail;
		}
	}

	/*
	 * Physically allocate the new bucket's primary page.  We want to do this
	 * before changing the metapage's mapping info, in case we can't get the
	 * disk space.  Ideally, we don't need to check for cleanup lock on new
	 * bucket as no other backend could find this bucket unless meta page is
	 * updated.  However, it is good to be consistent with old bucket locking.
	 */
	buf_nblkno = _hash_getnewbuf(rel, start_nblkno, MAIN_FORKNUM);
	if (!IsBufferCleanupOK(buf_nblkno))
	{
		_hash_relbuf(rel, buf_oblkno);
		_hash_relbuf(rel, buf_nblkno);
		goto fail;
	}


	/*
	 * Okay to proceed with split.  Update the metapage bucket mapping info.
	 *
	 * Since we are scribbling on the metapage data right in the shared
	 * buffer, any failure in this next little bit leaves us with a big
	 * problem: the metapage is effectively corrupt but could get written back
	 * to disk.  We don't really expect any failure, but just to be sure,
	 * establish a critical section.
	 */
	START_CRIT_SECTION();

	metap->hashm_maxbucket = new_bucket;

	if (new_bucket > metap->hashm_highmask)
	{
		/* Starting a new doubling */
		metap->hashm_lowmask = metap->hashm_highmask;
		metap->hashm_highmask = new_bucket | metap->hashm_lowmask;
	}

	/*
	 * If the split point is increasing (hashm_maxbucket's log base 2
	 * increases), we need to adjust the hashm_spares[] array and
	 * hashm_ovflpoint so that future overflow pages will be created beyond
	 * this new batch of bucket pages.
	 */
	if (spare_ndx > metap->hashm_ovflpoint)
	{
		metap->hashm_spares[spare_ndx] = metap->hashm_spares[metap->hashm_ovflpoint];
		metap->hashm_ovflpoint = spare_ndx;
	}

	/* Done mucking with metapage */
	END_CRIT_SECTION();

	/*
	 * Copy bucket mapping info now; this saves re-accessing the meta page
	 * inside _hash_splitbucket's inner loop.  Note that once we drop the
	 * split lock, other splits could begin, so these values might be out of
	 * date before _hash_splitbucket finishes.  That's okay, since all it
	 * needs is to tell which of these two buckets to map hashkeys into.
	 */
	maxbucket = metap->hashm_maxbucket;
	highmask = metap->hashm_highmask;
	lowmask = metap->hashm_lowmask;

	/* Write out the metapage and drop lock, but keep pin */
	MarkBufferDirty(metabuf);
	LockBuffer(metabuf, BUFFER_LOCK_UNLOCK);

	/* Relocate records to the new bucket */
	_hash_splitbucket(rel, metabuf,
					  old_bucket, new_bucket,
					  buf_oblkno, buf_nblkno,
					  maxbucket, highmask, lowmask);

	return;

	/* Here if decide not to split or fail to acquire old bucket lock */
fail:

	/* We didn't write the metapage, so just drop lock */
	LockBuffer(metabuf, BUFFER_LOCK_UNLOCK);
}


/*
 * _hash_alloc_buckets -- allocate a new splitpoint's worth of bucket pages
 *
 * This does not need to initialize the new bucket pages; we'll do that as
 * each one is used by _hash_expandtable().  But we have to extend the logical
 * EOF to the end of the splitpoint; this keeps smgr's idea of the EOF in
 * sync with ours, so that we don't get complaints from smgr.
 *
 * We do this by writing a page of zeroes at the end of the splitpoint range.
 * We expect that the filesystem will ensure that the intervening pages read
 * as zeroes too.  On many filesystems this "hole" will not be allocated
 * immediately, which means that the index file may end up more fragmented
 * than if we forced it all to be allocated now; but since we don't scan
 * hash indexes sequentially anyway, that probably doesn't matter.
 *
 * XXX It's annoying that this code is executed with the metapage lock held.
 * We need to interlock against _hash_getovflpage() adding a new overflow page
 * concurrently, but it'd likely be better to use LockRelationForExtension
 * for the purpose.  OTOH, adding a splitpoint is a very infrequent operation,
 * so it may not be worth worrying about.
 *
 * Returns TRUE if successful, or FALSE if allocation failed due to
 * BlockNumber overflow.
 */
static bool
_hash_alloc_buckets(Relation rel, BlockNumber firstblock, uint32 nblocks)
{
	BlockNumber lastblock;
	char		zerobuf[BLCKSZ];

	lastblock = firstblock + nblocks - 1;

	/*
	 * Check for overflow in block number calculation; if so, we cannot extend
	 * the index anymore.
	 */
	if (lastblock < firstblock || lastblock == InvalidBlockNumber)
		return false;

	MemSet(zerobuf, 0, sizeof(zerobuf));

	RelationOpenSmgr(rel);
	smgrextend(rel->rd_smgr, MAIN_FORKNUM, lastblock, zerobuf, false);

	return true;
}


/*
 * _hash_splitbucket -- split 'obucket' into 'obucket' and 'nbucket'
 *
 * We are splitting a bucket that consists of a base bucket page and zero
 * or more overflow (bucket chain) pages.  We must relocate tuples that
 * belong in the new bucket, and compress out any free space in the old
 * bucket.
 *
 * The caller must hold cleanup locks on both buckets to ensure that
 * no one else is trying to access them (see README).
 *
 * The caller must hold a pin, but no lock, on the metapage buffer.
 * The buffer is returned in the same state.  (The metapage is only
 * touched if it becomes necessary to add or remove overflow pages.)
 *
 * Split needs to retain pin on primary bucket pages of both old and new
 * buckets till end of operation.  This is to prevent vacuum from starting
 * while a split is in progress.
 *
 * In addition, the caller must have created the new bucket's base page,
 * which is passed in buffer nbuf, pinned and write-locked.  That lock and
 * pin are released here.  (The API is set up this way because we must do
 * _hash_getnewbuf() before releasing the metapage write lock.  So instead of
 * passing the new bucket's start block number, we pass an actual buffer.)
 */
static void
_hash_splitbucket(Relation rel,
				  Buffer metabuf,
				  Bucket obucket,
				  Bucket nbucket,
				  Buffer obuf,
				  Buffer nbuf,
				  uint32 maxbucket,
				  uint32 highmask,
				  uint32 lowmask)
{
	Page		opage;
	Page		npage;
	HashPageOpaque oopaque;
	HashPageOpaque nopaque;

	opage = BufferGetPage(obuf);
	oopaque = (HashPageOpaque) PageGetSpecialPointer(opage);

	/*
	 * Mark the old bucket to indicate that split is in progress.  At
	 * operation end, we clear split-in-progress flag.
	 */
	oopaque->hasho_flag |= LH_BUCKET_BEING_SPLIT;

	npage = BufferGetPage(nbuf);

	/*
	 * initialize the new bucket's primary page and mark it to indicate that
	 * split is in progress.
	 */
	nopaque = (HashPageOpaque) PageGetSpecialPointer(npage);
	nopaque->hasho_prevblkno = InvalidBlockNumber;
	nopaque->hasho_nextblkno = InvalidBlockNumber;
	nopaque->hasho_bucket = nbucket;
	nopaque->hasho_flag = LH_BUCKET_PAGE | LH_BUCKET_BEING_POPULATED;
	nopaque->hasho_page_id = HASHO_PAGE_ID;

	_hash_splitbucket_guts(rel, metabuf, obucket,
						   nbucket, obuf, nbuf, NULL,
						   maxbucket, highmask, lowmask);

	/* all done, now release the locks and pins on primary buckets. */
	_hash_relbuf(rel, obuf);
	_hash_relbuf(rel, nbuf);
}

/*
 * _hash_splitbucket_guts -- Helper function to perform the split operation
 *
 * This routine is used to partition the tuples between old and new bucket and
 * to finish incomplete split operations.  To finish the previously
 * interrupted split operation, caller needs to fill htab.  If htab is set, then
 * we skip the movement of tuples that exists in htab, otherwise NULL value of
 * htab indicates movement of all the tuples that belong to new bucket.
 *
 * Caller needs to lock and unlock the old and new primary buckets.
 */
static void
_hash_splitbucket_guts(Relation rel,
					   Buffer metabuf,
					   Bucket obucket,
					   Bucket nbucket,
					   Buffer obuf,
					   Buffer nbuf,
					   HTAB *htab,
					   uint32 maxbucket,
					   uint32 highmask,
					   uint32 lowmask)
{
	Buffer		bucket_obuf;
	Buffer		bucket_nbuf;
	Page		opage;
	Page		npage;
	HashPageOpaque oopaque;
	HashPageOpaque nopaque;

	bucket_obuf = obuf;
	opage = BufferGetPage(obuf);
	oopaque = (HashPageOpaque) PageGetSpecialPointer(opage);

	bucket_nbuf = nbuf;
	npage = BufferGetPage(nbuf);
	nopaque = (HashPageOpaque) PageGetSpecialPointer(npage);

	/*
	 * Partition the tuples in the old bucket between the old bucket and the
	 * new bucket, advancing along the old bucket's overflow bucket chain and
	 * adding overflow pages to the new bucket as needed.  Outer loop iterates
	 * once per page in old bucket.
	 */
	for (;;)
	{
		BlockNumber oblkno;
		OffsetNumber ooffnum;
		OffsetNumber omaxoffnum;

		/* Scan each tuple in old page */
		omaxoffnum = PageGetMaxOffsetNumber(opage);
		for (ooffnum = FirstOffsetNumber;
			 ooffnum <= omaxoffnum;
			 ooffnum = OffsetNumberNext(ooffnum))
		{
			IndexTuple	itup;
			Size		itemsz;
			Bucket		bucket;
			bool		found = false;

			/* skip dead tuples */
			if (ItemIdIsDead(PageGetItemId(opage, ooffnum)))
				continue;

			/*
			 * Before inserting a tuple, probe the hash table containing TIDs
			 * of tuples belonging to new bucket, if we find a match, then
			 * skip that tuple, else fetch the item's hash key (conveniently
			 * stored in the item) and determine which bucket it now belongs
			 * in.
			 */
			itup = (IndexTuple) PageGetItem(opage,
											PageGetItemId(opage, ooffnum));

			if (htab)
				(void) hash_search(htab, &itup->t_tid, HASH_FIND, &found);

			if (found)
				continue;

			bucket = _hash_hashkey2bucket(_hash_get_indextuple_hashkey(itup),
										  maxbucket, highmask, lowmask);

			if (bucket == nbucket)
			{
				IndexTuple	new_itup;

				/*
				 * make a copy of index tuple as we have to scribble on it.
				 */
				new_itup = CopyIndexTuple(itup);

				/*
				 * mark the index tuple as moved by split, such tuples are
				 * skipped by scan if there is split in progress for a bucket.
				 */
				new_itup->t_info |= INDEX_MOVED_BY_SPLIT_MASK;

				/*
				 * insert the tuple into the new bucket.  if it doesn't fit on
				 * the current page in the new bucket, we must allocate a new
				 * overflow page and place the tuple on that page instead.
				 */
				itemsz = IndexTupleDSize(*new_itup);
				itemsz = MAXALIGN(itemsz);

				if (PageGetFreeSpace(npage) < itemsz)
				{
					/* write out nbuf and drop lock, but keep pin */
					MarkBufferDirty(nbuf);
					LockBuffer(nbuf, BUFFER_LOCK_UNLOCK);
					/* chain to a new overflow page */
					nbuf = _hash_addovflpage(rel, metabuf, nbuf, (nbuf == bucket_nbuf) ? true : false);
					npage = BufferGetPage(nbuf);
					nopaque = (HashPageOpaque) PageGetSpecialPointer(npage);
				}

				/*
				 * Insert tuple on new page, using _hash_pgaddtup to ensure
				 * correct ordering by hashkey.  This is a tad inefficient
				 * since we may have to shuffle itempointers repeatedly.
				 * Possible future improvement: accumulate all the items for
				 * the new page and qsort them before insertion.
				 */
				(void) _hash_pgaddtup(rel, nbuf, itemsz, new_itup);

				/* be tidy */
				pfree(new_itup);
			}
			else
			{
				/*
				 * the tuple stays on this page, so nothing to do.
				 */
				Assert(bucket == obucket);
			}
		}

		oblkno = oopaque->hasho_nextblkno;

		/* retain the pin on the old primary bucket */
		if (obuf == bucket_obuf)
			LockBuffer(obuf, BUFFER_LOCK_UNLOCK);
		else
			_hash_relbuf(rel, obuf);

		/* Exit loop if no more overflow pages in old bucket */
		if (!BlockNumberIsValid(oblkno))
			break;

		/* Else, advance to next old page */
		obuf = _hash_getbuf(rel, oblkno, HASH_READ, LH_OVERFLOW_PAGE);
		opage = BufferGetPage(obuf);
		oopaque = (HashPageOpaque) PageGetSpecialPointer(opage);
	}

	/*
	 * We're at the end of the old bucket chain, so we're done partitioning
	 * the tuples.  Mark the old and new buckets to indicate split is
	 * finished.
	 *
	 * To avoid deadlocks due to locking order of buckets, first lock the old
	 * bucket and then the new bucket.
	 */
	if (nbuf == bucket_nbuf)
	{
		MarkBufferDirty(bucket_nbuf);
		LockBuffer(bucket_nbuf, BUFFER_LOCK_UNLOCK);
	}
	else
	{
		MarkBufferDirty(nbuf);
		_hash_relbuf(rel, nbuf);
	}

	LockBuffer(bucket_obuf, BUFFER_LOCK_EXCLUSIVE);
	opage = BufferGetPage(bucket_obuf);
	oopaque = (HashPageOpaque) PageGetSpecialPointer(opage);

	LockBuffer(bucket_nbuf, BUFFER_LOCK_EXCLUSIVE);
	npage = BufferGetPage(bucket_nbuf);
	nopaque = (HashPageOpaque) PageGetSpecialPointer(npage);

	oopaque->hasho_flag &= ~LH_BUCKET_BEING_SPLIT;
	nopaque->hasho_flag &= ~LH_BUCKET_BEING_POPULATED;

	/*
	 * After the split is finished, mark the old bucket to indicate that it
	 * contains deletable tuples.  Vacuum will clear split-cleanup flag after
	 * deleting such tuples.
	 */
	oopaque->hasho_flag |= LH_BUCKET_NEEDS_SPLIT_CLEANUP;

	/*
	 * now write the buffers, here we don't release the locks as caller is
	 * responsible to release locks.
	 */
	MarkBufferDirty(bucket_obuf);
	MarkBufferDirty(bucket_nbuf);
}

/*
 *	_hash_finish_split() -- Finish the previously interrupted split operation
 *
 * To complete the split operation, we form the hash table of TIDs in new
 * bucket which is then used by split operation to skip tuples that are
 * already moved before the split operation was previously interrupted.
 *
 * The caller must hold a pin, but no lock, on the metapage and old bucket's
 * primary page buffer.  The buffers are returned in the same state.  (The
 * metapage is only touched if it becomes necessary to add or remove overflow
 * pages.)
 */
void
_hash_finish_split(Relation rel, Buffer metabuf, Buffer obuf, Bucket obucket,
				   uint32 maxbucket, uint32 highmask, uint32 lowmask)
{
	HASHCTL		hash_ctl;
	HTAB	   *tidhtab;
	Buffer		bucket_nbuf = InvalidBuffer;
	Buffer		nbuf;
	Page		npage;
	BlockNumber nblkno;
	BlockNumber bucket_nblkno;
	HashPageOpaque npageopaque;
	Bucket		nbucket;
	bool		found;

	/* Initialize hash tables used to track TIDs */
	memset(&hash_ctl, 0, sizeof(hash_ctl));
	hash_ctl.keysize = sizeof(ItemPointerData);
	hash_ctl.entrysize = sizeof(ItemPointerData);
	hash_ctl.hcxt = CurrentMemoryContext;

	tidhtab =
		hash_create("bucket ctids",
					256,		/* arbitrary initial size */
					&hash_ctl,
					HASH_ELEM | HASH_BLOBS | HASH_CONTEXT);

	bucket_nblkno = nblkno = _hash_get_newblock_from_oldbucket(rel, obucket);

	/*
	 * Scan the new bucket and build hash table of TIDs
	 */
	for (;;)
	{
		OffsetNumber noffnum;
		OffsetNumber nmaxoffnum;

		nbuf = _hash_getbuf(rel, nblkno, HASH_READ,
							LH_BUCKET_PAGE | LH_OVERFLOW_PAGE);

		/* remember the primary bucket buffer to acquire cleanup lock on it. */
		if (nblkno == bucket_nblkno)
			bucket_nbuf = nbuf;

		npage = BufferGetPage(nbuf);
		npageopaque = (HashPageOpaque) PageGetSpecialPointer(npage);

		/* Scan each tuple in new page */
		nmaxoffnum = PageGetMaxOffsetNumber(npage);
		for (noffnum = FirstOffsetNumber;
			 noffnum <= nmaxoffnum;
			 noffnum = OffsetNumberNext(noffnum))
		{
			IndexTuple	itup;

			/* Fetch the item's TID and insert it in hash table. */
			itup = (IndexTuple) PageGetItem(npage,
											PageGetItemId(npage, noffnum));

			(void) hash_search(tidhtab, &itup->t_tid, HASH_ENTER, &found);

			Assert(!found);
		}

		nblkno = npageopaque->hasho_nextblkno;

		/*
		 * release our write lock without modifying buffer and ensure to
		 * retain the pin on primary bucket.
		 */
		if (nbuf == bucket_nbuf)
			LockBuffer(nbuf, BUFFER_LOCK_UNLOCK);
		else
			_hash_relbuf(rel, nbuf);

		/* Exit loop if no more overflow pages in new bucket */
		if (!BlockNumberIsValid(nblkno))
			break;
	}

	/*
	 * Conditionally get the cleanup lock on old and new buckets to perform
	 * the split operation.  If we don't get the cleanup locks, silently give
	 * up and next insertion on old bucket will try again to complete the
	 * split.
	 */
	if (!ConditionalLockBufferForCleanup(obuf))
	{
		hash_destroy(tidhtab);
		return;
	}
	if (!ConditionalLockBufferForCleanup(bucket_nbuf))
	{
		LockBuffer(obuf, BUFFER_LOCK_UNLOCK);
		hash_destroy(tidhtab);
		return;
	}

	npage = BufferGetPage(bucket_nbuf);
	npageopaque = (HashPageOpaque) PageGetSpecialPointer(npage);
	nbucket = npageopaque->hasho_bucket;

	_hash_splitbucket_guts(rel, metabuf, obucket,
						   nbucket, obuf, bucket_nbuf, tidhtab,
						   maxbucket, highmask, lowmask);

	_hash_relbuf(rel, bucket_nbuf);
	LockBuffer(obuf, BUFFER_LOCK_UNLOCK);
	hash_destroy(tidhtab);
}
