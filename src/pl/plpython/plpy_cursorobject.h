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
 * src/pl/plpython/plpy_cursorobject.h
 */

#ifndef PLPY_CURSOROBJECT_H
#define PLPY_CURSOROBJECT_H

#include "plpy_typeio.h"


typedef struct PLyCursorObject
{
	PyObject_HEAD
	char	   *portalname;
	PLyDatumToOb result;
	bool		closed;
	MemoryContext mcxt;
} PLyCursorObject;

extern void PLy_cursor_init_type(void);
extern PyObject *PLy_cursor(PyObject *self, PyObject *args);
extern PyObject *PLy_cursor_plan(PyObject *ob, PyObject *args);

#endif							/* PLPY_CURSOROBJECT_H */
