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
 * dummy_seclabel.c
 *
 * Dummy security label provider.
 *
 * This module does not provide anything worthwhile from a security
 * perspective, but allows regression testing independent of platform-specific
 * features like SELinux.
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 */
#include "postgres.h"

#include "commands/seclabel.h"
#include "fmgr.h"
#include "miscadmin.h"
#include "utils/rel.h"

PG_MODULE_MAGIC;

/* Entrypoint of the module */
void		_PG_init(void);

PG_FUNCTION_INFO_V1(dummy_seclabel_dummy);

static void
dummy_object_relabel(const ObjectAddress *object, const char *seclabel)
{
	if (seclabel == NULL ||
		strcmp(seclabel, "unclassified") == 0 ||
		strcmp(seclabel, "classified") == 0)
		return;

	if (strcmp(seclabel, "secret") == 0 ||
		strcmp(seclabel, "top secret") == 0)
	{
		if (!superuser())
			ereport(ERROR,
					(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
					 errmsg("only superuser can set '%s' label", seclabel)));
		return;
	}
	ereport(ERROR,
			(errcode(ERRCODE_INVALID_NAME),
			 errmsg("'%s' is not a valid security label", seclabel)));
}

void
_PG_init(void)
{
	register_label_provider("dummy", dummy_object_relabel);
}

/*
 * This function is here just so that the extension is not completely empty
 * and the dynamic library is loaded when CREATE EXTENSION runs.
 */
Datum
dummy_seclabel_dummy(PG_FUNCTION_ARGS)
{
	PG_RETURN_VOID();
}
