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
 * link-canary.c
 *	  Detect whether src/common functions came from frontend or backend.
 *
 * Copyright (c) 2018-2020, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  src/common/link-canary.c
 *
 *-------------------------------------------------------------------------
 */
#include "c.h"

#include "common/link-canary.h"

/*
 * This function just reports whether this file was compiled for frontend
 * or backend environment.  We need this because in some systems, mainly
 * ELF-based platforms, it is possible for a shlib (such as libpq) loaded
 * into the backend to call a backend function named XYZ in preference to
 * the shlib's own function XYZ.  That's bad if the two functions don't
 * act identically.  This exact situation comes up for many functions in
 * src/common and src/port, where the same function names exist in both
 * libpq and the backend but they don't act quite identically.  To verify
 * that appropriate measures have been taken to prevent incorrect symbol
 * resolution, libpq should test that this function returns true.
 */
bool
pg_link_canary_is_frontend(void)
{
#ifdef FRONTEND
	return true;
#else
	return false;
#endif
}
