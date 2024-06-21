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
 * replorigindesc.c
 *	  rmgr descriptor routines for replication/logical/origin.c
 *
 * Portions Copyright (c) 2015-2021, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/access/rmgrdesc/replorigindesc.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "replication/origin.h"

void
replorigin_desc(StringInfo buf, XLogReaderState *record)
{
	char	   *rec = XLogRecGetData(record);
	uint8		info = XLogRecGetInfo(record) & ~XLR_INFO_MASK;

	switch (info)
	{
		case XLOG_REPLORIGIN_SET:
			{
				xl_replorigin_set *xlrec;

				xlrec = (xl_replorigin_set *) rec;

				appendStringInfo(buf, "set %u; lsn %X/%X; force: %d",
								 xlrec->node_id,
								 LSN_FORMAT_ARGS(xlrec->remote_lsn),
								 xlrec->force);
				break;
			}
		case XLOG_REPLORIGIN_DROP:
			{
				xl_replorigin_drop *xlrec;

				xlrec = (xl_replorigin_drop *) rec;

				appendStringInfo(buf, "drop %u", xlrec->node_id);
				break;
			}
	}
}

const char *
replorigin_identify(uint8 info)
{
	switch (info)
	{
		case XLOG_REPLORIGIN_SET:
			return "SET";
		case XLOG_REPLORIGIN_DROP:
			return "DROP";
		default:
			return NULL;
	}
}
