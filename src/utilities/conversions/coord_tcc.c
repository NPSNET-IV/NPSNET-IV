#ifndef lint
static char rcsid [] = "$RCSfile: coord_tcc.c,v $ $Revision: 1.5 $ $State: Exp $";
#endif
/****************************************************************************
*   File: coord_tcc.c                                                       *
*                                                                           *
*       Copyright 1993 by Loral Advanced Distributed Simulation, Inc.       *
*                                                                           *
*               Loral Advanced Distributed Simulation, Inc.                 *
*               10 Moulton Street                                           *
*               Cambridge, MA 02238                                         *
*               617-873-1850                                                *
*                                                                           *
*       This software was developed by Loral under U. S. Government contracts *
*       and may be reproduced by or for the U. S. Government pursuant to    *
*       the copyright license under the clause at DFARS 252.227-7013        *
*       (OCT 1988).                                                         *
*                                                                           *
*       Contents: Code to allocate TCC structures                           *
*       Created: Wed Sep  9 1992                                            *
*       Author: jesmith                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libcoord_local.h"
#include <math.h>
#include <stdext.h> /*common/include/global*/
#include <stdalloc.h> /*common/include/global*/

COORD_TCC_PTR coord_tcc_list = NULL;

COORD_TCC_PTR coord_define_tcc(source, northing, easting, znum, zlet, datum,
			       width, height)
    COORD_SYSTEM source;
    float64      northing;
    float64      easting;
    int32        znum;
    char         zlet;
    int32        datum;
    int32        width;
    int32        height;
{
    float64 latitude, longitude;
    float64 x, y, z, h;
    float64 sin_lat, cos_lat, sin_lon, cos_lon;
    COORD_TCC_PTR result = (COORD_TCC_PTR )STDALLOC(sizeof(struct coord_tcc));

    result->next = coord_tcc_list;
    coord_tcc_list = result;

    if ((source != COORD_GCC) && (source != COORD_UTM_NE))
    {
	printf("LIBCOORDINATES WARNING: Bad source passed to coord_define_tcc: %d\n",
	       source);
	source = COORD_GCC;
    }

    result->source = source;
    result->datum = datum;
    result->zone = znum;
    result->zone_letter = zlet;
    result->northing = northing;
    result->easting = easting;
    result->half_width = width * 0.5;
    result->half_height = height * 0.5;

    /* Find the x, y, & z in GCC */
    coord_utm_to_ll(datum, northing+height*0.5, easting+width*0.5, znum,
		    &latitude, &longitude);
    coord_llz_to_gcc(datum, latitude, longitude, 0.0, &x, &y, &z);

    /* Find the latitude & longitude in WGS84 (this will be different
     * from the latitude & longitude we just computed if the datum uses
     * spheroid model other than WGS84).
     */
    coord_gcc_to_llz(x, y, z, COORD_GCC_DATUM, &latitude, &longitude, &h);

    /* The following corresponds to equation 21 in the paper:
     * "Interconversions Between Different Coordinate Systems"
     * Kuo-Chi Lin, Huat Keng Ng; IST UCF
     */
    sin_lat = sin(DEG_TO_RAD(latitude));
    cos_lat = cos(DEG_TO_RAD(latitude));
    sin_lon = sin(DEG_TO_RAD(longitude));
    cos_lon = cos(DEG_TO_RAD(longitude));

    result->t_to_g_rot[X][X] = -sin_lon;
    result->t_to_g_rot[X][Y] =  cos_lon;
    result->t_to_g_rot[X][Z] =  0.0;

    result->t_to_g_rot[Y][X] = -sin_lat * cos_lon;
    result->t_to_g_rot[Y][Y] = -sin_lat * sin_lon;
    result->t_to_g_rot[Y][Z] =  cos_lat;

    result->t_to_g_rot[Z][X] =  cos_lat * cos_lon;
    result->t_to_g_rot[Z][Y] =  cos_lat * sin_lon;
    result->t_to_g_rot[Z][Z] =  sin_lat;

    result->center_in_gcc[X] = x;
    result->center_in_gcc[Y] = y;
    result->center_in_gcc[Z] = z;

    return result;
}

COORD_TCC_PTR coord_estimate_tcc(source,
				 latitude_hun_sec, longitude_hun_sec,
				 utm_grid, width, height)
    COORD_SYSTEM source;
    int32        latitude_hun_sec;
    int32        longitude_hun_sec;
    char        *utm_grid;
    int32        width;
    int32        height;
{
    float64 latitude = latitude_hun_sec / 360000.0;
    float64 longitude = longitude_hun_sec / 360000.0;
    int32 zone, datum;
    char zlet;
    float64 northing, easting;

    zone = 0;

    /* Find the zone, northing & easting from the lat/long */
    if (coord_convert(COORD_LATLON, latitude, longitude, TRUE,
		      COORD_UTM_NE,
		      &zone, &northing, &easting))
      return NULL;

    /* Look up the zone letter */
    zlet = coord_lookup_zone_letter(zone, northing);

    /* Guess at the datum (works on all current databases) */
    datum = coord_lookup_datum_by_ne(zone, northing, easting);

    /* The lat/long was good enough to find datum/zone type info, but
     * to get a solid northing/easting, we need to use the milgrid.
     */
    if (coord_grid_to_utm(zone, utm_grid, &zone, &northing, &easting))
      return NULL;

    return coord_define_tcc(source, northing, easting, zone, zlet, datum,
			    width, height);
}    

static void unit_vector(dx, dy, dz, row)
    float64 dx, dy, dz;
    float64 *row;
{
    float64 mag = 1.0 / sqrt(dx*dx + dy*dy + dz*dz);

    row[X] = dx * mag;
    row[Y] = dy * mag;
    row[Z] = dz * mag;
}

int32 coord_tcc_gcc_rotation(tcc, mat)
    COORD_TCC_PTR tcc;
    float64      *mat;
{
    float64 x0,y0,z0, x1,y1,z1;

    if (!tcc)
      return COORD_BAD_TCC;

    if (tcc->source == COORD_GCC)
    {
	bcopy(tcc->t_to_g_rot, mat, 9*sizeof(float64));
	return 0;
    }
    else if (tcc->source == COORD_UTM_NE)
    {
	/* Assemble a rotation matrix by converting points and making
	 * vectors.  (Horribly inefficient, but this will only be run
	 * at init time, and finding this analytically is a hard problem).
	 * Note that this matrix is only correct at the center of the
	 * database.
	 */
	coord_convert(COORD_TCC,
		      tcc, tcc->half_width, tcc->half_height, 0.0,
		      COORD_GCC,
		      &x0, &y0, &z0);

	/* X row */
	coord_convert(COORD_TCC,
		      tcc, tcc->half_width+1.0, tcc->half_height, 0.0,
		      COORD_GCC,
		      &x1, &y1, &z1);
	unit_vector(x1-x0, y1-y0, z1-z0, mat);

	/* Y row */
	coord_convert(COORD_TCC,
		      tcc, tcc->half_width, tcc->half_height+1.0, 0.0,
		      COORD_GCC,
		      &x1, &y1, &z1);
	unit_vector(x1-x0, y1-y0, z1-z0, mat+3);

	/* Z row */
	coord_convert(COORD_TCC,
		      tcc, tcc->half_width, tcc->half_height, 1.0,
		      COORD_GCC,
		      &x1, &y1, &z1);
	unit_vector(x1-x0, y1-y0, z1-z0, mat+6);

	return 0;
    }
    else
      return COORD_BAD_TCC;
}
