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
 * setenv.c
 *	  setenv() emulation for machines without it
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/port/setenv.c
 *
 *-------------------------------------------------------------------------
 */

#include "c.h"


int
setenv(const char *name, const char *value, int overwrite)
{
	char	   *envstr;

	/* Error conditions, per POSIX */
	if (name == NULL || name[0] == '\0' || strchr(name, '=') != NULL ||
		value == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	/* No work if variable exists and we're not to replace it */
	if (overwrite == 0 && getenv(name) != NULL)
		return 0;

	/*
	 * Add or replace the value using putenv().  This will leak memory if the
	 * same variable is repeatedly redefined, but there's little we can do
	 * about that when sitting atop putenv().
	 */
	envstr = (char *) malloc(strlen(name) + strlen(value) + 2);
	if (!envstr)				/* not much we can do if no memory */
		return -1;

	sprintf(envstr, "%s=%s", name, value);

	return putenv(envstr);
}
