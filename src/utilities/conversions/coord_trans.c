#ifndef lint
static char rcsid [] = "$RCSfile: coord_trans.c,v $ $Revision: 1.4 $ $State: Exp $";
#endif
/****************************************************************************
*   File: coord_trans.c                                                     *
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
*       Contents: Code to do messy coordinate system transforms             *
*       Created: Mon Aug 17 1992                                            *
*       Author: jesmith                                                     *
*       Remarks: Mostly code written by other people, especially kch & bpwise
*                                                                           *
****************************************************************************/

#include "libcoord_local.h"
#include <libvecmat.h>
#include <stdext.h> /*common/include/global*/
#include <math.h>

/* DEFINES */
#define k0            0.9996                 /* scale on central meridian */

#define WGS84_MAJOR_AXIS 6378137.0
#define WGS84_MINOR_AXIS 6356752.3142

static void llz_to_xyz();
static void xyz_to_llz();
static void calc_ellipse_params();
static void convert_utm_datum();

void coord_gcc_to_llz(x_in, y_in, z_in, to_datum, lat_out, lon_out, z_out)
    float64  x_in;
    float64  y_in;
    float64  z_in;
    int32    to_datum;
    float64 *lat_out;
    float64 *lon_out;
    float64 *z_out;
{
    float64 x, y, z;
    float64 dx, dy, dz;

    /* Note: In the original code, the author starts by taking the WGS84
     * cartesian, translating it to WGS84 lat/lon, then back to WGS84
     * cartesian.  I can't imagine why this would be necessary.
     */

    /* Get datum shift parameters for to_datum */
    coord_get_datum_shifts(to_datum, &dx, &dy, &dz);
    
    /* convert from WGS84 x,y,z to the to_datum x,y,z */
    x = x_in - dx;
    y = y_in - dy;
    z = z_in - dz;
   
    /* convert from to_datum x,y,z to the to_datum lat,lon */
    xyz_to_llz(coord_get_major_axis(to_datum),
	       coord_get_minor_axis(to_datum),
	       x, y, z, lat_out, lon_out, z_out);
}

void coord_llz_to_gcc(from_datum, lat_in, lon_in, z_in, x_out, y_out, z_out)
    int32    from_datum;
    float64  lat_in;
    float64  lon_in;
    float64  z_in;
    float64 *x_out;
    float64 *y_out;
    float64 *z_out;
{
    float64 dx, dy, dz;

    llz_to_xyz(coord_get_major_axis(from_datum),
	       coord_get_minor_axis(from_datum),
	       lat_in, lon_in, z_in, x_out, y_out, z_out);

    /* get datum shift parameters for from_datum */
    coord_get_datum_shifts(from_datum, &dx, &dy, &dz);

    /* convert from_datum cartesian to WGS84 cartesian */
    *x_out += dx;
    *y_out += dy;
    *z_out += dz;
}

