// File: <dude.cc>

/*
 * Copyright (c) 1995,
 *    Naval Postgraduate School
 *    Computer Graphics and Video Laboratory
 *    NPSNET Research Group
 *    npsnet@cs.nps.navy.mil
 *
 *
 *    Permission to use, copy, and modify this software and its
 * documentation for any non-commercial purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Naval Postgraduate School
 * Computer Graphics and Video Laboratory and the NPSNET Research Group
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your 
 * software generates.
 *    This restriction helps justify our research efforts to the
 * sponsors who fund our research.
 *
 *    Do not redistribute this code without the express written
 * consent of the NPSNET Research Group. (E-mail communication and our
 * confirmation qualifies as written permission.) As stated above, this 
 * restriction helps justify our research efforts to the sponsors who
 * fund our research.
 *
 *    This software was designed and implemented at U.S. Government
 * expense and by employees of the U.S. Government.  It is illegal
 * to charge any U.S. Government agency for its partial or full use.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * E-Mail addresses:
 *    npsnet@cs.nps.navy.mil
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */
#include <stdio.h>
#include <math.h>

#include "dude.h"
#include "filter.h"
#include "human_const.h"
#include "common_defs2.h"
#include "appearance.h"
#include "disnetlib.h"
#include "netutil.h"
#include "terrain.h"
#include "conversion_const.h"
#include "collision_const.h"
//#include "manager.h"

// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile STATIC_DATA     *G_static_data;
extern          VEH_LIST        *G_vehlist;
extern          VEHTYPE_LIST    *G_vehtype;
extern volatile int              G_drivenveh;
extern          DIS_net_manager *G_net;
extern          pfGroup         *G_moving;
extern          void             firebuttonhit(wdestype,int,PASS_DATA*); 

DUDECLASS::DUDECLASS(int id, int atype, ForceID the_force) 
: PERSON_VEH(id,atype,the_force){
  
  // initialize dude to be standing with no velocity
  status = Appearance_LifeForm_UprightStill;

  // clone node structure and then call local function
  create_entity(atype, the_force);   

  // initialize dude to not be in seen and to be alive
  displaying_model = TRUE;
  dead = FALSE;

  // highest LOD dude has nine articulated joints
  num_joints = 9;
 
  // initilize walking components
  ArmBy = 0.0f;
  DeltaArmBy = 2.0f;
  LegBy = 0.0f; 
  calfright = 0.0f; 
  calfleft = 0.0f; 
  first_step = 1;
  oldspeed = 0.0f;

  // initialize ranges for all three LOD models
  medLODrange    = 50;
  medfarLODrange = 100;
  farLODrange    = 250;
  Jack_offset    = 0;

  // get channel to allow intersection testing later
  chan = (CHANNEL_DATA *)G_dynamic_data->pipe[0].channel;

/*
  // allocate shared before fork()'ing parallel processes 
  Sphere = (pfSphere*)pfMalloc(sizeof(pfSphere), 
            pfGetSharedArena());


  // put bounding sphere around the dude
  pfMakeEmptySphere(Sphere);
  Sphere->radius = 1.2f;
  pfSetVec3(Sphere->center, posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
*/

  // attach culling point to dude at waist at proper X, Y, and Z
  pfSetVec3(pt, posture.xyz[X], posture.xyz[Y], posture.xyz[Z]+0.8);

}

DUDECLASS::~DUDECLASS(){

  cout<<"Dude destructor called"<<endl;
}

DUDECLASS::DUDECLASS(pfDCS *jadehull, pfSwitch *whichSwitch, int id): 
           PERSON_VEH(id){ 

  char nodename[80];
  char dudename[10];
  static int  counter = 0;

  // init to standing with no velocity 
  status = Appearance_LifeForm_UprightStill;
  hull = jadehull;

  // initialize dude to not be in seen and to be alive
  displaying_model = TRUE;
  dead = FALSE;

  // highest LOD dude has nine articulated joints
  num_joints = 9;
 
  // initilize walking components
  ArmBy = 0.0f;
  DeltaArmBy = 2.0f;
  LegBy = 0.0f; 
  calfright = 0.0f; 
  calfleft = 0.0f; 
  first_step = 1;
  oldspeed = 0.0f;

  // initialize ranges for all three LOD models
  medLODrange    = 50;
  medfarLODrange = 100;
  farLODrange    = 250;

  // get channel to allow intersection testing later
  chan = (CHANNEL_DATA *)G_dynamic_data->pipe[0].channel;

  // attach culling point to dude at waist at proper X, Y, and Z
  pfSetVec3(pt, posture.xyz[X], posture.xyz[Y], posture.xyz[Z]+0.8);
  
  // clone DCS nodes so each dude can move independently
  sprintf(dudename, "otherd%d", counter);
  counter++;

  body = pfGetChild(whichSwitch, 1);

  pfNodeName(body, dudename);

  // attach _DCS nodes to cloned structure
  sprintf(nodename, "%s/main_DCS", dudename);
  main_DCS       = pfFindDCS(nodename);

  sprintf(nodename, "%s/far_body_DCS", dudename);
  far_body_DCS       = pfFindDCS(nodename);

  sprintf(nodename, "%s/head_DCS", dudename);
  head_DCS       = pfFindDCS(nodename);

  sprintf(nodename, "%s/armright_DCS", dudename);
  armright_DCS   = pfFindDCS(nodename);

  sprintf(nodename, "%s/armleft_DCS", dudename);
  armleft_DCS    = pfFindDCS(nodename);

  sprintf(nodename, "%s/armrightlow_DCS", dudename);
  armrightlow_DCS = pfFindDCS(nodename);

  sprintf(nodename, "%s/armleftlow_DCS", dudename);
  armleftlow_DCS  = pfFindDCS(nodename);

  sprintf(nodename, "%s/legright_DCS", dudename);
  legright_DCS   = pfFindDCS(nodename);

  sprintf(nodename, "%s/legleft_DCS", dudename);
  legleft_DCS    = pfFindDCS(nodename);

  sprintf(nodename, "%s/legrightlow_DCS", dudename);
  legrightlow_DCS = pfFindDCS(nodename);

  sprintf(nodename, "%s/legleftlow_DCS", dudename);
  legleftlow_DCS  = pfFindDCS(nodename);

  sprintf(nodename, "%s/sw_SW", dudename);
  sw_SW = pfFindSwitch(nodename);

  sprintf(nodename, "%s/sw1_SW", dudename);
  sw1_SW = pfFindSwitch(nodename);

  sprintf(nodename, "%s/sw2_SW", dudename);
  sw2_SW = pfFindSwitch(nodename);

  sprintf(nodename, "%s/sw3_SW", dudename);
  sw3_SW = pfFindSwitch(nodename);

  sprintf(nodename, "%s/sw4_SW", dudename);
  sw4_SW = pfFindSwitch(nodename);
}

