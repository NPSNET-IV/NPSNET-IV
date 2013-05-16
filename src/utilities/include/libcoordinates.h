/*
 * $RCSfile: libcoordinates.h,v $ $Revision: 1.5 $ $State: Exp $
 */
/****************************************************************************
*   File: libcoordinates.h                                                  *
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
*       Contents: Public header file for libcoordinates                     *
*       Created: Fri Aug 14 1992                                            *
*       Author: jesmith                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _LIBCOORDINATES_H_INCLUDED
#define _LIBCOORDINATES_H_INCLUDED

#include <stdtypes.h> /*common/include/global*/

/* Coordinate systems */
typedef enum
{
    /* Pretty-printed version of one of the other coordinate systems.
     * You can only convert *to* this (not from).
     */
    COORD_CHARACTER_STRING,

    /* 3D cartesian, with Z thru the north pole, WGS84 ellipsoid
     */
    COORD_GCC,

    /* 3D cartesian, centered around a given point; may be "curved"
     * (GCC derived) or "flat" (UTM derived)
     */
    COORD_TCC,

    /* Lat/Long & ellipsoid Z
     */
    COORD_LATLON,

    /* Northing, Easting, Zone, ellipsoid Z
     */
    COORD_UTM_NE,

    /* Character string, Zone, ellipsoid Z
     */
    COORD_UTM_GRID
} COORD_SYSTEM;

/* Error codes */
#define COORD_SYNTAX         -1
#define COORD_TOO_LONG       -2
#define COORD_ZONE_UNKNOWN   -3
#define COORD_GRID_UNKNOWN   -4
#define COORD_BAD_TCC        -5
#define COORD_SYSTEM_UNKNOWN -6

typedef struct coord_tcc *COORD_TCC_PTR;

/* coord_init:
 *
 * Initializes libcoordinates, causing it to read its data file
 * (coordinates.rdr) either from '.' or the specified data path.
 * The reader_flags are as in reader_read.
 * The return value is zero if the read succeeds, or one of the
 * libreader return values: READER_READ_ERROR READER_FILE_NOT_FOUND
 */
#ifdef __cplusplus
int32 coord_init ( char *, uint32 );
#else
extern int32 coord_init(/* char *data_path,
			   uint32 reader_flags
			   */);
#endif

/* coord_define_tcc:
 *
 * Defines a "topocentric" coordinate system (centered around a given
 * location, at which Z is up, X is east, and Y is north).  The
 * source_system must be either COORD_GCC or COORD_UTM_NE.  GCC derived
 * TCC systems curve "down" in their Z values as distance increases
 * from the center point.  UTM derived TCC systems are completely flat.
 * It is significantly cheaper to convert TCC coordinates to the system
 * from which it was derived than to the other system.  Use the
 * returned pointer in calls to coord_convert.
 */
#ifdef __cplusplus
COORD_TCC_PTR coord_define_tcc ( COORD_SYSTEM, float64, float64, int32,
                                 char, int32, int32, int32 );
#else
extern COORD_TCC_PTR coord_define_tcc(/* COORD_SYSTEM source_system,
					 float64 origin_northing,
					 float64 origin_easting,
					 int32   origin_zone_number,
					 int32   origin_zone_letter,
					 int32   mapping_datum,
					 int32   database_width, (E->W)
					 int32   database_height (S->N)
					 */);
#endif

/* coord_estimate_tcc:
 *
 * Makes a reasonably accurate guess at the TCC parameters, based upon
 * commonly known information.
 */
extern COORD_TCC_PTR coord_estimate_tcc(/* COORD_SYSTEM source_system,
					   int32 latitude_hun_sec,
					   int32 longitude_hun_sec,
					   char *origin_utm_grid
					   int32 database_width, (E->W)
					   int32 database_height (S->N)
					   */);