void coord_ll_to_utm(datum, lat, lon, zone, northing, easting)
    int32    datum;
    float64  lat;
    float64  lon;
    int32   *zone; /* Forced, if non-zero */
    float64 *northing;
    float64 *easting;
{
    /* Equations are taken from "Map Projections - A Working Manual" by
     * John Snyder, USGS Professional Paper 1395, Page 61.
     */

    float64 phai;		/* input latitude in degrees               */
    float64 lamda;		/* input longitude in degrees              */
    float64 lamda0;		/* longitude of central meridian           */
    float64 M,N,T,C,A;		/* basic parameters used for calcualtaions */
    float64 A2, A3, A4, A5, A6;	/* 'D' raised to the numbered power        */
    float64 T2;			/* 'T' raised to the numbered power        */
    float64 C2;			/* 'C' raised to the numbered power        */
    float64 P1, P2, P3, P4, P5;	/* miscellaneous terms                     */
    float64 cos_phai;		/* cosine of latitude                      */
    float64 cos_2phai;		/* cosine of 2*latitude                    */
    float64 sin_phai;		/* sine of latitude                        */
    float64 tan_phai;		/* tangent of latitude                     */
    float64 radius;		/* Equatorial radius of ellipsoid          */
    float64 e_sq;		/* eccentricity squared, see equations below*/
    float64 ep_sq;		/* e-prime squared, see equations below    */
    float64 ef;			/* e factor, see equations below           */
    int32  izone;		/* utm zone                   */
    float64 e_sq_2, e_sq_3, e_sq_4;

    /* Calculate utm zone */
    if (*zone)
      izone = *zone;
    else
      izone = COORD_LATLONG_TO_GRID_ZONE(lat, lon);

    calc_ellipse_params(datum, &radius, &e_sq, &ep_sq, &ef);

    e_sq_2 = e_sq*e_sq;
    e_sq_3 = e_sq_2*e_sq;
    e_sq_4 = e_sq_3*e_sq;

    P1 = 1.0 - (e_sq/4.0) - (3.0*e_sq_2/64.0) - (5.0*e_sq_3/256.0)
      - (175*e_sq_4/16384.0);
    P2 = (3.0*e_sq/8.0) + (3.0*e_sq_2/32.0) + (45.0*e_sq_3/1024.0)
      - (1365*e_sq_4/32768); 
    P3 = (15.0*e_sq_2/256.0) + (45.0*e_sq_3/1024.0) 
      - (1155*e_sq_4/32768.0);
    P4 = 35.0*e_sq_3/3072.0 - (1435.0*e_sq_4/98304);

    P5 = -(315*e_sq_4/131072);

    /* Calculate longitude of the central meridian */
    if ( izone < 31 )
      lamda0 = (float64) ((izone * 6) - 183);
    else
      lamda0 = (float64) (((izone - 31) * 6) + 3);

    /* convert latitudes and longitude to radians */
    phai = DEG_TO_RAD(lat);
    lamda = DEG_TO_RAD(lon);
    lamda0 = DEG_TO_RAD(lamda0);

    /* calculate the basic parameters used in calculations */
    cos_phai = cos(phai);
    sin_phai = sin(phai);
    tan_phai = tan(phai);
    C = ep_sq * cos_phai * cos_phai;
    T = tan_phai * tan_phai;
    N = radius / sqrt( 1.0 - (e_sq * sin_phai * sin_phai));
    A = (lamda - lamda0) * cos_phai;
#ifdef OLD_VERSION
    M = radius * (P1*phai - P2*sin(2.0*phai) + P3*sin(4.0*phai) -
		  P4*sin(6.0*phai) + P5*sin(8.0*phai));
#else
    cos_2phai = cos(2.0*phai);
    M = radius *
      (P1*phai + (sin(2.0*phai) *
		  ((P4-P2) +
		   cos_2phai * ((2.0*P3-4.0*P5) +
				cos_2phai * (-4.0*P4 +
					     cos_2phai * 8.0*P5)))));
#endif

    /* calculate parameters derived from above */
    T2 = T * T;
    C2 = C * C;
    A2 = A * A;
    A3 = A2 * A;
    A4 = A3 * A;
    A5 = A4 * A;
    A6 = A5 * A;

    /* calculate the easting */
    P1 = (1 - T + C);
    P2 = (5.0 - 18.0*T + T2 + 72.0*C - 58.0*ep_sq);
    *easting = k0 * N * (A + (P1*A3/6.0) + (P2*A5/120.0));

    /* calculate the northing */
    P1 = (5.0 - T + 9.0*C + 4.0*C2);
    P2 = (61.0 - 58.0*T + T2 + 600.0*C - 330.0*ep_sq);
    *northing = k0 * (M + N * tan_phai * 
		      (A2/2.0 + (P1*A4/24.0) + (P2*A6/720.0)));

    /* add false northing and easting */
    if (lat < 0)
      *northing = -( (*northing) + 10000000.0);
    *easting += 500000.0;
    *zone = izone;
}

