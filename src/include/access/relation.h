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
 * relation.h
 *	  Generic relation related routines.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/relation.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef ACCESS_RELATION_H
#define ACCESS_RELATION_H

#include "nodes/primnodes.h"
#include "storage/lockdefs.h"
#include "utils/relcache.h"

extern Relation relation_open(Oid relationId, LOCKMODE lockmode);
extern Relation try_relation_open(Oid relationId, LOCKMODE lockmode);
extern Relation relation_openrv(const RangeVar *relation, LOCKMODE lockmode);
extern Relation relation_openrv_extended(const RangeVar *relation,
										 LOCKMODE lockmode, bool missing_ok);
extern void relation_close(Relation relation, LOCKMODE lockmode);

#endif							/* ACCESS_RELATION_H */
