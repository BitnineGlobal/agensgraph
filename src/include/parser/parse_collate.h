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
 * parse_collate.h
 *	Routines for assigning collation information.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_collate.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_COLLATE_H
#define PARSE_COLLATE_H

#include "parser/parse_node.h"

extern void assign_query_collations(ParseState *pstate, Query *query);

extern void assign_list_collations(ParseState *pstate, List *exprs);

extern void assign_expr_collations(ParseState *pstate, Node *expr);

extern Oid	select_common_collation(ParseState *pstate, List *exprs, bool none_ok);

#endif							/* PARSE_COLLATE_H */
