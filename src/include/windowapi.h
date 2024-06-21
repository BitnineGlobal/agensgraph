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
 * windowapi.h
 *	  API for window functions to extract data from their window
 *
 * A window function does not receive its arguments in the normal way
 * (and therefore the concept of strictness is irrelevant).  Instead it
 * receives a "WindowObject", which it can fetch with PG_WINDOW_OBJECT()
 * (note V1 calling convention must be used).  Correct call context can
 * be tested with WindowObjectIsValid().  Although argument values are
 * not passed, the call is correctly set up so that PG_NARGS() can be
 * used and argument type information can be obtained with
 * get_fn_expr_argtype(), get_fn_expr_arg_stable(), etc.
 *
 * Operations on the WindowObject allow the window function to find out
 * the current row number, total number of rows in the partition, etc
 * and to evaluate its argument expression(s) at various rows in the
 * window partition.  See the header comments for each WindowObject API
 * function in nodeWindowAgg.c for details.
 *
 *
 * Portions Copyright (c) 2000-2020, PostgreSQL Global Development Group
 *
 * src/include/windowapi.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef WINDOWAPI_H
#define WINDOWAPI_H

/* values of "seektype" */
#define WINDOW_SEEK_CURRENT 0
#define WINDOW_SEEK_HEAD 1
#define WINDOW_SEEK_TAIL 2

/* this struct is private in nodeWindowAgg.c */
typedef struct WindowObjectData *WindowObject;

#define PG_WINDOW_OBJECT() ((WindowObject) fcinfo->context)

#define WindowObjectIsValid(winobj) \
	((winobj) != NULL && IsA(winobj, WindowObjectData))

extern void *WinGetPartitionLocalMemory(WindowObject winobj, Size sz);

extern int64 WinGetCurrentPosition(WindowObject winobj);
extern int64 WinGetPartitionRowCount(WindowObject winobj);

extern void WinSetMarkPosition(WindowObject winobj, int64 markpos);

extern bool WinRowsArePeers(WindowObject winobj, int64 pos1, int64 pos2);

extern Datum WinGetFuncArgInPartition(WindowObject winobj, int argno,
									  int relpos, int seektype, bool set_mark,
									  bool *isnull, bool *isout);

extern Datum WinGetFuncArgInFrame(WindowObject winobj, int argno,
								  int relpos, int seektype, bool set_mark,
								  bool *isnull, bool *isout);

extern Datum WinGetFuncArgCurrent(WindowObject winobj, int argno,
								  bool *isnull);

#endif							/* WINDOWAPI_H */
