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
 * logicallauncher.h
 *	  Exports for logical replication launcher.
 *
 * Portions Copyright (c) 2016-2020, PostgreSQL Global Development Group
 *
 * src/include/replication/logicallauncher.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef LOGICALLAUNCHER_H
#define LOGICALLAUNCHER_H

extern int	max_logical_replication_workers;
extern int	max_sync_workers_per_subscription;

extern void ApplyLauncherRegister(void);
extern void ApplyLauncherMain(Datum main_arg);

extern Size ApplyLauncherShmemSize(void);
extern void ApplyLauncherShmemInit(void);

extern void ApplyLauncherWakeupAtCommit(void);
extern bool XactManipulatesLogicalReplicationWorkers(void);
extern void AtEOXact_ApplyLauncher(bool isCommit);
extern void AtEOSubXact_ApplyLauncher(bool isCommit, int nestDepth);

extern bool IsLogicalLauncher(void);

#endif							/* LOGICALLAUNCHER_H */
