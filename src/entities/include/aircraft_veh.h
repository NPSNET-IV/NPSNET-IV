
#ifndef __NPS_AIRCRAFT_VEH__
#define __NPS_AIRCRAFT_VEH__

#include "vehicle.h"

//************************************************************************//
//*************** class AIRCRAFT_VEH *************************************//
//************************************************************************//

class AIRCRAFT_VEH : public VEHICLE {

public:
   AIRCRAFT_VEH(int,int,ForceID);
   virtual ~AIRCRAFT_VEH();

   movestat move();
   vtype gettype();
   virtual int check_collide(pfVec3 &,pfVec3 &, pfVec3 &, float &, float &);
   virtual int transport ( float, float, float );
   virtual float get_altitude();

};

void air_hud_draw ( pfChannel *, void * );

#endif
