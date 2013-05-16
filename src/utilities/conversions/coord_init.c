/****************************************************************************
*   File: coord_init.c                                                      *
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
*       Contents: Code to convert from utm to milgrid based on lookup tables*
*       Created: Thu Feb 28 1990                                            *
*       Author: jesmith                                                     *
*      $Revision: 1.6 $                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include <ctype.h>
#include "libcoord_local.h"
#include <libreader.h>

/* DISCLAIMER: Code will need to be modified to work in the southern
 *             hemisphere!
 */

volatile int G_coord_error;

static COORD_DATUM_TABLE *datum_table = 0;
static COORD_UTM_TABLE *utm_table = 0;

static void check_datums();
static void check_zones();

int32 coord_init(data_path, flags)
    char   data_path[];
    uint32 flags;
{
    READER_UNION table;
    int32 ret = reader_read("coord.rdr", data_path, &table,
			    flags & ~READER_TYPING);

    if (ret != 0)
      return ret;

    if (!(datum_table = (COORD_DATUM_TABLE *)
	  reader_find_tag(reader_get_symbol("MOLODENSKIY"), table.array,
			  READER_UNTAGGED, READER_ERRORS_WITH_CONTEXT)))
    {
	printf("libcoord: Unable to find Molodenskiy entries in table.\n");
	return READER_READ_ERROR;
    }

    check_datums(NUM_DATUMS(*datum_table), datum_table->datums);

    if (!(utm_table = (COORD_UTM_TABLE *)
	  reader_find_tag(reader_get_symbol("UTM"), table.array,
			  READER_UNTAGGED, READER_ERRORS_WITH_CONTEXT)))
    {
	printf("libcoord: Unable to find UTM entries in table.\n");
	return READER_READ_ERROR;
    }

    check_zones(NUM_ZONES(*utm_table), utm_table->zones);

    return 0;
}

static void check_datums(n_datums, datums)
    uint32 n_datums;
    COORD_MOLODENSKIY_XFORM *datums[];
{
    int32 i;

    for (i=0;i<n_datums;i++)
    {
	if (datums[i]->length !=
	    (sizeof(COORD_MOLODENSKIY_XFORM)/sizeof(int32)))
	{
	    printf("Datum %d: Molodenskiy transform data incorrect (length = %d)\n",
		   i+1, datums[i]->length);
	}
	else if (datums[i]->datum != i)
	{
	    printf("Datum %d: Out of order\n", i+1);
	}
    }
}
	
static void check_zones(n_zones, zones)
    uint32             n_zones;
    COORD_ZONE_TABLE  *zones[];
{
    int32 i,j;
    COORD_GRID_BOX *box;
    int32 zone;

    for (i=0;i<n_zones;i++)
    {
	for (j=0;j<NUM_BOXES(*zones[i]);j++)
	{
	    box = zones[i]->boxes[j];
	    zone = zones[i]->zone_number;
	    /* Right number of entries? */
	    if (box->always_12 != 12)
	      printf("Zone %d: Box %d is corrupt (always_12 != 12)\n",
		     zone, j+1);
	    /* Is the datum one we've heard of? */
	    else if ((box->datum < 0) ||
		     (box->datum >= NUM_DATUMS(*datum_table)))
	      printf("Zone %d: Box %d is corrupt (unknown datum)\n",
		     zone, j+1);
	    /* Are the mins < maxs? */
	    else if ((box->min_northing > box->max_northing) ||
		     (box->min_easting > box->max_easting) ||
		     (box->min_latitude > box->max_latitude) ||
		     (box->min_longitude > box->max_longitude))
	      printf("Zone %d: Box %d is corrupt (min > max)\n",
		     zone, j+1);
	    /* Do northing letters match box height? */
	    else if (((box->max_northing-1)/100000 -
		      box->min_northing/100000 + 1) !=
		     NUM_LETTERS(*box->n_letters))
	      printf("Zone %d: Box %d is corrupt (northing letters)\n",
		     zone, j+1);
	    /* Do easting letters match box height? */
	    else if (((box->max_easting-1)/100000 -
		      box->min_easting/100000 + 1) !=
		     NUM_LETTERS(*box->e_letters))
	      printf("Zone %d: Box %d is corrupt (easting letters)\n",
		     zone, j+1);
	}
    }
}

float64 coord_get_major_axis(datum)
    int32 datum;
{
    return datum_table->datums[datum]->major_axis;
}

