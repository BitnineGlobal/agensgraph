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

/* -------------------------------------------------------------------------
 *
 * pg_seclabel.h
 *	  definition of the "security label" system catalog (pg_seclabel)
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_seclabel.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 * -------------------------------------------------------------------------
 */
#ifndef PG_SECLABEL_H
#define PG_SECLABEL_H

#include "catalog/genbki.h"
#include "catalog/pg_seclabel_d.h"

/* ----------------
 *		pg_seclabel definition.  cpp turns this into
 *		typedef struct FormData_pg_seclabel
 * ----------------
 */
CATALOG(pg_seclabel,3596,SecLabelRelationId)
{
	Oid			objoid;			/* OID of the object itself */
	Oid			classoid;		/* OID of table containing the object */
	int32		objsubid;		/* column number, or 0 if not used */

#ifdef CATALOG_VARLEN			/* variable-length fields start here */
	text		provider BKI_FORCE_NOT_NULL;	/* name of label provider */
	text		label BKI_FORCE_NOT_NULL;	/* security label of the object */
#endif
} FormData_pg_seclabel;

#endif							/* PG_SECLABEL_H */
