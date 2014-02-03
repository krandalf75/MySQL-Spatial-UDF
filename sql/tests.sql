/* Some basic tests ... */

select CASE WHEN astext(transform(geomfromtext('POINT(2.1699187 41.387917)',4326),4326)) != 'POINT(2.1699187 41.387917)' 
	THEN 'Test 1: Failed.' 
	ELSE 'Test 1: OK.' END;

select CASE WHEN  srid(transform(geomfromtext('POINT (2.1699187 41.387917)',4326),4326)) != 4326
	THEN 'Test 2: Failed.' 
	ELSE 'Test 2: OK.' END;

select CASE WHEN  astext(transform(geomfromtext('POINT (2.1699187 41.387917)',4326),3395)) != 'POINT(241554.24474680217 5041471.505374054)'
	THEN 'Test 3: Failed.' 
	ELSE 'Test 3: OK.' END;

select CASE WHEN  srid(transform(geomfromtext('POINT (2.1699187 41.387917)',4326),3395)) != 3395
	THEN 'Test 4: Failed.' 
	ELSE 'Test 4: OK.' END;

select CASE WHEN  ST_LineMerge(geomfromtext('MULTILINESTRING((10 10, 15 15),(15 15, 30 30, 100 90))',4326)) != geomfromtext('LINESTRING(10 10,15 15,30 30,100 90)))',4326)
	THEN 'Test 5: Failed.' 
	ELSE 'Test 5: OK.' END;
	
select CASE WHEN  ST_Reverse(geomfromtext('POINT (2.1699187 41.387917)',4326)) != geomfromtext('POINT (2.1699187 41.387917)',4326)
	THEN 'Test 6: Failed.' 
	ELSE 'Test 6: OK.' END;

select CASE WHEN  ST_Reverse(geomfromtext('LINESTRING(-4.21875 26.3671875, -1.7578125 32.6953125, 7.03125 24.2578125, -0.87890625 21.09375)',4326)) != geomfromtext('LINESTRING( -0.87890625 21.09375, 7.03125 24.2578125, -1.7578125 32.6953125, -4.21875 26.3671875)',4326)
	THEN 'Test 7: Failed.' 
	ELSE 'Test 7: OK.' END;

select CASE WHEN  ST_Reverse(geomfromtext('MULTILINESTRING((10 10, 190 190),(15 15, 30 30, 100 90))',4326)) != geomfromtext('"MULTILINESTRING((190 190,10 10),(100 90,30 30,15 15))"',4326)
	THEN 'Test 8: Failed.' 
	ELSE 'Test 8: OK.' END;

select CASE WHEN  ST_Reverse(geomfromtext('POLYGON((9.4482421875 31.4208984375, 7.20703125 26.982421875, 12.12890625 27.6416015625, 9.4482421875 31.4208984375))',4326)) != geomfromtext('POLYGON(( 9.4482421875 31.4208984375, 12.12890625 27.6416015625, 7.20703125 26.982421875, 9.4482421875 31.4208984375))',4326)
	THEN 'Test 9: Failed.' 
	ELSE 'Test 9: OK.' END;

select CASE WHEN  ST_Union(geomfromtext('POINT( 2 2) ',4326),geomfromtext('POINT(2 2)',4326)) != geomfromtext('POINT(2 2)',4326)
	THEN 'Test 10: Failed.' 
	ELSE 'Test 10: OK.' END;

select CASE WHEN  ST_Union(geomfromtext('POINT( 2 2) ',4326),geomfromtext('POINT(2 4)',4326)) != geomfromtext('MULTIPOINT(2 2,2 4)',4326)
	THEN 'Test 11: Failed.' 
	ELSE 'Test 11: OK.' END;

select CASE WHEN  ST_Buffer(geomfromtext('POINT (2 2)',4326),10,1) != geomfromtext('POLYGON((12 2,2.000000000000016 -8,-8 1.9999999999999676,1.9999999999999538 12,12 2))',4326)
	THEN 'Test 12: Failed.' 
	ELSE 'Test 12: OK.' END;
