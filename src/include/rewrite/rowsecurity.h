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

/* -------------------------------------------------------------------------
 *
 * rowsecurity.h
 *
 *	  prototypes for rewrite/rowsecurity.c and the structures for managing
 *	  the row security policies for relations in relcache.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * -------------------------------------------------------------------------
 */
#ifndef ROWSECURITY_H
#define ROWSECURITY_H

#include "nodes/parsenodes.h"
#include "utils/array.h"
#include "utils/relcache.h"

typedef struct RowSecurityPolicy
{
	char	   *policy_name;	/* Name of the policy */
	char		polcmd;			/* Type of command policy is for */
	ArrayType  *roles;			/* Array of roles policy is for */
	bool		permissive;		/* restrictive or permissive policy */
	Expr	   *qual;			/* Expression to filter rows */
	Expr	   *with_check_qual;	/* Expression to limit rows allowed */
	bool		hassublinks;	/* If either expression has sublinks */
} RowSecurityPolicy;

typedef struct RowSecurityDesc
{
	MemoryContext rscxt;		/* row security memory context */
	List	   *policies;		/* list of row security policies */
} RowSecurityDesc;

typedef List *(*row_security_policy_hook_type) (CmdType cmdtype,
												Relation relation);

extern PGDLLIMPORT row_security_policy_hook_type row_security_policy_hook_permissive;

extern PGDLLIMPORT row_security_policy_hook_type row_security_policy_hook_restrictive;

extern void get_row_security_policies(Query *root,
									  RangeTblEntry *rte, int rt_index,
									  List **securityQuals, List **withCheckOptions,
									  bool *hasRowSecurity, bool *hasSubLinks);

#endif							/* ROWSECURITY_H */
