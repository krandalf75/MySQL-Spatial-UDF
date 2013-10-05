
#define MSUDF_INTERNAL 1

#include "MySpatialUDF.h"
#include "GeometryUtils.h"


// #define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
	#define DEBUG(...) gu_debug(__VA_ARGS__);
#else
	#define DEBUG(...)
#endif

static int initGeosInitialized = 0;

void msudf_init(UDF_INIT *initid)
{
	msudf_params *params;

	DEBUG("initGeosInitialized %d", initGeosInitialized);

	if (!initGeosInitialized) {
		DEBUG("Initializing GEOS ...");
		initGeosInitialized = 1;
		initGEOS(gu_debug,gu_debug);
	}

	params = (msudf_params *) malloc(sizeof(msudf_params));
	params->pj_src = NULL;
	params->pj_dst = NULL;
	params->srid_src = 0;
	params->srid_dst = 0;
	params->buf = NULL;
	initid->ptr = (char *)params;

}

void msudf_deinit(UDF_INIT *initid)
{
	msudf_params *params;

	if (initid->ptr != NULL) {
		params = (msudf_params *) initid->ptr;
		DEBUG("A punt per alliberar buf %p",params->buf);
		if (params->buf != NULL) free(params->buf);
		DEBUG("A punt per alliberar pj_src %p",params->pj_src);
		if (params->pj_src != NULL) pj_free(params->pj_src);
		DEBUG("A punt per alliberar pj_dst %p",params->pj_dst);
		if (params->pj_dst != NULL) pj_free(params->pj_dst);
		DEBUG("pj_dst OK");
		free(initid->ptr);
		DEBUG("punter OK");
		initid->ptr = NULL;
	}
}

char *msudf_returnGeometry(UDF_INIT *initid,unsigned long* length,GEOSGeom geom)
{
	unsigned char *wkb;
	msudf_params *params;
	size_t wkbsize;
	int srid;

	wkb = GEOSGeomToWKB_buf(geom,&wkbsize);
	*length = (long)wkbsize + 4;
	srid = GEOSGetSRID(geom);

	params = (msudf_params *) initid->ptr;
	if (params->buf != NULL) free(params->buf);
	params->buf = (char *) malloc(*length);

	memcpy(params->buf,&srid,4);
	memcpy((char *)params->buf + 4,wkb,wkbsize);
	GEOSFree((char *)wkb);
	return params->buf;
}


int msudf_getInt(const unsigned char *buf)
{
	int result;
    unsigned char *p = (unsigned char *)&result;

    p[0] = buf[0];
    p[1] = buf[1];
    p[2] = buf[2];
    p[3] = buf[3];

	return result;
}

GEOSGeom msudf_getGeometry(unsigned char *buf,unsigned int length)
{
	int srid;
	GEOSGeom geom;

	geom = GEOSGeomFromWKB_buf(buf + 4,length - 4);
	if (geom != NULL) {
		srid = msudf_getInt(buf);
		GEOSSetSRID(geom,srid);
	}
	return geom;
}

my_bool msudf_boundary_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_boundary_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;

}

void msudf_boundary_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_boundary_deinit");
	msudf_deinit(initid);
}

char *msudf_boundary(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geom1,geom2;

	DEBUG("msudf_boundary");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}
	
	geom2 = GEOSBoundary(geom1);

	if (geom2 != NULL) {
		GEOSSetSRID(geom2,GEOSGetSRID(geom1));
		result = msudf_returnGeometry(initid,length,geom2);
		GEOSGeom_destroy(geom1);
		GEOSGeom_destroy(geom2);
		return result;
	} else {
		GEOSGeom_destroy(geom1);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_buffer_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_buffer_init");

	if (args->arg_count < 2 || args->arg_count > 3 ) {
		strcpy(message,"Wrong # arguments");
		return 1;
	}

	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = REAL_RESULT;
	if (args->arg_count > 2) {
		args->arg_type[2] = INT_RESULT;
	};
	
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;

}

void msudf_buffer_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_buffer_deinit");
	msudf_deinit(initid);
}

