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
 * nodeIndexscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeIndexscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEINDEXSCAN_H
#define NODEINDEXSCAN_H

#include "access/genam.h"
#include "access/parallel.h"
#include "nodes/execnodes.h"

extern IndexScanState *ExecInitIndexScan(IndexScan *node, EState *estate, int eflags);
extern void ExecEndIndexScan(IndexScanState *node);
extern void ExecIndexMarkPos(IndexScanState *node);
extern void ExecIndexRestrPos(IndexScanState *node);
extern void ExecReScanIndexScan(IndexScanState *node);

extern void ExecNextIndexScanContext(IndexScanState *node);
extern void ExecPrevIndexScanContext(IndexScanState *node);

extern void ExecIndexScanEstimate(IndexScanState *node, ParallelContext *pcxt);
extern void ExecIndexScanInitializeDSM(IndexScanState *node, ParallelContext *pcxt);
extern void ExecIndexScanReInitializeDSM(IndexScanState *node, ParallelContext *pcxt);
extern void ExecIndexScanInitializeWorker(IndexScanState *node,
										  ParallelWorkerContext *pwcxt);

/*
 * These routines are exported to share code with nodeIndexonlyscan.c and
 * nodeBitmapIndexscan.c
 */
extern void ExecIndexBuildScanKeys(PlanState *planstate, Relation index,
								   List *quals, bool isorderby,
								   ScanKey *scanKeys, int *numScanKeys,
								   IndexRuntimeKeyInfo **runtimeKeys, int *numRuntimeKeys,
								   IndexArrayKeyInfo **arrayKeys, int *numArrayKeys);
extern void ExecIndexEvalRuntimeKeys(ExprContext *econtext,
									 IndexRuntimeKeyInfo *runtimeKeys, int numRuntimeKeys);
extern bool ExecIndexEvalArrayKeys(ExprContext *econtext,
								   IndexArrayKeyInfo *arrayKeys, int numArrayKeys);
extern bool ExecIndexAdvanceArrayKeys(IndexArrayKeyInfo *arrayKeys, int numArrayKeys);

#endif							/* NODEINDEXSCAN_H */
