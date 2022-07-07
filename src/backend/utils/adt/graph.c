/*
 * graph.c
 *	  Functions for vertex and edge data type.
 *
 * Copyright (c) 2016 by Bitnine Global, Inc.
 *
 * IDENTIFICATION
 *	  src/backend/utils/adt/graph.c
 */

#include "postgres.h"

#include "ag_const.h"
#include "access/hash.h"
#include "access/htup_details.h"
#include "access/tupdesc.h"
#include "catalog/ag_graph_fn.h"
#include "catalog/namespace.h"
#include "catalog/pg_type.h"
#include "catalog/pg_inherits_fn.h"
#include "executor/spi.h"
#include "funcapi.h"
#include "libpq/libpq.h"
#include "libpq/pqformat.h"
#include "utils/array.h"
#include "utils/arrayaccess.h"
#include "utils/builtins.h"
#include "utils/graph.h"
#include "utils/int8.h"
#include "utils/jsonb.h"
#include "utils/lsyscache.h"
#include "utils/typcache.h"

#define GRAPHID_FMTSTR			"%hu." UINT64_FORMAT
#define GRAPHID_BUFLEN			32	/* "65535.281474976710655" */

typedef struct LabelOutData {
	uint16		label_labid;
	NameData	label;
} LabelOutData;

typedef struct GraphpathOutData {
	ArrayMetaState vertex;
	ArrayMetaState edge;
} GraphpathOutData;

typedef enum EdgeVertexKind {
	EVK_START,
	EVK_END
} EdgeVertexKind;

static void graphid_out_si(StringInfo si, Datum graphid);
static int graphid_cmp(FunctionCallInfo fcinfo);
static Jsonb *int_to_jsonb(int i);
static LabelOutData *cache_label(FmgrInfo *flinfo, uint16 labid);
static void elems_out_si(StringInfo si, AnyArrayType *elems, FmgrInfo *flinfo);
static void get_elem_type_output(ArrayMetaState *state, Oid elem_type,
								 MemoryContext mctx);
static Datum array_iter_next_(array_iter *it, int idx, ArrayMetaState *state);
static void deform_tuple(HeapTupleHeader tuphdr, Datum *values, bool *isnull);
static Datum tuple_getattr(HeapTupleHeader tuphdr, int attnum);
static Datum getEdgeVertex(HeapTupleHeader edge, EdgeVertexKind evk);
static Datum makeArrayTypeDatum(Datum *elems, int nelem, Oid type);
static Datum graphid_minval(void);

Datum
graphid(PG_FUNCTION_ARGS)
{
	uint16		labid = PG_GETARG_UINT16(0);
	uint64		locid = DatumGetUInt64(PG_GETARG_DATUM(1));
	Graphid		id;

	GraphidSet(&id, labid, locid);

	PG_RETURN_GRAPHID(id);
}

Datum
graphid_in(PG_FUNCTION_ARGS)
{
	const char	GRAPHID_DELIM = '.';
	char	   *str = PG_GETARG_CSTRING(0);
	char	   *next;
	char	   *endptr;
	uint16		labid;
	uint64		locid;
	Graphid		id;

	errno = 0;
	labid = strtoul(str, &endptr, 10);
	if (errno != 0 || endptr == str || *endptr != GRAPHID_DELIM)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type graphid: \"%s\"", str)));

	next = endptr + 1;
#ifdef HAVE_STRTOLL
	locid = strtoll(next, &endptr, 10);
	if (endptr == next || *endptr != '\0')
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for type graphid: \"%s\"", str)));
#else
	locid = DatumGetUInt64(DirectFunctionCall1(int8in, CStringGetDatum(next)));
#endif

	GraphidSet(&id, labid, locid);

	PG_RETURN_GRAPHID(id);
}

Datum
graphid_out(PG_FUNCTION_ARGS)
{
	Graphid		id = PG_GETARG_GRAPHID(0);
	char	   *buf;

	buf = palloc(GRAPHID_BUFLEN);
	snprintf(buf, GRAPHID_BUFLEN, GRAPHID_FMTSTR,
			 GraphidGetLabid(id), GraphidGetLocid(id));

	PG_RETURN_CSTRING(buf);
}

Datum
graphid_recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);

	PG_RETURN_GRAPHID((Graphid) pq_getmsgint64(buf));
}

