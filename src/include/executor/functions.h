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
 * functions.h
 *		Declarations for execution of SQL-language functions.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/functions.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "nodes/execnodes.h"
#include "tcop/dest.h"

/* This struct is known only within executor/functions.c */
typedef struct SQLFunctionParseInfo *SQLFunctionParseInfoPtr;

extern Datum fmgr_sql(PG_FUNCTION_ARGS);

extern SQLFunctionParseInfoPtr prepare_sql_fn_parse_info(HeapTuple procedureTuple,
														 Node *call_expr,
														 Oid inputCollation);

extern void sql_fn_parser_setup(struct ParseState *pstate,
								SQLFunctionParseInfoPtr pinfo);

extern void check_sql_fn_statements(List *queryTreeLists);

extern bool check_sql_fn_retval(List *queryTreeLists,
								Oid rettype, TupleDesc rettupdesc,
								bool insertDroppedCols,
								List **resultTargetList);

extern DestReceiver *CreateSQLFunctionDestReceiver(void);

#endif							/* FUNCTIONS_H */
