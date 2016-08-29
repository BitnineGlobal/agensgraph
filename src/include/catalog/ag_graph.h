/*-------------------------------------------------------------------------
 *
 * ag_graph.h
 *	  definition of the system "graph" relation (ag_graph)
 *	  along with the relation's initial contents.
 *
 *
 * Copyright (c) 2016 by Bitnine Global, Inc.
 *
 * src/include/catalog/ag_graph.h
 *
 * NOTES
 *	  the genbki.pl script reads this file and generates .bki
 *	  information from the DATA() statements.
 *
 *-------------------------------------------------------------------------
 */
#ifndef AG_GRAPH_H
#define AG_GRAPH_H

#include "catalog/genbki.h"

/* ----------------
 *		ag_graph definition.  cpp turns this into
 *		typedef struct FormData_ag_graph
 * ----------------
 */
#define GraphRelationId	7040


CATALOG(ag_graph ,7040) BKI_SCHEMA_MACRO
{
	NameData	graphname;
	Oid			graphowner;
} FormData_ag_graph;

/* ----------------
 *		Form_ag_graph corresponds to a pointer to a tuple with
 *		the format of ag_graph relation.
 * ----------------
 */
typedef FormData_ag_graph *Form_ag_graph;

/* ----------------
 *		compiler constants for ag_inherits
 * ----------------
 */

#define Natts_ag_graph					2
#define Anum_ag_graph_graphname			1
#define Anum_ag_graph_graphowner		2

/* ----------------
 *		ag_graph has no initial contents
 * ----------------
 */

extern char *graph_path;

#endif   /* AG_GRAPH_H */
