--
-- PL/agCypher - Use Parameter in PL/agCypher
--

-- setup

drop graph if exists g1 cascade;
drop function if exists param_in_vertex( vertex );
drop function if exists param_in_edge( edge );
drop function if exists param_in_graphpath( graphpath );
drop function if exists param_in_graphid( graphid );
drop function if exists param_out_vertex();
drop function if exists param_out_edge();
drop function if exists param_out_graphpath();
drop function if exists param_out_graphid();
drop function if exists param_inout_vertex( vertex );
drop function if exists param_inout_edge( edge );
drop function if exists param_inout_graphpath( graphpath );
drop function if exists param_inout_graphid( graphid );

create graph g1;
set graph_path=g1;

create (:person{name : 'Anders'}), (:person{name : 'Dilshad'}), (:person{name : 'Cesar'}), (:person{name : 'Becky'}), (:person{name : 'Filipa'}), (:person{name : 'Emil'});

match (a), (b)
where a.name = 'Anders' and b.name = 'Dilshad'
create (a)-[e:knows{name:'friend1'}]->(b)
return e;

match (a), (b)
where a.name = 'Anders' and b.name = 'Cesar'
create (a)-[e:knows{name:'friend2'}]->(b)
return e;

match (a), (b)
where a.name = 'Anders' and b.name = 'Becky'
create (a)-[e:knows{name:'friend3'}]->(b)
return e;

match (a), (b)
where a.name = 'Dilshad' and b.name = 'Filipa'
create (a)-[e:knows{name:'friend4'}]->(b)
return e;

match (a), (b)
where a.name = 'Cesar' and b.name = 'Emil'
create (a)-[e:knows{name:'friend5'}]->(b)
return e;

match (a), (b)
where a.name = 'Becky' and b.name = 'Emil'
create (a)-[e:knows{name:'friend6'}]->(b)
return e;

-- test in type parameter( vertex )

create or replace function param_in_vertex( in vertex ) returns vertex as $$
declare
var1 vertex;
begin
raise notice '[parameter of param_in_vertex( in vertex )] $1 : %' , $1;
match (x)-[z]->(y) where y.name =$1.name return x into var1;
return var1;
end;
$$ language plagcypher;

do $$
declare
var1 vertex;
var2 vertex;
begin
var2 := param_in_vertex( var1 );
raise notice '[result for param_in_vertex( in vertex )] var2 : %' , var2;
end;
$$ language plagcypher;

-- test in type parameter( edge )

create or replace function param_in_edge( in edge ) returns edge as $$
declare
var1 edge;
begin
raise notice '[parameter of param_in_edge( in edge )] $1 : %' , $1;
match (a)-[b]-(c)-[d]-(e) where c.name = 'Dilshad' and d.name = $1.name and a.name = 'Anders' return b into var1;
return var1;
end;
$$ language plagcypher;

do $$
declare
var1 edge;
var2 edge;
begin
match (x)-[z]->(y) where x.name = 'Dilshad' and y.name = 'Filipa' return z into var1;
var2 := param_in_edge( var1 );
raise notice '[result for param_in_edge( in edge )] var2 : %' , var2;
end;
$$ language plagcypher;

-- test in type parameter( graphpath )

create or replace function param_in_graphpath( in graphpath ) returns vertex as $$
declare
var1 vertex;
begin
raise notice '[parameter of param_in_graphpath( in graphpath )] $1 : %' , $1;
match p=allshortestpaths( (n)-[*..4]->(m) ) where p=$1 return n into var1;
return var1;
end;
$$ language plagcypher;

do $$
declare
var1 graphpath;
var2 vertex;
begin
match p=allshortestpaths( (n)-[*..4]->(m) ) where m.name = 'Cesar' return p into var1;
var2 := param_in_graphpath( var1 );
raise notice '[result for param_in_graphpath( in graphpath )] var2 : %' , var2;
end;
$$ language plagcypher;

-- test in type parameter( graphth )

create or replace function param_in_graphid( in graphid ) returns vertex as $$
declare
var1 vertex;
begin
raise notice '[parameter of param_in_graphid( in graphid )] $1 : %' , $1;
match (a) where id(a) = $1 return a into var1;
return var1;
end;
$$ language plagcypher;

do $$
declare
var1 graphid;
var2 vertex;
begin
match (x)-[z]-(y) where x.name = 'Becky' and z.name = 'friend6' return id(y) into var1;
var2 := param_in_graphid( var1 );

if var1 = (var2).id then
raise notice '[true] vertex "%" of grpah id is %.' , var2 , var1;
else
raise notice '[false]';
end if;
end;
$$ language plagcypher;

