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
 * nodeNestloopVle.h
 *
 * Copyright (c) 2017 by Bitnine Global, Inc.
 *
 * src/include/executor/nodeNestloopVle.h
 */

#ifndef NODENESTLOOPVLE_H
#define NODENESTLOOPVLE_H

#include "nodes/execnodes.h"

extern NestLoopVLEState *ExecInitNestLoopVLE(NestLoopVLE *node, EState *estate,
											 int eflags);
extern void ExecEndNestLoopVLE(NestLoopVLEState *node);
extern void ExecReScanNestLoopVLE(NestLoopVLEState *node);

#endif   /* NODENESTLOOPVLE_H */
