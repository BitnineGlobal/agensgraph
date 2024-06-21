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
 * getpeereid.c
 *		get peer userid for UNIX-domain socket connection
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/port/getpeereid.c
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"

#include <sys/param.h>
#include <sys/socket.h>
#include <unistd.h>
#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#ifdef HAVE_UCRED_H
#include <ucred.h>
#endif
#ifdef HAVE_SYS_UCRED_H
#include <sys/ucred.h>
#endif


/*
 * BSD-style getpeereid() for platforms that lack it.
 */
int
getpeereid(int sock, uid_t *uid, gid_t *gid)
{
#if defined(SO_PEERCRED)
	/* Linux: use getsockopt(SO_PEERCRED) */
	struct ucred peercred;
	ACCEPT_TYPE_ARG3 so_len = sizeof(peercred);

	if (getsockopt(sock, SOL_SOCKET, SO_PEERCRED, &peercred, &so_len) != 0 ||
		so_len != sizeof(peercred))
		return -1;
	*uid = peercred.uid;
	*gid = peercred.gid;
	return 0;
#elif defined(LOCAL_PEERCRED)
	/* Debian with FreeBSD kernel: use getsockopt(LOCAL_PEERCRED) */
	struct xucred peercred;
	ACCEPT_TYPE_ARG3 so_len = sizeof(peercred);

	if (getsockopt(sock, 0, LOCAL_PEERCRED, &peercred, &so_len) != 0 ||
		so_len != sizeof(peercred) ||
		peercred.cr_version != XUCRED_VERSION)
		return -1;
	*uid = peercred.cr_uid;
	*gid = peercred.cr_gid;
	return 0;
#elif defined(HAVE_GETPEERUCRED)
	/* Solaris: use getpeerucred() */
	ucred_t    *ucred;

	ucred = NULL;				/* must be initialized to NULL */
	if (getpeerucred(sock, &ucred) == -1)
		return -1;

	*uid = ucred_geteuid(ucred);
	*gid = ucred_getegid(ucred);
	ucred_free(ucred);

	if (*uid == (uid_t) (-1) || *gid == (gid_t) (-1))
		return -1;
	return 0;
#else
	/* No implementation available on this platform */
	errno = ENOSYS;
	return -1;
#endif
}
