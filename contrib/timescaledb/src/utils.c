#include <postgres.h>
#include <fmgr.h>
#include <catalog/pg_type.h>
#include <catalog/pg_trigger.h>
#include <catalog/namespace.h>
#include <catalog/pg_inherits.h>
#include <catalog/indexing.h>
#include <access/htup.h>
#include <access/htup_details.h>
#include <access/heapam.h>
#include <access/genam.h>
#include <nodes/nodes.h>
#include <nodes/makefuncs.h>
#include <parser/scansup.h>
#include <utils/guc.h>
#include <utils/date.h>
#include <utils/datetime.h>
#include <utils/lsyscache.h>
#include <utils/syscache.h>
#include <utils/relcache.h>
#include <utils/fmgroids.h>
#include <catalog/pg_cast.h>
#include <parser/parse_coerce.h>

#include "utils.h"
#include "compat.h"

#if PG10
#include <utils/fmgrprotos.h>
#endif

TS_FUNCTION_INFO_V1(pg_timestamp_to_microseconds);

/*
 * Convert a Postgres TIMESTAMP to BIGINT microseconds relative the Postgres epoch.
 */
Datum
pg_timestamp_to_microseconds(PG_FUNCTION_ARGS)
{
	TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(0);
	int64		microseconds;

	if (!IS_VALID_TIMESTAMP(timestamp))
		ereport(ERROR,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("timestamp out of range")));

#ifdef HAVE_INT64_TIMESTAMP
	microseconds = timestamp;
#else
	if (1)
	{
		int64		seconds = (int64) timestamp;

		microseconds = (seconds * USECS_PER_SEC) + ((timestamp - seconds) * USECS_PER_SEC);
	}
#endif
	PG_RETURN_INT64(microseconds);
}

TS_FUNCTION_INFO_V1(pg_microseconds_to_timestamp);

/*
 * Convert BIGINT microseconds relative the UNIX epoch to a Postgres TIMESTAMP.
 */
Datum
pg_microseconds_to_timestamp(PG_FUNCTION_ARGS)
{
	int64		microseconds = PG_GETARG_INT64(0);
	TimestampTz timestamp;

#ifdef HAVE_INT64_TIMESTAMP
	timestamp = microseconds;
#else
	timestamp = microseconds / USECS_PER_SEC;
#endif

	if (!IS_VALID_TIMESTAMP(timestamp))
		ereport(ERROR,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("timestamp out of range")));

	PG_RETURN_TIMESTAMPTZ(timestamp);
}

TS_FUNCTION_INFO_V1(pg_timestamp_to_unix_microseconds);

/*
 * Convert a Postgres TIMESTAMP to BIGINT microseconds relative the UNIX epoch.
 */
Datum
pg_timestamp_to_unix_microseconds(PG_FUNCTION_ARGS)
{
	TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(0);
	int64		epoch_diff_microseconds = (POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * USECS_PER_DAY;
	int64		microseconds;

	if (timestamp < MIN_TIMESTAMP)
		ereport(ERROR,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("timestamp out of range")));

	if (timestamp >= (END_TIMESTAMP - epoch_diff_microseconds))
		ereport(ERROR,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("timestamp out of range")));

#ifdef HAVE_INT64_TIMESTAMP
	microseconds = timestamp + epoch_diff_microseconds;
#else
	if (1)
	{
		int64		seconds = (int64) timestamp;

		microseconds = (seconds * USECS_PER_SEC) + ((timestamp - seconds) * USECS_PER_SEC) + epoch_diff_microseconds;
	}
#endif
	PG_RETURN_INT64(microseconds);
}

TS_FUNCTION_INFO_V1(pg_unix_microseconds_to_timestamp);

/*
 * Convert BIGINT microseconds relative the UNIX epoch to a Postgres TIMESTAMP.
 */
