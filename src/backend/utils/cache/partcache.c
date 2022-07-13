/*-------------------------------------------------------------------------
 *
 * partcache.c
 *		Support routines for manipulating partition information cached in
 *		relcache
 *
 * Portions Copyright (c) 1996-2018, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * IDENTIFICATION
 *		  src/backend/utils/cache/partcache.c
 *
 *-------------------------------------------------------------------------
*/
#include "postgres.h"

#include "access/hash.h"
#include "access/heapam.h"
#include "access/htup_details.h"
#include "access/nbtree.h"
#include "catalog/partition.h"
#include "catalog/pg_inherits.h"
#include "catalog/pg_opclass.h"
#include "catalog/pg_partitioned_table.h"
#include "miscadmin.h"
#include "nodes/makefuncs.h"
#include "nodes/nodeFuncs.h"
#include "optimizer/clauses.h"
#include "optimizer/planner.h"
#include "partitioning/partbounds.h"
#include "utils/builtins.h"
#include "utils/datum.h"
#include "utils/lsyscache.h"
#include "utils/memutils.h"
#include "utils/partcache.h"
#include "utils/rel.h"
#include "utils/syscache.h"


static List *generate_partition_qual(Relation rel);

/*
 * RelationBuildPartitionKey
 *		Build and attach to relcache partition key data of relation
 *
 * Partitioning key data is a complex structure; to avoid complicated logic to
 * free individual elements whenever the relcache entry is flushed, we give it
 * its own memory context, child of CacheMemoryContext, which can easily be
 * deleted on its own.  To avoid leaking memory in that context in case of an
 * error partway through this function, the context is initially created as a
 * child of CurTransactionContext and only re-parented to CacheMemoryContext
 * at the end, when no further errors are possible.  Also, we don't make this
 * context the current context except in very brief code sections, out of fear
 * that some of our callees allocate memory on their own which would be leaked
 * permanently.
 */
