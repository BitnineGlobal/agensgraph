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
 * extension.h
 *		Extension management commands (create/drop extension).
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/extension.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef EXTENSION_H
#define EXTENSION_H

#include "catalog/objectaddress.h"
#include "parser/parse_node.h"


/*
 * creating_extension is only true while running a CREATE EXTENSION or ALTER
 * EXTENSION UPDATE command.  It instructs recordDependencyOnCurrentExtension()
 * to register a dependency on the current pg_extension object for each SQL
 * object created by an extension script.  It also instructs performDeletion()
 * to remove such dependencies without following them, so that extension
 * scripts can drop member objects without having to explicitly dissociate
 * them from the extension first.
 */
extern PGDLLIMPORT bool creating_extension;
extern PGDLLIMPORT Oid CurrentExtensionObject;


extern ObjectAddress CreateExtension(ParseState *pstate, CreateExtensionStmt *stmt);

extern void RemoveExtensionById(Oid extId);

extern ObjectAddress InsertExtensionTuple(const char *extName, Oid extOwner,
										  Oid schemaOid, bool relocatable, const char *extVersion,
										  Datum extConfig, Datum extCondition,
										  List *requiredExtensions);

extern ObjectAddress ExecAlterExtensionStmt(ParseState *pstate, AlterExtensionStmt *stmt);

extern ObjectAddress ExecAlterExtensionContentsStmt(AlterExtensionContentsStmt *stmt,
													ObjectAddress *objAddr);

extern Oid	get_extension_oid(const char *extname, bool missing_ok);
extern char *get_extension_name(Oid ext_oid);
extern bool extension_file_exists(const char *extensionName);

extern ObjectAddress AlterExtensionNamespace(const char *extensionName, const char *newschema,
											 Oid *oldschema);

#endif							/* EXTENSION_H */