Datum
pg_unix_microseconds_to_timestamp(PG_FUNCTION_ARGS)
{
	int64		microseconds = PG_GETARG_INT64(0);
	TimestampTz timestamp;

	/*
	 * Test that the UNIX us timestamp is within bounds. Note that an int64 at
	 * UNIX epoch and microsecond precision cannot represent the upper limit
	 * of the supported date range (Julian end date), so INT64_MAX is the
	 * natural upper bound for this function.
	 */
	if (microseconds < ((int64) USECS_PER_DAY * (DATETIME_MIN_JULIAN - UNIX_EPOCH_JDATE)))
		ereport(ERROR,
				(errcode(ERRCODE_DATETIME_VALUE_OUT_OF_RANGE),
				 errmsg("timestamp out of range")));

#ifdef HAVE_INT64_TIMESTAMP
	timestamp = microseconds - ((POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * USECS_PER_DAY);
#else
	/* Shift the epoch using integer arithmetic to reduce precision errors */
	timestamp = microseconds / USECS_PER_SEC;	/* seconds */
	microseconds = microseconds - ((int64) timestamp * USECS_PER_SEC);
	timestamp = (float8) ((int64) seconds - ((POSTGRES_EPOCH_JDATE - UNIX_EPOCH_JDATE) * SECS_PER_DAY))
		+ (float8) microseconds / USECS_PER_SEC;
#endif
	PG_RETURN_TIMESTAMPTZ(timestamp);
}

TS_FUNCTION_INFO_V1(time_to_internal);

Datum
time_to_internal(PG_FUNCTION_ARGS)
{
	if (PG_ARGISNULL(0))
		PG_RETURN_NULL();

	PG_RETURN_INT64(time_value_to_internal(PG_GETARG_DATUM(0), get_fn_expr_argtype(fcinfo->flinfo, 0), false));
}

/*	*/
int64
time_value_to_internal(Datum time_val, Oid type_oid, bool failure_ok)
{
	Datum		res,
				tz;

	switch (type_oid)
	{
		case INT8OID:
			return DatumGetInt64(time_val);
		case INT4OID:
			return (int64) DatumGetInt32(time_val);
		case INT2OID:
			return (int64) DatumGetInt16(time_val);
		case TIMESTAMPOID:

			/*
			 * for timestamps, ignore timezones, make believe the timestamp is
			 * at UTC
			 */
			res = DirectFunctionCall1(pg_timestamp_to_unix_microseconds, time_val);

			return DatumGetInt64(res);
		case TIMESTAMPTZOID:
			res = DirectFunctionCall1(pg_timestamp_to_unix_microseconds, time_val);

			return DatumGetInt64(res);
		case DATEOID:
			tz = DirectFunctionCall1(date_timestamp, time_val);
			res = DirectFunctionCall1(pg_timestamp_to_unix_microseconds, tz);

			return DatumGetInt64(res);
		default:
			if (type_is_int8_binary_compatible(type_oid))
				return DatumGetInt64(time_val);
			if (!failure_ok)
				elog(ERROR, "unkown time type OID %d", type_oid);
			return -1;
	}
}

/* Make a RangeVar from a regclass Oid */
RangeVar *
makeRangeVarFromRelid(Oid relid)
{
	Oid			namespace = get_rel_namespace(relid);
	char	   *tableName = get_rel_name(relid);
	char	   *schemaName = get_namespace_name(namespace);

	return makeRangeVar(schemaName, tableName, -1);
}

int
int_cmp(const void *a, const void *b)
{
	const int  *ia = (const int *) a;
	const int  *ib = (const int *) b;

	return *ia - *ib;
}

FmgrInfo *
create_fmgr(char *schema, char *function_name, int num_args)
{
	FmgrInfo   *finfo = palloc(sizeof(FmgrInfo));
	FuncCandidateList func_list = FuncnameGetCandidates(list_make2(makeString(schema),
																   makeString(function_name)),
														num_args, NULL, false, false, false);

	if (func_list == NULL)
	{
		elog(ERROR, "could not find the function \"%s.%s\"", schema, function_name);
	}
	if (func_list->next != NULL)
	{
		elog(ERROR, "multiple functions found");
	}

	fmgr_info(func_list->oid, finfo);

	return finfo;
}

/* Returns the period in the same representation as Postgres Timestamps.
 * (i.e. in microseconds if  HAVE_INT64_TIMESTAMP, seconds otherwise).
 * Note that this is not our internal representation (microseconds).
 * Always returns an exact value.*/
static inline int64
get_interval_period_timestamp_units(Interval *interval)
{
	if (interval->month != 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("interval defined in terms of month, year, century etc. not supported")
				 ));
	}
