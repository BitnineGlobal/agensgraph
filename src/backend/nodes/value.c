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
 * value.c
 *	  implementation of Value nodes
 *
 *
 * Copyright (c) 2003-2020, PostgreSQL Global Development Group
 *
 *
 * IDENTIFICATION
 *	  src/backend/nodes/value.c
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include "nodes/parsenodes.h"

/*
 *	makeInteger
 */
Value *
makeInteger(int i)
{
	Value	   *v = makeNode(Value);

	v->type = T_Integer;
	v->val.ival = i;
	return v;
}

/*
 *	makeFloat
 *
 * Caller is responsible for passing a palloc'd string.
 */
Value *
makeFloat(char *numericStr)
{
	Value	   *v = makeNode(Value);

	v->type = T_Float;
	v->val.str = numericStr;
	return v;
}

/*
 *	makeString
 *
 * Caller is responsible for passing a palloc'd string.
 */
Value *
makeString(char *str)
{
	Value	   *v = makeNode(Value);

	v->type = T_String;
	v->val.str = str;
	return v;
}

/*
 *	makeBitString
 *
 * Caller is responsible for passing a palloc'd string.
 */
Value *
makeBitString(char *str)
{
	Value	   *v = makeNode(Value);

	v->type = T_BitString;
	v->val.str = str;
	return v;
}
