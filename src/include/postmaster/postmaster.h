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
 * postmaster.h
 *	  Exports from postmaster/postmaster.c.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/postmaster/postmaster.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef _POSTMASTER_H
#define _POSTMASTER_H

/* GUC options */
extern bool EnableSSL;
extern int	ReservedBackends;
extern PGDLLIMPORT int PostPortNumber;
extern int	Unix_socket_permissions;
extern char *Unix_socket_group;
extern char *Unix_socket_directories;
extern char *ListenAddresses;
extern bool ClientAuthInProgress;
extern int	PreAuthDelay;
extern int	AuthenticationTimeout;
extern bool Log_connections;
extern bool log_hostname;
extern bool enable_bonjour;
extern char *bonjour_name;
extern bool restart_after_crash;

#ifdef WIN32
extern HANDLE PostmasterHandle;
#else
extern int	postmaster_alive_fds[2];

/*
 * Constants that represent which of postmaster_alive_fds is held by
 * postmaster, and which is used in children to check for postmaster death.
 */
#define POSTMASTER_FD_WATCH		0	/* used in children to check for
									 * postmaster death */
#define POSTMASTER_FD_OWN		1	/* kept open by postmaster only */
#endif

extern PGDLLIMPORT const char *progname;

extern void PostmasterMain(int argc, char *argv[]) pg_attribute_noreturn();
extern void ClosePostmasterPorts(bool am_syslogger);
extern void InitProcessGlobals(void);

extern int	MaxLivePostmasterChildren(void);

extern bool PostmasterMarkPIDForWorkerNotify(int);

#ifdef EXEC_BACKEND
extern pid_t postmaster_forkexec(int argc, char *argv[]);
extern void SubPostmasterMain(int argc, char *argv[]) pg_attribute_noreturn();

extern Size ShmemBackendArraySize(void);
extern void ShmemBackendArrayAllocation(void);
#endif

/*
 * Note: MAX_BACKENDS is limited to 2^18-1 because that's the width reserved
 * for buffer references in buf_internals.h.  This limitation could be lifted
 * by using a 64bit state; but it's unlikely to be worthwhile as 2^18-1
 * backends exceed currently realistic configurations. Even if that limitation
 * were removed, we still could not a) exceed 2^23-1 because inval.c stores
 * the backend ID as a 3-byte signed integer, b) INT_MAX/4 because some places
 * compute 4*MaxBackends without any overflow check.  This is rechecked in the
 * relevant GUC check hooks and in RegisterBackgroundWorker().
 */
#define MAX_BACKENDS	0x3FFFF

#endif							/* _POSTMASTER_H */