void coord_utm_to_ll(datum, northing, easting, zone, lat, lon)
    int32    datum;
    float64  northing;
    float64  easting;
    int32    zone;
    float64 *lat;
    float64 *lon;
{
    float64 phai;               /* resulting latitude in radians           */
    float64 lamda;              /* resutling longitude in radians          */
    float64 phai1;              /* footprint latitude                      */
    float64 lamda0;             /* longitude of central meridian           */
    float64 mu,M,N,R,T,C,D;     /* basic parameters used for calcualtaions */
    float64 D2, D3, D4, D5, D6; /* 'D' raised to the numbered power        */
    float64 T2;                 /* 'T' raised to the numbered power        */
    float64 C2;                 /* 'C' raised to the numbered power        */
    float64 P1, P2, P3, P4, P5; /* miscellaneous terms                     */
    float64 cos_2mu;
    float64 cos_phai1;          /* cosine of latitude                      */
    float64 sin_phai1;          /* sine of latitude                        */
    float64 tan_phai1;          /* tangent of latitude                     */
    float64 radius;             /* Equatorial radius of ellipsoid          */
    float64 e_sq;               /* eccentricity squared                    */
    float64 ep_sq;              /* e-prime squared                         */
    float64 ef;                 /* e factor                                */
    float64 adj_easting;        /* easting minus false easting             */
    float64 adj_northing;       /* northing minus false northing           */
    float64 ef_4, ef_3, e_sq_2, e_sq_3;
   
    /* Equations are taken from "Map Projections - A Working Manual" by
     * John Snyder, USGS Professional Paper 1395, Pages 63 and 64.
     */

    calc_ellipse_params(datum, &radius, &e_sq, &ep_sq, &ef);

    ef_3 = ef*ef*ef;
    ef_4 = ef_3*ef;
    e_sq_2 = e_sq*e_sq;
    e_sq_3 = e_sq_2*e_sq;

    P1 =  1.0 - (e_sq/4.0) - (3.0*e_sq_2/64.0) - (5.0*e_sq_3/256.0);
    P2 = (3.0 * ef/2.0) - (27.0 * ef_3/32.0);
    P3 = (21.0 * ef*ef / 16.0 ) - (55.0 * ef_4 / 32.0); 
    P4 = 151.0 * ef_3 / 96.0;
    P5 = 1097.0 * ef_4 / 512.0;

    adj_easting = easting - 500000.0;
    if (northing < 0)
      adj_northing = -northing - 10000000.0;
    else
      adj_northing = northing;

    /* Calculate longitude of the central meridian */
    if ( zone < 31 )
      lamda0 = ( float64) ( ( zone * 6 ) - 183);
    else
      lamda0 =( float64)( ( ( zone - 31 ) * 6 ) + 3 );

    /* convert to radians */
    lamda0 = DEG_TO_RAD(lamda0);

    /* calculate true distance along central meridian from equator to phi */
    M = adj_northing / k0;

    /* calculate the footprint latitude, phai1 */
    mu = M / (radius * P1);
#ifdef OLD_VERSION
    phai1 = mu + P2*sin(2.0*mu) + P3*sin(4.0*mu) + P4*sin(6.0*mu) +
      P5*sin(8.0*mu);
#else
    cos_2mu = cos(2.0*mu);
    phai1 = mu + (sin(2.0*mu) *
		  ((P2-P4) +
		   cos_2mu * ((2.0*P3-4.0*P5) +
			      cos_2mu * (4.0*P4 +
					 cos_2mu * 8.0*P5))));
#endif

    /* calculate the basic parameters used calculations */
    cos_phai1 = cos(phai1);
    sin_phai1 = sin(phai1);
    tan_phai1 = tan(phai1);
    C = ep_sq * cos_phai1 * cos_phai1;
    T = tan_phai1 * tan_phai1;
    N = radius / sqrt( 1.0 - (e_sq * sin_phai1 * sin_phai1));
    P1 = sin_phai1 * sin_phai1;
    R = ( radius * ( 1 - e_sq ) ) /
      ( (1 - e_sq*P1)*sqrt(1 - e_sq*P1) );
    D =  adj_easting / ( N * k0);

    /* calculate various derived parameters from squaring, cubing, etc. */
    D2 = D * D ;
    D3 = D2 * D;
    D4 = D3 * D;
    D5 = D4 * D;
    D6 = D5 * D;
    C2 =  C * C;
    T2 =  T * T;

    /* calculate the latitude, phai */
    P1 =   D2/2.0;
    P2 = ( D4/24.0  ) * ( 5.0 + 3.0*T + 10.0*C - 4.0*C2 - 9.0*ep_sq) ;
    P3 = ( D6/720.0 ) * (61.0 + 90.0*T + 298.0*C + 45.0*T2 - 
			 252.0*ep_sq - 3.0*C2 );
    phai = phai1 - ( N * tan_phai1 / R ) * ( P1 - P2 + P3 );

    /* calculate the longitude, lambda */
    P1 = ( 1.0 + 2.0*T + C ) * D3 / 6.0;
    P2 = ( 5.0 - 2.0*C + 28.0*T - 3.0*C2 + 8.0*ep_sq + 24.0*T2 ) * D5 / 120.0;
    lamda = lamda0 + ((D - P1 + P2) / cos_phai1);

    /* convert to degrees and fraction thereof */
    *lat = RAD_TO_DEG(phai);
    *lon = RAD_TO_DEG(lamda);
}

