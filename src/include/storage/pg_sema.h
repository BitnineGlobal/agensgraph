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
 * pg_sema.h
 *	  Platform-independent API for semaphores.
 *
 * PostgreSQL requires counting semaphores (the kind that keep track of
 * multiple unlock operations, and will allow an equal number of subsequent
 * lock operations before blocking).  The underlying implementation is
 * not the same on every platform.  This file defines the API that must
 * be provided by each port.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/pg_sema.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_SEMA_H
#define PG_SEMA_H

/*
 * struct PGSemaphoreData and pointer type PGSemaphore are the data structure
 * representing an individual semaphore.  The contents of PGSemaphoreData vary
 * across implementations and must never be touched by platform-independent
 * code; hence, PGSemaphoreData is declared as an opaque struct here.
 *
 * However, Windows is sufficiently unlike our other ports that it doesn't
 * seem worth insisting on ABI compatibility for Windows too.  Hence, on
 * that platform just define PGSemaphore as HANDLE.
 */
#ifndef USE_WIN32_SEMAPHORES
typedef struct PGSemaphoreData *PGSemaphore;
#else
typedef HANDLE PGSemaphore;
#endif


/* Report amount of shared memory needed */
extern Size PGSemaphoreShmemSize(int maxSemas);

/* Module initialization (called during postmaster start or shmem reinit) */
extern void PGReserveSemaphores(int maxSemas);

/* Allocate a PGSemaphore structure with initial count 1 */
extern PGSemaphore PGSemaphoreCreate(void);

/* Reset a previously-initialized PGSemaphore to have count 0 */
extern void PGSemaphoreReset(PGSemaphore sema);

/* Lock a semaphore (decrement count), blocking if count would be < 0 */
extern void PGSemaphoreLock(PGSemaphore sema);

/* Unlock a semaphore (increment count) */
extern void PGSemaphoreUnlock(PGSemaphore sema);

/* Lock a semaphore only if able to do so without blocking */
extern bool PGSemaphoreTryLock(PGSemaphore sema);

#endif							/* PG_SEMA_H */