void
RelationBuildPartitionKey(Relation relation)
{
	Form_pg_partitioned_table form;
	HeapTuple	tuple;
	bool		isnull;
	int			i;
	PartitionKey key;
	AttrNumber *attrs;
	oidvector  *opclass;
	oidvector  *collation;
	ListCell   *partexprs_item;
	Datum		datum;
	MemoryContext partkeycxt,
				oldcxt;
	int16		procnum;

	tuple = SearchSysCache1(PARTRELID,
							ObjectIdGetDatum(RelationGetRelid(relation)));

	/*
	 * The following happens when we have created our pg_class entry but not
	 * the pg_partitioned_table entry yet.
	 */
	if (!HeapTupleIsValid(tuple))
		return;

	partkeycxt = AllocSetContextCreate(CurTransactionContext,
									   "partition key",
									   ALLOCSET_SMALL_SIZES);
	MemoryContextCopyAndSetIdentifier(partkeycxt,
									  RelationGetRelationName(relation));

	key = (PartitionKey) MemoryContextAllocZero(partkeycxt,
												sizeof(PartitionKeyData));

	/* Fixed-length attributes */
	form = (Form_pg_partitioned_table) GETSTRUCT(tuple);
	key->strategy = form->partstrat;
	key->partnatts = form->partnatts;

	/*
	 * We can rely on the first variable-length attribute being mapped to the
	 * relevant field of the catalog's C struct, because all previous
	 * attributes are non-nullable and fixed-length.
	 */
	attrs = form->partattrs.values;

	/* But use the hard way to retrieve further variable-length attributes */
	/* Operator class */
	datum = SysCacheGetAttr(PARTRELID, tuple,
							Anum_pg_partitioned_table_partclass, &isnull);
	Assert(!isnull);
	opclass = (oidvector *) DatumGetPointer(datum);

	/* Collation */
	datum = SysCacheGetAttr(PARTRELID, tuple,
							Anum_pg_partitioned_table_partcollation, &isnull);
	Assert(!isnull);
	collation = (oidvector *) DatumGetPointer(datum);

	/* Expressions */
	datum = SysCacheGetAttr(PARTRELID, tuple,
							Anum_pg_partitioned_table_partexprs, &isnull);
	if (!isnull)
	{
		char	   *exprString;
		Node	   *expr;

		exprString = TextDatumGetCString(datum);
		expr = stringToNode(exprString);
		pfree(exprString);

		/*
		 * Run the expressions through const-simplification since the planner
		 * will be comparing them to similarly-processed qual clause operands,
		 * and may fail to detect valid matches without this step; fix
		 * opfuncids while at it.  We don't need to bother with
		 * canonicalize_qual() though, because partition expressions should be
		 * in canonical form already (ie, no need for OR-merging or constant
		 * elimination).
		 */
		expr = eval_const_expressions(NULL, expr);
		fix_opfuncids(expr);

		oldcxt = MemoryContextSwitchTo(partkeycxt);
		key->partexprs = (List *) copyObject(expr);
		MemoryContextSwitchTo(oldcxt);
	}

	oldcxt = MemoryContextSwitchTo(partkeycxt);
	key->partattrs = (AttrNumber *) palloc0(key->partnatts * sizeof(AttrNumber));
	key->partopfamily = (Oid *) palloc0(key->partnatts * sizeof(Oid));
	key->partopcintype = (Oid *) palloc0(key->partnatts * sizeof(Oid));
	key->partsupfunc = (FmgrInfo *) palloc0(key->partnatts * sizeof(FmgrInfo));

	key->partcollation = (Oid *) palloc0(key->partnatts * sizeof(Oid));

	/* Gather type and collation info as well */
	key->parttypid = (Oid *) palloc0(key->partnatts * sizeof(Oid));
	key->parttypmod = (int32 *) palloc0(key->partnatts * sizeof(int32));
	key->parttyplen = (int16 *) palloc0(key->partnatts * sizeof(int16));
	key->parttypbyval = (bool *) palloc0(key->partnatts * sizeof(bool));
	key->parttypalign = (char *) palloc0(key->partnatts * sizeof(char));
	key->parttypcoll = (Oid *) palloc0(key->partnatts * sizeof(Oid));
	MemoryContextSwitchTo(oldcxt);

	/* determine support function number to search for */
	procnum = (key->strategy == PARTITION_STRATEGY_HASH) ?
		HASHEXTENDED_PROC : BTORDER_PROC;

	/* Copy partattrs and fill other per-attribute info */
	memcpy(key->partattrs, attrs, key->partnatts * sizeof(int16));
	partexprs_item = list_head(key->partexprs);
	for (i = 0; i < key->partnatts; i++)
	{
		AttrNumber	attno = key->partattrs[i];
		HeapTuple	opclasstup;
		Form_pg_opclass opclassform;
		Oid			funcid;

		/* Collect opfamily information */
		opclasstup = SearchSysCache1(CLAOID,
									 ObjectIdGetDatum(opclass->values[i]));
		if (!HeapTupleIsValid(opclasstup))
			elog(ERROR, "cache lookup failed for opclass %u", opclass->values[i]);

		opclassform = (Form_pg_opclass) GETSTRUCT(opclasstup);
		key->partopfamily[i] = opclassform->opcfamily;
		key->partopcintype[i] = opclassform->opcintype;

		/* Get a support function for the specified opfamily and datatypes */
		funcid = get_opfamily_proc(opclassform->opcfamily,
								   opclassform->opcintype,
								   opclassform->opcintype,
								   procnum);
		if (!OidIsValid(funcid))
			ereport(ERROR,
					(errcode(ERRCODE_INVALID_OBJECT_DEFINITION),
					 errmsg("operator class \"%s\" of access method %s is missing support function %d for type %s",
							NameStr(opclassform->opcname),
							(key->strategy == PARTITION_STRATEGY_HASH) ?
							"hash" : "btree",
							procnum,
							format_type_be(opclassform->opcintype))));

		fmgr_info_cxt(funcid, &key->partsupfunc[i], partkeycxt);

		/* Collation */
		key->partcollation[i] = collation->values[i];

		/* Collect type information */
		if (attno != 0)
		{
			Form_pg_attribute att = TupleDescAttr(relation->rd_att, attno - 1);

			key->parttypid[i] = att->atttypid;
			key->parttypmod[i] = att->atttypmod;
			key->parttypcoll[i] = att->attcollation;
		}
		else
		{
			if (partexprs_item == NULL)
				elog(ERROR, "wrong number of partition key expressions");

			key->parttypid[i] = exprType(lfirst(partexprs_item));
			key->parttypmod[i] = exprTypmod(lfirst(partexprs_item));
			key->parttypcoll[i] = exprCollation(lfirst(partexprs_item));

			partexprs_item = lnext(partexprs_item);
		}
		get_typlenbyvalalign(key->parttypid[i],
							 &key->parttyplen[i],
							 &key->parttypbyval[i],
							 &key->parttypalign[i]);

		ReleaseSysCache(opclasstup);
	}

	ReleaseSysCache(tuple);

	/*
	 * Success --- reparent our context and make the relcache point to the
	 * newly constructed key
	 */
	MemoryContextSetParent(partkeycxt, CacheMemoryContext);
	relation->rd_partkeycxt = partkeycxt;
	relation->rd_partkey = key;
}

