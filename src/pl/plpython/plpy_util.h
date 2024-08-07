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

/*--------------------------
 * common utility functions
 *--------------------------
 */

#ifndef PLPY_UTIL_H
#define PLPY_UTIL_H

#include "plpython.h"

extern PyObject *PLyUnicode_Bytes(PyObject *unicode);
extern char *PLyUnicode_AsString(PyObject *unicode);

#if PY_MAJOR_VERSION >= 3
extern PyObject *PLyUnicode_FromString(const char *s);
extern PyObject *PLyUnicode_FromStringAndSize(const char *s, Py_ssize_t size);
#endif

#endif							/* PLPY_UTIL_H */
