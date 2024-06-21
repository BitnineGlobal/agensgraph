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
 * seclabel.h
 *
 * Prototypes for functions in commands/seclabel.c
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 */
#ifndef SECLABEL_H
#define SECLABEL_H

#include "catalog/objectaddress.h"

/*
 * Internal APIs
 */
extern char *GetSecurityLabel(const ObjectAddress *object,
							  const char *provider);
extern void SetSecurityLabel(const ObjectAddress *object,
							 const char *provider, const char *label);
extern void DeleteSecurityLabel(const ObjectAddress *object);
extern void DeleteSharedSecurityLabel(Oid objectId, Oid classId);

/*
 * Statement and ESP hook support
 */
extern ObjectAddress ExecSecLabelStmt(SecLabelStmt *stmt);

typedef void (*check_object_relabel_type) (const ObjectAddress *object,
										   const char *seclabel);
extern void register_label_provider(const char *provider,
									check_object_relabel_type hook);

#endif							/* SECLABEL_H */
