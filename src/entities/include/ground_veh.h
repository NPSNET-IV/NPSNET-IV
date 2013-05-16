
#ifndef __NPS_GROUND_VEH__
#define __NPS_GROUND_VEH__

#include "vehicle.h"

class GROUND_VEH : public VEHICLE {

protected:
   int subpartscnt; //number of articulated parts
   pfDCS *turret; // the Turret's DCS
   pfDCS *gun;    // the Gun's DCS
   float turret1_angle;
   float turret1_rate;
   float gun1_angle;
   float gun1_rate;
   float DRturret1_angle;
   float DRturret1_rate;
   float DRgun1_angle;
   float DRgun1_rate;
   int gun1_a_change;
   int gun1_r_change;
   int turret1_a_change;
   int turret1_r_change;

public:
   GROUND_VEH(int,int,ForceID);
   virtual ~GROUND_VEH();
    void del_vehicle(void);  //public distructor

   void slewturret(float);    // move the terret
   void elevatgun(float);     // raise the gun
   void turret_set(float);
   void gun_set(float);
   void turret_update(float);
   void gun_update(float);

   virtual movestat move();
   virtual movestat moveDR(float,float);
   virtual movestat movedead();

   virtual vtype gettype();
   virtual void sendentitystate();
   virtual void entitystateupdate(EntityStatePDU *, sender_info &);
   virtual int check_collide(pfVec3 &,pfVec3 &, pfVec3 &, float &, float &);

   virtual int transport ( float, float, float );

};

#endif

