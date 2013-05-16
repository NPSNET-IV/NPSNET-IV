/*
 * $RCSfile: libcoord_local.h,v $ $Revision: 1.3 $ $State: Exp $
 */
/****************************************************************************
*   File: libcoord_local.h                                                  *
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
*       Contents: Private header file for libcoordinates                    *
*       Created: Fri Aug 14 1992                                            *
*       Author: jesmith                                                     *
*       Remarks:                                                            *
*                                                                           *
****************************************************************************/

#include "libcoordinates.h"

/*
 * This has a peculiar definition for historical reasons.
 */
#define COORD_GCC_DATUM 3

/*
 * Private data structures.
 */

struct coord_tcc
{
    COORD_SYSTEM      source; /* GCC or UTM_NE */
    int32             datum;
    int32             zone;
    char              zone_letter;
    float64           northing; /* of center, NOT SW corner */
    float64           easting;  /* of center, NOT SW corner */
    float64           half_width;
    float64           half_height;
    float64           t_to_g_rot[3][3];
    float64           center_in_gcc[3];
    struct coord_tcc *next;
};

extern COORD_TCC_PTR coord_tcc_list;

/*
 * Data structures for interpreting libreader data file.
 */

typedef struct coord_letter_list
{
    int32   length;
    char   *letters[1];
} COORD_LETTER_LIST;
#define NUM_LETTERS(list) \
  ((list).length-sizeof(COORD_LETTER_LIST)/sizeof(int32)+1)

typedef struct coord_letter_range
{
    int32   min_northing;
    int32   max_northing;
    char   *letter;
} COORD_LETTER_RANGE;

typedef struct coord_letter_table
{
    int32                length;
    COORD_LETTER_RANGE   ranges[1];
} COORD_LETTER_TABLE;
#define NUM_RANGES(list) \
  (((list).length-sizeof(COORD_LETTER_LIST)/sizeof(int32))/3+1)

typedef struct coord_grid_box
{
    int32                always_12;
    int32                min_northing;
    int32                max_northing;
    int32                min_easting;
    int32                max_easting;
    float32              min_latitude;
    float32              max_latitude;
    float32              min_longitude;
    float32              max_longitude;
    int32                datum;
    COORD_LETTER_LIST   *n_letters;
    COORD_LETTER_LIST   *e_letters;
} COORD_GRID_BOX;

typedef struct coord_zone_table
{
    int32                 length;
    int32                 zone_number;
    COORD_LETTER_TABLE   *z_letters;
    COORD_GRID_BOX       *boxes[1];
} COORD_ZONE_TABLE;
#define NUM_BOXES(table) \
  ((table).length-sizeof(COORD_ZONE_TABLE)/sizeof(int32)+1)

typedef struct coord_utm_table
{
    int32             length;
    char             *tag;
    COORD_ZONE_TABLE *zones[1];
} COORD_UTM_TABLE;
#define NUM_ZONES(table) \
  ((table).length-sizeof(COORD_UTM_TABLE)/sizeof(int32)+1)

typedef struct coord_molodenskiy_xform
{
    int32   length;
    int32   datum;
    char   *description;
    float32 dx;
    float32 dy;
    float32 dz;
    float32 major_axis;
    float32 minor_axis;
} COORD_MOLODENSKIY_XFORM;

typedef struct coord_datum_table
{
    int32                      length;
    char                      *tag;
    COORD_MOLODENSKIY_XFORM   *datums[1];
} COORD_DATUM_TABLE;
#define NUM_DATUMS(table) \
  ((table).length-sizeof(COORD_DATUM_TABLE)/sizeof(int32)+1)

/* Private prototypes */
extern void coord_gcc_to_llz();
extern void coord_llz_to_gcc();
extern void coord_utm_to_ll();
extern void coord_ll_to_utm();
extern void coord_gcc_to_tcc();
extern void coord_tcc_to_gcc();
extern void coord_utm_to_tcc();
extern void coord_tcc_to_utm();
extern void coord_utm_to_grid();
extern int32 coord_grid_to_utm();
extern void coord_change_utm_zone();
extern float64 coord_get_major_axis();
extern float64 coord_get_minor_axis();
extern void coord_get_datum_shifts();
extern int32 coord_lookup_datum_by_ne();
extern int32 coord_lookup_datum_by_ll();
extern char coord_lookup_zone_letter();
extern void coord_lookup_utm();
extern int32 coord_parse_utm();
