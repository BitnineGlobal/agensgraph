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
 * nodeForeignscan.h
 *
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeForeignscan.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODEFOREIGNSCAN_H
#define NODEFOREIGNSCAN_H

#include "access/parallel.h"
#include "nodes/execnodes.h"

extern ForeignScanState *ExecInitForeignScan(ForeignScan *node, EState *estate, int eflags);
extern void ExecEndForeignScan(ForeignScanState *node);
extern void ExecReScanForeignScan(ForeignScanState *node);

extern void ExecForeignScanEstimate(ForeignScanState *node,
									ParallelContext *pcxt);
extern void ExecForeignScanInitializeDSM(ForeignScanState *node,
										 ParallelContext *pcxt);
extern void ExecForeignScanReInitializeDSM(ForeignScanState *node,
										   ParallelContext *pcxt);
extern void ExecForeignScanInitializeWorker(ForeignScanState *node,
											ParallelWorkerContext *pwcxt);
extern void ExecShutdownForeignScan(ForeignScanState *node);

#endif							/* NODEFOREIGNSCAN_H */
