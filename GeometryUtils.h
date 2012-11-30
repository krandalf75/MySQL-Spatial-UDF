
#ifndef _GeometryUtils_
#define _GeometryUtils_

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>


#include <proj_api.h>
#include <geos_c.h>


#ifndef PI
	#define PI       3.14159265358979323846
#endif

void gu_debug(const char *fmt, ...);

GEOSCoordSeq gu_reverseCoordSeq(const GEOSCoordSequence *seq);
GEOSCoordSeq gu_transformCoordSeq(const GEOSCoordSequence *seq,projPJ pj_src,projPJ pj_dst);
GEOSCoordSeq gu_substringCoordSeq(const GEOSCoordSequence *seq, double dstart, double dend);

GEOSGeom gu_reverseGeom(const GEOSGeom geom);
GEOSGeom gu_transformGeom(const GEOSGeom geom,projPJ pj_src,projPJ pj_dst);
GEOSGeom gu_substringLineGeom(const GEOSGeom geom,double start,double end);

#endif