void DUDECLASS::changestatus(unsigned int stat) {

  // change the appearance status
  status = stat;
  if (status & Appearance_DamageDestroyed) 
     pfSetVec3(velocity,0.0f,0.0f,0.0f);
}

void DUDECLASS::create_entity(int atype, ForceID the_force) {

  //fill in the fields that the constuctor does not
  char nodename[80];
  char dudename[5];
  static int  counter = 0;

  // set force to either friend or enemy
  force = the_force;
  
  // clone DCS nodes so each dude can move independently
  sprintf(dudename, "d%d", counter);
  counter++;

  body = pfGetChild(models, 0);

  pfNodeName(body, dudename);

  // attach _DCS nodes to cloned structure
  sprintf(nodename, "%s/main_DCS", dudename);
  main_DCS       = pfFindDCS(nodename);

  sprintf(nodename, "%s/far_body_DCS", dudename);
  far_body_DCS       = pfFindDCS(nodename);

  sprintf(nodename, "%s/head_DCS", dudename);
  head_DCS       = pfFindDCS(nodename);

  sprintf(nodename, "%s/armright_DCS", dudename);
  armright_DCS   = pfFindDCS(nodename);

  sprintf(nodename, "%s/armleft_DCS", dudename);
  armleft_DCS    = pfFindDCS(nodename);

  sprintf(nodename, "%s/armrightlow_DCS", dudename);
  armrightlow_DCS = pfFindDCS(nodename);

  sprintf(nodename, "%s/armleftlow_DCS", dudename);
  armleftlow_DCS  = pfFindDCS(nodename);

  sprintf(nodename, "%s/legright_DCS", dudename);
  legright_DCS   = pfFindDCS(nodename);

  sprintf(nodename, "%s/legleft_DCS", dudename);
  legleft_DCS    = pfFindDCS(nodename);

  sprintf(nodename, "%s/legrightlow_DCS", dudename);
  legrightlow_DCS = pfFindDCS(nodename);

  sprintf(nodename, "%s/legleftlow_DCS", dudename);
  legleftlow_DCS  = pfFindDCS(nodename);

  sprintf(nodename, "%s/sw_SW", dudename);
  sw_SW = pfFindSwitch(nodename);

  sprintf(nodename, "%s/sw1_SW", dudename);
  sw1_SW = pfFindSwitch(nodename);

  sprintf(nodename, "%s/sw2_SW", dudename);
  sw2_SW = pfFindSwitch(nodename);

  sprintf(nodename, "%s/sw3_SW", dudename);
  sw3_SW = pfFindSwitch(nodename);

  sprintf(nodename, "%s/sw4_SW", dudename);
  sw4_SW = pfFindSwitch(nodename);
}


void DUDECLASS::draw() {

  //get Boolean to see if cull pt. is in viwer's frustrum or not
  cull = pfSphereIsectFrust(Sphere, (pfFrustum*) chan); 

  // based on Boolean draw or not
  if(!cull) {

    if (displaying_model) {

      pfSwitchVal(sw_SW, PFSWITCH_OFF);
      displaying_model = FALSE;
    }
  }
  else {
      
    if (!displaying_model)
      displaying_model = TRUE;
  }
}

movestat DUDECLASS::move() {

  //move routine for driven vehicle
  pfVec3 oldpos;
  int hitSomething = FALSE;
  static int old_tethered;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tDUDE::move" << endl;
#endif

   update_time ();
   collect_inputs ();

#ifdef TRACE
   cerr << "Moving a DUDE\n";
#endif

   initMove(old_tethered,tethered);
   pfCopyVec3(oldpos,posture.xyz);

   if (!(paused || tethered || cdata.rigid_body) ) {

      if (!dead) {

        determineSpeed(cdata,input_control);

        //move the GROUND vehicle type in the x-y plane
        posture.xyz[X] += velocity[X]*cdata.delta_time;
        posture.xyz[Y] += velocity[Y]*cdata.delta_time;
      }

      determinePosture(cdata,oldpos,hitSomething);
      if(hitSomething) {
         return(HITSOMETHING);
      }

   } // End !paused !tethered !rigid_body

   // If tethered, let the other vehicle calculate our position for us
   if (tethered) 
     handleTether(cdata,tethered);

   // attach cull point to dude's position 
   attach_pt();

   // check to see if cull point is in viewers frustrum
   drawpt();

   // calculate where dude is at with respect to viewer's frustrum
   calc_dist();

   // set appearance
   dude_appear();

   if ((cull) && (distance <= (260 + Jack_offset))) {

     computeFirepoint();

     checkUnderground(tethered);

     check_targets ( posture.xyz, posture.hpr, look.hpr );

     //put the vehicle in the correct location in space
     placeVehicle();

     // draw correct dude LOD model
     dude_switch();
   }

   return(FINEWITHME);
} 


