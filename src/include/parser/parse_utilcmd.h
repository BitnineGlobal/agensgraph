/*-------------------------------------------------------------------------
 *
 * parse_utilcmd.h
 *		parse analysis for utility commands
 *
 *
 * Portions Copyright (c) 2018, Bitnine Inc.
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/parser/parse_utilcmd.h
 *
 *-------------------------------------------------------------------------
 */
#ifndef PARSE_UTILCMD_H
#define PARSE_UTILCMD_H

#include "parser/parse_node.h"


extern List *transformCreateStmt(CreateStmt *stmt, const char *queryString);
extern List *transformAlterTableStmt(Oid relid, AlterTableStmt *stmt,
									 const char *queryString);
extern IndexStmt *transformIndexStmt(Oid relid, IndexStmt *stmt,
									 const char *queryString);
extern void transformRuleStmt(RuleStmt *stmt, const char *queryString,
							  List **actions, Node **whereClause);
extern List *transformCreateSchemaStmt(CreateSchemaStmt *stmt);
extern PartitionBoundSpec *transformPartitionBound(ParseState *pstate, Relation parent,
												   PartitionBoundSpec *spec);
extern List *expandTableLikeClause(RangeVar *heapRel,
								   TableLikeClause *table_like_clause);
extern IndexStmt *generateClonedIndexStmt(RangeVar *heapRel,
										  Relation source_idx,
										  const AttrNumber *attmap, int attmap_length,
										  Oid *constraintOid);

extern List *transformCreateGraphStmt(CreateGraphStmt *stmt);
extern List *transformCreateLabelStmt(CreateLabelStmt *labelStmt,
									  const char *queryString);
extern char getLabelKind(char *labname, Oid graphid);
extern AlterTableStmt *transformAlterLabelStmt(AlterTableStmt *stmt);

extern Node *transformCreateConstraintStmt(ParseState *pstate,
										   CreateConstraintStmt *stmt);
extern Node *transformDropConstraintStmt(ParseState *pstate,
										 DropConstraintStmt *stmt);

extern IndexStmt *transformCreatePropertyIndexStmt(Oid relid,
												CreatePropertyIndexStmt *stmt,
												const char *queryString);

#endif							/* PARSE_UTILCMD_H */
