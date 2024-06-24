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
 * AM-callable functions for BRIN indexes
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		src/include/access/brin.h
 */
#ifndef BRIN_H
#define BRIN_H

#include "nodes/execnodes.h"
#include "utils/relcache.h"


/*
 * Storage type for BRIN's reloptions
 */
typedef struct BrinOptions
{
	int32		vl_len_;		/* varlena header (do not touch directly!) */
	BlockNumber pagesPerRange;
	bool		autosummarize;
} BrinOptions;


/*
 * BrinStatsData represents stats data for planner use
 */
typedef struct BrinStatsData
{
	BlockNumber pagesPerRange;
	BlockNumber revmapNumPages;
} BrinStatsData;


#define BRIN_DEFAULT_PAGES_PER_RANGE	128
#define BrinGetPagesPerRange(relation) \
	(AssertMacro(relation->rd_rel->relkind == RELKIND_INDEX && \
				 relation->rd_rel->relam == BRIN_AM_OID), \
	 (relation)->rd_options ? \
	 ((BrinOptions *) (relation)->rd_options)->pagesPerRange : \
	  BRIN_DEFAULT_PAGES_PER_RANGE)
#define BrinGetAutoSummarize(relation) \
	(AssertMacro(relation->rd_rel->relkind == RELKIND_INDEX && \
				 relation->rd_rel->relam == BRIN_AM_OID), \
	 (relation)->rd_options ? \
	 ((BrinOptions *) (relation)->rd_options)->autosummarize : \
	  false)


extern void brinGetStats(Relation index, BrinStatsData *stats);

#endif							/* BRIN_H */
