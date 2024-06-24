/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*-------------------------------------------------------------------------
 *
 * itemptr.c
 *	  POSTGRES disk item pointer code.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/storage/page/itemptr.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "storage/itemptr.h"


/*
 * ItemPointerEquals
 *	Returns true if both item pointers point to the same item,
 *	 otherwise returns false.
 *
 * Note:
 *	Asserts that the disk item pointers are both valid!
 */
bool
ItemPointerEquals(ItemPointer pointer1, ItemPointer pointer2)
{
	/*
	 * We really want ItemPointerData to be exactly 6 bytes.  This is rather a
	 * random place to check, but there is no better place.
	 */
	StaticAssertStmt(sizeof(ItemPointerData) == 3 * sizeof(uint16),
					 "ItemPointerData struct is improperly padded");

	if (ItemPointerGetBlockNumber(pointer1) ==
		ItemPointerGetBlockNumber(pointer2) &&
		ItemPointerGetOffsetNumber(pointer1) ==
		ItemPointerGetOffsetNumber(pointer2))
		return true;
	else
		return false;
}

/*
 * ItemPointerCompare
 *		Generic btree-style comparison for item pointers.
 */
int32
ItemPointerCompare(ItemPointer arg1, ItemPointer arg2)
{
	/*
	 * Use ItemPointerGet{Offset,Block}NumberNoCheck to avoid asserting
	 * ip_posid != 0, which may not be true for a user-supplied TID.
	 */
	BlockNumber b1 = ItemPointerGetBlockNumberNoCheck(arg1);
	BlockNumber b2 = ItemPointerGetBlockNumberNoCheck(arg2);

	if (b1 < b2)
		return -1;
	else if (b1 > b2)
		return 1;
	else if (ItemPointerGetOffsetNumberNoCheck(arg1) <
			 ItemPointerGetOffsetNumberNoCheck(arg2))
		return -1;
	else if (ItemPointerGetOffsetNumberNoCheck(arg1) >
			 ItemPointerGetOffsetNumberNoCheck(arg2))
		return 1;
	else
		return 0;
}

/*
 * ItemPointerInc
 *		Increment 'pointer' by 1 only paying attention to the ItemPointer's
 *		type's range limits and not MaxOffsetNumber and FirstOffsetNumber.
 *		This may result in 'pointer' becoming !OffsetNumberIsValid.
 *
 * If the pointer is already the maximum possible values permitted by the
 * range of the ItemPointer's types, then do nothing.
 */
void
ItemPointerInc(ItemPointer pointer)
{
	BlockNumber blk = ItemPointerGetBlockNumberNoCheck(pointer);
	OffsetNumber off = ItemPointerGetOffsetNumberNoCheck(pointer);

	if (off == PG_UINT16_MAX)
	{
		if (blk != InvalidBlockNumber)
		{
			off = 0;
			blk++;
		}
	}
	else
		off++;

	ItemPointerSet(pointer, blk, off);
}

/*
 * ItemPointerDec
 *		Decrement 'pointer' by 1 only paying attention to the ItemPointer's
 *		type's range limits and not MaxOffsetNumber and FirstOffsetNumber.
 *		This may result in 'pointer' becoming !OffsetNumberIsValid.
 *
 * If the pointer is already the minimum possible values permitted by the
 * range of the ItemPointer's types, then do nothing.  This does rely on
 * FirstOffsetNumber being 1 rather than 0.
 */
void
ItemPointerDec(ItemPointer pointer)
{
	BlockNumber blk = ItemPointerGetBlockNumberNoCheck(pointer);
	OffsetNumber off = ItemPointerGetOffsetNumberNoCheck(pointer);

	if (off == 0)
	{
		if (blk != 0)
		{
			off = PG_UINT16_MAX;
			blk--;
		}
	}
	else
		off--;

	ItemPointerSet(pointer, blk, off);
}
