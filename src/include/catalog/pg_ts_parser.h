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
 * pg_ts_parser.h
 *	  definition of the "text search parser" system catalog (pg_ts_parser)
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/catalog/pg_ts_parser.h
 *
 * NOTES
 *	  The Catalog.pm module reads this file and derives schema
 *	  information.
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_TS_PARSER_H
#define PG_TS_PARSER_H

#include "catalog/genbki.h"
#include "catalog/pg_ts_parser_d.h"

/* ----------------
 *		pg_ts_parser definition.  cpp turns this into
 *		typedef struct FormData_pg_ts_parser
 * ----------------
 */
CATALOG(pg_ts_parser,3601,TSParserRelationId)
{
	Oid			oid;			/* oid */

	/* parser's name */
	NameData	prsname;

	/* name space */
	Oid			prsnamespace BKI_DEFAULT(PGNSP);

	/* init parsing session */
	regproc		prsstart BKI_LOOKUP(pg_proc);

	/* return next token */
	regproc		prstoken BKI_LOOKUP(pg_proc);

	/* finalize parsing session */
	regproc		prsend BKI_LOOKUP(pg_proc);

	/* return data for headline creation */
	regproc		prsheadline BKI_LOOKUP(pg_proc);

	/* return descriptions of lexeme's types */
	regproc		prslextype BKI_LOOKUP(pg_proc);
} FormData_pg_ts_parser;

typedef FormData_pg_ts_parser *Form_pg_ts_parser;

#endif							/* PG_TS_PARSER_H */