#ifdef HAVE_INT64_TIMESTAMP
	return interval->time + (interval->day * USECS_PER_DAY);
#else
	if (interval->time != trunc(interval->time))
	{
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("interval must not have sub-second precision")
				 ));
	}
	return interval->time + (interval->day * SECS_PER_DAY);
#endif
}

TS_FUNCTION_INFO_V1(timestamp_bucket);
Datum
timestamp_bucket(PG_FUNCTION_ARGS)
{
	Interval   *interval = PG_GETARG_INTERVAL_P(0);
	Timestamp	timestamp = PG_GETARG_TIMESTAMP(1);
	Timestamp	result;
	int64		period = -1;

	if (TIMESTAMP_NOT_FINITE(timestamp))
		PG_RETURN_TIMESTAMP(timestamp);

	period = get_interval_period_timestamp_units(interval);
	/* result = (timestamp / period) * period */
	TMODULO(timestamp, result, period);
	if (timestamp < 0)
	{
		/*
		 * need to subtract another period if remainder < 0 this only happens
		 * if timestamp is negative to begin with and there is a remainder
		 * after division. Need to subtract another period since division
		 * truncates toward 0 in C99.
		 */
		result = (result * period) - period;
	}
	else
	{
		result *= period;
	}
	PG_RETURN_TIMESTAMP(result);
}

TS_FUNCTION_INFO_V1(timestamptz_bucket);
Datum
timestamptz_bucket(PG_FUNCTION_ARGS)
{
	Interval   *interval = PG_GETARG_INTERVAL_P(0);
	TimestampTz timestamp = PG_GETARG_TIMESTAMPTZ(1);
	TimestampTz result;
	int64		period = -1;

	if (TIMESTAMP_NOT_FINITE(timestamp))
		PG_RETURN_TIMESTAMPTZ(timestamp);

	period = get_interval_period_timestamp_units(interval);
	/* result = (timestamp / period) * period */
	TMODULO(timestamp, result, period);
	if (timestamp < 0)
	{
		/*
		 * need to subtract another period if remainder < 0 this only happens
		 * if timestamp is negative to begin with and there is a remainder
		 * after division. Need to subtract another period since division
		 * truncates toward 0 in C99.
		 */
		result = (result * period) - period;
	}
	else
	{
		result *= period;
	}
	PG_RETURN_TIMESTAMPTZ(result);
}

static inline void
check_period_is_daily(int64 period)
{
#ifdef HAVE_INT64_TIMESTAMP
	int64		day = USECS_PER_DAY;
#else
	int64		day = SECS_PER_DAY;
#endif
	if (period < day)
	{
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("interval must not have sub-day precision")
				 ));
	}
	if (period % day != 0)
	{
		ereport(ERROR,
				(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
				 errmsg("interval must be a multiple of a day")
				 ));

	}
}

TS_FUNCTION_INFO_V1(date_bucket);

Datum
date_bucket(PG_FUNCTION_ARGS)
{
	Interval   *interval = PG_GETARG_INTERVAL_P(0);
	DateADT		date = PG_GETARG_DATEADT(1);
	Datum		converted_ts,
				bucketed;
	int64		period = -1;

	if (DATE_NOT_FINITE(date))
		PG_RETURN_DATEADT(date);

	period = get_interval_period_timestamp_units(interval);
	/* check the period aligns on a date */
	check_period_is_daily(period);

	/* convert to timestamp (NOT tz), bucket, convert back to date */
	converted_ts = DirectFunctionCall1(date_timestamp, PG_GETARG_DATUM(1));
	bucketed = DirectFunctionCall2(timestamp_bucket, PG_GETARG_DATUM(0), converted_ts);
	return DirectFunctionCall1(timestamp_date, bucketed);
}

