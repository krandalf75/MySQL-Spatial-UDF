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

// #define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
	#define DEBUG(...) msudf_debug(__VA_ARGS__);
#else
	#define DEBUG(...)
#endif


void msudf_debug(const char *fmt, ...) {
		va_list ap;

		FILE * fd = fopen("C:\\log.txt","a+" );

		fprintf( fd, "DEBUG: ");

		va_start (ap, fmt);
		vfprintf(fd, fmt, ap);
		va_end(ap);
		fprintf( fd, "\n" );

		fclose(fd);
}


GEOSCoordSeq msudf_transform_CoordSeq(const GEOSCoordSequence *seq,projPJ pj_src,projPJ pj_dst)
{
	double x,y;
	unsigned int i;
	unsigned int ncoords;
	GEOSCoordSeq seq2;

	DEBUG("msudf_transform_CoordSeq");
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

	DEBUG("msudf_transform_Geom");
	g1type = GEOSGeomTypeId(g1);

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
			g2ext = msudf_transform_Geom(g1aux,pj_src,pj_dst);
			ngeoms = GEOSGetNumInteriorRings(g1);
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
			DEBUG("GEOS_TYPE: default  %d",g1type);
			ngeoms = GEOSGetNumGeometries(g1);
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


typedef struct {
	projPJ pj_src, pj_dst;
	int srid_src, srid_dst;
	char *buf;
} transform_params;



my_bool msudf_transform_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	transform_params *params;

	DEBUG("msudf_transform_init");

	if (args->arg_count != 4) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	} else if(args->arg_type[2] != INT_RESULT) {
		strcpy(message,"Wrong type on parameter #3");
		return 1;
	} else if(args->arg_type[3] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #3");
		return 1;
	}
 
	DEBUG("msudf_transform_init OK1");
	initid->max_length= 0xFFFFFF;
	params = (transform_params *) malloc(sizeof(transform_params));
	params->pj_src = NULL;
	params->pj_dst = NULL;
	params->srid_src = 0;
	params->srid_dst = 0;
	params->buf = NULL;
	initid->ptr = (char *)params;
	DEBUG("msudf_transform_init OK2");
	return 0;
}

int getInt(const unsigned char *buf)
{
	int result;
    unsigned char *p = (unsigned char *)&result;

    p[0] = buf[0];
    p[1] = buf[1];
    p[2] = buf[2];
    p[3] = buf[3];

	return result;
}

char *msudf_transform(UDF_INIT *initid, UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb = NULL;
	size_t wkbsize;
	GEOSGeom g1,g2 =NULL;
	
	transform_params *params;
	int srid_src,srid_dst;


	params = (transform_params *) initid->ptr;
	

	DEBUG("msudf_transform");
	wkb = (unsigned char *) (args->args[0]);
	g1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	srid_src = getInt(wkb);
	wkb = NULL;

	DEBUG("srid_src: %d",srid_src);
	if ((params->pj_src == NULL) || (params->srid_src != srid_src)) {
		params->pj_src = pj_init_plus(args->args[1]);
		params->srid_src = srid_src;
	}

	srid_dst = getInt((unsigned char *) args->args[2]);
	DEBUG("srid_dst: %d",srid_dst);
	if ((params->pj_dst == NULL) || (params->srid_dst != srid_dst)) {
		params->pj_dst = pj_init_plus(args->args[3]);
		params->srid_dst = srid_dst;
	}

	if (params->pj_src != NULL && params->pj_dst != NULL && g1 != NULL) {
		g2 =msudf_transform_Geom(g1,params->pj_src,params->pj_dst);
		wkb = GEOSGeomToWKB_buf(g2,&wkbsize);
	} else {
		// initid->ptr = NULL;
		strcpy(error,"Invalid geometry.");
	}

	if (g1 != NULL) GEOSGeom_destroy(g1);
	if (g2 != NULL) GEOSGeom_destroy(g2);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (params->buf != NULL) free(params->buf);
		params->buf = (char *) malloc(*length);
		memcpy(params->buf,args->args[2],4);
		memcpy((char *)params->buf + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return params->buf;
	} else {
		*is_null = 1;
		return NULL;
	}

}


