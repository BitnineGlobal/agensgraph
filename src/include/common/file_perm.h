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
 * File and directory permission definitions
 *
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/common/file_perm.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef FILE_PERM_H
#define FILE_PERM_H

#include <sys/stat.h>

/*
 * Mode mask for data directory permissions that only allows the owner to
 * read/write directories and files.
 *
 * This is the default.
 */
#define PG_MODE_MASK_OWNER		    (S_IRWXG | S_IRWXO)

/*
 * Mode mask for data directory permissions that also allows group read/execute.
 */
#define PG_MODE_MASK_GROUP			(S_IWGRP | S_IRWXO)

/* Default mode for creating directories */
#define PG_DIR_MODE_OWNER			S_IRWXU

/* Mode for creating directories that allows group read/execute */
#define PG_DIR_MODE_GROUP			(S_IRWXU | S_IRGRP | S_IXGRP)

/* Default mode for creating files */
#define PG_FILE_MODE_OWNER		    (S_IRUSR | S_IWUSR)

/* Mode for creating files that allows group read */
#define PG_FILE_MODE_GROUP			(S_IRUSR | S_IWUSR | S_IRGRP)

/* Modes for creating directories and files in the data directory */
extern int	pg_dir_create_mode;
extern int	pg_file_create_mode;

/* Mode mask to pass to umask() */
extern int	pg_mode_mask;

/* Set permissions and mask based on the provided mode */
extern void SetDataDirectoryCreatePerm(int dataDirMode);

/* Set permissions and mask based on the mode of the data directory */
extern bool GetDataDirectoryCreatePerm(const char *dataDir);

#endif							/* FILE_PERM_H */
