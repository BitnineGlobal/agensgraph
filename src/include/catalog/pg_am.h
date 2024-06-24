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
 * pg_am.h
 *	  definition of the "access method" system catalog (pg_am)
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_am.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_AM_H
#define PG_AM_H

#include "catalog/genbki.h"
#include "catalog/pg_am_d.h"

/* ----------------
 *		pg_am definition.  cpp turns this into
 *		typedef struct FormData_pg_am
 * ----------------
 */
CATALOG(pg_am,2601,AccessMethodRelationId)
{
	Oid			oid;			/* oid */

	/* access method name */
	NameData	amname;

	/* handler function */
	regproc		amhandler BKI_LOOKUP(pg_proc);

	/* see AMTYPE_xxx constants below */
	char		amtype;
} FormData_pg_am;

/* ----------------
 *		Form_pg_am corresponds to a pointer to a tuple with
 *		the format of pg_am relation.
 * ----------------
 */
typedef FormData_pg_am *Form_pg_am;

#ifdef EXPOSE_TO_CLIENT_CODE

/*
 * Allowed values for amtype
 */
#define AMTYPE_INDEX					'i' /* index access method */
#define AMTYPE_TABLE					't' /* table access method */

#endif							/* EXPOSE_TO_CLIENT_CODE */

#endif							/* PG_AM_H */
