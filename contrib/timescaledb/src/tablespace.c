#include <postgres.h>
#include <fmgr.h>
#include <utils/lsyscache.h>
#include <utils/spccache.h>
#include <utils/acl.h>
#include <utils/builtins.h>
#include <utils/fmgroids.h>
#include <commands/tablespace.h>
#include <access/xact.h>
#include <miscadmin.h>
#include <funcapi.h>

#include "hypertable_cache.h"
#include "errors.h"
#include "catalog.h"
#include "scanner.h"
#include "tablespace.h"
#include "compat.h"

#define TABLESPACE_DEFAULT_CAPACITY 4

static Tablespaces *
tablespaces_alloc(int capacity)
{
	Tablespaces *tspcs;

	tspcs = palloc(sizeof(Tablespaces));
	tspcs->capacity = capacity;
	tspcs->num_tablespaces = 0;
	tspcs->tablespaces = palloc(sizeof(Tablespace) * tspcs->capacity);

	return tspcs;
}

Tablespace *
tablespaces_add(Tablespaces *tspcs, FormData_tablespace *form, Oid tspc_oid)
{
	Tablespace *tspc;

	if (tspcs->num_tablespaces >= tspcs->capacity)
	{
		tspcs->capacity += TABLESPACE_DEFAULT_CAPACITY;
		tspcs->tablespaces = repalloc(tspcs->tablespaces, sizeof(Tablespace) * tspcs->capacity);
	}

	tspc = &tspcs->tablespaces[tspcs->num_tablespaces++];
	memcpy(&tspc->fd, form, sizeof(FormData_tablespace));
	tspc->tablespace_oid = tspc_oid;

	return tspc;
}

int
tablespaces_clear(Tablespaces *tspcs)
{
	int			num = tspcs->num_tablespaces;

	tspcs->num_tablespaces = 0;

	return num;
}

bool
tablespaces_delete(Tablespaces *tspcs, Oid tspc_oid)
{
	int			i;

	for (i = 0; i < tspcs->num_tablespaces; i++)
	{
		if (tspc_oid == tspcs->tablespaces[i].tablespace_oid)
		{
			memcpy(&tspcs->tablespaces[i],
				   &tspcs->tablespaces[i + 1],
				   sizeof(Tablespace) * (tspcs->num_tablespaces - i - 1));
			tspcs->num_tablespaces--;
			return true;
		}
	}

	return false;
}

bool
tablespaces_contain(Tablespaces *tspcs, Oid tspc_oid)
{
	int			i;

	for (i = 0; i < tspcs->num_tablespaces; i++)
		if (tspc_oid == tspcs->tablespaces[i].tablespace_oid)
			return true;

	return false;
}


static bool
tablespace_tuple_found(TupleInfo *ti, void *data)
{
	Tablespaces *tspcs = data;
	FormData_tablespace *form = (FormData_tablespace *) GETSTRUCT(ti->tuple);
	Oid			tspcoid = get_tablespace_oid(NameStr(form->tablespace_name), true);

	if (NULL != tspcs)
		tablespaces_add(tspcs, form, tspcoid);

	return true;
}

static int
tablespace_scan_internal(int indexid,
						 ScanKeyData *scankey,
						 int nkeys,
						 tuple_found_func tuple_found,
						 tuple_found_func tuple_filter,
						 void *data,
						 int limit,
						 LOCKMODE lockmode)
{
	Catalog    *catalog = catalog_get();
	ScannerCtx	scanctx = {
		.table = catalog->tables[TABLESPACE].id,
		.index = CATALOG_INDEX(catalog, TABLESPACE, indexid),
		.nkeys = nkeys,
		.scankey = scankey,
		.tuple_found = tuple_found,
		.filter = tuple_filter,
		.data = data,
		.limit = limit,
		.lockmode = lockmode,
		.scandirection = ForwardScanDirection,
	};

	return scanner_scan(&scanctx);
}