movestat DUDECLASS::moveDR(float deltatime,float curtime) {

   //move routine for DR vehicles simple move
   float speed;

#ifdef TRACE
   cerr << "Dead Recon a PLAIN DI.\n";
#endif

   if(((curtime - lastPDU) > G_static_data->DIS_deactivate_delta ) && 
      (vid != G_drivenveh)) { 
      //Deactivate the puppy
      return(DEACTIVE);
   }

   computeDR(deltatime);

   if (vid != G_drivenveh){

     // attach cull point to dude's position 
     attach_pt();

     // check to see if cull point is in viewers frustrum
     drawpt();

     // set appearance
     dude_DR(JointVals[HEAD_H], JointVals[HEAD_P]);

     // calculate where dude is at with respect to viewer's frustrum
     calc_dist();

     if ((cull) && (distance <= (260 + Jack_offset))) {

       // Update DR entity 
       posture = drpos;

       //put the vehicle in the correct location in space
       placeVehicle();

       // draw correct dude LOD model
       dude_switch();
     }
   }
   else if ((!unitMember) &&
      (!G_static_data->stealth)) { // sendentitystate() only if G_drivenveh
      speed = pfLengthVec3(velocity);

      // This is us, have we exceeded the DR error or time limit?
      if((G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) || 
         (delta_send(&posture,&drpos))) 
        time2Send(speed); 
      else 
        reason2Send(speed);

      oldstatus = status;

   } // End if not G_drivenveh and not stealth

   return(FINEWITHME);
}

void DUDECLASS::dude_appear() {

  // make sure dude is in managing range
  if (distance <= (260 + Jack_offset)) {

    // set head and eyes to correct position
    rot_elev_head();

    // sort out what the status should be based on speed and current posture
    if(((status & Appearance_LifeForm_State_Mask) == Appearance_LifeForm_Prone)
       || ((status & Appearance_LifeForm_State_Mask) 
       == Appearance_LifeForm_Crawling)) {

      if( speed == 0.0f)  {

        if((status & Appearance_Damage_Mask) 
            == Appearance_DamageDestroyed) { 
          status = Appearance_DamageDestroyed;
          dude_dead();
        }
        else {
          status = Appearance_LifeForm_Prone;
          dude_prone();
        }
      }
      else {

        if((status & Appearance_Damage_Mask) 
          == Appearance_DamageDestroyed) { 
          status = Appearance_DamageDestroyed;
          dude_dead();
        }
        else {
          status = Appearance_LifeForm_Crawling; 
          dude_prone();
        }
      }
    }
    else if((status & Appearance_LifeForm_State_Mask) 
           != Appearance_LifeForm_Kneeling) {
     
      if(speed == 0.0f) {

        if((status & Appearance_Damage_Mask) 
          == Appearance_DamageDestroyed) { 
          status = Appearance_DamageDestroyed;
          dude_dead();
        }
        else {
          status = Appearance_LifeForm_UprightStill;
          dude_stand();
        }
      }
      else if(fabs(speed) <= 3.0f) {

        if((status & Appearance_Damage_Mask) 
          == Appearance_DamageDestroyed) { 
          status = Appearance_DamageDestroyed;
          dude_dead();
        }
        else {
          status = Appearance_LifeForm_UprightWalking;
          dude_walk();
        }
      } 
      else if(speed > 3.0f) {

        if((status & Appearance_Damage_Mask) 
          == Appearance_DamageDestroyed) { 
          status = Appearance_DamageDestroyed;
          dude_dead();
        }
        else {
          status = Appearance_LifeForm_UprightRunning;
          dude_walk();
        }
      }
    }
    else if((status & Appearance_LifeForm_State_Mask) 
           == Appearance_LifeForm_Kneeling) {

      if((status & Appearance_Damage_Mask) 
        == Appearance_DamageDestroyed) { 
        status = Appearance_DamageDestroyed;
        dude_dead();
      }
      else {
        status = Appearance_LifeForm_Kneeling;
        dude_kneel();
      }
    }
  }
}

