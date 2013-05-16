#ifndef __NPS_STEALTH_VEH__
#define __NPS_STEALTH_VEH__

#include <iostream.h>

#include "vehicle.h"

class STEALTH_VEH : public VEHICLE {

protected:
   pfCoord gun_pos;
   pfCoord m_veh;

public:
   STEALTH_VEH(int,int, ForceID);
   virtual ~STEALTH_VEH() {;} // cerr << "STEALTH - destructor\n";}
   virtual void get_weapon_view (pfCoord &);
   virtual void process_keyboard_speed_settings ();
   movestat move ();
   movestat moveDR(float,float);
   movestat movedead();
   int oktofire ( wdestype, float );
   vtype gettype();
};


#endif
