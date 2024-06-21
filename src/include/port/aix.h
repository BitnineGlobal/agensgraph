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
 * src/include/port/aix.h
 */
#define CLASS_CONFLICT
#define DISABLE_XOPEN_NLS

/*
 * "IBM XL C/C++ for AIX, V12.1" miscompiles, for 32-bit, some inline
 * expansions of ginCompareItemPointers() "long long" arithmetic.  To take
 * advantage of inlining, build a 64-bit PostgreSQL.
 */
#if defined(__ILP32__) && defined(__IBMC__)
#define PG_FORCE_DISABLE_INLINE
#endif
