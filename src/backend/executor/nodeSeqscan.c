/*-------------------------------------------------------------------------
 *
 * nodeSeqscan.c
 *	  Support routines for sequential scans of relations.
 *
 * Portions Copyright (c) 1996-2018, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  src/backend/executor/nodeSeqscan.c
 *
 *-------------------------------------------------------------------------
 */
/*
 * INTERFACE ROUTINES
 *		ExecSeqScan				sequentially scans a relation.
 *		ExecSeqNext				retrieve next tuple in sequential order.
 *		ExecInitSeqScan			creates and initializes a seqscan node.
 *		ExecEndSeqScan			releases any storage allocated.
 *		ExecReScanSeqScan		rescans the relation
 *
 *		ExecSeqScanEstimate		estimates DSM space needed for parallel scan
 *		ExecSeqScanInitializeDSM initialize DSM for parallel scan
 *		ExecSeqScanReInitializeDSM reinitialize DSM for fresh parallel scan
 *		ExecSeqScanInitializeWorker attach to DSM info in parallel worker
 */
#include "postgres.h"

#include "access/relscan.h"
#include "catalog/pg_operator.h"
#include "executor/execdebug.h"
#include "executor/nodeSeqscan.h"
#include "optimizer/clauses.h"
#include "utils/graph.h"
#include "utils/memutils.h"
#include "utils/rel.h"

static TupleTableSlot *SeqNext(SeqScanState *node);

static void InitScanLabelSkipExpr(SeqScanState *node);
static ExprState *GetScanLabelSkipExpr(SeqScanState *node, Expr *opexpr);
static bool IsGraphidColumn(SeqScanState *node, Node *expr);

/* ----------------------------------------------------------------
 *						Scan Support
 * ----------------------------------------------------------------
 */

/* ----------------------------------------------------------------
 *		SeqNext
 *
 *		This is a workhorse for ExecSeqScan
 * ----------------------------------------------------------------
 */
static TupleTableSlot *
SeqNext(SeqScanState *node)
{
	HeapTuple	tuple;
	HeapScanDesc scandesc;
	EState	   *estate;
	ScanDirection direction;
	TupleTableSlot *slot;

	/*
	 * get information from the estate and scan state
	 */
	scandesc = node->ss.ss_currentScanDesc;
	estate = node->ss.ps.state;
	direction = estate->es_direction;
	slot = node->ss.ss_ScanTupleSlot;

	if (scandesc == NULL)
	{
		/*
		 * We reach here if the scan is not parallel, or if we're executing a
		 * scan that was intended to be parallel serially.
		 */
		scandesc = heap_beginscan(node->ss.ss_currentRelation,
								  estate->es_snapshot,
								  0, NULL);
		node->ss.ss_currentScanDesc = scandesc;
	}

	/*
	 * get the next tuple from the table
	 */
	tuple = heap_getnext(scandesc, direction);

	/*
	 * save the tuple and the buffer returned to us by the access methods in
	 * our scan tuple slot and return the slot.  Note: we pass 'false' because
	 * tuples returned by heap_getnext() are pointers onto disk pages and were
	 * not created with palloc() and so should not be pfree()'d.  Note also
	 * that ExecStoreTuple will increment the refcount of the buffer; the
	 * refcount will not be dropped until the tuple table slot is cleared.
	 */
	if (tuple)
		ExecStoreTuple(tuple,	/* tuple to store */
					   slot,	/* slot to store in */
					   scandesc->rs_cbuf,	/* buffer associated with this
											 * tuple */
					   false);	/* don't pfree this pointer */
	else
		ExecClearTuple(slot);

	return slot;
}

/*
 * SeqRecheck -- access method routine to recheck a tuple in EvalPlanQual
 */
static bool
SeqRecheck(SeqScanState *node, TupleTableSlot *slot)
{
	/*
	 * Note that unlike IndexScan, SeqScan never use keys in heap_beginscan
	 * (and this is very bad) - so, here we do not check are keys ok or not.
	 */
	return true;
}

/* ----------------------------------------------------------------
 *		ExecSeqScan(node)
 *
 *		Scans the relation sequentially and returns the next qualifying
 *		tuple.
 *		We call the ExecScan() routine and pass it the appropriate
 *		access method functions.
 * ----------------------------------------------------------------
 */
