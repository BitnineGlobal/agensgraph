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
 * datapagemap.h
 *
 * Copyright (c) 2013-2020, PostgreSQL Global Development Group
 *
 *-------------------------------------------------------------------------
 */
#ifndef DATAPAGEMAP_H
#define DATAPAGEMAP_H

#include "storage/block.h"
#include "storage/relfilenode.h"

struct datapagemap
{
	char	   *bitmap;
	int			bitmapsize;
};

typedef struct datapagemap datapagemap_t;
typedef struct datapagemap_iterator datapagemap_iterator_t;

extern void datapagemap_add(datapagemap_t *map, BlockNumber blkno);
extern datapagemap_iterator_t *datapagemap_iterate(datapagemap_t *map);
extern bool datapagemap_next(datapagemap_iterator_t *iter, BlockNumber *blkno);
extern void datapagemap_print(datapagemap_t *map);

#endif							/* DATAPAGEMAP_H */
