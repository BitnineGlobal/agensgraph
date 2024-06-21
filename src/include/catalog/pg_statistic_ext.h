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
 * pg_statistic_ext.h
 *	  definition of the "extended statistics" system catalog
 *	  (pg_statistic_ext)
 *
 * Note that pg_statistic_ext contains the definitions of extended statistics
 * objects, created by CREATE STATISTICS, but not the actual statistical data,
 * created by running ANALYZE.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_statistic_ext.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_STATISTIC_EXT_H
#define PG_STATISTIC_EXT_H

#include "catalog/genbki.h"
#include "catalog/pg_statistic_ext_d.h"

/* ----------------
 *		pg_statistic_ext definition.  cpp turns this into
 *		typedef struct FormData_pg_statistic_ext
 * ----------------
 */
CATALOG(pg_statistic_ext,3381,StatisticExtRelationId)
{
	Oid			oid;			/* oid */

	Oid			stxrelid;		/* relation containing attributes */

	/* These two fields form the unique key for the entry: */
	NameData	stxname;		/* statistics object name */
	Oid			stxnamespace;	/* OID of statistics object's namespace */

	Oid			stxowner;		/* statistics object's owner */
	int32		stxstattarget BKI_DEFAULT(-1);	/* statistics target */

	/*
	 * variable-length fields start here, but we allow direct access to
	 * stxkeys
	 */
	int2vector	stxkeys;		/* array of column keys */

#ifdef CATALOG_VARLEN
	char		stxkind[1] BKI_FORCE_NOT_NULL;	/* statistics kinds requested
												 * to build */
#endif

} FormData_pg_statistic_ext;

/* ----------------
 *		Form_pg_statistic_ext corresponds to a pointer to a tuple with
 *		the format of pg_statistic_ext relation.
 * ----------------
 */
typedef FormData_pg_statistic_ext *Form_pg_statistic_ext;

#ifdef EXPOSE_TO_CLIENT_CODE

#define STATS_EXT_NDISTINCT			'd'
#define STATS_EXT_DEPENDENCIES		'f'
#define STATS_EXT_MCV				'm'

#endif							/* EXPOSE_TO_CLIENT_CODE */

#endif							/* PG_STATISTIC_EXT_H */
