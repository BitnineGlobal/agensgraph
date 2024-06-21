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
 * dict.c
 *		Standard interface to dictionary
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/tsearch/dict.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "catalog/pg_type.h"
#include "tsearch/ts_cache.h"
#include "tsearch/ts_utils.h"
#include "utils/builtins.h"


/*
 * Lexize one word by dictionary, mostly debug function
 */
Datum
ts_lexize(PG_FUNCTION_ARGS)
{
	Oid			dictId = PG_GETARG_OID(0);
	text	   *in = PG_GETARG_TEXT_PP(1);
	ArrayType  *a;
	TSDictionaryCacheEntry *dict;
	TSLexeme   *res,
			   *ptr;
	Datum	   *da;
	DictSubState dstate = {false, false, NULL};

	dict = lookup_ts_dictionary_cache(dictId);

	res = (TSLexeme *) DatumGetPointer(FunctionCall4(&dict->lexize,
													 PointerGetDatum(dict->dictData),
													 PointerGetDatum(VARDATA_ANY(in)),
													 Int32GetDatum(VARSIZE_ANY_EXHDR(in)),
													 PointerGetDatum(&dstate)));

	if (dstate.getnext)
	{
		dstate.isend = true;
		ptr = (TSLexeme *) DatumGetPointer(FunctionCall4(&dict->lexize,
														 PointerGetDatum(dict->dictData),
														 PointerGetDatum(VARDATA_ANY(in)),
														 Int32GetDatum(VARSIZE_ANY_EXHDR(in)),
														 PointerGetDatum(&dstate)));
		if (ptr != NULL)
			res = ptr;
	}

	if (!res)
		PG_RETURN_NULL();

	ptr = res;
	while (ptr->lexeme)
		ptr++;
	da = (Datum *) palloc(sizeof(Datum) * (ptr - res));
	ptr = res;
	while (ptr->lexeme)
	{
		da[ptr - res] = CStringGetTextDatum(ptr->lexeme);
		ptr++;
	}

	a = construct_array(da,
						ptr - res,
						TEXTOID,
						-1,
						false,
						TYPALIGN_INT);

	ptr = res;
	while (ptr->lexeme)
	{
		pfree(DatumGetPointer(da[ptr - res]));
		pfree(ptr->lexeme);
		ptr++;
	}
	pfree(res);
	pfree(da);

	PG_RETURN_POINTER(a);
}
