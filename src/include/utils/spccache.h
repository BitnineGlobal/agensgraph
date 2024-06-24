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
 * spccache.h
 *	  Tablespace cache.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/spccache.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef SPCCACHE_H
#define SPCCACHE_H

void		get_tablespace_page_costs(Oid spcid, float8 *spc_random_page_cost,
									  float8 *spc_seq_page_cost);
int			get_tablespace_io_concurrency(Oid spcid);
int			get_tablespace_maintenance_io_concurrency(Oid spcid);

#endif							/* SPCCACHE_H */
