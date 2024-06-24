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
 * placeholder.h
 *	  prototypes for optimizer/util/placeholder.c.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/placeholder.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PLACEHOLDER_H
#define PLACEHOLDER_H

#include "nodes/pathnodes.h"


extern PlaceHolderVar *make_placeholder_expr(PlannerInfo *root, Expr *expr,
											 Relids phrels);
extern PlaceHolderInfo *find_placeholder_info(PlannerInfo *root,
											  PlaceHolderVar *phv, bool create_new_ph);
extern void find_placeholders_in_jointree(PlannerInfo *root);
extern void update_placeholder_eval_levels(PlannerInfo *root,
										   SpecialJoinInfo *new_sjinfo);
extern void fix_placeholder_input_needed_levels(PlannerInfo *root);
extern void add_placeholders_to_base_rels(PlannerInfo *root);
extern void add_placeholders_to_joinrel(PlannerInfo *root, RelOptInfo *joinrel,
										RelOptInfo *outer_rel, RelOptInfo *inner_rel);

#endif							/* PLACEHOLDER_H */
