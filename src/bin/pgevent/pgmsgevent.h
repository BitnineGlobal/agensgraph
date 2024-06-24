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

/* src/bin/pgevent/pgmsgevent.h */

/*	*/
/*	 Values are 32 bit values laid out as follows: */
/*	*/
/*	  3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1 */
/*	  1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 */
/*	 +---+-+-+-----------------------+-------------------------------+ */
/*	 |Sev|C|R|	   Facility			 |				 Code			 | */
/*	 +---+-+-+-----------------------+-------------------------------+ */
/*	*/
/*	 where */
/*	*/
/*		 Sev - is the severity code */
/*	*/
/*			 00 - Success */
/*			 01 - Informational */
/*			 10 - Warning */
/*			 11 - Error */
/*	*/
/*		 C - is the Customer code flag */
/*	*/
/*		 R - is a reserved bit */
/*	*/
/*		 Facility - is the facility code */
/*	*/
/*		 Code - is the facility's status code */
/*	*/
/*	*/
/*	Define the facility codes */
/*	*/


/*	*/
/*	Define the severity codes */
/*	*/


/*	*/
/*	MessageId: PGWIN32_EVENTLOG_MSG */
/*	*/
/*	MessageText: */
/*	*/
/*	 %1 */
/*	*/
#define PGWIN32_EVENTLOG_MSG			 0x00000000L
