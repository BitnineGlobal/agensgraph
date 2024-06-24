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
 * print_double(x) has the same effect as printf("%g", x), but is intended
 * to produce the same formatting across all platforms.
 */
static void
print_double(double x)
{
#ifdef WIN32
	/* Change Windows' 3-digit exponents to look like everyone else's */
	char		convert[128];
	int			vallen;

	sprintf(convert, "%g", x);
	vallen = strlen(convert);

	if (vallen >= 6 &&
		convert[vallen - 5] == 'e' &&
		convert[vallen - 3] == '0')
	{
		convert[vallen - 3] = convert[vallen - 2];
		convert[vallen - 2] = convert[vallen - 1];
		convert[vallen - 1] = '\0';
	}

	printf("%s", convert);
#else
	printf("%g", x);
#endif
}
