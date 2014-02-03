use mysql;

source "sql/create_spatial_ref_sys.sql"

/* UDF Functions and procedures */
DROP FUNCTION IF EXISTS msudf_boundary;
DROP FUNCTION IF EXISTS msudf_buffer;
DROP FUNCTION IF EXISTS msudf_centroid;
DROP FUNCTION IF EXISTS msudf_convexHull;
DROP FUNCTION IF EXISTS msudf_contains;
DROP FUNCTION IF EXISTS msudf_crosses;
DROP FUNCTION IF EXISTS msudf_difference;
DROP FUNCTION IF EXISTS msudf_disjoint;
DROP FUNCTION IF EXISTS msudf_intersection;
DROP FUNCTION IF EXISTS msudf_intersects;
DROP FUNCTION IF EXISTS msudf_isEmpty;
DROP FUNCTION IF EXISTS msudf_isSimple;
DROP FUNCTION IF EXISTS msudf_isRing;
DROP FUNCTION IF EXISTS msudf_lineMerge;
DROP FUNCTION IF EXISTS msudf_lineSubstring;
DROP FUNCTION IF EXISTS msudf_overlaps;
DROP FUNCTION IF EXISTS msudf_pointOnSurface;
DROP FUNCTION IF EXISTS msudf_reverse;
DROP FUNCTION IF EXISTS msudf_simplify;
DROP FUNCTION IF EXISTS msudf_simplifyPreserveTopology;
DROP FUNCTION IF EXISTS msudf_symDifference;
DROP FUNCTION IF EXISTS msudf_touches;
DROP FUNCTION IF EXISTS msudf_transform;
DROP FUNCTION IF EXISTS msudf_union;
DROP FUNCTION IF EXISTS msudf_within;


CREATE FUNCTION  msudf_boundary					RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_buffer					RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_centroid					RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_convexHull				RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_contains					RETURNS INTEGER SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_crosses					RETURNS INTEGER SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_difference				RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_disjoint					RETURNS INTEGER SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_intersection				RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_intersects				RETURNS INTEGER SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_isEmpty					RETURNS INTEGER SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_isSimple					RETURNS INTEGER SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_isRing					RETURNS INTEGER SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_lineMerge				RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_lineSubstring			RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_overlaps					RETURNS INTEGER SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_pointOnSurface			RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_reverse					RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_simplify					RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_simplifyPreserveTopology	RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_symDifference			RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_touches					RETURNS INTEGER SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_transform				RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_union					RETURNS STRING SONAME "MySpatialUDF.dll";
CREATE FUNCTION  msudf_within					RETURNS INTEGER SONAME "MySpatialUDF.dll";


/* SQL Functions and procedures */

DROP FUNCTION IF EXISTS proj4text;
DELIMITER $$
CREATE FUNCTION proj4text(srid INTEGER) 
RETURNS TEXT DETERMINISTIC 
BEGIN
	DECLARE myval TEXT;
	SELECT a.proj4text into myval FROM spatial_ref_sys a where a.srid = srid;
	return myval;
END $$
DELIMITER ;

DROP FUNCTION IF EXISTS ST_Boundary;
CREATE FUNCTION ST_Boundary(geom GEOMETRY) returns geometry DETERMINISTIC return msudf_boundary(geom);

DROP FUNCTION IF EXISTS ST_Buffer;
CREATE FUNCTION ST_Buffer(geom GEOMETRY,width DOUBLE) returns geometry DETERMINISTIC return msudf_buffer(geom,width);

DROP FUNCTION IF EXISTS ST_Centroid;
CREATE FUNCTION ST_Centroid(geom GEOMETRY) returns geometry DETERMINISTIC return msudf_centroid(geom);

