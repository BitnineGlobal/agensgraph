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
 * File and directory permission routines
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/common/file_perm.c
 *
 *-------------------------------------------------------------------------
 */
#include "c.h"

#include "common/file_perm.h"

/* Modes for creating directories and files in the data directory */
int			pg_dir_create_mode = PG_DIR_MODE_OWNER;
int			pg_file_create_mode = PG_FILE_MODE_OWNER;

/*
 * Mode mask to pass to umask().  This is more of a preventative measure since
 * all file/directory creates should be performed using the create modes above.
 */
int			pg_mode_mask = PG_MODE_MASK_OWNER;

/*
 * Set create modes and mask to use when writing to PGDATA based on the data
 * directory mode passed.  If group read/execute are present in the mode, then
 * create modes and mask will be relaxed to allow group read/execute on all
 * newly created files and directories.
 */
void
SetDataDirectoryCreatePerm(int dataDirMode)
{
	/* If the data directory mode has group access */
	if ((PG_DIR_MODE_GROUP & dataDirMode) == PG_DIR_MODE_GROUP)
	{
		pg_dir_create_mode = PG_DIR_MODE_GROUP;
		pg_file_create_mode = PG_FILE_MODE_GROUP;
		pg_mode_mask = PG_MODE_MASK_GROUP;
	}
	/* Else use default permissions */
	else
	{
		pg_dir_create_mode = PG_DIR_MODE_OWNER;
		pg_file_create_mode = PG_FILE_MODE_OWNER;
		pg_mode_mask = PG_MODE_MASK_OWNER;
	}
}

#ifdef FRONTEND

/*
 * Get the create modes and mask to use when writing to PGDATA by examining the
 * mode of the PGDATA directory and calling SetDataDirectoryCreatePerm().
 *
 * Errors are not handled here and should be reported by the application when
 * false is returned.
 *
 * Suppress when on Windows, because there may not be proper support for Unix-y
 * file permissions.
 */
bool
GetDataDirectoryCreatePerm(const char *dataDir)
{
#if !defined(WIN32) && !defined(__CYGWIN__)
	struct stat statBuf;

	/*
	 * If an error occurs getting the mode then return false.  The caller is
	 * responsible for generating an error, if appropriate, indicating that we
	 * were unable to access the data directory.
	 */
	if (stat(dataDir, &statBuf) == -1)
		return false;

	/* Set permissions */
	SetDataDirectoryCreatePerm(statBuf.st_mode);
	return true;
#else							/* !defined(WIN32) && !defined(__CYGWIN__) */
	/*
	 * On Windows, we don't have anything to do here since they don't have
	 * Unix-y permissions.
	 */
	return true;
#endif
}


#endif							/* FRONTEND */
