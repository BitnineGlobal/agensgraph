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
 * Copyright (c) 2000-2021, PostgreSQL Global Development Group
 *
 * src/bin/psql/describe.h
 */
#ifndef DESCRIBE_H
#define DESCRIBE_H

extern bool validateSQLNamePattern(PQExpBuffer buf, const char *pattern,
								   bool have_where, bool force_escape,
								   const char *schemavar, const char *namevar,
								   const char *altnamevar,
								   const char *visibilityrule,
								   bool *added_clause, int maxparts);

/* \da */
extern bool describeAggregates(const char *pattern, bool verbose, bool showSystem);

/* \dA */
extern bool describeAccessMethods(const char *pattern, bool verbose);

/* \db */
extern bool describeTablespaces(const char *pattern, bool verbose);

/* \df, \dfa, \dfn, \dft, \dfw, etc. */
extern bool describeFunctions(const char *functypes, const char *func_pattern,
							  char **arg_patterns, int num_arg_patterns,
							  bool verbose, bool showSystem);

/* \dT */
extern bool describeTypes(const char *pattern, bool verbose, bool showSystem);

/* \do */
extern bool describeOperators(const char *oper_pattern,
							  char **arg_patterns, int num_arg_patterns,
							  bool verbose, bool showSystem);

/* \du, \dg */
extern bool describeRoles(const char *pattern, bool verbose, bool showSystem);

/* \drds */
extern bool listDbRoleSettings(const char *pattern, const char *pattern2);

/* \z (or \dp) */
extern bool permissionsList(const char *pattern);

/* \ddp */
extern bool listDefaultACLs(const char *pattern);

/* \dd */
extern bool objectDescription(const char *pattern, bool showSystem);

/* \d foo */
extern bool describeTableDetails(const char *pattern, bool verbose, bool showSystem);

/* \dF */
extern bool listTSConfigs(const char *pattern, bool verbose);

/* \dFp */
extern bool listTSParsers(const char *pattern, bool verbose);

/* \dFd */
extern bool listTSDictionaries(const char *pattern, bool verbose);

/* \dFt */
extern bool listTSTemplates(const char *pattern, bool verbose);

/* \l */
extern bool listAllDbs(const char *pattern, bool verbose);

/* \dt, \di, \ds, \dS, etc. */
extern bool listTables(const char *tabtypes, const char *pattern, bool verbose, bool showSystem);

/* \dP */
extern bool listPartitionedTables(const char *reltypes, const char *pattern, bool verbose);

/* \dD */
extern bool listDomains(const char *pattern, bool verbose, bool showSystem);

/* \dc */
extern bool listConversions(const char *pattern, bool verbose, bool showSystem);

/* \dC */
extern bool listCasts(const char *pattern, bool verbose);

/* \dO */
extern bool listCollations(const char *pattern, bool verbose, bool showSystem);

/* \dn */
extern bool listSchemas(const char *pattern, bool verbose, bool showSystem);

/* \dew */
extern bool listForeignDataWrappers(const char *pattern, bool verbose);

/* \des */
extern bool listForeignServers(const char *pattern, bool verbose);

/* \deu */
extern bool listUserMappings(const char *pattern, bool verbose);

/* \det */
extern bool listForeignTables(const char *pattern, bool verbose);

/* \dL */
extern bool listLanguages(const char *pattern, bool verbose, bool showSystem);

/* \dx */
extern bool listExtensions(const char *pattern);

/* \dx+ */
extern bool listExtensionContents(const char *pattern);

/* \dX */
extern bool listExtendedStats(const char *pattern);

/* \dy */
extern bool listEventTriggers(const char *pattern, bool verbose);

/* \dRp */
bool		listPublications(const char *pattern);

/* \dRp+ */
bool		describePublications(const char *pattern);

/* \dRs */
bool		describeSubscriptions(const char *pattern, bool verbose);

/* \dAc */
extern bool listOperatorClasses(const char *access_method_pattern,
								const char *opclass_pattern,
								bool verbose);

/* \dAf */
extern bool listOperatorFamilies(const char *access_method_pattern,
								 const char *opclass_pattern,
								 bool verbose);

/* \dAo */
extern bool listOpFamilyOperators(const char *accessMethod_pattern,
								  const char *family_pattern, bool verbose);

/* \dAp */
extern bool listOpFamilyFunctions(const char *access_method_pattern,
								  const char *family_pattern, bool verbose);

/* AgensGraph Commands */
/* \dG */
extern bool listGraphs(const char *pattern, bool verbose);

/* \dGl, \dGv, \dGe */
extern bool listLabels(const char *pattern, bool verbose, const char labkind);

#endif							/* DESCRIBE_H */
