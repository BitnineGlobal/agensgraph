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
 * scram.h
 *	  Interface to libpq/scram.c
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/libpq/scram.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_SCRAM_H
#define PG_SCRAM_H

#include "lib/stringinfo.h"
#include "libpq/libpq-be.h"

/* Status codes for message exchange */
#define SASL_EXCHANGE_CONTINUE		0
#define SASL_EXCHANGE_SUCCESS		1
#define SASL_EXCHANGE_FAILURE		2

/* Routines dedicated to authentication */
extern void pg_be_scram_get_mechanisms(Port *port, StringInfo buf);
extern void *pg_be_scram_init(Port *port, const char *selected_mech, const char *shadow_pass);
extern int	pg_be_scram_exchange(void *opaq, const char *input, int inputlen,
								 char **output, int *outputlen, char **logdetail);

/* Routines to handle and check SCRAM-SHA-256 secret */
extern char *pg_be_scram_build_secret(const char *password);
extern bool parse_scram_secret(const char *secret, int *iterations, char **salt,
							   uint8 *stored_key, uint8 *server_key);
extern bool scram_verify_plain_password(const char *username,
										const char *password, const char *secret);

#endif							/* PG_SCRAM_H */
