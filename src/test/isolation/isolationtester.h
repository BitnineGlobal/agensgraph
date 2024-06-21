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
 * isolationtester.h
 *	  include file for isolation tests
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		src/test/isolation/isolationtester.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef ISOLATIONTESTER_H
#define ISOLATIONTESTER_H

/*
 * The structs declared here are used in the output of specparse.y.
 * Except where noted, all fields are set in the grammar and not
 * changed thereafter.
 */
typedef struct Step Step;

typedef struct
{
	char	   *name;
	char	   *setupsql;
	char	   *teardownsql;
	Step	  **steps;
	int			nsteps;
} Session;

struct Step
{
	char	   *name;
	char	   *sql;
	/* These fields are filled by check_testspec(): */
	int			session;		/* identifies owning session */
	bool		used;			/* has step been used in a permutation? */
};

typedef enum
{
	PSB_ONCE,					/* force step to wait once */
	PSB_OTHER_STEP,				/* wait for another step to complete first */
	PSB_NUM_NOTICES				/* wait for N notices from another session */
} PermutationStepBlockerType;

typedef struct
{
	char	   *stepname;
	PermutationStepBlockerType blocktype;
	int			num_notices;	/* only used for PSB_NUM_NOTICES */
	/* These fields are filled by check_testspec(): */
	Step	   *step;			/* link to referenced step (if any) */
	/* These fields are runtime workspace: */
	int			target_notices; /* total notices needed from other session */
} PermutationStepBlocker;

typedef struct
{
	char	   *name;			/* name of referenced Step */
	PermutationStepBlocker **blockers;
	int			nblockers;
	/* These fields are filled by check_testspec(): */
	Step	   *step;			/* link to referenced Step */
} PermutationStep;

typedef struct
{
	int			nsteps;
	PermutationStep **steps;
} Permutation;

typedef struct
{
	char	  **setupsqls;
	int			nsetupsqls;
	char	   *teardownsql;
	Session   **sessions;
	int			nsessions;
	Permutation **permutations;
	int			npermutations;
} TestSpec;

extern TestSpec parseresult;

extern int	spec_yyparse(void);

extern int	spec_yylex(void);
extern void spec_yyerror(const char *str);

#endif							/* ISOLATIONTESTER_H */
