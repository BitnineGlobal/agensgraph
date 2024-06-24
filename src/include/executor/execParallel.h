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
 * execParallel.h
 *		POSTGRES parallel execution interface
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		src/include/executor/execParallel.h
 *--------------------------------------------------------------------
 */

#ifndef EXECPARALLEL_H
#define EXECPARALLEL_H

#include "access/parallel.h"
#include "nodes/execnodes.h"
#include "nodes/parsenodes.h"
#include "nodes/plannodes.h"
#include "utils/dsa.h"

typedef struct SharedExecutorInstrumentation SharedExecutorInstrumentation;

typedef struct ParallelExecutorInfo
{
	PlanState  *planstate;		/* plan subtree we're running in parallel */
	ParallelContext *pcxt;		/* parallel context we're using */
	BufferUsage *buffer_usage;	/* points to bufusage area in DSM */
	WalUsage   *wal_usage;		/* walusage area in DSM */
	SharedExecutorInstrumentation *instrumentation; /* optional */
	struct SharedJitInstrumentation *jit_instrumentation;	/* optional */
	dsa_area   *area;			/* points to DSA area in DSM */
	dsa_pointer param_exec;		/* serialized PARAM_EXEC parameters */
	bool		finished;		/* set true by ExecParallelFinish */
	/* These two arrays have pcxt->nworkers_launched entries: */
	shm_mq_handle **tqueue;		/* tuple queues for worker output */
	struct TupleQueueReader **reader;	/* tuple reader/writer support */
} ParallelExecutorInfo;

extern ParallelExecutorInfo *ExecInitParallelPlan(PlanState *planstate,
												  EState *estate, Bitmapset *sendParam, int nworkers,
												  int64 tuples_needed);
extern void ExecParallelCreateReaders(ParallelExecutorInfo *pei);
extern void ExecParallelFinish(ParallelExecutorInfo *pei);
extern void ExecParallelCleanup(ParallelExecutorInfo *pei);
extern void ExecParallelReinitialize(PlanState *planstate,
									 ParallelExecutorInfo *pei, Bitmapset *sendParam);

extern void ParallelQueryMain(dsm_segment *seg, shm_toc *toc);

#endif							/* EXECPARALLEL_H */
