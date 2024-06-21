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

/*
 * Headers for port/dirent.c, win32 native implementation of dirent functions
 *
 * src/include/port/win32_msvc/dirent.h
 */

#ifndef _WIN32VC_DIRENT_H
#define _WIN32VC_DIRENT_H
struct dirent
{
	long		d_ino;
	unsigned short d_reclen;
	unsigned char d_type;
	unsigned short d_namlen;
	char		d_name[MAX_PATH];
};

typedef struct DIR DIR;

DIR		   *opendir(const char *);
struct dirent *readdir(DIR *);
int			closedir(DIR *);

/* File types for 'd_type'.  */
#define DT_UNKNOWN		0
#define DT_FIFO		1
#define DT_CHR			2
#define DT_DIR			4
#define DT_BLK			6
#define DT_REG			8
#define DT_LNK			10
#define DT_SOCK		12
#define DT_WHT			14
#endif