/* coord_tcc_gcc_rotation:
 *
 * Fills in the passed 3x3 TCC to GCC rotation matrix based upon the
 * GCC-derived TCC (the GCC to TCC matrix is the transpose of the
 * returned matrix).  The return value, if non-zero indicates that
 * an error occured.  Passing a UTM_NE-derived TCC will yield an error 
 * (the correct rotation from UTM to GCC depends upon the UTM location).
 */
extern int32 coord_tcc_gcc_rotation(/* COORD_TCC_PTR tcc,
				       float64 result[3][3]
				       */);

/* coord_convert:
 *
 * Takes two sets of arguments -- from_system and to_system.  Each
 * system has a unique code and a series of values:
 *
 * COORD_CHARACTER_STRING  string
 * COORD_GCC               x y z
 * COORD_TCC               tcc_ptr x y z
 * COORD_LATLON            latitude longitude z local_datum
 * COORD_UTM_NE            zone northing easting z
 * COORD_UTM_GRID          zone resolution string z
 *
 * var         from_type  to_type   units        example
 * ---         --------- ---------  -----        -------
 * x           float64   float64 *  meters       5000.0
 * y           float64   float64 *  meters       5000.0
 * z           float64   float64 *  meters       283.0
 * zone        int32     int32 *                 38 or COORD_DEFAULT_ZONE
 * northing    float64   float64 *  meters       500000.0 (+N -S)
 * easting     float64   float64 *  meters       500000.0
 * resolution  int32     int32      digits       3          --> ES450550
 * latitude    float64   float64 *  degrees      39.0 (+N -S)
 * longitude   float64   float64 *  degrees      42.0 (+E -W)
 * local_datum       int32                       TRUE/FALSE
 * tcc_ptr        COORD_TCC_PTR
 * string            char *
 *
 * Note: UTM_GRID resolution is ignored for from_system
 *       Explicit zone in UTM_GRID string overrides passed zone
 *       local_datum=FALSE implies to use WGS84, as with GPS
 *
 * Non-zero return values indicate errors:
 *
 * COORD_SYNTAX: Character string not of correct format
 * COORD_TOO_LONG: Too many milgrid digits passed
 * COORD_ZONE_UNKNOWN: Milgrid in unrecognized UTM zone
 * COORD_GRID_UNKNOWN: Milgrid grid not recognized in UTM zone
 * COORD_BAD_TCC: Passed TCC pointer invalid
 * COORD_SYSTEM_UNKNOWN: Either to_system or from_system is not recognized
 */
#define COORD_DEFAULT_ZONE 0
#ifdef __cplusplus
int32 coord_convert ( ... );
#else
extern int32 coord_convert(/* COORD_SYSTEM from_system,
			      values...,
			      COORD_SYSTEM to_system,
			      values...
			      */);
#endif

/* coord_error:
 *
 * Returns a string describing the error code returned by
 * coord_convert.
 */
extern char * coord_error(/* int32 code */);

/* coord_generate_grid:
 *
 * Takes bounding points in TCC coordinates and generates two lists of
 * line segments (thin and thick) which form a grid with the specified
 * spacing, and a list of text strings which can be used to label the
 * grid.
 *
 * label_mask indicates which sides to label
 * segments is a flat list x0 y0 x1 y1 x0 y0 x1 y1 ...
 * segments has 4*n_segments points returned
 * max_segments and max_labels indicate the length of the passed arrays
 */
/* values for label_mask */
#define COORD_LABEL_LEFT   0x1
#define COORD_LABEL_RIGHT  0x2
#define COORD_LABEL_TOP    0x4
#define COORD_LABEL_BOTTOM 0x8
#define COORD_LABEL_ALL    0xF
/* labels are of the form... */
#define COORD_MAX_GRID_LABEL_TXT 8
typedef struct
{
    int32 x;
    int32 y;
    char txt[COORD_MAX_GRID_LABEL_TXT];
} COORD_GRID_LABEL;
extern void coord_generate_grid(/* COORD_TCC_PTR tcc,
				   int32 min_x, max_x, min_y, max_y,
				   int32 spacing, digits,
				   uint32 label_mask,
				   int32 max_thin_segments,
				   int32 max_thick_segments,
				   int32 max_labels,
				   int32 *n_thin_segments,
				   float32 thin_segments[],
				   int32 *n_thick_segments,
				   float32 thick_segments[],
				   int32 *n_labels,
				   COORD_GRID_LABEL labels[]
				   */);

