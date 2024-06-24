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

struct mytype
{
	int			id;
	char		t[64];
	double		d1;				/* dec_t */
	double		d2;
	char		c[30];
};
typedef struct mytype MYTYPE;

struct mynulltype
{
	int			id;
	int			t;
	int			d1;
	int			d2;
	int			c;
};
typedef struct mynulltype MYNULLTYPE;
