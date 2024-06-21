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
 * ecpg_keywords.c
 *	  lexical token lookup for reserved words in postgres embedded SQL
 *
 * IDENTIFICATION
 *	  src/interfaces/ecpg/preproc/ecpg_keywords.c
 *
 *-------------------------------------------------------------------------
 */

#include "postgres_fe.h"

#include <ctype.h>

/* ScanKeywordList lookup data for ECPG keywords */
#include "ecpg_kwlist_d.h"
#include "preproc_extern.h"
#include "preproc.h"

/* Token codes for ECPG keywords */
#define PG_KEYWORD(kwname, value) value,

static const uint16 ECPGScanKeywordTokens[] = {
#include "ecpg_kwlist.h"
};

#undef PG_KEYWORD


/*
 * ScanECPGKeywordLookup - see if a given word is a keyword
 *
 * Returns the token value of the keyword, or -1 if no match.
 *
 * Keywords are matched using the same case-folding rules as in the backend.
 */
int
ScanECPGKeywordLookup(const char *text)
{
	int			kwnum;

	/* First check SQL symbols defined by the backend. */
	kwnum = ScanKeywordLookup(text, &ScanKeywords);
	if (kwnum >= 0)
		return SQLScanKeywordTokens[kwnum];

	/* Try ECPG-specific keywords. */
	kwnum = ScanKeywordLookup(text, &ScanECPGKeywords);
	if (kwnum >= 0)
		return ECPGScanKeywordTokens[kwnum];

	return -1;
}
