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

/*
 * ag_graph_fn.h
 *	  prototypes for functions in backend/catalog/ag_graph.c
 *
 * Copyright (c) 2016 by Bitnine Global, Inc.
 *
 * IDENTIFICATION
 *	  src/include/catalog/ag_graph_fn.h
 */
#ifndef AG_GRAPH_FN_H
#define AG_GRAPH_FN_H

#include "nodes/parsenodes.h"

extern char *graph_path;
extern bool cypher_allow_unsafe_dml;
extern bool cypher_allow_unsafe_ddl;

extern char *get_graph_path(bool lookup_cache);
extern Oid get_graph_path_oid(void);

extern Oid GraphCreate(CreateGraphStmt *stmt, const char *queryString,
					   int stmt_location, int stmt_len);

#endif	/* AG_GRAPH_FN_H */