static TupleTableSlot *
ExecSeqScan(PlanState *pstate)
{
	SeqScanState *node = castNode(SeqScanState, pstate);

	if (node->ss.ss_skipLabelScan)
	{
		node->ss.ss_skipLabelScan = false;
		return NULL;
	}

	return ExecScan(&node->ss,
					(ExecScanAccessMtd) SeqNext,
					(ExecScanRecheckMtd) SeqRecheck);
}

/* ----------------------------------------------------------------
 *		InitScanRelation
 *
 *		Set up to access the scan relation.
 * ----------------------------------------------------------------
 */
static void
InitScanRelation(SeqScanState *node, EState *estate, int eflags)
{
	Relation	currentRelation;

	/*
	 * get the relation object id from the relid'th entry in the range table,
	 * open that relation and acquire appropriate lock on it.
	 */
	currentRelation = ExecOpenScanRelation(estate,
										   ((SeqScan *) node->ss.ps.plan)->scanrelid,
										   eflags);

	node->ss.ss_currentRelation = currentRelation;

	/* and report the scan tuple slot's rowtype */
	ExecAssignScanType(&node->ss, RelationGetDescr(currentRelation));
}

static void
InitScanLabelSkipExpr(SeqScanState *node)
{
	List	   *qual = node->ss.ps.plan->qual;
	ListCell   *la;

	AssertArg(node->ss.ss_isLabel);

	if (qual == NIL)
		return;

	/* qual was implicitly-ANDed, so; */
	foreach(la, qual)
	{
		Expr	   *expr = lfirst(la);
		ExprState  *xstate;

		if (!is_opclause(expr))
			continue;

		if (((OpExpr *) expr)->opno != OID_GRAPHID_EQ_OP)
			continue;

		/* expr is of the form `graphid = graphid` */

		xstate = GetScanLabelSkipExpr(node, expr);
		if (xstate == NULL)
			continue;

		node->ss.ss_labelSkipExpr = xstate;
		break;
	}
}

static ExprState *
GetScanLabelSkipExpr(SeqScanState *node, Expr *opexpr)
{
	Node	   *left = get_leftop(opexpr);
	Node	   *right = get_rightop(opexpr);
	Node	   *expr;

	if (IsGraphidColumn(node, left))
		expr = right;
	else if (IsGraphidColumn(node, right))
		expr = left;
	else
		return NULL;

	/* Const or Param expected */
	if (IsA(expr, Const) || IsA(expr, Param))
		return ExecInitExpr((Expr *) expr, (PlanState *) node);

	return NULL;
}

static bool
IsGraphidColumn(SeqScanState *node, Node *expr)
{
	Var *var = (Var *) expr;

	return (IsA(expr, Var) &&
			var->varno == ((SeqScan *) node->ss.ps.plan)->scanrelid &&
			var->varattno == Anum_vertex_id);
}


/* ----------------------------------------------------------------
 *		ExecInitSeqScan
 * ----------------------------------------------------------------
 */
SeqScanState *
ExecInitSeqScan(SeqScan *node, EState *estate, int eflags)
{
	SeqScanState *scanstate;

	/*
	 * Once upon a time it was possible to have an outerPlan of a SeqScan, but
	 * not any more.
	 */
	Assert(outerPlan(node) == NULL);
	Assert(innerPlan(node) == NULL);

	/*
	 * create state structure
	 */
	scanstate = makeNode(SeqScanState);
	scanstate->ss.ps.plan = (Plan *) node;
	scanstate->ss.ps.state = estate;
	scanstate->ss.ps.ExecProcNode = ExecSeqScan;

	/*
	 * Miscellaneous initialization
	 *
	 * create expression context for node
	 */
	ExecAssignExprContext(estate, &scanstate->ss.ps);

	/*
	 * Initialize scan relation.
	 *
	 * Get the relation object id from the relid'th entry in the range table,
	 * open that relation and acquire appropriate lock on it.
	 */
	scanstate->ss.ss_currentRelation =
		ExecOpenScanRelation(estate,
							 node->scanrelid,
							 eflags);

	/* and create slot with the appropriate rowtype */
	ExecInitScanTupleSlot(estate, &scanstate->ss,
						  RelationGetDescr(scanstate->ss.ss_currentRelation));

	/*
	 * Initialize result slot, type and projection.
	 */
	ExecInitResultTupleSlotTL(estate, &scanstate->ss.ps);
	ExecAssignScanProjectionInfo(&scanstate->ss);

	/*
	 * initialize child expressions
	 */
	scanstate->ss.ps.qual =
		ExecInitQual(node->plan.qual, (PlanState *) scanstate);

	/*
	 * tuple table initialization
	 */
	ExecInitResultTupleSlot(estate, &scanstate->ss.ps);
	ExecInitScanTupleSlot(estate, &scanstate->ss);

	/*
	 * initialize scan relation
	 */
	InitScanRelation(scanstate, estate, eflags);

	InitScanLabelInfo((ScanState *) scanstate);
	if (scanstate->ss.ss_isLabel)
		InitScanLabelSkipExpr(scanstate);

	/*
	 * Initialize result tuple type and projection info.
	 */
	ExecAssignResultTypeFromTL(&scanstate->ss.ps);
	ExecAssignScanProjectionInfo(&scanstate->ss);

	dlist_init(&scanstate->vle_ctxs);
	scanstate->cur_ctx = NULL;

	return scanstate;
}

