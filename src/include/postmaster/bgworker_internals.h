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

/*--------------------------------------------------------------------
 * bgworker_internals.h
 *		POSTGRES pluggable background workers internals
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		src/include/postmaster/bgworker_internals.h
 *--------------------------------------------------------------------
 */
#ifndef BGWORKER_INTERNALS_H
#define BGWORKER_INTERNALS_H

#include "datatype/timestamp.h"
#include "lib/ilist.h"
#include "postmaster/bgworker.h"

/* GUC options */

/*
 * Maximum possible value of parallel workers.
 */
#define MAX_PARALLEL_WORKER_LIMIT 1024

/*
 * List of background workers, private to postmaster.
 *
 * A worker that requests a database connection during registration will have
 * rw_backend set, and will be present in BackendList.  Note: do not rely on
 * rw_backend being non-NULL for shmem-connected workers!
 */
typedef struct RegisteredBgWorker
{
	BackgroundWorker rw_worker; /* its registry entry */
	struct bkend *rw_backend;	/* its BackendList entry, or NULL */
	pid_t		rw_pid;			/* 0 if not running */
	int			rw_child_slot;
	TimestampTz rw_crashed_at;	/* if not 0, time it last crashed */
	int			rw_shmem_slot;
	bool		rw_terminate;
	slist_node	rw_lnode;		/* list link */
} RegisteredBgWorker;

extern slist_head BackgroundWorkerList;

extern Size BackgroundWorkerShmemSize(void);
extern void BackgroundWorkerShmemInit(void);
extern void BackgroundWorkerStateChange(bool allow_new_workers);
extern void ForgetBackgroundWorker(slist_mutable_iter *cur);
extern void ReportBackgroundWorkerPID(RegisteredBgWorker *);
extern void ReportBackgroundWorkerExit(slist_mutable_iter *cur);
extern void BackgroundWorkerStopNotifications(pid_t pid);
extern void ForgetUnstartedBackgroundWorkers(void);
extern void ResetBackgroundWorkerCrashTimes(void);

/* Function to start a background worker, called from postmaster.c */
extern void StartBackgroundWorker(void) pg_attribute_noreturn();

#ifdef EXEC_BACKEND
extern BackgroundWorker *BackgroundWorkerEntry(int slotno);
#endif

#endif							/* BGWORKER_INTERNALS_H */
