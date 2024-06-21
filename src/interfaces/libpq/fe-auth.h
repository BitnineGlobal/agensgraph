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
 * fe-auth.h
 *
 *	  Definitions for network authentication routines
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/interfaces/libpq/fe-auth.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef FE_AUTH_H
#define FE_AUTH_H

#include "libpq-fe.h"
#include "libpq-int.h"


/* Prototypes for functions in fe-auth.c */
extern int	pg_fe_sendauth(AuthRequest areq, int payloadlen, PGconn *conn);
extern char *pg_fe_getauthname(PQExpBuffer errorMessage);

/* Prototypes for functions in fe-auth-scram.c */
extern void *pg_fe_scram_init(PGconn *conn,
							  const char *password,
							  const char *sasl_mechanism);
extern bool pg_fe_scram_channel_bound(void *opaq);
extern void pg_fe_scram_free(void *opaq);
extern void pg_fe_scram_exchange(void *opaq, char *input, int inputlen,
								 char **output, int *outputlen,
								 bool *done, bool *success);
extern char *pg_fe_scram_build_secret(const char *password);

#endif							/* FE_AUTH_H */