void DUDECLASS::dude_DR(float head_dir, float head_pit) {

  // make sure dude is in managing range
  if (distance <= (260.0f + Jack_offset)) {

    //set head to correct position   
    rot_elev_headDR(head_dir, head_pit);
 
    // sort out what the status should be based on speed and current posture
    if(((status & Appearance_LifeForm_State_Mask) == Appearance_LifeForm_Prone)
       || ((status & Appearance_LifeForm_State_Mask) 
       == Appearance_LifeForm_Crawling)) {

      if(speed == 0.0f)  {

        if((status & Appearance_Damage_Mask) 
          == Appearance_DamageDestroyed) { 
          status = Appearance_DamageDestroyed;
          dude_deadDR();
        }
        else {
          status = Appearance_LifeForm_Prone;
          dude_prone();
        }
      }
      else {

        if((status & Appearance_Damage_Mask) 
          == Appearance_DamageDestroyed) { 
          status = Appearance_DamageDestroyed;
          dude_deadDR();
        }
        else {
          status = Appearance_LifeForm_Crawling; 
          dude_prone();
        }
      }
    }
    else if((status & Appearance_LifeForm_State_Mask) 
           != Appearance_LifeForm_Kneeling) {
     
      if(pfLengthVec3(velocity) == 0.0f) {

        if((status & Appearance_Damage_Mask) 
          == Appearance_DamageDestroyed) { 
          status = Appearance_DamageDestroyed;
          dude_deadDR();
        }
        else {
          status = Appearance_LifeForm_UprightStill;
          dude_stand();
        }
      }
      else if(fabs(pfLengthVec3(velocity)) <= 3.0f) {

        if((status & Appearance_Damage_Mask) 
          == Appearance_DamageDestroyed) { 
          status = Appearance_DamageDestroyed;
          dude_deadDR();
        }
        else {
          status = Appearance_LifeForm_UprightWalking;
          dude_walk();
        }
      } 
      else if(pfLengthVec3(velocity) > 3.0f) {

        if((status & Appearance_Damage_Mask) 
          == Appearance_DamageDestroyed) { 
          status = Appearance_DamageDestroyed;
          dude_deadDR();
        }
        else {
          status = Appearance_LifeForm_UprightRunning;
          dude_walk();
        }
      }
    }
    else if((status & Appearance_LifeForm_State_Mask) 
           == Appearance_LifeForm_Kneeling) {

      if((status & Appearance_Damage_Mask) 
        == Appearance_DamageDestroyed) { 
        status = Appearance_DamageDestroyed;
        dude_deadDR();
      }
      else {
        status = Appearance_LifeForm_Kneeling;
        dude_kneel();
      }
    }
  } 
}

void DUDECLASS::drawpt() {

  // get Boolean to see is pt is in viewer's frustum or not
  cull = pfPtInFrust(pt, (pfFrustum*) chan); 

  // based on Boolean draw or not
  if(!cull){  

    if (displaying_model) {
      pfSwitchVal(sw_SW, PFSWITCH_OFF);
      displaying_model = FALSE;
    }
  }
  else{
      
    if (!displaying_model)
      displaying_model = TRUE;
  }
}

void DUDECLASS::attach_pt() {

  // move the attached point
  pfSetVec3(pt, posture.xyz[X], posture.xyz[Y], posture.xyz[Z]+0.8);
}

void DUDECLASS::calc_dist() {

  // calculate distance between viewer's eyepoint & dude
  distance =  pfDistancePt3(chan->view.xyz, posture.xyz);
}
 
void DUDECLASS::dude_switch() {

  // decide what dude to draw and set switches to get that model  
  if (distance > farLODrange)  
    pfSwitchVal(sw_SW, PFSWITCH_OFF);
  else if(distance <= farLODrange && distance > medfarLODrange) 
    pfSwitchVal(sw_SW, 1);
   
  else if(distance <= medfarLODrange) {

    if (distance < medLODrange) {

      pfSwitchVal(sw_SW, 0);
      pfSwitchVal(sw1_SW, 0);
      pfSwitchVal(sw2_SW, 0);
      pfSwitchVal(sw3_SW, 0);
      pfSwitchVal(sw4_SW, 0);
    }
    else if (distance >= medLODrange) {

      pfSwitchVal(sw_SW, 0);
      pfSwitchVal(sw1_SW, 1);
      pfSwitchVal(sw2_SW, 1);
      pfSwitchVal(sw3_SW, 1);
      pfSwitchVal(sw4_SW, 1);
    }
  }    
}