Datum
graphid_send(PG_FUNCTION_ARGS)
{
	Graphid		arg1 = PG_GETARG_GRAPHID(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint64(&buf, arg1);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

static void
graphid_out_si(StringInfo si, Datum graphid)
{
	Graphid id = DatumGetGraphid(graphid);

	appendStringInfo(si, GRAPHID_FMTSTR,
					 GraphidGetLabid(id), GraphidGetLocid(id));
}

Datum
graphid_labid(PG_FUNCTION_ARGS)
{
	Graphid id = PG_GETARG_GRAPHID(0);

	PG_RETURN_INT32(GraphidGetLabid(id));
}

Datum
graphid_locid(PG_FUNCTION_ARGS)
{
	Graphid id = PG_GETARG_GRAPHID(0);

	PG_RETURN_INT64(GraphidGetLocid(id));
}

Datum
graph_labid(PG_FUNCTION_ARGS)
{
	char	   *labname = PG_GETARG_CSTRING(0);
	List	   *names;
	RangeVar   *rv;
	Oid			graphoid;
	uint16		labid;

	names = stringToQualifiedNameList(labname);
	rv = makeRangeVarFromNameList(names);
	graphoid = get_graphname_oid(rv->schemaname);
	labid = get_labname_labid(rv->relname, graphoid);

	PG_RETURN_INT32((int32) labid);
}

static int
graphid_cmp(FunctionCallInfo fcinfo)
{
	Graphid	   id1 = PG_GETARG_GRAPHID(0);
	Graphid	   id2 = PG_GETARG_GRAPHID(1);

	if (id1 < id2)
		return -1;
	if (id1 > id2)
		return 1;

	return 0;
}

Datum
graphid_eq(PG_FUNCTION_ARGS)
{
	/* use graphid_cmp() here, since graphid have a total ordering */
	PG_RETURN_BOOL(graphid_cmp(fcinfo) == 0);
}

Datum
graphid_ne(PG_FUNCTION_ARGS)
{
	PG_RETURN_BOOL(graphid_cmp(fcinfo) != 0);
}

Datum
graphid_lt(PG_FUNCTION_ARGS)
{
	PG_RETURN_BOOL(graphid_cmp(fcinfo) < 0);
}

Datum
graphid_gt(PG_FUNCTION_ARGS)
{
	PG_RETURN_BOOL(graphid_cmp(fcinfo) > 0);
}

Datum
graphid_le(PG_FUNCTION_ARGS)
{
	PG_RETURN_BOOL(graphid_cmp(fcinfo) <= 0);
}

Datum
graphid_ge(PG_FUNCTION_ARGS)
{
	PG_RETURN_BOOL(graphid_cmp(fcinfo) >= 0);
}

/* edgeref APIs */

#define DatumGetItemPointer(X)		((ItemPointer) DatumGetPointer(X))
#define PG_GETARG_ITEMPOINTER(n)	DatumGetItemPointer(PG_GETARG_DATUM(n))

Datum
edgeref(PG_FUNCTION_ARGS)
{
	int			relid = PG_GETARG_INT32(0);
	ItemPointer	tid = PG_GETARG_ITEMPOINTER(1);
	EdgeRef	   	edgeref;

	EdgeRefSet(edgeref, relid, tid);

	PG_RETURN_EDGEREF(edgeref);
}

Datum
edgeref_in(PG_FUNCTION_ARGS)
{
	const char	DELIM = ',';
	char	   *str = PG_GETARG_CSTRING(0);
	char	   *ptr;
	char	   *rdimpos;
	char	   *endptr;
	char	   *ctidbuf;
	int			ctidbuflen;
	Oid			typiofunc;
	Oid			typioparam;
	FmgrInfo	proc;
	uint16		relid;
	Datum		tid;
	EdgeRef	   	edgeref;

	ptr = str;

	/* Allow leading whitespace */
	while (*ptr && isspace((unsigned char) *ptr))
		ptr++;
	if (*ptr++ != '(')
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("malformed edgeref literal: \"%s\"", str),
				 errdetail("Missing left parenthesis.")));

	errno = 0;
	ptr++;
	relid = strtoul(ptr, &endptr, 10);
	if (errno || *endptr != DELIM)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("malformed edgeref literal: \"%s\"", str),
				 errdetail("Invalid relation index")));

	ptr = endptr + 1;
	rdimpos = strrchr(ptr, ')');
	if (rdimpos == NULL)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("malformed edgeref literal: \"%s\"", str),
				 errdetail("Invalid tid")));

	ctidbuflen = rdimpos - ptr;
	ctidbuf = palloc(sizeof(char) * ctidbuflen + 1);
	strncpy(ctidbuf, ptr, ctidbuflen);
	ctidbuf[ctidbuflen] = '\0';

	getTypeInputInfo(TIDOID, &typiofunc, &typioparam);
	fmgr_info_cxt(typiofunc, &proc, fcinfo->flinfo->fn_mcxt);

	tid = InputFunctionCall(&proc, ctidbuf, typioparam, -1);

	EdgeRefSet(edgeref, relid, (ItemPointer) DatumGetPointer(tid));

	ptr = rdimpos + 1;

	/* Allow trailing whitespace */
	while (*ptr && isspace((unsigned char) *ptr))
		ptr++;
	if (*ptr)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("malformed record literal: \"%s\"", str),
				 errdetail("Junk after right parenthesis.")));

	pfree(ctidbuf);

	PG_RETURN_EDGEREF(edgeref);
}

Datum
edgeref_out(PG_FUNCTION_ARGS)
{
	EdgeRef	    eref = PG_GETARG_EDGEREF(0);
	StringInfoData buf;
	Oid			typiofunc;
	bool		typisvarlena;
	ItemPointerData tid;
	FmgrInfo	proc;
	char	   *value;

	initStringInfo(&buf);
	appendStringInfo(&buf, "(%u,", EdgeRefGetRelid(eref));

	getTypeOutputInfo(TIDOID, &typiofunc, &typisvarlena);
	fmgr_info_cxt(typiofunc, &proc, fcinfo->flinfo->fn_mcxt);
	ItemPointerSet(&tid, EdgeRefGetBlockNumber(eref), EdgeRefGetOffsetNumber(eref));
	value = OutputFunctionCall(&proc, PointerGetDatum(&tid));

	appendStringInfo(&buf, "%s)", value);

	PG_RETURN_CSTRING(buf.data);
}

/* rowid APIs */

Datum
rowid(PG_FUNCTION_ARGS)
{
	Oid			tableoid = PG_GETARG_OID(0);
	ItemPointer	tid = PG_GETARG_ITEMPOINTER(1);
	Rowid	   *result;

	result = (Rowid *) palloc(sizeof(Rowid));
	result->tableoid = tableoid;
	memcpy(&result->tid, tid, sizeof(ItemPointerData));

	PG_RETURN_ROWID(result);
}

Datum
rowid_in(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("\"rowid\" type does not support input function")));

	PG_RETURN_NULL();
}

Datum
rowid_out(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("\"rowid\" type does not support output function")));

	PG_RETURN_NULL();
}

Datum
rowid_tableoid(PG_FUNCTION_ARGS)
{
	Rowid *rowid = PG_GETARG_ROWID(0);

	PG_RETURN_OID(rowid->tableoid);
}

