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
 * rewriteDefine.h
 *
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/rewrite/rewriteDefine.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef REWRITEDEFINE_H
#define REWRITEDEFINE_H

#include "catalog/objectaddress.h"
#include "nodes/parsenodes.h"
#include "utils/relcache.h"

#define RULE_FIRES_ON_ORIGIN	'O'
#define RULE_FIRES_ALWAYS		'A'
#define RULE_FIRES_ON_REPLICA	'R'
#define RULE_DISABLED			'D'

extern ObjectAddress DefineRule(RuleStmt *stmt, const char *queryString);

extern ObjectAddress DefineQueryRewrite(const char *rulename,
										Oid event_relid,
										Node *event_qual,
										CmdType event_type,
										bool is_instead,
										bool replace,
										List *action);

extern ObjectAddress RenameRewriteRule(RangeVar *relation, const char *oldName,
									   const char *newName);

extern void setRuleCheckAsUser(Node *node, Oid userid);

extern void EnableDisableRule(Relation rel, const char *rulename,
							  char fires_when);

#endif							/* REWRITEDEFINE_H */
