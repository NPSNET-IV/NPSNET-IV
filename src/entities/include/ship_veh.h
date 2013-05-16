
#ifndef __NPS_SHIP_VEH__
#define __NPS_SHIP_VEH__

#include <iostream.h>

#include "vehicle.h"
#include <idunetlib.h>

//SEA_VEH vehicle derived type
class SHIP_VEH : public VEHICLE {

protected:
float rudderAngle;                  //Rudder angle deflection
float mass;                         //Mass in kg
float thrustLeft;                   //Left engine thrust
float thrustRight;                  //Right engine thrust
float thrustTotal;                  //Total thrust
float torqueLeft;                   //Torque generated by left engine 
float torqueRight;                  //Torque generated by right engine
float torqueRudder;                 //Torque generated by rudder deflection
float torqueTotal;                  //Total torque
float thrustPerRPM;                 //Thrust per rpm
float propOffset;                   //Prop shaft lateral offset from CG
float rudderOffset;                 //Rudder longitudinal offset from CG 
double momentInertia;                //Mass moment of inertia
float alpha;                        //Angular acceleration
float omega;                        //Angular velocity
float linspeed;                     //Linear speed (scaler)
float linacceleration;              //Linear acceleration 
float portRPM;
float stbdRPM;
float delTime;
pfVec3 bridgeview;		    //Location of viewpoint on bridge of ship

#ifndef NONPSSHIP
VEHICLE *mounted_veh[MAX_VEH];	    //List of vehicles mounted to us
#endif // NONPSSHIP

float ordered_rudder_angle; // 000
float ordered_rpm;          // 000
float ordered_port_bell;    //-3 - 5 (back full - flank)
float ordered_stbd_bell;    //-3 - 5 (back full - flank)
int keyboard_order;

//COMMS---------------------------------
IDUType type;
char *idu;
TestIDU *testidu;
NPSNETToShipIDU       updateshipidu;
OodToNPSNETIDU       *oodidu;
   
sender_info idu_sender_info;
int swap_bufs;
IDU_net_manager *net;

void oodupdate( OodToNPSNETIDU *oodidu);
//COMMS---------------------------------

public:
   SHIP_VEH(int id, int atype, ForceID the_force);
   ~SHIP_VEH();

   virtual SHIP_VEH *is_WALK_CLASS(){ return NULL; }
   virtual movestat move();
   virtual movestat moveDR(float, float);
   virtual movestat movedead ();//crash routine
   vtype gettype();
   virtual int check_collide(pfVec3 &, pfVec3 &, pfVec3 &, float &, float & );
   virtual void sendentitystate ();
   virtual void entitystateupdate ( EntityStatePDU *, sender_info & );
   void getbridgeview(pfVec3 &);

#ifndef NONPSSHIP
   virtual void mount(VEHICLE *, int);
   virtual void dismount(int);
#endif // NONPSSHIP

   virtual void process_keyboard_speed_settings ();
   virtual void process_keyboard_input ();
   virtual void class_specific_keyboard () {;}

};

#endif