char *msudf_buffer(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geom1,geom2;
	double buffer;
	int quadsegs;

	DEBUG("msudf_buffer");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	buffer = *((double*) args->args[1]);
	
	if (args->arg_count > 2 && args->arg_type[2] == INT_RESULT) {
		quadsegs = *((int *)args->args[2]);
	} else {
		quadsegs = 8;
	}

	geom2 = GEOSBuffer(geom1,buffer,quadsegs);

	if (geom2 != NULL) {
        GEOSSetSRID(geom2,GEOSGetSRID(geom1));
		result = msudf_returnGeometry(initid,length,geom2);
		GEOSGeom_destroy(geom1);
		GEOSGeom_destroy(geom2);
		return result;
	} else {
		GEOSGeom_destroy(geom1);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_centroid_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_centroid_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;

}

void msudf_centroid_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_centroid_deinit");
	msudf_deinit(initid);
}

char *msudf_centroid(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geom1,geom2;

	DEBUG("msudf_centroid");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}
	
	geom2 = GEOSGetCentroid(geom1);

	if (geom2 != NULL) {
		GEOSSetSRID(geom2,GEOSGetSRID(geom1));
		result = msudf_returnGeometry(initid,length,geom2);
		GEOSGeom_destroy(geom1);
		GEOSGeom_destroy(geom2);
		return result;
	} else {
		GEOSGeom_destroy(geom1);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_contains_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_contains_init");
	
	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_contains_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_contains(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom1,geom2;
	long long result;

	DEBUG("msudf_contains");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);		
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
	
	geom2 = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geom2 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
		
	result = GEOSContains(geom1,geom2);
	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);
	
	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}

my_bool msudf_convexHull_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_convexHull_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;

}

void msudf_convexHull_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_convexHull_deinit");
	msudf_deinit(initid);
}

char *msudf_convexHull(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geom1,geom2;

	DEBUG("msudf_convexHull");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}
	
	geom2 = GEOSConvexHull(geom1);

	if (geom2 != NULL) {
		GEOSSetSRID(geom2,GEOSGetSRID(geom1));
		result = msudf_returnGeometry(initid,length,geom2);
		GEOSGeom_destroy(geom1);
		GEOSGeom_destroy(geom2);
		return result;
	} else {
		GEOSGeom_destroy(geom1);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_crosses_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_crosses_init");
	
	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_crosses_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_crosses(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom1,geom2;
	long long result;

	DEBUG("msudf_crosses");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);		
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
	
	geom2 = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geom2 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
		
	result = GEOSCrosses(geom1,geom2);
	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);
	
	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}

my_bool msudf_difference_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_difference_init");

	if (args->arg_count != 2 ) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;
}

void msudf_difference_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_difference_deinit");
	msudf_deinit(initid);
}

char *msudf_difference(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geomFirst,geomSecond,geomResult;

	DEBUG("msudf_difference");
	
	geomFirst = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);
	if (geomFirst == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	geomSecond = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geomSecond == NULL) {
		GEOSGeom_destroy(geomFirst);
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	geomResult = GEOSDifference(geomFirst,geomSecond);

	if (geomResult!= NULL) {
		GEOSSetSRID(geomResult,GEOSGetSRID(geomFirst));
		result = msudf_returnGeometry(initid,length,geomResult);
		GEOSGeom_destroy(geomFirst);
		GEOSGeom_destroy(geomSecond);
		GEOSGeom_destroy(geomResult);
		return result;
	} else {
		GEOSGeom_destroy(geomFirst);
		GEOSGeom_destroy(geomSecond);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_disjoint_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_disjoint_init");
	
	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_disjoint_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_disjoint(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom1,geom2;
	long long result;

	DEBUG("msudf_disjoint");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);		
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
	
	geom2 = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geom2 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
		
	result = GEOSDisjoint(geom1,geom2);
	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);
	
	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}

my_bool msudf_intersection_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_intersection_init");

	if (args->arg_count != 2 ) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;
}

void msudf_intersection_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_intersection_deinit");
	msudf_deinit(initid);
}

