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
 * session.h
 *	  Encapsulation of user session.
 *
 * Copyright (c) 2017-2021, PostgreSQL Global Development Group
 *
 * src/include/access/session.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SESSION_H
#define SESSION_H

#include "lib/dshash.h"

/* Avoid including typcache.h */
struct SharedRecordTypmodRegistry;

/*
 * A struct encapsulating some elements of a user's session.  For now this
 * manages state that applies to parallel query, but in principle it could
 * include other things that are currently global variables.
 */
typedef struct Session
{
	dsm_segment *segment;		/* The session-scoped DSM segment. */
	dsa_area   *area;			/* The session-scoped DSA area. */

	/* State managed by typcache.c. */
	struct SharedRecordTypmodRegistry *shared_typmod_registry;
	dshash_table *shared_record_table;
	dshash_table *shared_typmod_table;
} Session;

extern void InitializeSession(void);
extern dsm_handle GetSessionDsmHandle(void);
extern void AttachSession(dsm_handle handle);
extern void DetachSession(void);

/* The current session, or NULL for none. */
extern Session *CurrentSession;

#endif							/* SESSION_H */
