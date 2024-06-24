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
 * Generator for recovery configuration
 *
 * Portions Copyright (c) 2011-2021, PostgreSQL Global Development Group
 *
 * src/include/fe_utils/recovery_gen.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef RECOVERY_GEN_H
#define RECOVERY_GEN_H

#include "libpq-fe.h"
#include "pqexpbuffer.h"

/*
 * recovery configuration is part of postgresql.conf in version 12 and up, and
 * in recovery.conf before that.
 */
#define MINIMUM_VERSION_FOR_RECOVERY_GUC 120000

extern PQExpBuffer GenerateRecoveryConfig(PGconn *pgconn,
										  char *pg_replication_slot);
extern void WriteRecoveryConfig(PGconn *pgconn, char *target_dir,
								PQExpBuffer contents);

#endif							/* RECOVERY_GEN_H */
