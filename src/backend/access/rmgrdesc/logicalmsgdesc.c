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
 * logicalmsgdesc.c
 *	  rmgr descriptor routines for replication/logical/message.c
 *
 * Portions Copyright (c) 2015-2020, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/access/rmgrdesc/logicalmsgdesc.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "replication/message.h"

void
logicalmsg_desc(StringInfo buf, XLogReaderState *record)
{
	char	   *rec = XLogRecGetData(record);
	uint8		info = XLogRecGetInfo(record) & ~XLR_INFO_MASK;

	if (info == XLOG_LOGICAL_MESSAGE)
	{
		xl_logical_message *xlrec = (xl_logical_message *) rec;

		appendStringInfo(buf, "%s message size %zu bytes",
						 xlrec->transactional ? "transactional" : "nontransactional",
						 xlrec->message_size);
	}
}

const char *
logicalmsg_identify(uint8 info)
{
	if ((info & ~XLR_INFO_MASK) == XLOG_LOGICAL_MESSAGE)
		return "MESSAGE";

	return NULL;
}
