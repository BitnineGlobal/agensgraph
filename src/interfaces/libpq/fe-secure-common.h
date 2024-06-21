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
 * fe-secure-common.h
 *
 * common implementation-independent SSL support code
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/interfaces/libpq/fe-secure-common.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef FE_SECURE_COMMON_H
#define FE_SECURE_COMMON_H

#include "libpq-fe.h"

extern int	pq_verify_peer_name_matches_certificate_name(PGconn *conn,
														 const char *namedata, size_t namelen,
														 char **store_name);
extern bool pq_verify_peer_name_matches_certificate(PGconn *conn);

#endif							/* FE_SECURE_COMMON_H */
