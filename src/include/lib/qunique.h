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
 * qunique.h
 *		inline array unique functions
 * Portions Copyright (c) 2019-2021, PostgreSQL Global Development Group
 *
 * IDENTIFICATION
 *		src/include/lib/qunique.h
 *-------------------------------------------------------------------------
 */

#ifndef QUNIQUE_H
#define QUNIQUE_H

/*
 * Remove duplicates from a pre-sorted array, according to a user-supplied
 * comparator.  Usually the array should have been sorted with qsort() using
 * the same arguments.  Return the new size.
 */
static inline size_t
qunique(void *array, size_t elements, size_t width,
		int (*compare) (const void *, const void *))
{
	char	   *bytes = (char *) array;
	size_t		i,
				j;

	if (elements <= 1)
		return elements;

	for (i = 1, j = 0; i < elements; ++i)
	{
		if (compare(bytes + i * width, bytes + j * width) != 0 &&
			++j != i)
			memcpy(bytes + j * width, bytes + i * width, width);
	}

	return j + 1;
}

/*
 * Like qunique(), but takes a comparator with an extra user data argument
 * which is passed through, for compatibility with qsort_arg().
 */
static inline size_t
qunique_arg(void *array, size_t elements, size_t width,
			int (*compare) (const void *, const void *, void *),
			void *arg)
{
	char	   *bytes = (char *) array;
	size_t		i,
				j;

	if (elements <= 1)
		return elements;

	for (i = 1, j = 0; i < elements; ++i)
	{
		if (compare(bytes + i * width, bytes + j * width, arg) != 0 &&
			++j != i)
			memcpy(bytes + j * width, bytes + i * width, width);
	}

	return j + 1;
}

#endif							/* QUNIQUE_H */
