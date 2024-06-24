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

#line 1 "charfuncs.pgc"
#include <stdio.h>
#include <stdlib.h>
#include <sqltypes.h>

int main(void)
{
	char t1[] = "abc  def  ghi  ";
	          /* 123456789012345 */
	char buf[50];
	int k;

	printf("t1: _%s_\n", t1);
	rupshift(t1);
	printf("t1: _%s_\n", t1);

	k = 2;
	ldchar(t1, k, buf);
	printf("byleng(t1, %d): %d, ldchar: _%s_\n", k, byleng(t1, k), buf);
	k = 5;
	ldchar(t1, k, buf);
	printf("byleng(t1, %d): %d, ldchar: _%s_\n", k, byleng(t1, k), buf);
	k = 9;
	ldchar(t1, k, buf);
	printf("byleng(t1, %d): %d, ldchar: _%s_\n", k, byleng(t1, k), buf);
	k = 15;
	ldchar(t1, k, buf);
	printf("byleng(t1, %d): %d, ldchar: _%s_\n", k, byleng(t1, k), buf);


	return 0;
}
