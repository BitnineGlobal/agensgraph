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
 * timeline.h
 *
 * Functions for reading and writing timeline history files.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/timeline.h
 */
#ifndef TIMELINE_H
#define TIMELINE_H

#include "access/xlogdefs.h"
#include "nodes/pg_list.h"

/*
 * A list of these structs describes the timeline history of the server. Each
 * TimeLineHistoryEntry represents a piece of WAL belonging to the history,
 * from newest to oldest. All WAL locations between 'begin' and 'end' belong to
 * the timeline represented by the entry. Together the 'begin' and 'end'
 * pointers of all the entries form a contiguous line from beginning of time
 * to infinity.
 */
typedef struct
{
	TimeLineID	tli;
	XLogRecPtr	begin;			/* inclusive */
	XLogRecPtr	end;			/* exclusive, InvalidXLogRecPtr means infinity */
} TimeLineHistoryEntry;

extern List *readTimeLineHistory(TimeLineID targetTLI);
extern bool existsTimeLineHistory(TimeLineID probeTLI);
extern TimeLineID findNewestTimeLine(TimeLineID startTLI);
extern void writeTimeLineHistory(TimeLineID newTLI, TimeLineID parentTLI,
								 XLogRecPtr switchpoint, char *reason);
extern void writeTimeLineHistoryFile(TimeLineID tli, char *content, int size);
extern void restoreTimeLineHistoryFiles(TimeLineID begin, TimeLineID end);
extern bool tliInHistory(TimeLineID tli, List *expectedTLEs);
extern TimeLineID tliOfPointInHistory(XLogRecPtr ptr, List *history);
extern XLogRecPtr tliSwitchPoint(TimeLineID tli, List *history,
								 TimeLineID *nextTLI);

#endif							/* TIMELINE_H */
