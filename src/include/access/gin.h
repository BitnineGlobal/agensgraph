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

/*--------------------------------------------------------------------------
 * gin.h
 *	  Public header file for Generalized Inverted Index access method.
 *
 *	Copyright (c) 2006-2020, PostgreSQL Global Development Group
 *
 *	src/include/access/gin.h
 *--------------------------------------------------------------------------
 */
#ifndef GIN_H
#define GIN_H

#include "access/xlogreader.h"
#include "lib/stringinfo.h"
#include "storage/block.h"
#include "utils/relcache.h"


/*
 * amproc indexes for inverted indexes.
 */
#define GIN_COMPARE_PROC			   1
#define GIN_EXTRACTVALUE_PROC		   2
#define GIN_EXTRACTQUERY_PROC		   3
#define GIN_CONSISTENT_PROC			   4
#define GIN_COMPARE_PARTIAL_PROC	   5
#define GIN_TRICONSISTENT_PROC		   6
#define GIN_OPTIONS_PROC	   7
#define GINNProcs					   7

/*
 * searchMode settings for extractQueryFn.
 */
#define GIN_SEARCH_MODE_DEFAULT			0
#define GIN_SEARCH_MODE_INCLUDE_EMPTY	1
#define GIN_SEARCH_MODE_ALL				2
#define GIN_SEARCH_MODE_EVERYTHING		3	/* for internal use only */

/*
 * GinStatsData represents stats data for planner use
 */
typedef struct GinStatsData
{
	BlockNumber nPendingPages;
	BlockNumber nTotalPages;
	BlockNumber nEntryPages;
	BlockNumber nDataPages;
	int64		nEntries;
	int32		ginVersion;
} GinStatsData;

/*
 * A ternary value used by tri-consistent functions.
 *
 * This must be of the same size as a bool because some code will cast a
 * pointer to a bool to a pointer to a GinTernaryValue.
 */
typedef char GinTernaryValue;

#define GIN_FALSE		0		/* item is not present / does not match */
#define GIN_TRUE		1		/* item is present / matches */
#define GIN_MAYBE		2		/* don't know if item is present / don't know
								 * if matches */

#define DatumGetGinTernaryValue(X) ((GinTernaryValue)(X))
#define GinTernaryValueGetDatum(X) ((Datum)(X))
#define PG_RETURN_GIN_TERNARY_VALUE(x) return GinTernaryValueGetDatum(x)

/* GUC parameters */
extern PGDLLIMPORT int GinFuzzySearchLimit;
extern int	gin_pending_list_limit;

/* ginutil.c */
extern void ginGetStats(Relation index, GinStatsData *stats);
extern void ginUpdateStats(Relation index, const GinStatsData *stats,
						   bool is_build);

#endif							/* GIN_H */
