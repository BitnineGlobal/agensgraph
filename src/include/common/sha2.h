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
 * sha2.h
 *	  Constants related to SHA224, 256, 384 AND 512.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		  src/include/common/sha2.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef _PG_SHA2_H_
#define _PG_SHA2_H_

/*** SHA224/256/384/512 Various Length Definitions ***********************/
#define PG_SHA224_BLOCK_LENGTH			64
#define PG_SHA224_DIGEST_LENGTH			28
#define PG_SHA224_DIGEST_STRING_LENGTH	(PG_SHA224_DIGEST_LENGTH * 2 + 1)
#define PG_SHA256_BLOCK_LENGTH			64
#define PG_SHA256_DIGEST_LENGTH			32
#define PG_SHA256_DIGEST_STRING_LENGTH	(PG_SHA256_DIGEST_LENGTH * 2 + 1)
#define PG_SHA384_BLOCK_LENGTH			128
#define PG_SHA384_DIGEST_LENGTH			48
#define PG_SHA384_DIGEST_STRING_LENGTH	(PG_SHA384_DIGEST_LENGTH * 2 + 1)
#define PG_SHA512_BLOCK_LENGTH			128
#define PG_SHA512_DIGEST_LENGTH			64
#define PG_SHA512_DIGEST_STRING_LENGTH	(PG_SHA512_DIGEST_LENGTH * 2 + 1)

#endif							/* _PG_SHA2_H_ */
