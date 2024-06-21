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

/* Processed by ecpg (regression mode) */
/* These include files are added by the preprocessor */
#include <ecpglib.h>
#include <ecpgerrno.h>
#include <sqlca.h>
/* Needed for informix compatibility */
#include <ecpg_informix.h>
/* End of automatic include section */
#define ECPGdebug(X,Y) ECPGdebug((X)+100,(Y))

#line 1 "rfmtlong.pgc"
#include <stdio.h>
#include <stdlib.h>
#include <pgtypes_error.h>
#include <sqltypes.h>

/*
 * This file tests various forms of long-input/output by means of
 * rfmtlong
 */


static void
check_return(int ret);

static void
fmtlong(long lng, const char *fmt)
{
	static int i;
	int r;
	char buf[30];

	r = rfmtlong(lng, fmt, buf);
	printf("r: %d ", r);
	if (r == 0)
	{
		printf("%d: %s (fmt was: %s)\n", i++, buf, fmt);
	}
	else
		check_return(r);
}

int
main(void)
{
	ECPGdebug(1, stderr);

	fmtlong(-8494493, "-<<<<,<<<,<<<,<<<");
	fmtlong(-8494493, "################");
	fmtlong(-8494493, "+++$$$$$$$$$$$$$.##");
	fmtlong(-8494493, "(&,&&&,&&&,&&&.)");
	fmtlong(-8494493, "<<<<,<<<,<<<,<<<");
	fmtlong(-8494493, "$************.**");
	fmtlong(-8494493, "---$************.**");
	fmtlong(-8494493, "+-+################");
	fmtlong(-8494493, "abc: ################+-+");
	fmtlong(-8494493, "+<<<<,<<<,<<<,<<<");

	return 0;
}

static void
check_return(int ret)
{
	switch(ret)
	{
		case ECPG_INFORMIX_ENOTDMY:
			printf("(ECPG_INFORMIX_ENOTDMY)");
			break;
		case ECPG_INFORMIX_ENOSHORTDATE:
			printf("(ECPG_INFORMIX_ENOSHORTDATE)");
			break;
		case ECPG_INFORMIX_BAD_DAY:
			printf("(ECPG_INFORMIX_BAD_DAY)");
			break;
		case ECPG_INFORMIX_BAD_MONTH:
			printf("(ECPG_INFORMIX_BAD_MONTH)");
			break;
		default:
			printf("(unknown ret: %d)", ret);
			break;
	}
	printf("\n");
}
