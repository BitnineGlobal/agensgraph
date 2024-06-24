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

/*--------------------------------------------------------------------------
 *
 * test_shm_mq.h
 *		Definitions for shared memory message queues
 *
 * Copyright (c) 2013-2020, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *		src/test/modules/test_shm_mq/test_shm_mq.h
 *
 * -------------------------------------------------------------------------
 */

#ifndef TEST_SHM_MQ_H
#define TEST_SHM_MQ_H

#include "storage/dsm.h"
#include "storage/shm_mq.h"
#include "storage/spin.h"

/* Identifier for shared memory segments used by this extension. */
#define		PG_TEST_SHM_MQ_MAGIC		0x79fb2447

/*
 * This structure is stored in the dynamic shared memory segment.  We use
 * it to determine whether all workers started up OK and successfully
 * attached to their respective shared message queues.
 */
typedef struct
{
	slock_t		mutex;
	int			workers_total;
	int			workers_attached;
	int			workers_ready;
} test_shm_mq_header;

/* Set up dynamic shared memory and background workers for test run. */
extern void test_shm_mq_setup(int64 queue_size, int32 nworkers,
							  dsm_segment **seg, shm_mq_handle **output,
							  shm_mq_handle **input);

/* Main entrypoint for a worker. */
extern void test_shm_mq_main(Datum) pg_attribute_noreturn();

#endif
