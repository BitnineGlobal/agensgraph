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
 * index_selfuncs.h
 *	  Index cost estimation functions for standard index access methods.
 *
 *
 * Note: this is split out of selfuncs.h mainly to avoid importing all of the
 * planner's data structures into the non-planner parts of the index AMs.
 * If you make it depend on anything besides access/amapi.h, that's likely
 * a mistake.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/index_selfuncs.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef INDEX_SELFUNCS_H
#define INDEX_SELFUNCS_H

#include "access/amapi.h"

/* Functions in selfuncs.c */
extern void brincostestimate(struct PlannerInfo *root,
							 struct IndexPath *path,
							 double loop_count,
							 Cost *indexStartupCost,
							 Cost *indexTotalCost,
							 Selectivity *indexSelectivity,
							 double *indexCorrelation,
							 double *indexPages);
extern void btcostestimate(struct PlannerInfo *root,
						   struct IndexPath *path,
						   double loop_count,
						   Cost *indexStartupCost,
						   Cost *indexTotalCost,
						   Selectivity *indexSelectivity,
						   double *indexCorrelation,
						   double *indexPages);
extern void hashcostestimate(struct PlannerInfo *root,
							 struct IndexPath *path,
							 double loop_count,
							 Cost *indexStartupCost,
							 Cost *indexTotalCost,
							 Selectivity *indexSelectivity,
							 double *indexCorrelation,
							 double *indexPages);
extern void gistcostestimate(struct PlannerInfo *root,
							 struct IndexPath *path,
							 double loop_count,
							 Cost *indexStartupCost,
							 Cost *indexTotalCost,
							 Selectivity *indexSelectivity,
							 double *indexCorrelation,
							 double *indexPages);
extern void spgcostestimate(struct PlannerInfo *root,
							struct IndexPath *path,
							double loop_count,
							Cost *indexStartupCost,
							Cost *indexTotalCost,
							Selectivity *indexSelectivity,
							double *indexCorrelation,
							double *indexPages);
extern void gincostestimate(struct PlannerInfo *root,
							struct IndexPath *path,
							double loop_count,
							Cost *indexStartupCost,
							Cost *indexTotalCost,
							Selectivity *indexSelectivity,
							double *indexCorrelation,
							double *indexPages);

#endif							/* INDEX_SELFUNCS_H */