-- test out type parameter( vertex )

create or replace function param_out_vertex( out vertex ) as $$
begin
match (x)-[z]->(y) where x.name = 'Dilshad' and z.name = 'friend4' return y into $1;
end;
$$ language plagcypher;

select param_out_vertex();

-- test out type parameter( edge )

create or replace function param_out_edge( out edge ) as $$
begin
match (x)-[z]->(y) where x.name = 'Anders' and y.name = 'Dilshad' return z into $1;
end;
$$ language plagcypher;

select param_out_edge();

-- test out type parameter( graphpath )

create or replace function param_out_graphpath( out graphpath ) as $$
begin
match p=allshortestpaths( (n)-[*..4]->(m) ) where m.name = 'Cesar' return p into $1;
end;
$$ language plagcypher;

select param_out_graphpath();

-- test out type parameter( graphid )

create or replace function param_out_graphid( out graphid ) as $$
begin
match (a) where a.name = 'Anders' return id(a) into $1;
end;
$$ language plagcypher;

select param_out_graphid();

-- test inout type parameter( vertex )

create or replace function param_inout_vertex( inout vertex ) as $$
declare
var1 vertex;
begin
match (a)-[b]-(c)-[d]-(e) where c.name = 'Dilshad' and e.name = 'Filipa' return a into $1;
raise notice '[out result] $1 : %' , $1;
match (a)-[b]->(c) where b.name = 'friend2' and a.name = $1.name return c into var1;
raise notice '[result] var1 : %' , var1;
end;
$$ language plagcypher;

do $$
declare
var1 vertex;
var2 vertex;
begin
var2 := param_inout_vertex( var1 );
raise notice '[result of param_inout_vertex( inout vertex )] var2 : %' , var2;
end;
$$ language plagcypher;

-- test inout type parameter( edge )

create or replace function param_inout_edge( inout edge ) as $$
declare
var1 edge;
begin
match (a)-[b]-(c)-[d]-(e) where c.name = 'Dilshad' and e.name = 'Filipa' return d into $1;
raise notice '[out result] $1 : %' , $1;
match (a)-[b]-(c)-[d]->(e) where a.name = 'Anders' and c.name = 'Dilshad' and d.name = $1.name return b into var1;
raise notice '[result] var1 : %' , var1;
end;
$$ language plagcypher;

do $$
declare
var1 edge;
var2 edge;
begin
var2 := param_inout_edge( var1 );
raise notice '[result of param_inout_edge( inout edge )] var2 : %' , var2;
end;
$$ language plagcypher;

-- test inout type parameter( graphpath )

create or replace function param_inout_graphpath( inout graphpath ) as $$
declare
var1 vertex;
begin
match p=allshortestpaths( (n)-[*..4]->(m) ) where n.name = 'Anders' and m.name = 'Filipa' return p into $1;
raise notice '[out result] $1 : %' , $1;
match p=allshortestpaths( (n)-[*..4]->(m) ) where p =$1 return m into var1;
raise notice '[result] var1 : %' , var1;
end;
$$ language plagcypher;

do $$
declare
var1 graphpath;
var2 graphpath;
begin
var2 := param_inout_graphpath( var1 );
raise notice '[result of param_inout_graphpath( inout graphpath )] var2 : %' , var2;
end;
$$ language plagcypher;

-- test inout type parameter( graphid )

create or replace function param_inout_graphid( inout graphid ) as $$
declare
var1 vertex;
begin
match (a) where a.name = 'Becky' return id(a) into $1;
raise notice '[out result] $1 : %' , $1;
match (x)-[z]-(y) where id(x) = $1 and z.name = 'friend6' return y into var1;
raise notice '[result] var1 : %' , var1;
end;
$$ language plagcypher;

do $$
declare
var1 graphid;
var2 graphid;
begin
var2 := param_inout_graphid( var1 );
raise notice '[result of param_inout_graphid( inout graphid )] var2 : %' , var2;
end;
$$ language plagcypher;

-- cleanup

drop function if exists param_inout_graphid( graphid );
drop function if exists param_inout_graphpath( graphpath );
drop function if exists param_inout_edge( edge );
drop function if exists param_inout_vertex( vertex );
drop function if exists param_out_graphid();
drop function if exists param_out_graphpath();
drop function if exists param_out_edge();
drop function if exists param_out_vertex();
drop function if exists param_in_graphid( graphid );
drop function if exists param_in_graphpath( graphpath );
drop function if exists param_in_edge( edge );
drop function if exists param_in_vertex( vertex );
drop graph if exists g1 cascade;
