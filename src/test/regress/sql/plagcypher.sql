--
-- PL/agCypher - Use Parameter in PL/agCypher
--

-- setup

DROP GRAPH IF EXISTS g1 CASCADE;
DROP FUNCTION IF EXISTS param_in_vertex( vertex );
DROP FUNCTION IF EXISTS param_in_edge( edge );
DROP FUNCTION IF EXISTS param_in_graphpath( graphpath );
DROP FUNCTION IF EXISTS param_in_graphid( graphid );
DROP FUNCTION IF EXISTS param_out_vertex();
DROP FUNCTION IF EXISTS param_out_edge();
DROP FUNCTION IF EXISTS param_out_graphpath();
DROP FUNCTION IF EXISTS param_out_graphid();
DROP FUNCTION IF EXISTS param_inout_vertex( vertex );
DROP FUNCTION IF EXISTS param_inout_edge( edge );
DROP FUNCTION IF EXISTS param_inout_graphpath( graphpath );
DROP FUNCTION IF EXISTS param_inout_graphid( graphid );

CREATE GRAPH g1;
SET GRAPH_PATH=g1;

CREATE (a:person{name : 'Anders'})-[:knows {name:'friend1'}]->(b:person{name : 'Dilshad'}),
(a)-[:knows {name:'friend2'}]->(c:person{name : 'Cesar'}),
(a)-[:knows {name:'friend3'}]->(d:person{name : 'Becky'}),
(b)-[:knows {name:'friend4'}]->(:person{name : 'Filipa'}),
(c)-[:knows {name:'friend5'}]->(e:person{name : 'Emil'}),
(d)-[:knows {name:'friend6'}]->(e);

-- test in type parameter( vertex )

CREATE OR REPLACE FUNCTION param_in_vertex( IN vertex ) RETURNS vertex AS $$
DECLARE
var1 vertex;
BEGIN
MATCH (x)-[z]->(y) WHERE y.name =$1.name RETURN x INTO var1;
RETURN var1;
END;
$$ LANGUAGE plagcypher;

DO $$
DECLARE
var1 vertex;
var2 vertex;
BEGIN
MATCH (x)-[z]->(y) WHERE x.name = 'Dilshad' RETURN y INTO var1;
var2 := param_in_vertex( var1 );
END;
$$ LANGUAGE plagcypher;

-- test in type parameter( edge )

CREATE OR REPLACE FUNCTION param_in_edge( IN edge ) RETURNS edge AS $$
DECLARE
var1 edge;
BEGIN
MATCH (a)-[b]-(c)-[d]-(e) WHERE c.name = 'Dilshad' AND d.name = $1.name AND a.name = 'Anders' RETURN b INTO var1;
RETURN var1;
END;
$$ LANGUAGE plagcypher;

DO $$
DECLARE
var1 edge;
var2 edge;
BEGIN
MATCH (x)-[z]->(y) WHERE x.name = 'Dilshad' AND y.name = 'Filipa' RETURN z INTO var1;
var2 := param_in_edge( var1 );
END;
$$ LANGUAGE plagcypher;

-- test in type parameter( graphpath )

CREATE OR REPLACE FUNCTION param_in_graphpath( IN graphpath ) RETURNS vertex AS $$
DECLARE
var1 vertex;
BEGIN
MATCH p=allshortestpaths( (n)-[*..4]->(m) ) WHERE p=$1 RETURN n INTO var1;
RETURN var1;
END;
$$ LANGUAGE plagcypher;

DO $$
DECLARE
var1 graphpath;
var2 vertex;
BEGIN
MATCH p=allshortestpaths( (n)-[*..4]->(m) ) WHERE m.name = 'Cesar' RETURN p INTO var1;
var2 := param_in_graphpath( var1 );
END;
$$ LANGUAGE plagcypher;

-- test in type parameter( graphth )

CREATE OR REPLACE FUNCTION param_in_graphid( IN graphid ) RETURNS vertex AS $$
DECLARE
var1 vertex;
BEGIN
MATCH (a) WHERE id(a) = $1 RETURN a INTO var1;
RETURN var1;
END;
$$ LANGUAGE plagcypher;

DO $$
DECLARE
var1 graphid;
var2 vertex;
BEGIN
MATCH (x)-[z]-(y) WHERE x.name = 'Becky' AND z.name = 'friend6' RETURN id(y) INTO var1;
var2 := param_in_graphid( var1 );

IF var1 = (var2).id THEN
RAISE NOTICE '[true]';
ELSE
RAISE NOTICE '[false]';
END IF;
END;
$$ LANGUAGE plagcypher;

-- test out type parameter( vertex )

CREATE OR REPLACE FUNCTION param_out_vertex( OUT vertex ) AS $$
BEGIN
match (x)-[z]->(y) where x.name = 'Dilshad' and z.name = 'friend4' return y into $1;
END;
$$ LANGUAGE plagcypher;

SELECT properties( param_out_vertex() );

-- test out type parameter( edge )

