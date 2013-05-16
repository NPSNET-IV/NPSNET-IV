/****************************************************************************
*   File: coord_grid.c                                                      *
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
*       Contents: Code to generate grid vertices                            *
*       Created: Fri Mar  1 1990                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.7 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libcoord_local.h"
#include <stdext.h> /*common/include/global*/
#include <math.h>

#define ROUND_UP_TO_SPACING(x,s)   (ceil((x)/(s)) * (s))
#define ROUND_DOWN_TO_SPACING(x,s) (floor((x)/(s)) * (s))
#define ROUND(x) ((int32)((x)+0.5))

#define MAX_PTS_PER_LINE 1000

/* Numbers used to scale meters to digits */
static digit_scale[] = { 100000 , 10000 , 1000 , 100 , 10 , 1 };

#define IS_BOLD(x,spacing) (!((int32)((x)+0.5) % (10*spacing)))
#define IS_ROUNDED(x,spacing) (!((int32)((x)+0.5) % (spacing)))

void coord_generate_grid(tcc,
			 min_x, max_x, min_y, max_y, spacing, digits,
			 label_mask,
			 max_thin_segments, max_thick_segments, max_labels,
			 n_thin_segments, thin_segments,
			 n_thick_segments, thick_segments,
			 n_labels, labels)
    COORD_TCC_PTR     tcc;
    int32             min_x;
    int32             max_x;
    int32             min_y;
    int32             max_y;
    int32             spacing;
    int32             digits;
    uint32            label_mask;
    int32             max_thin_segments;
    int32             max_thick_segments;
    int32             max_labels;
    int32            *n_thin_segments;
    float32           thin_segments[];
    int32            *n_thick_segments;
    float32           thick_segments[];
    int32            *n_labels;
    COORD_GRID_LABEL  labels[];
{
    float64  min_lng, max_lng, min_lat, max_lat;
    float64  lng0, lng1, mid_lng;
    float64  lng, lat;
    int32    min_zone, max_zone, zone, munged;
    float64  min_northing, max_northing, northing;
    float64  min_easting, max_easting, easting, mid_easting;
    int32    reasting; /* Rounded easting */
    float64  latitude, ignored;
    int32    thin_seg_cnt = 0;
    int32    thick_seg_cnt = 0;
    int32    lbl_cnt = 0;
    int32    lbl_num;
    float64  x, y, last_x, last_y, z;
    char     format_string[32];
    struct
    {
	int32 easting;
	float32 x, y;
    } prev_line[MAX_PTS_PER_LINE], this_line[MAX_PTS_PER_LINE];
    int32 ecnt, p_ecnt, pm_ecnt;

    sprintf(format_string,"%%0%dd",digits);

/* code to record a northing/easting label */
#define LABEL_NE(zone,e0,n0,num)				\
    {								\
	if (lbl_cnt < max_labels)				\
	{							\
	    coord_convert(COORD_UTM_NE,zone,n0,e0,0.0,		\
			  COORD_TCC,tcc,&x,&y,&z);		\
	    labels[lbl_cnt].x = x;				\
	    labels[lbl_cnt].y = y;				\
	    sprintf(labels[lbl_cnt].txt,format_string, num);	\
            lbl_cnt++;						\
	}							\
    }

/* code to record a longitude label */
#define LABEL_LN(xp,yp,num)						\
    {									\
	if (lbl_cnt < max_labels)					\
	{								\
	    labels[lbl_cnt].x = xp;					\
	    labels[lbl_cnt].y = yp;					\
	    if (num < 0)						\
	      sprintf(labels[lbl_cnt].txt,"%dW", -num);			\
	    else							\
	      sprintf(labels[lbl_cnt].txt,"%dE", num);			\
            lbl_cnt++;							\
	}								\
    }

    /* First figure out which grid zones these points cover.
     * This code assumes larger grid zones have larger longitude.
     * I don't know for sure that this assumption works everywhere,
     * but it does work in the places I'm interested in.
     */
    if (coord_convert(COORD_TCC, tcc, (float64)min_x,(double)min_y,0.0,
		      COORD_LATLON, &min_lat, &min_lng, &z, TRUE) ||
	coord_convert(COORD_TCC, tcc, (float64)max_x,(double)max_y,0.0,
		      COORD_LATLON, &max_lat, &max_lng, &z, TRUE))
    {
	*n_thin_segments = 0;
	*n_thick_segments = 0;
	*n_labels = 0;
	return;
    }

    min_zone = COORD_LATLONG_TO_GRID_ZONE(min_lat, min_lng);
    max_zone = COORD_LATLONG_TO_GRID_ZONE(max_lat, max_lng);

    /* Within each zone, generate a grid pattern */
    for (zone = min_zone; zone <= max_zone; zone++)
    {
	/* Find the min and max longitude for this zone. */

	lng0 = COORD_WEST_LONG_OF_GRID_ZONE(zone);
	lng1 = COORD_EAST_LONG_OF_GRID_ZONE(zone);

	/* Label the longitudes */
	for (lng=lng0;lng<lng1;lng+=1.0)
	  if ((lng >= min_lng) && (lng < max_lng))
	  {
	      if (!coord_convert(COORD_LATLON, min_lat, lng, 0.0, TRUE,
				 COORD_TCC, tcc, &last_x, &last_y, &z) &&
		  !coord_convert(COORD_LATLON, max_lat, lng, 0.0, TRUE,
				 COORD_TCC, tcc, &x, &y, &z))
	      {
		  thick_segments[thick_seg_cnt] = last_x;
		  thick_segments[thick_seg_cnt+1] = last_y;
		  thick_segments[thick_seg_cnt+2] = x;
		  thick_segments[thick_seg_cnt+3] = y;
		  thick_seg_cnt += 4;
		  LABEL_LN(x-(x-last_x)/8,y-(y-last_y)/8,(int32)lng);
		  LABEL_LN(last_x+(x-last_x)/8,last_y+(y-last_y)/8,(int32)lng);
	      }
	  }
	    
	if (lng0 < min_lng)
	  lng0 = min_lng;
	if (lng1 > max_lng)
	  lng1 = max_lng;

	/* Northing varies slightly by longitude.  To find the min & max
	 * northing for the area we're drawing, we use the mean longitude.
	 */

	mid_lng = (lng1 + lng0)/2.0;

	/* Find the upper and lower northing bounds. */
	munged = zone;
	coord_convert(COORD_LATLON, min_lat, mid_lng, 0.0, TRUE,
		      COORD_UTM_NE,
		      &munged, &min_northing, &mid_easting, &z);

	munged = zone;
	coord_convert(COORD_LATLON, max_lat, mid_lng, 0.0, TRUE,
		      COORD_UTM_NE,
		      &munged, &max_northing, &mid_easting, &z);

	pm_ecnt = 0;

	/* Figure out the horizontal lines.  Northing starts at min_northing
	 * then proceeds north at even steps (depending upon spacing) and
	 * finally stops at max_northing.  Horizontal lines are drawn
	 * everywhere except the min & max.
	 */
	northing = min_northing;
	for(;;)
	{
	    /* Find the latitude of this northing */
	    coord_convert(COORD_UTM_NE,
			  zone, northing, mid_easting, 0.0,
			  COORD_LATLON,
			  &latitude, &ignored, &z, TRUE);

	    /* Find the left & right easting bounds */
	    munged = zone;
	    coord_convert(COORD_LATLON, latitude, lng0, 0.0, TRUE,
			  COORD_UTM_NE,
			  &munged, &ignored, &min_easting, &z);

	    munged = zone;
	    coord_convert(COORD_LATLON, latitude, lng1, 0.0, TRUE,
			  COORD_UTM_NE,
			  &munged, &ignored, &max_easting, &z);

	    coord_convert(COORD_UTM_NE,zone,northing, min_easting, 0.0,
			  COORD_TCC,tcc,&last_x,&last_y, &z);

	    /* Easting proceeds from left to right the same way northing
	     * goes from bottom to top (see comment above).
	     */
	    easting = ROUND_UP_TO_SPACING(min_easting, spacing);
	    if (easting > max_easting)
	      easting = max_easting;
	    for (ecnt=0; ecnt < MAX_PTS_PER_LINE; ecnt++)
	    {
		/* Make sure there is room */
		if ((thin_seg_cnt >= (max_thin_segments-1)*4) ||
		    (thick_seg_cnt >= (max_thick_segments-1)*4))
		  break;

		coord_convert(COORD_UTM_NE,zone,northing,easting,0.0,
			      COORD_TCC,tcc,&x,&y,&z);

		/* Label verticals at the top and bottom. */
		if (((northing == min_northing) &&
		     (label_mask & COORD_LABEL_BOTTOM)) ||
		    ((northing == max_northing) &&
		     (label_mask & COORD_LABEL_TOP)))
		{
		    if ((lbl_cnt < max_labels) &&
			(easting < max_easting))
		    {
			lbl_num = ((int32)easting % 100000) /
			  digit_scale[digits];

			labels[lbl_cnt].x = x;
			labels[lbl_cnt].y = y;
			sprintf(labels[lbl_cnt].txt,format_string, lbl_num);
			lbl_cnt++;
		    }
		}

		/* Generate a horizontal segment everywhere else. */
		if (IS_ROUNDED(northing,spacing))
		{
		    if (IS_BOLD(northing,spacing))
		    {
			thick_segments[thick_seg_cnt] = last_x;
			thick_segments[thick_seg_cnt+1] = last_y;
			thick_segments[thick_seg_cnt+2] = x;
			thick_segments[thick_seg_cnt+3] = y;
			thick_seg_cnt += 4;
		    }
		    else
		    {
			thin_segments[thin_seg_cnt] = last_x;
			thin_segments[thin_seg_cnt+1] = last_y;
			thin_segments[thin_seg_cnt+2] = x;
			thin_segments[thin_seg_cnt+3] = y;
			thin_seg_cnt += 4;
		    }
		}

		last_x = x;
		last_y = y;

		/* Try to generate a vertical segment. */

		if (IS_ROUNDED(easting,spacing))
		{
		    /* In most cases the same easting on the previous
		     * line will have the same ecnt.  If not, search back
		     * and then forward to see if you can find the match.
		     */
		    reasting = ROUND(easting);
		    for (p_ecnt = ecnt;
			 (p_ecnt >= 0) && (p_ecnt < pm_ecnt) &&
			 (prev_line[p_ecnt].easting != reasting);
			 p_ecnt--);
		    if (prev_line[p_ecnt].easting != reasting)
		      for (p_ecnt = ecnt+1;
			   (p_ecnt < pm_ecnt) &&
			   (prev_line[p_ecnt].easting != reasting);
			   p_ecnt++);

		    /* If a match was found, generate the vertical,
		     * otherwise, just skip this one.
		     */
		    if (prev_line[p_ecnt].easting == reasting)
		    {
			if (IS_BOLD(easting,spacing))
			{
			    thick_segments[thick_seg_cnt] = x;
			    thick_segments[thick_seg_cnt+1] = y;
			    thick_segments[thick_seg_cnt+2] =
			      prev_line[p_ecnt].x;
			    thick_segments[thick_seg_cnt+3] =
			      prev_line[p_ecnt].y;
			    thick_seg_cnt += 4;
			}
			else
			{
			    thin_segments[thin_seg_cnt] = x;
			    thin_segments[thin_seg_cnt+1] = y;
			    thin_segments[thin_seg_cnt+2] =
			      prev_line[p_ecnt].x;
			    thin_segments[thin_seg_cnt+3] =
			      prev_line[p_ecnt].y;
			    thin_seg_cnt += 4;
			}
		    }
		}

		this_line[ecnt].easting = reasting;
		this_line[ecnt].x = x;
		this_line[ecnt].y = y;

		if (easting == max_easting)
		  break;

		easting += spacing;
		if (easting > max_easting)
		  easting = max_easting;
	    }
	    /* Remember the points we generated this pass, so the
	     * next pass can use them.
	     */
	    pm_ecnt = ecnt;
	    bcopy(this_line, prev_line, (int)(pm_ecnt*sizeof(this_line[0])));

	    /* Label horizontals only at far left/right.  Also, don't
	     * label top and bottom, since that's done when the verticals
	     * are labeled.
	     */
	    if (((northing != min_northing) ||
		 !(label_mask & COORD_LABEL_BOTTOM)) &&
		((northing != max_northing) ||
		 !(label_mask & COORD_LABEL_TOP)))
	    {
		lbl_num = ((int32)northing % 100000) / digit_scale[digits];
		if ((zone == min_zone) && (label_mask & COORD_LABEL_LEFT))
		  LABEL_NE(zone, min_easting, northing, lbl_num);
		if ((zone == max_zone) && (label_mask & COORD_LABEL_RIGHT))
		  LABEL_NE(zone, max_easting, northing, lbl_num);
	    }

	    if (northing == max_northing)
	      break;

	    if ((northing == min_northing) &&
		(fmod(northing, (float64)spacing) > 0.1))
	      northing = ROUND_UP_TO_SPACING(northing, spacing);
	    else
	      northing += spacing;

	    if (northing > max_northing)
	      northing = max_northing;
	}
    }

    *n_thin_segments = thin_seg_cnt/4;
    *n_thick_segments = thick_seg_cnt/4;
    *n_labels = lbl_cnt;
}
