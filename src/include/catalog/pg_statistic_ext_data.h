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
 * pg_statistic_ext_data.h
 *	  definition of the "extended statistics data" system catalog
 *	  (pg_statistic_ext_data)
 *
 * This catalog stores the statistical data for extended statistics objects.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_statistic_ext_data.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_STATISTIC_EXT_DATA_H
#define PG_STATISTIC_EXT_DATA_H

#include "catalog/genbki.h"
#include "catalog/pg_statistic_ext_data_d.h"

/* ----------------
 *		pg_statistic_ext_data definition.  cpp turns this into
 *		typedef struct FormData_pg_statistic_ext_data
 * ----------------
 */
CATALOG(pg_statistic_ext_data,3429,StatisticExtDataRelationId)
{
	Oid			stxoid BKI_LOOKUP(pg_statistic_ext);	/* statistics object
														 * this data is for */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */

	pg_ndistinct stxdndistinct; /* ndistinct coefficients (serialized) */
	pg_dependencies stxddependencies;	/* dependencies (serialized) */
	pg_mcv_list stxdmcv;		/* MCV (serialized) */
	pg_statistic stxdexpr[1];	/* stats for expressions */

#endif

} FormData_pg_statistic_ext_data;

/* ----------------
 *		Form_pg_statistic_ext_data corresponds to a pointer to a tuple with
 *		the format of pg_statistic_ext_data relation.
 * ----------------
 */
typedef FormData_pg_statistic_ext_data * Form_pg_statistic_ext_data;

DECLARE_TOAST(pg_statistic_ext_data, 3430, 3431);

DECLARE_UNIQUE_INDEX_PKEY(pg_statistic_ext_data_stxoid_index, 3433, on pg_statistic_ext_data using btree(stxoid oid_ops));
#define StatisticExtDataStxoidIndexId 3433

#endif							/* PG_STATISTIC_EXT_DATA_H */
