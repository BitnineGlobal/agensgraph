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
 * fetch.c
 *	  Functions for fetching files from a local or remote data dir
 *
 * This file forms an abstraction of getting files from the "source".
 * There are two implementations of this interface: one for copying files
 * from a data directory via normal filesystem operations (copy_fetch.c),
 * and another for fetching files from a remote server via a libpq
 * connection (libpq_fetch.c)
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 *
 *-------------------------------------------------------------------------
 */
#include "postgres_fe.h"

#include <sys/stat.h>
#include <unistd.h>

#include "fetch.h"
#include "file_ops.h"
#include "filemap.h"
#include "pg_rewind.h"

void
fetchSourceFileList(void)
{
	if (datadir_source)
		traverse_datadir(datadir_source, &process_source_file);
	else
		libpqProcessFileList();
}

/*
 * Fetch all relation data files that are marked in the given data page map.
 */
void
executeFileMap(void)
{
	if (datadir_source)
		copy_executeFileMap(filemap);
	else
		libpq_executeFileMap(filemap);
}

/*
 * Fetch a single file into a malloc'd buffer. The file size is returned
 * in *filesize. The returned buffer is always zero-terminated, which is
 * handy for text files.
 */
char *
fetchFile(const char *filename, size_t *filesize)
{
	if (datadir_source)
		return slurpFile(datadir_source, filename, filesize);
	else
		return libpqGetFile(filename, filesize);
}
