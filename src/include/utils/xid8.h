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
 * xid8.h
 *	  Header file for the "xid8" ADT.
 *
 * Copyright (c) 2020, PostgreSQL Global Development Group
 *
 * src/include/utils/xid8.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef XID8_H
#define XID8_H

#include "access/transam.h"

#define DatumGetFullTransactionId(X) (FullTransactionIdFromU64(DatumGetUInt64(X)))
#define FullTransactionIdGetDatum(X) (UInt64GetDatum(U64FromFullTransactionId(X)))
#define PG_GETARG_FULLTRANSACTIONID(X) DatumGetFullTransactionId(PG_GETARG_DATUM(X))
#define PG_RETURN_FULLTRANSACTIONID(X) return FullTransactionIdGetDatum(X)

#endif							/* XID8_H */
