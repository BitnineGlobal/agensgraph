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
 * crypt.h
 *	  Interface to libpq/crypt.c
 *
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/libpq/crypt.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PG_CRYPT_H
#define PG_CRYPT_H

#include "datatype/timestamp.h"

/*
 * Types of password hashes or secrets.
 *
 * Plaintext passwords can be passed in by the user, in a CREATE/ALTER USER
 * command. They will be encrypted to MD5 or SCRAM-SHA-256 format, before
 * storing on-disk, so only MD5 and SCRAM-SHA-256 passwords should appear
 * in pg_authid.rolpassword. They are also the allowed values for the
 * password_encryption GUC.
 */
typedef enum PasswordType
{
	PASSWORD_TYPE_PLAINTEXT = 0,
	PASSWORD_TYPE_MD5,
	PASSWORD_TYPE_SCRAM_SHA_256
} PasswordType;

extern PasswordType get_password_type(const char *shadow_pass);
extern char *encrypt_password(PasswordType target_type, const char *role,
							  const char *password);

extern char *get_role_password(const char *role, char **logdetail);

extern int	md5_crypt_verify(const char *role, const char *shadow_pass,
							 const char *client_pass, const char *md5_salt,
							 int md5_salt_len, char **logdetail);
extern int	plain_crypt_verify(const char *role, const char *shadow_pass,
							   const char *client_pass, char **logdetail);

#endif
