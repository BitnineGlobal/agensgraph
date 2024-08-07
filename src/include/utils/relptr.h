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
 * relptr.h
 *	  This file contains basic declarations for relative pointers.
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/utils/relptr.h
 *
 *-------------------------------------------------------------------------
 */

#ifndef RELPTR_H
#define RELPTR_H

/*
 * Relative pointers are intended to be used when storing an address that may
 * be relative either to the base of the process's address space or some
 * dynamic shared memory segment mapped therein.
 *
 * The idea here is that you declare a relative pointer as relptr(type)
 * and then use relptr_access to dereference it and relptr_store to change
 * it.  The use of a union here is a hack, because what's stored in the
 * relptr is always a Size, never an actual pointer.  But including a pointer
 * in the union allows us to use stupid macro tricks to provide some measure
 * of type-safety.
 */
#define relptr(type)	 union { type *relptr_type; Size relptr_off; }

/*
 * pgindent gets confused by declarations that use "relptr(type)" directly,
 * so preferred style is to write
 *		typedef struct ... SomeStruct;
 *		relptr_declare(SomeStruct, RelptrSomeStruct);
 * and then declare pointer variables as "RelptrSomeStruct someptr".
 */
#define relptr_declare(type, relptrtype) \
	typedef relptr(type) relptrtype

#ifdef HAVE__BUILTIN_TYPES_COMPATIBLE_P
#define relptr_access(base, rp) \
	(AssertVariableIsOfTypeMacro(base, char *), \
	 (__typeof__((rp).relptr_type)) ((rp).relptr_off == 0 ? NULL : \
		(base + (rp).relptr_off)))
#else
/*
 * If we don't have __builtin_types_compatible_p, assume we might not have
 * __typeof__ either.
 */
#define relptr_access(base, rp) \
	(AssertVariableIsOfTypeMacro(base, char *), \
	 (void *) ((rp).relptr_off == 0 ? NULL : (base + (rp).relptr_off)))
#endif

#define relptr_is_null(rp) \
	((rp).relptr_off == 0)

/* We use this inline to avoid double eval of "val" in relptr_store */
static inline Size
relptr_store_eval(char *base, char *val)
{
	if (val == NULL)
		return 0;
	else
	{
		Assert(val > base);
		return val - base;
	}
}

#ifdef HAVE__BUILTIN_TYPES_COMPATIBLE_P
#define relptr_store(base, rp, val) \
	(AssertVariableIsOfTypeMacro(base, char *), \
	 AssertVariableIsOfTypeMacro(val, __typeof__((rp).relptr_type)), \
	 (rp).relptr_off = relptr_store_eval(base, (char *) (val)))
#else
/*
 * If we don't have __builtin_types_compatible_p, assume we might not have
 * __typeof__ either.
 */
#define relptr_store(base, rp, val) \
	(AssertVariableIsOfTypeMacro(base, char *), \
	 (rp).relptr_off = relptr_store_eval(base, (char *) (val)))
#endif

#define relptr_copy(rp1, rp2) \
	((rp1).relptr_off = (rp2).relptr_off)

#endif							/* RELPTR_H */