void DUDECLASS::dude_walk() {

  static int counter = 0;
  
  // make dude walk if in viewer's frustrum
  if (cull) {

  // based on user velocity calculate a good dude speed  
  speed = pfLengthVec3(velocity)/2.0f + 2.0f;

  // reset joints for new speed
  if (oldspeed != speed) {
    //right arm
    pfDCSRot(armright_DCS, 0.0f, 0.0f, 0.0f);
    pfDCSTrans(armright_DCS, 0.4f, -0.12f, 0.8f); 
    pfDCSRot(armrightlow_DCS, 0.0f, 0.0f, 0.0f); 
    pfDCSTrans(armrightlow_DCS, 0.0f, 0.0f, -0.5f);
     
    //left arm
    pfDCSRot(armleft_DCS, 0.0f, 0.0f, 0.0f);
    pfDCSTrans(armleft_DCS, -0.1f, -0.12f, 0.8f); 
    pfDCSRot(armleftlow_DCS, 0.0f, 0.0f, 0.0f);
    pfDCSTrans(armleftlow_DCS, 0.0f, 0.0f, -0.5f);

    //right leg
    pfDCSRot(legright_DCS, 180.0f, 0.0f, 0.0f);  
    pfDCSTrans(legright_DCS, 0.3f, -0.12f, 0.0f); 
    pfDCSRot(legrightlow_DCS, 0.0f, 0.0f, 0.0f);  
    pfDCSTrans(legrightlow_DCS, 0.0f, 0.0f, -0.45f);

    //left leg
    pfDCSRot(legleftlow_DCS, 0.0f, 0.0f, 0.0f);
    pfDCSRot(legleft_DCS, 0.0f, 0.0f, 0.0f);
    pfDCSTrans(legleft_DCS, 0.1f, -0.12f, 0.0f);
    pfDCSTrans(legleftlow_DCS, 0.0f, 0.0f, -0.45f);
  }

  // make sure dude is not far_dude which has no limbs
  if (distance <= medfarLODrange) {

    // torso
    pfDCSTrans(main_DCS, 0.0f, 0.0f, 1.0f); 
    pfDCSRot(main_DCS, 0.0f, 0.0f, 0.0f);

    // master variables to control algorithm
    if (ArmBy > 45.0)
      DeltaArmBy = -2.0f * speed;
     
    if (ArmBy < -45.0) 
      DeltaArmBy = 2.0f * speed;

    ArmBy = ArmBy + DeltaArmBy;
    LegBy = ArmBy/2.0f;
    
    // arm motion - upper and lower
    if ((ArmBy > 0.0f) && (DeltaArmBy > 0.0f)) { 

      pfDCSRot(armleft_DCS, 0.0f, ArmBy*0.66f, 0.0f);
      pfDCSRot(armleftlow_DCS, 0.0f, ArmBy*0.75f, 0.0f); 
      pfDCSRot(armright_DCS, 0.0f, -ArmBy*0.33f, 0.0f);
      pfDCSRot(armrightlow_DCS, 0.0f, 0.0f, 0.0f);
    }
    else if ((ArmBy > 0.0f) && (DeltaArmBy < 0.0f)) {   

      pfDCSRot(armleft_DCS, 0.0f, ArmBy*0.66f, 0.0f);
      pfDCSRot(armleftlow_DCS, 0.0f, ArmBy*0.75f, 0.0f); 
      pfDCSRot(armright_DCS, 0.0f, -ArmBy*0.33f, 0.0f);
      pfDCSRot(armrightlow_DCS, 0.0f, 0.0f, 0.0f);
    }
    else if ((ArmBy <= 0.0f) && (DeltaArmBy > 0.0f)) {    

      pfDCSRot(armleft_DCS, 0.0f, ArmBy*0.33f, 0.0f);
      pfDCSRot(armleftlow_DCS, 0.0f, 0.0f, 0.0f); 
      pfDCSRot(armright_DCS, 0.0f, -ArmBy*0.66f, 0.0f);
      pfDCSRot(armrightlow_DCS, 0.0f, -ArmBy*0.75f, 0.0f);
    }  
    else if ((ArmBy <= 0.0f) && (DeltaArmBy < 0.0f)) {  

      pfDCSRot(armleft_DCS, 0.0f, ArmBy*0.33f, 0.0f);
      pfDCSRot(armleftlow_DCS, 0.0f, 0.0f, 0.0f); 
      pfDCSRot(armright_DCS, 0.0f, -ArmBy*0.66f, 0.0f);
      pfDCSRot(armrightlow_DCS, 0.0f, -ArmBy*0.75f, 0.0f);
    }

    // leg motion upper and lower 
    if ((LegBy > 0.0f) && (LegBy <= 15.0f) && 
        (DeltaArmBy > 0.0f)) { 

      if (first_step) {

        calfright = calfright - 2.0f;
        pfDCSRot(legright_DCS, 0.0f, LegBy, 0.0f);
        pfDCSRot(legrightlow_DCS, 0.0f, calfright, 0.0f); 
      }
      else {

        calfright = calfright + 0.741935f;
        pfDCSRot(legright_DCS, 0.0f, LegBy*0.66f, 0.0f);
        pfDCSRot(legrightlow_DCS, 0.0f, calfright, 0.0f); 
      }

      calfleft = calfleft - 1.0f;
      pfDCSRot(legleft_DCS, 0.0f, -LegBy*0.66f, 0.0f);
      pfDCSRot(legleftlow_DCS, 0.0f, calfleft, 0.0f);
    }
    else if ((LegBy > 15.0f) && (DeltaArmBy > 0.0f)) {     

      if (first_step) {

        calfright = calfright + 3.75f;
        pfDCSRot(legright_DCS, 0.0f, LegBy, 0.0f);
        pfDCSRot(legrightlow_DCS, 0.0f, calfright, 0.0f); 
            
        if (calfright == 0.0f)
          first_step = 0;
      }
      else {

        calfright = calfright + 0.741935f;
        pfDCSRot(legright_DCS, 0.0f, LegBy*0.66f, 0.0f);
        pfDCSRot(legrightlow_DCS, 0.0f, calfright, 0.0f); 
      }

      calfleft = calfleft - 1.0f;
      pfDCSRot(legleft_DCS, 0.0f, -LegBy*0.66f, 0.0f);
      pfDCSRot(legleftlow_DCS, 0.0f, calfleft, 0.0f);
    }
    else if ((LegBy >= 7.5f)  && (DeltaArmBy < 0.0f)) {     

      pfDCSRot(legright_DCS, 0.0f, LegBy, 0.0f);
      pfDCSRot(legrightlow_DCS, 0.0f, 0.0f, 0.0f); 
      pfDCSRot(legleft_DCS, 0.0f, -LegBy*0.66f, 0.0f);
      pfDCSRot(legleftlow_DCS, 0.0f, calfleft, 0.0f);
    }  
    else if ((LegBy >= 0.0f) && (LegBy < 7.5f) && 
             (DeltaArmBy < 0.0f)) {     

      calfleft = calfleft + 0.741935f;
      pfDCSRot(legright_DCS, 0.0f, LegBy, 0.0f);
      pfDCSRot(legrightlow_DCS, 0.0f, 0.0f, 0.0f); 
      pfDCSRot(legleft_DCS, 0.0f, -LegBy*0.66f, 0.0f);
      pfDCSRot(legleftlow_DCS, 0.0f, calfleft, 0.0f);
    }  
    else if ((LegBy < 0.0f) && (LegBy >= -15.0f) && 
             (DeltaArmBy < 0.0f)) {     

      calfleft = calfleft + 0.741935f;
      calfright = calfright - 1.0f;
      pfDCSRot(legright_DCS, 0.0f, LegBy*0.66f, 0.0f);
      pfDCSRot(legrightlow_DCS, 0.0f, calfright, 0.0f); 
      pfDCSRot(legleft_DCS, 0.0f, -LegBy, 0.0f);
      pfDCSRot(legleftlow_DCS, 0.0f, calfleft, 0.0f);
    }
    else if ((LegBy < -15.0f) && (DeltaArmBy < 0.0f)) {     
 
      calfleft = calfleft + + 0.741935f;
      calfright = calfright - 1.0f;
      pfDCSRot(legright_DCS, 0.0f, LegBy*0.66f, 0.0f);
      pfDCSRot(legrightlow_DCS, 0.0f, calfright, 0.0f); 
      pfDCSRot(legleft_DCS, 0.0f, -LegBy, 0.0f);
      pfDCSRot(legleftlow_DCS, 0.0f, calfleft, 0.0f);
    }
    else if ((LegBy <= -7.5f) && (DeltaArmBy > 0.0f)) {     

      pfDCSRot(legright_DCS, 0.0f, LegBy*0.66, 0.0f);
      pfDCSRot(legrightlow_DCS, 0.0f, calfright, 0.0f); 
      pfDCSRot(legleft_DCS, 0.0f, -LegBy, 0.0f);
      pfDCSRot(legleftlow_DCS, 0.0f, 0.0f, 0.0f);
    }
    else if ((LegBy > -7.5f) && (LegBy <= 0.0f) && 
             (DeltaArmBy > 0.0f)) { 
      calfright = calfright + 0.741935f;    
      pfDCSRot(legright_DCS, 0.0f, LegBy*0.66f, 0.0f);
      pfDCSRot(legrightlow_DCS, 0.0f, calfright, 0.0f); 
      pfDCSRot(legleft_DCS, 0.0f, -LegBy, 0.0f);
      pfDCSRot(legleftlow_DCS, 0.0f, 0.0f, 0.0f);
    }
  }

  // keep track of speed so next time through we can see if it changed  
  oldspeed = speed;

  // reset limbs to not accumulate errors
  if (counter == 1000) {
    oldspeed = 99;
    counter = 0;

    // re-initilize walking components
    ArmBy = 0.0f;
    DeltaArmBy = 2.0f;
    LegBy = 0.0f; 
    calfright = 0.0f; 
    calfleft = 0.0f; 
    first_step = 1;
  }

  // reset dude to be alive and increment counter
  dead = FALSE;
  counter++;
  }
}

