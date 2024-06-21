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
 * Simple list facilities for frontend code
 *
 * Data structures for simple lists of OIDs, strings, and pointers.  The
 * support for these is very primitive compared to the backend's List
 * facilities, but it's all we need in, eg, pg_dump.
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/fe_utils/simple_list.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SIMPLE_LIST_H
#define SIMPLE_LIST_H

typedef struct SimpleOidListCell
{
	struct SimpleOidListCell *next;
	Oid			val;
} SimpleOidListCell;

typedef struct SimpleOidList
{
	SimpleOidListCell *head;
	SimpleOidListCell *tail;
} SimpleOidList;

typedef struct SimpleStringListCell
{
	struct SimpleStringListCell *next;
	bool		touched;		/* true, when this string was searched and
								 * touched */
	char		val[FLEXIBLE_ARRAY_MEMBER]; /* null-terminated string here */
} SimpleStringListCell;

typedef struct SimpleStringList
{
	SimpleStringListCell *head;
	SimpleStringListCell *tail;
} SimpleStringList;

typedef struct SimplePtrListCell
{
	struct SimplePtrListCell *next;
	void	   *ptr;
} SimplePtrListCell;

typedef struct SimplePtrList
{
	SimplePtrListCell *head;
	SimplePtrListCell *tail;
} SimplePtrList;

extern void simple_oid_list_append(SimpleOidList *list, Oid val);
extern bool simple_oid_list_member(SimpleOidList *list, Oid val);
extern void simple_oid_list_destroy(SimpleOidList *list);

extern void simple_string_list_append(SimpleStringList *list, const char *val);
extern bool simple_string_list_member(SimpleStringList *list, const char *val);
extern void simple_string_list_destroy(SimpleStringList *list);

extern const char *simple_string_list_not_touched(SimpleStringList *list);

extern void simple_ptr_list_append(SimplePtrList *list, void *val);

#endif							/* SIMPLE_LIST_H */
