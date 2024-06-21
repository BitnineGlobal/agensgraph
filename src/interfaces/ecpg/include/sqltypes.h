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

#ifndef ECPG_SQLTYPES_H
#define ECPG_SQLTYPES_H

#include <limits.h>

#define CCHARTYPE	ECPGt_char
#define CSHORTTYPE	ECPGt_short
#define CINTTYPE	ECPGt_int
#define CLONGTYPE	ECPGt_long
#define CFLOATTYPE	ECPGt_float
#define CDOUBLETYPE ECPGt_double
#define CDECIMALTYPE	ECPGt_decimal
#define CFIXCHARTYPE	108
#define CSTRINGTYPE ECPGt_char
#define CDATETYPE	ECPGt_date
#define CMONEYTYPE	111
#define CDTIMETYPE	ECPGt_timestamp
#define CLOCATORTYPE	113
#define CVCHARTYPE	ECPGt_varchar
#define CINVTYPE	115
#define CFILETYPE	116
#define CINT8TYPE	ECPGt_long_long
#define CCOLLTYPE		118
#define CLVCHARTYPE		119
#define CFIXBINTYPE		120
#define CVARBINTYPE		121
#define CBOOLTYPE		ECPGt_bool
#define CROWTYPE		123
#define CLVCHARPTRTYPE	124
#define CTYPEMAX	25

/*
 * Values used in sqlda->sqlvar[i]->sqltype
 */
#define SQLCHAR		ECPGt_char
#define SQLSMINT	ECPGt_short
#define SQLINT		ECPGt_int
#define SQLFLOAT	ECPGt_double
#define SQLSMFLOAT	ECPGt_float
#define SQLDECIMAL	ECPGt_decimal
#define SQLSERIAL	ECPGt_int
#define SQLDATE		ECPGt_date
#define SQLDTIME	ECPGt_timestamp
#define SQLTEXT		ECPGt_char
#define SQLVCHAR	ECPGt_char
#define SQLINTERVAL		ECPGt_interval
#define SQLNCHAR	ECPGt_char
#define SQLNVCHAR	ECPGt_char
#ifdef HAVE_LONG_LONG_INT_64
#define SQLINT8		ECPGt_long_long
#define SQLSERIAL8	ECPGt_long_long
#else
#define SQLINT8		ECPGt_long
#define SQLSERIAL8	ECPGt_long
#endif

#endif							/* ndef ECPG_SQLTYPES_H */
