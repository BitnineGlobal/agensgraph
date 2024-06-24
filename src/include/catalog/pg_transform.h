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
 * pg_transform.h
 *	  definition of the "transform" system catalog (pg_transform)
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_transform.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_TRANSFORM_H
#define PG_TRANSFORM_H

#include "catalog/genbki.h"
#include "catalog/pg_transform_d.h"

/* ----------------
 *		pg_transform definition.  cpp turns this into
 *		typedef struct FormData_pg_transform
 * ----------------
 */
CATALOG(pg_transform,3576,TransformRelationId)
{
	Oid			oid;			/* oid */
	Oid			trftype;
	Oid			trflang;
	regproc		trffromsql;
	regproc		trftosql;
} FormData_pg_transform;

/* ----------------
 *		Form_pg_transform corresponds to a pointer to a tuple with
 *		the format of pg_transform relation.
 * ----------------
 */
typedef FormData_pg_transform *Form_pg_transform;

#endif							/* PG_TRANSFORM_H */
