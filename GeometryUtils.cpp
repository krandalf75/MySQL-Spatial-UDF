
#include "GeometryUtils.h"

// #define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
	#define DEBUG(...) gu_debug(__VA_ARGS__);
#else
	#define DEBUG(...)
#endif


void gu_debug(const char *fmt, ...) {
		va_list ap;

		FILE * fd = fopen("C:\\log.txt","a+" );

		fprintf( fd, "DEBUG: ");

		va_start (ap, fmt);
		vfprintf(fd, fmt, ap);
		va_end(ap);
		fprintf( fd, "\n" );

		fclose(fd);
}

GEOSCoordSeq gu_transformCoordSeq(const GEOSCoordSequence *seq,projPJ pj_src,projPJ pj_dst)
{
	double x,y;
	unsigned int i;
	unsigned int ncoords;
	GEOSCoordSeq seq2;

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

GEOSGeom gu_transformGeom(const GEOSGeom geom,projPJ pj_src,projPJ pj_dst)
{
	int geomType;
	unsigned int numGeoms;
	unsigned int i;
	GEOSCoordSeq seq2;
	GEOSGeom g2 =NULL;
	GEOSGeom geomAux = NULL;
	GEOSGeom g2ext = NULL;
	GEOSGeom *geomArray = NULL;

	DEBUG("gu_transformGeom");
	geomType = GEOSGeomTypeId(geom);

	switch (geomType) {
		case GEOS_POINT:
			DEBUG("GEOS_POINT");
			seq2 = gu_transformCoordSeq(GEOSGeom_getCoordSeq(geom),pj_src,pj_dst);
			g2 = GEOSGeom_createPoint(seq2);
			break;
		case GEOS_LINESTRING:
			DEBUG("GEOS_LINESTRING");
			seq2 = gu_transformCoordSeq(GEOSGeom_getCoordSeq(geom),pj_src,pj_dst);
			g2 = GEOSGeom_createLineString(seq2);
			break;
		case GEOS_LINEARRING:
			DEBUG("GEOS_LINEARRING");
			seq2 = gu_transformCoordSeq(GEOSGeom_getCoordSeq(geom),pj_src,pj_dst);
			g2 = GEOSGeom_createLinearRing(seq2);
			break;
		case GEOS_POLYGON:
			DEBUG("GEOS_POLYGON");
			geomAux = (GEOSGeom) GEOSGetExteriorRing(geom);
			g2ext = gu_transformGeom(geomAux,pj_src,pj_dst);
			numGeoms = GEOSGetNumInteriorRings(geom);
			if (numGeoms > 0) {
				geomArray = (GEOSGeom *) calloc(numGeoms,sizeof(GEOSGeom));
				for (i=0;i<numGeoms;i++) {
					geomAux = (GEOSGeom) GEOSGetInteriorRingN(geom,i);
					geomArray[i] = gu_transformGeom(geomAux,pj_src,pj_dst);
				}
			} else {
				geomArray = NULL;
			}

			g2 = GEOSGeom_createPolygon(g2ext,geomArray,numGeoms);
			free(geomArray);
			break;
		default:
			DEBUG("GEOS_TYPE: default  %d",geomType);
			numGeoms = GEOSGetNumGeometries(geom);
			geomArray = (GEOSGeom *) calloc(numGeoms,sizeof(GEOSGeom));
			for (i=0;i<numGeoms;i++) {
				geomAux = (GEOSGeom) GEOSGetGeometryN(geom,i);
				geomArray[i] = gu_transformGeom(geomAux,pj_src,pj_dst);
			}
			g2 = GEOSGeom_createCollection(geomType,geomArray,numGeoms);
			free(geomArray);
	}

	return g2;
}

GEOSCoordSeq gu_reverseCoordSeq(const GEOSCoordSequence *seq)
{
	unsigned int i,j,k;
	unsigned int numCoords;
	unsigned int dimCoords;
	GEOSCoordSeq seqResult;
	double value;
	
	GEOSCoordSeq_getSize(seq,&numCoords);
	GEOSCoordSeq_getDimensions(seq,&dimCoords);
	seqResult = GEOSCoordSeq_create(numCoords,dimCoords);
	k = numCoords -1;
	for (i=0; i<numCoords;i++) {
		for (j=0; j<dimCoords;j++) {
			GEOSCoordSeq_getOrdinate(seq,i,j,&value);
			GEOSCoordSeq_setOrdinate(seqResult,k,j,value);
		}
		k--;
	}
	return seqResult;
}




GEOSGeom gu_reverseGeom(const GEOSGeom geom)
{
	int geomType;
	unsigned int i;
	unsigned int numGeoms;	
	GEOSGeom geomResult,geomAux,geomExt;
	GEOSCoordSequence* cs;
	GEOSGeom *geomArray = NULL;

	geomType = GEOSGeomTypeId(geom);

	switch (geomType) {
		case GEOS_POINT:
			cs = GEOSCoordSeq_clone(GEOSGeom_getCoordSeq(geom));			
			geomResult = GEOSGeom_createPoint(cs);
			break;
		case GEOS_LINESTRING:
			cs = gu_reverseCoordSeq(GEOSGeom_getCoordSeq(geom));
			geomResult = GEOSGeom_createLineString(cs);
			break;
		case GEOS_LINEARRING:
			cs = gu_reverseCoordSeq(GEOSGeom_getCoordSeq(geom));
			geomResult = GEOSGeom_createLinearRing(cs);
			break;
		case GEOS_POLYGON:
			numGeoms = GEOSGetNumInteriorRings(geom);
			if (numGeoms > 0) {
				geomArray = (GEOSGeom *) calloc(numGeoms,sizeof(GEOSGeom));
				for (i=0;i<numGeoms;i++) {
					geomAux = (GEOSGeom) GEOSGetInteriorRingN(geom,i);
					geomArray[i] = gu_reverseGeom(geomAux);
				}
			} else {
				geomArray = NULL;
			}

			geomAux = (GEOSGeom) GEOSGetExteriorRing(geom);
			geomExt = gu_reverseGeom(geomAux);

			geomResult = GEOSGeom_createPolygon(geomExt,geomArray,numGeoms);
			free(geomArray);
			break;

		default:
			numGeoms = GEOSGetNumGeometries(geom);
			geomArray = (GEOSGeom *) calloc(numGeoms,sizeof(GEOSGeom));
			for (i=0;i<numGeoms;i++) {
				geomAux = (GEOSGeom) GEOSGetGeometryN(geom,i);
				cs = gu_reverseCoordSeq(GEOSGeom_getCoordSeq(geomAux));
				geomArray[i] = gu_reverseGeom(geomAux);
			}
			geomResult = GEOSGeom_createCollection(geomType,geomArray,numGeoms);
			free(geomArray);
	}

	return geomResult;
}


GEOSCoordSeq gu_substringCoordSeq(const GEOSCoordSequence *seq, double dstart, double dend)
{	
	unsigned int numCoords,dimCoords,i,j,curCoord;
	double ox,oy,fx,fy,dx,dy,px,py,distance0,distance1,value;
	GEOSCoordSequence *seqAux;
	GEOSCoordSequence *seqResult;
	int in = 0;


	GEOSCoordSeq_getSize(seq,&numCoords);
	DEBUG("gu_substringCoordSeq numCoords: %d",numCoords);
	GEOSCoordSeq_getDimensions(seq,&dimCoords);
	DEBUG("gu_substringCoordSeq dimCoords: %d",dimCoords);
	seqAux = GEOSCoordSeq_clone(seq);
	DEBUG("gu_substringCoordSeq OK0");
	if (numCoords > 1) {
		i=1;
		distance0 = 0;
		distance1 = 0;
		curCoord =0;
		GEOSCoordSeq_getX(seq,0,&fx);
		GEOSCoordSeq_getY(seq,0,&fy);

		do {
			ox = fx;
			oy = fy;
			distance0 = distance1;
			GEOSCoordSeq_getX(seq,i,&fx);
			GEOSCoordSeq_getY(seq,i,&fy);
			dx = fx - ox;
			dy = fy - oy;
			distance1 += sqrt(dx * dx + dy * dy);
			i++;
		} while ((distance1 < dstart) && (i < numCoords));

		// Add start point
		px = ox +  dx * (dstart - distance0) / (distance1 - distance0);
		py = oy +  dy * (dstart - distance0) / (distance1 - distance0);
		GEOSCoordSeq_setX(seqAux,curCoord, px);
		GEOSCoordSeq_setY(seqAux,curCoord, py);
		curCoord++;

	
		while ((distance1 < dend) && (i < numCoords)) {
			DEBUG("gu_substringCoordSeq in %f,%f",fx,fy);
			if ( px != fx ||py != fy) {
				GEOSCoordSeq_setX(seqAux,curCoord,fx);
				GEOSCoordSeq_setY(seqAux,curCoord,fy);
				curCoord++;
				px = fx;
				py = fy;
			}
			ox = fx;
			oy = fy;
			distance0 = distance1;
			GEOSCoordSeq_getX(seq,i,&fx);
			GEOSCoordSeq_getY(seq,i,&fy);
			dx = fx - ox;
			dy = fy - oy;
			distance1 += sqrt(dx * dx + dy * dy);
			i++;
		}

		DEBUG("FIN: gu_substringCoordSeq in %d distance %f",i,distance1);

		// Add end point
		if (dstart != dend) {
			px = ox +  dx * (dend- distance0) / (distance1 - distance0);
			py = oy +  dy * (dend- distance0) / (distance1 - distance0);
			GEOSCoordSeq_setX(seqAux,curCoord, px);
			GEOSCoordSeq_setY(seqAux,curCoord, py);
			curCoord++;
		}
		DEBUG("gu_substringCoordSeq done!!");
	} else {
		curCoord = 1;
	}

	seqResult = GEOSCoordSeq_create(curCoord,dimCoords);
	for (i=0; i< curCoord; i++) {
		for (j=0; j<dimCoords;j++) {
			GEOSCoordSeq_getOrdinate(seqAux,i,j,&value);
			GEOSCoordSeq_setOrdinate(seqResult,i,j,value);
		}
	}

	GEOSCoordSeq_destroy(seqAux);

	return seqResult;				
}

GEOSGeom gu_substringLineGeom(const GEOSGeom geom,double start,double end)
{
	int geomType;
	GEOSGeom geomResult;
	double distance,dstart,dend;
	GEOSCoordSequence* cs;
	unsigned int numCoords;

	geomType = GEOSGeomTypeId(geom);

	switch (geomType) {
		case GEOS_LINESTRING:
			GEOSLength(geom,&distance);
			DEBUG("gu_substringLineGeom distance %f",distance);
			dstart = distance * start;
			dend = distance * end;
			cs = gu_substringCoordSeq(GEOSGeom_getCoordSeq(geom),dstart,dend);
			GEOSCoordSeq_getSize(cs,&numCoords);
			if (numCoords > 1) {
				geomResult = GEOSGeom_createLineString(cs);
			} else {
				geomResult = GEOSGeom_createPoint(cs);
			}
			break;
		default:
			geomResult = NULL;
	}

	return geomResult;
}

