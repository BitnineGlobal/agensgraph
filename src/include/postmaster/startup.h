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
 * startup.h
 *	  Exports from postmaster/startup.c.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 *
 * src/include/postmaster/startup.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef _STARTUP_H
#define _STARTUP_H

extern void HandleStartupProcInterrupts(void);
extern void StartupProcessMain(void) pg_attribute_noreturn();
extern void PreRestoreCommand(void);
extern void PostRestoreCommand(void);
extern bool IsPromoteSignaled(void);
extern void ResetPromoteSignaled(void);

#endif							/* _STARTUP_H */
