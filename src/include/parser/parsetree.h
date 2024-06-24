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
 * parsetree.h
 *	  Routines to access various components and subcomponents of
 *	  parse trees.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parsetree.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSETREE_H
#define PARSETREE_H

#include "nodes/parsenodes.h"


/* ----------------
 *		range table operations
 * ----------------
 */

/*
 *		rt_fetch
 *
 * NB: this will crash and burn if handed an out-of-range RT index
 */
#define rt_fetch(rangetable_index, rangetable) \
	((RangeTblEntry *) list_nth(rangetable, (rangetable_index)-1))

/*
 * Given an RTE and an attribute number, return the appropriate
 * variable name or alias for that attribute of that RTE.
 */
extern char *get_rte_attribute_name(RangeTblEntry *rte, AttrNumber attnum);

/*
 * Check whether an attribute of an RTE has been dropped
 */
extern bool get_rte_attribute_is_dropped(RangeTblEntry *rte,
										 AttrNumber attnum);


/* ----------------
 *		target list operations
 * ----------------
 */

extern TargetEntry *get_tle_by_resno(List *tlist, AttrNumber resno);

/* ----------------
 *		FOR UPDATE/SHARE info
 * ----------------
 */

extern RowMarkClause *get_parse_rowmark(Query *qry, Index rtindex);

#endif							/* PARSETREE_H */
