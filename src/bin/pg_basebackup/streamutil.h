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
 * streamutil.h
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *		  src/bin/pg_basebackup/streamutil.h
 *-------------------------------------------------------------------------
 */

#ifndef STREAMUTIL_H
#define STREAMUTIL_H

#include "access/xlogdefs.h"
#include "datatype/timestamp.h"
#include "libpq-fe.h"

extern const char *progname;
extern char *connection_string;
extern char *dbhost;
extern char *dbuser;
extern char *dbport;
extern char *dbname;
extern int	dbgetpassword;
extern uint32 WalSegSz;

/* Connection kept global so we can disconnect easily */
extern PGconn *conn;

extern PGconn *GetConnection(void);

/* Replication commands */
extern bool CreateReplicationSlot(PGconn *conn, const char *slot_name,
								  const char *plugin, bool is_temporary,
								  bool is_physical, bool reserve_wal,
								  bool slot_exists_ok);
extern bool DropReplicationSlot(PGconn *conn, const char *slot_name);
extern bool RunIdentifySystem(PGconn *conn, char **sysid,
							  TimeLineID *starttli,
							  XLogRecPtr *startpos,
							  char **db_name);
extern bool RetrieveWalSegSize(PGconn *conn);
extern TimestampTz feGetCurrentTimestamp(void);
extern void feTimestampDifference(TimestampTz start_time, TimestampTz stop_time,
								  long *secs, int *microsecs);

extern bool feTimestampDifferenceExceeds(TimestampTz start_time, TimestampTz stop_time,
										 int msec);
extern void fe_sendint64(int64 i, char *buf);
extern int64 fe_recvint64(char *buf);

#endif							/* STREAMUTIL_H */
