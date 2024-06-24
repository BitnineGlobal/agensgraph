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
 * header.h
 *		Replacement header file for Snowball stemmer modules
 *
 * The Snowball stemmer modules do #include "header.h", and think they
 * are including snowball/libstemmer/header.h.  We adjust the CPPFLAGS
 * so that this file is found instead, and thereby we can modify the
 * headers they see.  The main point here is to ensure that pg_config.h
 * is included before any system headers such as <stdio.h>; without that,
 * we have portability issues on some platforms due to variation in
 * largefile options across different modules in the backend.
 *
 * NOTE: this file should not be included into any non-snowball sources!
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 *
 * src/include/snowball/header.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SNOWBALL_HEADR_H
#define SNOWBALL_HEADR_H

/*
 * It's against Postgres coding conventions to include postgres.h in a
 * header file, but we allow the violation here because the alternative is
 * to modify the machine-generated .c files provided by the Snowball project.
 */
#include "postgres.h"

/* Some platforms define MAXINT and/or MININT, causing conflicts */
#ifdef MAXINT
#undef MAXINT
#endif
#ifdef MININT
#undef MININT
#endif

/* Now we can include the original Snowball header.h */
#include "snowball/libstemmer/header.h" /* pgrminclude ignore */

/*
 * Redefine standard memory allocation interface to pgsql's one.
 * This allows us to control where the Snowball code allocates stuff.
 */
#ifdef malloc
#undef malloc
#endif
#define malloc(a)		palloc(a)

#ifdef calloc
#undef calloc
#endif
#define calloc(a,b)		palloc0((a) * (b))

#ifdef realloc
#undef realloc
#endif
#define realloc(a,b)	repalloc(a,b)

#ifdef free
#undef free
#endif
#define free(a)			pfree(a)

#endif							/* SNOWBALL_HEADR_H */
