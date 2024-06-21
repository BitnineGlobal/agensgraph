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
 * parse_oper.h
 *		handle operator things for parser
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_oper.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_OPER_H
#define PARSE_OPER_H

#include "access/htup.h"
#include "nodes/parsenodes.h"
#include "parser/parse_node.h"


typedef HeapTuple Operator;

/* Routines to look up an operator given name and exact input type(s) */
extern Oid	LookupOperName(ParseState *pstate, List *opername,
						   Oid oprleft, Oid oprright,
						   bool noError, int location);
extern Oid	LookupOperWithArgs(ObjectWithArgs *oper, bool noError);

/* Routines to find operators matching a name and given input types */
/* NB: the selected operator may require coercion of the input types! */
extern Operator oper(ParseState *pstate, List *op, Oid arg1, Oid arg2,
					 bool noError, int location);
extern Operator right_oper(ParseState *pstate, List *op, Oid arg,
						   bool noError, int location);
extern Operator left_oper(ParseState *pstate, List *op, Oid arg,
						  bool noError, int location);

/* Routines to find operators that DO NOT require coercion --- ie, their */
/* input types are either exactly as given, or binary-compatible */
extern Operator compatible_oper(ParseState *pstate, List *op,
								Oid arg1, Oid arg2,
								bool noError, int location);

/* currently no need for compatible_left_oper/compatible_right_oper */

/* Routines for identifying "<", "=", ">" operators for a type */
extern void get_sort_group_operators(Oid argtype,
									 bool needLT, bool needEQ, bool needGT,
									 Oid *ltOpr, Oid *eqOpr, Oid *gtOpr,
									 bool *isHashable);

/* Convenience routines for common calls on the above */
extern Oid	compatible_oper_opid(List *op, Oid arg1, Oid arg2, bool noError);

/* Extract operator OID or underlying-function OID from an Operator tuple */
extern Oid	oprid(Operator op);
extern Oid	oprfuncid(Operator op);

/* Build expression tree for an operator invocation */
extern Expr *make_op(ParseState *pstate, List *opname,
					 Node *ltree, Node *rtree, Node *last_srf, int location);
extern Expr *make_scalar_array_op(ParseState *pstate, List *opname,
								  bool useOr,
								  Node *ltree, Node *rtree, int location);

#endif							/* PARSE_OPER_H */
