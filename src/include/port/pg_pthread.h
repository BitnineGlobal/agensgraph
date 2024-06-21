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
 * Declarations for missing POSIX thread components.
 *
 *	  Currently this supplies an implementation of pthread_barrier_t for the
 *	  benefit of macOS, which lacks it.  These declarations are not in port.h,
 *	  because that'd require <pthread.h> to be included by every translation
 *	  unit.
 *
 *-------------------------------------------------------------------------
 */

#ifndef PG_PTHREAD_H
#define PG_PTHREAD_H

#include <pthread.h>

#ifndef HAVE_PTHREAD_BARRIER_WAIT

#ifndef PTHREAD_BARRIER_SERIAL_THREAD
#define PTHREAD_BARRIER_SERIAL_THREAD (-1)
#endif

typedef struct pg_pthread_barrier
{
	bool		sense;			/* we only need a one bit phase */
	int			count;			/* number of threads expected */
	int			arrived;		/* number of threads that have arrived */
	pthread_mutex_t mutex;
	pthread_cond_t cond;
} pthread_barrier_t;

extern int	pthread_barrier_init(pthread_barrier_t *barrier,
								 const void *attr,
								 int count);
extern int	pthread_barrier_wait(pthread_barrier_t *barrier);
extern int	pthread_barrier_destroy(pthread_barrier_t *barrier);

#endif

#endif