/* ----------------------------------------------------------------
 *		ExecEndSeqScan
 *
 *		frees any storage allocated through C routines.
 * ----------------------------------------------------------------
 */
void
ExecEndSeqScan(SeqScanState *node)
{
	Relation	relation;
	HeapScanDesc scanDesc;
	dlist_mutable_iter miter;

	/*
	 * get information from node
	 */
	relation = node->ss.ss_currentRelation;
	scanDesc = node->ss.ss_currentScanDesc;

	/*
	 * Free the exprcontext
	 */
	ExecFreeExprContext(&node->ss.ps);

	/*
	 * clean out the tuple table
	 */
	ExecClearTuple(node->ss.ps.ps_ResultTupleSlot);
	ExecClearTuple(node->ss.ss_ScanTupleSlot);

	/*
	 * close heap scan
	 */
	dlist_foreach_modify(miter, &node->vle_ctxs)
	{
		SeqScanVLECtx *ctx;

		ctx = dlist_container(SeqScanVLECtx, list, miter.cur);
		dlist_delete(miter.cur);

		if (miter.cur != node->cur_ctx)
		{
			if (ctx->ss_currentScanDesc != NULL)
				heap_endscan(ctx->ss_currentScanDesc);
		}

		pfree(ctx);
	}
	node->cur_ctx = NULL;

	if (scanDesc != NULL)
		heap_endscan(scanDesc);

	/*
	 * close the heap relation.
	 */
	ExecCloseScanRelation(relation);
}

/* ----------------------------------------------------------------
 *						Join Support
 * ----------------------------------------------------------------
 */

/* ----------------------------------------------------------------
 *		ExecReScanSeqScan
 *
 *		Rescans the relation.
 * ----------------------------------------------------------------
 */
void
ExecReScanSeqScan(SeqScanState *node)
{
	HeapScanDesc scan;

	/* determine whether we can skip this label scan or not */
	if (node->ss.ss_isLabel && node->ss.ss_labelSkipExpr != NULL)
	{
		ExprContext *econtext = node->ss.ps.ps_ExprContext;
		MemoryContext oldmctx;
		bool		isnull;
		Datum		graphid;

		oldmctx = MemoryContextSwitchTo(econtext->ecxt_per_tuple_memory);

		graphid = ExecEvalExpr(node->ss.ss_labelSkipExpr, econtext, &isnull);
		if (isnull)
		{
			node->ss.ss_skipLabelScan = true;
		}
		else
		{
			uint16 labid;

			labid = DatumGetUInt16(DirectFunctionCall1(graphid_labid, graphid));
			if (node->ss.ss_labid != labid)
				node->ss.ss_skipLabelScan = true;
		}

		ResetExprContext(econtext);

		MemoryContextSwitchTo(oldmctx);
	}

	scan = node->ss.ss_currentScanDesc;

	if (scan != NULL)
		heap_rescan(scan,		/* scan desc */
					NULL);		/* new scan keys */

	ExecScanReScan((ScanState *) node);
}

void
ExecUpScanSeqScan(SeqScanState *node)
{
	SeqScanVLECtx *ctx;

	node->cur_ctx = dlist_prev_node(&node->vle_ctxs, node->cur_ctx);
	ctx = dlist_container(SeqScanVLECtx, list, node->cur_ctx);
	node->ss.ss_currentScanDesc = ctx->ss_currentScanDesc;
}

