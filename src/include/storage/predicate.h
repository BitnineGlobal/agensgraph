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
 * predicate.h
 *	  POSTGRES public predicate locking definitions.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/predicate.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PREDICATE_H
#define PREDICATE_H

#include "storage/lock.h"
#include "utils/relcache.h"
#include "utils/snapshot.h"


/*
 * GUC variables
 */
extern int	max_predicate_locks_per_xact;
extern int	max_predicate_locks_per_relation;
extern int	max_predicate_locks_per_page;


/* Number of SLRU buffers to use for Serial SLRU */
#define NUM_SERIAL_BUFFERS		16

/*
 * A handle used for sharing SERIALIZABLEXACT objects between the participants
 * in a parallel query.
 */
typedef void *SerializableXactHandle;

/*
 * function prototypes
 */

/* housekeeping for shared memory predicate lock structures */
extern void InitPredicateLocks(void);
extern Size PredicateLockShmemSize(void);

extern void CheckPointPredicate(void);

/* predicate lock reporting */
extern bool PageIsPredicateLocked(Relation relation, BlockNumber blkno);

/* predicate lock maintenance */
extern Snapshot GetSerializableTransactionSnapshot(Snapshot snapshot);
extern void SetSerializableTransactionSnapshot(Snapshot snapshot,
											   VirtualTransactionId *sourcevxid,
											   int sourcepid);
extern void RegisterPredicateLockingXid(TransactionId xid);
extern void PredicateLockRelation(Relation relation, Snapshot snapshot);
extern void PredicateLockPage(Relation relation, BlockNumber blkno, Snapshot snapshot);
extern void PredicateLockTID(Relation relation, ItemPointer tid, Snapshot snapshot,
							 TransactionId insert_xid);
extern void PredicateLockPageSplit(Relation relation, BlockNumber oldblkno, BlockNumber newblkno);
extern void PredicateLockPageCombine(Relation relation, BlockNumber oldblkno, BlockNumber newblkno);
extern void TransferPredicateLocksToHeapRelation(Relation relation);
extern void ReleasePredicateLocks(bool isCommit, bool isReadOnlySafe);

/* conflict detection (may also trigger rollback) */
extern bool CheckForSerializableConflictOutNeeded(Relation relation, Snapshot snapshot);
extern void CheckForSerializableConflictOut(Relation relation, TransactionId xid, Snapshot snapshot);
extern void CheckForSerializableConflictIn(Relation relation, ItemPointer tid, BlockNumber blkno);
extern void CheckTableForSerializableConflictIn(Relation relation);

/* final rollback checking */
extern void PreCommit_CheckForSerializationFailure(void);

/* two-phase commit support */
extern void AtPrepare_PredicateLocks(void);
extern void PostPrepare_PredicateLocks(TransactionId xid);
extern void PredicateLockTwoPhaseFinish(TransactionId xid, bool isCommit);
extern void predicatelock_twophase_recover(TransactionId xid, uint16 info,
										   void *recdata, uint32 len);

/* parallel query support */
extern SerializableXactHandle ShareSerializableXact(void);
extern void AttachSerializableXact(SerializableXactHandle handle);

#endif							/* PREDICATE_H */