Datum
rowid_ctid(PG_FUNCTION_ARGS)
{
	Rowid	   *rowid = PG_GETARG_ROWID(0);
	ItemPointer result;

	result = palloc(sizeof(ItemPointerData));
	ItemPointerCopy(&rowid->tid, result);

	return PointerGetDatum(result);
}

#define ItemPointerGetDatum(X)	PointerGetDatum(X)

Datum
rowid_eq(PG_FUNCTION_ARGS)
{
	Rowid	   *id0 = PG_GETARG_ROWID(0);
	Rowid	   *id1 = PG_GETARG_ROWID(1);
	Datum		sub_id0;
	Datum		sub_id1;
	bool		result;

	sub_id0 = ObjectIdGetDatum(id0->tableoid);
	sub_id1 = ObjectIdGetDatum(id1->tableoid);
	result = DatumGetBool(DirectFunctionCall2(oideq, sub_id0, sub_id1));
	if (result == false)
		PG_RETURN_BOOL(false);

	sub_id0 = ItemPointerGetDatum(&id0->tid);
	sub_id1 = ItemPointerGetDatum(&id1->tid);
	result = DatumGetBool(DirectFunctionCall2(tideq, sub_id0, sub_id1));
	PG_RETURN_BOOL(result);
}

Datum
rowid_ne(PG_FUNCTION_ARGS)
{
	bool result;

	result = DatumGetBool(DirectFunctionCall2(
				rowid_eq, PG_GETARG_DATUM(0), PG_GETARG_DATUM(1)));

	PG_RETURN_BOOL(!result);
}

Datum
rowid_lt(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot apply \"less-then\" to \"rowid\" type")));

	PG_RETURN_NULL();
}

Datum
rowid_gt(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot apply \"greater-then\" to \"rowid\" type")));

	PG_RETURN_NULL();
}

Datum
rowid_le(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot apply \"less-or-equal\" to \"rowid\" type")));

	PG_RETURN_NULL();
}

Datum
rowid_ge(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("cannot apply \"greater-or-equal\" to \"rowid\" type")));

	PG_RETURN_NULL();
}

Datum
btrowidcmp(PG_FUNCTION_ARGS)
{
	ereport(ERROR,
			(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
			 errmsg("\"rowid\" does not support comparison operation")));

	PG_RETURN_NULL();
}

Datum
vertex_out(PG_FUNCTION_ARGS)
{
	HeapTupleHeader vertex = PG_GETARG_HEAPTUPLEHEADER(0);
	Datum		values[Natts_vertex];
	bool		isnull[Natts_vertex];
	Graphid		id;
	Jsonb	   *prop_map;
	LabelOutData *my_extra;
	StringInfoData si;

	deform_tuple(vertex, values, isnull);

	if (isnull[Anum_vertex_id - 1])
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("id in vertex cannot be NULL")));
	if (isnull[Anum_vertex_properties - 1])
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("properties in vertex cannot be NULL")));

	id = DatumGetGraphid(values[Anum_vertex_id - 1]);
	prop_map = DatumGetJsonb(values[Anum_vertex_properties - 1]);

	my_extra = cache_label(fcinfo->flinfo, GraphidGetLabid(id));

	initStringInfo(&si);
	appendStringInfo(&si, "%s[" GRAPHID_FMTSTR "]",
					 NameStr(my_extra->label),
					 GraphidGetLabid(id), GraphidGetLocid(id));
	JsonbToCString(&si, &prop_map->root, VARSIZE(prop_map));

	PG_RETURN_CSTRING(si.data);
}

Datum
_vertex_out(PG_FUNCTION_ARGS)
{
	AnyArrayType *vertices = PG_GETARG_ANY_ARRAY(0);
	StringInfoData si;

	initStringInfo(&si);
	elems_out_si(&si, vertices, fcinfo->flinfo);

	PG_RETURN_CSTRING(si.data);
}

Datum
vertex_label(PG_FUNCTION_ARGS)
{
	Graphid		id;
	LabelOutData *my_extra;
	char	   *label;
	JsonbValue	jv;

	id = DatumGetGraphid(getVertexIdDatum(PG_GETARG_DATUM(0)));

	my_extra = cache_label(fcinfo->flinfo, GraphidGetLabid(id));

	label = NameStr(my_extra->label);

	jv.type = jbvString;
	jv.val.string.len = strlen(label);
	jv.val.string.val = label;

	PG_RETURN_JSONB(JsonbValueToJsonb(&jv));
}

Datum
_vertex_length(PG_FUNCTION_ARGS)
{
	AnyArrayType *vertices = PG_GETARG_ANY_ARRAY(0);
	int			nvertices;

	nvertices = ArrayGetNItems(AARR_NDIM(vertices), AARR_DIMS(vertices));

	PG_RETURN_JSONB(int_to_jsonb(nvertices));
}

static Jsonb *
int_to_jsonb(int i)
{
	Datum		n;
	JsonbValue	jv;

	n = DirectFunctionCall1(int4_numeric, Int32GetDatum(i));

	jv.type = jbvNumeric;
	jv.val.numeric = DatumGetNumeric(n);

	return JsonbValueToJsonb(&jv);
}

Datum
vtojb(PG_FUNCTION_ARGS)
{
	HeapTupleHeader vertex = PG_GETARG_HEAPTUPLEHEADER(0);

	PG_RETURN_DATUM(tuple_getattr(vertex, Anum_vertex_properties));
}

Datum
vertex_eq(PG_FUNCTION_ARGS)
{
	Datum		id1 = getVertexIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getVertexIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_eq, id1, id2));
}

Datum
vertex_ne(PG_FUNCTION_ARGS)
{
	Datum		id1 = getVertexIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getVertexIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_ne, id1, id2) != 0);
}

