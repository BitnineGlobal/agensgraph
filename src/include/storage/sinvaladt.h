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
 * sinvaladt.h
 *	  POSTGRES shared cache invalidation data manager.
 *
 * The shared cache invalidation manager is responsible for transmitting
 * invalidation messages between backends.  Any message sent by any backend
 * must be delivered to all already-running backends before it can be
 * forgotten.  (If we run out of space, we instead deliver a "RESET"
 * message to backends that have fallen too far behind.)
 *
 * The struct type SharedInvalidationMessage, defining the contents of
 * a single message, is defined in sinval.h.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/sinvaladt.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SINVALADT_H
#define SINVALADT_H

#include "storage/lock.h"
#include "storage/sinval.h"

/*
 * prototypes for functions in sinvaladt.c
 */
extern Size SInvalShmemSize(void);
extern void CreateSharedInvalidationState(void);
extern void SharedInvalBackendInit(bool sendOnly);
extern PGPROC *BackendIdGetProc(int backendID);
extern void BackendIdGetTransactionIds(int backendID, TransactionId *xid, TransactionId *xmin);

extern void SIInsertDataEntries(const SharedInvalidationMessage *data, int n);
extern int	SIGetDataEntries(SharedInvalidationMessage *data, int datasize);
extern void SICleanupQueue(bool callerHasWriteLock, int minFree);

extern LocalTransactionId GetNextLocalTransactionId(void);

#endif							/* SINVALADT_H */
