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
 * sync.h
 *	  File synchronization management code.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/sync.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SYNC_H
#define SYNC_H

#include "storage/relfilenode.h"

/*
 * Type of sync request.  These are used to manage the set of pending
 * requests to call a sync handler's sync or unlink functions at the next
 * checkpoint.
 */
typedef enum SyncRequestType
{
	SYNC_REQUEST,				/* schedule a call of sync function */
	SYNC_UNLINK_REQUEST,		/* schedule a call of unlink function */
	SYNC_FORGET_REQUEST,		/* forget all calls for a tag */
	SYNC_FILTER_REQUEST			/* forget all calls satisfying match fn */
} SyncRequestType;

/*
 * Which set of functions to use to handle a given request.  The values of
 * the enumerators must match the indexes of the function table in sync.c.
 */
typedef enum SyncRequestHandler
{
	SYNC_HANDLER_MD = 0			/* md smgr */
} SyncRequestHandler;

/*
 * A tag identifying a file.  Currently it has the members required for md.c's
 * usage, but sync.c has no knowledge of the internal structure, and it is
 * liable to change as required by future handlers.
 */
typedef struct FileTag
{
	int16		handler;		/* SyncRequestHandler value, saving space */
	int16		forknum;		/* ForkNumber, saving space */
	RelFileNode rnode;
	uint32		segno;
} FileTag;

extern void InitSync(void);
extern void SyncPreCheckpoint(void);
extern void SyncPostCheckpoint(void);
extern void ProcessSyncRequests(void);
extern void RememberSyncRequest(const FileTag *ftag, SyncRequestType type);
extern void EnableSyncRequestForwarding(void);
extern bool RegisterSyncRequest(const FileTag *ftag, SyncRequestType type,
								bool retryOnError);

#endif							/* SYNC_H */
