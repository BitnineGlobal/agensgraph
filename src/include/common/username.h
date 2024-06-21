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
 *	username.h
 *		lookup effective username
 *
 *	Copyright (c) 2003-2020, PostgreSQL Global Development Group
 *
 *	src/include/common/username.h
 */
#ifndef USERNAME_H
#define USERNAME_H

extern const char *get_user_name(char **errstr);
extern const char *get_user_name_or_exit(const char *progname);

#endif							/* USERNAME_H */
