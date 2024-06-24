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
 * parse_agg.h
 *	  handle aggregates and window functions in parser
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_agg.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_AGG_H
#define PARSE_AGG_H

#include "parser/parse_node.h"

extern void transformAggregateCall(ParseState *pstate, Aggref *agg,
								   List *args, List *aggorder,
								   bool agg_distinct);

extern Node *transformGroupingFunc(ParseState *pstate, GroupingFunc *g);

extern void transformWindowFuncCall(ParseState *pstate, WindowFunc *wfunc,
									WindowDef *windef);

extern void parseCheckAggregates(ParseState *pstate, Query *qry);

extern List *expand_grouping_sets(List *groupingSets, int limit);

extern int	get_aggregate_argtypes(Aggref *aggref, Oid *inputTypes);

extern Oid	resolve_aggregate_transtype(Oid aggfuncid,
										Oid aggtranstype,
										Oid *inputTypes,
										int numArguments);

extern void build_aggregate_transfn_expr(Oid *agg_input_types,
										 int agg_num_inputs,
										 int agg_num_direct_inputs,
										 bool agg_variadic,
										 Oid agg_state_type,
										 Oid agg_input_collation,
										 Oid transfn_oid,
										 Oid invtransfn_oid,
										 Expr **transfnexpr,
										 Expr **invtransfnexpr);

extern void build_aggregate_combinefn_expr(Oid agg_state_type,
										   Oid agg_input_collation,
										   Oid combinefn_oid,
										   Expr **combinefnexpr);

extern void build_aggregate_serialfn_expr(Oid serialfn_oid,
										  Expr **serialfnexpr);

extern void build_aggregate_deserialfn_expr(Oid deserialfn_oid,
											Expr **deserialfnexpr);

extern void build_aggregate_finalfn_expr(Oid *agg_input_types,
										 int num_finalfn_inputs,
										 Oid agg_state_type,
										 Oid agg_result_type,
										 Oid agg_input_collation,
										 Oid finalfn_oid,
										 Expr **finalfnexpr);

#endif							/* PARSE_AGG_H */
