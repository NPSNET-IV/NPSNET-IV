
#ifndef __NPS_FIXED_WING_VEH__
#define __NPS_FIXED_WING_VEH__

#include <iostream.h>
#include "aircraft_veh.h"

class FIXED_WING_VEH : public AIRCRAFT_VEH {

public:
   FIXED_WING_VEH(int,int,ForceID);
   virtual ~FIXED_WING_VEH() {;} // cerr << "Aircraft Vehicle - Fixed wing destructor\n"; };

   movestat move();
   vtype gettype();
};



#endif

