/* pg_hint_plan/pg_hint_plan--1.1.3.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_hint_plan" to load this file. \quit

CREATE TABLE hint_plan.hints (
	id					serial	NOT NULL,
	norm_query_string	text	NOT NULL,
	application_name	text	NOT NULL,
	hints				text	NOT NULL,
	PRIMARY KEY (id)
);
CREATE UNIQUE INDEX hints_norm_and_app ON hint_plan.hints (
 	norm_query_string,
	application_name
);

GRANT SELECT ON hint_plan.hints TO PUBLIC;
GRANT USAGE ON SCHEMA hint_plan TO PUBLIC;
