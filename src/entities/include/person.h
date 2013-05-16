// File: <person.h> 
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
#ifndef __PERSON_H__
#define __PERSON_H__

#include "human_constants.h"


const int          HEAD_DRPARAMS = 8;

enum headDREnum {ROT_ANGLE, ELEV_ANGLE,DRROT_ANGLE,DRELEV_ANGLE,
   ROT_RATE,ELEV_RATE,DRROT_RATE,DRELEV_RATE};

// types of soldier groups (if DIS SubCategory <= 100, nbr of indivs)
// ref DIS specs section 4.2.3.3
enum teamEnum {MORTAR_TEAM = 4,PASSING_REV = 41, FIRE_TEAM=100, SQUAD_TEAM,
   PLATOON_TEAM, COMPANY_TEAM, MEDIC_TEAM=257, STRETCHER_TEAM=259};

// "weapon types" for soldiers (EntityType.specific field)
enum specificEnum {NOSPECIFIC, M16=32, AK47=205, STRETCHER=255};

// behavior types for soldiers
enum behaveEnum {NOBEHAVIOR, COMBAT, TWOMAN_STRETCHER,
 RIGHT_STRETCHER, LEFT_STRETCHER,
 MORTAR_TEAM1, MORTAR_TEAM2, PASSING_REV1, PASSING_REV2, LEADER, GUIDEON};

enum jointEnum {NECK_P,NECK_H,NECK_R,HEAD_H,HEAD_P,HEAD_R,
   WAIST_H,WAIST_P,WAIST_R,RIGHT_TOE_P,RIGHT_ANKLE_H,RIGHT_ANKLE_P,RIGHT_ANKLE_R,
   RIGHT_KNEE_P,RIGHT_HIP_H,RIGHT_HIP_P,RIGHT_HIP_R,
   LEFT_TOE_P,LEFT_ANKLE_H,LEFT_ANKLE_P,LEFT_ANKLE_R,LEFT_KNEE_P,
   LEFT_HIP_H,LEFT_HIP_P,LEFT_HIP_R,
   RIGHT_WRIST_H,RIGHT_WRIST_P,RIGHT_WRIST_R,RIGHT_ELBOW_P,
   RIGHT_SHOULDER_H,RIGHT_SHOULDER_P,RIGHT_SHOULDER_R,
   LEFT_WRIST_H,LEFT_WRIST_P,LEFT_WRIST_R,LEFT_ELBOW_P,
   LEFT_SHOULDER_H,LEFT_SHOULDER_P,LEFT_SHOULDER_R};

enum articModeEnum {NO_ARTIC, JACK_ARTIC, DUDE_ARTIC};

#include <iostream.h>

#include "ground_veh.h"
#include "pdu.h"
#include "jointmods.h"

class PERSON_VEH : public GROUND_VEH {

protected:

   fixedDatumRecord hiResJointAngles[NUM_HIRES_VALUES];

   int okToSendHiRes;
   int okToUpdateHiRes;
   unsigned int numHiResJointAngles;
   float hiResTimeStamp;
   pfCoord gun_pos;

   float oldspeed;
   float abs_speed;
   int   old_index;
   
   unsigned int oldstatus;
   float *JointVals; 
   int movement_index; // misc index for various motions
   float head[HEAD_DRPARAMS];
   int unitMember;			// TRUE, if part of another unit
   int immed_transition;       // TRUE if want snap position changes
   int quiet;                  // TRUE if want no joint updates
   float posture_chg_time;     // Time of last posture change

#ifndef NONPSSHIP
   long mounted_vid;         // The vid of vehicle mounted to
   pfVec3 relposvec;
   pfVec3 oldmounthpr;
#endif // NONPSSHIP

   pfSwitch *jack_rifle_sw, *sensor_rifle_sw, *iport_rifle_sw;
   pfDCS *sensor_rifle_dcs;
   pfDCS *iport_rifle_dcs;
   pfCoord iport_rifle_coord;

   // called only by move
   void initMove(int&,int&);
   void determineSpeed( CONTROL&,int);
   void determineZ(int &, pfVec3 *);
   void determinePosture( CONTROL&,pfVec3&,int&);
   void handleTether(CONTROL&,int);
   virtual void computeFirepoint();
   void placeVehicle();
   void checkUnderground(int);
   virtual int check_collide(pfVec3 &,pfVec3 &, pfVec3 &, float &, float &);

   // called only by moveDR
   void computeDR(float deltatime);
   void time2Send(float&);
   void reason2Send(float&);

public:
   PERSON_VEH(int,int,ForceID);
#ifndef NODUDE
   PERSON_VEH(int);
#endif // NODUDE

   virtual void get_weapon_view (pfCoord &);

#ifndef NONPSSHIP
   virtual ~PERSON_VEH();
//for mounting humans
   void mount(int);
   void dismount();
   virtual void updateRelPosVec() {;}
   virtual void reset_on_mounted_vid() {;}
#else
   virtual ~PERSON_VEH() {;} //cerr << "Person - destructor\n";};
#endif // NONPSSHIP

#ifndef NOHIRESNET
   void ok_to_send () { okToSendHiRes = TRUE; }
   void set_hi_res_time_stamp (float the_time) { hiResTimeStamp = the_time; }
#endif // NOHIRESNET
   
   void ok_to_update () { okToUpdateHiRes = TRUE; }
   
   virtual void rot_elev_head();
   void walk(float);
   void normalize_angles();    
   void apply_angles ();
   virtual vtype gettype() {return(PERSON);}
   virtual movestat move();
   virtual movestat moveDR(float,float);
   virtual void entitystateupdate(EntityStatePDU *, sender_info &);
   virtual void sendentitystate();
   virtual void immed_transition_on() {immed_transition = TRUE;}
   virtual void immed_transition_off() {immed_transition = FALSE;}
   virtual void quiet_on() {quiet = TRUE;}
   virtual void quiet_off() {quiet = FALSE;}
   virtual int checkSignal() { return FALSE; }
   virtual void executeSignal(int) {;}

   int lase ();

   virtual void fire_weapon ( const wdestype, const int);

   virtual void go_standing();
   virtual void go_kneeling();
   virtual void go_prone();
   virtual void toggle_weapon_status();

   virtual void process_flybox_input();
   virtual void class_specific_keyboard();
   virtual void class_specific_fcs ();
#ifndef NO_IPORT
   virtual void class_specific_testport ();
   virtual void class_specific_uniport ();
#endif // NO_IPORT
#ifndef NOUPPERBODY
   virtual void class_specific_upperbody ();
#endif // NOUPPERBODY
   virtual int posture_chg_ok ();

   virtual int transport ( float, float, float );

   virtual void apply_hi_res (){;}
   virtual void set_hi_res (fixedDatumRecord *, unsigned int){;}

   virtual void process_keyboard_speed_settings ();

};

#endif