DROP FUNCTION IF EXISTS ST_Contains;
CREATE FUNCTION ST_Contains(geom1 GEOMETRY, geom2 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_contains(geom1,geom2);

DROP FUNCTION IF EXISTS ST_ConvexHull;
CREATE FUNCTION ST_ConvexHull(geom GEOMETRY) returns geometry DETERMINISTIC return msudf_convexHull(geom);

DROP FUNCTION IF EXISTS ST_Crosses;
CREATE FUNCTION ST_Crosses(geom1 GEOMETRY, geom2 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_crosses(geom1,geom2);

DROP FUNCTION IF EXISTS ST_Difference;
CREATE FUNCTION ST_Difference(geom1 GEOMETRY,geom2 GEOMETRY) returns geometry DETERMINISTIC return msudf_difference(geom1,geom2);

DROP FUNCTION IF EXISTS ST_Disjoint;
CREATE FUNCTION ST_Disjoint(geom1 GEOMETRY, geom2 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_disjoint(geom1,geom2);

DROP FUNCTION IF EXISTS ST_Intersection;
CREATE FUNCTION ST_Intersection(geom1 GEOMETRY,geom2 GEOMETRY) returns geometry DETERMINISTIC return msudf_intersection(geom1,geom2);

DROP FUNCTION IF EXISTS ST_Intersects;
CREATE FUNCTION ST_Intersects(geom1 GEOMETRY, geom2 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_intersects(geom1,geom2);

DROP FUNCTION IF EXISTS ST_IsEmpty;
CREATE FUNCTION ST_IsEmpty(geom1 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_isEmpty(geom1);

DROP FUNCTION IF EXISTS ST_IsSimple;
CREATE FUNCTION ST_IsSimple(geom1 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_isSimple(geom1);

DROP FUNCTION IF EXISTS ST_IsRing;
CREATE FUNCTION ST_IsRing(geom1 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_isRing(geom1);

DROP FUNCTION IF EXISTS ST_IsValid;
CREATE FUNCTION ST_IsValid(geom1 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_isValid(geom1);

DROP FUNCTION IF EXISTS ST_LineMerge;
CREATE FUNCTION ST_LineMerge(geom GEOMETRY) returns geometry DETERMINISTIC return msudf_lineMerge(geom);

DROP FUNCTION IF EXISTS ST_LineSubstring;
CREATE FUNCTION ST_LineSubstring(geom GEOMETRY, start DOUBLE, finish DOUBLE) returns geometry DETERMINISTIC return msudf_lineSubstring(geom,start,finish);

DROP FUNCTION IF EXISTS ST_Overlaps;
CREATE FUNCTION ST_Overlaps(geom1 GEOMETRY, geom2 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_overlaps(geom1,geom2);

DROP FUNCTION IF EXISTS ST_PointOnSurface;
CREATE FUNCTION ST_PointOnSurface(geom GEOMETRY) returns geometry DETERMINISTIC return msudf_pointOnSurface(geom);

DROP FUNCTION IF EXISTS ST_Reverse;
CREATE FUNCTION ST_Reverse(geom GEOMETRY) returns geometry DETERMINISTIC return msudf_reverse(geom);

DROP FUNCTION IF EXISTS ST_Simplify;
CREATE FUNCTION ST_Simplify(geom GEOMETRY,tolerance DOUBLE) returns geometry DETERMINISTIC return msudf_simplify(geom,tolerance);

DROP FUNCTION IF EXISTS ST_SimplifyPreserveTopology;
CREATE FUNCTION ST_SimplifyPreserveTopology(geom GEOMETRY,tolerance DOUBLE) returns geometry DETERMINISTIC return msudf_simplifyPreserveTopology(geom,tolerance);

DROP FUNCTION IF EXISTS ST_SymDifference;
CREATE FUNCTION ST_SymDifference(geom1 GEOMETRY,geom2 GEOMETRY) returns geometry DETERMINISTIC return msudf_symDifference(geom1,geom2);

DROP FUNCTION IF EXISTS ST_Touches;
CREATE FUNCTION ST_Touches(geom1 GEOMETRY, geom2 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_touches(geom1,geom2);

DROP FUNCTION IF EXISTS ST_Transform;
CREATE FUNCTION ST_Transform(geom GEOMETRY,srid INTEGER) returns geometry DETERMINISTIC return msudf_transform(geom,proj4text(SRID(geom)),srid,proj4text(srid));

DROP FUNCTION IF EXISTS ST_Union;
CREATE FUNCTION ST_Union(geom1 GEOMETRY,geom2 GEOMETRY) returns geometry DETERMINISTIC return msudf_union(geom1,geom2);

DROP FUNCTION IF EXISTS ST_Within;
CREATE FUNCTION ST_Within(geom1 GEOMETRY, geom2 GEOMETRY) returns INTEGER DETERMINISTIC return msudf_within(geom1,geom2);

/* Deprecated functions */

DROP FUNCTION IF EXISTS transform;
CREATE FUNCTION transform(geom GEOMETRY,srid INTEGER) returns geometry DETERMINISTIC return msudf_transform(geom,proj4text(SRID(geom)),srid,proj4text(srid));

DROP FUNCTION IF EXISTS simplify;
CREATE FUNCTION simplify(geom GEOMETRY,tolerance DOUBLE) returns geometry DETERMINISTIC return msudf_simplify(geom,tolerance);

DROP FUNCTION IF EXISTS simplifyPreserveTopology;
CREATE FUNCTION simplifyPreserveTopology(geom GEOMETRY,tolerance DOUBLE) returns geometry DETERMINISTIC return msudf_simplifyPreserveTopology(geom,tolerance);

DROP FUNCTION IF EXISTS lineMerge;
CREATE FUNCTION lineMerge(geom GEOMETRY) returns geometry DETERMINISTIC return msudf_lineMerge(geom);


/* Some basic tests ... */

source "sql/tests.sql"
