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
 * dbasedesc.c
 *	  rmgr descriptor routines for commands/dbcommands.c
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/access/rmgrdesc/dbasedesc.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "commands/dbcommands_xlog.h"
#include "lib/stringinfo.h"


void
dbase_desc(StringInfo buf, XLogReaderState *record)
{
	char	   *rec = XLogRecGetData(record);
	uint8		info = XLogRecGetInfo(record) & ~XLR_INFO_MASK;

	if (info == XLOG_DBASE_CREATE)
	{
		xl_dbase_create_rec *xlrec = (xl_dbase_create_rec *) rec;

		appendStringInfo(buf, "copy dir %u/%u to %u/%u",
						 xlrec->src_tablespace_id, xlrec->src_db_id,
						 xlrec->tablespace_id, xlrec->db_id);
	}
	else if (info == XLOG_DBASE_DROP)
	{
		xl_dbase_drop_rec *xlrec = (xl_dbase_drop_rec *) rec;
		int			i;

		appendStringInfo(buf, "dir");
		for (i = 0; i < xlrec->ntablespaces; i++)
			appendStringInfo(buf, " %u/%u",
							 xlrec->tablespace_ids[i], xlrec->db_id);
	}
}

const char *
dbase_identify(uint8 info)
{
	const char *id = NULL;

	switch (info & ~XLR_INFO_MASK)
	{
		case XLOG_DBASE_CREATE:
			id = "CREATE";
			break;
		case XLOG_DBASE_DROP:
			id = "DROP";
			break;
	}

	return id;
}
