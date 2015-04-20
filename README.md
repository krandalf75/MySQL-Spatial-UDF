[![Build Status](https://drone.io/github.com/krandalf75/MySQL-Spatial-UDF/status.png)](https://drone.io/github.com/krandalf75/MySQL-Spatial-UDF/latest)

[Linux binary](https://drone.io/github.com/krandalf75/MySQL-Spatial-UDF/files/libMySpatialUDF.so)

# MySQL Spatial User Defined Functions

This project add missing spatial functionality to MySQL through an UDF library. 
At this time this library implements more than 20 functions not present on current MySQL database.


## What is an User Defined Function?

An User Defined Function (UDF) is a C/C++ library that extends the functionality 
of a MySQL server by adding new functions that behaves just like a native (built-in) 
MySQL functions like abs() or concat().


## Build requirements

The code is written in C and requires the following libraries:

- [PROJ.4](http://trac.osgeo.org/proj)
- [GEOS](http://trac.osgeo.org/geos)


## Building

### on UNIX

On Linux or similars systems you will need to invoke the command "cmake ." This command will resolve 
dependences and generate an Makefile. After this a simple call to "make" will do the job. 

### on WINDOWS

If you have an MSVC 2010 compiler you can use included MySpatialUDF.sln project.  
Before, you must correct the paths to search library dependences. 

If you have another version of Visual Studio you will need to install cmake windows environment.
cmake will build a Visual Studio Project with the version you needed and with all the library dependences solved.

If you want use Cygwin you will also need cmake. In this case cmake will build a Makefile file that you can use with
thie command make. 

## Installation

### on UNIX or Cygwin

If cmake detects the plugin MySQL's directory  the command "make -install" will copy the library to the correct folder.
If not you can copy the library to this folder manually. Ensure that MySQL can execute this file and his library dependences.

Execute script MySpatialUDF.sql as a mysql administrator.

	WARNING !!
	This script drop and creates several tables and procedures in mysql schema.
	Ensure that you don't delete any table or procedure that is useful !!!!

### on WINDOWS

Copy Release/MySpatialUDF.dll and dependent libraries to <MySql Home>\lib\plugin

Execute script MySpatialUDF.sql as a mysql administrator.

 
	WARNING !!
	This script drop and creates several tables and procedures in mysql schema.
	Ensure that you don't delete any table or procedure that is useful !!!!


If you don't like write in mysql schema you can change the first line of the script.


## Current Function List Support

    +-------------------------------+--------+-------+
    | Function                      | native | msudf |
    +-------------------------------+--------+-------+
    | ST_Area                       | Y      | N     |
    | ST_AsBinary                   | Y      | N     |
    | ST_AsText                     | Y      | N     |
    | ST_Boundary                   | N      | Y     |
    | ST_Buffer                     | N      | Y     |
    | ST_Centroid                   | N      | Y     |
    | ST_Contains                   | N      | Y     |
    | ST_ConvexHull                 | N      | Y     |
    | ST_Crosses                    | N      | Y     |
    | ST_Difference                 | N      | Y     |
    | ST_Dimension                  | Y      | N     |
    | ST_Disjoint                   | N      | Y     |
    | ST_EndPoint                   | Y      | N     |
    | ST_Envelope                   | Y      | N     |
    | ST_ExteriorRing               | Y      | N     |
    | ST_GeomFromText               | Y      | N     |
    | ST_GeomFromWKB                | Y      | N     |
    | ST_GeometryN                  | Y      | N     |
    | ST_GeometryType               | Y      | N     |
    | ST_InteriorRingN              | Y      | N     |
    | ST_Intersection               | N      | Y     |
    | ST_Intersects                 | N      | Y     |
    | ST_IsClosed                   | Y      | N     |
    | ST_IsEmpty                    | N      | Y     |
    | ST_IsRing                     | N      | Y     |
    | ST_IsSimple                   | N      | Y     |
    | ST_IsValid                    | N      | Y     |
    | ST_Length                     | Y      | N     |
    | ST_Length2D                   | Y      | N     |
    | ST_LineMerge                  | N      | Y     |
    | ST_Line_Substring             | N      | Y     |
    | ST_NumGeometries              | Y      | N     |
    | ST_NumInteriorRings           | Y      | N     |
    | ST_NumPoints                  | Y      | N     |
    | ST_Overlaps                   | N      | Y     |
    | ST_PointN                     | Y      | N     |
    | ST_PointOnSurface             | N      | Y     |
    | ST_Reverse                    | N      | Y     |
    | ST_SRID                       | Y      | N     |
    | ST_Simplify                   | N      | Y     |
    | ST_SimplifyPreserveTopology   | N      | Y     |
    | ST_SymDifference              | N      | Y     |
    | ST_Touches                    | N      | Y     |
    | ST_Transform                  | N      | Y     |
    | ST_Union                      | N      | Y     |
    | ST_Within                     | N      | Y     |
    | ST_X                          | Y      | N     |
    | ST_Y                          | Y      | N     |
    +-------------------------------+--------+-------+

Note: Native functions don't have 'ST_' prefix.




## Current Function List Support vs Mysql 5.6

    +-------------------------------+--------+-------+
    | Function                      | native | msudf |
    +-------------------------------+--------+-------+
    | ST_Area                       | Y      | N     |
    | ST_AsBinary                   | Y      | N     |
    | ST_AsText                     | Y      | N     |
    | ST_Boundary                   | N      | Y     |
    | ST_Buffer                     | Y      | Y     |
    | ST_Centroid                   | Y      | Y     |
    | ST_Contains                   | Y      | Y     |
    | ST_ConvexHull                 | N      | Y     |
    | ST_Crosses                    | Y      | Y     |
    | ST_Difference                 | Y      | Y     |
    | ST_Dimension                  | Y      | N     |
    | ST_Disjoint                   | Y      | Y     |
    | ST_EndPoint                   | Y      | N     |
    | ST_Envelope                   | Y      | N     |
    | ST_ExteriorRing               | Y      | N     |
    | ST_GeomFromText               | Y      | N     |
    | ST_GeomFromWKB                | Y      | N     |
    | ST_GeometryN                  | Y      | N     |
    | ST_GeometryType               | Y      | N     |
    | ST_InteriorRingN              | Y      | N     |
    | ST_Intersection               | Y      | Y     |
    | ST_Intersects                 | Y      | Y     |
    | ST_IsClosed                   | Y      | N     |
    | ST_IsEmpty                    | Y      | Y     |
    | ST_IsRing                     | N      | Y     |
    | ST_IsSimple                   | Y      | Y     |
    | ST_IsValid                    | N      | Y     |
    | ST_Length                     | Y      | N     |
    | ST_Length2D                   | Y      | N     |
    | ST_LineMerge                  | N      | Y     |
    | ST_Line_Substring             | N      | Y     |
    | ST_NumGeometries              | Y      | N     |
    | ST_NumInteriorRings           | Y      | N     |
    | ST_NumPoints                  | Y      | N     |
    | ST_Overlaps                   | Y      | Y     |
    | ST_PointN                     | Y      | N     |
    | ST_PointOnSurface             | N      | Y     |
    | ST_Reverse                    | N      | Y     |
    | ST_SRID                       | Y      | N     |
    | ST_Simplify                   | N      | Y     |
    | ST_SimplifyPreserveTopology   | N      | Y     |
    | ST_SymDifference              | Y      | Y     |
    | ST_Touches                    | Y      | Y     |
    | ST_Transform                  | N      | Y     |
    | ST_Union                      | Y      | Y     |
    | ST_Within                     | Y      | Y     |
    | ST_X                          | Y      | N     |
    | ST_Y                          | Y      | N     |
    +-------------------------------+--------+-------+

Note: Native functions have 'ST_' prefix.


## Sample Usage

You can load a table with GDAL tool ogr2ogr.

After this check the SRID of the geometry is correct.

	mysql> select srid(geom) into @srid from geom_table limit 1;
	Query OK, 1 row affected (0.00 sec)

	mysql> select @srid;
	+-------+
	| @srid |
	+-------+
	|  3395 |
	+-------+
	1 row in set (0.00 sec)

	mysql> select @srid;
	+-------+
	| @srid |
	+-------+
	|  3395 |
	+-------+
	1 row in set (0.00 sec)

	mysql> select auth_name,auth_srid,ref_sys_name from mysql.spatial_ref_sys where
	srid = @srid;
	+-----------+-----------+-------------------------+
	| auth_name | auth_srid | ref_sys_name            |
	+-----------+-----------+-------------------------+
	| epsg      |      3395 | WGS 84 / World Mercator |
	+-----------+-----------+-------------------------+
	1 row in set (0.00 sec)


If it's wrong you can change the SRID value (without transform) with this update:

	mysql> update geom_table set geom = GeomFromWKB(aswkb(geom),3395); 


You can create another geometry with different projection with this sentence:

	mysql> alter table geom_table add column (geom2 geometry);
	Query OK, 1 row affected (0.13 sec)
	Records: 1  Duplicates: 0  Warnings: 0 

	mysql> update test set geom2 = mysql.transform(geom,4326);
	Query OK, 1 row affected (0.00 sec)
	Rows matched: 1000  Changed: 1000  Warnings: 0



## Copyright

Copyright (c) 2012, Albert Rovira. 

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along 
with this program (COPYING.txt).  If not, see <http://www.gnu.org/licenses/>.