void DUDECLASS::dude_stand() {

  //right arm
  pfDCSRot(armright_DCS, 0.0f, 0.0f, 0.0f);
  pfDCSTrans(armright_DCS, 0.4f, -0.12f, 0.8f); 
  pfDCSRot(armrightlow_DCS, 0.0f, 0.0f, 0.0f); 
  pfDCSTrans(armrightlow_DCS, 0.0f, 0.0f, -0.5f);
     
  //left arm
  pfDCSRot(armleft_DCS, 0.0f, 0.0f, 0.0f);
  pfDCSTrans(armleft_DCS, -0.1f, -0.12f, 0.8f); 
  pfDCSRot(armleftlow_DCS, 0.0f, 0.0f, 0.0f);
  pfDCSTrans(armleftlow_DCS, 0.0f, 0.0f, -0.5f);
     
  //right leg
  pfDCSRot(legright_DCS, 180.0f, 0.0f, 0.0f);  
  pfDCSTrans(legright_DCS, 0.3f, -0.12f, 0.0f); 
  pfDCSRot(legrightlow_DCS, 0.0f, 0.0f, 0.0f);  
  pfDCSTrans(legrightlow_DCS, 0.0f, 0.0f, -0.45f);

  //left leg
  pfDCSRot(legleftlow_DCS, 0.0f, 0.0f, 0.0f);
  pfDCSRot(legleft_DCS, 0.0f, 0.0f, 0.0f);
  pfDCSTrans(legleft_DCS, 0.1f, -0.12f, 0.0f);
  pfDCSTrans(legleftlow_DCS, 0.0f, 0.0f, -0.45f);

  // torso
  pfDCSTrans(main_DCS, 0.0f, 0.0f, 1.0f); 
  pfDCSRot(main_DCS, 0.0f, 0.0f, 0.0f);

  // re-initilize walking components
  ArmBy = 0.0f;
  DeltaArmBy = 2.0f;
  LegBy = 0.0f; 
  calfright = 0.0f; 
  calfleft = 0.0f; 
  first_step = 1;

  // reset dude to be alive
  dead = FALSE;
}

