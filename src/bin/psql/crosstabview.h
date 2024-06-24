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
 * psql - the PostgreSQL interactive terminal
 *
 * Copyright (c) 2000-2021, PostgreSQL Global Development Group
 *
 * src/bin/psql/crosstabview.h
 */

#ifndef CROSSTABVIEW_H
#define CROSSTABVIEW_H

#include "libpq-fe.h"

/*
 * Limit the number of output columns generated in memory by the crosstabview
 * algorithm. A new output column is added for each distinct value found in the
 * column that pivots (to form the horizontal header).
 * The purpose of this limit is to fail early instead of over-allocating or spending
 * too much time if the crosstab to generate happens to be unreasonably large
 * (worst case: a NxN cartesian product with N=number of tuples).
 * The value of 1600 corresponds to the maximum columns per table in storage,
 * but it could be as much as INT_MAX theoretically.
 */
#define CROSSTABVIEW_MAX_COLUMNS 1600

/* prototypes */
extern bool PrintResultsInCrosstab(const PGresult *res);

#endif							/* CROSSTABVIEW_H */