/* coord_describe_datums:
 *
 * Prints descriptions of all known datums (from table read in coord_init).
 */
extern void coord_describe_datums();

/* coord_format_latlon:
 *
 * Returns a pointer to a character buffer which has the passed lat/long
 * represented as ASCII text.  The buffer is static, so only one invocation
 * per argument list is allowed.
 */
extern char *coord_format_latlon(/* float64 latitude,
				    float64 longitude
				    */);

/* coord_fixed_point_degrees:
 *
 * Returns an integer which is the passed number of degrees in a
 * DDMMSS.HH format.
 */
extern int32 coord_fixed_point_degrees(/* float64 degrees */);

/* coord_floating_point_degrees:
 *
 * Decodes DDMMSS.HH format into simple degrees.
 */
extern float64 coord_floating_point_degrees(/* int32 ddmmsshh */);

/* coord_parse_fixed_point_degrees:
 *
 * Parses character string to generate DDMMSSHH format fixed point integer.
 */
extern int32 coord_parse_fixed_point_degrees(/* char *string */);

/* coord_count_utm_zones:
 *
 * Returns the number of utm zones covered by the database TCC, and
 * the number of the lowest zone.
 */
extern void coord_count_utm_zones(/* COORD_TCC_PTR tcc_ptr,
				     int32 *n_zones,
				     int32 *base_zone
				     */);

/* coord_lookup_datum_by_ne:
 *
 * Returns the DMA suggested mapping datum for a particular location.
 */
extern int32 coord_lookup_datum_by_ne(/* int32 zone,
					 float64 northing,
					 float64 easting
					 */);

/* coord_lookup_zone_letter:
 *
 * Given a zone & a northing, looks up the correct letter designation.
 */
extern char coord_lookup_zone_letter(/* int32 zone
					float64 northing
					*/);

/* miscellaneous macros */

/* COORD_LATLONG_TO_GRID_ZONE:
 *
 * Given a latitude & longitude, returns the UTM zone number in which
 * that coordinates resides.  With the exception of a couple anomolies,
 * the world is broken up into 6 degree wide slices.
 */
#define COORD_LATLONG_TO_GRID_ZONE(latitude, longitude)	\
  (((latitude >= 56.0) && (latitude <= 64.0) &&		\
    (longitude >= 3.0) && (longitude <= 12.0)) ? 32 :	\
   (((latitude >= 72.0) && (latitude <= 80.0) &&	\
     (longitude >= 0.0) && (longitude <= 42.0)) ? 	\
    (2 * ((int32) (((longitude)+3.0) / 12.0)) + 31) :	\
    ((int32) ((longitude) / 6.0 + 31.0))))

/* COORD_WEST_LONG_OF_GRID_ZONE:
 *
 * Given a grid zone (in the range 1 thru 60), returns the longitude
 * of the western edge of that zone at the equator.
 */
#define COORD_WEST_LONG_OF_GRID_ZONE(zone) ((float64) (((zone) - 31.0) * 6.0))

/* COORD_EAST_LONG_OF_GRID_ZONE:
 *
 * Given a grid zone (in the range 1 thru 60), returns the longitude
 * of the eastern edge of that zone at the equator.
 */
#define COORD_EAST_LONG_OF_GRID_ZONE(zone) ((float64) (((zone) - 30.0) * 6.0))

#endif /* _LIBCOORDINATES_H_INCLUDED */

#ifdef __cplusplus
}
#endif