Tablespaces *
tablespace_scan(int32 hypertable_id)
{
	Tablespaces *tspcs = tablespaces_alloc(TABLESPACE_DEFAULT_CAPACITY);
	ScanKeyData scankey[1];

	ScanKeyInit(&scankey[0], Anum_tablespace_hypertable_id_tablespace_name_idx_hypertable_id,
				BTEqualStrategyNumber, F_INT4EQ, Int32GetDatum(hypertable_id));

	tablespace_scan_internal(TABLESPACE_HYPERTABLE_ID_TABLESPACE_NAME_IDX,
							 scankey,
							 1,
							 tablespace_tuple_found,
							 NULL,
							 tspcs,
							 0,
							 AccessShareLock);

	return tspcs;
}


typedef struct TablespaceScanInfo
{
	Catalog    *catalog;
	Cache	   *hcache;
	Oid			userid;
	int			num_filtered;
	int			stopcount;
	void	   *data;
} TablespaceScanInfo;

static int
tablespace_scan_by_name(const char *tspcname, tuple_found_func tuple_found, void *data)
{
	ScanKeyData scankey[1];
	int			nkeys = 0;

	if (NULL != tspcname)
		ScanKeyInit(&scankey[nkeys++],
					Anum_tablespace_tablespace_name,
					BTEqualStrategyNumber, F_NAMEEQ,
					DirectFunctionCall1(namein, CStringGetDatum(tspcname)));

	return tablespace_scan_internal(INVALID_INDEXID,
									scankey,
									nkeys,
									tuple_found,
									NULL,
									data,
									0,
									AccessShareLock);
}

int
tablespace_count_attached(const char *tspcname)
{
	return tablespace_scan_by_name(tspcname, NULL, NULL);
}

static void
tablespace_validate_revoke_internal(const char *tspcname, tuple_found_func tuple_found, void *stmt)
{
	TablespaceScanInfo info = {
		.catalog = catalog_get(),
		.hcache = hypertable_cache_pin(),
		.data = stmt,
	};

	tablespace_scan_by_name(tspcname, tuple_found, &info);

	cache_release(info.hcache);
}

static void
validate_revoke_create(Oid tspcoid, Oid role, Oid relid)
{
	AclResult	aclresult = pg_tablespace_aclcheck(tspcoid, role, ACL_CREATE);

	if (aclresult != ACLCHECK_OK)
		ereport(ERROR,
				(errcode(ERRCODE_DEPENDENT_OBJECTS_STILL_EXIST),
				 errmsg("cannot revoke privilege while tablespace \"%s\" is attached to hypertable \"%s\"",
						get_tablespace_name(tspcoid), get_rel_name(relid)),
				 errhint("Detach the tablespace before revoking the privilege on it.")));
}

/*
 * Verify that the REVOKE of permissions on a tablespace does not make it
 * impossible to use the tablespace for new chunks.
 *
 * This check should be done after the REVOKE has been applied.
 */
static bool
revoke_tuple_found(TupleInfo *ti, void *data)
{
	TablespaceScanInfo *info = data;
	GrantStmt  *stmt = info->data;
	ListCell   *lc_role;
	Form_tablespace form = (Form_tablespace) GETSTRUCT(ti->tuple);
	Oid			tspcoid = get_tablespace_oid(NameStr(form->tablespace_name), false);
	Hypertable *ht = hypertable_cache_get_entry_by_id(info->hcache, form->hypertable_id);
	Oid			relowner = rel_get_owner(ht->main_table_relid);

	foreach(lc_role, stmt->grantees)
	{
		RoleSpec   *role = lfirst(lc_role);
		Oid			roleoid = get_role_oid_or_public(role->rolename);

		/* Check if this is a role we're interested in */
		if (!OidIsValid(roleoid))
			continue;

		/*
		 * A revoke on a tablespace can only be for 'CREATE' (or ALL), so no
		 * need to check which privilege is revoked.
		 */
		validate_revoke_create(tspcoid, relowner, ht->main_table_relid);
	}

	return true;
}

