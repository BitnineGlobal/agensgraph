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

/*---------------------------------------------------------------------------
 *
 * Ryu floating-point output for double precision.
 *
 * Portions Copyright (c) 2018-2021, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *	  src/common/d2s_intrinsics.h
 *
 * This is a modification of code taken from github.com/ulfjack/ryu under the
 * terms of the Boost license (not the Apache license). The original copyright
 * notice follows:
 *
 * Copyright 2018 Ulf Adams
 *
 * The contents of this file may be used under the terms of the Apache
 * License, Version 2.0.
 *
 *     (See accompanying file LICENSE-Apache or copy at
 *      http://www.apache.org/licenses/LICENSE-2.0)
 *
 * Alternatively, the contents of this file may be used under the terms of the
 * Boost Software License, Version 1.0.
 *
 *     (See accompanying file LICENSE-Boost or copy at
 *      https://www.boost.org/LICENSE_1_0.txt)
 *
 * Unless required by applicable law or agreed to in writing, this software is
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.
 *
 *---------------------------------------------------------------------------
 */
#ifndef RYU_D2S_INTRINSICS_H
#define RYU_D2S_INTRINSICS_H

#if defined(HAS_64_BIT_INTRINSICS)

#include <intrin.h>

static inline uint64
umul128(const uint64 a, const uint64 b, uint64 *const productHi)
{
	return _umul128(a, b, productHi);
}

static inline uint64
shiftright128(const uint64 lo, const uint64 hi, const uint32 dist)
{
	/*
	 * For the __shiftright128 intrinsic, the shift value is always modulo 64.
	 * In the current implementation of the double-precision version of Ryu,
	 * the shift value is always < 64. (In the case RYU_OPTIMIZE_SIZE == 0,
	 * the shift value is in the range [49, 58]. Otherwise in the range [2,
	 * 59].) Check this here in case a future change requires larger shift
	 * values. In this case this function needs to be adjusted.
	 */
	Assert(dist < 64);
	return __shiftright128(lo, hi, (unsigned char) dist);
}

#else							/* defined(HAS_64_BIT_INTRINSICS) */

static inline uint64
umul128(const uint64 a, const uint64 b, uint64 *const productHi)
{
	/*
	 * The casts here help MSVC to avoid calls to the __allmul library
	 * function.
	 */
	const uint32 aLo = (uint32) a;
	const uint32 aHi = (uint32) (a >> 32);
	const uint32 bLo = (uint32) b;
	const uint32 bHi = (uint32) (b >> 32);

	const uint64 b00 = (uint64) aLo * bLo;
	const uint64 b01 = (uint64) aLo * bHi;
	const uint64 b10 = (uint64) aHi * bLo;
	const uint64 b11 = (uint64) aHi * bHi;

	const uint32 b00Lo = (uint32) b00;
	const uint32 b00Hi = (uint32) (b00 >> 32);

	const uint64 mid1 = b10 + b00Hi;
	const uint32 mid1Lo = (uint32) (mid1);
	const uint32 mid1Hi = (uint32) (mid1 >> 32);

	const uint64 mid2 = b01 + mid1Lo;
	const uint32 mid2Lo = (uint32) (mid2);
	const uint32 mid2Hi = (uint32) (mid2 >> 32);

	const uint64 pHi = b11 + mid1Hi + mid2Hi;
	const uint64 pLo = ((uint64) mid2Lo << 32) + b00Lo;

	*productHi = pHi;
	return pLo;
}

static inline uint64
shiftright128(const uint64 lo, const uint64 hi, const uint32 dist)
{
	/* We don't need to handle the case dist >= 64 here (see above). */
	Assert(dist < 64);
#if !defined(RYU_32_BIT_PLATFORM)
	Assert(dist > 0);
	return (hi << (64 - dist)) | (lo >> dist);
#else
	/* Avoid a 64-bit shift by taking advantage of the range of shift values. */
	Assert(dist >= 32);
	return (hi << (64 - dist)) | ((uint32) (lo >> 32) >> (dist - 32));
#endif
}

#endif							/* // defined(HAS_64_BIT_INTRINSICS) */

#ifdef RYU_32_BIT_PLATFORM

/*  Returns the high 64 bits of the 128-bit product of a and b. */
static inline uint64
umulh(const uint64 a, const uint64 b)
{
	/*
	 * Reuse the umul128 implementation. Optimizers will likely eliminate the
	 * instructions used to compute the low part of the product.
	 */
	uint64		hi;

	umul128(a, b, &hi);
	return hi;
}

/*----
 *  On 32-bit platforms, compilers typically generate calls to library
 *  functions for 64-bit divisions, even if the divisor is a constant.
 *
 *  E.g.:
 *  https://bugs.llvm.org/show_bug.cgi?id=37932
 *  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=17958
 *  https://gcc.gnu.org/bugzilla/show_bug.cgi?id=37443
 *
 *  The functions here perform division-by-constant using multiplications
 *  in the same way as 64-bit compilers would do.
 *
 *  NB:
 *  The multipliers and shift values are the ones generated by clang x64
 *  for expressions like x/5, x/10, etc.
 *----
 */

static inline uint64
div5(const uint64 x)
{
	return umulh(x, UINT64CONST(0xCCCCCCCCCCCCCCCD)) >> 2;
}

static inline uint64
div10(const uint64 x)
{
	return umulh(x, UINT64CONST(0xCCCCCCCCCCCCCCCD)) >> 3;
}

static inline uint64
div100(const uint64 x)
{
	return umulh(x >> 2, UINT64CONST(0x28F5C28F5C28F5C3)) >> 2;
}

static inline uint64
div1e8(const uint64 x)
{
	return umulh(x, UINT64CONST(0xABCC77118461CEFD)) >> 26;
}

#else							/* RYU_32_BIT_PLATFORM */

static inline uint64
div5(const uint64 x)
{
	return x / 5;
}

static inline uint64
div10(const uint64 x)
{
	return x / 10;
}

static inline uint64
div100(const uint64 x)
{
	return x / 100;
}

static inline uint64
div1e8(const uint64 x)
{
	return x / 100000000;
}

#endif							/* RYU_32_BIT_PLATFORM */

#endif							/* RYU_D2S_INTRINSICS_H */
