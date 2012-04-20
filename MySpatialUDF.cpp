#include "MySpatialUDF.h"

void notice(const char *fmt, ...) {
		va_list ap;

		FILE * fd = fopen("C:\\log.txt","a+" );

		fprintf( fd, "NOTICE: ");

		va_start (ap, fmt);
		vfprintf(fd, fmt, ap);
		va_end(ap);
		fprintf( fd, "\n" );

		fclose(fd);
}
#ifdef DEBUG
	#define DEBUG(...) debug(__VA_ARGS__);
#else
	#define DEBUG(...)
#endif


void debug(const char *fmt, ...) {
		va_list ap;

		FILE * fd = fopen("C:\\log.txt","a+" );

		fprintf( fd, "DEBUG: ");

		va_start (ap, fmt);
		vfprintf(fd, fmt, ap);
		va_end(ap);
		fprintf( fd, "\n" );

		fclose(fd);
}


my_bool msudf_transform_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	if (args->arg_count != 4) {
		strcpy(message,"Wrong #arguments to msudf_validate");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT || 
		args->arg_type[1] != STRING_RESULT ||
		args->arg_type[2] != INT_RESULT ||
		args->arg_type[3] != STRING_RESULT) {
			strcpy(message,"Wrong #arguments to msudf_validate");
		return 1;
	} 
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	return 0;

}


/*
	MYSQL *mysql;

	mysql = mysql_init(NULL);
	if (mysql_real_connect(mysql,NULL,"root","admin",NULL,0,NULL,0)== NULL) {
		notice("Error %u: %s\n", mysql_errno(mysql), mysql_error(mysql));
	}
	
	notice("%s",mysql_get_host_info(mysql));

	if (mysql_query(mysql, "create database testdb")) {
		notice("Error %u: %s\n", mysql_errno(mysql), mysql_error(mysql));
		
	}



	mysql_close(mysql);
*/


GEOSCoordSeq msudf_transform_CoordSeq(const GEOSCoordSequence *seq,projPJ pj_src,projPJ pj_dst)
{
	double x,y;
	unsigned int i;
	unsigned int ncoords;
	GEOSCoordSeq seq2;

	DEBUG("msudf_transform_CoordSeq\n");
	ncoords = 0;
	GEOSCoordSeq_getSize(seq,&ncoords);
	seq2 = GEOSCoordSeq_create(ncoords,2);
	for (i=0; i<ncoords;i++) {
		// MySQL only supports 2D geometries
		GEOSCoordSeq_getX(seq,i,&x);
		GEOSCoordSeq_getY(seq,i,&y);
		if (pj_is_latlong(pj_src)) {
			// radiants conversion
			x = x * PI/180.0;
			y = y * PI/180.0;
		}
		pj_transform(pj_src,pj_dst,1,0,&x,&y,NULL);
		if (pj_is_latlong(pj_dst)) {
			// degrees conversion
			x = x * 180.0/PI;
			y = y * 180.0/PI;
		}
		GEOSCoordSeq_setX(seq2,i,x);
		GEOSCoordSeq_setY(seq2,i,y);
	}

	return seq2;
}

GEOSGeom msudf_transform_Geom(const GEOSGeom g1,projPJ pj_src,projPJ pj_dst)
{
	int g1type;
	unsigned int ngeoms;
	unsigned int i;
	GEOSCoordSeq seq2;
	GEOSGeom g2 =NULL;
	GEOSGeom g1aux = NULL;
	GEOSGeom g2ext = NULL;
	GEOSGeom *g2int = NULL;

	DEBUG("msudf_transform_Geom\n");
	g1type = GEOSGeomTypeId(g1);
	DEBUG("HERE2\n");
	DEBUG("g1type: %d\n",g1type);

	switch (g1type) {
		case GEOS_POINT:
			DEBUG("GEOS_POINT");
			seq2 = msudf_transform_CoordSeq(GEOSGeom_getCoordSeq(g1),pj_src,pj_dst);
			g2 = GEOSGeom_createPoint(seq2);
			break;
		case GEOS_LINESTRING:
			DEBUG("GEOS_LINESTRING");
			seq2 = msudf_transform_CoordSeq(GEOSGeom_getCoordSeq(g1),pj_src,pj_dst);
			g2 = GEOSGeom_createLineString(seq2);
			break;
		case GEOS_LINEARRING:
			DEBUG("GEOS_LINEARRING");
			seq2 = msudf_transform_CoordSeq(GEOSGeom_getCoordSeq(g1),pj_src,pj_dst);
			g2 = GEOSGeom_createLinearRing(seq2);
			break;
		case GEOS_POLYGON:
			DEBUG("GEOS_POLYGON");
			g1aux = (GEOSGeom) GEOSGetExteriorRing(g1);
			DEBUG("g1aux done.\n");
			g2ext = msudf_transform_Geom(g1aux,pj_src,pj_dst);
			DEBUG("g2ext done.\n");
			ngeoms = GEOSGetNumInteriorRings(g1);
			DEBUG("ngeoms: %d\n",ngeoms);
			if (ngeoms > 0) {
				g2int = (GEOSGeom *) calloc(ngeoms,sizeof(GEOSGeom));
				for (i=0;i<ngeoms;i++) {
					g1aux = (GEOSGeom) GEOSGetInteriorRingN(g1,i);
					g2int[i] = msudf_transform_Geom(g1aux,pj_src,pj_dst);
				}
			} else {
				g2int = NULL;
			}

			g2 = GEOSGeom_createPolygon(g2ext,g2int,ngeoms);
			free(g2int);
			break;
		default:
			DEBUG("DEFAULT %d\n",g1type);
			ngeoms = GEOSGetNumGeometries(g1);
			DEBUG("ngeoms: %d\n",ngeoms);
			g2int = (GEOSGeom *) calloc(ngeoms,sizeof(GEOSGeom));
			for (i=0;i<ngeoms;i++) {
				g1aux = (GEOSGeom) GEOSGetGeometryN(g1,i);
				g2int[i] = msudf_transform_Geom(g1aux,pj_src,pj_dst);
			}
			g2 = GEOSGeom_createCollection(g1type,g2int,ngeoms);
			free(g2int);
	}

	return g2;
}

char *msudf_transform(UDF_INIT *initid, UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	GEOSGeom g1,g2 =NULL;
	projPJ pj_src, pj_dst;
	size_t wkbsize;
	unsigned char *wkb = NULL;
	

	DEBUG("msudf_transform\n");
	pj_src = pj_init_plus(args->args[1]);
	pj_dst = pj_init_plus(args->args[3]);
	wkb = (unsigned char *) (args->args[0]);
	g1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	wkb = NULL;

	if (pj_src != NULL && pj_dst != NULL && g1 != NULL) {
		g2 =msudf_transform_Geom(g1,pj_src,pj_dst);
		wkb = GEOSGeomToWKB_buf(g2,&wkbsize);
	} else {
		initid->ptr = NULL;
		strcpy(error,"Invalid geometry.");
	}

	if (g1 != NULL) GEOSGeom_destroy(g1);
	if (g2 != NULL) GEOSGeom_destroy(g2);
	if (pj_src != NULL) pj_free(pj_src);
	if (pj_dst != NULL) pj_free(pj_dst);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[2],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}

}


void msudf_transform_deinit(UDF_INIT *initid)
{
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
}