void DUDECLASS::dude_kneel() {

  //right arm      
  pfDCSRot(armright_DCS, 0.0f, 45.0f, 0.0f);
  pfDCSTrans(armright_DCS, 0.4f, -0.05f, 0.75f); 
  pfDCSRot(armrightlow_DCS, 0.0f, 90.0f, 0.0f); 
    
  //left arm
  pfDCSRot(armleft_DCS, 0.0f, -70.0f, 0.0f);
  pfDCSTrans(armleft_DCS, -0.1f, 0.0f, 0.75f); 
  pfDCSRot(armleftlow_DCS, 0.0f, 70.0f, 0.0f);
        
  if (distance < medLODrange) {

    //right leg
    pfDCSTrans(legright_DCS, 0.3f, 0.0f, 0.0f); 
    pfDCSRot(legright_DCS, 0.0f, 89.0f, 0.0f);  
    pfDCSRot(legrightlow_DCS, 0.0f, -90.0f, 0.0f);  

    //left leg
    pfDCSRot(legleftlow_DCS, 0.0f, -90.0f, 0.0f);
    pfDCSRot(legleft_DCS, 0.0f, 0.0f, 0.0f);
    pfDCSTrans(legleft_DCS, 0.1f, -0.12f, 0.0f);
  }
  else if (distance >= medLODrange) {

    //right leg
    pfDCSTrans(legright_DCS, 0.3f, 0.0f, 0.0f); 
    pfDCSRot(legright_DCS, 0.0f, 45.0f, 0.0f);  

    //left leg
    pfDCSRot(legleft_DCS, 0.0f, -45.0f, 0.0f);
    pfDCSTrans(legleft_DCS, 0.1f, -0.12f, 0.0f);
  }

  // torso
  pfDCSTrans(main_DCS, 0.0f, 0.0f, 0.5f); 
  pfDCSRot(main_DCS, 0.0f, 0.0f, 0.0f);

  // reset dude to be alive
  dead = FALSE;
}

void DUDECLASS::dude_prone() {

  // right arm
  pfDCSRot(armright_DCS, 45.0f, 180.0f, 0.0f);
  pfDCSTrans(armright_DCS, 0.4f, -0.13f, 0.75f); 
  pfDCSRot(armrightlow_DCS, 0.0f, 0.0f, 0.0f); 
        
  // left arm
  pfDCSRot(armleft_DCS, 0.0f, 0.0f, 95.0f);
  pfDCSTrans(armleft_DCS, 0.0f, -0.1f, 0.75f); 
  pfDCSRot(armleftlow_DCS, 0.0f, 0.0f, 95.0f);
       
  // right leg
  pfDCSRot(legright_DCS, 0.0f, 0.0f, 0.0f);  
  pfDCSTrans(legright_DCS, 0.3f, -0.12f, 0.0f); 
  pfDCSRot(legrightlow_DCS, 0.0f, 0.0f, 0.0f);
  
  // left leg 
  pfDCSRot(legleft_DCS, 0.0f, 0.0f, 40.0f);
  pfDCSTrans(legleft_DCS, 0.0f, -0.12f, 0.0f);
  pfDCSRot(legleftlow_DCS, 0.0f, 0.0f, -50.0f);

  // torso
  pfDCSTrans(main_DCS, 0.0f, 0.0f, 0.0f); 
  pfDCSRot(main_DCS, 0.0f, -90.0f, 0.0f);

  // reset dude to be alive
  dead = FALSE;
}

void DUDECLASS::dude_dead() {

  //set gaze to seeing the stars    
  look.hpr[HEADING] = 0.0;
  look.hpr[PITCH] = 90.0;
  look.xyz[X] = 0.2;
  look.xyz[Y] = 0.2;
  look.xyz[Z] = 0.2;
 
  //set to dead    
  dead = TRUE;
 
  // right arm
  pfDCSRot(armright_DCS, 0.0f, 0.0f, -25.0f);
  pfDCSRot(armrightlow_DCS, 0.0f, 0.0f, -75.0f); 
  pfDCSTrans(armright_DCS, 0.35f, -0.12f, 0.72f); 

  // left arm
  pfDCSRot(armleft_DCS, 0.0f, 0.0f, 45.0f);
  pfDCSRot(armleftlow_DCS, 0.0f, 0.0f, -90.0f);
  pfDCSTrans(armleft_DCS, 0.0f, -0.12f, 0.8f); 

  // right leg
  pfDCSTrans(legright_DCS, 0.3f, -0.12f, 0.0f); 
  pfDCSRot(legright_DCS, 0.0f, 0.0f, -25.0f);  
  pfDCSRot(legrightlow_DCS, 0.0f, 0.0f, 0.0f);

  // left leg 
  pfDCSTrans(legleft_DCS, 0.1f, -0.12f, 0.0f); 
  pfDCSRot(legleft_DCS, 0.0f, 0.0f, 45.0f);
  pfDCSRot(legleftlow_DCS, 0.0f, 0.0f, -75.0f);

  // torso
  pfDCSTrans(main_DCS, 0.0f, 0.0f, 0.0f); 
  pfDCSRot(main_DCS, -20.0f, -90.0f, 180.0f);

  // head
  pfDCSRot(head_DCS, 180.0f, 0.0f, 0.0f);
  pfDCSTrans(head_DCS, 0.30f, -0.25f, 0.8f); 
}
 