char *msudf_intersection(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geomFirst,geomSecond,geomResult;

	DEBUG("msudf_intersection");
	
	geomFirst = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);
	if (geomFirst == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	geomSecond = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geomSecond == NULL) {
		GEOSGeom_destroy(geomFirst);
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	geomResult = GEOSIntersection(geomFirst,geomSecond);

	if (geomResult!= NULL) {
		GEOSSetSRID(geomResult,GEOSGetSRID(geomFirst));
		result = msudf_returnGeometry(initid,length,geomResult);
		GEOSGeom_destroy(geomFirst);
		GEOSGeom_destroy(geomSecond);
		GEOSGeom_destroy(geomResult);
		return result;
	} else {
		GEOSGeom_destroy(geomFirst);
		GEOSGeom_destroy(geomSecond);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_intersects_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_intersects_init");
	
	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_intersects_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_intersects(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom1,geom2;
	long long result;

	DEBUG("msudf_intersects");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);		
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
	
	geom2 = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geom2 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
		
	result = GEOSIntersects(geom1,geom2);
	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);
	
	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}

my_bool msudf_isEmpty_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_isEmpty_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_isEmpty_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_isEmpty(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom;
	long long result;

	DEBUG("msudf_isEmpty");
	
	geom = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);
	
	if (geom == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	result = GEOSisEmpty(geom);
	if (geom != NULL) GEOSGeom_destroy(geom);

	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}

my_bool msudf_isSimple_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_isSimple_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_isSimple_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_isSimple(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom;
	long long result;

	DEBUG("msudf_isSimple");
	
	geom = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);
	
	if (geom == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	result = GEOSisSimple(geom);
	if (geom != NULL) GEOSGeom_destroy(geom);

	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}

my_bool msudf_isRing_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_isRing_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_isRing_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_isRing(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom;
	long long result;

	DEBUG("msudf_isRing");
	
	geom = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);
	
	if (geom == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	result = GEOSisRing(geom);
	if (geom != NULL) GEOSGeom_destroy(geom);

	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}

my_bool msudf_isValid_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_isValid_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_isValid_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_isValid(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom;
	long long result;

	DEBUG("msudf_isValid");
	
	geom = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);
	
	if (geom == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	result = GEOSisValid(geom);
	if (geom != NULL) GEOSGeom_destroy(geom);

	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}

my_bool msudf_overlaps_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_overlaps_init");
	
	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_overlaps_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_overlaps(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom1,geom2;
	long long result;

	DEBUG("msudf_overlaps");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);		
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
	
	geom2 = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geom2 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
		
	result = GEOSOverlaps(geom1,geom2);
	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);
	
	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}

my_bool msudf_touches_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_touches_init");
	
	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_touches_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_touches(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom1,geom2;
	long long result;

	DEBUG("msudf_touches");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);		
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
	
	geom2 = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geom2 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
		
	result = GEOSTouches(geom1,geom2);
	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);
	
	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}



my_bool msudf_transform_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_transform_init");

	if (args->arg_count != 4) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 

	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	args->arg_type[2] = INT_RESULT;
	args->arg_type[3] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_transform_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_transform_deinit");
	msudf_deinit(initid);
}

char *msudf_transform(UDF_INIT *initid, UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	unsigned char *wkb = NULL;
	size_t wkbsize;
	GEOSGeom g1,g2 =NULL;
	
	msudf_params *params;
	int srid_src,srid_dst;


	params = (msudf_params *) initid->ptr;
	

	DEBUG("msudf_transform");
	wkb = (unsigned char *) (args->args[0]);
	g1 = GEOSGeomFromWKB_buf(wkb + 4,args->lengths[0] - 4);
	srid_src = msudf_getInt(wkb);
	wkb = NULL;

	DEBUG("srid_src: %d",srid_src);
	if ((params->pj_src == NULL) || (params->srid_src != srid_src)) {
		params->pj_src = pj_init_plus(args->args[1]);
		params->srid_src = srid_src;
	}

	srid_dst = msudf_getInt((unsigned char *) args->args[2]);
	DEBUG("srid_dst: %d",srid_dst);
	if ((params->pj_dst == NULL) || (params->srid_dst != srid_dst)) {
		params->pj_dst = pj_init_plus(args->args[3]);
		params->srid_dst = srid_dst;
	}

	if (params->pj_src != NULL && params->pj_dst != NULL && g1 != NULL) {
		g2 = gu_transformGeom(g1,params->pj_src,params->pj_dst);
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

my_bool msudf_symDifference_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_symDifference_init");

	if (args->arg_count != 2 ) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;
}

void msudf_symDifference_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_symDifference_deinit");
	msudf_deinit(initid);
}

