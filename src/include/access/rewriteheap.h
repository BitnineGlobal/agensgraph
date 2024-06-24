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
 * rewriteheap.h
 *	  Declarations for heap rewrite support functions
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994-5, Regents of the University of California
 *
 * src/include/access/rewriteheap.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef REWRITE_HEAP_H
#define REWRITE_HEAP_H

#include "access/htup.h"
#include "storage/itemptr.h"
#include "storage/relfilenode.h"
#include "utils/relcache.h"

/* struct definition is private to rewriteheap.c */
typedef struct RewriteStateData *RewriteState;

extern RewriteState begin_heap_rewrite(Relation OldHeap, Relation NewHeap,
									   TransactionId OldestXmin, TransactionId FreezeXid,
									   MultiXactId MultiXactCutoff);
extern void end_heap_rewrite(RewriteState state);
extern void rewrite_heap_tuple(RewriteState state, HeapTuple oldTuple,
							   HeapTuple newTuple);
extern bool rewrite_heap_dead_tuple(RewriteState state, HeapTuple oldTuple);

/*
 * On-Disk data format for an individual logical rewrite mapping.
 */
typedef struct LogicalRewriteMappingData
{
	RelFileNode old_node;
	RelFileNode new_node;
	ItemPointerData old_tid;
	ItemPointerData new_tid;
} LogicalRewriteMappingData;

/* ---
 * The filename consists of the following, dash separated,
 * components:
 * 1) database oid or InvalidOid for shared relations
 * 2) the oid of the relation
 * 3) upper 32bit of the LSN at which a rewrite started
 * 4) lower 32bit of the LSN at which a rewrite started
 * 5) xid we are mapping for
 * 6) xid of the xact performing the mapping
 * ---
 */
#define LOGICAL_REWRITE_FORMAT "map-%x-%x-%X_%X-%x-%x"
void		CheckPointLogicalRewriteHeap(void);

#endif							/* REWRITE_HEAP_H */
