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
 * freespace.h
 *	  POSTGRES free space map for quickly finding free space in relations
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/freespace.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef FREESPACE_H_
#define FREESPACE_H_

#include "storage/block.h"
#include "storage/relfilenode.h"
#include "utils/relcache.h"

/* prototypes for public functions in freespace.c */
extern Size GetRecordedFreeSpace(Relation rel, BlockNumber heapBlk);
extern BlockNumber GetPageWithFreeSpace(Relation rel, Size spaceNeeded);
extern BlockNumber RecordAndGetPageWithFreeSpace(Relation rel,
												 BlockNumber oldPage,
												 Size oldSpaceAvail,
												 Size spaceNeeded);
extern void RecordPageWithFreeSpace(Relation rel, BlockNumber heapBlk,
									Size spaceAvail);
extern void XLogRecordPageWithFreeSpace(RelFileNode rnode, BlockNumber heapBlk,
										Size spaceAvail);

extern BlockNumber FreeSpaceMapPrepareTruncateRel(Relation rel,
												  BlockNumber nblocks);
extern void FreeSpaceMapVacuum(Relation rel);
extern void FreeSpaceMapVacuumRange(Relation rel, BlockNumber start,
									BlockNumber end);

#endif							/* FREESPACE_H_ */
