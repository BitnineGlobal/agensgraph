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

/*------------------------------------------------------------------------
 *
 * xlogarchive.h
 *		Prototypes for WAL archives in the backend
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		src/include/access/xlogarchive.h
 *
 *------------------------------------------------------------------------
 */

#ifndef XLOG_ARCHIVE_H
#define XLOG_ARCHIVE_H

#include "access/xlogdefs.h"

extern bool RestoreArchivedFile(char *path, const char *xlogfname,
								const char *recovername, off_t expectedSize,
								bool cleanupEnabled);
extern void ExecuteRecoveryCommand(const char *command, const char *commandName,
								   bool failOnSignal);
extern void KeepFileRestoredFromArchive(const char *path, const char *xlogfname);
extern void XLogArchiveNotify(const char *xlog);
extern void XLogArchiveNotifySeg(XLogSegNo segno);
extern void XLogArchiveForceDone(const char *xlog);
extern bool XLogArchiveCheckDone(const char *xlog);
extern bool XLogArchiveIsBusy(const char *xlog);
extern bool XLogArchiveIsReady(const char *xlog);
extern bool XLogArchiveIsReadyOrDone(const char *xlog);
extern void XLogArchiveCleanup(const char *xlog);

#endif							/* XLOG_ARCHIVE_H */
