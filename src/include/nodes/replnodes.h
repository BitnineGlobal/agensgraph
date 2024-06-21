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
 * replnodes.h
 *	  definitions for replication grammar parse nodes
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/nodes/replnodes.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef REPLNODES_H
#define REPLNODES_H

#include "access/xlogdefs.h"
#include "nodes/pg_list.h"

typedef enum ReplicationKind
{
	REPLICATION_KIND_PHYSICAL,
	REPLICATION_KIND_LOGICAL
} ReplicationKind;


/* ----------------------
 *		IDENTIFY_SYSTEM command
 * ----------------------
 */
typedef struct IdentifySystemCmd
{
	NodeTag		type;
} IdentifySystemCmd;


/* ----------------------
 *		BASE_BACKUP command
 * ----------------------
 */
typedef struct BaseBackupCmd
{
	NodeTag		type;
	List	   *options;
} BaseBackupCmd;


/* ----------------------
 *		CREATE_REPLICATION_SLOT command
 * ----------------------
 */
typedef struct CreateReplicationSlotCmd
{
	NodeTag		type;
	char	   *slotname;
	ReplicationKind kind;
	char	   *plugin;
	bool		temporary;
	List	   *options;
} CreateReplicationSlotCmd;


/* ----------------------
 *		DROP_REPLICATION_SLOT command
 * ----------------------
 */
typedef struct DropReplicationSlotCmd
{
	NodeTag		type;
	char	   *slotname;
	bool		wait;
} DropReplicationSlotCmd;


/* ----------------------
 *		START_REPLICATION command
 * ----------------------
 */
typedef struct StartReplicationCmd
{
	NodeTag		type;
	ReplicationKind kind;
	char	   *slotname;
	TimeLineID	timeline;
	XLogRecPtr	startpoint;
	List	   *options;
} StartReplicationCmd;


/* ----------------------
 *		TIMELINE_HISTORY command
 * ----------------------
 */
typedef struct TimeLineHistoryCmd
{
	NodeTag		type;
	TimeLineID	timeline;
} TimeLineHistoryCmd;

/* ----------------------
 *		SQL commands
 * ----------------------
 */
typedef struct SQLCmd
{
	NodeTag		type;
} SQLCmd;

#endif							/* REPLNODES_H */
