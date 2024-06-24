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
 * amvalidate.h
 *	  Support routines for index access methods' amvalidate functions.
 *
 * Copyright (c) 2016-2020, PostgreSQL Global Development Group
 *
 * src/include/access/amvalidate.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef AMVALIDATE_H
#define AMVALIDATE_H

#include "utils/catcache.h"


/* Struct returned (in a list) by identify_opfamily_groups() */
typedef struct OpFamilyOpFuncGroup
{
	Oid			lefttype;		/* amoplefttype/amproclefttype */
	Oid			righttype;		/* amoprighttype/amprocrighttype */
	uint64		operatorset;	/* bitmask of operators with these types */
	uint64		functionset;	/* bitmask of support funcs with these types */
} OpFamilyOpFuncGroup;


/* Functions in access/index/amvalidate.c */
extern List *identify_opfamily_groups(CatCList *oprlist, CatCList *proclist);
extern bool check_amproc_signature(Oid funcid, Oid restype, bool exact,
								   int minargs, int maxargs,...);
extern bool check_amoptsproc_signature(Oid funcid);
extern bool check_amop_signature(Oid opno, Oid restype,
								 Oid lefttype, Oid righttype);
extern bool opfamily_can_sort_type(Oid opfamilyoid, Oid datatypeoid);

#endif							/* AMVALIDATE_H */
