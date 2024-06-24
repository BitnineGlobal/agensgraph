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
 * src/pl/plpython/plpy_planobject.h
 */

#ifndef PLPY_PLANOBJECT_H
#define PLPY_PLANOBJECT_H

#include "executor/spi.h"
#include "plpy_typeio.h"


typedef struct PLyPlanObject
{
	PyObject_HEAD
	SPIPlanPtr	plan;
	int			nargs;
	Oid		   *types;
	Datum	   *values;
	PLyObToDatum *args;
	MemoryContext mcxt;
} PLyPlanObject;

extern void PLy_plan_init_type(void);
extern PyObject *PLy_plan_new(void);
extern bool is_PLyPlanObject(PyObject *ob);

#endif							/* PLPY_PLANOBJECT_H */
