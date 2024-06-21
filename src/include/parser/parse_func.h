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
 * parse_func.h
 *
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_func.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_FUNC_H
#define PARSE_FUNC_H

#include "catalog/namespace.h"
#include "parser/parse_node.h"


/* Result codes for func_get_detail */
typedef enum
{
	FUNCDETAIL_NOTFOUND,		/* no matching function */
	FUNCDETAIL_MULTIPLE,		/* too many matching functions */
	FUNCDETAIL_NORMAL,			/* found a matching regular function */
	FUNCDETAIL_PROCEDURE,		/* found a matching procedure */
	FUNCDETAIL_AGGREGATE,		/* found a matching aggregate function */
	FUNCDETAIL_WINDOWFUNC,		/* found a matching window function */
	FUNCDETAIL_COERCION			/* it's a type coercion request */
} FuncDetailCode;


extern Node *ParseFuncOrColumn(ParseState *pstate, List *funcname, List *fargs,
							   Node *last_srf, FuncCall *fn, bool proc_call,
							   int location);

extern FuncDetailCode func_get_detail(List *funcname,
									  List *fargs, List *fargnames,
									  int nargs, Oid *argtypes,
									  bool expand_variadic, bool expand_defaults,
									  Oid *funcid, Oid *rettype,
									  bool *retset, int *nvargs, Oid *vatype,
									  Oid **true_typeids, List **argdefaults);

extern int	func_match_argtypes(int nargs,
								Oid *input_typeids,
								FuncCandidateList raw_candidates,
								FuncCandidateList *candidates);

extern FuncCandidateList func_select_candidate(int nargs,
											   Oid *input_typeids,
											   FuncCandidateList candidates);

extern void make_fn_arguments(ParseState *pstate,
							  List *fargs,
							  Oid *actual_arg_types,
							  Oid *declared_arg_types);

extern const char *funcname_signature_string(const char *funcname, int nargs,
											 List *argnames, const Oid *argtypes);
extern const char *func_signature_string(List *funcname, int nargs,
										 List *argnames, const Oid *argtypes);

extern Oid	LookupFuncName(List *funcname, int nargs, const Oid *argtypes,
						   bool missing_ok);
extern Oid	LookupFuncWithArgs(ObjectType objtype, ObjectWithArgs *func,
							   bool missing_ok);

extern void check_srf_call_placement(ParseState *pstate, Node *last_srf,
									 int location);

#endif							/* PARSE_FUNC_H */
