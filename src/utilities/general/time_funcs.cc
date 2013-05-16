
#include <math.h>
#include "time_funcs.h"
#include "conversion_const.h"


void seconds_to_hms ( float origseconds,
                      int &seconds,
                      int &minutes,
                      int &hours )
{
   float remainder = origseconds;

   hours = (int)ftrunc(remainder/SECONDS_IN_AN_HOUR);
   remainder -= (float)(hours * SECONDS_IN_AN_HOUR);
   minutes = (int)ftrunc(remainder/SECONDS_IN_A_MINUTE);
   remainder -= (float)(minutes * SECONDS_IN_A_MINUTE);
   seconds = (int)remainder;
}