void
tablespace_validate_revoke(GrantStmt *stmt)
{
	tablespace_validate_revoke_internal(strVal(linitial(stmt->objects)),
										revoke_tuple_found, stmt);
}

/*
 * Verify that the REVOKE of a role on a tablespace does not make it impossible
 * to use the tablespace for new chunks.
 *
 * This check should be done after the REVOKE has been applied.
 */
static bool
revoke_role_tuple_found(TupleInfo *ti, void *data)
{
	TablespaceScanInfo *info = data;
	GrantRoleStmt *stmt = info->data;
	Form_tablespace form = (Form_tablespace) GETSTRUCT(ti->tuple);
	Oid			tspcoid = get_tablespace_oid(NameStr(form->tablespace_name), false);
	Hypertable *ht = hypertable_cache_get_entry_by_id(info->hcache, form->hypertable_id);
	Oid			relowner = rel_get_owner(ht->main_table_relid);
	ListCell   *lc_role;

	foreach(lc_role, stmt->grantee_roles)
	{
		RoleSpec   *rolespec = lfirst(lc_role);
#if PG96
		Oid			grantee = get_rolespec_oid((Node *) rolespec, true);
#else
		Oid			grantee = get_rolespec_oid(rolespec, true);
#endif
		/* Only interested in revokes on table owners */
		if (grantee != relowner)
			continue;

		/*
		 * No need to check which role that was revoked since we are only
		 * interested in the resulting permissions for the table owner. A
		 * table owner could have CREATE on the tablespace from multiple
		 * roles, so revoking one of those roles might not mean the owner no
		 * longer has CREATE on the tablespace.
		 */
		validate_revoke_create(tspcoid, relowner, ht->main_table_relid);
	}

	return true;
}

void
tablespace_validate_revoke_role(GrantRoleStmt *stmt)
{
	tablespace_validate_revoke_internal(NULL, revoke_role_tuple_found, stmt);
}

static int32
tablespace_insert_relation(Relation rel, int32 hypertable_id, const char *tspcname)
{
	TupleDesc	desc = RelationGetDescr(rel);
	Datum		values[Natts_tablespace];
	bool		nulls[Natts_tablespace] = {false};
	int32		id;

	memset(values, 0, sizeof(values));
	id = catalog_table_next_seq_id(catalog_get(), TABLESPACE);
	values[Anum_tablespace_id - 1] = Int32GetDatum(id);
	values[Anum_tablespace_hypertable_id - 1] = Int32GetDatum(hypertable_id);
	values[Anum_tablespace_tablespace_name - 1] =
		DirectFunctionCall1(namein, CStringGetDatum(tspcname));

	catalog_insert_values(rel, desc, values, nulls);

	return id;
}

static int32
tablespace_insert(int32 hypertable_id, const char *tspcname)
{
	Catalog    *catalog = catalog_get();
	Relation	rel;
	int32		id;

	rel = heap_open(catalog->tables[TABLESPACE].id, RowExclusiveLock);
	id = tablespace_insert_relation(rel, hypertable_id, tspcname);
	heap_close(rel, RowExclusiveLock);

	return id;
}

static bool
tablespace_tuple_delete(TupleInfo *ti, void *data)
{
	TablespaceScanInfo *info = data;
	CatalogSecurityContext sec_ctx;

	catalog_become_owner(info->catalog, &sec_ctx);
	catalog_delete_only(ti->scanrel, ti->tuple);
	catalog_restore_user(&sec_ctx);

	return (info->stopcount == 0 || ti->count < info->stopcount);
}

int
tablespace_delete(int32 hypertable_id, const char *tspcname)

