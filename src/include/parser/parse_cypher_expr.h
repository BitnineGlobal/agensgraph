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
 * parse_cypher_expr.h
 *	  handle Cypher expressions in parser
 *
 * Copyright (c) 2017 by Bitnine Global, Inc.
 *
 * IDENTIFICATION
 *	  src/include/parser/parse_cypher_expr.h
 */

#ifndef PARSE_CYPHER_EXPR_H
#define PARSE_CYPHER_EXPR_H

#include "parser/parse_node.h"

/* GUC variable (enable/disable null properties) */
extern bool allow_null_properties;

extern Node *transformCypherExpr(ParseState *pstate, Node *expr,
								 ParseExprKind exprKind);
extern Node *transformCypherMapForSet(ParseState *pstate, Node *expr,
									  List **pathelems, char **varname);

/* coerce functions */
extern Node *coerce_expr(ParseState *pstate, Node *expr, Oid ityp, Oid otyp,
						 int32 otypmod, CoercionContext cctx,
						 CoercionForm cform, int loc);

/* clause functions */
extern Node *transformCypherWhere(ParseState *pstate, Node *clause,
								  ParseExprKind exprKind);
extern Node *transformCypherLimit(ParseState *pstate, Node *clause,
								  ParseExprKind exprKind,
								  const char *constructName);
extern List *transformCypherOrderBy(ParseState *pstate, List *orderlist,
									List **targetlist);

/* item list functions */
extern List *transformItemList(ParseState *pstate, List *items,
							   ParseExprKind exprKind);
extern void resolveItemList(ParseState *pstate, List *items);
extern List *transformCypherExprList(ParseState *pstate, List *exprlist,
									 ParseExprKind exprKind);

#endif	/* PARSE_CYPHER_EXPR_H */
