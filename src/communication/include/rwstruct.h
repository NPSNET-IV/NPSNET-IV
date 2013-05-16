#ifndef RWSTRUCT_DOT_H
#define RWSTRUCT_DOT_H

typedef struct _roundWorldStruct {
   double northing;
   double easting;
   int    o_zone_num;
   char   o_zone_letter;
   int    map_datum;
   int    width;
   int    height;
   int    rw_valid;
} roundWorldStruct;

#endif