void
ExecDownScanSeqScan(SeqScanState *node)
{
	EState *estate = node->ss.ps.state;
	SeqScanVLECtx *ctx;

	if (node->cur_ctx == NULL)
	{
		if (node->ss.ss_currentScanDesc == NULL)
			node->ss.ss_currentScanDesc = heap_beginscan(
					node->ss.ss_currentRelation, estate->es_snapshot, 0, NULL);

		ctx = palloc(sizeof(*ctx));
		ctx->ss_currentScanDesc = node->ss.ss_currentScanDesc;
		dlist_push_tail(&node->vle_ctxs, &ctx->list);
		node->cur_ctx = dlist_tail_node(&node->vle_ctxs);
	}

	if (dlist_has_next(&node->vle_ctxs, node->cur_ctx))
	{
		node->cur_ctx = dlist_next_node(&node->vle_ctxs, node->cur_ctx);
		ctx = dlist_container(SeqScanVLECtx, list, node->cur_ctx);
		node->ss.ss_currentScanDesc = ctx->ss_currentScanDesc;
	}
	else
	{
		node->ss.ss_currentScanDesc = heap_beginscan(
				node->ss.ss_currentRelation, estate->es_snapshot, 0, NULL);

		ctx = palloc(sizeof(*ctx));
		ctx->ss_currentScanDesc = node->ss.ss_currentScanDesc;
		dlist_push_tail(&node->vle_ctxs, &ctx->list);
		node->cur_ctx = dlist_tail_node(&node->vle_ctxs);
	}
}

/* ----------------------------------------------------------------
 *						Parallel Scan Support
 * ----------------------------------------------------------------
 */

/* ----------------------------------------------------------------
 *		ExecSeqScanEstimate
 *
 *		Compute the amount of space we'll need in the parallel
 *		query DSM, and inform pcxt->estimator about our needs.
 * ----------------------------------------------------------------
 */
void
ExecSeqScanEstimate(SeqScanState *node,
					ParallelContext *pcxt)
{
	EState	   *estate = node->ss.ps.state;

	node->pscan_len = heap_parallelscan_estimate(estate->es_snapshot);
	shm_toc_estimate_chunk(&pcxt->estimator, node->pscan_len);
	shm_toc_estimate_keys(&pcxt->estimator, 1);
}

/* ----------------------------------------------------------------
 *		ExecSeqScanInitializeDSM
 *
 *		Set up a parallel heap scan descriptor.
 * ----------------------------------------------------------------
 */
void
ExecSeqScanInitializeDSM(SeqScanState *node,
						 ParallelContext *pcxt)
{
	EState	   *estate = node->ss.ps.state;
	ParallelHeapScanDesc pscan;

	pscan = shm_toc_allocate(pcxt->toc, node->pscan_len);
	heap_parallelscan_initialize(pscan,
								 node->ss.ss_currentRelation,
								 estate->es_snapshot);
	shm_toc_insert(pcxt->toc, node->ss.ps.plan->plan_node_id, pscan);
	node->ss.ss_currentScanDesc =
		heap_beginscan_parallel(node->ss.ss_currentRelation, pscan);
}

/* ----------------------------------------------------------------
 *		ExecSeqScanReInitializeDSM
 *
 *		Reset shared state before beginning a fresh scan.
 * ----------------------------------------------------------------
 */
void
ExecSeqScanReInitializeDSM(SeqScanState *node,
						   ParallelContext *pcxt)
{
	HeapScanDesc scan = node->ss.ss_currentScanDesc;

	heap_parallelscan_reinitialize(scan->rs_parallel);
}

/* ----------------------------------------------------------------
 *		ExecSeqScanInitializeWorker
 *
 *		Copy relevant information from TOC into planstate.
 * ----------------------------------------------------------------
 */
void
ExecSeqScanInitializeWorker(SeqScanState *node,
							ParallelWorkerContext *pwcxt)
{
	ParallelHeapScanDesc pscan;

	pscan = shm_toc_lookup(pwcxt->toc, node->ss.ps.plan->plan_node_id, false);
	node->ss.ss_currentScanDesc =
		heap_beginscan_parallel(node->ss.ss_currentRelation, pscan);
}
