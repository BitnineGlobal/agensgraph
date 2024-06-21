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
 * arch-ia64.h
 *	  Atomic operations considerations specific to intel itanium
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * NOTES:
 *
 * src/include/port/atomics/arch-ia64.h
 *
 *-------------------------------------------------------------------------
 */

/*
 * Itanium is weakly ordered, so read and write barriers require a full
 * fence.
 */
#if defined(__INTEL_COMPILER)
#	define pg_memory_barrier_impl()		__mf()
#elif defined(__GNUC__)
#	define pg_memory_barrier_impl()		__asm__ __volatile__ ("mf" : : : "memory")
#elif defined(__hpux)
#	define pg_memory_barrier_impl()		_Asm_mf()
#endif

/* per architecture manual doubleword accesses have single copy atomicity */
#define PG_HAVE_8BYTE_SINGLE_COPY_ATOMICITY
