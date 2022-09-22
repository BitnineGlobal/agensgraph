/*-------------------------------------------------------------------------
 *
 * cluster.h
 *	  header file for postgres cluster command stuff
 *
 * Portions Copyright (c) 1996-2021, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994-5, Regents of the University of California
 *
 * src/include/commands/cluster.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef CLUSTER_H
#define CLUSTER_H

#include "nodes/parsenodes.h"
#include "parser/parse_node.h"
#include "storage/lock.h"
#include "utils/relcache.h"


/* options for CLUSTER */
typedef enum ClusterOption
{
	CLUOPT_RECHECK = 1 << 0,	/* recheck relation state */
	CLUOPT_VERBOSE = 1 << 1		/* print progress info */
} ClusterOption;

extern void cluster(ParseState *pstate, ClusterStmt *stmt, bool isTopLevel);
extern void cluster_rel(Oid tableOid, Oid indexOid, int options);
extern void check_index_is_clusterable(Relation OldHeap, Oid indexOid,
									   bool recheck, LOCKMODE lockmode);
extern void mark_index_clustered(Relation rel, Oid indexOid, bool is_internal);

extern Oid	make_new_heap(Oid OIDOldHeap, Oid NewTableSpace, char relpersistence,
						  LOCKMODE lockmode);
extern void finish_heap_swap(Oid OIDOldHeap, Oid OIDNewHeap,
							 bool is_system_catalog,
							 bool swap_toast_by_content,
							 bool check_constraints,
							 bool is_internal,
							 TransactionId frozenXid,
							 MultiXactId minMulti,
							 char newrelpersistence);

#endif							/* CLUSTER_H */
