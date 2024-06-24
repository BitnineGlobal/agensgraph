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
 * pqsignal.c
 *	  reliable BSD-style signal(2) routine stolen from RWW who stole it
 *	  from Stevens...
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/port/pqsignal.c
 *
 *	We now assume that all Unix-oid systems have POSIX sigaction(2)
 *	with support for restartable signals (SA_RESTART).  We used to also
 *	support BSD-style signal(2), but there really shouldn't be anything
 *	out there anymore that doesn't have the POSIX API.
 *
 *	Windows, of course, is resolutely in a class by itself.  In the backend,
 *	we don't use this file at all; src/backend/port/win32/signal.c provides
 *	pqsignal() for the backend environment.  Frontend programs can use
 *	this version of pqsignal() if they wish, but beware that this does
 *	not provide restartable signals on Windows.
 *
 * ------------------------------------------------------------------------
 */

#include "c.h"

#include <signal.h>

#if !defined(WIN32) || defined(FRONTEND)

/*
 * Set up a signal handler, with SA_RESTART, for signal "signo"
 *
 * Returns the previous handler.
 */
pqsigfunc
pqsignal(int signo, pqsigfunc func)
{
#ifndef WIN32
	struct sigaction act,
				oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
#ifdef SA_NOCLDSTOP
	if (signo == SIGCHLD)
		act.sa_flags |= SA_NOCLDSTOP;
#endif
	if (sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;
	return oact.sa_handler;
#else							/* WIN32 */
	return signal(signo, func);
#endif
}

#endif							/* !defined(WIN32) || defined(FRONTEND) */
