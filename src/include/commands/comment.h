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
 * src/include/commands/comment.h
 *
 *-------------------------------------------------------------------------
 *
 * comment.h
 *
 * Prototypes for functions in commands/comment.c
 *
 * Copyright (c) 1999-2021, PostgreSQL Global Development Group
 *
 *-------------------------------------------------------------------------
 */

#ifndef COMMENT_H
#define COMMENT_H

#include "catalog/objectaddress.h"
#include "nodes/parsenodes.h"

/*------------------------------------------------------------------
 * Function Prototypes --
 *
 * The following prototypes define the public functions of the comment
 * related routines.  CommentObject() implements the SQL "COMMENT ON"
 * command.  DeleteComments() deletes all comments for an object.
 * CreateComments creates (or deletes, if comment is NULL) a comment
 * for a specific key.  There are versions of these two methods for
 * both normal and shared objects.
 *------------------------------------------------------------------
 */

extern ObjectAddress CommentObject(CommentStmt *stmt);

extern void DeleteComments(Oid oid, Oid classoid, int32 subid);

extern void CreateComments(Oid oid, Oid classoid, int32 subid, const char *comment);

extern void DeleteSharedComments(Oid oid, Oid classoid);

extern void CreateSharedComments(Oid oid, Oid classoid, const char *comment);

extern char *GetComment(Oid oid, Oid classoid, int32 subid);

#endif							/* COMMENT_H */
