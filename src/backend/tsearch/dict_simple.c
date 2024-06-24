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
 * dict_simple.c
 *		Simple dictionary: just lowercase and check for stopword
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/tsearch/dict_simple.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "commands/defrem.h"
#include "tsearch/ts_locale.h"
#include "tsearch/ts_utils.h"
#include "utils/builtins.h"


typedef struct
{
	StopList	stoplist;
	bool		accept;
} DictSimple;


Datum
dsimple_init(PG_FUNCTION_ARGS)
{
	List	   *dictoptions = (List *) PG_GETARG_POINTER(0);
	DictSimple *d = (DictSimple *) palloc0(sizeof(DictSimple));
	bool		stoploaded = false,
				acceptloaded = false;
	ListCell   *l;

	d->accept = true;			/* default */

	foreach(l, dictoptions)
	{
		DefElem    *defel = (DefElem *) lfirst(l);

		if (strcmp(defel->defname, "stopwords") == 0)
		{
			if (stoploaded)
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						 errmsg("multiple StopWords parameters")));
			readstoplist(defGetString(defel), &d->stoplist, lowerstr);
			stoploaded = true;
		}
		else if (strcmp(defel->defname, "accept") == 0)
		{
			if (acceptloaded)
				ereport(ERROR,
						(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						 errmsg("multiple Accept parameters")));
			d->accept = defGetBoolean(defel);
			acceptloaded = true;
		}
		else
		{
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					 errmsg("unrecognized simple dictionary parameter: \"%s\"",
							defel->defname)));
		}
	}

	PG_RETURN_POINTER(d);
}

Datum
dsimple_lexize(PG_FUNCTION_ARGS)
{
	DictSimple *d = (DictSimple *) PG_GETARG_POINTER(0);
	char	   *in = (char *) PG_GETARG_POINTER(1);
	int32		len = PG_GETARG_INT32(2);
	char	   *txt;
	TSLexeme   *res;

	txt = lowerstr_with_len(in, len);

	if (*txt == '\0' || searchstoplist(&(d->stoplist), txt))
	{
		/* reject as stopword */
		pfree(txt);
		res = palloc0(sizeof(TSLexeme) * 2);
		PG_RETURN_POINTER(res);
	}
	else if (d->accept)
	{
		/* accept */
		res = palloc0(sizeof(TSLexeme) * 2);
		res[0].lexeme = txt;
		PG_RETURN_POINTER(res);
	}
	else
	{
		/* report as unrecognized */
		pfree(txt);
		PG_RETURN_POINTER(NULL);
	}
}
