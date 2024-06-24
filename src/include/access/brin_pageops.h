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
 * brin_pageops.h
 *		Prototypes for operating on BRIN pages.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *	  src/include/access/brin_pageops.h
 */
#ifndef BRIN_PAGEOPS_H
#define BRIN_PAGEOPS_H

#include "access/brin_revmap.h"

extern bool brin_doupdate(Relation idxrel, BlockNumber pagesPerRange,
						  BrinRevmap *revmap, BlockNumber heapBlk,
						  Buffer oldbuf, OffsetNumber oldoff,
						  const BrinTuple *origtup, Size origsz,
						  const BrinTuple *newtup, Size newsz,
						  bool samepage);
extern bool brin_can_do_samepage_update(Buffer buffer, Size origsz,
										Size newsz);
extern OffsetNumber brin_doinsert(Relation idxrel, BlockNumber pagesPerRange,
								  BrinRevmap *revmap, Buffer *buffer, BlockNumber heapBlk,
								  BrinTuple *tup, Size itemsz);

extern void brin_page_init(Page page, uint16 type);
extern void brin_metapage_init(Page page, BlockNumber pagesPerRange,
							   uint16 version);

extern bool brin_start_evacuating_page(Relation idxRel, Buffer buf);
extern void brin_evacuate_page(Relation idxRel, BlockNumber pagesPerRange,
							   BrinRevmap *revmap, Buffer buf);

extern void brin_page_cleanup(Relation idxrel, Buffer buf);

#endif							/* BRIN_PAGEOPS_H */
