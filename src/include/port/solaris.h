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

/* src/include/port/solaris.h */

/*
 * Sort this out for all operating systems some time.  The __xxx
 * symbols are defined on both GCC and Solaris CC, although GCC
 * doesn't document them.  The __xxx__ symbols are only on GCC.
 */
#if defined(__i386) && !defined(__i386__)
#define __i386__
#endif

#if defined(__amd64) && !defined(__amd64__)
#define __amd64__
#endif

#if defined(__x86_64) && !defined(__x86_64__)
#define __x86_64__
#endif

#if defined(__sparc) && !defined(__sparc__)
#define __sparc__
#endif

#if defined(__i386__)
#include <sys/isa_defs.h>
#endif

/*
 * Many versions of Solaris have broken strtod() --- see bug #4751182.
 * This has been fixed in current versions of Solaris:
 *
 * http://sunsolve.sun.com/search/document.do?assetkey=1-21-108993-62-1&searchclause=108993-62
 * http://sunsolve.sun.com/search/document.do?assetkey=1-21-112874-34-1&searchclause=112874-34
 *
 * However, many people might not have patched versions, so
 * still use our own fix for the buggy version.
 */
#define HAVE_BUGGY_SOLARIS_STRTOD
