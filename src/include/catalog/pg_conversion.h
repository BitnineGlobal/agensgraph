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
 * pg_conversion.h
 *	  definition of the "conversion" system catalog (pg_conversion)
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_conversion.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_CONVERSION_H
#define PG_CONVERSION_H

#include "catalog/genbki.h"
#include "catalog/objectaddress.h"
#include "catalog/pg_conversion_d.h"

/* ----------------
 *		pg_conversion definition.  cpp turns this into
 *		typedef struct FormData_pg_conversion
 * ----------------
 */
CATALOG(pg_conversion,2607,ConversionRelationId)
{
	/* oid */
	Oid			oid;

	/* name of the conversion */
	NameData	conname;

	/* namespace that the conversion belongs to */
	Oid			connamespace BKI_DEFAULT(PGNSP);

	/* owner of the conversion */
	Oid			conowner BKI_DEFAULT(PGUID);

	/* FOR encoding id */
	int32		conforencoding BKI_LOOKUP(encoding);

	/* TO encoding id */
	int32		contoencoding BKI_LOOKUP(encoding);

	/* OID of the conversion proc */
	regproc		conproc BKI_LOOKUP(pg_proc);

	/* true if this is a default conversion */
	bool		condefault BKI_DEFAULT(t);
} FormData_pg_conversion;

/* ----------------
 *		Form_pg_conversion corresponds to a pointer to a tuple with
 *		the format of pg_conversion relation.
 * ----------------
 */
typedef FormData_pg_conversion *Form_pg_conversion;


extern ObjectAddress ConversionCreate(const char *conname, Oid connamespace,
									  Oid conowner,
									  int32 conforencoding, int32 contoencoding,
									  Oid conproc, bool def);
extern void RemoveConversionById(Oid conversionOid);
extern Oid	FindDefaultConversion(Oid connamespace, int32 for_encoding,
								  int32 to_encoding);

#endif							/* PG_CONVERSION_H */
