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
 * sharedfileset.h
 *	  Shared temporary file management.
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/storage/sharedfileset.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef SHAREDFILESET_H
#define SHAREDFILESET_H

#include "storage/dsm.h"
#include "storage/fd.h"
#include "storage/spin.h"

/*
 * A set of temporary files that can be shared by multiple backends.
 */
typedef struct SharedFileSet
{
	pid_t		creator_pid;	/* PID of the creating process */
	uint32		number;			/* per-PID identifier */
	slock_t		mutex;			/* mutex protecting the reference count */
	int			refcnt;			/* number of attached backends */
	int			ntablespaces;	/* number of tablespaces to use */
	Oid			tablespaces[8]; /* OIDs of tablespaces to use. Assumes that
								 * it's rare that there more than temp
								 * tablespaces. */
} SharedFileSet;

extern void SharedFileSetInit(SharedFileSet *fileset, dsm_segment *seg);
extern void SharedFileSetAttach(SharedFileSet *fileset, dsm_segment *seg);
extern File SharedFileSetCreate(SharedFileSet *fileset, const char *name);
extern File SharedFileSetOpen(SharedFileSet *fileset, const char *name,
							  int mode);
extern bool SharedFileSetDelete(SharedFileSet *fileset, const char *name,
								bool error_on_failure);
extern void SharedFileSetDeleteAll(SharedFileSet *fileset);
extern void SharedFileSetUnregister(SharedFileSet *input_fileset);

#endif
