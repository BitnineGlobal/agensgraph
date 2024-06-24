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
 * pg_regress.h --- regression test driver
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/test/regress/pg_regress.h
 *-------------------------------------------------------------------------
 */

#include <unistd.h>

#ifndef WIN32
#define PID_TYPE pid_t
#define INVALID_PID (-1)
#else
#define PID_TYPE HANDLE
#define INVALID_PID INVALID_HANDLE_VALUE
#endif

/* simple list of strings */
typedef struct _stringlist
{
	char	   *str;
	struct _stringlist *next;
} _stringlist;

typedef PID_TYPE(*test_function) (const char *,
								  _stringlist **,
								  _stringlist **,
								  _stringlist **);
typedef void (*init_function) (int argc, char **argv);

extern char *bindir;
extern char *libdir;
extern char *datadir;
extern char *host_platform;

extern _stringlist *dblist;
extern bool debug;
extern char *inputdir;
extern char *outputdir;
extern char *launcher;

extern const char *basic_diff_opts;
extern const char *pretty_diff_opts;

int			regression_main(int argc, char *argv[],
							init_function ifunc, test_function tfunc);
void		add_stringlist_item(_stringlist **listhead, const char *str);
PID_TYPE	spawn_process(const char *cmdline);
void		replace_string(char *string, const char *replace, const char *replacement);
bool		file_exists(const char *file);
