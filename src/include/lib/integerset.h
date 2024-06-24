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
 * integerset.h
 *	  In-memory data structure to hold a large set of integers efficiently
 *
 * Portions Copyright (c) 2012-2021, PostgreSQL Global Development Group
 *
 * src/include/lib/integerset.h
 */
#ifndef INTEGERSET_H
#define INTEGERSET_H

typedef struct IntegerSet IntegerSet;

extern IntegerSet *intset_create(void);
extern void intset_add_member(IntegerSet *intset, uint64 x);
extern bool intset_is_member(IntegerSet *intset, uint64 x);

extern uint64 intset_num_entries(IntegerSet *intset);
extern uint64 intset_memory_usage(IntegerSet *intset);

extern void intset_begin_iterate(IntegerSet *intset);
extern bool intset_iterate_next(IntegerSet *intset, uint64 *next);

#endif							/* INTEGERSET_H */
