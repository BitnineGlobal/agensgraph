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

/*------------------------------------------------------------------------
*
* geqo_mutation.c
*
*	 TSP mutation routines
*
* src/backend/optimizer/geqo/geqo_mutation.c
*
*-------------------------------------------------------------------------
*/

/* contributed by:
   =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
   *  Martin Utesch				 * Institute of Automatic Control	   *
   =							 = University of Mining and Technology =
   *  utesch@aut.tu-freiberg.de  * Freiberg, Germany				   *
   =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=
 */

/* this is adopted from Genitor : */
/*************************************************************/
/*															 */
/*	Copyright (c) 1990										 */
/*	Darrell L. Whitley										 */
/*	Computer Science Department								 */
/*	Colorado State University								 */
/*															 */
/*	Permission is hereby granted to copy all or any part of  */
/*	this program for free distribution.   The author's name  */
/*	and this copyright notice must be included in any copy.  */
/*															 */
/*************************************************************/

#include "postgres.h"
#include "optimizer/geqo_mutation.h"
#include "optimizer/geqo_random.h"

#if defined(CX)					/* currently used only in CX mode */

void
geqo_mutation(PlannerInfo *root, Gene *tour, int num_gene)
{
	int			swap1;
	int			swap2;
	int			num_swaps = geqo_randint(root, num_gene / 3, 0);
	Gene		temp;


	while (num_swaps > 0)
	{
		swap1 = geqo_randint(root, num_gene - 1, 0);
		swap2 = geqo_randint(root, num_gene - 1, 0);

		while (swap1 == swap2)
			swap2 = geqo_randint(root, num_gene - 1, 0);

		temp = tour[swap1];
		tour[swap1] = tour[swap2];
		tour[swap2] = temp;


		num_swaps -= 1;
	}
}

#endif							/* defined(CX) */