char *msudf_symDifference(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geomFirst,geomSecond,geomResult;

	DEBUG("msudf_symDifference");
	
	geomFirst = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);
	if (geomFirst == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	geomSecond = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geomSecond == NULL) {
		GEOSGeom_destroy(geomFirst);
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	geomResult = GEOSSymDifference(geomFirst,geomSecond);

	if (geomResult!= NULL) {
		GEOSSetSRID(geomResult,GEOSGetSRID(geomFirst));
		result = msudf_returnGeometry(initid,length,geomResult);
		GEOSGeom_destroy(geomFirst);
		GEOSGeom_destroy(geomSecond);
		GEOSGeom_destroy(geomResult);
		return result;
	} else {
		GEOSGeom_destroy(geomFirst);
		GEOSGeom_destroy(geomSecond);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_simplify_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_simplify_init");

	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	}
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = REAL_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;
}

void msudf_simplify_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_simplify_deinit");
	msudf_deinit(initid);
}

char *msudf_simplify(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geom2;
	GEOSGeom geom1; 
	double tolerance; 
	DEBUG("msudf_simplify");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	tolerance = *(double*) args->args[1] ;

	geom2 = GEOSSimplify(geom1,tolerance);

	if (geom2 != NULL) {
		GEOSSetSRID(geom2,GEOSGetSRID(geom1));
		result = msudf_returnGeometry(initid,length,geom2);
		GEOSGeom_destroy(geom1);
		GEOSGeom_destroy(geom2);
		return result;
	} else {
		GEOSGeom_destroy(geom1);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_simplifyPreserveTopology_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_simplifyPreserveTopology_init");

	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	}
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = REAL_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;
}

void msudf_simplifyPreserveTopology_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_simplifyPreserveTopology_deinit");
	msudf_deinit(initid);
}

char *msudf_simplifyPreserveTopology(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geom2;
	GEOSGeom geom1; 
	double tolerance; 
	DEBUG("msudf_simplifyPreserveTopology");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	tolerance = *(double*) args->args[1] ;

	geom2 = GEOSTopologyPreserveSimplify(geom1,tolerance);

	if (geom2 != NULL) {
		GEOSSetSRID(geom2,GEOSGetSRID(geom1));
		result = msudf_returnGeometry(initid,length,geom2);
		GEOSGeom_destroy(geom1);
		GEOSGeom_destroy(geom2);
		return result;
	} else {
		GEOSGeom_destroy(geom1);
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
	} 
	
	args->arg_type[0] = STRING_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;

}

void msudf_lineMerge_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_lineMerge_deinit");
	msudf_deinit(initid);
}

char *msudf_lineMerge(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geom1,geom2;

	DEBUG("msudf_lineMerge");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}
	
	geom2 = GEOSLineMerge(geom1);

	if (geom2 != NULL) {
		GEOSSetSRID(geom2,GEOSGetSRID(geom1));
		result = msudf_returnGeometry(initid,length,geom2);
		GEOSGeom_destroy(geom1);
		GEOSGeom_destroy(geom2);
		return result;
	} else {
		GEOSGeom_destroy(geom1);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_lineSubstring_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_lineSubstring_init");

	if (args->arg_count != 3) {
		strcpy(message,"Wrong # arguments");
		return 1;
	}
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = REAL_RESULT;
	args->arg_type[2] = REAL_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;
}

void msudf_lineSubstring_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_lineSubstring_deinit");
	msudf_deinit(initid);
}

