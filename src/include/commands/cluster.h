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
 * cluster.h
 *	  header file for postgres cluster command stuff
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994-5, Regents of the University of California
 *
 * src/include/commands/cluster.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef CLUSTER_H
#define CLUSTER_H

#include "nodes/parsenodes.h"
#include "storage/lock.h"
#include "utils/relcache.h"


extern void cluster(ClusterStmt *stmt, bool isTopLevel);
extern void cluster_rel(Oid tableOid, Oid indexOid, int options);
extern void check_index_is_clusterable(Relation OldHeap, Oid indexOid,
									   bool recheck, LOCKMODE lockmode);
extern void mark_index_clustered(Relation rel, Oid indexOid, bool is_internal);

extern Oid	make_new_heap(Oid OIDOldHeap, Oid NewTableSpace, char relpersistence,
						  LOCKMODE lockmode);
extern void finish_heap_swap(Oid OIDOldHeap, Oid OIDNewHeap,
							 bool is_system_catalog,
							 bool swap_toast_by_content,
							 bool check_constraints,
							 bool is_internal,
							 TransactionId frozenXid,
							 MultiXactId minMulti,
							 char newrelpersistence);

#endif							/* CLUSTER_H */
