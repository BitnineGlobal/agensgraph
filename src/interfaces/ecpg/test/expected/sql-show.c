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
/* End of automatic include section */
#define ECPGdebug(X,Y) ECPGdebug((X)+100,(Y))

#line 1 "show.pgc"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#line 1 "regression.h"






#line 5 "show.pgc"


int main() {
  /* exec sql begin declare section */
       
  
#line 9 "show.pgc"
 char var [ 25 ] = "public" ;
/* exec sql end declare section */
#line 10 "show.pgc"


  ECPGdebug(1, stderr);
  { ECPGconnect(__LINE__, 0, "ecpg1_regression" , NULL, NULL , NULL, 0); }
#line 13 "show.pgc"


  /* exec sql whenever sql_warning  sqlprint ; */
#line 15 "show.pgc"

  /* exec sql whenever sqlerror  sqlprint ; */
#line 16 "show.pgc"


  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "set search_path to $0", 
	ECPGt_char,(var),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EOIT, ECPGt_EORT);
#line 18 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 18 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 18 "show.pgc"

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "show search_path", ECPGt_EOIT, 
	ECPGt_char,(var),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);
#line 19 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 19 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 19 "show.pgc"

  printf("Var: Search path: %s\n", var);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "set search_path to 'public'", ECPGt_EOIT, ECPGt_EORT);
#line 22 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 22 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 22 "show.pgc"

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "show search_path", ECPGt_EOIT, 
	ECPGt_char,(var),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);
#line 23 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 23 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 23 "show.pgc"

  printf("Var: Search path: %s\n", var);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "set standard_conforming_strings to off", ECPGt_EOIT, ECPGt_EORT);
#line 26 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 26 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 26 "show.pgc"

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "show standard_conforming_strings", ECPGt_EOIT, 
	ECPGt_char,(var),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);
#line 27 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 27 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 27 "show.pgc"

  printf("Var: Standard conforming strings: %s\n", var);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "set time zone PST8PDT", ECPGt_EOIT, ECPGt_EORT);
#line 30 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 30 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 30 "show.pgc"

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "show time zone", ECPGt_EOIT, 
	ECPGt_char,(var),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);
#line 31 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 31 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 31 "show.pgc"

  printf("Time Zone: %s\n", var);

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "set transaction isolation level read committed", ECPGt_EOIT, ECPGt_EORT);
#line 34 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 34 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 34 "show.pgc"

  { ECPGdo(__LINE__, 0, 1, NULL, 0, ECPGst_normal, "show transaction isolation level", ECPGt_EOIT, 
	ECPGt_char,(var),(long)25,(long)1,(25)*sizeof(char), 
	ECPGt_NO_INDICATOR, NULL , 0L, 0L, 0L, ECPGt_EORT);
#line 35 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 35 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 35 "show.pgc"

  printf("Transaction isolation level: %s\n", var);

  { ECPGdisconnect(__LINE__, "ALL");
#line 38 "show.pgc"

if (sqlca.sqlwarn[0] == 'W') sqlprint();
#line 38 "show.pgc"

if (sqlca.sqlcode < 0) sqlprint();}
#line 38 "show.pgc"


  return 0;
}
