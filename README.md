# MySQL Spatial User Defined Functions

This library tries to add to Mysql spatial functionality that MySQL does not provide using an UDF library.

At this time it can only transform an geometry from one projection to another.



## What is an User Defined Function?

Basically an User Defined Function (UDF) is a C/C++ library that extends the functionality 
of a MySQL server by adding new functions that behaves just like a native (built-in) MySQL functions like abs() or concat().


## Build requirements

The code is written in C and requires the following libraries:

- [PROJ.4](http://trac.osgeo.org/proj)
- [GEOS](http://trac.osgeo.org/geos)


## Build

For now you can only compile it with an MSVC 2010 compiler but I think it can be 
ported easily to Linux or other compilers.


Open MySpatialUDF.sln and correct directory paths for includes and libs.

Make a release build and check there is no errors. 


## Installation

Copy Release/MySpatialUDF.dll and dependant libraries to <MySql Home>\lib\plugin

Execute script MySpatialUDF.sql as a mysql administrator.
 
	WARNING !!
	This script drop and recreates several tables and procedures in mysql schema.
	Ensure that you don't delete any table or procedure that is useful !!!!


If you don't like write in mysql schema you can change the first line of the script.


## Usage

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


If it's wrong you cab change the SRID value (without transform) with this update:

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