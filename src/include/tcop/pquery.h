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
 * pquery.h
 *	  prototypes for pquery.c.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/tcop/pquery.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PQUERY_H
#define PQUERY_H

#include "nodes/parsenodes.h"
#include "utils/portal.h"

struct PlannedStmt;				/* avoid including plannodes.h here */


extern PGDLLIMPORT Portal ActivePortal;


extern PortalStrategy ChoosePortalStrategy(List *stmts);

extern List *FetchPortalTargetList(Portal portal);

extern List *FetchStatementTargetList(Node *stmt);

extern void PortalStart(Portal portal, ParamListInfo params,
						int eflags, Snapshot snapshot);

extern void PortalSetResultFormat(Portal portal, int nFormats,
								  int16 *formats);

extern bool PortalRun(Portal portal, long count, bool isTopLevel,
					  bool run_once, DestReceiver *dest, DestReceiver *altdest,
					  QueryCompletion *qc);

extern uint64 PortalRunFetch(Portal portal,
							 FetchDirection fdirection,
							 long count,
							 DestReceiver *dest);

extern bool PlannedStmtRequiresSnapshot(struct PlannedStmt *pstmt);

extern void EnsurePortalSnapshotExists(void);

#endif							/* PQUERY_H */