float64 coord_get_minor_axis(datum)
    int32 datum;
{
    return datum_table->datums[datum]->minor_axis;
}

void coord_get_datum_shifts(datum, dx, dy, dz)
    int32 datum;
    float64 *dx, *dy, *dz;
{
    *dx = datum_table->datums[datum]->dx;
    *dy = datum_table->datums[datum]->dy;
    *dz = datum_table->datums[datum]->dz;
}

static COORD_ZONE_TABLE *find_zone(zone)
    int32 zone;
{
    int32 i;
    COORD_ZONE_TABLE *zt;

    for(i=0;i<NUM_ZONES(*utm_table);i++)
    {
	zt = utm_table->zones[i];
	if (zt->zone_number == zone)
	  return zt;
    }
    return 0;
}

int32 coord_lookup_datum_by_ne(zone, northing, easting)
    int32 zone;
    float64 northing;
    float64 easting;
{
    COORD_ZONE_TABLE *zt;
    int32 i;
    static int32 zwarned=0, dwarned=0;

    /* First find the zone */
    if (!(zt = find_zone(zone)))
    {
	if (!zwarned)
	{
	    printf("LIBCOORDINATES WARNING: Zone %d not found\n",zone);
	    zwarned = 1;
	}
        G_coord_error = 1;
	return COORD_GCC_DATUM;
    }

    /* Then find the box into which this n/e fits */
    for (i=0;i<NUM_BOXES(*zt);i++)
    {
	if ((zt->boxes[i]->min_northing <= northing) &&
	    (zt->boxes[i]->max_northing > northing) &&
	    (zt->boxes[i]->min_easting <= easting) &&
	    (zt->boxes[i]->max_easting > easting))
	  return zt->boxes[i]->datum;
    }

    if (!dwarned)
    {
	printf("LIBCOORDINATES WARNING: Datum not found for %fN %fE Z%d\n",
	       northing,easting,zone);
	dwarned = 1;
    }

    G_coord_error = 1;
    return COORD_GCC_DATUM;
}

int32 coord_lookup_datum_by_ll(latitude, longitude)
    float64 latitude;
    float64 longitude;
{
    COORD_ZONE_TABLE *zt;
    int32 i;
    int32 zone = COORD_LATLONG_TO_GRID_ZONE(latitude, longitude);
    static int32 zwarned=0, dwarned=0;

    /* First find the zone */
    if (!(zt = find_zone(zone)))
    {
	if (!zwarned)
	{
	    printf("LIBCOORDINATES WARNING: Zone %d not found\n",zone);
	    zwarned = 1;
	}

        G_coord_error = 1;
	return COORD_GCC_DATUM;
    }

    /* Then find the box into which this lat/long fits */
    for (i=0;i<NUM_BOXES(*zt);i++)
    {
	if ((zt->boxes[i]->min_latitude <= latitude) &&
	    (zt->boxes[i]->max_latitude > latitude) &&
	    (zt->boxes[i]->min_longitude <= longitude) &&
	    (zt->boxes[i]->max_longitude > longitude))
	  return zt->boxes[i]->datum;
    }

    if (!dwarned)
    {
	printf("LIBCOORDINATES WARNING: Datum not found for %s\n",
	       coord_format_latlon(latitude, longitude));
	dwarned = 1;
    }

    G_coord_error = 1;
    return COORD_GCC_DATUM;
}

char coord_lookup_zone_letter(zone, northing)
    int32    zone;
    float64 northing;
{
    int32 i;
    COORD_ZONE_TABLE *zt;

    if (zt = find_zone(zone))
    {
	for (i=0;i<NUM_RANGES(*zt->z_letters);i++)
	{
	    if ((zt->z_letters->ranges[i].min_northing <= northing) &&
		(zt->z_letters->ranges[i].max_northing > northing))
	    {
		return *(zt->z_letters->ranges[i].letter);
	    }
	}
    }
    return '?';
}

/* Numbers used to scale northing/easting to utm resolutions */
static res_denoms[] = { 100000 , 10000 , 1000 , 100 , 10 , 1 };

