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

MSUDF_API my_bool msudf_buffer_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_buffer_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_buffer(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_convexHull_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_convexHull_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_convexHull(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_difference_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_difference_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_difference(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_intersection_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_intersection_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_intersection(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_lineMerge_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_lineMerge_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_lineMerge(UDF_INIT *initid,UDF_ARGS *args, char *buf,
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

MSUDF_API my_bool msudf_transform_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_transform_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_transform(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

MSUDF_API my_bool msudf_union_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_union_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_union(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

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
	} transform_params;

	int msduf_getInt(const unsigned char *buf);

#endif