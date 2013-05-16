
#ifndef __NPS_REAL_ROTARY_WING_VEH__
#define __NPS_REAL_ROTARY_WING_VEH__

#include "rotary_wing_veh.h"

// Constants added for helo
#define HELO_WEIGHT 20000.0f
#define HELO_MASS 0.0145f
#define MAX_POWER 2.0f*HELO_WEIGHT
#define MAX_LAT_SPEED  40.0f*MPH_TO_MPS
#define MIN_LAT_SPEED  -40.0f*MPH_TO_MPS
#define MAX_ACCEL  1.0f*MPH_TO_MPS
#define MAX_DECEL  -1.0f*MPH_TO_MPS
#define MAX_FW_SPEED  180.0f*MPH_TO_MPS
#define MIN_FW_SPEED  -40.0f*MPH_TO_MPS
#define MAX_GND_SPEED  45.0f*MPH_TO_MPS
#define MIN_GND_SPEED  0.0f*MPH_TO_MPS
#define MY_FCS_SPEED_CHANGE MAX_FW_SPEED*0.0005f

class REAL_ROTARY_WING_VEH : public ROTARY_WING_VEH {

protected:
   int subpartscnt;		//number of articulated parts - NOT USING
   pfDCS *mainrotor;		// the Main Rotor's DCS - NOT USING
   pfDCS *tailrotor;		// the tail Rotor's DCS - NOT USING
   float mrangle,trangle;	//current main and tail angles
   float mrrate,trrate;		//current dead reckon rotor angle rates

public:
   REAL_ROTARY_WING_VEH(int,int,ForceID);
   ~REAL_ROTARY_WING_VEH();
   void del_vehicle(void);

   movestat move();

   vtype gettype();
};

#endif