Datum
vertex_lt(PG_FUNCTION_ARGS)
{
	Datum		id1 = getVertexIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getVertexIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_lt, id1, id2) < 0);
}

Datum
vertex_gt(PG_FUNCTION_ARGS)
{
	Datum		id1 = getVertexIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getVertexIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_gt, id1, id2) > 0);
}

Datum
vertex_le(PG_FUNCTION_ARGS)
{
	Datum		id1 = getVertexIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getVertexIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_le, id1, id2) <= 0);
}

Datum
vertex_ge(PG_FUNCTION_ARGS)
{
	Datum		id1 = getVertexIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getVertexIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_ge, id1, id2) >= 0);
}

Datum
edge_out(PG_FUNCTION_ARGS)
{
	HeapTupleHeader edge = PG_GETARG_HEAPTUPLEHEADER(0);
	Datum		values[Natts_edge];
	bool		isnull[Natts_edge];
	Graphid		id;
	Jsonb	   *prop_map;
	LabelOutData *my_extra;
	StringInfoData si;

	deform_tuple(edge, values, isnull);

	if (isnull[Anum_edge_id - 1])
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("id in edge cannot be NULL")));
	if (isnull[Anum_edge_start - 1])
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("start in edge cannot be NULL")));
	if (isnull[Anum_edge_end - 1])
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("end in edge cannot be NULL")));
	if (isnull[Anum_edge_properties - 1])
		ereport(ERROR,
				(errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
				 errmsg("properties in edge cannot be NULL")));

	id = DatumGetGraphid(values[Anum_edge_id - 1]);
	prop_map = DatumGetJsonb(values[Anum_edge_properties - 1]);

	my_extra = cache_label(fcinfo->flinfo, GraphidGetLabid(id));

	initStringInfo(&si);
	appendStringInfo(&si, "%s[" GRAPHID_FMTSTR "][",
					 NameStr(my_extra->label),
					 GraphidGetLabid(id), GraphidGetLocid(id));
	graphid_out_si(&si, values[Anum_edge_start - 1]);
	appendStringInfoChar(&si, ',');
	graphid_out_si(&si, values[Anum_edge_end - 1]);
	appendStringInfoChar(&si, ']');
	JsonbToCString(&si, &prop_map->root, VARSIZE(prop_map));

	PG_RETURN_CSTRING(si.data);
}

Datum
_edge_out(PG_FUNCTION_ARGS)
{
	AnyArrayType *edges = PG_GETARG_ANY_ARRAY(0);
	StringInfoData si;

	initStringInfo(&si);
	elems_out_si(&si, edges, fcinfo->flinfo);

	PG_RETURN_CSTRING(si.data);
}

Datum
edge_label(PG_FUNCTION_ARGS)
{
	Graphid		id;
	LabelOutData *my_extra;
	char	   *label;
	JsonbValue	jv;

	id = DatumGetGraphid(getEdgeIdDatum(PG_GETARG_DATUM(0)));

	my_extra = cache_label(fcinfo->flinfo, GraphidGetLabid(id));

	label = NameStr(my_extra->label);

	jv.type = jbvString;
	jv.val.string.len = strlen(label);
	jv.val.string.val = label;

	PG_RETURN_JSONB(JsonbValueToJsonb(&jv));
}

Datum
_edge_length(PG_FUNCTION_ARGS)
{
	AnyArrayType *edges = PG_GETARG_ANY_ARRAY(0);
	int			nedges;

	nedges = ArrayGetNItems(AARR_NDIM(edges), AARR_DIMS(edges));

	PG_RETURN_JSONB(int_to_jsonb(nedges));
}

Datum
etojb(PG_FUNCTION_ARGS)
{
	HeapTupleHeader edge = PG_GETARG_HEAPTUPLEHEADER(0);

	PG_RETURN_DATUM(tuple_getattr(edge, Anum_edge_properties));
}

Datum
edge_eq(PG_FUNCTION_ARGS)
{
	Datum		id1 = getEdgeIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getEdgeIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_eq, id1, id2) == 0);
}

Datum
edge_ne(PG_FUNCTION_ARGS)
{
	Datum		id1 = getEdgeIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getEdgeIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_ne, id1, id2) != 0);
}

Datum
edge_lt(PG_FUNCTION_ARGS)
{
	Datum		id1 = getEdgeIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getEdgeIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_lt, id1, id2) < 0);
}

Datum
edge_gt(PG_FUNCTION_ARGS)
{
	Datum		id1 = getEdgeIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getEdgeIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_gt, id1, id2) > 0);
}

Datum
edge_le(PG_FUNCTION_ARGS)
{
	Datum		id1 = getEdgeIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getEdgeIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_le, id1, id2) <= 0);
}

Datum
edge_ge(PG_FUNCTION_ARGS)
{
	Datum		id1 = getEdgeIdDatum(PG_GETARG_DATUM(0));
	Datum		id2 = getEdgeIdDatum(PG_GETARG_DATUM(1));

	PG_RETURN_DATUM(DirectFunctionCall2(graphid_ge, id1, id2) >= 0);
}

static LabelOutData *
cache_label(FmgrInfo *flinfo, uint16 labid)
{
	MemoryContext oldMemoryContext;
	LabelOutData *my_extra;

	AssertArg(flinfo != NULL);

	oldMemoryContext = MemoryContextSwitchTo(flinfo->fn_mcxt);

	my_extra = (LabelOutData *) flinfo->fn_extra;
	if (my_extra == NULL)
	{
		flinfo->fn_extra = palloc(sizeof(*my_extra));
		my_extra = (LabelOutData *) flinfo->fn_extra;
		my_extra->label_labid = 0;
		MemSetLoop(NameStr(my_extra->label), '\0', sizeof(my_extra->label));
	}

	if (my_extra->label_labid != labid)
	{
		Oid			graphoid = get_graph_path_oid();
		char	   *label;

		label = get_labid_labname(graphoid, labid);
		if (label == NULL)
			elog(ERROR, "cache lookup failed for label %u", labid);

		my_extra->label_labid = labid;
		strncpy(NameStr(my_extra->label), label, sizeof(my_extra->label));
	}

	MemoryContextSwitchTo(oldMemoryContext);

	return my_extra;
}

