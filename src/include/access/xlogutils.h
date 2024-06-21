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

/*
 * xlogutils.h
 *
 * Utilities for replaying WAL records.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/xlogutils.h
 */
#ifndef XLOG_UTILS_H
#define XLOG_UTILS_H

#include "access/xlogreader.h"
#include "storage/bufmgr.h"


extern bool XLogHaveInvalidPages(void);
extern void XLogCheckInvalidPages(void);

extern void XLogDropRelation(RelFileNode rnode, ForkNumber forknum);
extern void XLogDropDatabase(Oid dbid);
extern void XLogTruncateRelation(RelFileNode rnode, ForkNumber forkNum,
								 BlockNumber nblocks);

/* Result codes for XLogReadBufferForRedo[Extended] */
typedef enum
{
	BLK_NEEDS_REDO,				/* changes from WAL record need to be applied */
	BLK_DONE,					/* block is already up-to-date */
	BLK_RESTORED,				/* block was restored from a full-page image */
	BLK_NOTFOUND				/* block was not found (and hence does not
								 * need to be replayed) */
} XLogRedoAction;

extern XLogRedoAction XLogReadBufferForRedo(XLogReaderState *record,
											uint8 buffer_id, Buffer *buf);
extern Buffer XLogInitBufferForRedo(XLogReaderState *record, uint8 block_id);
extern XLogRedoAction XLogReadBufferForRedoExtended(XLogReaderState *record,
													uint8 buffer_id,
													ReadBufferMode mode, bool get_cleanup_lock,
													Buffer *buf);

extern Buffer XLogReadBufferExtended(RelFileNode rnode, ForkNumber forknum,
									 BlockNumber blkno, ReadBufferMode mode);

extern Relation CreateFakeRelcacheEntry(RelFileNode rnode);
extern void FreeFakeRelcacheEntry(Relation fakerel);

extern int	read_local_xlog_page(XLogReaderState *state,
								 XLogRecPtr targetPagePtr, int reqLen,
								 XLogRecPtr targetRecPtr, char *cur_page);
extern void wal_segment_open(XLogReaderState *state,
							 XLogSegNo nextSegNo,
							 TimeLineID *tli_p);
extern void wal_segment_close(XLogReaderState *state);

extern void XLogReadDetermineTimeline(XLogReaderState *state,
									  XLogRecPtr wantPage, uint32 wantLength);

extern void WALReadRaiseError(WALReadError *errinfo);

#endif
