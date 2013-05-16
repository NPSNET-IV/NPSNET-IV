#ifndef lint
static char rcsid [] = "$RCSfile: coord_convert.c,v $ $Revision: 1.5 $ $State: Exp $";
#endif
/****************************************************************************
*   File: coord_convert.c                                                   *
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
*       Contents: Code to do simple conversions                             *
*       Created: Fri Aug 14 1992                                            *
*       Author: jesmith                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libcoord_local.h"
#include <ctype.h>
#include <varargs.h>

#ifdef USESTRINGDOTH
# include <string.h>
#else
# include <strings.h>
#endif

#define ROUND(x) ((int32)((x)+0.5))

/*VARARGS*/
int32 coord_convert(va_alist)
    va_dcl
{
    va_list       ap;
    /* Possible from args */
    COORD_SYSTEM  from_system;
    float64       from_x;
    float64       from_y;
    float64       from_z;
    COORD_TCC_PTR from_tcc;
    int32         from_zone;
    float64       from_northing;
    float64       from_easting;
    int32         from_resolution;
    char         *from_string;
    float64       from_latitude;
    float64       from_longitude;
    int32         from_local_datum;
    /* Possible to args */
    COORD_SYSTEM  to_system;
    float64      *to_x;
    float64      *to_y;
    float64      *to_z;
    COORD_TCC_PTR to_tcc;
    int32        *to_zone;
    float64      *to_northing;
    float64      *to_easting;
    int32         to_resolution;
    char         *to_string;
    float64      *to_latitude;
    float64      *to_longitude;
    int32         to_local_datum;
    /* Intermediate step variables */
    int32         ret;
    float64       northing, easting;
    float64       latitude, longitude;
    float64       x, y, z;
    int32         datum;
    int32         zone;
    int32         dummy_zone = 0;

    va_start(ap);

    /* Get the from_system args */

    from_system = (COORD_SYSTEM)va_arg(ap, int32);
    switch(from_system)
    {
      case COORD_GCC:
	from_x = va_arg(ap,float64);
	from_y = va_arg(ap,float64);
	from_z = va_arg(ap,float64);
	break;
      case COORD_TCC:
	from_tcc = va_arg(ap,COORD_TCC_PTR);
	if (!from_tcc)
	  return COORD_BAD_TCC;
	from_x = va_arg(ap,float64);
	from_y = va_arg(ap,float64);
	from_z = va_arg(ap,float64);
	break;
      case COORD_UTM_NE:
	from_zone = va_arg(ap,int32);
	from_northing = va_arg(ap,float64);
	from_easting = va_arg(ap,float64);
	from_z = va_arg(ap,float64);
	break;
      case COORD_UTM_GRID:
	from_zone = va_arg(ap,int32);
	from_resolution = va_arg(ap,int32);
	from_string = va_arg(ap,char *);
	from_z = va_arg(ap,float64);
	break;
      case COORD_LATLON:
	from_latitude = va_arg(ap,float64);
	from_longitude = va_arg(ap,float64);
	from_z = va_arg(ap,float64);
	from_local_datum = va_arg(ap,int32);
	break;
      default:
	return COORD_SYSTEM_UNKNOWN;
    }

    /* Get the to_system args */

    to_system = (COORD_SYSTEM)va_arg(ap, int32);
    switch(to_system)
    {
      case COORD_GCC:
	to_x = va_arg(ap,float64 *);
	to_y = va_arg(ap,float64 *);
	to_z = va_arg(ap,float64 *);
	break;
      case COORD_TCC:
	to_tcc = va_arg(ap,COORD_TCC_PTR);
	if (!to_tcc)
	  return COORD_BAD_TCC;
	to_x = va_arg(ap,float64 *);
	to_y = va_arg(ap,float64 *);
	to_z = va_arg(ap,float64 *);
	break;
      case COORD_UTM_NE:
	to_zone = va_arg(ap,int32 *);
	to_northing = va_arg(ap,float64 *);
	to_easting = va_arg(ap,float64 *);
	to_z = va_arg(ap,float64 *);
	break;
      case COORD_UTM_GRID:
	to_zone = va_arg(ap,int32 *);
	if (to_zone == COORD_DEFAULT_ZONE)
	  to_zone = &dummy_zone;
	to_resolution = va_arg(ap,int32);
	to_string = va_arg(ap,char *);
	to_z = va_arg(ap,float64 *);
	break;
      case COORD_LATLON:
	to_latitude = va_arg(ap,float64 *);
	to_longitude = va_arg(ap,float64 *);
	to_z = va_arg(ap,float64 *);
	to_local_datum = va_arg(ap,int32);
	break;
      case COORD_CHARACTER_STRING:
	to_string = va_arg(ap,char *);
	break;
      default:
	return COORD_SYSTEM_UNKNOWN;
    }

    switch(from_system)
    {
      case COORD_GCC:
	switch(to_system)
	{
	  case COORD_GCC:
	    *to_x = from_x;
	    *to_y = from_y;
	    *to_z = from_z;
	    return 0;
	  case COORD_TCC:
	    if (to_tcc->source == COORD_GCC)
	    {
		coord_gcc_to_tcc(from_x, from_y, from_z,
				 to_tcc, to_x, to_y, to_z);
		return 0;
	    }
	    else /* (to_tcc->source == COORD_UTM_NE) */
	    {
		zone = to_tcc->zone;
		if (ret = coord_convert(COORD_GCC,
					from_x, from_y, from_z,
					COORD_UTM_NE,
					&zone, &northing, &easting, &z))
		  return ret;
		return coord_convert(COORD_UTM_NE,
				     zone, northing, easting, z,
				     COORD_TCC,
				     to_tcc, to_x, to_y, to_z);
	    }
	  case COORD_UTM_NE:
	    /* First we translate to llz using the GCC datum, then
	     * we will have a lat/long with which we can find out what
	     * datum we should have used.
	     */
	    coord_gcc_to_llz(from_x, from_y, from_z,
			     COORD_GCC_DATUM, &latitude, &longitude, to_z);
	    datum = coord_lookup_datum_by_ll(latitude, longitude);
	    if (datum != COORD_GCC_DATUM)
	    {
		/* We guessed wrong, translate again using the right datum */
		coord_gcc_to_llz(from_x, from_y, from_z,
				 datum, &latitude, &longitude, to_z);
	    }
	    coord_ll_to_utm(datum, latitude, longitude,
			    to_zone, to_northing, to_easting);
	    return 0;
	  case COORD_UTM_GRID:
	    zone = *to_zone;
	    if (ret = coord_convert(COORD_GCC,
				    from_x, from_y, from_z,
				    COORD_UTM_NE,
				    &zone, &northing, &easting, &z))
	      return ret;
	    return coord_convert(COORD_UTM_NE,
				 zone, northing, easting, z,
				 COORD_UTM_GRID,
				 &zone, to_resolution, to_string, to_z);
	  case COORD_LATLON:
	    coord_gcc_to_llz(from_x, from_y, from_z,
			     COORD_GCC_DATUM, &latitude, &longitude, to_z);
	    if (to_local_datum)
	    {
		/* See if we needed a different datum */
		datum = coord_lookup_datum_by_ll(latitude, longitude);
		if (datum != COORD_GCC_DATUM)
		{
		    /* Translate again using the right datum */
		    coord_gcc_to_llz(from_x, from_y, from_z,
				     datum, &latitude, &longitude, to_z);
		}
	    }
	    *to_latitude = latitude;
	    *to_longitude = longitude;
	    return 0;
	  case COORD_CHARACTER_STRING:
	    sprintf(to_string,"%d, %d, %d",
		    ROUND(from_x),ROUND(from_y),ROUND(from_z));
	    return 0;
	}
	break;
      case COORD_TCC:
	switch(to_system)
	{
	  case COORD_GCC:
	    if (from_tcc->source == COORD_GCC)
	    {
		coord_tcc_to_gcc(from_tcc, from_x, from_y, from_z,
				 to_x, to_y, to_z);
		return 0;
	    }
	    else /* (to_tcc->source == COORD_UTM_NE) */
	    {
		zone = from_tcc->zone;
		if (ret = coord_convert(COORD_TCC,
					from_tcc, from_x, from_y, from_z,
					COORD_UTM_NE,
					&zone, &northing, &easting, &z))
		  return ret;
		return coord_convert(COORD_UTM_NE,
				     zone, northing, easting, z,
				     COORD_GCC,
				     to_x, to_y, to_z);
	    }
	  case COORD_TCC:
	    if (to_tcc == from_tcc)
	    {
		*to_x = from_x;
		*to_y = from_y;
		*to_z = from_z;
		return 0;
	    }
	    else if ((from_tcc->source == COORD_GCC) ||
		     (to_tcc->source == COORD_GCC))
	    {
		if (ret = coord_convert(COORD_TCC,
					from_tcc, from_x, from_y, from_z,
					COORD_GCC,
					&x, &y, &z))
		  return ret;
		return coord_convert(COORD_GCC,
				     x, y, z,
				     COORD_TCC,
				     to_tcc, to_x, to_y, to_z);
	    }
	    else /* (to_tcc->source == from_tcc->source == COORD_UTM_NE) */
	    {
		zone = 0;
		if (ret = coord_convert(COORD_TCC,
					from_tcc, from_x, from_y, from_z,
					COORD_UTM_NE,
					&zone, &northing, &easting, &z))
		  return ret;
		return coord_convert(COORD_UTM_NE,
				     zone, northing, easting, z,
				     COORD_TCC,
				     to_tcc, to_x, to_y, to_z);
	    }
	  case COORD_UTM_NE:
	    if (from_tcc->source == COORD_UTM_NE)
	    {
		*to_z = from_z;
		coord_tcc_to_utm(from_tcc, from_x, from_y,
				 to_zone, to_northing, to_easting);
		return 0;
	    }
	    else /* (to_tcc->source == COORD_GCC) */
	    {
		if (ret = coord_convert(COORD_TCC,
					from_tcc, from_x, from_y, from_z,
					COORD_GCC,
					&x, &y, &z))
		  return ret;
		return coord_convert(COORD_GCC,
				     x, y, z,
				     COORD_UTM_NE,
				     to_zone, to_northing, to_easting, to_z);
	    }
	  case COORD_UTM_GRID:
	    zone = *to_zone;
	    if (ret = coord_convert(COORD_TCC,
				    from_tcc, from_x, from_y, from_z,
				    COORD_UTM_NE,
				    &zone, &northing, &easting, &z))
	      return ret;
	    return coord_convert(COORD_UTM_NE,
				 zone, northing, easting, z,
				 COORD_UTM_GRID,
				 &zone, to_resolution, to_string, to_z);
	  case COORD_LATLON:
	    if (ret = coord_convert(COORD_TCC,
				    from_tcc, from_x, from_y, from_z,
				    COORD_GCC,
				    &x, &y, &z))
	      return ret;
	    return coord_convert(COORD_GCC,
				 x, y, z,
				 COORD_LATLON,
				 to_latitude, to_longitude, to_z,
				 to_local_datum);
	  case COORD_CHARACTER_STRING:
	    sprintf(to_string,"%d, %d, %d",
		    ROUND(from_x),ROUND(from_y),ROUND(from_z));
	    return 0;
	}
	break;
      case COORD_UTM_NE:
	switch(to_system)
	{
	  case COORD_GCC:
	    datum = coord_lookup_datum_by_ne(from_zone,
					     from_northing, from_easting);
	    coord_utm_to_ll(datum, from_northing, from_easting, from_zone,
			    &latitude, &longitude);
	    coord_llz_to_gcc(datum, latitude, longitude, from_z,
			     to_x, to_y, to_z);
	    return 0;
	  case COORD_TCC:
	    if (to_tcc->source == COORD_UTM_NE)
	    {
		*to_z = from_z;
		coord_utm_to_tcc(from_zone, from_northing, from_easting,
				 to_tcc, to_x, to_y);
		return 0;
	    }
	    else /* (to_system->source == COORD_GCC) */
	    {
		if (ret = coord_convert(COORD_UTM_NE,
					from_zone,
					from_northing, from_easting, from_z,
					COORD_GCC,
					&x, &y, &z))
		  return ret;
		return coord_convert(COORD_GCC,
				     x, y, z,
				     COORD_TCC,
				     to_tcc, to_x, to_y, to_z);
	    }
	  case COORD_UTM_NE:
	    *to_z = from_z;
	    coord_change_utm_zone(from_zone, from_northing, from_easting,
				  to_zone, to_northing, to_easting);
	    return 0;
	  case COORD_UTM_GRID:
	    *to_z = from_z;
	    coord_utm_to_grid(from_zone, from_northing, from_easting,
			      to_zone, to_resolution, to_string);
	    return 0;
	  case COORD_LATLON:
	    datum = coord_lookup_datum_by_ne(from_zone,
					     from_northing, from_easting);
	    coord_utm_to_ll(datum, from_northing, from_easting, from_zone,
			    &latitude, &longitude);
	    if ((datum != COORD_GCC_DATUM) && !to_local_datum)
	    {
		coord_llz_to_gcc(datum, latitude, longitude, from_z,
				 &x, &y, &z);
		coord_gcc_to_llz(x, y, z,
				 COORD_GCC_DATUM, to_latitude, to_longitude,
				 to_z);
	    }
	    else
	    {
		*to_latitude = latitude;
		*to_longitude = longitude;
		*to_z = from_z;
	    }
	    return 0;
	  case COORD_CHARACTER_STRING:
	    sprintf(to_string,"N%d E%d [%d] ^%d",
		    ROUND(from_northing),ROUND(from_easting),from_zone,
		    ROUND(from_z));
	    return 0;
	}
	break;
      case COORD_UTM_GRID:
	switch(to_system)
	{
	  case COORD_GCC:
	    zone = from_zone;
	    if (ret = coord_convert(COORD_UTM_GRID,
				    from_zone, from_resolution, from_string,
				    from_z,
				    COORD_UTM_NE,
				    &zone, &northing, &easting, &z))
	      return ret;
	    return coord_convert(COORD_UTM_NE,
				 zone, northing, easting, z,
				 COORD_GCC,
				 to_x, to_y, to_z);
	  case COORD_TCC:
	    zone = to_tcc->zone;
	    if (ret = coord_convert(COORD_UTM_GRID,
				    from_zone, from_resolution, from_string,
				    from_z,
				    COORD_UTM_NE,
				    &zone, &northing, &easting, &z))
	      return ret;
	    return coord_convert(COORD_UTM_NE,
				 zone, northing, easting, z,
				 COORD_TCC,
				 to_tcc, to_x, to_y, to_z);
	  case COORD_UTM_NE:
	    *to_z = from_z;
	    return coord_grid_to_utm(from_zone, from_string,
				     to_zone, to_northing, to_easting);
	  case COORD_UTM_GRID:
	    *to_z = from_z;
	    strcpy(to_string, from_string);
	    return 0;
	  case COORD_LATLON:
	    zone = from_zone;
	    if (ret = coord_convert(COORD_UTM_GRID,
				    from_zone, from_resolution, from_string,
				    from_z,
				    COORD_UTM_NE,
				    &zone, &northing, &easting, &z))
	      return ret;
	    return coord_convert(COORD_UTM_NE,
				 zone, northing, easting, z,
				 COORD_LATLON,
				 to_latitude, to_longitude, to_z,
				 to_local_datum);
	  case COORD_CHARACTER_STRING:
	    strcpy(to_string, from_string);
	    return 0;
	}
	break;
      case COORD_LATLON:
	switch(to_system)
	{
	  case COORD_GCC:
	    if (from_local_datum)
	      datum = coord_lookup_datum_by_ll(from_latitude, from_longitude);
	    else
	      datum = COORD_GCC_DATUM;
	    coord_llz_to_gcc(datum, from_latitude, from_longitude, from_z,
			     to_x, to_y, to_z);
	    return 0;
	  case COORD_TCC:
	    if (ret = coord_convert(COORD_LATLON,
				    from_latitude, from_longitude, from_z,
				    from_local_datum,
				    COORD_GCC,
				    &x, &y, &z))
	      return ret;
	    return coord_convert(COORD_GCC,
				 x, y, z,
				 COORD_TCC,
				 to_tcc, to_x, to_y, to_z);
	  case COORD_UTM_NE:
	    *to_z = from_z;

	    datum = coord_lookup_datum_by_ll(from_latitude, from_longitude);
	    if ((datum != COORD_GCC_DATUM) && !from_local_datum)
	    {
		/* Translate the lat/long from the GCC datum to the
		 * local datum.
		 */
		coord_llz_to_gcc(COORD_GCC_DATUM,
				 from_latitude, from_longitude, from_z,
				 &x, &y, &z);
		coord_gcc_to_llz(x, y, z,
				 datum, &latitude, &longitude, to_z);
	    }
	    else
	    {
		latitude = from_latitude;
		longitude = from_longitude;
	    }
	    coord_ll_to_utm(datum, latitude, longitude,
			    to_zone, to_northing, to_easting);
	    return 0;
	  case COORD_UTM_GRID:
	    if (ret = coord_convert(COORD_LATLON,
				    from_latitude, from_longitude, from_z,
				    from_local_datum,
				    COORD_UTM_NE,
				    to_zone, &northing, &easting, &z))
	      return ret;
	    return coord_convert(COORD_UTM_NE,
				 *to_zone, northing, easting, z,
				 COORD_UTM_GRID,
				 to_zone, to_resolution, to_string, to_z);
	  case COORD_LATLON:
	    if (from_local_datum == to_local_datum)
	    {
		*to_latitude = from_latitude;
		*to_longitude = from_longitude;
		*to_z = from_z;
		return 0;
	    }
	    else
	    {
		if (from_local_datum)
		  datum = coord_lookup_datum_by_ll(from_latitude,
						   from_longitude);
		else
		  datum = COORD_GCC_DATUM;

		coord_llz_to_gcc(datum, from_latitude, from_longitude, from_z,
				 &x, &y, &z);

		if (to_local_datum)
		  datum = coord_lookup_datum_by_ll(from_latitude,
						   from_longitude);
		else
		  datum = COORD_GCC_DATUM;

		coord_gcc_to_llz(x, y, z,
				 datum, to_latitude, to_longitude, to_z);
				 
		return 0;
	    }
	  case COORD_CHARACTER_STRING:
	    sprintf(to_string, "%s ^%d",
		    coord_format_latlon(from_latitude, from_longitude),
		    ROUND(from_z));
	    return 0;
	}
	break;
    }

    /* If we got here, one of the switches must have fallen through */
    return COORD_SYSTEM_UNKNOWN;
}

char *coord_error(diag)
    int32 diag;
{
    switch (diag)
    {
      case 0:				return "No Error";
      case COORD_SYNTAX:		return "UTM Syntax Error";
      case COORD_TOO_LONG:		return "UTM Too Long";
      case COORD_ZONE_UNKNOWN:		return "UTM in Unrecognized Zone";
      case COORD_GRID_UNKNOWN:		return "UTM Grid not Found in Zone";
      case COORD_BAD_TCC:		return "Bad TCC pointer";
      case COORD_SYSTEM_UNKNOWN:	return "Unknown Coordinate System";
      default:				return "Unknown Error";
    }
}
