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
 *	scripts_parallel.h
 *		Parallel support for bin/scripts/
 *
 *	Copyright (c) 2003-2020, PostgreSQL Global Development Group
 *
 *	src/bin/scripts/scripts_parallel.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SCRIPTS_PARALLEL_H
#define SCRIPTS_PARALLEL_H

#include "common.h"
#include "libpq-fe.h"


typedef struct ParallelSlot
{
	PGconn	   *connection;		/* One connection */
	bool		isFree;			/* Is it known to be idle? */
} ParallelSlot;

extern ParallelSlot *ParallelSlotsGetIdle(ParallelSlot *slots, int numslots);

extern ParallelSlot *ParallelSlotsSetup(const ConnParams *cparams,
										const char *progname, bool echo,
										PGconn *conn, int numslots);

extern void ParallelSlotsTerminate(ParallelSlot *slots, int numslots);

extern bool ParallelSlotsWaitCompletion(ParallelSlot *slots, int numslots);


#endif							/* SCRIPTS_PARALLEL_H */
