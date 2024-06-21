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
 * restrictinfo.h
 *	  prototypes for restrictinfo.c.
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/restrictinfo.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef RESTRICTINFO_H
#define RESTRICTINFO_H

#include "nodes/pathnodes.h"


/* Convenience macro for the common case of a valid-everywhere qual */
#define make_simple_restrictinfo(root, clause)  \
	make_restrictinfo(root, clause, true, false, false, 0, NULL, NULL, NULL)

extern RestrictInfo *make_restrictinfo(PlannerInfo *root,
									   Expr *clause,
									   bool is_pushed_down,
									   bool outerjoin_delayed,
									   bool pseudoconstant,
									   Index security_level,
									   Relids required_relids,
									   Relids outer_relids,
									   Relids nullable_relids);
extern RestrictInfo *commute_restrictinfo(RestrictInfo *rinfo, Oid comm_op);
extern bool restriction_is_or_clause(RestrictInfo *restrictinfo);
extern bool restriction_is_securely_promotable(RestrictInfo *restrictinfo,
											   RelOptInfo *rel);
extern List *get_actual_clauses(List *restrictinfo_list);
extern List *extract_actual_clauses(List *restrictinfo_list,
									bool pseudoconstant);
extern void extract_actual_join_clauses(List *restrictinfo_list,
										Relids joinrelids,
										List **joinquals,
										List **otherquals);
extern bool join_clause_is_movable_to(RestrictInfo *rinfo, RelOptInfo *baserel);
extern bool join_clause_is_movable_into(RestrictInfo *rinfo,
										Relids currentrelids,
										Relids current_and_outer);

#endif							/* RESTRICTINFO_H */
