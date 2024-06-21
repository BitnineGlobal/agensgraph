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
 * String-processing utility routines for frontend code
 *
 * Utility functions that interpret backend output or quote strings for
 * assorted contexts.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/fe_utils/string_utils.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include "libpq-fe.h"
#include "pqexpbuffer.h"

/* Global variables controlling behavior of fmtId() and fmtQualifiedId() */
extern int	quote_all_identifiers;
extern PQExpBuffer (*getLocalPQExpBuffer) (void);

/* Functions */
extern const char *fmtId(const char *identifier);
extern const char *fmtQualifiedId(const char *schema, const char *id);

extern char *formatPGVersionNumber(int version_number, bool include_minor,
								   char *buf, size_t buflen);

extern void appendStringLiteral(PQExpBuffer buf, const char *str,
								int encoding, bool std_strings);
extern void appendStringLiteralConn(PQExpBuffer buf, const char *str,
									PGconn *conn);
extern void appendStringLiteralDQ(PQExpBuffer buf, const char *str,
								  const char *dqprefix);
extern void appendByteaLiteral(PQExpBuffer buf,
							   const unsigned char *str, size_t length,
							   bool std_strings);

extern void appendShellString(PQExpBuffer buf, const char *str);
extern bool appendShellStringNoError(PQExpBuffer buf, const char *str);
extern void appendConnStrVal(PQExpBuffer buf, const char *str);
extern void appendPsqlMetaConnect(PQExpBuffer buf, const char *dbname);

extern bool parsePGArray(const char *atext, char ***itemarray, int *nitems);

extern bool appendReloptionsArray(PQExpBuffer buffer, const char *reloptions,
								  const char *prefix, int encoding, bool std_strings);

extern bool processSQLNamePattern(PGconn *conn, PQExpBuffer buf,
								  const char *pattern,
								  bool have_where, bool force_escape,
								  const char *schemavar, const char *namevar,
								  const char *altnamevar, const char *visibilityrule);

#endif							/* STRING_UTILS_H */
