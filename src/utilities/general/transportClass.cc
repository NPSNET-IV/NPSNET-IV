#include <iostream.h>
#include <pf.h>

#include "npsIOStream.h"
#include "transportClass.h"

npsTransportInfo::npsTransportInfo (float min_x, float min_y, float min_z,
                                    float max_x, float max_y, float max_z,
                                    float theX, float theY, float theZ,
                                    float theH, float theP, float theR,
                                    int theMode)
{
   min[PF_X] = min_x;
   min[PF_Y] = min_y;
   min[PF_Z] = min_z;

   max[PF_X] = max_x;
   max[PF_Y] = max_y;
   max[PF_Z] = max_z;

   pos.xyz[PF_X] = theX;
   pos.xyz[PF_Y] = theY;
   pos.xyz[PF_Z] = theZ;

   pos.hpr[PF_H] = theH;
   pos.hpr[PF_P] = theP;
   pos.hpr[PF_R] = theR;

   mode = theMode;
}

int
npsTransportInfo::in_zone (pfVec3 location)
{
   int inZone = FALSE;

   if ((min[PF_X] <= location[PF_X]) &&
       (min[PF_Y] <= location[PF_Y]) &&
       (min[PF_Z] <= location[PF_Z]) &&
       (max[PF_X] >= location[PF_X]) &&
       (max[PF_Y] >= location[PF_Y]) &&
       (max[PF_Z] >= location[PF_Z])) {
      inZone = TRUE;
   }

   return inZone;
}

void
npsTransportInfo::get_point (pfCoord &point)
{
   pfCopyVec3 (point.xyz, pos.xyz);
   pfCopyVec3 (point.hpr, pos.hpr);
}

void
npsTransportInfo::get_mode (int &theMode)
{
   theMode = mode;
}
