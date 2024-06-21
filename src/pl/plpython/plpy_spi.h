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
 * src/pl/plpython/plpy_spi.h
 */

#ifndef PLPY_SPI_H
#define PLPY_SPI_H

#include "plpython.h"
#include "utils/resowner.h"

extern PyObject *PLy_spi_prepare(PyObject *self, PyObject *args);
extern PyObject *PLy_spi_execute(PyObject *self, PyObject *args);
extern PyObject *PLy_spi_execute_plan(PyObject *ob, PyObject *list, long limit);

extern PyObject *PLy_commit(PyObject *self, PyObject *args);
extern PyObject *PLy_rollback(PyObject *self, PyObject *args);

typedef struct PLyExceptionEntry
{
	int			sqlstate;		/* hash key, must be first */
	PyObject   *exc;			/* corresponding exception */
} PLyExceptionEntry;

/* handling of SPI operations inside subtransactions */
extern void PLy_spi_subtransaction_begin(MemoryContext oldcontext, ResourceOwner oldowner);
extern void PLy_spi_subtransaction_commit(MemoryContext oldcontext, ResourceOwner oldowner);
extern void PLy_spi_subtransaction_abort(MemoryContext oldcontext, ResourceOwner oldowner);

#endif							/* PLPY_SPI_H */
