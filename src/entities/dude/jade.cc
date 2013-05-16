// File: <jade.cc>

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

#include "jade.h"
#include "human_const.h"
#include "common_defs2.h"
#include "appearance.h"
#include "disnetlib.h"
#include "netutil.h"
#include "terrain.h"
#include "conversion_const.h"
#include "collision_const.h"
#include "jack.h"

// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile STATIC_DATA     *G_static_data;
extern          VEH_LIST        *G_vehlist;
extern          VEHTYPE_LIST    *G_vehtype;
extern volatile int              G_drivenveh;
extern          DIS_net_manager *G_net;
extern          pfGroup         *G_moving;
extern          void             firebuttonhit(wdestype,int,PASS_DATA*); 

JADECLASS::JADECLASS(int id, int atype, ForceID the_force) 
: PERSON_VEH(id,atype,the_force){

  char jadename[10];
  char nodename[80];
  static int counter = 0;

  // clone jade node
  create_entity(atype, the_force);

  // set force to either friend or enemy
  force = the_force;

  // get channel to allow intersection testing later
  chan = (CHANNEL_DATA *)G_dynamic_data->pipe[0].channel;

  // clone switch node so each jade can move independently
  sprintf(jadename, "jaded%d", counter);
  counter++;

  body = pfGetChild(models, 0);

  pfNodeName(body, jadename);

  // find jade nodes from cloned structure and name them 
  sprintf(nodename, "%s/mainjade_GP", jadename);
  jade_GP       =  pfFindGroup(nodename);

  sprintf(nodename, "%s/whichSwitch", jadename);
  whichSwitch       = pfFindSwitch(nodename);
  pfSwitchVal((pfSwitch*)pfGetChild(whichSwitch,0),PFSWITCH_ON);
      
  // create dude & jack 
  jack = new JACK_DI_VEH(whichSwitch, hull, id, joint);
  dude = new DUDECLASS(hull, whichSwitch, id);

  jack->unitMember = TRUE;
  dude->unitMember = TRUE;

  // set range at which jack becomes dude
  dude->Jack_offset = JADERANGE;

  //apply range offset to existing dude ranges
  dude->medLODrange    = dude->medLODrange    + dude->Jack_offset;
  dude->medfarLODrange = dude->medfarLODrange + dude->Jack_offset;
  dude->farLODrange    = dude->farLODrange    + dude->Jack_offset;

  // initialize jade to standing and Jack
  status = Appearance_LifeForm_UprightStill;
  human = JACKHUMAN;
  switchHuman();

  // initialize num_joints to send out APRs
  num_joints = 2;

  delete JointVals;
  JointVals = jack->js->JointVals;
}

JADECLASS::~JADECLASS(){

  cout<<"Jade destructor called"<<endl;
  
  delete jack;
  delete dude;
}

void JADECLASS::switchHuman() {

  int        local = JACKHUMAN;
  float      distance;
  static int switcher = 0;

  if ((switcher%5) == 0) { 
  
    // calculate distance from view point to jade  
    distance =  pfDistancePt3(chan->view.xyz, posture.xyz);

    // assign 'local' to appropriate enum 
    if (distance>JADERANGE) {
  
      local = DUDEHUMAN;
      switcher++;    
      jack->immed_transition = TRUE;
    
      if(distance < JADERANGE*1.05f) {
      
        // start getting jack into position
        downloadData2Jack();
        float speed = pfLengthVec3(velocity);
        jack->determineStatus(speed);
        jack->SendPacket();
        CheckJackUpdate(jack->js,jack->upperJointMods,0.0f,0.0f,
          FALSE, FALSE);
      }
    }

    // see if jade switched between jack or dude, if so xfer data
    if (local != human) {  
    
      pfSwitchVal(whichSwitch, local); 
      downloadData();
      human = local;

      if(human == JACKHUMAN) {
        // stop snap posture changes and do transitions
        jack->immed_transition = FALSE;
      }
    }
  }
  else if ((switcher%5) != 0)
    switcher++;    

}

