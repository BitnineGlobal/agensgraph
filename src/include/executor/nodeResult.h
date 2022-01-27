/*-------------------------------------------------------------------------
 *
 * nodeResult.h
 *
 *
 *
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/executor/nodeResult.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef NODERESULT_H
#define NODERESULT_H

#include "nodes/execnodes.h"

extern ResultState *ExecInitResult(Result *node, EState *estate, int eflags);
extern void ExecEndResult(ResultState *node);
extern void ExecResultMarkPos(ResultState *node);
extern void ExecResultRestrPos(ResultState *node);
extern void ExecReScanResult(ResultState *node);
extern void ExecNextResultContext(ResultState *node);
extern void ExecPrevResultContext(ResultState *node);

#endif							/* NODERESULT_H */
