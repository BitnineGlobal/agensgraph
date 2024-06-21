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

/*
 * Postgres files that use getopt(3) always include this file.
 * We must cope with three different scenarios:
 * 1. We're using the platform's getopt(), and we should just import the
 *    appropriate declarations.
 * 2. The platform lacks getopt(), and we must declare everything.
 * 3. The platform has getopt(), but we're not using it because we don't
 *    like its behavior.  The declarations we make here must be compatible
 *    with both the platform's getopt() and our src/port/getopt.c.
 *
 * Portions Copyright (c) 1987, 1993, 1994
 * The Regents of the University of California.  All rights reserved.
 *
 * Portions Copyright (c) 2003-2021, PostgreSQL Global Development Group
 *
 * src/include/pg_getopt.h
 */
#ifndef PG_GETOPT_H
#define PG_GETOPT_H

/* POSIX says getopt() is provided by unistd.h */
#include <unistd.h>

/* rely on the system's getopt.h if present */
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

/*
 * If we have <getopt.h>, assume it declares these variables, else do that
 * ourselves.  (We used to just declare them unconditionally, but Cygwin
 * doesn't like that.)
 */
#ifndef HAVE_GETOPT_H

extern char *optarg;
extern int	optind;
extern int	opterr;
extern int	optopt;

#endif							/* HAVE_GETOPT_H */

/*
 * Some platforms have optreset but fail to declare it in <getopt.h>, so cope.
 * Cygwin, however, doesn't like this either.
 */
#if defined(HAVE_INT_OPTRESET) && !defined(__CYGWIN__)
extern int	optreset;
#endif

/* Provide getopt() declaration if the platform doesn't have it */
#ifndef HAVE_GETOPT
extern int	getopt(int nargc, char *const *nargv, const char *ostr);
#endif

#endif							/* PG_GETOPT_H */
