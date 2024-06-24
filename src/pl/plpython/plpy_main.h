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
 * src/pl/plpython/plpy_main.h
 */

#ifndef PLPY_MAIN_H
#define PLPY_MAIN_H

#include "plpy_procedure.h"

/* the interpreter's globals dict */
extern PyObject *PLy_interp_globals;

/*
 * A stack of PL/Python execution contexts. Each time user-defined Python code
 * is called, an execution context is created and put on the stack. After the
 * Python code returns, the context is destroyed.
 */
typedef struct PLyExecutionContext
{
	PLyProcedure *curr_proc;	/* the currently executing procedure */
	MemoryContext scratch_ctx;	/* a context for things like type I/O */
	struct PLyExecutionContext *next;	/* previous stack level */
} PLyExecutionContext;

/* Get the current execution context */
extern PLyExecutionContext *PLy_current_execution_context(void);

/* Get the scratch memory context for specified execution context */
extern MemoryContext PLy_get_scratch_context(PLyExecutionContext *context);

#endif							/* PLPY_MAIN_H */
