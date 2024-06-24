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
 * execGraphVle.h
 *
 * Copyright (c) 2022 by Bitnine Global, Inc.
 *
 * src/include/executor/execGraphVle.h
 */

#ifndef AGENSGRAPH_EXECGRAPHVLE_H
#define AGENSGRAPH_EXECGRAPHVLE_H

#include "nodes/execnodes.h"

extern GraphVLEState *ExecInitGraphVLE(GraphVLE *vleplan, EState *estate, int eflags);

extern void ExecReScanGraphVLE(GraphVLEState *vle_state);
extern void ExecEndGraphVLE(GraphVLEState *vle_state);


#endif							/* AGENSGRAPH_EXECGRAPHVLE_H */