static void
elems_out_si(StringInfo si, AnyArrayType *elems, FmgrInfo *flinfo)
{
	const char	delim = ',';
	ArrayMetaState *my_extra;
	int			nelems;
	array_iter	it;
	Datum		value;
	int			i;

	my_extra = (ArrayMetaState *) flinfo->fn_extra;
	if (my_extra == NULL)
	{
		flinfo->fn_extra = MemoryContextAlloc(flinfo->fn_mcxt,
											  sizeof(*my_extra));
		my_extra = (ArrayMetaState *) flinfo->fn_extra;
		get_elem_type_output(my_extra, AARR_ELEMTYPE(elems), flinfo->fn_mcxt);
	}

	nelems = ArrayGetNItems(AARR_NDIM(elems), AARR_DIMS(elems));

	appendStringInfoChar(si, '[');
	array_iter_setup(&it, elems);
	if (nelems > 0)
	{
		value = array_iter_next_(&it, 0, my_extra);
		appendStringInfoString(si, OutputFunctionCall(&my_extra->proc, value));
	}
	for (i = 1; i < nelems; i++)
	{
		appendStringInfoChar(si, delim);

		value = array_iter_next_(&it, i, my_extra);
		appendStringInfoString(si, OutputFunctionCall(&my_extra->proc, value));
	}
	appendStringInfoChar(si, ']');
}

Datum
graphpath_out(PG_FUNCTION_ARGS)
{
	const char	delim = ',';
	Datum		vertices_datum;
	Datum		edges_datum;
	AnyArrayType *vertices;
	AnyArrayType *edges;
	GraphpathOutData *my_extra;
	int			nvertices;
	int			nedges;
	StringInfoData si;
	array_iter	it_v;
	array_iter	it_e;
	Datum		value;
	int			i;

	getGraphpathArrays(PG_GETARG_DATUM(0), &vertices_datum, &edges_datum);

	vertices = DatumGetAnyArray(vertices_datum);
	edges = DatumGetAnyArray(edges_datum);

	/* cache vertex/edge output information */
	my_extra = (GraphpathOutData *) fcinfo->flinfo->fn_extra;
	if (my_extra == NULL)
	{
		fcinfo->flinfo->fn_extra = MemoryContextAlloc(fcinfo->flinfo->fn_mcxt,
													  sizeof(*my_extra));
		my_extra = (GraphpathOutData *) fcinfo->flinfo->fn_extra;
		get_elem_type_output(&my_extra->vertex, AARR_ELEMTYPE(vertices),
							 fcinfo->flinfo->fn_mcxt);
		get_elem_type_output(&my_extra->edge, AARR_ELEMTYPE(edges),
							 fcinfo->flinfo->fn_mcxt);
	}

	nvertices = ArrayGetNItems(AARR_NDIM(vertices), AARR_DIMS(vertices));
	nedges = ArrayGetNItems(AARR_NDIM(edges), AARR_DIMS(edges));
	if (nvertices != nedges + 1)
		ereport(ERROR,
				(errcode(ERRCODE_INTERNAL_ERROR),
				 errmsg("the numbers of vertices and edges are mismatched")));

	initStringInfo(&si);
	appendStringInfoChar(&si, '[');

	array_iter_setup(&it_v, vertices);
	array_iter_setup(&it_e, edges);

	value = array_iter_next_(&it_v, 0, &my_extra->vertex);
	appendStringInfoString(&si,
			OutputFunctionCall(&my_extra->vertex.proc, value));

	for (i = 0; i < nedges; i++)
	{
		appendStringInfoChar(&si, delim);

		value = array_iter_next_(&it_e, i, &my_extra->edge);
		appendStringInfoString(&si,
				OutputFunctionCall(&my_extra->edge.proc, value));

		appendStringInfoChar(&si, delim);

		value = array_iter_next_(&it_v, i + 1, &my_extra->vertex);
		appendStringInfoString(&si,
				OutputFunctionCall(&my_extra->vertex.proc, value));
	}

	appendStringInfoChar(&si, ']');

	PG_RETURN_CSTRING(si.data);
}

static void
get_elem_type_output(ArrayMetaState *state, Oid elem_type, MemoryContext mctx)
{
	get_type_io_data(elem_type, IOFunc_output,
					 &state->typlen, &state->typbyval, &state->typalign,
					 &state->typdelim, &state->typioparam, &state->typiofunc);
	fmgr_info_cxt(state->typiofunc, &state->proc, mctx);
}

static Datum
array_iter_next_(array_iter *it, int idx, ArrayMetaState *state)
{
	bool		isnull;
	Datum		value;

	value = array_iter_next(it, &isnull, idx,
							state->typlen, state->typbyval, state->typalign);
	Assert(!isnull);

	return value;
}

Datum
_graphpath_length(PG_FUNCTION_ARGS)
{
	AnyArrayType *graphpaths = PG_GETARG_ANY_ARRAY(0);
	int			ngraphpaths;

	ngraphpaths = ArrayGetNItems(AARR_NDIM(graphpaths), AARR_DIMS(graphpaths));

	PG_RETURN_JSONB(int_to_jsonb(ngraphpaths));
}

