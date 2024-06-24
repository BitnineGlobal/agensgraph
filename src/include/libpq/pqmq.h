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
 * pqmq.h
 *	  Use the frontend/backend protocol for communication over a shm_mq
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/libpq/pqmq.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PQMQ_H
#define PQMQ_H

#include "lib/stringinfo.h"
#include "storage/shm_mq.h"

extern void pq_redirect_to_shm_mq(dsm_segment *seg, shm_mq_handle *mqh);
extern void pq_set_parallel_leader(pid_t pid, BackendId backend_id);

extern void pq_parse_errornotice(StringInfo str, ErrorData *edata);

#endif							/* PQMQ_H */
