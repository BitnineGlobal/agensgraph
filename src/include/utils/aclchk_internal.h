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
 * aclchk_internal.h
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/aclchk_internal.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef ACLCHK_INTERNAL_H
#define ACLCHK_INTERNAL_H

#include "nodes/parsenodes.h"
#include "nodes/pg_list.h"

/*
 * The information about one Grant/Revoke statement, in internal format: object
 * and grantees names have been turned into Oids, the privilege list is an
 * AclMode bitmask.  If 'privileges' is ACL_NO_RIGHTS (the 0 value) and
 * all_privs is true, 'privileges' will be internally set to the right kind of
 * ACL_ALL_RIGHTS_*, depending on the object type (NB - this will modify the
 * InternalGrant struct!)
 *
 * Note: 'all_privs' and 'privileges' represent object-level privileges only.
 * There might also be column-level privilege specifications, which are
 * represented in col_privs (this is a list of untransformed AccessPriv nodes).
 * Column privileges are only valid for objtype OBJECT_TABLE.
 */
typedef struct
{
	bool		is_grant;
	ObjectType	objtype;
	List	   *objects;
	bool		all_privs;
	AclMode		privileges;
	List	   *col_privs;
	List	   *grantees;
	bool		grant_option;
	DropBehavior behavior;
} InternalGrant;


#endif							/* ACLCHK_INTERNAL_H */
