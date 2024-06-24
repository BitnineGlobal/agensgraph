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
 * fe-gssapi-common.h
 *
 *      Definitions for GSSAPI common routines
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/interfaces/libpq/fe-gssapi-common.h
 */

#ifndef FE_GSSAPI_COMMON_H
#define FE_GSSAPI_COMMON_H

#include "libpq-fe.h"
#include "libpq-int.h"

#ifdef ENABLE_GSS

void		pg_GSS_error(const char *mprefix, PGconn *conn,
						 OM_uint32 maj_stat, OM_uint32 min_stat);
bool		pg_GSS_have_cred_cache(gss_cred_id_t *cred_out);
int			pg_GSS_load_servicename(PGconn *conn);

#endif

#endif							/* FE_GSSAPI_COMMON_H */
