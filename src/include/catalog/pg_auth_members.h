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
 * pg_auth_members.h
 *	  definition of the "authorization identifier members" system catalog
 *	  (pg_auth_members).
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_auth_members.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_AUTH_MEMBERS_H
#define PG_AUTH_MEMBERS_H

#include "catalog/genbki.h"
#include "catalog/pg_auth_members_d.h"

/* ----------------
 *		pg_auth_members definition.  cpp turns this into
 *		typedef struct FormData_pg_auth_members
 * ----------------
 */
CATALOG(pg_auth_members,1261,AuthMemRelationId) BKI_SHARED_RELATION BKI_ROWTYPE_OID(2843,AuthMemRelation_Rowtype_Id) BKI_SCHEMA_MACRO
{
	Oid			roleid BKI_LOOKUP(pg_authid);	/* ID of a role */
	Oid			member BKI_LOOKUP(pg_authid);	/* ID of a member of that role */
	Oid			grantor BKI_LOOKUP(pg_authid);	/* who granted the membership */
	bool		admin_option;	/* granted with admin option? */
} FormData_pg_auth_members;

/* ----------------
 *		Form_pg_auth_members corresponds to a pointer to a tuple with
 *		the format of pg_auth_members relation.
 * ----------------
 */
typedef FormData_pg_auth_members *Form_pg_auth_members;

DECLARE_UNIQUE_INDEX_PKEY(pg_auth_members_role_member_index, 2694, on pg_auth_members using btree(roleid oid_ops, member oid_ops));
#define AuthMemRoleMemIndexId	2694
DECLARE_UNIQUE_INDEX(pg_auth_members_member_role_index, 2695, on pg_auth_members using btree(member oid_ops, roleid oid_ops));
#define AuthMemMemRoleIndexId	2695

#endif							/* PG_AUTH_MEMBERS_H */
