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
 * saslprep.h
 *	  SASLprep normalization, for SCRAM authentication
 *
 * These definitions are used by both frontend and backend code.
 *
 * Copyright (c) 2017-2020, PostgreSQL Global Development Group
 *
 * src/include/common/saslprep.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SASLPREP_H
#define SASLPREP_H

/*
 * Return codes for pg_saslprep() function.
 */
typedef enum
{
	SASLPREP_SUCCESS = 0,
	SASLPREP_OOM = -1,			/* out of memory (only in frontend) */
	SASLPREP_INVALID_UTF8 = -2, /* input is not a valid UTF-8 string */
	SASLPREP_PROHIBITED = -3	/* output would contain prohibited characters */
} pg_saslprep_rc;

extern pg_saslprep_rc pg_saslprep(const char *input, char **output);

#endif							/* SASLPREP_H */
