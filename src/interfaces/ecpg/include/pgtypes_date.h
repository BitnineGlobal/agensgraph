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

/* src/interfaces/ecpg/include/pgtypes_date.h */

#ifndef PGTYPES_DATETIME
#define PGTYPES_DATETIME

#include <pgtypes.h>
#include <pgtypes_timestamp.h>

typedef long date;

#ifdef __cplusplus
extern "C"
{
#endif

extern date * PGTYPESdate_new(void);
extern void PGTYPESdate_free(date *);
extern date PGTYPESdate_from_asc(char *, char **);
extern char *PGTYPESdate_to_asc(date);
extern date PGTYPESdate_from_timestamp(timestamp);
extern void PGTYPESdate_julmdy(date, int *);
extern void PGTYPESdate_mdyjul(int *, date *);
extern int	PGTYPESdate_dayofweek(date);
extern void PGTYPESdate_today(date *);
extern int	PGTYPESdate_defmt_asc(date *, const char *, const char *);
extern int	PGTYPESdate_fmt_asc(date, const char *, char *);

#ifdef __cplusplus
}
#endif

#endif							/* PGTYPES_DATETIME */
