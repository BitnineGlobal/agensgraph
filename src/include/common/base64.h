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
 * base64.h
 *	  Encoding and decoding routines for base64 without whitespace
 *	  support.
 *
 * Portions Copyright (c) 2001-2020, PostgreSQL Global Development Group
 *
 * src/include/common/base64.h
 */
#ifndef BASE64_H
#define BASE64_H

/* base 64 */
extern int	pg_b64_encode(const char *src, int len, char *dst, int dstlen);
extern int	pg_b64_decode(const char *src, int len, char *dst, int dstlen);
extern int	pg_b64_enc_len(int srclen);
extern int	pg_b64_dec_len(int srclen);

#endif							/* BASE64_H */