/*
 * RelationBuildPartitionDesc
 *		Form rel's partition descriptor
 *
 * Not flushed from the cache by RelationClearRelation() unless changed because
 * of addition or removal of partition.
 */
void
RelationBuildPartitionDesc(Relation rel)
{
	PartitionDesc partdesc;
	PartitionBoundInfo boundinfo = NULL;
	List	   *inhoids;
	PartitionBoundSpec **boundspecs = NULL;
	Oid		   *oids = NULL;
	ListCell   *cell;
	int			i,
				nparts;
	PartitionKey key = RelationGetPartitionKey(rel);
	MemoryContext oldcxt;
	int		   *mapping;

	/* Get partition oids from pg_inherits */
	inhoids = find_inheritance_children(RelationGetRelid(rel), NoLock);
	nparts = list_length(inhoids);

	if (nparts > 0)
	{
		oids = palloc(nparts * sizeof(Oid));
		boundspecs = palloc(nparts * sizeof(PartitionBoundSpec *));
	}

	/* Collect bound spec nodes for each partition */
	i = 0;
	foreach(cell, inhoids)
	{
		Oid			inhrelid = lfirst_oid(cell);
		HeapTuple	tuple;
		Datum		datum;
		bool		isnull;
		PartitionBoundSpec *boundspec;

		tuple = SearchSysCache1(RELOID, inhrelid);
		if (!HeapTupleIsValid(tuple))
			elog(ERROR, "cache lookup failed for relation %u", inhrelid);

		datum = SysCacheGetAttr(RELOID, tuple,
								Anum_pg_class_relpartbound,
								&isnull);
		if (isnull)
			elog(ERROR, "null relpartbound for relation %u", inhrelid);
		boundspec = stringToNode(TextDatumGetCString(datum));
		if (!IsA(boundspec, PartitionBoundSpec))
			elog(ERROR, "invalid relpartbound for relation %u", inhrelid);

		/*
		 * Sanity check: If the PartitionBoundSpec says this is the default
		 * partition, its OID should correspond to whatever's stored in
		 * pg_partitioned_table.partdefid; if not, the catalog is corrupt.
		 */
		if (boundspec->is_default)
		{
			Oid			partdefid;

			partdefid = get_default_partition_oid(RelationGetRelid(rel));
			if (partdefid != inhrelid)
				elog(ERROR, "expected partdefid %u, but got %u",
					 inhrelid, partdefid);
		}

		oids[i] = inhrelid;
		boundspecs[i] = boundspec;
		++i;
		ReleaseSysCache(tuple);
	}

	/* Now build the actual relcache partition descriptor */
	rel->rd_pdcxt = AllocSetContextCreate(CacheMemoryContext,
										  "partition descriptor",
										  ALLOCSET_DEFAULT_SIZES);
	MemoryContextCopyAndSetIdentifier(rel->rd_pdcxt, RelationGetRelationName(rel));

	oldcxt = MemoryContextSwitchTo(rel->rd_pdcxt);
	partdesc = (PartitionDescData *) palloc0(sizeof(PartitionDescData));
	partdesc->nparts = nparts;
	/* oids and boundinfo are allocated below. */

	MemoryContextSwitchTo(oldcxt);

	if (nparts == 0)
	{
		rel->rd_partdesc = partdesc;
		return;
	}

	/* First create PartitionBoundInfo */
	boundinfo = partition_bounds_create(boundspecs, nparts, key, &mapping);

	/* Now copy boundinfo and oids into partdesc. */
	oldcxt = MemoryContextSwitchTo(rel->rd_pdcxt);
	partdesc->boundinfo = partition_bounds_copy(boundinfo, key);
	partdesc->oids = (Oid *) palloc(partdesc->nparts * sizeof(Oid));
	partdesc->is_leaf = (bool *) palloc(partdesc->nparts * sizeof(bool));

	/*
	 * Now assign OIDs from the original array into mapped indexes of the
	 * result array.  The order of OIDs in the former is defined by the
	 * catalog scan that retrieved them, whereas that in the latter is defined
	 * by canonicalized representation of the partition bounds.
	 */
	for (i = 0; i < partdesc->nparts; i++)
	{
		int			index = mapping[i];

		partdesc->oids[index] = oids[i];
		/* Record if the partition is a leaf partition */
		partdesc->is_leaf[index] =
				(get_rel_relkind(oids[i]) != RELKIND_PARTITIONED_TABLE);
	}
	MemoryContextSwitchTo(oldcxt);

	rel->rd_partdesc = partdesc;
}

