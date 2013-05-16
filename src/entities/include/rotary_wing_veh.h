
#ifndef __NPS_ROTARY_WING_VEH__
#define __NPS_ROTARY_WING_VEH__

#include "aircraft_veh.h"

class ROTARY_WING_VEH : public AIRCRAFT_VEH {

protected:
   int subpartscnt;		//number of articulated parts - NOT USING
   pfDCS *mainrotor;		// the Main Rotor's DCS - NOT USING
   pfDCS *tailrotor;		// the tail Rotor's DCS - NOT USING
   float mrangle,trangle;	//current main and tail angles
   float mrrate,trrate;		//current dead reckon rotor angle rates

public:
   ROTARY_WING_VEH(int,int,ForceID);
   virtual ~ROTARY_WING_VEH();
   void del_vehicle(void);

   void moverotorblades(float);
   movestat move();
   movestat moveDR(float,float);

   vtype gettype();
};

#endif