void coord_lookup_utm(zone, northing, easting, resolution, string)
    int32    zone;
    float64 northing;
    float64 easting;
    int32    resolution;
    char  *string;
{
    int32 i;
    char letter_part[3];
    COORD_ZONE_TABLE *zt;
    int32 n_index, e_index;
    int32 n_num, e_num;
    char format_string[32];

    if (easting < 0 || easting > 1000000)
    {
	string[0] = 0;
	return;
    }

    letter_part[0] = '?';
    letter_part[1] = '?';
    letter_part[2] = '?';

    /* First find this zone in the table */
    if (zt = find_zone(zone))
    {
	/* Find the letter for this 8* chunk */
	for (i=0;i<NUM_RANGES(*zt->z_letters);i++)
	{
	    if ((zt->z_letters->ranges[i].min_northing <= northing) &&
		(zt->z_letters->ranges[i].max_northing > northing))
	    {
		letter_part[0] = *(zt->z_letters->ranges[i].letter);
		break;
	    }
	}

	/* Then find the box into which this n/e fits */
	for (i=0;i<NUM_BOXES(*zt);i++)
	{
	    if ((zt->boxes[i]->min_northing <= northing) &&
		(zt->boxes[i]->max_northing > northing) &&
		(zt->boxes[i]->min_easting <= easting) &&
		(zt->boxes[i]->max_easting > easting))
	    {
		e_index = easting/100000 - zt->boxes[i]->min_easting/100000;
		n_index = northing/100000 - zt->boxes[i]->min_northing/100000;
		if (n_index < 0)
		  n_index = -n_index;

		letter_part[1] =
		  *(zt->boxes[i]->e_letters->letters[e_index]);
		letter_part[2] =
		  *(zt->boxes[i]->n_letters->letters[n_index]);
		break;
	    }
	}
    }

    if (resolution > 5)
      resolution = 5;
    if (resolution < 1)
      resolution = 1;

    n_num = (((int32) northing) % 100000) / res_denoms[resolution];
    e_num = (((int32) easting) % 100000) / res_denoms[resolution];

    /* generate someting like "%02d%c%c%c%0{res}d%0{res}d" */
    sprintf(format_string,"%%02d%%c%%c%%c%%0%dd%%0%dd",resolution,resolution);

    sprintf(string,format_string,
	    zone, letter_part[0], letter_part[1], letter_part[2],
	    e_num,n_num);
}

static int32 reverse_lookup_utm(zone, zone_letter,
				e_char, n_char, e_num, n_num,
				northing, easting)
    int32     zone;
    char      zone_letter;
    char      e_char;
    char      n_char;
    int32     e_num;
    int32     n_num;
    float64  *northing;
    float64  *easting;
{
    COORD_ZONE_TABLE *zt;
    float64 min_northing;
    float64 max_northing;
    int32 min_index, max_index, n_index, e_index;
    int32 i, j;

    /* First find this zone in the lookup table */
    if (!(zt = find_zone(zone)))
      return COORD_ZONE_UNKNOWN;

    /* Bound the northing via the zone letter */
    min_northing = 0.0;
    max_northing = 0.0;
    for (i=0;i<NUM_RANGES(*zt->z_letters);i++)
    {
	if (zone_letter == *(zt->z_letters->ranges[i].letter))
	{
	    min_northing = zt->z_letters->ranges[i].min_northing;
	    max_northing = zt->z_letters->ranges[i].max_northing;
	    break;
	}
    }
    if (!min_northing && !max_northing)
      return COORD_ZONE_UNKNOWN;

    /* The same utm northing letter pair can appear many times
     * within a zone.  Hence, we use the min/max northing to bound
     * the search within the list of northing letters.  We search
     * each box until we find one which has both a matching northing
     * and a matching easting.
     */
    for (i=0;i<NUM_BOXES(*zt);i++)
    {
	/* Skip boxes not in this letter zone */
	if ((zt->boxes[i]->max_northing < min_northing) ||
	    (zt->boxes[i]->min_northing > max_northing))
	  continue;

	min_index = min_northing/100000 - zt->boxes[i]->min_northing/100000;
	if (min_index < 0)
	  min_index = 0;

	max_index =
	  max_northing/100000 - zt->boxes[i]->min_northing/100000 + 1;
	if (max_index > NUM_LETTERS(*zt->boxes[i]->n_letters))
	  max_index = NUM_LETTERS(*zt->boxes[i]->n_letters);

	/* If I can find a northing letter in this range we're golden */
	n_index = -1;
	for (j=min_index;j<max_index;j++)
	{
	    if (*(zt->boxes[i]->n_letters->letters[j]) == n_char)
	    {
		n_index = j;
		break;
	    }
	}
	if (n_index == -1)
	  continue;

	/* I found a northing letter, now lets look for our easting letter */
	e_index = -1;
	for (j=0;j<NUM_LETTERS(*zt->boxes[i]->e_letters);j++)
	{
	    if (*(zt->boxes[i]->e_letters->letters[j]) == e_char)
	    {
		e_index = j;
		break;
	    }
	}
	if (e_index == -1)
	  continue;

	/* This box has a letter pair we like.  Now compute the southwest
	 * northing and easting and add them to our offsets.
	 */

	*northing = n_num +
	  ((zt->boxes[i]->min_northing / 100000) + n_index) * 100000;
	*easting = e_num +
	  ((zt->boxes[i]->min_easting / 100000) + e_index) * 100000;

	return 0; /* Success */
    }

    return COORD_GRID_UNKNOWN;
}

