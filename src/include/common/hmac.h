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
 * hmac.h
 *	  Generic headers for HMAC
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		  src/include/common/hmac.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef PG_HMAC_H
#define PG_HMAC_H

#include "common/cryptohash.h"

/* opaque context, private to each HMAC implementation */
typedef struct pg_hmac_ctx pg_hmac_ctx;

extern pg_hmac_ctx *pg_hmac_create(pg_cryptohash_type type);
extern int	pg_hmac_init(pg_hmac_ctx *ctx, const uint8 *key, size_t len);
extern int	pg_hmac_update(pg_hmac_ctx *ctx, const uint8 *data, size_t len);
extern int	pg_hmac_final(pg_hmac_ctx *ctx, uint8 *dest, size_t len);
extern void pg_hmac_free(pg_hmac_ctx *ctx);

#endif							/* PG_HMAC_H */
