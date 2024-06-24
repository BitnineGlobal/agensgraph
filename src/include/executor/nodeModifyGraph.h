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

/*
 * nodeModifyGraph.h
 *
 * Copyright (c) 2022 by Bitnine Global, Inc.
 *
 * src/include/executor/nodeModifyGraph.h
 */

#ifndef NODEMODIFYGRAPH_H
#define NODEMODIFYGRAPH_H

#include "nodes/execnodes.h"
#include "nodes/graphnodes.h"

extern bool enable_multiple_update;

extern ModifyGraphState *ExecInitModifyGraph(ModifyGraph *mgplan,
											 EState *estate, int eflags);
extern void ExecEndModifyGraph(ModifyGraphState *mgstate);

/* global variable - see postgres.c */
extern GraphWriteStats graphWriteStats;

/* for visibility between Cypher clauses */
typedef enum ModifyCid
{
	MODIFY_CID_LOWER_BOUND,		/* for previous clause */
	MODIFY_CID_OUTPUT,			/* for CREATE, MERGE, DELETE */
	MODIFY_CID_SET,				/* for SET, ON MATCH SET, ON CREATE SET */
	MODIFY_CID_NLJOIN_MATCH,	/* for DELETE JOIN, MERGE JOIN */
	MODIFY_CID_MAX
} ModifyCid;

/* hash entry */
typedef struct ModifiedElemEntry
{
	Graphid		key;			/* key */
	Datum		elem;			/* modified graph element ( For set operator ) */
} ModifiedElemEntry;

extern ResultRelInfo *getResultRelInfo(ModifyGraphState *mgstate, Oid relid);
extern Datum findVertex(TupleTableSlot *slot, GraphVertex *gvertex, Graphid *vid);
extern Datum findEdge(TupleTableSlot *slot, GraphEdge *gedge, Graphid *eid);
extern AttrNumber findAttrInSlotByName(TupleTableSlot *slot, char *name);
extern void setSlotValueByName(TupleTableSlot *slot, Datum value, char *name);
extern void setSlotValueByAttnum(TupleTableSlot *slot, Datum value, int attnum);
extern Datum *makeDatumArray(int len);

extern Datum getElementFromEleTable(ModifyGraphState *mgstate, Oid type_oid,
									Datum orig_elem, Datum gid, bool *found);
extern Datum getPathFinal(ModifyGraphState *mgstate, Datum origin);

#endif
