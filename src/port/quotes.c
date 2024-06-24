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
 * quotes.c
 *	  string quoting and escaping functions
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/port/quotes.c
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"

/*
 * Escape (by doubling) any single quotes or backslashes in given string
 *
 * Note: this is used to process postgresql.conf entries and to quote
 * string literals in pg_basebackup for writing the recovery configuration.
 * Since postgresql.conf strings are defined to treat backslashes as escapes,
 * we have to double backslashes here.
 *
 * Since this function is only used for parsing or creating configuration
 * files, we do not care about encoding considerations.
 *
 * Returns a malloced() string that it's the responsibility of the caller
 * to free.
 */
char *
escape_single_quotes_ascii(const char *src)
{
	int			len = strlen(src),
				i,
				j;
	char	   *result = malloc(len * 2 + 1);

	if (!result)
		return NULL;

	for (i = 0, j = 0; i < len; i++)
	{
		if (SQL_STR_DOUBLE(src[i], true))
			result[j++] = src[i];
		result[j++] = src[i];
	}
	result[j] = '\0';
	return result;
}
