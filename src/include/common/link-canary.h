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
 * link-canary.h
 *	  Detect whether src/common functions came from frontend or backend.
 *
 * Copyright (c) 2018-2020, PostgreSQL Global Development Group
 *
 * src/include/common/link-canary.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef LINK_CANARY_H
#define LINK_CANARY_H

extern bool pg_link_canary_is_frontend(void);

#endif							/* LINK_CANARY_H */
