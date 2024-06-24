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

/*
 * src/interfaces/ecpg/include/sqlda-native.h
 */

#ifndef ECPG_SQLDA_NATIVE_H
#define ECPG_SQLDA_NATIVE_H

/*
 * Maximum length for identifiers (e.g. table names, column names,
 * function names).  Names actually are limited to one fewer byte than this,
 * because the length must include a trailing zero byte.
 *
 * This should be at least as much as NAMEDATALEN of the database the
 * applications run against.
 */
#define NAMEDATALEN 64

struct sqlname
{
	short		length;
	char		data[NAMEDATALEN];
};

struct sqlvar_struct
{
	short		sqltype;
	short		sqllen;
	char	   *sqldata;
	short	   *sqlind;
	struct sqlname sqlname;
};

struct sqlda_struct
{
	char		sqldaid[8];
	long		sqldabc;
	short		sqln;
	short		sqld;
	struct sqlda_struct *desc_next;
	struct sqlvar_struct sqlvar[1];
};

#endif							/* ECPG_SQLDA_NATIVE_H */
