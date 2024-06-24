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
 * deparse_utility.h
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/tcop/deparse_utility.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef DEPARSE_UTILITY_H
#define DEPARSE_UTILITY_H

#include "access/attnum.h"
#include "catalog/objectaddress.h"
#include "nodes/nodes.h"
#include "utils/aclchk_internal.h"


/*
 * Support for keeping track of collected commands.
 */
typedef enum CollectedCommandType
{
	SCT_Simple,
	SCT_AlterTable,
	SCT_Grant,
	SCT_AlterOpFamily,
	SCT_AlterDefaultPrivileges,
	SCT_CreateOpClass,
	SCT_AlterTSConfig
} CollectedCommandType;

/*
 * For ALTER TABLE commands, we keep a list of the subcommands therein.
 */
typedef struct CollectedATSubcmd
{
	ObjectAddress address;		/* affected column, constraint, index, ... */
	Node	   *parsetree;
} CollectedATSubcmd;

typedef struct CollectedCommand
{
	CollectedCommandType type;

	bool		in_extension;
	Node	   *parsetree;

	union
	{
		/* most commands */
		struct
		{
			ObjectAddress address;
			ObjectAddress secondaryObject;
		}			simple;

		/* ALTER TABLE, and internal uses thereof */
		struct
		{
			Oid			objectId;
			Oid			classId;
			List	   *subcmds;
		}			alterTable;

		/* GRANT / REVOKE */
		struct
		{
			InternalGrant *istmt;
		}			grant;

		/* ALTER OPERATOR FAMILY */
		struct
		{
			ObjectAddress address;
			List	   *operators;
			List	   *procedures;
		}			opfam;

		/* CREATE OPERATOR CLASS */
		struct
		{
			ObjectAddress address;
			List	   *operators;
			List	   *procedures;
		}			createopc;

		/* ALTER TEXT SEARCH CONFIGURATION ADD/ALTER/DROP MAPPING */
		struct
		{
			ObjectAddress address;
			Oid		   *dictIds;
			int			ndicts;
		}			atscfg;

		/* ALTER DEFAULT PRIVILEGES */
		struct
		{
			ObjectType	objtype;
		}			defprivs;
	}			d;

	struct CollectedCommand *parent;	/* when nested */
} CollectedCommand;

#endif							/* DEPARSE_UTILITY_H */
