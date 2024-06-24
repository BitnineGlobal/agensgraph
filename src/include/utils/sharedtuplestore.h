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
 * sharedtuplestore.h
 *	  Simple mechanism for sharing tuples between backends.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/sharedtuplestore.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SHAREDTUPLESTORE_H
#define SHAREDTUPLESTORE_H

#include "access/htup.h"
#include "storage/fd.h"
#include "storage/sharedfileset.h"

struct SharedTuplestore;
typedef struct SharedTuplestore SharedTuplestore;

struct SharedTuplestoreAccessor;
typedef struct SharedTuplestoreAccessor SharedTuplestoreAccessor;

/*
 * A flag indicating that the tuplestore will only be scanned once, so backing
 * files can be unlinked early.
 */
#define SHARED_TUPLESTORE_SINGLE_PASS 0x01

extern size_t sts_estimate(int participants);

extern SharedTuplestoreAccessor *sts_initialize(SharedTuplestore *sts,
												int participants,
												int my_participant_number,
												size_t meta_data_size,
												int flags,
												SharedFileSet *fileset,
												const char *name);

extern SharedTuplestoreAccessor *sts_attach(SharedTuplestore *sts,
											int my_participant_number,
											SharedFileSet *fileset);

extern void sts_end_write(SharedTuplestoreAccessor *accessor);

extern void sts_reinitialize(SharedTuplestoreAccessor *accessor);

extern void sts_begin_parallel_scan(SharedTuplestoreAccessor *accessor);

extern void sts_end_parallel_scan(SharedTuplestoreAccessor *accessor);

extern void sts_puttuple(SharedTuplestoreAccessor *accessor,
						 void *meta_data,
						 MinimalTuple tuple);

extern MinimalTuple sts_parallel_scan_next(SharedTuplestoreAccessor *accessor,
										   void *meta_data);

#endif							/* SHAREDTUPLESTORE_H */
