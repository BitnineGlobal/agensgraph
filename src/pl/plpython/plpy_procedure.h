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
 * src/pl/plpython/plpy_procedure.h
 */

#ifndef PLPY_PROCEDURE_H
#define PLPY_PROCEDURE_H

#include "plpy_typeio.h"


extern void init_procedure_caches(void);


/* saved arguments for outer recursion level or set-returning function */
typedef struct PLySavedArgs
{
	struct PLySavedArgs *next;	/* linked-list pointer */
	PyObject   *args;			/* "args" element of globals dict */
	int			nargs;			/* length of namedargs array */
	PyObject   *namedargs[FLEXIBLE_ARRAY_MEMBER];	/* named args */
} PLySavedArgs;

/* cached procedure data */
typedef struct PLyProcedure
{
	MemoryContext mcxt;			/* context holding this PLyProcedure and its
								 * subsidiary data */
	char	   *proname;		/* SQL name of procedure */
	char	   *pyname;			/* Python name of procedure */
	TransactionId fn_xmin;
	ItemPointerData fn_tid;
	bool		fn_readonly;
	bool		is_setof;		/* true, if function returns result set */
	bool		is_procedure;
	PLyObToDatum result;		/* Function result output conversion info */
	PLyDatumToOb result_in;		/* For converting input tuples in a trigger */
	char	   *src;			/* textual procedure code, after mangling */
	char	  **argnames;		/* Argument names */
	PLyDatumToOb *args;			/* Argument input conversion info */
	int			nargs;			/* Number of elements in above arrays */
	Oid			langid;			/* OID of plpython pg_language entry */
	List	   *trftypes;		/* OID list of transform types */
	PyObject   *code;			/* compiled procedure code */
	PyObject   *statics;		/* data saved across calls, local scope */
	PyObject   *globals;		/* data saved across calls, global scope */
	long		calldepth;		/* depth of recursive calls of function */
	PLySavedArgs *argstack;		/* stack of outer-level call arguments */
} PLyProcedure;

/* the procedure cache key */
typedef struct PLyProcedureKey
{
	Oid			fn_oid;			/* function OID */
	Oid			fn_rel;			/* triggered-on relation or InvalidOid */
} PLyProcedureKey;

/* the procedure cache entry */
typedef struct PLyProcedureEntry
{
	PLyProcedureKey key;		/* hash key */
	PLyProcedure *proc;
} PLyProcedureEntry;

/* PLyProcedure manipulation */
extern char *PLy_procedure_name(PLyProcedure *proc);
extern PLyProcedure *PLy_procedure_get(Oid fn_oid, Oid fn_rel, bool is_trigger);
extern void PLy_procedure_compile(PLyProcedure *proc, const char *src);
extern void PLy_procedure_delete(PLyProcedure *proc);

#endif							/* PLPY_PROCEDURE_H */
