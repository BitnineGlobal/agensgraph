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
 *	parallel_slot.h
 *		Parallel support for bin/scripts/
 *
 *	Copyright (c) 2003-2021, PostgreSQL Global Development Group
 *
 *	src/include/fe_utils/parallel_slot.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARALLEL_SLOT_H
#define PARALLEL_SLOT_H

#include "fe_utils/connect_utils.h"
#include "libpq-fe.h"

typedef bool (*ParallelSlotResultHandler) (PGresult *res, PGconn *conn,
										   void *context);

typedef struct ParallelSlot
{
	PGconn	   *connection;		/* One connection */
	bool		inUse;			/* Is the slot being used? */

	/*
	 * Prior to issuing a command or query on 'connection', a handler callback
	 * function may optionally be registered to be invoked to process the
	 * results, and context information may optionally be registered for use
	 * by the handler.  If unset, these fields should be NULL.
	 */
	ParallelSlotResultHandler handler;
	void	   *handler_context;
} ParallelSlot;

typedef struct ParallelSlotArray
{
	int			numslots;
	ConnParams *cparams;
	const char *progname;
	bool		echo;
	const char *initcmd;
	ParallelSlot slots[FLEXIBLE_ARRAY_MEMBER];
} ParallelSlotArray;

static inline void
ParallelSlotSetHandler(ParallelSlot *slot, ParallelSlotResultHandler handler,
					   void *context)
{
	slot->handler = handler;
	slot->handler_context = context;
}

static inline void
ParallelSlotClearHandler(ParallelSlot *slot)
{
	slot->handler = NULL;
	slot->handler_context = NULL;
}

extern ParallelSlot *ParallelSlotsGetIdle(ParallelSlotArray *slots,
										  const char *dbname);

extern ParallelSlotArray *ParallelSlotsSetup(int numslots, ConnParams *cparams,
											 const char *progname, bool echo,
											 const char *initcmd);

extern void ParallelSlotsAdoptConn(ParallelSlotArray *sa, PGconn *conn);

extern void ParallelSlotsTerminate(ParallelSlotArray *sa);

extern bool ParallelSlotsWaitCompletion(ParallelSlotArray *sa);

extern bool TableCommandResultHandler(PGresult *res, PGconn *conn,
									  void *context);

#endif							/* PARALLEL_SLOT_H */
