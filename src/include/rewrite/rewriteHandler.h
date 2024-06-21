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
 * rewriteHandler.h
 *		External interface to query rewriter.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/rewrite/rewriteHandler.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef REWRITEHANDLER_H
#define REWRITEHANDLER_H

#include "nodes/parsenodes.h"
#include "utils/relcache.h"

extern List *QueryRewrite(Query *parsetree);
extern void AcquireRewriteLocks(Query *parsetree,
								bool forExecute,
								bool forUpdatePushedDown);

extern Node *build_column_default(Relation rel, int attrno);
extern void rewriteTargetListUD(Query *parsetree, RangeTblEntry *target_rte,
								Relation target_relation);

extern void fill_extraUpdatedCols(RangeTblEntry *target_rte,
								  Relation target_relation);

extern Query *get_view_query(Relation view);
extern const char *view_query_is_auto_updatable(Query *viewquery,
												bool check_cols);
extern int	relation_is_updatable(Oid reloid,
								  List *outer_reloids,
								  bool include_triggers,
								  Bitmapset *include_cols);

#endif							/* REWRITEHANDLER_H */
