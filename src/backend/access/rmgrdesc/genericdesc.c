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
 * genericdesc.c
 *	  rmgr descriptor routines for access/transam/generic_xlog.c
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/backend/access/rmgrdesc/genericdesc.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "access/generic_xlog.h"
#include "lib/stringinfo.h"
#include "storage/relfilenode.h"

/*
 * Description of generic xlog record: write page regions that this record
 * overrides.
 */
void
generic_desc(StringInfo buf, XLogReaderState *record)
{
	Pointer		ptr = XLogRecGetData(record),
				end = ptr + XLogRecGetDataLen(record);

	while (ptr < end)
	{
		OffsetNumber offset,
					length;

		memcpy(&offset, ptr, sizeof(offset));
		ptr += sizeof(offset);
		memcpy(&length, ptr, sizeof(length));
		ptr += sizeof(length);
		ptr += length;

		if (ptr < end)
			appendStringInfo(buf, "offset %u, length %u; ", offset, length);
		else
			appendStringInfo(buf, "offset %u, length %u", offset, length);
	}
}

/*
 * Identification of generic xlog record: we don't distinguish any subtypes
 * inside generic xlog records.
 */
const char *
generic_identify(uint8 info)
{
	return "Generic";
}