/* Returns approximate period in microseconds */
int64
get_interval_period_approx(Interval *interval)
{
	return interval->time + (((interval->month * DAYS_PER_MONTH) + interval->day) * USECS_PER_DAY);
}

#define DAYS_PER_WEEK 7
#define DAYS_PER_QUARTER 89
#define YEARS_PER_DECADE 10
#define YEARS_PER_CENTURY 100
#define YEARS_PER_MILLENNIUM 1000

/* Returns approximate period in microseconds */
int64
date_trunc_interval_period_approx(text *units)
{
	int			decode_type,
				val;
	char	   *lowunits = downcase_truncate_identifier(VARDATA_ANY(units),
														VARSIZE_ANY_EXHDR(units),
														false);

	decode_type = DecodeUnits(0, lowunits, &val);

	if (decode_type != UNITS)
		return -1;

	switch (val)
	{
		case DTK_WEEK:
			return DAYS_PER_WEEK * USECS_PER_DAY;
		case DTK_MILLENNIUM:
			return YEARS_PER_MILLENNIUM * DAYS_PER_YEAR * USECS_PER_DAY;
		case DTK_CENTURY:
			return YEARS_PER_CENTURY * DAYS_PER_YEAR * USECS_PER_DAY;
		case DTK_DECADE:
			return YEARS_PER_DECADE * DAYS_PER_YEAR * USECS_PER_DAY;
		case DTK_YEAR:
			return 1 * DAYS_PER_YEAR * USECS_PER_DAY;
		case DTK_QUARTER:
			return DAYS_PER_QUARTER * USECS_PER_DAY;
		case DTK_MONTH:
			return DAYS_PER_MONTH * USECS_PER_DAY;
		case DTK_DAY:
			return USECS_PER_DAY;
		case DTK_HOUR:
			return USECS_PER_HOUR;
		case DTK_MINUTE:
			return USECS_PER_MINUTE;
		case DTK_SECOND:
			return USECS_PER_SEC;
		case DTK_MILLISEC:
			return USECS_PER_SEC / 1000;
		case DTK_MICROSEC:
			return 1;
		default:
			ereport(ERROR,
					(errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
					 errmsg("timestamp units \"%s\" not supported",
							lowunits)));
	}
	return -1;
}

Oid
inheritance_parent_relid(Oid relid)
{
	Relation	catalog;
	SysScanDesc scan;
	ScanKeyData skey;
	Oid			parent = InvalidOid;
	HeapTuple	tuple;

	catalog = heap_open(InheritsRelationId, AccessShareLock);
	ScanKeyInit(&skey, Anum_pg_inherits_inhrelid, BTEqualStrategyNumber,
				F_OIDEQ, ObjectIdGetDatum(relid));
	scan = systable_beginscan(catalog, InheritsRelidSeqnoIndexId, true,
							  NULL, 1, &skey);
	tuple = systable_getnext(scan);

	if (HeapTupleIsValid(tuple))
		parent = ((Form_pg_inherits) GETSTRUCT(tuple))->inhparent;

	systable_endscan(scan);
	heap_close(catalog, AccessShareLock);

	return parent;
}


bool
type_is_int8_binary_compatible(Oid sourcetype)
{
	HeapTuple	tuple;
	Form_pg_cast castForm;
	bool		result;

	tuple = SearchSysCache2(CASTSOURCETARGET,
							ObjectIdGetDatum(sourcetype),
							ObjectIdGetDatum(INT8OID));
	if (!HeapTupleIsValid(tuple))
		return false;			/* no cast */
	castForm = (Form_pg_cast) GETSTRUCT(tuple);
	result = castForm->castmethod == COERCION_METHOD_BINARY;
	ReleaseSysCache(tuple);
	return result;
}
