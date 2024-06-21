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
 * gistdesc.c
 *	  rmgr descriptor routines for access/gist/gistxlog.c
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/access/rmgrdesc/gistdesc.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/gistxlog.h"
#include "lib/stringinfo.h"
#include "storage/relfilenode.h"

static void
out_gistxlogPageUpdate(StringInfo buf, gistxlogPageUpdate *xlrec)
{
}

static void
out_gistxlogPageReuse(StringInfo buf, gistxlogPageReuse *xlrec)
{
	appendStringInfo(buf, "rel %u/%u/%u; blk %u; latestRemovedXid %u:%u",
					 xlrec->node.spcNode, xlrec->node.dbNode,
					 xlrec->node.relNode, xlrec->block,
					 EpochFromFullTransactionId(xlrec->latestRemovedFullXid),
					 XidFromFullTransactionId(xlrec->latestRemovedFullXid));
}

static void
out_gistxlogDelete(StringInfo buf, gistxlogDelete *xlrec)
{
	appendStringInfo(buf, "delete: latestRemovedXid %u, nitems: %u",
					 xlrec->latestRemovedXid, xlrec->ntodelete);

}

static void
out_gistxlogPageSplit(StringInfo buf, gistxlogPageSplit *xlrec)
{
	appendStringInfo(buf, "page_split: splits to %d pages",
					 xlrec->npage);
}

static void
out_gistxlogPageDelete(StringInfo buf, gistxlogPageDelete *xlrec)
{
	appendStringInfo(buf, "deleteXid %u:%u; downlink %u",
					 EpochFromFullTransactionId(xlrec->deleteXid),
					 XidFromFullTransactionId(xlrec->deleteXid),
					 xlrec->downlinkOffset);
}

void
gist_desc(StringInfo buf, XLogReaderState *record)
{
	char	   *rec = XLogRecGetData(record);
	uint8		info = XLogRecGetInfo(record) & ~XLR_INFO_MASK;

	switch (info)
	{
		case XLOG_GIST_PAGE_UPDATE:
			out_gistxlogPageUpdate(buf, (gistxlogPageUpdate *) rec);
			break;
		case XLOG_GIST_PAGE_REUSE:
			out_gistxlogPageReuse(buf, (gistxlogPageReuse *) rec);
			break;
		case XLOG_GIST_DELETE:
			out_gistxlogDelete(buf, (gistxlogDelete *) rec);
			break;
		case XLOG_GIST_PAGE_SPLIT:
			out_gistxlogPageSplit(buf, (gistxlogPageSplit *) rec);
			break;
		case XLOG_GIST_PAGE_DELETE:
			out_gistxlogPageDelete(buf, (gistxlogPageDelete *) rec);
			break;
		case XLOG_GIST_ASSIGN_LSN:
			/* No details to write out */
			break;
	}
}

const char *
gist_identify(uint8 info)
{
	const char *id = NULL;

	switch (info & ~XLR_INFO_MASK)
	{
		case XLOG_GIST_PAGE_UPDATE:
			id = "PAGE_UPDATE";
			break;
		case XLOG_GIST_DELETE:
			id = "DELETE";
			break;
		case XLOG_GIST_PAGE_REUSE:
			id = "PAGE_REUSE";
			break;
		case XLOG_GIST_PAGE_SPLIT:
			id = "PAGE_SPLIT";
			break;
		case XLOG_GIST_PAGE_DELETE:
			id = "PAGE_DELETE";
			break;
		case XLOG_GIST_ASSIGN_LSN:
			id = "ASSIGN_LSN";
			break;
	}

	return id;
}
