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
 * parse_clause.h
 *	  handle clauses in parser
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_clause.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_CLAUSE_H
#define PARSE_CLAUSE_H

#include "parser/parse_node.h"

extern void transformFromClause(ParseState *pstate, List *frmList);
extern int	setTargetTable(ParseState *pstate, RangeVar *relation,
						   bool inh, bool alsoSource, AclMode requiredPerms);

extern Node *transformWhereClause(ParseState *pstate, Node *clause,
								  ParseExprKind exprKind, const char *constructName);
extern Node *transformLimitClause(ParseState *pstate, Node *clause,
								  ParseExprKind exprKind, const char *constructName,
								  LimitOption limitOption);
extern List *transformGroupClause(ParseState *pstate, List *grouplist,
								  List **groupingSets,
								  List **targetlist, List *sortClause,
								  ParseExprKind exprKind, bool useSQL99);
extern List *transformSortClause(ParseState *pstate, List *orderlist,
								 List **targetlist, ParseExprKind exprKind,
								 bool useSQL99);

extern List *transformWindowDefinitions(ParseState *pstate,
										List *windowdefs,
										List **targetlist);

extern List *transformDistinctClause(ParseState *pstate,
									 List **targetlist, List *sortClause, bool is_agg);
extern List *transformDistinctOnClause(ParseState *pstate, List *distinctlist,
									   List **targetlist, List *sortClause);
extern void transformOnConflictArbiter(ParseState *pstate,
									   OnConflictClause *onConflictClause,
									   List **arbiterExpr, Node **arbiterWhere,
									   Oid *constraint);

extern List *addTargetToSortList(ParseState *pstate, TargetEntry *tle,
								 List *sortlist, List *targetlist, SortBy *sortby);
extern Index assignSortGroupRef(TargetEntry *tle, List *tlist);
extern bool targetIsInSortList(TargetEntry *tle, Oid sortop, List *sortList);

/* used from agensgraph */
extern List *generateGroupClause(ParseState *pstate, List **targetlist,
								 List *sortClause);

#endif							/* PARSE_CLAUSE_H */
