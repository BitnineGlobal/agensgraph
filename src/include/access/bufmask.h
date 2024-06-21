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
 * bufmask.h
 *	  Definitions for buffer masking routines, used to mask certain bits
 *	  in a page which can be different when the WAL is generated
 *	  and when the WAL is applied. This is really the job of each
 *	  individual rmgr, but we make things easier by providing some
 *	  common routines to handle cases which occur in multiple rmgrs.
 *
 * Portions Copyright (c) 2016-2021, PostgreSQL Global Development Group
 *
 * src/include/access/bufmask.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef BUFMASK_H
#define BUFMASK_H

#include "storage/block.h"
#include "storage/bufmgr.h"

/* Marker used to mask pages consistently */
#define MASK_MARKER		0

extern void mask_page_lsn_and_checksum(Page page);
extern void mask_page_hint_bits(Page page);
extern void mask_unused_space(Page page);
extern void mask_lp_flags(Page page);
extern void mask_page_content(Page page);

#endif