CREATE OR REPLACE FUNCTION param_out_edge( OUT edge ) AS $$
BEGIN
MATCH (x)-[z]->(y) WHERE x.name = 'Anders' AND y.name = 'Dilshad' RETURN z INTO $1;
END;
$$ LANGUAGE plagcypher;

SELECT properties( param_out_edge() );

-- test out type parameter( graphpath )

CREATE OR REPLACE FUNCTION param_out_graphpath( OUT graphpath ) AS $$
BEGIN
MATCH p=allshortestpaths( (n)-[*..4]->(m) ) WHERE m.name = 'Cesar' RETURN p INTO $1;
END;
$$ LANGUAGE plagcypher;

DO $$
DECLARE
var1 vertex[];
var2 jsonb[];
var3 vertex;
BEGIN
var1 = nodes( param_out_graphpath() );
FOREACH var3 IN ARRAY var1 LOOP
var2 = array_append( var2, var3::jsonb );
END LOOP;
RAISE NOTICE '%' , var2;
END;
$$ LANGUAGE plagcypher;

CREATE OR REPLACE FUNCTION param_out_graphid( OUT graphid ) AS $$
BEGIN
MATCH (a) WHERE a.name = 'Anders' RETURN id(a) INTO $1;
END;
$$ LANGUAGE plagcypher;

SELECT properties from g1.ag_vertex where id = param_out_graphid();

-- test inout type parameter( vertex )

CREATE OR REPLACE FUNCTION param_inout_vertex( INOUT vertex ) AS $$
DECLARE
var1 vertex;
BEGIN
MATCH (a)-[b]-(c)-[d]-(e) WHERE c.name = 'Dilshad' AND e.name = 'Filipa' RETURN a INTO $1;
MATCH (a)-[b]->(c) WHERE b.name = 'friend2' AND a.name = $1.name RETURN c INTO var1;
END;
$$ LANGUAGE plagcypher;

DO $$
DECLARE
var1 vertex;
var2 vertex;
BEGIN
var2 := param_inout_vertex( var1 );
END;
$$ LANGUAGE plagcypher;

-- test inout type parameter( edge )

CREATE OR REPLACE FUNCTION param_inout_edge( INOUT edge ) AS $$
DECLARE
var1 edge;
BEGIN
MATCH (a)-[b]-(c)-[d]-(e) WHERE c.name = 'Dilshad' AND e.name = 'Filipa' RETURN d INTO $1;
MATCH (a)-[b]-(c)-[d]->(e) WHERE a.name = 'Anders' AND c.name = 'Dilshad' AND d.name = $1.name RETURN b INTO var1;
END;
$$ LANGUAGE plagcypher;

DO $$
DECLARE
var1 edge;
var2 edge;
BEGIN
var2 := param_inout_edge( var1 );
END;
$$ LANGUAGE plagcypher;

-- test inout type parameter( graphpath )

CREATE OR REPLACE FUNCTION param_inout_graphpath( INOUT graphpath ) AS $$
DECLARE
var1 vertex;
BEGIN
MATCH p=allshortestpaths( (n)-[*..4]->(m) ) WHERE n.name = 'Anders' AND m.name = 'Filipa' RETURN p INTO $1;
MATCH p=allshortestpaths( (n)-[*..4]->(m) ) WHERE p =$1 RETURN m INTO var1;
END;
$$ LANGUAGE plagcypher;

DO $$
DECLARE
var1 graphpath;
var2 graphpath;
BEGIN
var2 := param_inout_graphpath( var1 );
END;
$$ LANGUAGE plagcypher;

-- test inout type parameter( graphid )

CREATE OR REPLACE FUNCTION param_inout_graphid( INOUT graphid ) AS $$
DECLARE
var1 vertex;
BEGIN
MATCH (a) WHERE a.name = 'Becky' RETURN id(a) INTO $1;
MATCH (x)-[z]-(y) WHERE id(x) = $1 AND z.name = 'friend6' RETURN y INTO var1;
END;
$$ LANGUAGE plagcypher;

DO $$
DECLARE
var1 graphid;
var2 graphid;
BEGIN
var2 := param_inout_graphid( var1 );
END;
$$ LANGUAGE plagcypher;

-- cleanup

DROP FUNCTION IF EXISTS param_inout_graphid( graphid );
DROP FUNCTION IF EXISTS param_inout_graphpath( graphpath );
DROP FUNCTION IF EXISTS param_inout_edge( edge );
DROP FUNCTION IF EXISTS param_inout_vertex( vertex );
DROP FUNCTION IF EXISTS param_out_graphid();
DROP FUNCTION IF EXISTS param_out_graphpath();
DROP FUNCTION IF EXISTS param_out_edge();
DROP FUNCTION IF EXISTS param_out_vertex();
DROP FUNCTION IF EXISTS param_in_graphid( graphid );
DROP FUNCTION IF EXISTS param_in_graphpath( graphpath );
DROP FUNCTION IF EXISTS param_in_edge( edge );
DROP FUNCTION IF EXISTS param_in_vertex( vertex );
DROP GRAPH IF EXISTS g1 CASCADE;
