--
-- Cypher Query Language - User Defined Function
--


-- setup

DROP FUNCTION IF EXISTS udf_var(jsonb);
DROP FUNCTION IF EXISTS udf_param(jsonb);
DROP FUNCTION IF EXISTS udf_if();
DROP FUNCTION IF EXISTS udf_if_exists();
DROP FUNCTION IF EXISTS udf_if_not_exists();
DROP GRAPH IF EXISTS udf CASCADE;
DROP GRAPH IF EXISTS udf2 CASCADE;

CREATE GRAPH udf;
SET graph_path = udf;

CREATE (:v {id: 1, refs: [2, 3, 4]}), (:v {id: 2});


-- test param and scope of the iterator variable used in a list comprehension

CREATE FUNCTION udf_param(id jsonb) RETURNS jsonb AS $$
DECLARE
  r jsonb;
BEGIN
  MATCH (n:v) WHERE n.id = id RETURN [id IN n.refs WHERE id < 3] INTO r;
  RETURN r;
END;
$$ LANGUAGE plpgsql;

RETURN udf_param(1);


-- test var

CREATE FUNCTION udf_var(id jsonb) RETURNS jsonb AS $$
DECLARE
  i jsonb;
  p jsonb;
BEGIN
  i := id;
  MATCH (n:v) WHERE n.id = i RETURN properties(n) INTO p;
  RETURN p;
END;
$$ LANGUAGE plpgsql;

RETURN udf_var(2);

-- test if

CREATE GRAPH udf2;
SET GRAPH_PATH = udf2;

CREATE (:people{name:'Anders'}) , (:people{name:'Bossman'});

MATCH (a) , (b)
WHERE (a.name = 'Anders' AND b.name = 'Bossman')
CREATE (a)-[e:knows{type:'knows'}]->(b);

CREATE OR REPLACE FUNCTION udf_if() RETURNS boolean AS $$
BEGIN
IF ( MATCH (a)-[b]->(c) WHERE a.name = 'Anders' AND c.name = 'Bossman' RETURN b.type ) THEN
RETURN true;
ELSE
RETURN false;
END IF;
END;
$$ LANGUAGE plpgsql;

-- This test originally worked due to a side effect of having a cast from jsonb
-- to boolean. That cast has been removed, so now this will fail as expected.
SELECT udf_if();

CREATE OR REPLACE FUNCTION udf_if_exists() RETURNS boolean AS $$
BEGIN
IF EXISTS ( MATCH (a)-[b]->(c) WHERE a.name = 'Anders' AND c.name = 'Bossman' RETURN b ) THEN
RETURN true;
ELSE
RETURN false;
END IF;
END;
$$ LANGUAGE plpgsql;

SELECT udf_if_exists();

CREATE OR REPLACE FUNCTION udf_if_not_exists() RETURNS boolean AS $$
BEGIN
IF NOT EXISTS ( MATCH (a)-[b]->(c) WHERE a.name = 'Anders' AND c.name = 'Bossman' RETURN b ) THEN
RETURN true;
ELSE
RETURN false;
END IF;
END;
$$ LANGUAGE plpgsql;

SELECT udf_if_not_exists();


-- teardown

DROP FUNCTION udf_var(jsonb);
DROP FUNCTION udf_param(jsonb);
DROP FUNCTION udf_if();
DROP FUNCTION udf_if_exists();
DROP FUNCTION udf_if_not_exists();
DROP GRAPH udf CASCADE;
DROP GRAPH udf2 CASCADE;
