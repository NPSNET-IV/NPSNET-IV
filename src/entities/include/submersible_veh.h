
#ifndef __NPS_SUBMERSIBLE_VEH__
#define __NPS_SUBMERSIBLE_VEH__

#include <iostream.h>

#include "vehicle.h"
#ifndef NOSUB
#include "UUVmodel.h"
#include <idunetlib.h>  //COMMS
#endif // NOSUB

class SUBMERSIBLE_VEH : public VEHICLE {
#ifndef NOSUB
protected:
  float Max_RPM ;
  float Min_RPM;
  float total_RPM;
  float total_planes;
  float total_rudder;

  float ordered_depth;  // 0000
  float ordered_course; // 000
  float ordered_speed;  //-3 - 4 (back full - flank)
  float helm_ordered_speed;  //-3 - 4 (back full - flank)

  float embtblow_onoff;   //1 0
  float mbtvents_openshut; //1 0

  float scope_raiselower;  //1 0
  float scope_updown; //1 -1
  float scope_leftright; //1 -1
  float scope_mark;  //1 0
  float scope_by;    //bearing that scope is looking at when mark is hit.
  float scope_rh;    //range to object that scope is looking at when mark 
                         //is hit.

  float selecttube_2or4; //1 0
  float port_tube_shoot;
  float port_tube_course; 
  float selecttube_1or3; //1 0
  float stbd_tube_shoot; 
  float stbd_tube_course;
  float selecttlam;      //1 - 6
  float tlam_shoot; 
  float tlam_course;

  UUVBody subbody;
  //COMMS---------------------------------
    IDUType type;
    char *idu;
    TestIDU *testidu;
    NPSNETToSubIDU       updatesubidu;
    HelmToNPSNETIDU      *helmidu;
    SubOodToNPSNETIDU       *oodidu;
    WeapsToNPSNETIDU     *weapsidu;
   
    sender_info idu_sender_info;
    int swap_bufs;
    IDU_net_manager *net;
  //COMMS---------------------------------
   void helmupdate( HelmToNPSNETIDU *helmidu);
   void oodupdate( SubOodToNPSNETIDU *oodidu);
   void weapsupdate( WeapsToNPSNETIDU *weapsidu);
#endif // NOSUB
public:
#ifndef NOSUB
   SUBMERSIBLE_VEH(int id,int atype, ForceID the_force);
   virtual ~SUBMERSIBLE_VEH();
#else
   SUBMERSIBLE_VEH(int id,int atype, ForceID the_force):VEHICLE(id,atype,the_force){};
   virtual ~SUBMERSIBLE_VEH() {;} // cerr << "Sub - destructor\n";};
#endif // NOSUB

   movestat move();
   movestat moveDR(float,float);
   vtype gettype();
   virtual int check_collide(pfVec3 &, pfVec3 &, pfVec3 &, float &, float & );
#ifndef NOSUB
   float get_depth();
#endif NOSUB
};


#endif

