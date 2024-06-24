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
 * noblock.c
 *	  set a file descriptor as blocking or non-blocking
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/port/noblock.c
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"

#include <fcntl.h>


/*
 * Put socket into nonblock mode.
 * Returns true on success, false on failure.
 */
bool
pg_set_noblock(pgsocket sock)
{
#if !defined(WIN32)
	int			flags;

	flags = fcntl(sock, F_GETFL);
	if (flags < 0)
		return false;
	if (fcntl(sock, F_SETFL, (flags | O_NONBLOCK)) == -1)
		return false;
	return true;
#else
	unsigned long ioctlsocket_ret = 1;

	/* Returns non-0 on failure, while fcntl() returns -1 on failure */
	return (ioctlsocket(sock, FIONBIO, &ioctlsocket_ret) == 0);
#endif
}

/*
 * Put socket into blocking mode.
 * Returns true on success, false on failure.
 */
bool
pg_set_block(pgsocket sock)
{
#if !defined(WIN32)
	int			flags;

	flags = fcntl(sock, F_GETFL);
	if (flags < 0)
		return false;
	if (fcntl(sock, F_SETFL, (flags & ~O_NONBLOCK)) == -1)
		return false;
	return true;
#else
	unsigned long ioctlsocket_ret = 0;

	/* Returns non-0 on failure, while fcntl() returns -1 on failure */
	return (ioctlsocket(sock, FIONBIO, &ioctlsocket_ret) == 0);
#endif
}
