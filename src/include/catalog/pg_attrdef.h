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
 * pg_attrdef.h
 *	  definition of the "attribute defaults" system catalog (pg_attrdef)
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_attrdef.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_ATTRDEF_H
#define PG_ATTRDEF_H

#include "catalog/genbki.h"
#include "catalog/pg_attrdef_d.h"

/* ----------------
 *		pg_attrdef definition.  cpp turns this into
 *		typedef struct FormData_pg_attrdef
 * ----------------
 */
CATALOG(pg_attrdef,2604,AttrDefaultRelationId)
{
	Oid			oid;			/* oid */

	Oid			adrelid;		/* OID of table containing attribute */
	int16		adnum;			/* attnum of attribute */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	pg_node_tree adbin BKI_FORCE_NOT_NULL;	/* nodeToString representation of
											 * default */
#endif
} FormData_pg_attrdef;

/* ----------------
 *		Form_pg_attrdef corresponds to a pointer to a tuple with
 *		the format of pg_attrdef relation.
 * ----------------
 */
typedef FormData_pg_attrdef *Form_pg_attrdef;

#endif							/* PG_ATTRDEF_H */
