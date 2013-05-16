// File: <vehicle.h> 
//
// NPSNETIV Version 6.0  dtd 06-06-94
// Performer Version 1.0, 1.1 and 1.2 Simulation Code
// Written by:  Paul T. Barham, Roy D. Young, David R. Pratt & Randall Barker
// Naval Postgraduate School, Computer Science Department
// Code CS/Pratt, Monterey, CA 93940-5100 USA
//
// This source code is property of the Naval Postgraduate School.
// Copying, distributing and/or using all or part of this code for
// commercial gain without written permission is prohibited.
//
// Please do not distribute part or all of this source code without
// notifying one or all of the following people:
//
//    Paul Barham, barham@cs.nps.navy.mil
//    David Pratt, pratt@cs.nps.navy.mil
//    Michael Zyda, zyda@cs.nps.navy.mil
//
// This condition is so we can keep track of who has our code to
// assist in justifying our research efforts to our sponsors.
//
// We are also starting a NPSNET-IV mailing list for notification of
//    future changes.  To have your name added to the mailing list,
//    send email to barham@cs.nps.navy.mil.
//
// We do requeust that you give us credit in all projects / papers / 
// demonstrations that use our code.  Do not remove the 
// "Naval Postgraduate School" title from the graphics screen without
// written permission from one of the above listed people.
//
// Change Record:
// Date        Changes                                       Programmer
// 10-19-93    Official release 2.0 w/ documentation         All
// 03-15-94    Change release to 3.0 for addition of
//             articulated parts, animations, munition-
//             specific effects, round-world coordinates,
//             and excersise ID usage.                       Barham
// 06-06-94    Official release 4.0 with Multicast
//             and major upgrades for graphics speed.        Barham
// 07-07-94    Ported to Performer 1.2 for release 5.0.      Barker
// 07-18-94    Used the new Performer 1.2 meathod for        Barker
//             interection testing in release 6.0.
//             This fixed the front face testing
//             failure from release 5.0.            
//

// This is the Vehicle Class definition
//      This is the base class for vehicle
//              Derived classes are GND_VEH, AIR_VEH, FIXED_WING, 
//                 ROTARY_WING, SHIP_VEH
#ifndef __NPS_VEH_CLASS__
#define __NPS_VEH_CLASS__

#include "G_data.h"

#ifdef FREE_MEMORY
void init_frame_queue ();
void update_frame_queue ();
#endif

/*
#ifdef JACK
#include "jacksoldier.h"
#endif
*/

class VEHICLE {

protected:

    // The position and orientation for the vehicle is maintained in a pfCoord
    pfCoord  posture;    // location and orientation of vehicle
    pfVec3   velocity;
    pfVec3   acceleration;
    pfVec3   eyepoint;
    pfVec3   firepoint;
    int      num_joints;
    JNT_INFO joint[MAXJOINTS];
    float    firetimes[MAX_FIRE_INFO];
    pfDCS    *hull;
    pfSwitch *models;
    float    lastPDU;  // last time PDU sent/received for this vehicle
    pfCoord  drpos;     // computed DR position/orientation
    pfCoord  drparam;  //Dead Reckoning parameters velocity/accelerations
    EntityID disname;  // site host entity for dis
    int      icontype;      // index into the icon/vehtype array
    mtype    weapons[3];  // the types of weapons available to this thing
    EntityType    type;   // dis entity type
    ForceID force; //which side is it on
    unsigned int  status;  //32 bit field to discribe it's apperance
    DeadReckAlgorithm dralgo; //the dead Reckoning algorithm 
    EntityMarking marking;
    int displaying_model;
    int birds_eye;
    int snap_switch;
    ctltype Control;
    sender_info controller;

public:

    int vid;    // local vehicle ID
    VEHICLE(int);
    virtual ~VEHICLE();
    void snap_on();
    void snap_off();
    void del_vehicle(void);  //public distructor

    virtual void create_entity(int,ForceID); //Initializes the pfDCS & pfSwitch
    virtual vtype gettype();   // what am i
    virtual ctltype getControl(); // how am i controlled
    virtual vtype getCat()  {return (gettype()); }// what veh category am i
    virtual vtype getSubcat() {return (gettype());}// what veh subcat am i 
    virtual void setSignal(int index) {};

    virtual movestat move();  // move the vehicle for the next frame
    virtual movestat moveDR(float,float);     //move routine for DR vehicles
    virtual movestat movedead();   //crash routine
    float *getposition();       //return current position
    float *getorientation();	// return currient orientation
    pfDCS *gethull();

