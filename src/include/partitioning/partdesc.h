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
 * partdesc.h
 *
 * Copyright (c) 1996-2021, PostgreSQL Global Development Group
 *
 * src/include/partitioning/partdesc.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef PARTDESC_H
#define PARTDESC_H

#include "partitioning/partdefs.h"
#include "utils/relcache.h"

/*
 * Information about partitions of a partitioned table.
 *
 * For partitioned tables where detached partitions exist, we only cache
 * descriptors that include all partitions, including detached; when we're
 * requested a descriptor without the detached partitions, we create one
 * afresh each time.  (The reason for this is that the set of detached
 * partitions that are visible to each caller depends on the snapshot it has,
 * so it's pretty much impossible to evict a descriptor from cache at the
 * right time.)
 */
typedef struct PartitionDescData
{
	int			nparts;			/* Number of partitions */
	bool		detached_exist; /* Are there any detached partitions? */
	Oid		   *oids;			/* Array of 'nparts' elements containing
								 * partition OIDs in order of the their bounds */
	bool	   *is_leaf;		/* Array of 'nparts' elements storing whether
								 * the corresponding 'oids' element belongs to
								 * a leaf partition or not */
	PartitionBoundInfo boundinfo;	/* collection of partition bounds */
} PartitionDescData;


extern PartitionDesc RelationGetPartitionDesc(Relation rel, bool omit_detached);

extern PartitionDirectory CreatePartitionDirectory(MemoryContext mcxt, bool omit_detached);
extern PartitionDesc PartitionDirectoryLookup(PartitionDirectory, Relation);
extern void DestroyPartitionDirectory(PartitionDirectory pdir);

extern Oid	get_default_oid_from_partdesc(PartitionDesc partdesc);

#endif							/* PARTCACHE_H */
