\ir include/insert_two_partitions.sql

SELECT * FROM test.show_columnsp('_timescaledb_internal.%');
SELECT * FROM test.show_indexesp('_timescaledb_internal.%');
SELECT * FROM _timescaledb_catalog.chunk;

SELECT * FROM "two_Partitions" ORDER BY "timeCustom", device_id;
SELECT * FROM ONLY "two_Partitions";

CREATE TABLE error_test(time timestamp, temp float8, device text NOT NULL);
SELECT create_hypertable('error_test', 'time', 'device', 2);

\set QUIET off
INSERT INTO error_test VALUES ('Mon Mar 20 09:18:20.1 2017', 21.3, 'dev1');
\set ON_ERROR_STOP 0
-- generate insert error
INSERT INTO error_test VALUES ('Mon Mar 20 09:18:22.3 2017', 21.1, NULL);
\set ON_ERROR_STOP 1
INSERT INTO error_test VALUES ('Mon Mar 20 09:18:25.7 2017', 22.4, 'dev2');
\set QUIET on
SELECT * FROM error_test;

--test character(9) partition keys since there were issues with padding causing partitioning errors
CREATE TABLE tick_character (
    symbol      character(9) NOT NULL,
    mid       REAL NOT NULL,
    spread      REAL NOT NULL,
    time        TIMESTAMPTZ       NOT NULL
);

SELECT create_hypertable ('tick_character', 'time', 'symbol', 2);
INSERT INTO tick_character ( symbol, mid, spread, time ) VALUES ( 'GBPJPY', 142.639000, 5.80, 'Mon Mar 20 09:18:22.3 2017') RETURNING time, symbol, mid;
SELECT * FROM tick_character;


CREATE TABLE  many_partitions_test(time timestamp, temp float8, device text NOT NULL);
SELECT create_hypertable('many_partitions_test', 'time', 'device', 1000);
--NOTE: how much slower the first two queries are -- they are creating chunks
INSERT INTO many_partitions_test
    SELECT to_timestamp(ser), ser, ser::text FROM generate_series(1,100) ser;
INSERT INTO many_partitions_test
    SELECT to_timestamp(ser), ser, ser::text FROM generate_series(101,200) ser;
INSERT INTO many_partitions_test
    SELECT to_timestamp(ser), ser, (ser-201)::text FROM generate_series(201,300) ser;

SELECT * FROM  many_partitions_test ORDER BY time DESC LIMIT 2;
SELECT count(*) FROM  many_partitions_test;

CREATE TABLE  date_col_test(time date, temp float8, device text NOT NULL);
SELECT create_hypertable('date_col_test', 'time', 'device', 1000, chunk_time_interval => INTERVAL '1 Day');
INSERT INTO date_col_test
VALUES ('2001-02-01', 98, 'dev1'),
('2001-03-02', 98, 'dev1');

SELECT * FROM date_col_test WHERE time > '2001-01-01';

CREATE TABLE many_partitions_test_1m (time timestamp, temp float8, device text NOT NULL);
SELECT create_hypertable('many_partitions_test_1m', 'time', 'device', 1000);

EXPLAIN (verbose on, costs off)
INSERT INTO many_partitions_test_1m(time, temp, device)
SELECT time_bucket('1 minute', time) AS period, avg(temp), device
FROM many_partitions_test
GROUP BY period, device;

INSERT INTO many_partitions_test_1m(time, temp, device)
SELECT time_bucket('1 minute', time) AS period, avg(temp), device
FROM many_partitions_test
GROUP BY period, device;

SELECT * FROM many_partitions_test_1m ORDER BY time, device LIMIT 10;

CREATE TABLE one_space_test(time timestamp, temp float8, device text NOT NULL);
SELECT create_hypertable('one_space_test', 'time', 'device', 1);
INSERT INTO one_space_test VALUES
('2001-01-01 01:01:01', 1.0, 'device'),
('2002-01-01 01:02:01', 1.0, 'device');
SELECT * FROM one_space_test;

--CTE & EXPLAIN ANALYZE TESTS
WITH insert_cte as (
	INSERT INTO one_space_test VALUES
		('2001-01-01 01:02:01', 1.0, 'device')
	RETURNING *)
SELECT * FROM insert_cte;

EXPLAIN (analyze, costs off, timing off) --can't turn summary off in 9.6 so instead grep it away at end.
WITH insert_cte as (
	INSERT INTO one_space_test VALUES
		('2001-01-01 01:03:01', 1.0, 'device')
	)
SELECT 1 \g | grep -v "Planning" | grep -v "Execution"
