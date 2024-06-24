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
 * barrier.h
 *	  Barriers for synchronizing cooperating processes.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/barrier.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef BARRIER_H
#define BARRIER_H

/*
 * For the header previously known as "barrier.h", please include
 * "port/atomics.h", which deals with atomics, compiler barriers and memory
 * barriers.
 */

#include "storage/condition_variable.h"
#include "storage/spin.h"

typedef struct Barrier
{
	slock_t		mutex;
	int			phase;			/* phase counter */
	int			participants;	/* the number of participants attached */
	int			arrived;		/* the number of participants that have
								 * arrived */
	int			elected;		/* highest phase elected */
	bool		static_party;	/* used only for assertions */
	ConditionVariable condition_variable;
} Barrier;

extern void BarrierInit(Barrier *barrier, int num_workers);
extern bool BarrierArriveAndWait(Barrier *barrier, uint32 wait_event_info);
extern bool BarrierArriveAndDetach(Barrier *barrier);
extern bool BarrierArriveAndDetachExceptLast(Barrier *barrier);
extern int	BarrierAttach(Barrier *barrier);
extern bool BarrierDetach(Barrier *barrier);
extern int	BarrierPhase(Barrier *barrier);
extern int	BarrierParticipants(Barrier *barrier);

#endif							/* BARRIER_H */
