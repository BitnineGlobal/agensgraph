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
 * psql - the PostgreSQL interactive terminal
 *
 * Copyright (c) 2000-2020, PostgreSQL Global Development Group
 *
 * src/bin/psql/input.h
 */
#ifndef INPUT_H
#define INPUT_H

/*
 * If some other file needs to have access to readline/history, include this
 * file and save yourself all this work.
 *
 * USE_READLINE is what to conditionalize readline-dependent code on.
 */
#ifdef HAVE_LIBREADLINE
#define USE_READLINE 1

#if defined(HAVE_READLINE_READLINE_H)
#include <readline/readline.h>
#if defined(HAVE_READLINE_HISTORY_H)
#include <readline/history.h>
#endif
#elif defined(HAVE_EDITLINE_READLINE_H)
#include <editline/readline.h>
#if defined(HAVE_EDITLINE_HISTORY_H)
#include <editline/history.h>
#endif
#elif defined(HAVE_READLINE_H)
#include <readline.h>
#if defined(HAVE_HISTORY_H)
#include <history.h>
#endif
#endif							/* HAVE_READLINE_READLINE_H, etc */
#endif							/* HAVE_LIBREADLINE */

#include "pqexpbuffer.h"


extern char *gets_interactive(const char *prompt, PQExpBuffer query_buf);
extern char *gets_fromFile(FILE *source);

extern void initializeInput(int flags);

extern bool printHistory(const char *fname, unsigned short int pager);

extern void pg_append_history(const char *s, PQExpBuffer history_buf);
extern void pg_send_history(PQExpBuffer history_buf);

#endif							/* INPUT_H */
