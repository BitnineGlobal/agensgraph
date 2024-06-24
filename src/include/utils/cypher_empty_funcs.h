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
 * cypher_funcs.h
 *	  Functions in Cypher expressions.
 *
 * Copyright (c) 2022 by Bitnine Global, Inc.
 *
 * src/include/utils/cypher_empty_funcs.h
 */

#ifndef CYPHER_EMPTY_FUNCS_H
#define CYPHER_EMPTY_FUNCS_H

#include "fmgr.h"

extern Datum cypher_to_jsonb(PG_FUNCTION_ARGS);
extern Datum cypher_isempty_jsonb(PG_FUNCTION_ARGS);

#endif
