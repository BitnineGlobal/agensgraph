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
 * Portions Copyright (c) 1987, 1993, 1994
 * The Regents of the University of California.  All rights reserved.
 *
 * Portions Copyright (c) 2003-2020, PostgreSQL Global Development Group
 *
 * src/include/getopt_long.h
 */
#ifndef GETOPT_LONG_H
#define GETOPT_LONG_H

#include "pg_getopt.h"

#ifndef HAVE_STRUCT_OPTION

struct option
{
	const char *name;
	int			has_arg;
	int		   *flag;
	int			val;
};

#define no_argument 0
#define required_argument 1
#define optional_argument 2
#endif

#ifndef HAVE_GETOPT_LONG

extern int	getopt_long(int argc, char *const argv[],
						const char *optstring,
						const struct option *longopts, int *longindex);
#endif

#endif							/* GETOPT_LONG_H */