{
	ScanKeyData scankey[2];
	TablespaceScanInfo info = {
		.catalog = catalog_get(),
		.stopcount = (NULL != tspcname),
	};
	int			num_deleted,
				nkeys = 0;

	ScanKeyInit(&scankey[nkeys++],
				Anum_tablespace_hypertable_id_tablespace_name_idx_hypertable_id,
				BTEqualStrategyNumber,
				F_INT4EQ,
				Int32GetDatum(hypertable_id));

	if (NULL != tspcname)
		ScanKeyInit(&scankey[nkeys++],
					Anum_tablespace_hypertable_id_tablespace_name_idx_tablespace_name,
					BTEqualStrategyNumber,
					F_NAMEEQ,
					DirectFunctionCall1(namein, CStringGetDatum(tspcname)));

	num_deleted = tablespace_scan_internal(TABLESPACE_HYPERTABLE_ID_TABLESPACE_NAME_IDX,
										   scankey,
										   nkeys,
										   tablespace_tuple_delete,
										   NULL,
										   &info,
										   0,
										   RowExclusiveLock);

	if (num_deleted > 0)
		CommandCounterIncrement();

	return num_deleted;
}

static bool
tablespace_tuple_owner_filter(TupleInfo *ti, void *data)
{
	TablespaceScanInfo *info = data;
	FormData_tablespace *form = (FormData_tablespace *) GETSTRUCT(ti->tuple);
	Hypertable *ht;

	ht = hypertable_cache_get_entry_by_id(info->hcache, form->hypertable_id);

	Assert(NULL != ht);

	if (hypertable_has_privs_of(ht->main_table_relid, info->userid))
		return true;

	info->num_filtered++;

	return false;
}

static int
tablespace_delete_from_all(const char *tspcname, Oid userid)
{
	ScanKeyData scankey[1];
	TablespaceScanInfo info = {
		.catalog = catalog_get(),
		.hcache = hypertable_cache_pin(),
		.userid = userid,
	};
	int			num_deleted;

	ScanKeyInit(&scankey[0],
				Anum_tablespace_tablespace_name,
				BTEqualStrategyNumber, F_NAMEEQ,
				DirectFunctionCall1(namein, CStringGetDatum(tspcname)));

	num_deleted = tablespace_scan_internal(INVALID_INDEXID,
										   scankey,
										   1,
										   tablespace_tuple_delete,
										   tablespace_tuple_owner_filter,
										   &info,
										   0,
										   RowExclusiveLock);
	cache_release(info.hcache);

	if (num_deleted > 0)
		CommandCounterIncrement();

	if (info.num_filtered > 0)
		ereport(NOTICE,
				(errmsg("tablespace \"%s\" remains attached to %d hypertable(s) due to lack of permissions",
						tspcname, info.num_filtered)));

	return num_deleted;
}

TS_FUNCTION_INFO_V1(tablespace_attach);

Datum
tablespace_attach(PG_FUNCTION_ARGS)
{
	Name		tspcname = PG_ARGISNULL(0) ? NULL : PG_GETARG_NAME(0);
	Oid			hypertable_oid = PG_ARGISNULL(1) ? InvalidOid : PG_GETARG_OID(1);
	bool		if_not_attached = PG_ARGISNULL(2) ? false : PG_GETARG_BOOL(2);

	if (PG_NARGS() < 2 || PG_NARGS() > 3)
		elog(ERROR, "invalid number of arguments");

	tablespace_attach_internal(tspcname, hypertable_oid, if_not_attached);

	PG_RETURN_VOID();
}