void coord_gcc_to_tcc(x_in, y_in, z_in, tcc, x_out, y_out, z_out)
    float64       x_in;
    float64       y_in;
    float64       z_in;
    COORD_TCC_PTR tcc;
    float64      *x_out;
    float64      *y_out;
    float64      *z_out;
{
    float64 result[3];

    /* This routine *assumes* the TCC is GCC derived */

    vmat3e_mat_vec_mul64(tcc->t_to_g_rot,
			 x_in - tcc->center_in_gcc[X],
			 y_in - tcc->center_in_gcc[Y],
			 z_in - tcc->center_in_gcc[Z],
			 result);

    *x_out = result[X] + tcc->half_width; 
    *y_out = result[Y] + tcc->half_height;
    *z_out = result[Z];
}

void coord_tcc_to_gcc(tcc, x_in, y_in, z_in, x_out, y_out, z_out)
    COORD_TCC_PTR tcc;
    float64       x_in;
    float64       y_in;
    float64       z_in;
    float64      *x_out;
    float64      *y_out;
    float64      *z_out;
{
    float64 result[3];

    /* This routine *assumes* the TCC is GCC derived */

    result[X] = x_in - tcc->half_width;
    result[Y] = y_in - tcc->half_height;
    result[Z] = z_in;

    vmat3_vec_mat_mul64(result, tcc->t_to_g_rot, result);
    VMAT3_VEC_ADD(result, tcc->center_in_gcc, result);

    *x_out = result[X];
    *y_out = result[Y];
    *z_out = result[Z];
}

void coord_tcc_to_utm(tcc, x, y, zone, northing, easting)
    COORD_TCC_PTR tcc;
    float64       x;
    float64       y;
    int32        *zone;
    float64      *northing;
    float64      *easting;
{
    int32   from_zone = tcc->zone;
    float64 from_northing = tcc->northing + y;
    float64 from_easting = tcc->easting + x;
    int32   correct_datum;

    /* This routine *assumes* the TCC is UTM_NE derived */

    correct_datum = coord_lookup_datum_by_ne(from_zone,
					     from_northing, from_easting);

    if (correct_datum != tcc->datum)
      convert_utm_datum(from_zone, from_northing, from_easting, tcc->datum,
			&from_northing, &from_easting, correct_datum);

    /* Coerce to requested zone */
    coord_change_utm_zone(from_zone, from_northing, from_easting,
			  zone, northing, easting);
}

void coord_utm_to_tcc(zone, northing, easting, tcc, x, y)
    int32         zone;
    float64       northing;
    float64       easting;
    COORD_TCC_PTR tcc;
    float64      *x;
    float64      *y;
{
    int32   to_zone;
    float64 to_northing;
    float64 to_easting;
    int32   correct_datum;

    /* This routine *assumes* the TCC is UTM_NE derived */

    /* Coerce to TCC zone */
    to_zone = tcc->zone;
    coord_change_utm_zone(zone, northing, easting,
			  &to_zone, &to_northing, &to_easting);

    correct_datum = coord_lookup_datum_by_ne(to_zone,
					     to_northing, to_easting);

    if (correct_datum != tcc->datum)
      convert_utm_datum(zone, to_northing, to_easting, correct_datum,
			&to_northing, &to_easting, tcc->datum);

    *x = to_easting - tcc->easting;
    *y = to_northing - tcc->northing;
}

void coord_utm_to_grid(from_zone, from_northing, from_easting,
		       to_zone, to_resolution, to_string)
    int32     from_zone;
    float64   from_northing;
    float64   from_easting;
    int32    *to_zone;
    int32     to_resolution;
    char     *to_string;
{
    float64 to_northing;
    float64 to_easting;

    coord_change_utm_zone(from_zone, from_northing, from_easting,
			  to_zone, &to_northing, &to_easting);

    coord_lookup_utm(*to_zone, to_northing, to_easting,
		     to_resolution, to_string);
}

