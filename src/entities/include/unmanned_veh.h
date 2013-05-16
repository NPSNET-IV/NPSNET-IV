
#ifndef __NPS_UNMANNED_VEH__
#define __NPS_UNMANNED_VEH__

#include <iostream.h>

#include "aircraft_veh.h"

class UNMANNED_VEH : public AIRCRAFT_VEH {

public:
   UNMANNED_VEH(int,int,ForceID);
   virtual ~UNMANNED_VEH() {;} // cerr << "Aircraft Vehicle - Fixed wing destructor\n"; };

   movestat move();
   vtype gettype();
};

void drawhud_UAV ( pfChannel *, void * );

#endif

