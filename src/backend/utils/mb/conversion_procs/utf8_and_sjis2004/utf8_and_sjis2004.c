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
 *	  SHIFT_JIS_2004 <--> UTF8
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/backend/utils/mb/conversion_procs/utf8_and_sjis2004/utf8_and_sjis2004.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres.h"
#include "fmgr.h"
#include "mb/pg_wchar.h"
#include "../../Unicode/shift_jis_2004_to_utf8.map"
#include "../../Unicode/utf8_to_shift_jis_2004.map"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(shift_jis_2004_to_utf8);
PG_FUNCTION_INFO_V1(utf8_to_shift_jis_2004);

/* ----------
 * conv_proc(
 *		INTEGER,	-- source encoding id
 *		INTEGER,	-- destination encoding id
 *		CSTRING,	-- source string (null terminated C string)
 *		CSTRING,	-- destination string (null terminated C string)
 *		INTEGER		-- source string length
 * ) returns VOID;
 * ----------
 */
Datum
shift_jis_2004_to_utf8(PG_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) PG_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) PG_GETARG_CSTRING(3);
	int			len = PG_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(PG_SHIFT_JIS_2004, PG_UTF8);

	LocalToUtf(src, len, dest,
			   &shift_jis_2004_to_unicode_tree,
			   LUmapSHIFT_JIS_2004_combined, lengthof(LUmapSHIFT_JIS_2004_combined),
			   NULL,
			   PG_SHIFT_JIS_2004);

	PG_RETURN_VOID();
}

Datum
utf8_to_shift_jis_2004(PG_FUNCTION_ARGS)
{
	unsigned char *src = (unsigned char *) PG_GETARG_CSTRING(2);
	unsigned char *dest = (unsigned char *) PG_GETARG_CSTRING(3);
	int			len = PG_GETARG_INT32(4);

	CHECK_ENCODING_CONVERSION_ARGS(PG_UTF8, PG_SHIFT_JIS_2004);

	UtfToLocal(src, len, dest,
			   &shift_jis_2004_from_unicode_tree,
			   ULmapSHIFT_JIS_2004_combined, lengthof(ULmapSHIFT_JIS_2004_combined),
			   NULL,
			   PG_SHIFT_JIS_2004);

	PG_RETURN_VOID();
}
