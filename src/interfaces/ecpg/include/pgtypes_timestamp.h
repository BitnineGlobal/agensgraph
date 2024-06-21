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

/* src/interfaces/ecpg/include/pgtypes_timestamp.h */

#ifndef PGTYPES_TIMESTAMP
#define PGTYPES_TIMESTAMP

#include <pgtypes.h>
/* pgtypes_interval.h includes ecpg_config.h */
#include <pgtypes_interval.h>

typedef int64 timestamp;
typedef int64 TimestampTz;

#ifdef __cplusplus
extern "C"
{
#endif

extern timestamp PGTYPEStimestamp_from_asc(char *, char **);
extern char *PGTYPEStimestamp_to_asc(timestamp);
extern int	PGTYPEStimestamp_sub(timestamp *, timestamp *, interval *);
extern int	PGTYPEStimestamp_fmt_asc(timestamp *, char *, int, const char *);
extern void PGTYPEStimestamp_current(timestamp *);
extern int	PGTYPEStimestamp_defmt_asc(const char *, const char *, timestamp *);
extern int	PGTYPEStimestamp_add_interval(timestamp * tin, interval * span, timestamp * tout);
extern int	PGTYPEStimestamp_sub_interval(timestamp * tin, interval * span, timestamp * tout);

#ifdef __cplusplus
}
#endif

#endif							/* PGTYPES_TIMESTAMP */