void DUDECLASS::rot_elev_head() {

  // based on user inputs figure out how to draw head
  direction = float(look.hpr[HEADING]);
  elevation = float(look.hpr[PITCH]);

  // put head back to level after being reincarnated
  if (dead)
    look.hpr[PITCH] = 0.0;
    
  // set realistic limits for head movement
  if (look.hpr[HEADING] > 75.0f)
    direction = 75.0f;
  if (look.hpr[HEADING] < -75.0f)
     direction = -75.0f;

  if (look.hpr[PITCH] > 45.0f)
    elevation = 45.0f;
  if (look.hpr[PITCH] < -45.0f)
    elevation = -45.0f;

  if ((status == Appearance_LifeForm_Prone) ||
      (status == Appearance_LifeForm_Crawling)) {
 
    // can't let eyes go below ground
    if (look.hpr[PITCH] < -10.0f)
      elevation = -10.0f;
    if (look.hpr[HEADING] > 45.0f)
      direction = 45.0f;
    if (look.hpr[HEADING] < -45.0f)
      direction = -45.0f;
  }

  // set realistic limits for eye movement
  if (look.hpr[HEADING] > 90.0f)
    look.hpr[HEADING] = 90.0f;   
  if (look.hpr[HEADING] < -90.0f)
    look.hpr[HEADING] = -90.0f;   

  if (look.hpr[PITCH] > 90.0f)
    look.hpr[PITCH] = 90.0f;
  if (look.hpr[PITCH] < -90.0f)
    look.hpr[PITCH] = -90.0f;

  if ((status == Appearance_LifeForm_Prone) ||
      (status == Appearance_LifeForm_Crawling)) {

    // limit eye movement in prone
    if (look.hpr[HEADING] > 45.0f)
      look.hpr[HEADING] = 45.0f;   
    if (look.hpr[HEADING] < -45.0f)
      look.hpr[HEADING] = -45.0f;   

    if (look.hpr[PITCH] > 45.0f)
      look.hpr[PITCH] = 45.0f;
    if (look.hpr[PITCH] < -10.0f)
      look.hpr[PITCH] = -10.0f;
  }

  // rotate the head as user input dictates
  pfDCSRot(head_DCS, direction, elevation, 0.0f); 

  // keep head near torso while manipulating it
  if (direction <= 0.0f) 
    pfDCSTrans(head_DCS, 0.0625f-(0.0025f*direction), 0.0f, 0.8f+(0.0010f*elevation));
  else 
    pfDCSTrans(head_DCS, 0.0625f, -(0.0025f*direction), 0.8f+(0.0010f*elevation));

  // change point looking from to move with head
  if ((status == Appearance_LifeForm_UprightStill) ||
      (status == Appearance_LifeForm_UprightWalking) ||
      (status == Appearance_LifeForm_UprightRunning)) {
    look.xyz[X] = 0.2;
    look.xyz[Y] = 0.17;
    look.xyz[Z] = 2.0;
  }
  else if ((status == Appearance_LifeForm_Crawling) ||
           (status == Appearance_LifeForm_Prone)) {
    look.xyz[X] = 0.2;
    look.xyz[Y] = 2.1;
    look.xyz[Z] = 0.1;
  }
  else if (status == Appearance_LifeForm_Kneeling) {
    look.xyz[X] = 0.2;
    look.xyz[Y] = 0.17;
    look.xyz[Z] = 1.5; 
  }
  
  JointVals[HEAD_H] = direction;
  JointVals[HEAD_P] = elevation;
}

void DUDECLASS::dude_deadDR() {

  // set to dead     
  dead = TRUE;
 
  // right arm
  pfDCSRot(armright_DCS, 0.0f, 0.0f, -25.0f);
  pfDCSRot(armrightlow_DCS, 0.0f, 0.0f, -75.0f); 
  pfDCSTrans(armright_DCS, 0.35f, -0.12f, 0.72f); 

  // left arm
  pfDCSRot(armleft_DCS, 0.0f, 0.0f, 45.0f);
  pfDCSRot(armleftlow_DCS, 0.0f, 0.0f, -90.0f);
  pfDCSTrans(armleft_DCS, 0.0f, -0.12f, 0.8f); 

  // right leg
  pfDCSTrans(legright_DCS, 0.3f, -0.12f, 0.0f); 
  pfDCSRot(legright_DCS, 0.0f, 0.0f, -25.0f);  
  pfDCSRot(legrightlow_DCS, 0.0f, 0.0f, 0.0f);

  // left leg 
  pfDCSTrans(legleft_DCS, 0.1f, -0.12f, 0.0f); 
  pfDCSRot(legleft_DCS, 0.0f, 0.0f, 45.0f);
  pfDCSRot(legleftlow_DCS, 0.0f, 0.0f, -75.0f);

  // head
  pfDCSTrans(head_DCS, 0.0625f, 0.0f, .8f); 
  pfDCSRot(head_DCS, 0.0f, 0.0f, 0.0f);

  // torso
  pfDCSTrans(main_DCS, 0.0f, 0.0f, 0.2f); 
  pfDCSRot(main_DCS, -20.0f, -90.0f, 180.0f);

  // head
  pfDCSRot(head_DCS, 180.0f, 0.0f, 0.0f);
  pfDCSTrans(head_DCS, 0.30f, -0.25f, 0.8f); 
}

void DUDECLASS::rot_elev_headDR(float direction, float elevation) {

  // rotate the head as user input dictates
  pfDCSRot(head_DCS, direction, elevation, 0.0f); 

  // correct head direction to make it local 
  if (direction > 75.0f)
    direction = direction - 360.0f;

  // keep head near torso while manipulating it
  if (direction <= 0.0f) 
    pfDCSTrans(head_DCS, 0.0625f-(0.0025f*direction), 0.0f, 0.8f+(0.0010f*elevation));
  else 
    pfDCSTrans(head_DCS, 0.0625f, -(0.0025f*direction), 0.8f+(0.0010f*elevation));
}




