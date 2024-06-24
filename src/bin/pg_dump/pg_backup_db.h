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
 *	Definitions for pg_backup_db.c
 *
 *	IDENTIFICATION
 *		src/bin/pg_dump/pg_backup_db.h
 */

#ifndef PG_BACKUP_DB_H
#define PG_BACKUP_DB_H

#include "pg_backup.h"


extern int	ExecuteSqlCommandBuf(Archive *AHX, const char *buf, size_t bufLen);

extern void ExecuteSqlStatement(Archive *AHX, const char *query);
extern PGresult *ExecuteSqlQuery(Archive *AHX, const char *query,
								 ExecStatusType status);
extern PGresult *ExecuteSqlQueryForSingleRow(Archive *fout, const char *query);

extern void EndDBCopyMode(Archive *AHX, const char *tocEntryTag);

extern void StartTransaction(Archive *AHX);
extern void CommitTransaction(Archive *AHX);

#endif
