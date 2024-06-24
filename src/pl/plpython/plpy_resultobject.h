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
 * src/pl/plpython/plpy_resultobject.h
 */

#ifndef PLPY_RESULTOBJECT_H
#define PLPY_RESULTOBJECT_H

#include "access/tupdesc.h"

#include "plpython.h"


typedef struct PLyResultObject
{
	PyObject_HEAD
	/* HeapTuple *tuples; */
	PyObject   *nrows;			/* number of rows returned by query */
	PyObject   *rows;			/* data rows, or empty list if no data
								 * returned */
	PyObject   *status;			/* query status, SPI_OK_*, or SPI_ERR_* */
	TupleDesc	tupdesc;
} PLyResultObject;

extern void PLy_result_init_type(void);
extern PyObject *PLy_result_new(void);

#endif							/* PLPY_RESULTOBJECT_H */
