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
 * receivelog.h
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *		  src/bin/pg_basebackup/receivelog.h
 *-------------------------------------------------------------------------
 */

#ifndef RECEIVELOG_H
#define RECEIVELOG_H

#include "access/xlogdefs.h"
#include "libpq-fe.h"
#include "walmethods.h"

/*
 * Called before trying to read more data or when a segment is
 * finished. Return true to stop streaming.
 */
typedef bool (*stream_stop_callback) (XLogRecPtr segendpos, uint32 timeline, bool segment_finished);

/*
 * Global parameters when receiving xlog stream. For details about the individual fields,
 * see the function comment for ReceiveXlogStream().
 */
typedef struct StreamCtl
{
	XLogRecPtr	startpos;		/* Start position for streaming */
	TimeLineID	timeline;		/* Timeline to stream data from */
	char	   *sysidentifier;	/* Validate this system identifier and
								 * timeline */
	int			standby_message_timeout;	/* Send status messages this often */
	bool		synchronous;	/* Flush immediately WAL data on write */
	bool		mark_done;		/* Mark segment as done in generated archive */
	bool		do_sync;		/* Flush to disk to ensure consistent state of
								 * data */

	stream_stop_callback stream_stop;	/* Stop streaming when returns true */

	pgsocket	stop_socket;	/* if valid, watch for input on this socket
								 * and check stream_stop() when there is any */

	WalWriteMethod *walmethod;	/* How to write the WAL */
	char	   *partial_suffix; /* Suffix appended to partially received files */
	char	   *replication_slot;	/* Replication slot to use, or NULL */
} StreamCtl;



extern bool CheckServerVersionForStreaming(PGconn *conn);
extern bool ReceiveXlogStream(PGconn *conn,
							  StreamCtl *stream);

#endif							/* RECEIVELOG_H */
