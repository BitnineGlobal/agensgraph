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
 * src/bin/pg_dump/pg_backup_tar.h
 *
 * TAR Header (see "ustar interchange format" in POSIX 1003.1)
 *
 * Offset	Length	 Contents
 *	 0	  100 bytes  File name ('\0' terminated, 99 maximum length)
 * 100		8 bytes  File mode (in octal ascii)
 * 108		8 bytes  User ID (in octal ascii)
 * 116		8 bytes  Group ID (in octal ascii)
 * 124	   12 bytes  File size (in octal ascii)
 * 136	   12 bytes  Modify time (Unix timestamp in octal ascii)
 * 148		8 bytes  Header checksum (in octal ascii)
 * 156		1 bytes  Type flag (see below)
 * 157	  100 bytes  Linkname, if symlink ('\0' terminated, 99 maximum length)
 * 257		6 bytes  Magic ("ustar\0")
 * 263		2 bytes  Version ("00")
 * 265	   32 bytes  User name ('\0' terminated, 31 maximum length)
 * 297	   32 bytes  Group name ('\0' terminated, 31 maximum length)
 * 329		8 bytes  Major device ID (in octal ascii)
 * 337		8 bytes  Minor device ID (in octal ascii)
 * 345	  155 bytes  File name prefix (not used in our implementation)
 * 500	   12 bytes  Padding
 *
 * 512	 (s+p)bytes  File contents, padded out to 512-byte boundary
 */

/* The type flag defines the type of file */
#define  LF_OLDNORMAL '\0'		/* Normal disk file, Unix compatible */
#define  LF_NORMAL	  '0'		/* Normal disk file */
#define  LF_LINK	  '1'		/* Link to previously dumped file */
#define  LF_SYMLINK   '2'		/* Symbolic link */
#define  LF_CHR		  '3'		/* Character special file */
#define  LF_BLK		  '4'		/* Block special file */
#define  LF_DIR		  '5'		/* Directory */
#define  LF_FIFO	  '6'		/* FIFO special file */
#define  LF_CONTIG	  '7'		/* Contiguous file */