/*
 * RelationGetPartitionQual
 *
 * Returns a list of partition quals
 */
List *
RelationGetPartitionQual(Relation rel)
{
	/* Quick exit */
	if (!rel->rd_rel->relispartition)
		return NIL;

	return generate_partition_qual(rel);
}

/*
 * get_partition_qual_relid
 *
 * Returns an expression tree describing the passed-in relation's partition
 * constraint.
 *
 * If the relation is not found, or is not a partition, or there is no
 * partition constraint, return NULL.  We must guard against the first two
 * cases because this supports a SQL function that could be passed any OID.
 * The last case can happen even if relispartition is true, when a default
 * partition is the only partition.
 */
Expr *
get_partition_qual_relid(Oid relid)
{
	Expr	   *result = NULL;

	/* Do the work only if this relation exists and is a partition. */
	if (get_rel_relispartition(relid))
	{
		Relation	rel = relation_open(relid, AccessShareLock);
		List	   *and_args;

		and_args = generate_partition_qual(rel);

		/* Convert implicit-AND list format to boolean expression */
		if (and_args == NIL)
			result = NULL;
		else if (list_length(and_args) > 1)
			result = makeBoolExpr(AND_EXPR, and_args, -1);
		else
			result = linitial(and_args);

		/* Keep the lock, to allow safe deparsing against the rel by caller. */
		relation_close(rel, NoLock);
	}

	return result;
}

/*
 * generate_partition_qual
 *
 * Generate partition predicate from rel's partition bound expression. The
 * function returns a NIL list if there is no predicate.
 *
 * Result expression tree is stored CacheMemoryContext to ensure it survives
 * as long as the relcache entry. But we should be running in a less long-lived
 * working context. To avoid leaking cache memory if this routine fails partway
 * through, we build in working memory and then copy the completed structure
 * into cache memory.
 */
static List *
generate_partition_qual(Relation rel)
{
	HeapTuple	tuple;
	MemoryContext oldcxt;
	Datum		boundDatum;
	bool		isnull;
	List	   *my_qual = NIL,
			   *result = NIL;
	Relation	parent;
	bool		found_whole_row;

	/* Guard against stack overflow due to overly deep partition tree */
	check_stack_depth();

	/* Quick copy */
	if (rel->rd_partcheck != NIL)
		return copyObject(rel->rd_partcheck);

	/* Grab at least an AccessShareLock on the parent table */
	parent = relation_open(get_partition_parent(RelationGetRelid(rel)),
						   AccessShareLock);

	/* Get pg_class.relpartbound */
	tuple = SearchSysCache1(RELOID, RelationGetRelid(rel));
	if (!HeapTupleIsValid(tuple))
		elog(ERROR, "cache lookup failed for relation %u",
			 RelationGetRelid(rel));

	boundDatum = SysCacheGetAttr(RELOID, tuple,
								 Anum_pg_class_relpartbound,
								 &isnull);
	if (!isnull)
	{
		PartitionBoundSpec *bound;

		bound = castNode(PartitionBoundSpec,
						 stringToNode(TextDatumGetCString(boundDatum)));

		my_qual = get_qual_from_partbound(rel, parent, bound);
	}

	ReleaseSysCache(tuple);

	/* Add the parent's quals to the list (if any) */
	if (parent->rd_rel->relispartition)
		result = list_concat(generate_partition_qual(parent), my_qual);
	else
		result = my_qual;

	/*
	 * Change Vars to have partition's attnos instead of the parent's. We do
	 * this after we concatenate the parent's quals, because we want every Var
	 * in it to bear this relation's attnos. It's safe to assume varno = 1
	 * here.
	 */
	result = map_partition_varattnos(result, 1, rel, parent,
									 &found_whole_row);
	/* There can never be a whole-row reference here */
	if (found_whole_row)
		elog(ERROR, "unexpected whole-row reference found in partition key");

	/* Save a copy in the relcache */
	oldcxt = MemoryContextSwitchTo(CacheMemoryContext);
	rel->rd_partcheck = copyObject(result);
	MemoryContextSwitchTo(oldcxt);

	/* Keep the parent locked until commit */
	relation_close(parent, NoLock);

	return result;
}
