
#ifndef __NPS_CONVETER_H__
#define __NPS_CONVETER_H__

#include <stddef.h>
#include "stdtypes.h"
#include "rwstruct.h"

class nps_converter {

public:

   nps_converter ( const roundWorldStruct * );
   ~nps_converter ();

   // Overload new and delete so member variables are allocated out
   // of Performer shared memory.
   void *operator new(size_t);
   void operator delete(void *, size_t);

   int nps_to_utm ( const float64, const float64, float64 &, float64 & );
   int utm_to_nps ( const float64, const float64, float64 &, float64 & );


private:

   float64 northing;
   float64 easting;
   int32   o_zone_num;
   char    o_zone_letter;
   int32   map_datum;
   int32   width;      /* East to West */
   int32   height;     /* South to North */
   int     valid_converter;

};
#endif

