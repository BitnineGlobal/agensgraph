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
 * nodeIndexonlyscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeIndexonlyscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEINDEXONLYSCAN_H
#define NODEINDEXONLYSCAN_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

extern IndexOnlyScanState *ExecInitIndexOnlyScan(IndexOnlyScan *node, EState *estate, int eflags);
extern void ExecEndIndexOnlyScan(IndexOnlyScanState *node);
extern void ExecIndexOnlyMarkPos(IndexOnlyScanState *node);
extern void ExecIndexOnlyRestrPos(IndexOnlyScanState *node);
extern void ExecReScanIndexOnlyScan(IndexOnlyScanState *node);
extern void ExecNextIndexOnlyScanContext(IndexOnlyScanState *node);
extern void ExecPrevIndexOnlyScanContext(IndexOnlyScanState *node);

/* Support functions for parallel index-only scans */
extern void ExecIndexOnlyScanEstimate(IndexOnlyScanState *node,
									  ParallelContext *pcxt);
extern void ExecIndexOnlyScanInitializeDSM(IndexOnlyScanState *node,
										   ParallelContext *pcxt);
extern void ExecIndexOnlyScanReInitializeDSM(IndexOnlyScanState *node,
											 ParallelContext *pcxt);
extern void ExecIndexOnlyScanInitializeWorker(IndexOnlyScanState *node,
											  ParallelWorkerContext *pwcxt);

#endif							/* NODEINDEXONLYSCAN_H */
