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
 * brindesc.c
 *	  rmgr descriptor routines for BRIN indexes
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/access/rmgrdesc/brindesc.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/brin_xlog.h"

void
brin_desc(StringInfo buf, XLogReaderState *record)
{
	char	   *rec = XLogRecGetData(record);
	uint8		info = XLogRecGetInfo(record) & ~XLR_INFO_MASK;

	info &= XLOG_BRIN_OPMASK;
	if (info == XLOG_BRIN_CREATE_INDEX)
	{
		xl_brin_createidx *xlrec = (xl_brin_createidx *) rec;

		appendStringInfo(buf, "v%d pagesPerRange %u",
						 xlrec->version, xlrec->pagesPerRange);
	}
	else if (info == XLOG_BRIN_INSERT)
	{
		xl_brin_insert *xlrec = (xl_brin_insert *) rec;

		appendStringInfo(buf, "heapBlk %u pagesPerRange %u offnum %u",
						 xlrec->heapBlk,
						 xlrec->pagesPerRange,
						 xlrec->offnum);
	}
	else if (info == XLOG_BRIN_UPDATE)
	{
		xl_brin_update *xlrec = (xl_brin_update *) rec;

		appendStringInfo(buf, "heapBlk %u pagesPerRange %u old offnum %u, new offnum %u",
						 xlrec->insert.heapBlk,
						 xlrec->insert.pagesPerRange,
						 xlrec->oldOffnum,
						 xlrec->insert.offnum);
	}
	else if (info == XLOG_BRIN_SAMEPAGE_UPDATE)
	{
		xl_brin_samepage_update *xlrec = (xl_brin_samepage_update *) rec;

		appendStringInfo(buf, "offnum %u", xlrec->offnum);
	}
	else if (info == XLOG_BRIN_REVMAP_EXTEND)
	{
		xl_brin_revmap_extend *xlrec = (xl_brin_revmap_extend *) rec;

		appendStringInfo(buf, "targetBlk %u", xlrec->targetBlk);
	}
	else if (info == XLOG_BRIN_DESUMMARIZE)
	{
		xl_brin_desummarize *xlrec = (xl_brin_desummarize *) rec;

		appendStringInfo(buf, "pagesPerRange %u, heapBlk %u, page offset %u",
						 xlrec->pagesPerRange, xlrec->heapBlk, xlrec->regOffset);
	}
}

const char *
brin_identify(uint8 info)
{
	const char *id = NULL;

	switch (info & ~XLR_INFO_MASK)
	{
		case XLOG_BRIN_CREATE_INDEX:
			id = "CREATE_INDEX";
			break;
		case XLOG_BRIN_INSERT:
			id = "INSERT";
			break;
		case XLOG_BRIN_INSERT | XLOG_BRIN_INIT_PAGE:
			id = "INSERT+INIT";
			break;
		case XLOG_BRIN_UPDATE:
			id = "UPDATE";
			break;
		case XLOG_BRIN_UPDATE | XLOG_BRIN_INIT_PAGE:
			id = "UPDATE+INIT";
			break;
		case XLOG_BRIN_SAMEPAGE_UPDATE:
			id = "SAMEPAGE_UPDATE";
			break;
		case XLOG_BRIN_REVMAP_EXTEND:
			id = "REVMAP_EXTEND";
			break;
		case XLOG_BRIN_DESUMMARIZE:
			id = "DESUMMARIZE";
			break;
	}

	return id;
}
