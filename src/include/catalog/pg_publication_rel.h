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
 * pg_publication_rel.h
 *	  definition of the system catalog for mappings between relations and
 *	  publications (pg_publication_rel)
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_publication_rel.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_PUBLICATION_REL_H
#define PG_PUBLICATION_REL_H

#include "catalog/genbki.h"
#include "catalog/pg_publication_rel_d.h"

/* ----------------
 *		pg_publication_rel definition.  cpp turns this into
 *		typedef struct FormData_pg_publication_rel
 * ----------------
 */
CATALOG(pg_publication_rel,6106,PublicationRelRelationId)
{
	Oid			oid;			/* oid */
	Oid			prpubid;		/* Oid of the publication */
	Oid			prrelid;		/* Oid of the relation */
} FormData_pg_publication_rel;

/* ----------------
 *		Form_pg_publication_rel corresponds to a pointer to a tuple with
 *		the format of pg_publication_rel relation.
 * ----------------
 */
typedef FormData_pg_publication_rel *Form_pg_publication_rel;

#endif							/* PG_PUBLICATION_REL_H */