/* Numbers used to scale utm to meters */
static utm_scale[] = { 100000 , 10000 , 1000 , 100 , 10 , 1 };

int32 coord_parse_utm(default_zone, default_zl, string,
		      zone, northing, easting)
    int32     default_zone;
    char      default_zl;
    char     *string;
    int32    *zone;
    float64  *northing;
    float64  *easting;
{
    int32 len = strlen(string);
    char control[20];
    char e_char, n_char, z_char;
    int32 e_num, n_num;
    int32 i;

    /* Odd length utms must have zones designated */
    if ((len % 2) != 0)
    {
	if (isdigit(string[0]) &&
	    isdigit(string[1]) &&
	    isalpha(string[2]))
	{
	    *zone = 10*(string[0] - '0') + (string[1] - '0');
	    z_char = string[2];
	    string += 3;
	    len -= 3;
	}
	else
	  return COORD_SYNTAX;
    }
    else
    {
	*zone = default_zone;
	z_char = default_zl;
    }
     
    /* Is it too long? */
    if (len > 12)
      return COORD_TOO_LONG;

    /* Are the first two characters letters? */
    if ((!isalpha(string[0])) &&
	(!isalpha(string[1])))
      return COORD_SYNTAX;
      
    /* Are the rest of the characters numbers? */
    for(i=2; i<len; i++)
      if (!isdigit(string[i]))
	return COORD_SYNTAX;

    /* Passed preliminary error checking, go ahead and parse the string */

    len = (len - 2) / 2;
    sprintf(control, "%%c%%c%%%dd%%%dd", len, len);
    if (sscanf(string, control, &e_char, &n_char, &e_num, &n_num) != 4)
      return COORD_SYNTAX;

    /*
     * The string has passed error checking.
     */

    /* convert lower to upper case for leading chars */
    if (islower(z_char))
      z_char = toupper(z_char);
    if (islower(e_char))
      e_char = toupper(e_char);
    if (islower(n_char))
      n_char = toupper(n_char);

    e_num *= utm_scale[len];
    n_num *= utm_scale[len];

    return reverse_lookup_utm(*zone, z_char, e_char, n_char, e_num, n_num,
			      northing, easting);
}

void coord_count_utm_zones(tcc, n_zones, base_zone)
    COORD_TCC_PTR tcc;
    int32        *n_zones;
    int32        *base_zone;
{
    extern int32 simnet_origin_zone;
    int32 min_zone, max_zone;
    float64 lat, min_lng, max_lng, z;

    /* First figure out which grid zones these points cover.
     * This code assumes larger grid zones have larger longitude.
     * I don't know for sure that this assumption works everywhere,
     * but it does work in the places I'm interested in.
     */
    if (coord_convert(COORD_TCC, tcc, 0.0, 0.0, 0.0,
		      COORD_LATLON, &lat, &min_lng, &z) ||
	coord_convert(COORD_TCC, tcc, 2.0 * tcc->half_width, 0.0, 0.0,
		      COORD_LATLON, &lat, &max_lng, &z))
    {
	*n_zones = 1;
	*base_zone = tcc->zone;
	return;
    }

    min_zone = COORD_LATLONG_TO_GRID_ZONE(0.0, min_lng);
    max_zone = COORD_LATLONG_TO_GRID_ZONE(0.0, max_lng);

    *n_zones = max_zone - min_zone + 1;
    *base_zone = min_zone;
}

void coord_describe_datums()
{
    int32 i;

    for(i=0;i<NUM_DATUMS(*datum_table);i++)
      printf("Datum %2d: %s\n", i, datum_table->datums[i]->description);
}
