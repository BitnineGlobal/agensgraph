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
 * pg_range.h
 *	  definition of the "range type" system catalog (pg_range)
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_range.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_RANGE_H
#define PG_RANGE_H

#include "catalog/genbki.h"
#include "catalog/pg_range_d.h"

/* ----------------
 *		pg_range definition.  cpp turns this into
 *		typedef struct FormData_pg_range
 * ----------------
 */
CATALOG(pg_range,3541,RangeRelationId)
{
	/* OID of owning range type */
	Oid			rngtypid BKI_LOOKUP(pg_type);

	/* OID of range's element type (subtype) */
	Oid			rngsubtype BKI_LOOKUP(pg_type);

	/* collation for this range type, or 0 */
	Oid			rngcollation BKI_DEFAULT(0);

	/* subtype's btree opclass */
	Oid			rngsubopc BKI_LOOKUP(pg_opclass);

	/* canonicalize range, or 0 */
	regproc		rngcanonical BKI_LOOKUP(pg_proc);

	/* subtype difference as a float8, or 0 */
	regproc		rngsubdiff BKI_LOOKUP(pg_proc);
} FormData_pg_range;

/* ----------------
 *		Form_pg_range corresponds to a pointer to a tuple with
 *		the format of pg_range relation.
 * ----------------
 */
typedef FormData_pg_range *Form_pg_range;

/*
 * prototypes for functions in pg_range.c
 */

extern void RangeCreate(Oid rangeTypeOid, Oid rangeSubType, Oid rangeCollation,
						Oid rangeSubOpclass, RegProcedure rangeCanonical,
						RegProcedure rangeSubDiff);
extern void RangeDelete(Oid rangeTypeOid);

#endif							/* PG_RANGE_H */
