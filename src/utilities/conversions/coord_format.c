#ifndef lint
static char rcsid [] = "$RCSfile: coord_format.c,v $ $Revision: 1.5 $ $State: Exp $";
#endif
/****************************************************************************
*   File: coord_format.c                                                    *
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
*       Contents: Character formatting utilities                            *
*       Created: Wed Sep  9 1992                                            *
*       Author: jesmith                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include <ctype.h>
#include "libcoord_local.h"

static char *dmshh_format();

char *coord_format_latlon(latitude, longitude)
    float64 latitude;
    float64 longitude;
{
    static char buf[1024];

    sprintf(buf,"%s%c %s%c",
	    dmshh_format(latitude),
	    latitude > 0 ? 'N' : 'S',
	    dmshh_format(longitude),
	    longitude > 0 ? 'E' : 'W');
    return buf;
}

int32 coord_fixed_point_degrees(degrees)
    float64 degrees;
{
    int32 deg_part;
    int32 min_part;
    float64 sec_part;
    int32 int_secs;
    int32 sgn = 1;
    
    if (degrees < 0)
      degrees = -degrees, sgn = -1;

    deg_part = degrees;
    min_part = 60.0 * (degrees - deg_part);
    sec_part = 3600.0 * (degrees - deg_part - min_part / 60.0);

    /* Round off hundredths */
    int_secs = sec_part * 100 + 0.5;
    if (int_secs >= 6000)
      int_secs -= 6000, min_part += 1;
    if (min_part >= 60)
      min_part -= 60, deg_part += 1;

    return (deg_part * 1000000 + min_part * 10000 + int_secs) * sgn;
}

float64 coord_floating_point_degrees(ddmmsshh)
    int32 ddmmsshh;
{
    int32 deg_part;
    int32 min_part;
    float64 sec_part;
    int32 sgn = 1;

    if (ddmmsshh < 0)
      ddmmsshh = -ddmmsshh, sgn = -1;

    deg_part = ddmmsshh / 1000000;
    min_part = (ddmmsshh - (deg_part * 1000000)) / 10000;
    sec_part = (ddmmsshh - (deg_part * 1000000) - (min_part * 10000)) / 100;

    return (deg_part + min_part / 60.0 + sec_part / 3600.0) * sgn;
}

int32 coord_parse_fixed_point_degrees(string)
    char *string;
{
    char *s;
    int32 ret[2], sgn=1, part=0;

    ret[0] = 0; /* ddmmss part */
    ret[1] = 0; /* hh part */

    for(s=string;*s;s++)
    {
	if (*s == 'S' ||
	    *s == 's' ||
	    *s == 'W' ||
	    *s == 'w')
	  sgn = -1;
	else if (*s == '.')
	  part = 1;
	else if (isdigit(*s))
	  ret[part] = ret[part] * 10 + (*s - '0');
	/* Ignore other chars */
    }

    /* Fill in missing digits in ddmmss part */
    if (ret[0] < 100)
      ret[0] *= 100;
    if (ret[0] < 10000)
      ret[0] *= 100;

    /* Throw away > 1/100 accuracy */
    while (ret[1] > 100)
      ret[1] /= 10;

    return (ret[0] * 100 + ret[1]) * sgn;
}

static char *dmshh_format(degrees)
    float64 degrees;
{
    static char buf[1024];
    int32 deg_part;
    int32 min_part;
    float64 sec_part;
    static int32 bufcnt = 0;
    char *this_buf = buf+bufcnt;
    
    if (degrees < 0)
      degrees = -degrees;

    deg_part = degrees;
    min_part = 60.0 * (degrees - deg_part);
    sec_part = 3600.0 * (degrees - deg_part - min_part / 60.0);

    /* Round off hundredths */
    if (sec_part + 0.005 >= 60.0)
      sec_part -= 60.0, min_part += 1;
    if (min_part >= 60)
      min_part -= 60, deg_part += 1;

    sprintf(this_buf,"%02d*%02d\'%05.2f\"",deg_part,min_part,sec_part);

    /* This allows up to 64 calls to this routine in 1 function invocation */
    bufcnt = (bufcnt + 16) % 1024;

    return this_buf;
}
