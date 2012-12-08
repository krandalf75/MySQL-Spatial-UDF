// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MSUDF_API
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MSUDF_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(WIN32)
	#ifdef MSUDF_EXPORTS
	#define MSUDF_API __declspec(dllexport)
	#else
	#define MSUDF_API __declspec(dllimport)
	#endif
	#include <winsock2.h>
#else
	#define MSUDF_API
#endif

#include <mysql.h>

MSUDF_API my_bool msudf_boundary_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_boundary_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_boundary(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_buffer_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_buffer_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_buffer(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_centroid_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_centroid_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_centroid(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_contains_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_contains_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_contains(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);

MSUDF_API my_bool msudf_convexHull_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_convexHull_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_convexHull(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_crosses_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_crosses_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_crosses(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);

MSUDF_API my_bool msudf_difference_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_difference_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_difference(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_disjoint_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_disjoint_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_disjoint(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);

MSUDF_API my_bool msudf_intersects_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_intersects_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_intersects(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);

MSUDF_API my_bool msudf_intersection_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_intersection_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_intersection(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_isEmpty_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_isEmpty_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_isEmpty(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);

MSUDF_API my_bool msudf_isSimple_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_isSimple_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_isSimple(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);

MSUDF_API my_bool msudf_isRing_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_isRing_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_isRing(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);

MSUDF_API my_bool msudf_isValid_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_isValid_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_isValid(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);

MSUDF_API my_bool msudf_lineMerge_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_lineMerge_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_lineMerge(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_lineSubstring_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_lineSubstring_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_lineSubstring(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_overlaps_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_overlaps_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_overlaps(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);

MSUDF_API my_bool msudf_pointOnSurface_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_pointOnSurface_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_pointOnSurface(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_reverse_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_reverse_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_reverse(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_symDifference_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_symDifference_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_symDifference(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_simplify_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_simplify_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_simplify(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_simplifyPreserveTopology_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_simplifyPreserveTopology_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_simplifyPreserveTopology(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_touches_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_touches_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_touches(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);

MSUDF_API my_bool msudf_transform_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_transform_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_transform(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_union_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_union_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_union(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_within_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_within_deinit(UDF_INIT *initid);
MSUDF_API long long msudf_within(UDF_INIT *initid,UDF_ARGS *args,char *is_null, char *error);





#ifdef __cplusplus
}
#endif

#ifdef MSUDF_INTERNAL

	#include <proj_api.h>
	#include <geos_c.h>

	typedef struct {
		projPJ pj_src, pj_dst;
		int srid_src, srid_dst;
		char *buf;
	} msudf_params;

	int msudf_getInt(const unsigned char *buf);
	GEOSGeom msudf_getGeometry(unsigned char *buf,unsigned int length);
	char *msudf_returnGeometry(UDF_INIT *initid,unsigned long* length,GEOSGeom geom);
	void msudf_init(UDF_INIT *initid);
	void msudf_deinit(UDF_INIT *initid);
#endif