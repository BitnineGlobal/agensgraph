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
 * pg_lsn.h
 *		Declarations for operations on log sequence numbers (LSNs) of
 *		PostgreSQL.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/pg_lsn.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_LSN_H
#define PG_LSN_H

#include "access/xlogdefs.h"
#include "fmgr.h"

#define DatumGetLSN(X) ((XLogRecPtr) DatumGetInt64(X))
#define LSNGetDatum(X) (Int64GetDatum((int64) (X)))

#define PG_GETARG_LSN(n)	 DatumGetLSN(PG_GETARG_DATUM(n))
#define PG_RETURN_LSN(x)	 return LSNGetDatum(x)

extern XLogRecPtr pg_lsn_in_internal(const char *str, bool *have_error);

#endif							/* PG_LSN_H */