void
tablespace_attach_internal(Name tspcname, Oid hypertable_oid, bool if_not_attached)
{
	Cache	   *hcache;
	Hypertable *ht;
	Oid			tspc_oid;
	Oid			ownerid;
	AclResult	aclresult;
	CatalogSecurityContext sec_ctx;

	if (NULL == tspcname)
		elog(ERROR, "invalid tablespace name");

	if (!OidIsValid(hypertable_oid))
		elog(ERROR, "invalid hypertable");

	tspc_oid = get_tablespace_oid(NameStr(*tspcname), true);

	if (!OidIsValid(tspc_oid))
		ereport(ERROR,
				(errcode(ERRCODE_UNDEFINED_OBJECT),
				 errmsg("tablespace \"%s\" does not exist", NameStr(*tspcname)),
				 errhint("The tablespace needs to be created"
						 " before attaching it to a hypertable.")));

	ownerid = hypertable_permissions_check(hypertable_oid, GetUserId());

	/*
	 * Note that we check against the table owner rather than the current user
	 * here, since we're not actually creating a table using this tablespace
	 * at this point
	 */
	aclresult = pg_tablespace_aclcheck(tspc_oid, ownerid, ACL_CREATE);

	if (aclresult != ACLCHECK_OK)
		ereport(ERROR,
				(errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
				 errmsg("permission denied for tablespace \"%s\" by table owner \"%s\"",
						NameStr(*tspcname), GetUserNameFromId(ownerid, true))));

	hcache = hypertable_cache_pin();
	ht = hypertable_cache_get_entry(hcache, hypertable_oid);

	if (NULL == ht)
		ereport(ERROR,
				(errcode(ERRCODE_IO_HYPERTABLE_NOT_EXIST),
				 errmsg("table \"%s\" is not a hypertable",
						get_rel_name(hypertable_oid))));

	if (hypertable_has_tablespace(ht, tspc_oid))
	{
		if (if_not_attached)
			ereport(NOTICE,
					(errcode(ERRCODE_IO_TABLESPACE_ALREADY_ATTACHED),
					 errmsg("tablespace \"%s\" is already attached to hypertable \"%s\", skipping",
							NameStr(*tspcname), get_rel_name(hypertable_oid))));
		else
			ereport(ERROR,
					(errcode(ERRCODE_IO_TABLESPACE_ALREADY_ATTACHED),
					 errmsg("tablespace \"%s\" is already attached to hypertable \"%s\"",
							NameStr(*tspcname), get_rel_name(hypertable_oid))));
	}
	else
	{
		catalog_become_owner(catalog_get(), &sec_ctx);
		tablespace_insert(ht->fd.id, NameStr(*tspcname));
		catalog_restore_user(&sec_ctx);
	}

	cache_release(hcache);
}

static int
tablespace_detach_one(Oid hypertable_oid, const char *tspcname, Oid tspcoid, bool if_attached)
{
	Cache	   *hcache;
	Hypertable *ht;
	int			ret = 0;

	hypertable_permissions_check(hypertable_oid, GetUserId());

	hcache = hypertable_cache_pin();
	ht = hypertable_cache_get_entry(hcache, hypertable_oid);

	if (NULL == ht)
		ereport(ERROR,
				(errcode(ERRCODE_IO_HYPERTABLE_NOT_EXIST),
				 errmsg("table \"%s\" is not a hypertable",
						get_rel_name(hypertable_oid))));

	if (hypertable_has_tablespace(ht, tspcoid))
		ret = tablespace_delete(ht->fd.id, tspcname);
	else if (if_attached)
		ereport(NOTICE,
				(errcode(ERRCODE_IO_TABLESPACE_NOT_ATTACHED),
				 errmsg("tablespace \"%s\" is not attached to hypertable \"%s\", skipping",
						tspcname, get_rel_name(hypertable_oid))));
	else
		ereport(ERROR,
				(errcode(ERRCODE_IO_TABLESPACE_NOT_ATTACHED),
				 errmsg("tablespace \"%s\" is not attached to hypertable \"%s\"",
						tspcname, get_rel_name(hypertable_oid))));

	cache_release(hcache);

	return ret;
}

static int
tablespace_detach_all(Oid hypertable_oid)
{
	Cache	   *hcache;
	Hypertable *ht;
	int			ret;

	hypertable_permissions_check(hypertable_oid, GetUserId());

	hcache = hypertable_cache_pin();
	ht = hypertable_cache_get_entry(hcache, hypertable_oid);

	if (NULL == ht)
		ereport(ERROR,
				(errcode(ERRCODE_IO_HYPERTABLE_NOT_EXIST),
				 errmsg("table \"%s\" is not a hypertable",
						get_rel_name(hypertable_oid))));

	ret = tablespace_delete(ht->fd.id, NULL);

	cache_release(hcache);

	return ret;
}