int32 coord_grid_to_utm(from_zone, from_string,
			to_zone, to_northing, to_easting)
    int32     from_zone;
    char     *from_string;
    int32    *to_zone;
    float64  *to_northing;
    float64  *to_easting;
{
    int32 zone;
    float64 northing;
    float64 easting;
    int32 ret;
    COORD_TCC_PTR tcc;

    if (!coord_tcc_list)
    {
	if (ret = coord_parse_utm(0, 0, from_string,
				  &zone, &northing, &easting))
	  return ret;
    }
    else
    {
	/* Try the zone data from each tcc */
	for (tcc=coord_tcc_list;tcc;tcc=tcc->next)
	{
	    if (from_zone == COORD_DEFAULT_ZONE)
	      zone = tcc->zone;
	    else
	      zone = from_zone;

	    if (!(ret = coord_parse_utm(zone, tcc->zone_letter, from_string,
					&zone, &northing, &easting)))
	      break;
	}

	/* If 'ret' is set, we had errors with every TCC we tried */
	if (ret)
	  return ret;
    }

    *to_zone = zone;
    coord_change_utm_zone(zone, northing, easting,
			  to_zone, to_northing, to_easting);
    return 0;
}

void coord_change_utm_zone(from_zone, from_northing, from_easting,
			   to_zone, to_northing, to_easting)
    int32     from_zone;
    float64   from_northing;
    float64   from_easting;
    int32    *to_zone;
    float64  *to_northing;
    float64  *to_easting;
{
    float64 latitude;
    float64 longitude;
    int32 from_datum, to_datum;

    /* Check the simplest case */
    if ((*to_zone == COORD_DEFAULT_ZONE) ||
	(from_zone == *to_zone))
    {
	*to_zone = from_zone;
	*to_northing = from_northing;
	*to_easting = from_easting;
	return;
    }

    from_datum = coord_lookup_datum_by_ne(from_zone,
					  from_northing, from_easting);
    coord_utm_to_ll(from_datum, from_northing, from_easting, from_zone,
		    &latitude, &longitude);

    to_datum = coord_lookup_datum_by_ll(latitude, longitude);
    coord_ll_to_utm(to_datum, latitude, longitude,
		    to_zone, to_northing, to_easting);
}

/*
 * Private utility functions
 */

#ifdef OLD_VERSION
static float64 square(x)
    double x;
{
    return(x*x);
}

static void xyz_to_llz(major_axis, minor_axis,
		       init_x, init_y, init_z, phi, lambda, height)
    float64  major_axis;  /* the semi-major axis of the reference ellipsoid */
    float64  minor_axis;  /* the semi-minor axis of the reference ellipsoid */
    float64  init_x;      /* the x coordinate in the local datum          */
    float64  init_y;      /* the y coordinate in the local datum          */
    float64  init_z;      /* the z coordinate in the local datum          */
    float64 *phi;         /* O - the latitude                             */
    float64 *lambda;      /* O - the longitude                            */
    float64 *height;      /* O - the height above the reference ellipsoid */
{
    float64 x, y, z, K, L, tol;
    float64 radius, major_sqrd;
    float64 T1, T2;
    float64 rat;

    /* calculates the closest point on the reference ellipsoid using
     * the iterative procedure known as Ben Wise's algorithm.
     * Latitude and longitude are calulated in a non-iterative manner.
     * The height above ground level (along the local vertical) is
     * simply the distance from the original Geocentric Cartesian
     * point to the point on the reference ellipsoid. See Tech Report
     * 7756 for full details.
     */

    T1 = ((2.0*major_axis*major_axis) + (minor_axis*minor_axis))/3.0;
    T2 = (2.0*major_axis + minor_axis)/6.0;
    major_sqrd = major_axis * major_axis;
    /* 10 centimeters tolerance ON the ellipsoid.
     * Final point is more accurate.
     */
    tol = 0.10;
    x = init_x;
    y = init_y;
    z = init_z;
    L = 0.0;
    while (fabs((square(x/major_axis) +
		 square(y/major_axis) +
		 square(z/minor_axis) - 1.0) * T2) > tol)
    {
	K = sqrt(square(init_x/(major_axis+L/major_axis)) 
		 + square(init_y/(major_axis+L/major_axis)) 
		 + square(init_z/(minor_axis+L/minor_axis)));
	L = L + T1*(K-1.0);
	x = (major_axis * init_x)/(major_axis + L/major_axis);
	y = (major_axis * init_y)/(major_axis + L/major_axis);
	z = (minor_axis * init_z)/(minor_axis + L/minor_axis);
    }

    radius = sqrt(x*x + y*y);
    *height = sqrt((init_x - x)*(init_x - x) +
		   (init_y - y)*(init_y - y) +
		   (init_z - z)*(init_z - z));

    /* atan2 is necessary to yield the right sign on the resulting longitude
     */
    if ((y == 0.0) && (x == 0.0))
      *lambda = 0.0;
    else
      *lambda = atan2(y, x);

    /* Rounding error can yield a radius which is slightly
     * larger than the major axis. -jesmith
     */
    rat = (radius*radius / major_sqrd);
    if (rat > 1.0)
      rat = 1.0;

    *phi = atan( sqrt(1.0 - rat)
		* ( major_sqrd 
		   / (minor_axis * radius)));

    /* We need get negative latitudes below the equator */
    if (init_z < 0.0)
      *phi = - (*phi);

    *lambda = RAD_TO_DEG(*lambda);
    *phi = RAD_TO_DEG(*phi);
}
#else
/*
 *   geocentric_to_geodetic:
 *   Performs the transformation between geocentric and geodetic
 *   coordinate systems.  This routine receives geocentriccoordinates
 *   (X,Y,Z) in meters and converts it into geodetic coordinatesgiven
 *   in latitude, longitude, and height.  This algorithm usesNewton-
 *   Raphson's convergence algorithm, and will stop iteration atvalues
 *   less than 50 cm.
 *   author:
 *   Huat Ng, IST.
 *
 *   Note that I've cleaned up the code, and fixed some dirt stupid
 *   inefficiencies. -jesmith
 */