Datum
graphpath_length(PG_FUNCTION_ARGS)
{
	Datum		edges_datum;
	AnyArrayType *edges;
	int			nedges;

	edges_datum = DirectFunctionCall1(graphpath_edges, PG_GETARG_DATUM(0));
	edges = DatumGetAnyArray(edges_datum);
	nedges = ArrayGetNItems(AARR_NDIM(edges), AARR_DIMS(edges));

	PG_RETURN_JSONB(int_to_jsonb(nedges));
}

Datum
graphpath_vertices(PG_FUNCTION_ARGS)
{
	Datum vertices_datum;

	getGraphpathArrays(PG_GETARG_DATUM(0), &vertices_datum, NULL);

	PG_RETURN_DATUM(vertices_datum);
}

Datum
graphpath_edges(PG_FUNCTION_ARGS)
{
	Datum edges_datum;

	getGraphpathArrays(PG_GETARG_DATUM(0), NULL, &edges_datum);

	PG_RETURN_DATUM(edges_datum);
}

static void
deform_tuple(HeapTupleHeader tuphdr, Datum *values, bool *isnull)
{
	Oid			tupType;
	TupleDesc	tupDesc;
	HeapTupleData tuple;

	tupType = HeapTupleHeaderGetTypeId(tuphdr);
	tupDesc = lookup_rowtype_tupdesc(tupType, -1);

	tuple.t_len = HeapTupleHeaderGetDatumLength(tuphdr);
	ItemPointerSetInvalid(&tuple.t_self);
	tuple.t_tableOid = InvalidOid;
	tuple.t_data = tuphdr;

	heap_deform_tuple(&tuple, tupDesc, values, isnull);
	ReleaseTupleDesc(tupDesc);
}

static Datum
tuple_getattr(HeapTupleHeader tuphdr, int attnum)
{
	Oid			tupType;
	TupleDesc	tupDesc;
	HeapTupleData tuple;
	bool		isnull = false;
	Datum		attdat;

	tupType = HeapTupleHeaderGetTypeId(tuphdr);
	tupDesc = lookup_rowtype_tupdesc(tupType, -1);

	tuple.t_len = HeapTupleHeaderGetDatumLength(tuphdr);
	ItemPointerSetInvalid(&tuple.t_self);
	tuple.t_tableOid = InvalidOid;
	tuple.t_data = tuphdr;

	attdat = heap_getattr(&tuple, attnum, tupDesc, &isnull);
	ReleaseTupleDesc(tupDesc);
	Assert(!isnull);

	return attdat;
}

Datum
edge_start_vertex(PG_FUNCTION_ARGS)
{
	HeapTupleHeader edge = PG_GETARG_HEAPTUPLEHEADER(0);

	return getEdgeVertex(edge, EVK_START);
}

Datum
edge_end_vertex(PG_FUNCTION_ARGS)
{
	HeapTupleHeader edge = PG_GETARG_HEAPTUPLEHEADER(0);

	return getEdgeVertex(edge, EVK_END);
}

static Datum
getEdgeVertex(HeapTupleHeader edge, EdgeVertexKind evk)
{
	const char *querystr =
			"SELECT (" AG_ELEM_LOCAL_ID ", " AG_ELEM_PROP_MAP ")::vertex "
			"FROM \"%s\"." AG_VERTEX " WHERE " AG_ELEM_LOCAL_ID " = $1";
	char		sqlcmd[256];
	int			attnum = (evk == EVK_START ? Anum_edge_start : Anum_edge_end);
	Datum		values[1];
	Oid			argTypes[1] = {GRAPHIDOID};
	bool		spi_pushed;
	int			ret;
	Datum		vertex;
	bool		isnull;

	snprintf(sqlcmd, sizeof(sqlcmd), querystr, get_graph_path(false));

	values[0] = tuple_getattr(edge, attnum);

	spi_pushed = SPI_push_conditional();

	if (SPI_connect() != SPI_OK_CONNECT)
		elog(ERROR, "SPI_connect failed");

	ret = SPI_execute_with_args(sqlcmd, 2, argTypes, values, NULL, false, 0);
	if (ret != SPI_OK_SELECT)
		elog(ERROR, "SPI_execute failed: %s", sqlcmd);

	if (SPI_processed != 1)
		elog(ERROR, (evk == EVK_START
					 ? "SPI_execute: only one start vertex of edge exists"
					 : "SPI_execute: only one end vertex of edge exists"));

	vertex = SPI_getbinval(SPI_tuptable->vals[0], SPI_tuptable->tupdesc,
						   1, &isnull);
	Assert(!isnull);

	vertex = SPI_datumTransfer(vertex, false, -1);

	if (SPI_finish() != SPI_OK_FINISH)
		elog(ERROR, "SPI_finish failed");

	SPI_pop_conditional(spi_pushed);

	return vertex;
}

Datum
vertex_labels(PG_FUNCTION_ARGS)
{
	Graphid          id;
	JsonbValue       jv;
	JsonbParseState *jpstate = NULL;
	JsonbValue      *ajv;
	Oid              graphoid = get_graph_path_oid();
	Oid              labRelid;
	List            *ancestorRelids;
	ListCell        *l;

	/* get relation ids of ancestor label */
	id = DatumGetGraphid(getVertexIdDatum(PG_GETARG_DATUM(0)));
	labRelid = get_labid_relid(graphoid, GraphidGetLabid(id));
	ancestorRelids = find_all_ancestors( labRelid, AccessShareLock, NULL );

	pushJsonbValue(&jpstate, WJB_BEGIN_ARRAY, NULL);

	foreach(l, ancestorRelids)
	{
		Oid     ancestorRelid = lfirst_oid(l);
		uint16  ancestorLabid;
		char   *ancestorLabname;

		ancestorLabid   = get_relid_labid(ancestorRelid);
		ancestorLabname = get_labid_labname(graphoid, ancestorLabid);
		if (ancestorLabname == NULL)
			elog(ERROR, "cache lookup failed for label %u", ancestorLabid);

		if (strcmp(ancestorLabname, "ag_vertex") != 0)
		{
			jv.type = jbvString;
			jv.val.string.len = strlen(ancestorLabname);
			jv.val.string.val = ancestorLabname;

			pushJsonbValue(&jpstate, WJB_ELEM, &jv);
		}


	}

	ajv = pushJsonbValue(&jpstate, WJB_END_ARRAY, NULL);

	PG_RETURN_JSONB(JsonbValueToJsonb(ajv));
}

