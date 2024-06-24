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
 * pg_namespace.c
 *	  routines to support manipulation of the pg_namespace relation
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/catalog/pg_namespace.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/htup_details.h"
#include "access/table.h"
#include "catalog/catalog.h"
#include "catalog/dependency.h"
#include "catalog/indexing.h"
#include "catalog/objectaccess.h"
#include "catalog/pg_namespace.h"
#include "utils/builtins.h"
#include "utils/rel.h"
#include "utils/syscache.h"


/* ----------------
 * NamespaceCreate
 *
 * Create a namespace (schema) with the given name and owner OID.
 *
 * If isTemp is true, this schema is a per-backend schema for holding
 * temporary tables.  Currently, it is used to prevent it from being
 * linked as a member of any active extension.  (If someone does CREATE
 * TEMP TABLE in an extension script, we don't want the temp schema to
 * become part of the extension). And to avoid checking for default ACL
 * for temp namespace (as it is not necessary).
 * ---------------
 */
Oid
NamespaceCreate(const char *nspName, Oid ownerId, bool isTemp)
{
	Relation	nspdesc;
	HeapTuple	tup;
	Oid			nspoid;
	bool		nulls[Natts_pg_namespace];
	Datum		values[Natts_pg_namespace];
	NameData	nname;
	TupleDesc	tupDesc;
	ObjectAddress myself;
	int			i;
	Acl		   *nspacl;

	/* sanity checks */
	if (!nspName)
		elog(ERROR, "no namespace name supplied");

	/* make sure there is no existing namespace of same name */
	if (SearchSysCacheExists1(NAMESPACENAME, PointerGetDatum(nspName)))
		ereport(ERROR,
				(errcode(ERRCODE_DUPLICATE_SCHEMA),
				 errmsg("schema \"%s\" already exists", nspName)));

	if (!isTemp)
		nspacl = get_user_default_acl(OBJECT_SCHEMA, ownerId,
									  InvalidOid);
	else
		nspacl = NULL;

	nspdesc = table_open(NamespaceRelationId, RowExclusiveLock);
	tupDesc = nspdesc->rd_att;

	/* initialize nulls and values */
	for (i = 0; i < Natts_pg_namespace; i++)
	{
		nulls[i] = false;
		values[i] = (Datum) NULL;
	}

	nspoid = GetNewOidWithIndex(nspdesc, NamespaceOidIndexId,
								Anum_pg_namespace_oid);
	values[Anum_pg_namespace_oid - 1] = ObjectIdGetDatum(nspoid);
	namestrcpy(&nname, nspName);
	values[Anum_pg_namespace_nspname - 1] = NameGetDatum(&nname);
	values[Anum_pg_namespace_nspowner - 1] = ObjectIdGetDatum(ownerId);
	if (nspacl != NULL)
		values[Anum_pg_namespace_nspacl - 1] = PointerGetDatum(nspacl);
	else
		nulls[Anum_pg_namespace_nspacl - 1] = true;


	tup = heap_form_tuple(tupDesc, values, nulls);

	CatalogTupleInsert(nspdesc, tup);
	Assert(OidIsValid(nspoid));

	table_close(nspdesc, RowExclusiveLock);

	/* Record dependencies */
	myself.classId = NamespaceRelationId;
	myself.objectId = nspoid;
	myself.objectSubId = 0;

	/* dependency on owner */
	recordDependencyOnOwner(NamespaceRelationId, nspoid, ownerId);

	/* dependences on roles mentioned in default ACL */
	recordDependencyOnNewAcl(NamespaceRelationId, nspoid, 0, ownerId, nspacl);

	/* dependency on extension ... but not for magic temp schemas */
	if (!isTemp)
		recordDependencyOnCurrentExtension(&myself, false);

	/* Post creation hook for new schema */
	InvokeObjectPostCreateHook(NamespaceRelationId, nspoid, 0);

	return nspoid;
}
