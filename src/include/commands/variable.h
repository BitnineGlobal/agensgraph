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
 * variable.h
 *		Routines for handling specialized SET variables.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/commands/variable.h
 */
#ifndef VARIABLE_H
#define VARIABLE_H

#include "utils/guc.h"


extern bool check_datestyle(char **newval, void **extra, GucSource source);
extern void assign_datestyle(const char *newval, void *extra);
extern bool check_timezone(char **newval, void **extra, GucSource source);
extern void assign_timezone(const char *newval, void *extra);
extern const char *show_timezone(void);
extern bool check_log_timezone(char **newval, void **extra, GucSource source);
extern void assign_log_timezone(const char *newval, void *extra);
extern const char *show_log_timezone(void);
extern bool check_transaction_read_only(bool *newval, void **extra, GucSource source);
extern bool check_XactIsoLevel(int *newval, void **extra, GucSource source);
extern bool check_transaction_deferrable(bool *newval, void **extra, GucSource source);
extern bool check_random_seed(double *newval, void **extra, GucSource source);
extern void assign_random_seed(double newval, void *extra);
extern const char *show_random_seed(void);
extern bool check_client_encoding(char **newval, void **extra, GucSource source);
extern void assign_client_encoding(const char *newval, void *extra);
extern bool check_session_authorization(char **newval, void **extra, GucSource source);
extern void assign_session_authorization(const char *newval, void *extra);
extern bool check_role(char **newval, void **extra, GucSource source);
extern void assign_role(const char *newval, void *extra);
extern const char *show_role(void);

#endif							/* VARIABLE_H */