void msudf_transform_deinit(UDF_INIT *initid)
{
	transform_params *params;

	if (initid->ptr != NULL) {
		params = (transform_params *) initid->ptr;
		free(params->buf);
		if (params->pj_src != NULL) pj_free(params->pj_src);
		if (params->pj_dst != NULL) pj_free(params->pj_dst);
		free(initid->ptr);
		initid->ptr = NULL;
	}
}


my_bool msudf_simplify_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_simplify_init");

	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != REAL_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	}

	DEBUG("msudf_simplify_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	DEBUG("msudf_simplify_init OK2");
	return 0;

}

void msudf_simplify_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_simplify_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_simplify_deinit OK");
}

char *msudf_simplify(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geom1,geom2;
	double tolerance;

	DEBUG("msudf_simplify");

	wkb = (unsigned char *) (args->args[0]);
	geom1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	wkb = NULL;
	tolerance = *((double*) args->args[1]);
	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	geom2 = GEOSSimplify(geom1,tolerance);
	wkb = GEOSGeomToWKB_buf(geom2,&wkbsize);

	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_simplifyPreserveTopology_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_ssimplifyPreserveTopology_init");

	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	} else if (args->arg_type[1] != REAL_RESULT) {
		strcpy(message,"Wrong type on parameter #2");
		return 1;
	}

	DEBUG("msudf_simplifyPreserveTopology_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	DEBUG("msudf_simplifyPreserveTopology_init OK2");
	return 0;

}

void msudf_simplifyPreserveTopology_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_simplifyPreserveTopology_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_simplifyPreserveTopology_deinit OK");
}


char *msudf_simplifyPreserveTopology(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geom1,geom2;
	double tolerance;

	DEBUG("msudf_simplifyPreserveTopology");

	wkb = (unsigned char *) (args->args[0]);
	geom1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	wkb = NULL;
	tolerance = *((double*) args->args[1]);
	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	geom2 = GEOSTopologyPreserveSimplify(geom1,tolerance);
	wkb = GEOSGeomToWKB_buf(geom2,&wkbsize);

	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_lineMerge_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_lineMerge_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} else if (args->arg_type[0] != STRING_RESULT) {
		strcpy(message,"Wrong type on parameter #1");
		return 1;
	}

	DEBUG("msudf_lineMerge_init OK1");
	initid->max_length= 0xFFFFFF;
	initid->ptr = NULL;
	initid->const_item = 0;
	initid->decimals = 0;
	initid->maybe_null = 1;
	DEBUG("msudf_lineMerge_init OK2");
	return 0;

}

void msudf_lineMerge_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_lineMerge_deinit");
	if (initid->ptr != NULL) {
		free(initid->ptr);
		initid->ptr = NULL;
	}
	DEBUG("msudf_lineMerge_deinit OK");
}


char *msudf_lineMerge(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb;
	size_t wkbsize;
	GEOSGeom geom1,geom2;

	DEBUG("msudf_lineMerge");

	wkb = (unsigned char *) (args->args[0]);
	DEBUG("geom_length: %d",args->lengths[0]);
	geom1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	wkb = NULL;
	
	if (geom1 == NULL) {
		DEBUG("msudf_lineMerge: Invalid geometry.");
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	geom2 = GEOSLineMerge(geom1);
	wkb = GEOSGeomToWKB_buf(geom2,&wkbsize);

	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);


	if (wkb != NULL) {
		*length = (long)wkbsize + 4;
		if (initid->ptr != NULL) free(initid->ptr);
		initid->ptr = (char *) malloc(*length);
		memcpy(initid->ptr,args->args[0],4);
		memcpy((char *)initid->ptr + 4,wkb,wkbsize);
		GEOSFree((char *)wkb);
		return initid->ptr;
	} else {
		*is_null = 1;
		return NULL;
	}
}

