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
 * pg_opfamily.h
 *	  definition of the "operator family" system catalog (pg_opfamily)
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_opfamily.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_OPFAMILY_H
#define PG_OPFAMILY_H

#include "catalog/genbki.h"
#include "catalog/pg_opfamily_d.h"

/* ----------------
 *		pg_opfamily definition. cpp turns this into
 *		typedef struct FormData_pg_opfamily
 * ----------------
 */
CATALOG(pg_opfamily,2753,OperatorFamilyRelationId)
{
	Oid			oid;			/* oid */

	/* index access method opfamily is for */
	Oid			opfmethod BKI_LOOKUP(pg_am);

	/* name of this opfamily */
	NameData	opfname;

	/* namespace of this opfamily */
	Oid			opfnamespace BKI_DEFAULT(PGNSP);

	/* opfamily owner */
	Oid			opfowner BKI_DEFAULT(PGUID);
} FormData_pg_opfamily;

/* ----------------
 *		Form_pg_opfamily corresponds to a pointer to a tuple with
 *		the format of pg_opfamily relation.
 * ----------------
 */
typedef FormData_pg_opfamily *Form_pg_opfamily;

#ifdef EXPOSE_TO_CLIENT_CODE

#define IsBooleanOpfamily(opfamily) \
	((opfamily) == BOOL_BTREE_FAM_OID || (opfamily) == BOOL_HASH_FAM_OID)

#endif							/* EXPOSE_TO_CLIENT_CODE */

#endif							/* PG_OPFAMILY_H */
