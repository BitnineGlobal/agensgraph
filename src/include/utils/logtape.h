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
 * logtape.h
 *	  Management of "logical tapes" within temporary files.
 *
 * See logtape.c for explanations.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/logtape.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef LOGTAPE_H
#define LOGTAPE_H

#include "storage/sharedfileset.h"

/* LogicalTapeSet is an opaque type whose details are not known outside logtape.c. */

typedef struct LogicalTapeSet LogicalTapeSet;

/*
 * The approach tuplesort.c takes to parallel external sorts is that workers,
 * whose state is almost the same as independent serial sorts, are made to
 * produce a final materialized tape of sorted output in all cases.  This is
 * frozen, just like any case requiring a final materialized tape.  However,
 * there is one difference, which is that freezing will also export an
 * underlying shared fileset BufFile for sharing.  Freezing produces TapeShare
 * metadata for the worker when this happens, which is passed along through
 * shared memory to leader.
 *
 * The leader process can then pass an array of TapeShare metadata (one per
 * worker participant) to LogicalTapeSetCreate(), alongside a handle to a
 * shared fileset, which is sufficient to construct a new logical tapeset that
 * consists of each of the tapes materialized by workers.
 *
 * Note that while logtape.c does create an empty leader tape at the end of the
 * tapeset in the leader case, it can never be written to due to a restriction
 * in the shared buffile infrastructure.
 */
typedef struct TapeShare
{
	/*
	 * Currently, all the leader process needs is the location of the
	 * materialized tape's first block.
	 */
	long		firstblocknumber;
} TapeShare;

/*
 * prototypes for functions in logtape.c
 */

extern LogicalTapeSet *LogicalTapeSetCreate(int ntapes, bool preallocate,
											TapeShare *shared,
											SharedFileSet *fileset, int worker);
extern void LogicalTapeSetClose(LogicalTapeSet *lts);
extern void LogicalTapeSetForgetFreeSpace(LogicalTapeSet *lts);
extern size_t LogicalTapeRead(LogicalTapeSet *lts, int tapenum,
							  void *ptr, size_t size);
extern void LogicalTapeWrite(LogicalTapeSet *lts, int tapenum,
							 void *ptr, size_t size);
extern void LogicalTapeRewindForRead(LogicalTapeSet *lts, int tapenum,
									 size_t buffer_size);
extern void LogicalTapeRewindForWrite(LogicalTapeSet *lts, int tapenum);
extern void LogicalTapeFreeze(LogicalTapeSet *lts, int tapenum,
							  TapeShare *share);
extern void LogicalTapeSetExtend(LogicalTapeSet *lts, int nAdditional);
extern size_t LogicalTapeBackspace(LogicalTapeSet *lts, int tapenum,
								   size_t size);
extern void LogicalTapeSeek(LogicalTapeSet *lts, int tapenum,
							long blocknum, int offset);
extern void LogicalTapeTell(LogicalTapeSet *lts, int tapenum,
							long *blocknum, int *offset);
extern long LogicalTapeSetBlocks(LogicalTapeSet *lts);

#endif							/* LOGTAPE_H */
