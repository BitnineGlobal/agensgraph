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
 * twophase_rmgr.h
 *	  Two-phase-commit resource managers definition
 *
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/twophase_rmgr.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef TWOPHASE_RMGR_H
#define TWOPHASE_RMGR_H

typedef void (*TwoPhaseCallback) (TransactionId xid, uint16 info,
								  void *recdata, uint32 len);
typedef uint8 TwoPhaseRmgrId;

/*
 * Built-in resource managers
 */
#define TWOPHASE_RM_END_ID			0
#define TWOPHASE_RM_LOCK_ID			1
#define TWOPHASE_RM_PGSTAT_ID		2
#define TWOPHASE_RM_MULTIXACT_ID	3
#define TWOPHASE_RM_PREDICATELOCK_ID	4
#define TWOPHASE_RM_MAX_ID			TWOPHASE_RM_PREDICATELOCK_ID

extern const TwoPhaseCallback twophase_recover_callbacks[];
extern const TwoPhaseCallback twophase_postcommit_callbacks[];
extern const TwoPhaseCallback twophase_postabort_callbacks[];
extern const TwoPhaseCallback twophase_standby_recover_callbacks[];


extern void RegisterTwoPhaseRecord(TwoPhaseRmgrId rmid, uint16 info,
								   const void *data, uint32 len);

#endif							/* TWOPHASE_RMGR_H */
