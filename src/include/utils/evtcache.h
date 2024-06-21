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
 * evtcache.h
 *	  Special-purpose cache for event trigger data.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/include/utils/evtcache.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef EVTCACHE_H
#define EVTCACHE_H

#include "nodes/bitmapset.h"
#include "nodes/pg_list.h"

typedef enum
{
	EVT_DDLCommandStart,
	EVT_DDLCommandEnd,
	EVT_SQLDrop,
	EVT_TableRewrite
} EventTriggerEvent;

typedef struct
{
	Oid			fnoid;			/* function to be called */
	char		enabled;		/* as SESSION_REPLICATION_ROLE_* */
	Bitmapset  *tagset;			/* command tags, or NULL if empty */
} EventTriggerCacheItem;

extern List *EventCacheLookup(EventTriggerEvent event);

#endif							/* EVTCACHE_H */
