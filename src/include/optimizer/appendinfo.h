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
 * appendinfo.h
 *	  Routines for mapping expressions between append rel parent(s) and
 *	  children
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/optimizer/appendinfo.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef APPENDINFO_H
#define APPENDINFO_H

#include "nodes/pathnodes.h"
#include "utils/relcache.h"

extern AppendRelInfo *make_append_rel_info(Relation parentrel,
										   Relation childrel,
										   Index parentRTindex, Index childRTindex);
extern Node *adjust_appendrel_attrs(PlannerInfo *root, Node *node,
									int nappinfos, AppendRelInfo **appinfos);
extern Node *adjust_appendrel_attrs_multilevel(PlannerInfo *root, Node *node,
											   Relids child_relids,
											   Relids top_parent_relids);
extern Relids adjust_child_relids(Relids relids, int nappinfos,
								  AppendRelInfo **appinfos);
extern Relids adjust_child_relids_multilevel(PlannerInfo *root, Relids relids,
											 Relids child_relids,
											 Relids top_parent_relids);
extern List *adjust_inherited_attnums(List *attnums, AppendRelInfo *context);
extern List *adjust_inherited_attnums_multilevel(PlannerInfo *root,
												 List *attnums,
												 Index child_relid,
												 Index top_parent_relid);
extern void get_translated_update_targetlist(PlannerInfo *root, Index relid,
											 List **processed_tlist,
											 List **update_colnos);
extern AppendRelInfo **find_appinfos_by_relids(PlannerInfo *root,
											   Relids relids, int *nappinfos);
extern void add_row_identity_var(PlannerInfo *root, Var *rowid_var,
								 Index rtindex, const char *rowid_name);
extern void add_row_identity_columns(PlannerInfo *root, Index rtindex,
									 RangeTblEntry *target_rte,
									 Relation target_relation);
extern void distribute_row_identity_vars(PlannerInfo *root);

#endif							/* APPENDINFO_H */
