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
 * pthread_barrier_wait.c
 *    Implementation of pthread_barrier_t support for platforms lacking it.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *    src/port/pthread_barrier_wait.c
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"

#include "port/pg_pthread.h"

int
pthread_barrier_init(pthread_barrier_t *barrier, const void *attr, int count)
{
	int			error;

	barrier->sense = false;
	barrier->count = count;
	barrier->arrived = 0;
	if ((error = pthread_cond_init(&barrier->cond, NULL)) != 0)
		return error;
	if ((error = pthread_mutex_init(&barrier->mutex, NULL)) != 0)
	{
		pthread_cond_destroy(&barrier->cond);
		return error;
	}

	return 0;
}

int
pthread_barrier_wait(pthread_barrier_t *barrier)
{
	bool		initial_sense;

	pthread_mutex_lock(&barrier->mutex);

	/* We have arrived at the barrier. */
	barrier->arrived++;
	Assert(barrier->arrived <= barrier->count);

	/* If we were the last to arrive, release the others and return. */
	if (barrier->arrived == barrier->count)
	{
		barrier->arrived = 0;
		barrier->sense = !barrier->sense;
		pthread_mutex_unlock(&barrier->mutex);
		pthread_cond_broadcast(&barrier->cond);

		return PTHREAD_BARRIER_SERIAL_THREAD;
	}

	/* Wait for someone else to flip the sense. */
	initial_sense = barrier->sense;
	do
	{
		pthread_cond_wait(&barrier->cond, &barrier->mutex);
	} while (barrier->sense == initial_sense);

	pthread_mutex_unlock(&barrier->mutex);

	return 0;
}

int
pthread_barrier_destroy(pthread_barrier_t *barrier)
{
	pthread_cond_destroy(&barrier->cond);
	pthread_mutex_destroy(&barrier->mutex);
	return 0;
}
