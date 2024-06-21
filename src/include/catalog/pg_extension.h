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
 * pg_extension.h
 *	  definition of the "extension" system catalog (pg_extension)
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_extension.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_EXTENSION_H
#define PG_EXTENSION_H

#include "catalog/genbki.h"
#include "catalog/pg_extension_d.h"

/* ----------------
 *		pg_extension definition.  cpp turns this into
 *		typedef struct FormData_pg_extension
 * ----------------
 */
CATALOG(pg_extension,3079,ExtensionRelationId)
{
	Oid			oid;			/* oid */
	NameData	extname;		/* extension name */
	Oid			extowner BKI_LOOKUP(pg_authid); /* extension owner */
	Oid			extnamespace BKI_LOOKUP(pg_namespace);	/* namespace of
														 * contained objects */
	bool		extrelocatable; /* if true, allow ALTER EXTENSION SET SCHEMA */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	/* extversion may never be null, but the others can be. */
	text		extversion BKI_FORCE_NOT_NULL;	/* extension version name */
	Oid			extconfig[1] BKI_LOOKUP(pg_class);	/* dumpable configuration
													 * tables */
	text		extcondition[1];	/* WHERE clauses for config tables */
#endif
} FormData_pg_extension;

/* ----------------
 *		Form_pg_extension corresponds to a pointer to a tuple with
 *		the format of pg_extension relation.
 * ----------------
 */
typedef FormData_pg_extension *Form_pg_extension;

DECLARE_TOAST(pg_extension, 4147, 4148);

DECLARE_UNIQUE_INDEX_PKEY(pg_extension_oid_index, 3080, on pg_extension using btree(oid oid_ops));
#define ExtensionOidIndexId 3080
DECLARE_UNIQUE_INDEX(pg_extension_name_index, 3081, on pg_extension using btree(extname name_ops));
#define ExtensionNameIndexId 3081

#endif							/* PG_EXTENSION_H */