Datum
getVertexIdDatum(Datum datum)
{
	HeapTupleHeader	tuphdr = DatumGetHeapTupleHeader(datum);

	return tuple_getattr(tuphdr, Anum_vertex_id);
}


Datum
getVertexPropDatum(Datum datum)
{
	HeapTupleHeader	tuphdr = DatumGetHeapTupleHeader(datum);

	return tuple_getattr(tuphdr, Anum_vertex_properties);
}

Datum
getEdgeIdDatum(Datum datum)
{
	HeapTupleHeader	tuphdr = DatumGetHeapTupleHeader(datum);

	return tuple_getattr(tuphdr, Anum_edge_id);
}

Datum
getEdgeStartDatum(Datum datum)
{
	HeapTupleHeader	tuphdr = DatumGetHeapTupleHeader(datum);

	return tuple_getattr(tuphdr, Anum_edge_start);
}

Datum
getEdgeEndDatum(Datum datum)
{
	HeapTupleHeader	tuphdr = DatumGetHeapTupleHeader(datum);

	return tuple_getattr(tuphdr, Anum_edge_end);
}

Datum
getEdgePropDatum(Datum datum)
{
	HeapTupleHeader	tuphdr = DatumGetHeapTupleHeader(datum);

	return tuple_getattr(tuphdr, Anum_edge_properties);
}

void
getGraphpathArrays(Datum graphpath, Datum *vertices, Datum *edges)
{
	HeapTupleHeader	tuphdr;
	Oid			tupType;
	TupleDesc	tupDesc;
	HeapTupleData tuple;
	Datum		values[Natts_graphpath];
	bool		isnull[Natts_graphpath];

	tuphdr = DatumGetHeapTupleHeader(graphpath);

	tupType = HeapTupleHeaderGetTypeId(tuphdr);
	Assert(tupType == GRAPHPATHOID);

	tupDesc = lookup_rowtype_tupdesc(tupType, -1);
	Assert(tupDesc->natts == Natts_graphpath);

	tuple.t_len = HeapTupleHeaderGetDatumLength(tuphdr);
	ItemPointerSetInvalid(&tuple.t_self);
	tuple.t_tableOid = InvalidOid;
	tuple.t_data = tuphdr;

	heap_deform_tuple(&tuple, tupDesc, values, isnull);
	ReleaseTupleDesc(tupDesc);
	Assert(!isnull[Anum_graphpath_vertices - 1]);
	Assert(!isnull[Anum_graphpath_edges - 1]);

	if (vertices != NULL)
		*vertices = values[Anum_graphpath_vertices - 1];
	if (edges != NULL)
		*edges = values[Anum_graphpath_edges - 1];
}

Datum
makeGraphpathDatum(Datum *vertices, int nvertices, Datum *edges, int nedges)
{
	Datum		values[Natts_graphpath];
	bool		isnull[Natts_graphpath] = {false, false};
	TupleDesc	tupDesc;
	HeapTuple	graphpath;

	values[Anum_graphpath_vertices - 1]
					= makeArrayTypeDatum(vertices, nvertices, VERTEXOID);
	values[Anum_graphpath_edges - 1]
					= makeArrayTypeDatum(edges, nedges, EDGEOID);

	tupDesc = lookup_rowtype_tupdesc(GRAPHPATHOID, -1);
	Assert(tupDesc->natts == Natts_graphpath);

	graphpath = heap_form_tuple(tupDesc, values, isnull);

	ReleaseTupleDesc(tupDesc);

	return HeapTupleGetDatum(graphpath);
}

Datum
makeGraphVertexDatum(Datum id, Datum prop_map)
{
	Datum		values[Natts_vertex];
	bool		isnull[Natts_vertex] = {false, false};
	TupleDesc	tupDesc;
	HeapTuple	vertex;

	values[Anum_vertex_id - 1] = id;
	values[Anum_vertex_properties - 1] = prop_map;

	tupDesc = lookup_rowtype_tupdesc(VERTEXOID, -1);
	Assert(tupDesc->natts == Natts_vertex);

	vertex = heap_form_tuple(tupDesc, values, isnull);

	ReleaseTupleDesc(tupDesc);

	return HeapTupleGetDatum(vertex);
}

Datum
makeGraphEdgeDatum(Datum id, Datum start, Datum end, Datum prop_map)
{
	Datum		values[Natts_edge];
	bool		isnull[Natts_edge] = {false, false, false, false};
	TupleDesc	tupDesc;
	HeapTuple	edge;

	values[Anum_edge_id - 1] = id;
	values[Anum_edge_start - 1] = start;
	values[Anum_edge_end - 1] = end;
	values[Anum_edge_properties - 1] = prop_map;

	tupDesc = lookup_rowtype_tupdesc(EDGEOID, -1);
	Assert(tupDesc->natts == Natts_edge);

	edge = heap_form_tuple(tupDesc, values, isnull);

	ReleaseTupleDesc(tupDesc);

	return HeapTupleGetDatum(edge);
}

static Datum
makeArrayTypeDatum(Datum *elems, int nelem, Oid type)
{
	int16		typlen;
	bool		typbyval;
	char		typalign;
	ArrayType  *arr;

	get_typlenbyvalalign(type, &typlen, &typbyval, &typalign);

	arr = construct_array(elems, nelem, type, typlen, typbyval, typalign);

	return PointerGetDatum(arr);
}

