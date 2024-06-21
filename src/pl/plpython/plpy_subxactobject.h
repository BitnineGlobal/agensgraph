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
 * src/pl/plpython/plpy_subxactobject.h
 */

#ifndef PLPY_SUBXACTOBJECT
#define PLPY_SUBXACTOBJECT

#include "nodes/pg_list.h"
#include "plpython.h"
#include "utils/resowner.h"

/* a list of nested explicit subtransactions */
extern List *explicit_subtransactions;


typedef struct PLySubtransactionObject
{
	PyObject_HEAD
	bool		started;
	bool		exited;
} PLySubtransactionObject;

/* explicit subtransaction data */
typedef struct PLySubtransactionData
{
	MemoryContext oldcontext;
	ResourceOwner oldowner;
} PLySubtransactionData;

extern void PLy_subtransaction_init_type(void);
extern PyObject *PLy_subtransaction_new(PyObject *self, PyObject *unused);

#endif							/* PLPY_SUBXACTOBJECT */