#define PIOVER2 (PI * 0.5)

static void xyz_to_llz(major_axis, minor_axis,
		       xp, yp, zp, lat, lon, height)
    float64  major_axis;  /* the semi-major axis of the reference ellipsoid */
    float64  minor_axis;  /* the semi-minor axis of the reference ellipsoid */
    float64  xp;          /* the x coordinate in the local datum          */
    float64  yp;          /* the y coordinate in the local datum          */
    float64  zp;          /* the z coordinate in the local datum          */
    float64 *lat;         /* O - the latitude                             */
    float64 *lon;         /* O - the longitude                            */
    float64 *height;      /* O - the height above the reference ellipsoid */
{
    int32 at_pole;
    float64 m,h2,f,f_prime,h2_previous;
    float64 xp_sq,yp_sq,zp_sq,wp,wp_sq,x,y,z,w,w_sq;
    float64 tanphi,temp1,temp2,temp1_sq,temp2_sq,temp_m;
    float64 ellipse_a = major_axis;
    float64 ellipse_a_sq = major_axis*major_axis;
    float64 ellipse_b = minor_axis;
    float64 ellipse_b_sq = minor_axis*minor_axis;

    /* test for special cases on longitude.          */
    at_pole = FALSE;

    if (xp)			/* xp not equal to zero: ordinary case */
      *lon = atan2(yp,xp);
    else
    {
	/* xp is zero */
	if (yp > 0)
	  *lon = PIOVER2;
	else
	{
	    /* yp is either zero or less than zero */
	    if (yp < 0)
	      *lon = -PIOVER2;
	    else
	    {
		/* xp and yp is both zero: special case */
		at_pole = TRUE;
		if (zp > 0.0)
		  *lat = PIOVER2;
		else
		{
		    if(zp < 0.0)
		      *lat = -PIOVER2;
		    else
		    {
			*lat = *lon = *height = 0.0;
			return;
		    }
		}
	    }
	}
    }

    /* calculate squares of xp, yp, and zp */
    xp_sq = xp*xp;
    yp_sq = yp*yp;
    zp_sq = zp*zp;

    /* calculate wp square */
    wp_sq = xp_sq + yp_sq;
    wp = sqrt(wp_sq);

    /* Initial guess */
    temp_m = ellipse_a_sq*zp_sq + ellipse_b_sq*wp_sq;
    m = 0.5 * ((ellipse_a*ellipse_b*temp_m*(sqrt(temp_m) -
					    ellipse_a*ellipse_b))/
	       (ellipse_a_sq*ellipse_a_sq*zp_sq +
		ellipse_b_sq*ellipse_b_sq*wp_sq));

    /* calculate x,y,z */
    x = (1.0/(1.0+(2.0*m)/ellipse_a_sq))*xp;
    y = (1.0/(1.0+(2.0*m)/ellipse_a_sq))*yp;
    z = (1.0/(1.0+(2.0*m)/ellipse_b_sq))*zp;

    /* calculate height */
    h2 = (xp-x)*(xp-x) + (yp-y)*(yp-y) + (zp-z)*(zp-z);

    /* begins the iteration for convergence */
    do
    {
	h2_previous = h2;

	temp1 = ellipse_a + (2.0*m)/ellipse_a;
	temp2 = ellipse_b + (2.0*m)/ellipse_b;
	temp1_sq = temp1*temp1;
	temp2_sq = temp2*temp2;

	/* calculate f and f prime */
	f = wp_sq/temp1_sq + zp_sq/temp2_sq - 1.0;

	f_prime = - (4.0*wp_sq)/(ellipse_a*temp1*temp1_sq)
	  - (4.0*zp_sq)/(ellipse_b*temp2*temp2_sq);

	/* Newton-Raphson's convergence algorithm */

	m = m - f/f_prime;

	w = (1.0/(1.0+(2.0*m)/ellipse_a_sq))*wp;
	z = (1.0/(1.0+(2.0*m)/ellipse_b_sq))*zp;

	/* recalculate the height */
	h2 = (wp-w)*(wp-w) + (zp-z)*(zp-z);

    } while(fabs(h2-h2_previous) > 0.5);

    /* convert x,y,z into latitude, longitude, and height */
    w_sq = w*w;

    /* assign the value for height */
    *height = sqrt(h2);

    if ((wp_sq + zp_sq) < (w_sq + z*z))
      *height = -(*height);

    /* calculate for latitude */
    if (!at_pole)
    {
	tanphi = (ellipse_a_sq / ellipse_b *
		  sqrt(fabs(1.0 - w_sq / ellipse_a_sq))) / w;

	*lat = atan(tanphi);
	if (zp < 0) *lat = -(*lat);
    }

    *lat = RAD_TO_DEG(*lat);
    *lon = RAD_TO_DEG(*lon);
}
#endif

