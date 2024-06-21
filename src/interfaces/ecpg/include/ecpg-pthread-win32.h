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

/* src/interfaces/ecpg/include/ecpg-pthread-win32.h */
/*
 * pthread mapping macros for win32 native thread implementation
 */
#ifndef _ECPG_PTHREAD_WIN32_H
#define _ECPG_PTHREAD_WIN32_H

#ifdef ENABLE_THREAD_SAFETY

#ifndef WIN32

#include <pthread.h>
#else

typedef struct pthread_mutex_t
{
	HANDLE		handle;
	LONG		initlock;
} pthread_mutex_t;

typedef DWORD pthread_key_t;
typedef bool pthread_once_t;

#define PTHREAD_MUTEX_INITIALIZER	{ NULL, 0 }
#define PTHREAD_ONCE_INIT			false

void		win32_pthread_mutex(volatile pthread_mutex_t *mutex);
void		win32_pthread_once(volatile pthread_once_t *once, void (*fn) (void));

#define pthread_mutex_lock(mutex) \
	do { \
		if ((mutex)->handle == NULL) \
			win32_pthread_mutex((mutex)); \
		WaitForSingleObject((mutex)->handle, INFINITE); \
	} while(0)

#define pthread_mutex_unlock(mutex) \
	ReleaseMutex((mutex)->handle)

#define pthread_getspecific(key) \
	TlsGetValue((key))

#define pthread_setspecific(key, value) \
	TlsSetValue((key), (value))

/* FIXME: destructor is never called in Win32. */
#define pthread_key_create(key, destructor) \
	do { *(key) = TlsAlloc(); ((void)(destructor)); } while(0)

#define pthread_once(once, fn) \
	do { \
		if (!*(once)) \
			win32_pthread_once((once), (fn)); \
	} while(0)
#endif							/* WIN32 */
#endif							/* ENABLE_THREAD_SAFETY */

#endif							/* _ECPG_PTHREAD_WIN32_H */
