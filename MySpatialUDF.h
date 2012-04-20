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

#ifndef PI
	#define PI       3.14159265358979323846
#endif

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
#include <proj_api.h>
#include <geos_c.h>

MSUDF_API my_bool msudf_transform_init(UDF_INIT *initid,UDF_ARGS *args,char *message);
MSUDF_API void msudf_transform_deinit(UDF_INIT *initid);
MSUDF_API char *msudf_transform(UDF_INIT *initid,UDF_ARGS *args, char *buf,
	unsigned long *length, char *is_null, char *error);

GEOSCoordSeq msudf_transform_CoordSeq(const GEOSCoordSequence *seq,projPJ pj_src,projPJ pj_dst);

#ifdef __cplusplus
}
#endif