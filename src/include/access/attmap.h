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
 * attmap.h
 *	  Definitions for PostgreSQL attribute mappings
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/attmap.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef ATTMAP_H
#define ATTMAP_H

#include "access/attnum.h"
#include "access/tupdesc.h"

/*
 * Attribute mapping structure
 *
 * This maps attribute numbers between a pair of relations, designated
 * 'input' and 'output' (most typically inheritance parent and child
 * relations), whose common columns may have different attribute numbers.
 * Such difference may arise due to the columns being ordered differently
 * in the two relations or the two relations having dropped columns at
 * different positions.
 *
 * 'maplen' is set to the number of attributes of the 'output' relation,
 * taking into account any of its dropped attributes, with the corresponding
 * elements of the 'attnums' array set to 0.
 */
typedef struct AttrMap
{
	AttrNumber *attnums;
	int			maplen;
} AttrMap;

extern AttrMap *make_attrmap(int maplen);
extern void free_attrmap(AttrMap *map);

/* Conversion routines to build mappings */
extern AttrMap *build_attrmap_by_name(TupleDesc indesc,
									  TupleDesc outdesc);
extern AttrMap *build_attrmap_by_name_if_req(TupleDesc indesc,
											 TupleDesc outdesc);
extern AttrMap *build_attrmap_by_position(TupleDesc indesc,
										  TupleDesc outdesc,
										  const char *msg);

#endif							/* ATTMAP_H */
