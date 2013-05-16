#include <iostream.h>
#include <pf.h>

#include "npsIOStream.h"

ostream& operator<< (ostream& a, pfCoord &b)
{
   a << b.xyz << "  " << b.hpr;
   return a;
}

ostream& operator<< (ostream& a, const pfCoord &b)
{
   a << b.xyz << "  " << b.hpr;
   return a;
}

ostream& operator<< (ostream& a, pfVec3 b)
{
   a << b[PF_X] << "  "
     << b[PF_Y] << "  "
     << b[PF_Z];

   return a;
}

ostream& operator<< (ostream& a, const pfVec3 b)
{
   a << b[PF_X] << "  "
     << b[PF_Y] << "  "
     << b[PF_Z];

   return a;
}