    void getposparm(pfCoord *, pfVec3 *); //stuff for firing of weapons
    EntityID getdisname();
    ForceID getforce();
    void get_eyepoint ( pfVec3 & );
    void get_firepoint ( pfVec3 & );
    virtual int oktofire ( wdestype, float );
    int setfiretime ( wdestype, float );
    void basicentitystate(EntityStatePDU *);
    virtual void sendentitystate();
    void sendEPDU(EntityStatePDU *);
    void updateDRparams();
    virtual void entitystateupdate(EntityStatePDU *, sender_info &);
    void fillinDetonationPDU(DetonationPDU *,pfVec3);
    void senddetonation(pfVec3);  //used when we crash
    virtual void changestatus(unsigned int); //change the status
    virtual void changeWeaponStatus(unsigned int); // change weapon status
    unsigned int getstatus();
    void init_posture(pfCoord &);
    void init_velocity(pfVec3 );
    pfVec3 *getvelocity();
    void assign_dralgo(DeadReckAlgorithm);
    void assign_weapons(mtype *);
    void assign_entity_type(int);
    void toggle_model();
    void teleport(pfCoord &,float &);
    void attach(pfCoord &, float &, float );
    void turnoff(void);
    void turnon(void);
    void birds_eye_view(void);
    int check_collide(pfVec3 &,pfVec3 &, pfVec3 &, pfVec3 &, float &, float &);
    //friends
    friend void MUNITION::sendfire(int,VEHICLE *,int,PASS_DATA *,float,int);
    friend void acquire_veh_info ( INFO_PDATA * );

};


//GND_VEH Ground vehicle derived type
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
   GROUND_VEH(int);
   ~GROUND_VEH(); 
   void del_vehicle(void);  //public distructor

   void slewturret(float);    // move the terret
   void elevatgun(float);     // raise the gun
   void turret_set(float); 
   void gun_set(float);
   void turret_update(float); 
   void gun_update(float);

   virtual movestat move(PASS_DATA*,CONTROL&);
   virtual movestat moveDR(float,float); //snap to ground for ground vehciles
   virtual movestat movedead(PASS_DATA*,CONTROL&);   //crash routine

   virtual vtype gettype();
   virtual void sendentitystate();
   virtual void entitystateupdate(EntityStatePDU *, sender_info &);
};

   
//AIR_VEH generic vehicle derived type
class AIRCRAFT_VEH : public VEHICLE {

public:
   AIRCRAFT_VEH(int);
   ~AIRCRAFT_VEH() { cerr << "Aircraft Vehicle destructor\n";};

   movestat move(PASS_DATA*,CONTROL&);    // move the vehcile for the next frame
   vtype gettype();
};

//AIR_VEH fixed wing vehicle derived type
class FIXED_WING_VEH : public AIRCRAFT_VEH {

public:
   FIXED_WING_VEH(int);
   ~FIXED_WING_VEH() { cerr << "Aircraft Vehicle - Fixed wing destructor\n";};

   movestat move(PASS_DATA*,CONTROL&);    // move the vehcile for the next frame
   vtype gettype();
};


//AIR_VEH Helocopter vehicle derived type
class ROTARY_WING_VEH : public AIRCRAFT_VEH {

protected:
   int subpartscnt; //number of articulated parts - NOT USING
   pfDCS *mainrotor; // the Main Rotor's DCS - NOT USING
   pfDCS *tailrotor;    // the tail Rotor's DCS - NOT USING
   float mrangle,trangle; //current main and tail angles 
   float mrrate,trrate; //current dead reckon rotor angle rates

public:
   ROTARY_WING_VEH(int);
   ~ROTARY_WING_VEH();
    void del_vehicle(void);  //public distructor

   void moverotorblades(float);
   movestat move(PASS_DATA*,CONTROL&);    // move the vehcile for the next frame
   movestat moveDR(float,float); //rotate rotors for heliocopters

   vtype gettype();
};


//SEA_VEH vehicle derived type
class SHIP_VEH : public VEHICLE {

public:
   SHIP_VEH(int id):VEHICLE(id){};
   ~SHIP_VEH() { cerr << "Ships - destructor\n";};

   movestat move(PASS_DATA*,CONTROL&);    // move the vehcile for the next frame
   vtype gettype();
};


class STEALTH_VEH : public VEHICLE {

public:
   STEALTH_VEH(int id):VEHICLE(id){};
   ~STEALTH_VEH() { cerr << "STEALTH - destructor\n";};
   movestat move (PASS_DATA *pdata, CONTROL &cdata);
   movestat moveDR(float,float); 
   movestat movedead(PASS_DATA *pdata,CONTROL &cdata);
   int oktofire ( wdestype, float );
   vtype gettype();
};


//UNMANNED_VEH fixed wing vehicle derived type
class UNMANNED_VEH : public AIRCRAFT_VEH {

public:
   UNMANNED_VEH(int);
   ~UNMANNED_VEH() { cerr << "Aircraft Vehicle - Fixed wing destructor\n";};

   movestat move(PASS_DATA*,CONTROL&);  // move the vehcile for the next frame
   vtype gettype();
};


class SUBMERSIBLE_VEH : public VEHICLE {

public:
   SUBMERSIBLE_VEH(int id):VEHICLE(id){};
   ~SUBMERSIBLE_VEH() { cerr << "Sub - destructor\n";};

   movestat move(PASS_DATA*,CONTROL&);    // move the vehcile for the next frame
   movestat moveDR(float,float);
   vtype gettype();
};


#include "person.h"
#include "jack_di.h"
#include "iport_di.h"
#endif
