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
 * pg_ts_dict.h
 *	  definition of the "text search dictionary" system catalog (pg_ts_dict)
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_ts_dict.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_TS_DICT_H
#define PG_TS_DICT_H

#include "catalog/genbki.h"
#include "catalog/pg_ts_dict_d.h"

/* ----------------
 *		pg_ts_dict definition.  cpp turns this into
 *		typedef struct FormData_pg_ts_dict
 * ----------------
 */
CATALOG(pg_ts_dict,3600,TSDictionaryRelationId)
{
	/* oid */
	Oid			oid;

	/* dictionary name */
	NameData	dictname;

	/* name space */
	Oid			dictnamespace BKI_DEFAULT(pg_catalog) BKI_LOOKUP(pg_namespace);

	/* owner */
	Oid			dictowner BKI_DEFAULT(POSTGRES) BKI_LOOKUP(pg_authid);

	/* dictionary's template */
	Oid			dicttemplate BKI_LOOKUP(pg_ts_template);

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	/* options passed to dict_init() */
	text		dictinitoption;
#endif
} FormData_pg_ts_dict;

typedef FormData_pg_ts_dict *Form_pg_ts_dict;

DECLARE_TOAST(pg_ts_dict, 4169, 4170);

DECLARE_UNIQUE_INDEX(pg_ts_dict_dictname_index, 3604, on pg_ts_dict using btree(dictname name_ops, dictnamespace oid_ops));
#define TSDictionaryNameNspIndexId	3604
DECLARE_UNIQUE_INDEX_PKEY(pg_ts_dict_oid_index, 3605, on pg_ts_dict using btree(oid oid_ops));
#define TSDictionaryOidIndexId	3605

#endif							/* PG_TS_DICT_H */