TS_FUNCTION_INFO_V1(tablespace_detach);

Datum
tablespace_detach(PG_FUNCTION_ARGS)
{
	Name		tspcname = PG_ARGISNULL(0) ? NULL : PG_GETARG_NAME(0);
	Oid			hypertable_oid = PG_ARGISNULL(1) ? InvalidOid : PG_GETARG_OID(1);
	bool		if_attached = PG_ARGISNULL(2) ? false : PG_GETARG_BOOL(2);
	Oid			tspcoid;
	int			ret;

	if (PG_NARGS() < 1 || PG_NARGS() > 3)
		elog(ERROR, "invalid number of arguments");

	if (NULL == tspcname)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("invalid tablespace name")));

	if (!PG_ARGISNULL(1) && !OidIsValid(hypertable_oid))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				 errmsg("invalid hypertable")));

	tspcoid = get_tablespace_oid(NameStr(*tspcname), true);

	if (!OidIsValid(tspcoid))
		ereport(ERROR,
				(errcode(ERRCODE_UNDEFINED_OBJECT),
				 errmsg("tablespace \"%s\" doss not exist",
						NameStr(*tspcname))));

	if (OidIsValid(hypertable_oid))
		ret = tablespace_detach_one(hypertable_oid, NameStr(*tspcname), tspcoid, if_attached);
	else
		ret = tablespace_delete_from_all(NameStr(*tspcname), GetUserId());

	PG_RETURN_INT32(ret);
}

TS_FUNCTION_INFO_V1(tablespace_detach_all_from_hypertable);

Datum
tablespace_detach_all_from_hypertable(PG_FUNCTION_ARGS)
{
	if (PG_NARGS() != 1)
		elog(ERROR, "invalid number of arguments");

	if (PG_ARGISNULL(0))
		elog(ERROR, "invalid argument");

	PG_RETURN_INT32(tablespace_detach_all(PG_GETARG_OID(0)));
}

TS_FUNCTION_INFO_V1(tablespace_show);

Datum
tablespace_show(PG_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	Oid			hypertable_oid = PG_ARGISNULL(0) ? InvalidOid : PG_GETARG_OID(0);
	Cache	   *hcache;
	Hypertable *ht;
	Tablespaces *tspcs;

	if (SRF_IS_FIRSTCALL())
	{
		MemoryContext oldcontext;

		if (!OidIsValid(hypertable_oid))
			elog(ERROR, "invalid argument");

		funcctx = SRF_FIRSTCALL_INIT();
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		funcctx->user_fctx = hypertable_cache_pin();
		MemoryContextSwitchTo(oldcontext);
	}

	funcctx = SRF_PERCALL_SETUP();
	hcache = funcctx->user_fctx;
	ht = hypertable_cache_get_entry(hcache, hypertable_oid);

	if (NULL == ht)
		ereport(ERROR,
				(errcode(ERRCODE_IO_HYPERTABLE_NOT_EXIST),
				 errmsg("table \"%s\" is not a hypertable",
						get_rel_name(hypertable_oid))));

	tspcs = tablespace_scan(ht->fd.id);

	if (NULL != tspcs && funcctx->call_cntr < (uint64) tspcs->num_tablespaces)
	{
		Oid			tablespace_oid = tspcs->tablespaces[funcctx->call_cntr].tablespace_oid;
		const char *tablespace_name = get_tablespace_name(tablespace_oid);
		Datum		name;

		Assert(tablespace_name != NULL);
		name = DirectFunctionCall1(namein, CStringGetDatum(tablespace_name));

		SRF_RETURN_NEXT(funcctx, name);
	}
	else
	{
		cache_release(hcache);
		SRF_RETURN_DONE(funcctx);
	}
}
