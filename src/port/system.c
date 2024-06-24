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
 * system.c
 *	   Win32 system() and popen() replacements
 *
 *
 *	Win32 needs double quotes at the beginning and end of system()
 *	strings.  If not, it gets confused with multiple quoted strings.
 *	It also requires double-quotes around the executable name and
 *	any files used for redirection.  Filter other args through
 *	appendShellString() to quote them.
 *
 *	Generated using Win32 "CMD /?":
 *
 *	1. If all of the following conditions are met, then quote characters
 *	on the command line are preserved:
 *
 *	 - no /S switch
 *	 - exactly two quote characters
 *	 - no special characters between the two quote characters, where special
 *	   is one of: &<>()@^|
 *	 - there are one or more whitespace characters between the two quote
 *	   characters
 *	 - the string between the two quote characters is the name of an
 *	   executable file.
 *
 *	 2. Otherwise, old behavior is to see if the first character is a quote
 *	 character and if so, strip the leading character and remove the last
 *	 quote character on the command line, preserving any text after the last
 *	 quote character.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 *
 * src/port/system.c
 *
 *-------------------------------------------------------------------------
 */

#if defined(WIN32) && !defined(__CYGWIN__)

#ifndef FRONTEND
#include "postgres.h"
#else
#include "postgres_fe.h"
#endif

#include <fcntl.h>

#undef system
#undef popen

int
pgwin32_system(const char *command)
{
	size_t		cmdlen = strlen(command);
	char	   *buf;
	int			save_errno;
	int			res;

	/*
	 * Create a malloc'd copy of the command string, enclosed with an extra
	 * pair of quotes
	 */
	buf = malloc(cmdlen + 2 + 1);
	if (buf == NULL)
	{
		errno = ENOMEM;
		return -1;
	}
	buf[0] = '"';
	memcpy(&buf[1], command, cmdlen);
	buf[cmdlen + 1] = '"';
	buf[cmdlen + 2] = '\0';

	res = system(buf);

	save_errno = errno;
	free(buf);
	errno = save_errno;

	return res;
}


FILE *
pgwin32_popen(const char *command, const char *type)
{
	size_t		cmdlen = strlen(command);
	char	   *buf;
	int			save_errno;
	FILE	   *res;

	/*
	 * Create a malloc'd copy of the command string, enclosed with an extra
	 * pair of quotes
	 */
	buf = malloc(cmdlen + 2 + 1);
	if (buf == NULL)
	{
		errno = ENOMEM;
		return NULL;
	}
	buf[0] = '"';
	memcpy(&buf[1], command, cmdlen);
	buf[cmdlen + 1] = '"';
	buf[cmdlen + 2] = '\0';

	res = _popen(buf, type);

	save_errno = errno;
	free(buf);
	errno = save_errno;

	return res;
}

#endif