/*
 * BTree support functions
 */

/* BTORDER_PROC (1) */
Datum
btgraphidcmp(PG_FUNCTION_ARGS)
{
	PG_RETURN_INT32(graphid_cmp(fcinfo));
}

/*
 * Hash support functions
 */

/* HASHPROC (1) */
Datum
graphid_hash(PG_FUNCTION_ARGS)
{
	Graphid id = PG_GETARG_GRAPHID(0);

	StaticAssertStmt(sizeof(id) == 8, "the size of graphid must be 8");

	return hash_any((unsigned char *) &id, sizeof(id));
}

/*
 * GIN (as BTree) support functions
 */

/* Note: GIN_COMPARE_PROC (1) is btgraphidcmp() */

/* GIN_EXTRACTVALUE_PROC (2) - called by ginExtractEntries() */
Datum
gin_extract_value_graphid(PG_FUNCTION_ARGS)
{
	const int32	_nentries = 1;
	Datum		graphid = PG_GETARG_DATUM(0);
	int32	   *nentries = (int32 *) PG_GETARG_POINTER(1);
	Datum	   *entries = palloc(sizeof(*entries) * _nentries);

	*nentries = _nentries;
	entries[0] = graphid;

	PG_RETURN_POINTER(entries);
}

/*
 * GIN_EXTRACTQUERY_PROC (3) - called by ginNewScanKey()
 *
 * GIN does not have a fixed set of strategies. Instead, the support routines
 * of each operator class interpret the strategy numbers.
 * We use strategy numbers of BTree.
 *
 * nullFlags and searchMode will be set by the caller.
 */
Datum
gin_extract_query_graphid(PG_FUNCTION_ARGS)
{
	const int32	_nentries = 1;
	Datum		graphid = PG_GETARG_DATUM(0);
	int32	   *nentries = (int32 *) PG_GETARG_POINTER(1);
	StrategyNumber strategy = PG_GETARG_UINT16(2);
	bool	  **partial_matches = (bool **) PG_GETARG_POINTER(3);
	Pointer   **extra_data = (Pointer **) PG_GETARG_POINTER(4);
	Datum	   *entries;

	*nentries = _nentries;
	*partial_matches = palloc(sizeof(**partial_matches) * _nentries);
	entries = palloc(sizeof(*entries) * _nentries);

	switch (strategy)
	{
		case BTLessStrategyNumber:
		case BTLessEqualStrategyNumber:
			/*
			 * We should start scan from the smallest indexed key until the
			 * scan meets the given graphid. To do this, we set the
			 * entry(query value) to the minimum value of graphid (to make GIN
			 * to find the smallest indexed key), enable partial match, and
			 * store the original graphid into the extra_data for later use in
			 * partial match.
			 */
			{
				(*partial_matches)[0] = true;

				*extra_data = palloc(sizeof(**extra_data) * _nentries);
				(*extra_data)[0] = DatumGetPointer(graphid);

				entries[0] = graphid_minval();
			}
			break;
		case BTEqualStrategyNumber:
			/* exact match */
			(*partial_matches)[0] = false;
			entries[0] = graphid;
			break;
		case BTGreaterEqualStrategyNumber:
		case BTGreaterStrategyNumber:
			(*partial_matches)[0] = true;
			entries[0] = graphid;
			break;
		default:
			elog(ERROR, "unrecognized strategy number: %d", strategy);
	}

	PG_RETURN_POINTER(entries);
}

static Datum
graphid_minval(void)
{
	Graphid id;

	GraphidSet(&id, 0, 0);

	return GraphidGetDatum(id);
}

/* GIN_CONSISTENT_PROC (4) - same as trueConsistentFn() */
Datum
gin_consistent_graphid(PG_FUNCTION_ARGS)
{
	bool *recheck = (bool *) PG_GETARG_POINTER(5);

	*recheck = false;
	PG_RETURN_BOOL(true);
}

/*
 * GIN_COMPARE_PARTIAL_PROC (5)
 *
 * See collectMatchBitmap() for the caller's context
 */
Datum
gin_compare_partial_graphid(FunctionCallInfo fcinfo)
{
	Datum		qrykey = PG_GETARG_DATUM(0);
	Datum		idxkey = PG_GETARG_DATUM(1);
	StrategyNumber strategy = PG_GETARG_UINT16(2);
	Datum		graphid = PG_GETARG_DATUM(3);
	int32		cmp;
	int32		res;

	/*
	 * In these cases, qrykey is the minimum value of graphid. To compare
	 * qrykey with idxkey properly, restore the original graphid from the
	 * extra_data, and set it to qrykey.
	 */
	if (strategy == BTLessStrategyNumber ||
		strategy == BTLessEqualStrategyNumber)
		qrykey = graphid;

	cmp = DatumGetInt32(DirectFunctionCall2Coll(btgraphidcmp,
												PG_GET_COLLATION(),
												idxkey, qrykey));

	switch (strategy)
	{
		case BTLessStrategyNumber:
			/* idxkey < qrykey ? still match : finish scan */
			res = (cmp < 0 ? 0 : 1);
			break;
		case BTLessEqualStrategyNumber:
			res = (cmp <= 0 ? 0 : 1);
			break;
		case BTEqualStrategyNumber:
			res = (cmp == 0 ? 0 : 1);
			break;
		case BTGreaterEqualStrategyNumber:
			res = (cmp >= 0 ? 0 : 1);
			break;
		case BTGreaterStrategyNumber:
			if (cmp > 0)
				res = 0;
			else if (cmp == 0)
				res = -1;		/* not match, continue scan */
			else
				res = 1;
			break;
		default:
			elog(ERROR, "unrecognized strategy number: %d", strategy);
			res = 0;
	}

	PG_RETURN_INT32(res);
}
