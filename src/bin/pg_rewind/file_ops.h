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
 * file_ops.h
 *	  Helper functions for operating on files
 *
 * Copyright (c) 2013-2020, PostgreSQL Global Development Group
 *
 *-------------------------------------------------------------------------
 */
#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "filemap.h"

extern void open_target_file(const char *path, bool trunc);
extern void write_target_range(char *buf, off_t begin, size_t size);
extern void close_target_file(void);
extern void remove_target_file(const char *path, bool missing_ok);
extern void truncate_target_file(const char *path, off_t newsize);
extern void create_target(file_entry_t *t);
extern void remove_target(file_entry_t *t);

extern char *slurpFile(const char *datadir, const char *path, size_t *filesize);

#endif							/* FILE_OPS_H */