void JADECLASS::changestatus(unsigned int stat) {

  // change status for correct model
  if (human == DUDEHUMAN)
    dude->changestatus(stat);
  else
    jack->changestatus(stat);
  
  status = stat;

  // if jade is dead set velocity to zero 
  if (status & Appearance_DamageDestroyed) 
     pfSetVec3(velocity,0.0f,0.0f,0.0f);
}

void JADECLASS::entitystateupdate(EntityStatePDU *epdu, sender_info &sendinfo) {

  // update with appropriate model
  if (human == DUDEHUMAN)
    dude->entitystateupdate(epdu, sendinfo);
  else
    jack->entitystateupdate(epdu, sendinfo);
}

movestat JADECLASS::move() {

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tJADE_VEH::move" << endl;
#endif

#ifdef TRACE
   cerr << "Moving a JADE_VEH\n";
#endif

  // decide which model to display
  switchHuman();

  // call correct move for whatever model is shown
  if (human == DUDEHUMAN) {
    dude->move();
  }
  else {
    jack->move();
  }

  // get data from Jack/Dude and put into Jade  
  uploadData(); 
  
  return(FINEWITHME);
}  

movestat JADECLASS::moveDR(float deltatime, float curtime) {  

  // decide which model to display
  switchHuman();

  // call correct move for whatever model is shown
  if(vid != G_drivenveh) {
     if (human == DUDEHUMAN)  {
       dude->moveDR(deltatime, curtime);
     }
     else {
       jack->moveDR(deltatime, curtime);
     }

     // get data from Jack/Dude and put into Jade  
     uploadData();
  }
  else { // sendentitystate() only if G_drivenveh
     float speed = pfLengthVec3(velocity);

     // This is us, have we exceeded the DR error or time limit?
     if((G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) ||
        (delta_send(&posture,&drpos))) {
        time2Send(speed);
     }
     else {
        reason2Send(speed);
     }

     oldstatus = status;
  }
  return(FINEWITHME);
}      

void JADECLASS::uploadData() {

  // upload pertinent data to jade from correct model
  if (human == DUDEHUMAN) { 
    pfCopyVec3(posture.xyz, dude->posture.xyz);
    pfCopyVec3(posture.hpr, dude->posture.hpr);
    pfCopyVec3(velocity, dude->velocity);
    status = dude->status;
    JointVals[HEAD_H] = dude->direction;
    JointVals[HEAD_P] = dude->elevation;
    movement_index = 0;
  }
  else {
    pfCopyVec3(posture.xyz, jack->posture.xyz);
    pfCopyVec3(posture.hpr, jack->posture.hpr);
    pfCopyVec3(velocity, jack->velocity);
    status = jack->status;
    dude->status= jack->status;
    JointVals[HEAD_H] = jack->JointVals[HEAD_H];
    JointVals[HEAD_P] = jack->JointVals[HEAD_P];
    movement_index = jack->movement_index;    
  }
}

void JADECLASS::downloadData() {

  // download pertinent data from jade to correct model
  if (human == DUDEHUMAN) { 
    pfCopyVec3(dude->posture.xyz, posture.xyz);
    pfCopyVec3(dude->posture.hpr, posture.hpr);
    pfCopyVec3(dude->velocity, velocity);
    dude->status = status;
    jack->status = status;
    dude->direction = JointVals[HEAD_H]; 
    dude->elevation = JointVals[HEAD_P];
  }
  else {
    pfCopyVec3(jack->posture.xyz, posture.xyz);
    pfCopyVec3(jack->posture.hpr, posture.hpr);
    pfCopyVec3(jack->velocity, velocity);
    jack->status = status;
    dude->status = status;
    jack->JointVals[HEAD_H] = JointVals[HEAD_H]; 
    jack->JointVals[HEAD_P] = JointVals[HEAD_P];
    jack->movement_index = movement_index; 
  }
}

void JADECLASS::downloadData2Jack() {
  pfCopyVec3(jack->posture.xyz, posture.xyz);
  pfCopyVec3(jack->posture.hpr, posture.hpr);
  pfCopyVec3(jack->velocity, velocity);
  jack->status = status;
  dude->status = status;
  jack->JointVals[HEAD_H] = JointVals[HEAD_H];
  jack->JointVals[HEAD_P] = JointVals[HEAD_P];
  jack->movement_index = movement_index;
}