static void llz_to_xyz(a, b, lat_in, lon_in, z_in, x_out, y_out, z_out)
    float64  a;       /* semi-major axis of the reference ellipsoid */
    float64  b;       /* semi-minor axis of the reference ellipsoid */
    float64  lat_in;
    float64  lon_in;
    float64  z_in;
    float64 *x_out;
    float64 *y_out;
    float64 *z_out;
{
    float64 Rn, E2;
    float64 cos_lat_in, sin_lat_in;

    lat_in = DEG_TO_RAD(lat_in);
    lon_in = DEG_TO_RAD(lon_in);

    E2 = (((a*a)-(b*b))/(a*a));

    sin_lat_in = sin(lat_in);
    cos_lat_in = cos(lat_in);

    Rn = a / (sqrt((1 - (E2*sin_lat_in*sin_lat_in))));

    /* Compute the location in from_datum cartesian */
    *x_out = (Rn+z_in)*cos_lat_in*cos(lon_in);
    *y_out = (Rn+z_in)*cos_lat_in*sin(lon_in);
    *z_out =  ( ( ( ((b*b)/(a*a))*Rn+z_in) )*sin_lat_in );
}

static void calc_ellipse_params(datum, a, e_sq, ep_sq, ef)
    int32    datum;
    float64 *a;
    float64 *e_sq;
    float64 *ep_sq;
    float64 *ef;
{
    float64 b;
    float64 f;

    *a = coord_get_major_axis(datum);
    b = coord_get_minor_axis(datum);

    f = 1 - (b/(*a));

    *e_sq   = (2.0 - f) * f;
    *ep_sq  = *e_sq / (1.0 - *e_sq);
    *ef     = f / (2.0 - f);

    return;
}

static void convert_utm_datum(zone, from_northing, from_easting, from_datum,
			      to_northing, to_easting, to_datum)
    int32     zone;
    float64   from_northing;
    float64   from_easting;
    int32     from_datum;
    float64  *to_northing;
    float64  *to_easting;
    int32     to_datum;
{
    float64 latitude;
    float64 longitude;
    int32 to_zone = zone;

    coord_utm_to_ll(from_datum, from_northing, from_easting, zone,
		    &latitude, &longitude);
    coord_ll_to_utm(to_datum, latitude, longitude,
		    &to_zone, to_northing, to_easting);
}
