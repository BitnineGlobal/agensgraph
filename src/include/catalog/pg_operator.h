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
 * pg_operator.h
 *	  definition of the "operator" system catalog (pg_operator)
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_operator.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_OPERATOR_H
#define PG_OPERATOR_H

#include "catalog/genbki.h"
#include "catalog/objectaddress.h"
#include "catalog/pg_operator_d.h"
#include "nodes/pg_list.h"

/* ----------------
 *		pg_operator definition.  cpp turns this into
 *		typedef struct FormData_pg_operator
 * ----------------
 */
CATALOG(pg_operator,2617,OperatorRelationId)
{
	Oid			oid;			/* oid */

	/* name of operator */
	NameData	oprname;

	/* OID of namespace containing this oper */
	Oid			oprnamespace BKI_DEFAULT(PGNSP);

	/* operator owner */
	Oid			oprowner BKI_DEFAULT(PGUID);

	/* 'l', 'r', or 'b' */
	char		oprkind BKI_DEFAULT(b);

	/* can be used in merge join? */
	bool		oprcanmerge BKI_DEFAULT(f);

	/* can be used in hash join? */
	bool		oprcanhash BKI_DEFAULT(f);

	/* left arg type, or 0 if 'l' oprkind */
	Oid			oprleft BKI_LOOKUP(pg_type);

	/* right arg type, or 0 if 'r' oprkind */
	Oid			oprright BKI_LOOKUP(pg_type);

	/* result datatype */
	Oid			oprresult BKI_LOOKUP(pg_type);

	/* OID of commutator oper, or 0 if none */
	Oid			oprcom BKI_DEFAULT(0) BKI_LOOKUP(pg_operator);

	/* OID of negator oper, or 0 if none */
	Oid			oprnegate BKI_DEFAULT(0) BKI_LOOKUP(pg_operator);

	/* OID of underlying function */
	regproc		oprcode BKI_LOOKUP(pg_proc);

	/* OID of restriction estimator, or 0 */
	regproc		oprrest BKI_DEFAULT(-) BKI_LOOKUP(pg_proc);

	/* OID of join estimator, or 0 */
	regproc		oprjoin BKI_DEFAULT(-) BKI_LOOKUP(pg_proc);
} FormData_pg_operator;

/* ----------------
 *		Form_pg_operator corresponds to a pointer to a tuple with
 *		the format of pg_operator relation.
 * ----------------
 */
typedef FormData_pg_operator *Form_pg_operator;


extern ObjectAddress OperatorCreate(const char *operatorName,
									Oid operatorNamespace,
									Oid leftTypeId,
									Oid rightTypeId,
									Oid procedureId,
									List *commutatorName,
									List *negatorName,
									Oid restrictionId,
									Oid joinId,
									bool canMerge,
									bool canHash);

extern ObjectAddress makeOperatorDependencies(HeapTuple tuple,
											  bool makeExtensionDep,
											  bool isUpdate);

extern void OperatorUpd(Oid baseId, Oid commId, Oid negId, bool isDelete);

#endif							/* PG_OPERATOR_H */