char *msudf_lineSubstring(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geom2;
	GEOSGeom geom1; 
	double start; 
	double end; 
	DEBUG("msudf_lineSubstring");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	start = *(double*) args->args[1] ;
	end = *(double*) args->args[2] ;

	geom2 = gu_substringLineGeom(geom1,start,end);

	if (geom2 != NULL) {
		GEOSSetSRID(geom2,GEOSGetSRID(geom1));
		result = msudf_returnGeometry(initid,length,geom2);
		GEOSGeom_destroy(geom1);
		GEOSGeom_destroy(geom2);
		return result;
	} else {
		GEOSGeom_destroy(geom1);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_pointOnSurface_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_pointOnSurface_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;

}

void msudf_pointOnSurface_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_pointOnSurface_deinit");
	msudf_deinit(initid);
}

char *msudf_pointOnSurface(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geom1,geom2;

	DEBUG("msudf_pointOnSurface");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}
	
	geom2 = GEOSPointOnSurface(geom1);

	if (geom2 != NULL) {
		GEOSSetSRID(geom2,GEOSGetSRID(geom1));
		result = msudf_returnGeometry(initid,length,geom2);
		GEOSGeom_destroy(geom1);
		GEOSGeom_destroy(geom2);
		return result;
	} else {
		GEOSGeom_destroy(geom1);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_reverse_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_reverse_init");

	if (args->arg_count != 1) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;

}

void msudf_reverse_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_reverse_deinit");
	msudf_deinit(initid);
}

char *msudf_reverse(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geom1,geom2;

	DEBUG("msudf_reverse");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return NULL;
	}

	geom2 = gu_reverseGeom(geom1);

	if (geom2 != NULL) {
		GEOSSetSRID(geom2,GEOSGetSRID(geom1));
		result = msudf_returnGeometry(initid,length,geom2);
		GEOSGeom_destroy(geom1);
		GEOSGeom_destroy(geom2);
		return result;
	} else {
		GEOSGeom_destroy(geom1);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_union_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_union_init");

	if (args->arg_count != 2 ) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	initid->max_length= 0xFFFFFF;
	msudf_init(initid);
	return 0;
}

void msudf_union_deinit(UDF_INIT *initid)
{
	DEBUG("msudf_union_deinit");
	msudf_deinit(initid);
}

char *msudf_union(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error)
{
	char *result;
	GEOSGeom geomFirst,geomSecond,geomResult;

	DEBUG("msudf_union");
	
	geomFirst = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);
	if (geomFirst == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	geomSecond = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geomSecond == NULL) {
		GEOSGeom_destroy(geomFirst);
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	geomResult = GEOSUnion(geomFirst,geomSecond);

	if (geomResult!= NULL) {
		GEOSSetSRID(geomResult,GEOSGetSRID(geomFirst));
		result = msudf_returnGeometry(initid,length,geomResult);
		GEOSGeom_destroy(geomFirst);
		GEOSGeom_destroy(geomSecond);
		GEOSGeom_destroy(geomResult);
		return result;
	} else {
		GEOSGeom_destroy(geomFirst);
		GEOSGeom_destroy(geomSecond);
		*is_null = 1;
		return NULL;
	}
}

my_bool msudf_within_init(UDF_INIT *initid,UDF_ARGS *args,char *message)
{
	DEBUG("msudf_within_init");
	
	if (args->arg_count != 2) {
		strcpy(message,"Wrong # arguments");
		return 1;
	} 
	args->arg_type[0] = STRING_RESULT;
	args->arg_type[1] = STRING_RESULT;
	msudf_init(initid);
	return 0;
}

void msudf_within_deinit(UDF_INIT *initid)
{
	msudf_deinit(initid);
}

long long msudf_within(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error)
{
	GEOSGeom geom1,geom2;
	long long result;

	DEBUG("msudf_within");

	geom1 = msudf_getGeometry((unsigned char *)args->args[0],args->lengths[0]);	
	if (geom1 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}

	geom2 = msudf_getGeometry((unsigned char *)args->args[1],args->lengths[1]);	
	if (geom2 == NULL) {
		strcpy(error,"Invalid geometry.");
		*is_null = 1;
		return 0;
	}
	
	result = GEOSWithin(geom1,geom2);
	if (geom1 != NULL) GEOSGeom_destroy(geom1);
	if (geom2 != NULL) GEOSGeom_destroy(geom2);
	
	if (result >1) {
		*is_null = 1;
		return 0;
	} else {
		return result;
	}
}




