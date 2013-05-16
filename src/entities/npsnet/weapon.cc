// File: <weapon.cc>

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

#include "math.h"
#include "string.h"
#include "bstring.h"
#include "iostream.h"

#include "weapon.h"
#include "disnetlib.h"
#include "common_defs2.h"
#include "macros.h"
#include "effects.h"
#include "nps_smoke.h"
#include "identify.h"
#include "sound.h"
#include "terrain.h"
#include "munitions.h"
#include "appearance.h"
#ifdef JACK
#include "jack_di_veh.h"
#endif
#include "collision_const.h"
#include "conversion_const.h"
#include "view.h"

#ifndef NO_DVW
#define NO_PFSMOKE
#define NO_NPSSMOKE
#endif

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#endif

// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile STATIC_DATA     *G_static_data;
extern          VEH_LIST        *G_vehlist;
extern          VEHTYPE_LIST    *G_vehtype;
extern volatile float            G_curtime;
extern          pfGroup         *G_moving;
extern          pfGroup         *G_fixed;
extern volatile int              G_drivenveh;
extern          DIS_net_manager *G_net;
extern          WEAPON_LIST     *G_weaplist;

unsigned short event_cnt = 0;

static void play_effect(int, pfVec3, pfVec3, int);

#define NONE -1

#ifndef NO_PFSMOKE

extern int G_pfsmoke_msmoke_count;

static pfVec3 White = {1.0f,1.0f,1.0f};

#define PFUSMOKE_RADIUS 0.45f
#define PFUSMOKE_SPEED 0.55f
#define PFUSMOKE_TURBULENCE 0.0f
#define PFUSMOKE_DENSITY 15.70f
#define PFUSMOKE_DISSIPATION 0.5f
#define PFUSMOKE_EXPANSION 0.8f
#define PFUSMOKE_LENGTH 0.25f
#define PFUSMOKE_BEGIN_COLOR White
#define PFUSMOKE_END_COLOR White
#define PFUSMOKE_DURATION -1.0f // Infinite
#endif


MUNITION::MUNITION(int assignedmid) 
//the constuctor for the base class
{
  //almost all of the stuff is filed in by the fire pdu send routine
  pfSetVec3 ( posture.xyz, 0.0f, 0.0f, 0.0f );
  pfSetVec3 ( posture.hpr, 0.0f, 0.0f, 0.0f );
  pfSetVec3 ( drparam.xyz, 0.0f, 0.0f, 0.0f );
  pfSetVec3 ( drparam.hpr, 0.0f, 0.0f, 0.0f );
  pfSetVec3 ( velocity, 0.0f, 0.0f, 0.0f );
  pfSetVec3 ( acceleration, 0.0f, 0.0f, 0.0f );
  dcs = NULL;
  mid = assignedmid;
  lastPDU = -1; // this way a PDU is sent right out
  firedfrom = 0;
  target = -1;
  force = 0x00;
  pfSetVec3 ( velocity, 0.0f, 0.0f, 0.0f );
  effect.fire_effect = UNKNOWN;
  effect.det_effect = UNKNOWN;
  effect.fire_midi_snd = UNKNOWN;
  effect.det_midi_snd = UNKNOWN;
  effect.fire_local_snd = UNKNOWN;
  effect.det_local_snd = UNKNOWN;
  changestatus(0x0);
}

MUNITION::~MUNITION()
{
#ifdef DEBUG
   cerr << "Munition distructor\n";
#endif
}

void MUNITION::del_munition()
//public distructor
{
  this->MUNITION::~MUNITION();
}

void MUNITION::changestatus(unsigned int stat)
{
  status = stat;
}

unsigned int MUNITION::getstatus()
   {
   return status;
   }

movestat MUNITION::move(float deltatime,float currtime)
// this move routine for the base class
{
 pfVec3 oldpnt,move_dist;
 
//  cerr<<"Veloc "<<velocity[X]<< "  " <<velocity[Y]<< "  " <<velocity[Z]<<"\n";
  if((velocity[X] == 0.0f) &&
     (velocity[Y] == 0.0f) &&
     (velocity[Z] == 0.0f)){
    //for some reson the missile stopped, deactive it
    return(DEACTIVE);
  }
  //the old positon
  pfCopyVec3(oldpnt,posture.xyz);
  pfScaleVec3 ( move_dist, deltatime, velocity );
  pfAddVec3 ( posture.xyz, posture.xyz, move_dist );
  
#ifdef DEBUG
  cerr << "POS "<< posture.xyz[X] << "  " << posture.xyz[Y] << "  "  
       << posture.xyz[Z] << "  " << pfGetSwitchVal(G_weaplist[mid].icon) <<"\n";
#endif

  if(!inplay(posture.xyz)){
    //is it within bounds
    return(DEACTIVE);
  } 

#ifdef TRACE
  cerr << "going to impactdetermination\n";      
#endif

  if(impactdetermination(oldpnt,posture.xyz) == HITSOMETHING){
    // it hit something
    return(DEACTIVE);
  }
  
  if((currtime - lastPDU) > G_static_data->DIS_heartbeat_delta){
    // it is time to send a messge
    sendentitystate();
    lastPDU = currtime;
  }
  
  //put the missile in the correct location in space
  pfDCSTrans(dcs,posture.xyz[X], posture.xyz[Y],posture.xyz[Z]);
  pfDCSRot(dcs,posture.hpr[HEADING],
                posture.hpr[PITCH], posture.hpr[ROLL]);
#ifdef DEBUG_SCALE
  pfDCSScale(dcs,10.0f);
#endif

#ifdef TRACE
  cerr <<"Moved -- base class\n";
#endif

  return(FINEWITHME);
}


movestat BOMB::move(float deltatime,float curtime)
//this where the bomb follows a parabolic path
{
  float cumtime;  //the time since the last pdu was sent out
  pfVec3 oldpnt,oldvel;
  pfVec3 accel;
  float  time2;
  pfVec3 tempvec;
#ifdef DIS_2v4
 unsigned int bomb_disappear = getstatus();
 bomb_disappear |= Appearance_Munition_Status_Deactivated;
#endif


#ifdef TRACE
  cerr <<"Moved -- bomb\n";
#endif

  if((velocity[X] == 0.0f) &&
     (velocity[Y] == 0.0f) &&
     (velocity[Z] == 0.0f)){
    //for some reason the missile stopped, deactive it
#ifdef DIS_2v4
    changestatus(bomb_disappear);
    sendentitystate();
#endif
    return(DEACTIVE);
  }
  //the old position of bomb
  PFCOPY_VEC3(oldpnt,posture.xyz);
  PFCOPY_VEC3(oldvel,velocity);

  //compute the accellerations
  //slow down in the horizontal plane
  //Drag in proportional to the square of the velocity
  accel[X] = -POSNEG(velocity[X]) *BOMB_DRAG* velocity[X]*velocity[X];
  accel[Y] = -POSNEG(velocity[Y]) *BOMB_DRAG* velocity[Y]*velocity[Y];
  //speed up in the vertical plane
  accel[Z] = -GRAVITY -POSNEG(velocity[Z]) *BOMB_DRAG* velocity[Z]*velocity[Z];

  //and now the velocitys and distances using euler integtation
  velocity[X] = accel[X] *deltatime + velocity[X];
  velocity[Y] = accel[Y] *deltatime + velocity[Y];
  velocity[Z] = accel[Z] *deltatime + velocity[Z];
  //constant time and 1/2
  time2 = deltatime*deltatime*0.5;
  posture.xyz[X] = time2*accel[X]+deltatime*velocity[X] +posture.xyz[X];
  posture.xyz[Y] = time2*accel[Y]+deltatime*velocity[Y] +posture.xyz[Y];
  posture.xyz[Z] = time2*accel[Z]+deltatime*velocity[Z] +posture.xyz[Z];
  
  if(!inplay(posture.xyz)){
    //is it within bounds
#ifdef DIS_2v4
    changestatus(bomb_disappear);
    sendentitystate();
#endif
    return(DEACTIVE);
  } 
     
  if(impactdetermination(oldpnt,posture.xyz) == HITSOMETHING){
    // it hit something
#ifdef DIS_2v4
    changestatus(bomb_disappear);
    sendentitystate();
#endif
    return(DEACTIVE);
  }

  //compute the heading
  pfSubVec3(tempvec,posture.xyz,oldpnt);
  pfNormalizeVec3(tempvec);
  posture.hpr[PITCH] = pfArcSin(tempvec[Z]);
 
  //find out if we have to sent out an entity state PDU
  cumtime = curtime - lastPDU;
  pfScaleVec3(tempvec,cumtime,oldvel);
  pfAddVec3(tempvec,drparam.xyz,tempvec);
  if((cumtime > G_static_data->DIS_heartbeat_delta) ||
     (pfSqrDistancePt3(tempvec,posture.xyz)>G_static_data->DIS_position_delta)){ 
    //it exceeds the dead reckoning parameters, send and update
    sendentitystate();
    //update the DRparameters
    PFCOPY_VEC3(drparam.xyz,posture.xyz);
    lastPDU = curtime;
  }
 
  //put the bomb in the correct location in space
  pfDCSTrans(dcs,posture.xyz[X], posture.xyz[Y],posture.xyz[Z]);
  pfDCSRot(dcs,posture.hpr[HEADING]-90.0f,
                posture.hpr[PITCH], posture.hpr[ROLL]);
#ifdef DEBUG_SCALE
  pfDCSScale(dcs,10.0f);
#endif

  return(FINEWITHME);
}


void MUNITION::launcheffect ()
//add a muzzle flash effect to the scence graph
{
   // add a animatied node at the location of launch
}

void MUNITION::impacteffect ()
//add a burst / crater at the point of impact
{
 // see above

}

movestat MUNITION::impactdetermination(pfVec3 startpnt, pfVec3 endpnt)
//See if the missile hit anything during this frame
{
   long isect; //did we hit anything
   pfSegSet seg;
   pfHit    **hits[32];
   pfVec3   pnt;
   pfMatrix xform;
   pfVec3   normal;
   long     flags;
//   pfNode   *node;
   pfGeoSet *gset;
   pfVec3 hpr;
   pfSetVec3(seg.segs[0].dir,
         (endpnt[X] - startpnt[X]),
         (endpnt[Y] - startpnt[Y]),
         (endpnt[Z] - startpnt[Z]));
   seg.segs[0].length = pfLengthVec3(seg.segs[0].dir);
   PFCOPY_VEC3(seg.segs[0].pos,startpnt);
   //fix a performer bug
   if(seg.segs[0].dir[Y] == 0.0f) seg.segs[0].dir[Y] = 0.01f;
   if(seg.segs[0].dir[X] == 0.0f) seg.segs[0].dir[X] = 0.01f;
   pfNormalizeVec3(seg.segs[0].dir);
   if(pfGetNumChildren(G_moving)){
     /*check for a vehicle intersection*/
     seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
     seg.userData = NULL;
     seg.activeMask = 0x01;
     seg.isectMask = ENTITY_MASK;
     seg.bound = NULL;
     seg.discFunc = vehiclehit;
     isect = pfNodeIsectSegs(G_moving, &seg, hits);
//cerr << "Chck Pt 3\n";
     if (isect) { /*intersection with a vehicle*/
       /*set the point at the intersecotin point in the world COORDINATES*/
       pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
       pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
       pfFullXformPt3(endpnt, pnt, xform);
       senddetonation(endpnt,pnt,hitveh());
#ifdef DEBUG_X
     cout << "\n\nVehicle hit at " << endpnt[X] << " "
                               << endpnt[Y] << " "
                               << endpnt[Z] << endl;
#endif
       if ( G_static_data->scoring )
          {
#ifdef DATALOCK
          pfDPoolLock ( (void *) G_dynamic_data );
#endif
          G_dynamic_data->dynamic_hits += 1;
#ifdef DATALOCK
          pfDPoolUnlock ( (void *) G_dynamic_data ); 
#endif
          }
//       use_next_fire_plume(endpnt[X],endpnt[Y],endpnt[Z], 20.0f);
       return(HITSOMETHING);
     }
   }

//cerr << "Chck Pt 4\n";

   //intersect test against the fixed objects
   if(endpnt[Z] < 1000.0f){
     seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
     seg.userData = NULL;
     seg.activeMask = 0x01;
     seg.isectMask = DESTRUCT_MASK;
     seg.bound = NULL;
     seg.discFunc = buildinghit;
     isect = pfNodeIsectSegs(G_fixed, &seg, hits);
     if (isect) { /*intersection with a building*/
        /*set the point at the intersectin point in the world COORDINATES*/
       pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
//       pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
//       pfFullXformPt3(endpnt, pnt, xform);
#ifdef DEBUG_X
   cout << "\n\nBuilding hit by weapon at " << endpnt[X] << " "
                               << endpnt[Y] << " "
                               << endpnt[Z] << endl;
#endif
       if ( effect.det_effect == EXPLOSION )
          {
          use_next_fire_plume(pnt[X],pnt[Y],pnt[Z], 20.);
          }
       senddetonation(pnt,pnt);
       if ( G_static_data->scoring )
          {
#ifdef DATALOCK
          pfDPoolLock ( (void *) G_dynamic_data );
#endif
          G_dynamic_data->static_hits += 1;
#ifdef DATALOCK
          pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
          }
       return(HITSOMETHING);
     }
   }
   //intersect test against the ground
   // if if is going up at a 45 degree angle, it is not going to hit anything
   // neither is it if it is above the ground out in the hinterlands
      /*check for ground impact*/
      seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      seg.userData = NULL;
      seg.activeMask = 0x01;
      seg.isectMask = PERMANENT_MASK;
      seg.bound = NULL;
      seg.discFunc = NULL;
      if (pfNodeIsectSegs(G_fixed, &seg, hits))
        {
        /*find the orientation of the crater*/
        long the_mask;
//        pfQueryHit(*hits[0], PFQHIT_NODE, &node);
//        the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
       
        pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
        the_mask = pfGetGSetIsectMask(gset);
        if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
             == PERMANENT_DIRT )
        {
        pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
        pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
        pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
        if (flags & PFHIT_NORM)
           {
           pfVec3 head, head90;
           float dotp;
           hpr[HEADING]= 0.0f;
           head[0] = 0.0f;
           head[1] = 1.0f;
           head[2] = 0.0f;
           dotp = PFDOT_VEC3(head, normal);
           hpr[PITCH] = pfArcCos(dotp) - 90.0f;
           head90[0] =  1.0f;
           head90[1] = 0.0f;
           head90[2] = 0.0f;
           dotp = PFDOT_VEC3(head90, normal);
           hpr[ROLL]  = 90.0f - pfArcCos(dotp);
#ifdef DEBUG
           cerr << "Crater 1 " << hpr[X] << "  "<<hpr[Y]<<"  " <<hpr[Z] <<"\n";
#endif
        }

       /* Leave a creater at that location*/
#ifdef DEBUG_X
       cout << "\n\nGround hit at " << pnt[X] << " "
                            << pnt[Y] << " "
                            << pnt[Z] << endl;

#endif
       if ( (effect.det_effect == EXPLOSION) ||
            (effect.det_effect == FIREBALL) )
          {
          makecrater(pnt,hpr);
          //this generates smoke
          use_next_fire_plume(pnt[X], pnt[Y], pnt[Z],20.0f);
          }

       senddetonation(pnt);
       }     
       else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
             == PERMANENT_WATER )
          {
          senddetonation(DetonationResult(DetResult_Detonation));
          }
       else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
             == PERMANENT_FOILAGE)
          {
          if ( effect.det_effect == EXPLOSION )
             {
             //this generates smoke
             use_next_fire_plume(pnt[X], pnt[Y], pnt[Z],20.0f);
             }

          senddetonation(DetonationResult(DetResult_Detonation));
          }
       else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
             == PERMANENT_TUNNEL)
          {
          senddetonation(DetonationResult(DetResult_Detonation));
          }
       else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
             == PERMANENT_BRIDGE )
          {
          senddetonation(DetonationResult(DetResult_Detonation));
          }
       return(HITSOMETHING);
       }
   
   //if we got here it did not hit anything
   return(FINEWITHME);
}

void MUNITION::sendentitystate()
//send out the entity state pdu for the current munition;
{
  EntityStatePDU       ESpdu;
  char tempstr[MARKINGS_LEN];

//cerr <<"send entity state,MUNITION::sendentitystate\n";

  bzero ( (char *)&ESpdu, sizeof(EntityStatePDU) );

  /* first field after header */
  /*set where it is comming from*/
/*
  if ( G_dynamic_data->exercise < 0 )
     ESpdu->entity_state_header.exercise_ident = (unsigned char)1;
  else
     ESpdu->entity_state_header.exercise_ident =
        (unsigned char)G_dynamic_data->exercise;
*/
  ESpdu.entity_id.address.site = disname.address.site;
  ESpdu.entity_id.address.host = disname.address.host;
  ESpdu.entity_id.entity = disname.entity;
  ESpdu.force_id = force;

  /*what is it*/
  ESpdu.entity_type = type;

  bzero(&(ESpdu.alt_entity_type),sizeof(EntityType));
  ESpdu.entity_appearance = getstatus();
  ESpdu.capabilities = 0x0; /*it can't do anything*/

  /*where it is in the world and how it is oriented*/
  /*note: we are using the SW corner of the Database as the origin, NOT
    WGS 84 as it calls for in the DIS Spec*/
  ESpdu.entity_location.x = posture.xyz[X];
  ESpdu.entity_location.y = posture.xyz[Y];
  ESpdu.entity_location.z = posture.xyz[Z];

  /*I am not sure about this, but what the hell*/
  ESpdu.entity_orientation.psi   = -posture.hpr[HEADING]*DEG_TO_RAD;
  ESpdu.entity_orientation.theta =  posture.hpr[PITCH]*DEG_TO_RAD;
  ESpdu.entity_orientation.phi   =  posture.hpr[ROLL]*DEG_TO_RAD;

  /*movement parameters*/
  ESpdu.dead_reckon_params.algorithm = DRAlgo_DRM_FPW;
  ESpdu.entity_velocity.x = velocity[X];
  ESpdu.entity_velocity.y = velocity[Y];
  ESpdu.entity_velocity.z = velocity[Z];

  /*no articulated parameters on missiles*/
  ESpdu.num_articulat_params = (unsigned char)0;

  /*Don't put some markings on it*/
  ESpdu.entity_marking.character_set = CharSet_ASCII;
  strncpy ( tempstr, "Booth_618", MARKINGS_LEN);
  strcat ( (char *)ESpdu.entity_marking.markings, tempstr );

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          /*send it*/
          if (G_net->write_pdu((char *) &ESpdu, (PDUType) EntityStatePDU_Type) 
             == FALSE)
             cerr <<  "write_pdu() failed\n";
          break;
       }

#ifdef DEBUG
   cout << "Sent a EntityStatePDU\n";
#endif

}

void MUNITION::set_speed(float deltaspeed)
//sets the speed by the ammount in deltaspeed after converting it to 3d
{
  float sinpitch,cospitch,cosheading,sinheading;
  
  pfSinCos(posture.hpr[PITCH],&sinpitch,&cospitch);
  pfSinCos(posture.hpr[HEADING],&sinheading,&cosheading);
  velocity[Z] += sinpitch*deltaspeed;
  velocity[X] -= sinheading*cospitch*deltaspeed;
  velocity[Y] += cosheading*cospitch*deltaspeed;
}

void MUNITION::sendfire(int mid,VEHICLE *platform,int newcat,
                        pfCoord veh_posture, pfCoord veh_look,
                        float deltaspeed, int target )
//sends out the fire event for the munition;
//also fills in a lot of the fields
{
  FirePDU Fpdu;
  static teamMember = -1;
  int weapon_index;
  WEAPON_LIST *weapptr =  &(G_weaplist[mid]);
  pfMatrix orient;
  pfVec3 front = { 0.0f, 1.0f, 0.0f };
  pfVec3 right = { 1.0f, 0.0f, 0.0f };
  pfVec3 up = { 0.0f, 0.0f, 1.0f };
  long mask;
  pfCoord flashpt;
  JACK_TEAM_VEH *jack_team;
  static pfVec3 flashpt_xyz;

  if(platform->gettype() == JACK_TEAM) {
     jack_team = (JACK_TEAM_VEH *) platform;
     teamMember = (teamMember + 1) % jack_team->getTeamCount();
  }


  //do a little house cleaning first
  dcs = weapptr->dcs; 
  pfSwitchVal(weapptr->icon,(long)newcat);

  bzero ( (char *)&Fpdu, sizeof(FirePDU) );

  /*fill in the Fire, Target (Unkown), and Munition ID*/
  Fpdu.firing_entity_id.address.site = platform->disname.address.site;
  Fpdu.firing_entity_id.address.host = platform->disname.address.host;
  Fpdu.firing_entity_id.entity = platform->disname.entity;
#ifdef IEEE1278_1_95
  Fpdu.fire_mission_index = 0;
#endif

  firedfrom = platform->disname.entity;

  if ( gettype() == BULLET_TYPE )
     {
     /* non-tracked munition */
     Fpdu.munition_id.address.site = 0;
     Fpdu.munition_id.address.host = 0;
     Fpdu.munition_id.entity = 0;
     disname.address.site = 0;
     disname.address.host = 0;
     disname.entity = 0;
     }
  else
     {
     Fpdu.munition_id.address.site = platform->disname.address.site;
     Fpdu.munition_id.address.host = platform->disname.address.host;
     Fpdu.munition_id.entity = MUNITION_OFFSET + mid; 
     disname.address.site = platform->disname.address.site;
     disname.address.host = platform->disname.address.host;
     disname.entity = MUNITION_OFFSET + mid;
     }


  //don't admit who we are shoting at
  if ( target == -1 )
     {
     Fpdu.target_entity_id.address.host = 0; 
     Fpdu.target_entity_id.address.site = 0; 
     Fpdu.target_entity_id.entity = 0; 
     }
  else
     {
     Fpdu.target_entity_id.address.host = G_vehlist[target].DISid.address.host;
     Fpdu.target_entity_id.address.site = G_vehlist[target].DISid.address.site; 
     Fpdu.target_entity_id.entity = G_vehlist[target].DISid.entity;
     }
 
  /*the event ID*/
  Fpdu.event_id.address.site = disname.address.site;
  Fpdu.event_id.address.host = disname.address.host;

//  Fpdu.event_id.event = ++event_cnt;

  G_weaplist[mid].eventid = Fpdu.event_id.event = ++event_cnt;

  weapon_index = G_vehtype[platform->icontype].fireinfo[newcat].munition;

//cerr << "Sending a fire " << weapon_index << " from " << platform->icontype
//     << endl;

  /*note: we are using the SW corner of the Database as the origin, NOT
    WGS 84 as it calls for in the DIS Spec*/

  // For DIs flashpt at end of rifle but bullet goes in where DI is looking
  pfVec3 t_front,t_right,t_up;

  if(platform->getCat() != PERSON) {
//     pfVec3 eye_point;
//     pfVec3 aim_direction;
//     pfVec3 aim_point;
//     int aim_point_valid;
/*
     posture = platform->posture;
*/
     platform->get_weapon_view(posture);

     pfMakeEulerMat ( orient, posture.hpr[HEADING],
                           posture.hpr[PITCH],
                           posture.hpr[ROLL]);
     pfFullXformPt3 ( t_front, front, orient );
     pfFullXformPt3 ( t_right, right, orient );
     pfFullXformPt3 ( t_up, up, orient );
/*

     aim_direction[HEADING] = posture.hpr[HEADING] + veh_look.hpr[HEADING];
     aim_direction[PITCH] = posture.hpr[PITCH] + veh_look.hpr[PITCH];
     aim_direction[ROLL] = 0.0f;

cerr << "In Weapon: Aim_direction is " 
     << aim_direction[HEADING] << "H, "
     << aim_direction[PITCH] << "P, "
     << aim_direction[ROLL] << "R." << endl;

     platform->get_firepoint(eye_point);
**   pfScaleVec3 ( eye_point, EYE_OFFSET, t_up );
**   pfAddVec3 ( eye_point, eye_point, posture.xyz );
     aim_point_valid = compute_fixed_point ( posture.xyz,
                                             posture.hpr,
                                             aim_point );
*/

     pfScaleVec3 ( t_front,
                G_vehtype[platform->icontype].fireinfo[newcat].firepoint[Y],
                t_front );
     pfScaleVec3 ( t_right,
                G_vehtype[platform->icontype].fireinfo[newcat].firepoint[X],
                t_right );
     pfScaleVec3 ( t_up,
                G_vehtype[platform->icontype].fireinfo[newcat].firepoint[Z],
                t_up);

     flashpt = posture;

     pfAddVec3 (flashpt.xyz, flashpt.xyz, t_front);
     pfAddVec3 (flashpt.xyz, flashpt.xyz, t_up);
     pfAddVec3 (flashpt.xyz, flashpt.xyz, t_right);

/*
     if ( aim_point_valid ) {
        compute_hpr ( posture.xyz, aim_point, posture.hpr );
     }
     else {
        posture.hpr[HEADING] += veh_look.hpr[HEADING];
        posture.hpr[PITCH] += veh_look.hpr[PITCH];
     }
*/

  }
  else {
     /*where it is in the world and how it is oriented*/
     posture = veh_posture;

     memcpy(&flashpt, &veh_posture, sizeof(pfCoord));

     pfMakeEulerMat ( orient, veh_posture.hpr[HEADING],
                           veh_posture.hpr[PITCH],
                           veh_posture.hpr[ROLL]);
     pfFullXformPt3 ( t_front, front, orient );
     pfFullXformPt3 ( t_right, right, orient );
     pfFullXformPt3 ( t_up, up, orient );
     pfNormalizeVec3 ( t_front);
     pfNormalizeVec3 ( t_right );
     pfNormalizeVec3 ( t_up );

     pfAddVec3(posture.hpr,posture.hpr,veh_look.hpr);

     pfScaleVec3 ( t_front,veh_look.xyz[Y], t_front);
     pfScaleVec3 ( t_right,veh_look.xyz[X], t_right );
     pfScaleVec3 ( t_up, veh_look.xyz[Z], t_up);

     pfAddVec3 (posture.xyz, posture.xyz, t_front);
     pfAddVec3 (posture.xyz, posture.xyz, t_up);
     pfAddVec3 (posture.xyz, posture.xyz, t_right);


     pfMakeEulerMat ( orient, flashpt.hpr[HEADING],
                           flashpt.hpr[PITCH],
                           flashpt.hpr[ROLL]);

     pfFullXformPt3 ( t_front, front, orient );
     pfFullXformPt3 ( t_right, right, orient );
     pfFullXformPt3 ( t_up, up, orient );

     pfScaleVec3 ( t_front, platform->firepoint[Y], t_front );
     pfScaleVec3 ( t_right, platform->firepoint[X], t_right );
     pfScaleVec3 ( t_up, platform->firepoint[Z], t_up);
     pfAddVec3 (flashpt.xyz, flashpt.xyz, t_front);
     pfAddVec3 (flashpt.xyz, flashpt.xyz, t_right);
     pfAddVec3 (flashpt.xyz, flashpt.xyz, t_up);
      
  }

  
  // compute flashpt and posture for muntions from other team members
  if((platform->gettype() == JACK_TEAM) && (teamMember > 0)) { 
     jack_team->locateTeamPart(teamMember-1, flashpt.xyz,
        flashpt_xyz);
     Fpdu.location_in_world.x = flashpt_xyz[X];
     Fpdu.location_in_world.y = flashpt_xyz[Y];
     Fpdu.location_in_world.z = flashpt_xyz[Z];
     pfCopyVec3(posture.xyz,flashpt_xyz);
  }
  else {
     Fpdu.location_in_world.x = posture.xyz[X];
     Fpdu.location_in_world.y = posture.xyz[Y];
     Fpdu.location_in_world.z = posture.xyz[Z];
  }

  pfDCSTrans(dcs,posture.xyz[X],posture.xyz[Y],posture.xyz[Z]);

  mask = PRUNE_MASK;
  set_intersect_mask((pfGroup *)weapptr->dcs,
                     mask, COLLIDE_DYNAMIC);
  pfSync();
  pfAddChild ( G_moving, (pfGroup *)weapptr->dcs );
 
//  platform->get_firepoint(this->posture.xyz);

  force = platform->force;

  /*what did we shoot*/
  type = G_vehtype[weapon_index].distype;
  type.specific = (unsigned char)G_dynamic_data->specific;
  Fpdu.burst_descriptor.munition =  type;
  Fpdu.burst_descriptor.warhead = WarheadMunition_HighExp;
  Fpdu.burst_descriptor.fuze = FuzeMunition_Contact;
  Fpdu.burst_descriptor.quantity = 1; /*One at a time*/
  Fpdu.burst_descriptor.rate = 0;
/*
  mask = (long(type.entity_kind) << ENTITY_KIND_SHIFT);
  mask = mask | (long(type.domain) << ENTITY_DOMAIN_SHIFT);
  mask = mask | (long(force) << ENTITY_FORCE_SHIFT);
*/

#ifdef DEBUG
cerr << "Cat " << cat << " Index " << cat+START_WEAPON 
     << " " << G_vehtype[cat+START_WEAPON].name << "\n";
cerr << "DIS Type "<< 0 +type.entity_kind <<" "<< 0 +type.domain
        <<" "<<type.country<<" "<<0 +type.category<<" ";
cerr <<0 +type.subcategory<<" "<<0 +type.specific<<" Firesend\n";
#endif


  pfCopyVec3 ( velocity, platform->velocity );
  set_speed(deltaspeed);
  /*movement parameters*/
  Fpdu.velocity.x = velocity[X];
  Fpdu.velocity.y = velocity[Y];
  Fpdu.velocity.z = velocity[Z];

  /*how far can it go*/
  Fpdu.range = WEAP_RANGE;

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          /*send it*/
          if (G_net->write_pdu((char *) &Fpdu, (PDUType )FirePDU_Type) == FALSE)
            cerr << "write_pdu() failed\n";
          break;
       }

   G_dynamic_data->identify->effect ( &type, &effect );

   if ( G_static_data->sound )
      {
      LocationRec dummy_snd_loc;
      dummy_snd_loc.x = posture.xyz[X];
      dummy_snd_loc.y = posture.xyz[Y];
      dummy_snd_loc.z = posture.xyz[Z];
      playsound(&dummy_snd_loc,&dummy_snd_loc,effect.fire_local_snd,-1);
      }

  if(((platform->gettype() == JACK_TEAM) && (teamMember > 0))) {
     // create rifle puffs for each of the other team members
     play_effect(effect.fire_effect, flashpt_xyz, flashpt.hpr, FALSE);
  } // end if JACK_TEAM/UNIT and not main member
  else {
     play_effect(effect.fire_effect, flashpt.xyz,flashpt.hpr, FALSE);
  }

#ifdef DEBUG
   cout << "Sent a FirePDU\n";
   cout << "site " << Fpdu.munition_id.address.site
        << " host " << Fpdu.munition_id.address.host
        << " entity " << Fpdu.munition_id.entity << endl;
#endif
}



void MUNITION::fillinDetonationPDU(DetonationPDU *Dpdu,pfVec3 impact)
//fill in the standard part of the detonation
{
  float *mypos;

  /*fill in the Fire and Munition ID*/
/*
  if ( G_dynamic_data->exercise < 0 )
     Dpdu.detonation_header.exercise_ident = (unsigned char)1;
  else
     Dpdu.detonation_header.exercise_ident =
        (unsigned char)G_dynamic_data->exercise;
*/


  bzero ( (char *)Dpdu, sizeof(DetonationPDU) );

  velocity[X] = 0.0f;
  velocity[Y] = 0.0f;
  velocity[Z] = 0.0f;

  Dpdu->firing_entity_id.address.site = G_static_data->DIS_address.site;
  Dpdu->firing_entity_id.address.host = G_static_data->DIS_address.host;
  Dpdu->firing_entity_id.entity = firedfrom;
  Dpdu->munition_id.address.site = disname.address.site;
  Dpdu->munition_id.address.host = disname.address.host;
  Dpdu->munition_id.entity =  disname.entity;
  mypos = G_vehlist[G_drivenveh].vehptr->getposition();
  
  /*the event ID*/
  Dpdu->event_id.address.site = G_static_data->DIS_address.site;
  Dpdu->event_id.address.host = G_static_data->DIS_address.host;

//  Dpdu->event_id.event = ++event_cnt; // G_weaplist[mid].eventid;

  Dpdu->event_id.event = G_weaplist[mid].eventid;

  /*what did we shoot*/
  Dpdu->burst_descriptor.munition = type;

  Dpdu->burst_descriptor.warhead = WarheadMunition_HighExpAntiTank;
  Dpdu->burst_descriptor.fuze = FuzeMunition_ContactInstant;
  Dpdu->burst_descriptor.quantity = 1; /*One at a time*/
  Dpdu->burst_descriptor.rate = 0;

  /*movement parameters*/
  Dpdu->velocity.x = velocity[X];
  Dpdu->velocity.y = velocity[Y];
  Dpdu->velocity.z = velocity[Z];

  /*where we hit*/
  Dpdu->location_in_world.x = impact[X];
  Dpdu->location_in_world.y = impact[Y];
  Dpdu->location_in_world.z = impact[Z];

  /*no Aticulated Parameters,yet*/
  Dpdu->num_articulat_params = 0;

   if ( G_static_data->sound )
     {
     LocationRec my_snd_pos;
     LocationRec snd_pos;
     my_snd_pos.x = mypos[X];
     my_snd_pos.y = mypos[Y];
     my_snd_pos.z = mypos[Z];
     snd_pos.x = impact[X];
     snd_pos.y = impact[Y];
     snd_pos.z = impact[Z];
     playsound(&my_snd_pos,&snd_pos,effect.det_local_snd);
     }

  play_effect(effect.det_effect,impact, posture.hpr,TRUE);

  if ( effect.smoke_effect != NO_SMOKE ) {
     use_next_fire_plume( impact[X],impact[Y],impact[Z], 
                          20.0f, effect.smoke_effect );
  }

}

void MUNITION::senddetonation(pfVec3 impact)
//looks like we hit the dirt, so send out a message that we did
{
  DetonationPDU Dpdu; //the PDU 

  //allocate the memory for the message

  //fillin the standard part
  fillinDetonationPDU(&Dpdu,impact);

  //since we missed everything
  Dpdu.location_in_entity.x = 0;  
  Dpdu.location_in_entity.y = 0;  
  Dpdu.location_in_entity.z = 0;  

  Dpdu.target_entity_id.address.site = 0;
  Dpdu.target_entity_id.address.host = 0;
  Dpdu.target_entity_id.entity = 0;

  //we hit the dirt
  Dpdu.detonation_result = DetResult_GroundImpact;
//cerr << "Dpdu.detonation_result = " << 0+Dpdu->detonation_result <<"\n";

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          /*send it*/
          if (G_net->write_pdu((char *) &Dpdu, (PDUType )DetonationPDU_Type) 
             == FALSE)
             cerr << "write_pdu() failed\n";
          break;
       }

#ifdef DEBUG
   cout << "Sent a DetonationPDU\n";
   cout << "site " << Dpdu.munition_id.address.site
        << " host " << Dpdu.munition_id.address.host
        << " entity " << Dpdu.munition_id.entity << endl;
#endif

}

void MUNITION::senddetonation(DetonationResult Rtype)
//AIR BALL!
//explodes at the loaction of the munition
{
 DetonationPDU Dpdu; //the PDU

  //allocate the memory for the message

  //fillin the standard part
  fillinDetonationPDU(&Dpdu,posture.xyz);

  //since we missed everything
  Dpdu.location_in_entity.x = 0;
  Dpdu.location_in_entity.y = 0;
  Dpdu.location_in_entity.z = 0;

  Dpdu.target_entity_id.address.site = 0;
  Dpdu.target_entity_id.address.host = 0;
  Dpdu.target_entity_id.entity = 0;

  //we hit the dirt
  Dpdu.detonation_result = Rtype;

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          /*send it*/
          if (G_net->write_pdu((char *) &Dpdu, (PDUType )DetonationPDU_Type) 
             == FALSE)
             cerr << "write_pdu() failed\n";
          break;
       }

#ifdef DEBUG
   cout << "Sent a DetonationPDU\n";
   cout << "site " << Dpdu.munition_id.address.site
        << " host " << Dpdu.munition_id.address.host
        << " entity " << Dpdu.munition_id.entity << endl;
#endif

}

void MUNITION::senddetonation(pfVec3 impact,pfVec3 bodycoord)
//looks like we hit a building, so send out a message that we did
{
  DetonationPDU Dpdu; //the PDU 

  //allocate the memory for the message

  //fillin the standard part
  fillinDetonationPDU(&Dpdu,impact);
  
  //We hit a building, so enter the coordinates
  Dpdu.location_in_entity.x = bodycoord[X];
  Dpdu.location_in_entity.y = bodycoord[Y];
  Dpdu.location_in_entity.z = bodycoord[Z];

  Dpdu.target_entity_id.address.site = 0;
  Dpdu.target_entity_id.address.host = 0;
  Dpdu.target_entity_id.entity = 0;

  //we hit the dirt
  Dpdu.detonation_result = DetResult_Detonation;

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          /*send it*/
          if (G_net->write_pdu((char *) &Dpdu, (PDUType )DetonationPDU_Type) 
             == FALSE)
             cerr << "write_pdu() failed\n";
          break;
       }

#ifdef DEBUG
   cout << "Sent a DetonationPDU\n";
   cout << "site " << Dpdu.munition_id.address.site
        << " host " << Dpdu.munition_id.address.host
        << " entity " << Dpdu.munition_id.entity << endl;
#endif

}

void MUNITION::senddetonation(pfVec3 impact,pfVec3 bodycoord,int vid)
//looks like we hit a vehicle, so send out a message that we did
{
  DetonationPDU Dpdu; //the PDU 

//  cerr << "Sending out detonation for vehicle " << vid << endl;

  //allocate the memory for the message

  //fillin the standard part
  fillinDetonationPDU(&Dpdu,impact);

  //we hit the dirt
  Dpdu.detonation_result = DetResult_EntityImpact;

  //We hit a building, so enter the coordinates
  Dpdu.location_in_entity.x = bodycoord[X];
  Dpdu.location_in_entity.y = bodycoord[Y];
  Dpdu.location_in_entity.z = -bodycoord[Z];

  Dpdu.target_entity_id.address.site = G_vehlist[vid].DISid.address.site;
  Dpdu.target_entity_id.address.host = G_vehlist[vid].DISid.address.host;
  Dpdu.target_entity_id.entity = G_vehlist[vid].DISid.entity;

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          /*send it*/
          if (G_net->write_pdu((char *) &Dpdu, (PDUType )DetonationPDU_Type) 
             == FALSE)
             cerr << "write_pdu() failed\n";
          break;
       }

#ifdef DEBUG
   cout << "Sent a DetonationPDU\n";
   cout << "site " << Dpdu.munition_id.address.site
        << " host " << Dpdu.munition_id.address.host
        << " entity " << Dpdu.munition_id.entity << endl;
#endif

}

BULLET::BULLET(int mid) : MUNITION (mid) 
//bullet constuctor
{

  //cerr << "the bullet constructor mid " << mid << "\n";
  travelled_dist = 0.0f;

}


movestat BULLET::move(float deltatime,float currtime)
// this move routine for the bullet class
{
 pfVec3 oldpnt,move_dist;

//  cerr<<"Veloc "<<velocity[X]<< "  " <<velocity[Y]<< "  " <<velocity[Z]<<"\n";
  if((velocity[X] == 0.0f) &&
     (velocity[Y] == 0.0f) &&
     (velocity[Z] == 0.0f)){
    //for some reson the missile stopped, deactive it
    return(DEACTIVE);
  }
  //the old positon
  pfCopyVec3(oldpnt,posture.xyz);
  pfScaleVec3 ( move_dist, deltatime, velocity );
  travelled_dist += pfLengthVec3(move_dist);
  if (travelled_dist > MAX_BULLET_RANGE)
     return(DEACTIVE);
  pfAddVec3 ( posture.xyz, posture.xyz, move_dist );

#ifdef DEBUG
  cerr << "POS "<< posture.xyz[X] << "  " << posture.xyz[Y] << "  "
       << posture.xyz[Z] << "  " << pfGetSwitchVal(G_weaplist[mid].icon) <<"\n";
#endif

  if(!inplay(posture.xyz)){
    //is it within bounds
    return(DEACTIVE);
  }

#ifdef TRACE
  cerr << "going to impactdetermination\n";
#endif

  if(impactdetermination(oldpnt,posture.xyz) == HITSOMETHING){
    // it hit something
    return(DEACTIVE);
  }

  if((currtime - lastPDU) > G_static_data->DIS_heartbeat_delta){
    // it is time to send a messge
// Do NOT send an entity state PDU for bullets
    lastPDU = currtime;
  }

  //put the missile in the correct location in space
  pfDCSTrans(dcs,posture.xyz[X], posture.xyz[Y],posture.xyz[Z]);
  pfDCSRot(dcs,posture.hpr[HEADING],
                posture.hpr[PITCH], posture.hpr[ROLL]);
#ifdef DEBUG_SCALE
  pfDCSScale(dcs,10.0f);
#endif

#ifdef TRACE
  cerr <<"Moved -- bullet class\n";
#endif

  return(FINEWITHME);
}

MISSILE::MISSILE(int mid, int my_target) : MUNITION (mid)
{

   travelled_dist = 0.0f;
   target = my_target;
#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   if (G_dynamic_data->visual_mode)
      {
      G_dynamic_data->visual_active = 1;
      G_dynamic_data->visual_mode = 0;
      video = 1;
      pfChanTravMode(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                     PFTRAV_DRAW, PFDRAW_ON);
      pfChanView(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                 posture.xyz, posture.hpr);
      }
   else
      video = 0;
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
#ifndef NO_PFSMOKE
   setSmoke();
#endif

}

#ifndef NO_PFSMOKE
void MISSILE::setSmoke(void) {
        pfVec3 direction;
        pfMatrix rot_mat;

        pfSetVec3(direction, 0, 0-PFUSMOKE_LENGTH, 0);
        pfMakeEulerMat(rot_mat, posture.hpr[0], posture.hpr[1], posture.hpr[2]);        pfFullXformPt3(direction, direction, rot_mat);
        pfNormalizeVec3(direction);

  // Go to next pfuSmoke and turn it off (just in case it was on previously)
  G_pfsmoke_msmoke_count = (G_pfsmoke_msmoke_count + 1) % PFUMAXSMOKES;
  smoke_index = G_pfsmoke_msmoke_count;
  pfuSmokeMode(G_dynamic_data->msmoke[smoke_index],PFUSMOKE_STOP);
  G_dynamic_data->msmoke[smoke_index]->prevTime = -1.0f;

        pfuSmokeTex(G_dynamic_data->msmoke[smoke_index],G_dynamic_data->missileTex);
    pfuSmokeOrigin(G_dynamic_data->msmoke[smoke_index],posture.xyz,PFUSMOKE_RADIUS);
        pfuSmokeVelocity(G_dynamic_data->msmoke[smoke_index],PFUSMOKE_TURBULENCE,PFUSMOKE_SPEED);
    pfuSmokeDir(G_dynamic_data->msmoke[smoke_index],direction);
        pfuSmokeDensity(G_dynamic_data->msmoke[smoke_index],PFUSMOKE_DENSITY,PFUSMOKE_DISSIPATION,PFUSMOKE_EXPANSION);
    pfuSmokeColor(G_dynamic_data->msmoke[smoke_index],PFUSMOKE_BEGIN_COLOR,PFUSMOKE_END_COLOR);
    pfuSmokeDuration(G_dynamic_data->msmoke[smoke_index],PFUSMOKE_DURATION);
    pfuSmokeMode(G_dynamic_data->msmoke[smoke_index],PFUSMOKE_START);
}
#endif


void MISSILE::deactivate_missile ()
{
#ifdef DIS_2v4
   unsigned int missle_disappear = getstatus();
   missle_disappear |= Appearance_Munition_Status_Deactivated;
#endif

   if (video) {
#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
      pfChanTravMode(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                    PFTRAV_DRAW, PFDRAW_OFF);
      G_dynamic_data->visual_active = 0;
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
   }
#ifdef DIS_2v4
   changestatus(missle_disappear);
   sendentitystate();
#endif

#ifndef NO_PFSMOKE
   pfuSmokeMode(G_dynamic_data->msmoke[smoke_index],PFUSMOKE_STOP);
#endif

}

movestat MISSILE::move(float deltatime,float currtime)
// this move routine for the MISSILE class
{
 pfVec3 oldpnt,move_dist;
 float entity_x,entity_y,entity_z;
 pfVec3 target_loc;
 float bearing_angle,pitch_angle,ordered_heading;
 float old_heading = 0.0f;
 float target_dist;
 pfMatrix orient;
 pfVec3 direction = { 0.0f, 1.0f, 0.0f };
 float speed;
 pfVec3 offset;

  if((velocity[X] == 0.0f) &&
     (velocity[Y] == 0.0f) &&
     (velocity[Z] == 0.0f)) {
    //for some reson the missile stopped, deactive it
    deactivate_missile();
    return(DEACTIVE);
  }

  speed = pfLengthVec3(velocity);
  //the old positon
  pfCopyVec3(oldpnt,posture.xyz);

  if ( target > -1 ) {
     if (G_vehlist[target].vehptr == NULL) {
        makefireball(posture.xyz);
        senddetonation(DetonationResult(DetResult_Detonation));
        deactivate_missile();
        return(DEACTIVE);
     }
     else {
        old_heading = posture.hpr[HEADING];
        entity_x = posture.xyz[X];
        entity_y = posture.xyz[Y];
        entity_z = posture.xyz[Z];
        pfCopyVec3(target_loc, G_vehlist[target].vehptr->getposition());
        target_dist = pfDistancePt3(target_loc, posture.xyz);
        bearing_angle = atan2f((entity_y - target_loc[Y]),
                               (entity_x - target_loc[X]));
        pitch_angle = asinf((target_loc[Z] - entity_z + 1.0f)/target_dist);
        ordered_heading = RAD_TO_DEG * (HALFPI + bearing_angle);
        posture.hpr[PITCH] = RAD_TO_DEG * pitch_angle;
        while (ordered_heading >= 360.0f)
           ordered_heading -= 360.0f;
        while (ordered_heading < 0.0f)
           ordered_heading += 360.0f;
        posture.hpr[HEADING] = ordered_heading;
        /* This is an effort to try to keep the missile from missing an */
        /* entity when there is no geometry for Performer to intersect with */
        /* at the current missle altitude.  We start at 1 meter and go down */
        /* trying to hit geometry.                                          */
        if ( (fabs(old_heading - ordered_heading) > 165.0f) &&
             (target_dist < speed) ) {
           pfVec3 entity_coord = { 0.0f, 0.0f, -0.25f };
           senddetonation(target_loc,entity_coord,target);
           deactivate_missile();
           return(DEACTIVE);
        }
     }
  }
  
  posture.hpr[ROLL] = 0.0f;
  pfMakeEulerMat ( orient, posture.hpr[HEADING],
                           posture.hpr[PITCH],
                           posture.hpr[ROLL]);
  pfFullXformPt3 ( direction, direction, orient );
  pfNormalizeVec3 ( direction );
  if ( speed < 500.0f )
     speed += 10.0f;
  else if ( speed < 750.0f )
     speed += 5.0f;
  else if ( speed < 1000.0f )
     speed += 2.5f;
  pfScaleVec3 ( velocity, speed, direction);
  pfScaleVec3 ( move_dist, deltatime, velocity );
  pfAddVec3 ( posture.xyz, posture.xyz, move_dist );
  travelled_dist += pfLengthVec3(move_dist);
  if (travelled_dist > MAX_MISSLE_RANGE)
     {
     makefireball(posture.xyz);
     senddetonation(DetonationResult(DetResult_Detonation));
     deactivate_missile();
     return(DEACTIVE);
     }
 
#ifdef DEBUG
  cerr << "POS "<< posture.xyz[X] << "  " << posture.xyz[Y] << "  "
       << posture.xyz[Z] << "  " << pfGetSwitchVal(G_weaplist[mid].icon) <<"\n";
#endif

  if(!inplay(posture.xyz)){
    //is it within bounds
    {
     deactivate_missile();
     return(DEACTIVE);
    }
  }

#ifdef TRACE
  cerr << "going to impactdetermination\n";
#endif

  if(impactdetermination(oldpnt,posture.xyz) == HITSOMETHING){
    // it hit something
    {
     deactivate_missile();
     return(DEACTIVE);
    }
  }

  if((currtime - lastPDU) > G_static_data->DIS_heartbeat_delta){
    // it is time to send a messge
    sendentitystate();
    lastPDU = currtime;
  }

  if (video)
     {
     pfScaleVec3 ( offset, 5.0f, direction );
     pfAddVec3 ( offset, offset, posture.xyz );
     pfChanView(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                offset, posture.hpr);
     }

  //put the missile in the correct location in space
  pfDCSTrans(dcs,posture.xyz[X], posture.xyz[Y],posture.xyz[Z]);
  pfDCSRot(dcs,posture.hpr[HEADING],
                posture.hpr[PITCH], posture.hpr[ROLL]);
#ifdef DEBUG_SCALE
  pfDCSScale(dcs,10.0f);
#endif

#ifndef NO_PFSMOKE
  pfCopyVec3(G_dynamic_data->msmoke[smoke_index]->origin,posture.xyz);
#endif

#ifdef TRACE
  cerr <<"Moved -- base class\n";
#endif

  return(FINEWITHME);
}


mtype MUNITION::gettype() 
//what type is it
{ 
   return (MUNITION_TYPE);
}
mtype BULLET::gettype() 
//what type is it
{ 
   return (BULLET_TYPE);
}
mtype BOMB::gettype() 
//what type is it
{ 
   return (BOMB_TYPE);
}
mtype MISSILE::gettype() 
//what type is it
{ 
   return (MISSILE_TYPE);
}

void BOMB::launcheffect()
//a virtual function that does not do anything
{
   /*do nothing, when you launch a bomb nothing visual happens*/
};

void play_effect(int effect, pfVec3 xyz, pfVec3 hpr, int detFlag)
{
   switch (effect)  { 
      case NONE:
         break;
      case SMALL_FLASH:
         play_animation ( SMALL_FLASH, xyz, hpr );
         break;
      case MEDIUM_FLASH:
         play_animation ( MEDIUM_FLASH, xyz, hpr );
         break;
      case LARGE_FLASH:
         play_animation ( LARGE_FLASH, xyz, hpr );
         break;
      case SMALL_GUN:
         play_animation ( SMALL_GUN, xyz, hpr );
         break;
      case MEDIUM_GUN:
         play_animation ( MEDIUM_GUN, xyz, hpr );
         break;
      case LARGE_GUN:
         play_animation ( LARGE_GUN, xyz, hpr );
         break;
      case SMALL_SMOKE:
         play_animation ( SMALL_SMOKE, xyz, hpr );
         break;
      case MEDIUM_SMOKE:
         play_animation ( MEDIUM_SMOKE, xyz, hpr );
         break;
      case LARGE_SMOKE:
         play_animation ( LARGE_SMOKE, xyz, hpr );
         break;
      case EXPLOSION:
         play_animation ( EXPLOSION, xyz, hpr );
         break;
      case FIREBALL:
         makefireball (xyz);
         break;
      case AIR_BURST:
         play_animation ( AIR_BURST, xyz, hpr );
         break;
      case GROUND_HIT:
         play_animation ( GROUND_HIT, xyz, hpr );
         break;
      case IMPACT:
         play_animation ( IMPACT, xyz, hpr );
         break;
      case UNKNOWN:
      default:
         if(detFlag) {
            play_animation ( MEDIUM_SMOKE, xyz, hpr );
         }
         else {
            play_animation ( SMALL_FLASH, xyz, hpr );
         }
         break;
   }
} // end play_effect() 

#ifndef NOSUB
// *******************************************************************
// This section to end added by Dan Bacon //NEW
//
// This function is the constructor for the torpedo.  Its purpose
//  is to initialize variables and build an array of eligible targets,
//  namely ships or submarines, that are in the simulation

TORPEDO_SL::TORPEDO_SL(int mid, float launch_course) : MUNITION (mid)
{
   // Initialize variables for calculation of trajectory
   travelled_dist = 0.0f;
   is_underwater = 1;
   is_on_initial_course = 0;
   initial_course = launch_course;

   // Initialize variables for array of eligible ships and subs
   int target_num = 1;
   int ix = 0;

   // Initialize variables for targetting info for the torp
   target_acquired = 0;
   target_id = -1;

   // Make an array of maritime vehicles that are eligible to be hit by the
   //  torpedo.  The closest one at water impact will be the target.
   
   while ( target_num < 249 )
      {
       // Initailize all array elements to 0
      maritime_targets[target_num] = 0;

      // If it is a ship or sub, add its id to the array
      if (( G_vehlist[target_num].vehptr != NULL) &&
          (( G_vehlist[target_num].vehptr->gettype() == SUBMERSIBLE ) || 
           ( G_vehlist[target_num].vehptr->gettype() == SHIP )))
         {
         // id is eligible, so add to array
         maritime_targets[ix] = G_vehlist[target_num].id;
//cerr<<"got "<<ix<<endl<<endl;
//cerr<<"array # is "<<maritime_targets[ix]<<endl;
         // Increment counter to next array element
         ix++;

         // Increment counter to search all active vehicles
         target_num++;
         } // end if ship or sub

      else // not a ship or sub, so try the next vehicle
         target_num++;
      } // end of searching through vehicle list

   // Video torpedo was selected, so turn it on
#ifdef DATSLOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   if (G_dynamic_data->visual_mode)
      {
      G_dynamic_data->visual_active = 1;
      G_dynamic_data->visual_mode = 0;
      video = 1;
      pfChanTravMode(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                     PFTRAV_DRAW, PFDRAW_ON);
      pfChanView(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                 posture.xyz, posture.hpr);
      } // end if video torpedo selected

   else // video torpedo not selected
      video = 0;

#ifdef DATSLOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
} // end TORPEDO_SL constructor


// This function kills the torpedo, and sends an entity state 
//  PDU to kill it from the simulation.  It also turns off video
//  torpedo.

void TORPEDO_SL::deactivate_torpedo ()
{
#ifdef DIS_2v4
   unsigned int torpedo_sl_disappear = getstatus();
   torpedo_sl_disappear |= Appearance_Munition_Status_Deactivated;
#endif

   // If video torpedo was turned on, turn it off
   if (video) {
#ifdef DATSLOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
      pfChanTravMode(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                    PFTRAV_DRAW, PFDRAW_OFF);
      G_dynamic_data->visual_active = 0;
#ifdef DATSLOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
   } // end if video

#ifdef DIS_2v4
   changestatus(torpedo_sl_disappear);
   sendentitystate();
#endif
#ifdef DEBUG_TORP
   cerr << "Torpedo destroyed at X " << posture.xyz[X] <<
                              ", Y " << posture.xyz[Y] << 
                              ", Z " << posture.xyz[Z] << endl;
#endif
}


// This function determines if the torpedo hit anything in this frame.  
//  Functionality has been added to allow the torpedo to hit the water 
//  and remain alive.  Functionality has also been added to simulate 
//  that the torpedo stays in the confines of the water if it is in 
//  the water.

movestat TORPEDO_SL::impactdetermination(pfVec3 startpnt, pfVec3 endpnt)
{
   long isect; //did we hit anything
   pfSegSet seg;
   pfHit    **hits[32];
   pfVec3   pnt;
   pfMatrix xform;
   pfVec3   normal;
   long     flags;
//   pfNode   *node;
   pfGeoSet *gset;
   pfVec3 hpr;

   // Set up the segment representing the path of the torp in this frame
   pfSetVec3(seg.segs[0].dir,
         (endpnt[X] - startpnt[X]),
         (endpnt[Y] - startpnt[Y]),
         (endpnt[Z] - startpnt[Z]));
   seg.segs[0].length = pfLengthVec3(seg.segs[0].dir);
   PFCOPY_VEC3(seg.segs[0].pos,startpnt);
   //fix a performer bug
   if(seg.segs[0].dir[Y] == 0.0f) seg.segs[0].dir[Y] = 0.01f;
   if(seg.segs[0].dir[X] == 0.0f) seg.segs[0].dir[X] = 0.01f;
   pfNormalizeVec3(seg.segs[0].dir);

   // See if the torp hit a vehicle
   if(pfGetNumChildren(G_moving))
      {
      seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      seg.userData = NULL;
      seg.activeMask = 0x01;
      seg.isectMask = ENTITY_MASK;
      seg.bound = NULL;
      seg.discFunc = vehiclehit;
      isect = pfNodeIsectSegs(G_moving, &seg, hits);

      // We hit a vehicle, so blow up the torp and the vehicle
      if (isect) 
         {
         //set the point at the intersection point in the world COORDINATES
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit vehicle" << endl;
#endif
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
         pfFullXformPt3(endpnt, pnt, xform);
         senddetonation(endpnt,pnt,hitveh());
#ifdef DEBUG_X
     cout << "\n\nVehicle hit at " << endpnt[X] << " "
                               << endpnt[Y] << " "
                               << endpnt[Z] << endl;
#endif
         // Score is being kept, so increment the number of hits
         if ( G_static_data->scoring )
            {
#ifdef DATSLOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->dynamic_hits += 1;
#ifdef DATSLOCK
            pfDPoolUnlock ( (void *) G_dynamic_data ); 
#endif
            } // end if scoring

         // A vehicle was hit, so return to caller that we did
         return(HITSOMETHING);
         } // end if hit vehicle

      } // end if G_moving

   // Intersect test against the fixed objects.  Only tried if the torp
   //  is 1000 meters or lower
   if(endpnt[Z] < 1000.0f)
      {
      seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      seg.userData = NULL;
      seg.activeMask = 0x01;
      seg.isectMask = DESTRUCT_MASK;
      seg.bound = NULL;
      seg.discFunc = buildinghit;
      isect = pfNodeIsectSegs(G_fixed, &seg, hits);

      // The torp hit a building
      if (isect)
         {
         // Set the point at the intersection point in the world COORDINATES
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit building" << endl;
#endif
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
//       pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
//       pfFullXformPt3(endpnt, pnt, xform);
#ifdef DEBUG_X
   cout << "\n\nBuilding hit by weapon at " << endpnt[X] << " "
                               << endpnt[Y] << " "
                               << endpnt[Z] << endl;
#endif
         // If the desired effect is an explosion, do it
         if ( effect.det_effect == EXPLOSION )
            {
            use_next_fire_plume(pnt[X],pnt[Y],pnt[Z], 20.);
            } // end if EXPLOSION

         // Send a detonation PDU
         senddetonation(pnt,pnt);

         // Score is being kept, so increment the number of hits
         if ( G_static_data->scoring )
            {
#ifdef DATSLOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->static_hits += 1;
#ifdef DATSLOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
            } // end if scoring

         // We hit a building, so we can return from this function
         return(HITSOMETHING);
         } // end if hit a building

      } // end if Z less than 1000

      // Intersect test against the ground
      seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      seg.userData = NULL;
      seg.activeMask = 0x01;
      seg.isectMask = PERMANENT_MASK;
      seg.bound = NULL;
      seg.discFunc = NULL;
      if (pfNodeIsectSegs(G_fixed, &seg, hits))
         {
         // Find the orientation of the crater
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit ground" << endl;
#endif
         long the_mask;
//         pfQueryHit(*hits[0], PFQHIT_NODE, &node);
//         the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);

         pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
         the_mask = pfGetGSetIsectMask(gset);

         // The torp hit the ground
         if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
              == PERMANENT_DIRT )
            {
            pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
            pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
            pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
            if (flags & PFHIT_NORM)
               {
               // Figure out the orientation of the crater
               pfVec3 head, head90;
               float dotp;
               hpr[HEADING]= 0.0f;
               head[0] = 0.0f;
               head[1] = 1.0f;
               head[2] = 0.0f;
               dotp = PFDOT_VEC3(head, normal);
               hpr[PITCH] = pfArcCos(dotp) - 90.0f;
               head90[0] =  1.0f;
               head90[1] = 0.0f;
               head90[2] = 0.0f;
               dotp = PFDOT_VEC3(head90, normal);
               hpr[ROLL]  = 90.0f - pfArcCos(dotp);
#ifdef DEBUG
           cerr << "Crater 1 " << hpr[X] << "  "<<hpr[Y]<<"  " <<hpr[Z] <<"\n";
#endif
               } // end if flags & PFHIT_NORM

            /* Leave a creater at that location*/
#ifdef DEBUG_X
       cout << "\n\nGround hit at " << pnt[X] << " "
                            << pnt[Y] << " "
                            << pnt[Z] << endl;

#endif
            // If the desired effect was an explosion, do it and leave a
            //  crater.
            if ( effect.det_effect == EXPLOSION )
               {
               makecrater(pnt,hpr);

               // Generate smoke
               use_next_fire_plume(pnt[X], pnt[Y], pnt[Z],20.0f);
               } // end if EXPLOSION

            // Send detonation PDU
            senddetonation(pnt);
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit dirt" << endl;
#endif
            // The torp impacted the ground, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_DIRT   

         // We hit the water, so change the status of the torpedo so
         //  it trajectory will be calculated by different methods
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_WATER )
            {
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit water" << endl;
#endif
            
            is_underwater = 1;

            // Still lives if hit water, so do not return that anything was hit
            return(FINEWITHME);
            } // end if PERMANENT_WATER

         // The torp hit the trees
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_FOILAGE)
            {
            // Make an explosion if needed
            if ( effect.det_effect == EXPLOSION )
               {
               // Generate smoke
               use_next_fire_plume(pnt[X], pnt[Y], pnt[Z],20.0f);
               } // end if EXPLOSION

            // Send detonation PDU
            senddetonation(DetonationResult(DetResult_Detonation));
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit foilage" << endl;
#endif
            // The torp impacted trees, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_FOILAGE

         // The torp hit tunnel
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_TUNNEL)
            {
            // Send detonation PDU
            senddetonation(DetonationResult(DetResult_Detonation));
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit tunnel" << endl;
#endif
            // The torp impacted tunnel, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_TUNNEL

         // The torp hit bridge
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_BRIDGE )
            {
            // Send detonation PDU
            senddetonation(DetonationResult(DetResult_Detonation));
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit bridge" << endl;
#endif
            // The torp impacted bridge, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_BRIDGE

         } // end if PERMANENT_MASK

      // Torp stays alive in the boundary of the water.  If any part of
      //  the testing segment touches ground in the +Z direction, this
      //  is considered the same as impacting the ground underwater.
      if (is_underwater)
         {
         // Set up a long segment for intersection testing straight
         //  up from the torp in the +Z direction.
         PFSET_VEC3(seg.segs[0].dir, 0.0f, 0.0f, 1.0f);
         pfNormalizeVec3(seg.segs[0].dir);
         seg.segs[0].length = 20000.0f;

         // Test to see if the segment intersected anything fixed
         if (pfNodeIsectSegs(G_fixed, &seg, hits))
            {
            long the_mask;
//            pfQueryHit(*hits[0], PFQHIT_NODE, &node);
//            the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
            pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
            the_mask = pfGetGSetIsectMask(gset);
 
            // If the segment intersected anything but water, detonate it
            if ( int((the_mask & PERMANENT_MASK) >> PERMANENT_SHIFT)
                != PERMANENT_WATER)
               {   
               // Send detonation PDU
               senddetonation(posture.xyz);               
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit ground underwater" << endl;
#endif
               // The torp impacted the ground, so return that we did
               return(HITSOMETHING);
               } // end if we hit something other than water

            } // end if we have intersected something fixed

         } // end if underwater collision detection 

   // If we got here, it did not hit anything, so return that it didn't
   return(FINEWITHME);

} // end TORPEDO_SL :: impactdetermination()


// This is the function that determines the movement of the torpedo in 
//  each frame of the simulation.  The torpedo runs out to 50m then turns to
//  the initial course. Once on the initial course, the torpedo "searches"
//  a 100M radius for targets.  If a valid target is found (ship or sub), 
//  the torpedo chases after that target.

movestat TORPEDO_SL::move(float deltatime,float curtime)
{
   float cumtime;  //the time since the last pdu was sent out
   pfVec3 oldpnt,oldvel;
   pfVec3 accel;
   float  time2;
   pfVec3 tempvec;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 offset;

   // New variables for target following underwater
   pfVec3 move_dist;
   float entity_x,entity_y,entity_z;
   pfVec3 target_loc;
   float bearing_angle,pitch_angle;
   float old_heading = 0.0f;
   float old_pitch = 0.0f;
   float target_dist;
   pfMatrix orient;
   float speed;
   //float min_dist_to_target = 20;

#ifdef TRACE
   cerr <<"Moved -- torpedo_al\n";
#endif

   // Test to see if the torpedo stopped.  If so, deactive it.
   if((velocity[X] == 0.0f) &&
      (velocity[Y] == 0.0f) &&
      (velocity[Z] == 0.0f))
      {
      deactivate_torpedo();
      return(DEACTIVE);
      } // end if torpedo stopped

   // This is the old position of torpedo
   PFCOPY_VEC3(oldpnt,posture.xyz);
   PFCOPY_VEC3(oldvel,velocity);

// Compute the new position if the torp is underwater-------------------------
   if (is_underwater)
      {
      speed = pfLengthVec3(velocity);
      
       //torpedo armed (50M) and not on intial course
       // so turn to inital course after passing 50M
      if ( (travelled_dist >= 50) && (!is_on_initial_course))
        {
         old_heading =  posture.hpr[HEADING];

            // Calculate bearing to intitial_course
        float temp_heading;
        //float temp_initial_course;

        if ( posture.hpr[HEADING] <= 0) 
          temp_heading = - posture.hpr[HEADING];
        else
          temp_heading = 360.0 - posture.hpr[HEADING];
     
        bearing_angle = initial_course - temp_heading ;

        // Limit bearing angle between 0 to 359 degrees
        while (bearing_angle < 0.0f)
             {
              bearing_angle += 360.0f;
             }

#ifdef DEBUG_HOMING
   cerr << "Torpedo comming to initial course: bearing angle is " << bearing_angle << endl;
   cerr << "Torpedo comming to initial course: torp heading is " << old_heading << endl;
#endif
            // Torp is almost pointed at the target, so reset to new bearing
            if ( (bearing_angle >= 358.0f) || (bearing_angle <= 2.0f) )
               { 
                if ( initial_course >= 180.0) 
                  old_heading = -initial_course;
                else
                  old_heading = 360.0 - initial_course;
               
               is_on_initial_course = 1;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: heading in limits, so just turn towards" << endl;
#endif
               } // end of heading and bearing being close

            // Torp must turn left to initial course
            else if ( (bearing_angle > 180.0f) || ((bearing_angle < 0.0f) && 
                                 (bearing_angle > -180.0f)) )
               {
               old_heading += 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: heading out of limits, so add" << endl;
#endif
               } // end of left turn
            // Torp must turn right to initial course
            else
               {
            
               old_heading -= 1.0f;

#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: heading out of limits, so subtract" << endl;
#endif
               } // end of right turn

            if ( old_heading <= -180.0) 
                  old_heading = 360.0 + old_heading;
            if ( old_heading > 180.0) 
                  old_heading =  - (360 - old_heading);

            //set torpedo heading
            posture.hpr[HEADING] =   old_heading;
         } // end turning to initial course

//On inital course---------------------------------------------------------------
     if ( is_on_initial_course)
      {
      // No target designated, so find one is the array of ships/subs------------
      if ( !target_acquired )
         {
         // Initialize the counter of the array
         int ix = 0;

         // Initialize distance to maximum acquisition range
         float dist_to_target = (MAX_TORP_RANGE +1000.0 );

         // No target designated yet
         target_id = 0;

         // Search target array until no more targets in the array
         //  (empty elements are initialized to 0).
//cerr<<"checking maritime_targets[ix] #"<<ix<<" = "<<maritime_targets[ix]<<endl;
         while (maritime_targets[ix] != 0)
            {
            // Check to see if the id pointed to is valid and closest
            if ((G_vehlist[maritime_targets[ix]].vehptr == NULL) ||
                 ( pfDistancePt3( posture.xyz, 
                      G_vehlist[maritime_targets[ix]].vehptr->getposition())
                   > dist_to_target))
               {
               // Target is invalid or not the closest, so try keep trying
               ix++;
               } // end if NULL or farther than the last one

            // Target is valid and closer than the last one
            else
               {
                //If withing 100.0M then torpedo acquires
                if ( (pfDistancePt3( posture.xyz, 
                           G_vehlist[maritime_targets[ix]].vehptr->getposition()) ) < 100 )
                    {         
                      dist_to_target = pfDistancePt3( posture.xyz, 
                           G_vehlist[maritime_targets[ix]].vehptr->getposition());

                      // Set the target id to this closer target
                      target_id = maritime_targets[ix];
                      // We have acquired at least one target
                     
                      target_acquired = 1;
                    }
                  // Increment counter to test next available target
                  ix++;
            
               } // end valid and closer target

            } // end while maritime_targets[ix] != 0

         } // end no target acquired

      // We have already acquired our target, so move towards it
      else
         {
         // Test to see if the target has died or moved out of range.
         //  If so, we need to acquire another target.
         if ((G_vehlist[target_id].vehptr == NULL) ||
             ( pfDistancePt3( posture.xyz, 
                              G_vehlist[target_id].vehptr->getposition())
               > (MAX_TORP_RANGE + 1000.0f)))
            {
            target_acquired = 0;
#ifdef DEBUG_HOMING
   cerr << "Torpedo target now out of range" << endl;
#endif
            } // end target out of range

         // Target still in range, so move towards it
         else
            {
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing to target" << endl;
#endif
            // Initialize torpedo's HPR and XYZ from last frame
            old_heading = posture.hpr[HEADING];
            old_pitch = posture.hpr[PITCH];
            entity_x = posture.xyz[X];
            entity_y = posture.xyz[Y];
            entity_z = posture.xyz[Z];

            // Find the location and distance of the current target
            pfCopyVec3(target_loc, G_vehlist[target_id].vehptr->getposition());
            target_dist = pfDistancePt3(target_loc, posture.xyz);

            // Calculate bearing to target
            bearing_angle = RAD_TO_DEG * (atan2f((target_loc[Y] - entity_y),
                                   (target_loc[X] - entity_x)) - HALFPI);

            // Limit bearing angle between 0 to 359 degrees
            while (bearing_angle >= 360.0f)
               bearing_angle -= 360.0f;
            while (bearing_angle < 0.0f)
               bearing_angle += 360.0f;

#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: bearing angle is " << bearing_angle << endl;
   cerr << "Torpedo homing: torp heading is " << old_heading << endl;
#endif
            // Torp is almost pointed at the target, so reset to new bearing
            if (((old_heading - bearing_angle) >= -2.0f) &&
                ((old_heading - bearing_angle) <= 2.0f))
               {
               old_heading = bearing_angle;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: heading in limits, so just turn towards" << endl;
#endif
               } // end of heading and bearing being close

            // Torp must turn right to target
            else if ((old_heading - bearing_angle) < -2.0f)
               {
               old_heading += 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: heading out of limits, so add" << endl;
#endif
               } // end of right turn

            // Torp must turn left to target
            else
               {
               old_heading -= 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: heading out of limits, so subtract" << endl;
#endif
               } // end of left turn

            // Limit torpedo heading between 0 to 359 degrees
            while (old_heading >= 360.0f)
               old_heading -= 360.0f;
            while (old_heading < 0.0f)
               old_heading += 360.0f;

            // Set the torpedo's new heading
            posture.hpr[HEADING] = old_heading;
               
            // Calculate pitch angle to target
            pitch_angle = RAD_TO_DEG * asinf((target_loc[Z] - 
                                              entity_z + 1.0f)/target_dist);

            // Torp is almost pointed at the target, so reset to new pitch
            if (((old_pitch - pitch_angle) >= -2.0f) &&
                ((old_pitch - pitch_angle) <= 2.0f))
               {
               posture.hpr[PITCH] = pitch_angle;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: pitch in limits, so assign" << endl;
#endif
               } // end if pitch to target and current pitch are similar

            // Torp is lower than target, so pitch up
            else if ((old_pitch - pitch_angle) < -2.0f)
               {
               posture.hpr[PITCH] += 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: pitch out of limits, so add" << endl;
#endif
               } 

            // Torp is higher than target, so pitch down
            else
               {
               posture.hpr[PITCH] -= 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: pitch out of limits, so subtract" << endl;
#endif
               }

#ifdef DEBUG_HOMING
   cerr << "Torpedo end homing to target" << endl;
#endif

            } // end else target in range

         } // end if target already acquired

      } //end if is on inital course

//Torpedo control done, now update torpdeo's movements-------------------------
      // Roll not needed for torpedo underwater, so set to 0
      posture.hpr[ROLL] = 0.0f;

      // Calculate orientation of torpedo based on new HPR
      pfMakeEulerMat ( orient, posture.hpr[HEADING],
                               posture.hpr[PITCH],
                               posture.hpr[ROLL]);

      // Calculate vector out of the nose of the torp
      pfFullXformPt3 ( direction, direction, orient );

      // Normalize direction vector of torp to length 1
      pfNormalizeVec3 ( direction );

      // If speed of torp underwater is faster than 50, decel
      if ( speed > 50.0f )
         speed -= 2.0f;

      // If speed of torp underwater is slower than 50, accel
      else if ( speed < 45.0f )
         speed += 2.0f;

      // Speed is in an acceptable range, to make speed 50
      else
         speed = 50.0f;

      // Set the velocity vector for calculating distance in this frame
      pfScaleVec3 ( velocity, speed, direction);

      // Figure out the distance we traveled in this frame
      pfScaleVec3 ( move_dist, deltatime, velocity );

      // Move torp from last XYZ posit to new XYZ posit
      pfAddVec3 ( posture.xyz, posture.xyz, move_dist );

      // Add the amount we moved in this frame to the total distance 
      //  traveled since being underwater
      travelled_dist += pfLengthVec3(move_dist);

      // Once we have traveled our maximum range, kill the torp
      if (travelled_dist > MAX_TORP_RANGE)
         {
         deactivate_torpedo();
#ifdef DEBUG_TORP
   cerr << "Torpedo died: end of run" << endl;
#endif
         return(DEACTIVE);
         } // end kill torpedo at its max underwater range

      } // end if torpedo is underwater

   // Torp is above water, so compute ballistic motion
   else
      { 
      // Compute the accelerations.  Slow down in the horizontal plane.
      //  Drag is proportional to the square of the velocity
      accel[X] = -POSNEG(velocity[X]) *TORPEDO_DRAG* velocity[X]*velocity[X];
      accel[Y] = -POSNEG(velocity[Y]) *TORPEDO_DRAG* velocity[Y]*velocity[Y];

      // Speed up in the vertical plane
      accel[Z] = -GRAVITY -POSNEG(velocity[Z]) *TORPEDO_DRAG* velocity[Z]*velocity[Z];

      // Calculate velocities and distances using euler integration
      velocity[X] = accel[X] *deltatime + velocity[X];
      velocity[Y] = accel[Y] *deltatime + velocity[Y];
      velocity[Z] = accel[Z] *deltatime + velocity[Z];

      // Constant time and 1/2
      time2 = deltatime*deltatime*0.5;
      posture.xyz[X] = time2*accel[X]+deltatime*velocity[X] +posture.xyz[X];
      posture.xyz[Y] = time2*accel[Y]+deltatime*velocity[Y] +posture.xyz[Y];
      posture.xyz[Z] = time2*accel[Z]+deltatime*velocity[Z] +posture.xyz[Z];

      // Compute the pitch angle above water
      pfSubVec3(tempvec,posture.xyz,oldpnt);
      pfNormalizeVec3(tempvec);
      posture.hpr[PITCH] = pfArcSin(tempvec[Z]);

      // Calculate the direction of the torpedo for video torpedo
      posture.hpr[ROLL] = 0.0f;
      pfMakeEulerMat ( orient, posture.hpr[HEADING],
                               posture.hpr[PITCH],
                               posture.hpr[ROLL]);
      pfFullXformPt3 ( direction, direction, orient );
      pfNormalizeVec3 ( direction );
      } // end ballistic torpedo motion computation

   // Video torpedo was selected, so calculate where to look and open the 
   //  viewing channel
   if (video)
      {
      // Offset the direction to be out in front of the torpedo
      pfScaleVec3 ( offset, 5.0f, direction );

      // Add offset to current XYZ to look from the correct point in space
      pfAddVec3 ( offset, offset, posture.xyz );

      // Open the vidoe missile/torp channel
      pfChanView(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                 offset, posture.hpr);
#ifdef DEBUG_TORP
   cerr << "Video Torpedo is on." << endl;
#endif
      } // end video torpedo calculation

   // If the torpedo is out of bounds, kill it
   if(!inplay(posture.xyz))
      {
      deactivate_torpedo();
      return(DEACTIVE);
      } // end if torpedo out-of-bounds
     
   // Test to see if the torpedo hit something.  If it did, kill it
   if(impactdetermination(oldpnt,posture.xyz) == HITSOMETHING)
      {
      // torpdeo hit something -- call its own member function
      deactivate_torpedo();
      return(DEACTIVE);
      } // end if torpedo impacted anything

   // Find out if we have to sent out an entity state PDU
   cumtime = curtime - lastPDU;
   pfScaleVec3(tempvec,cumtime,oldvel);
   pfAddVec3(tempvec,drparam.xyz,tempvec);
   if((cumtime > G_static_data->DIS_heartbeat_delta) ||
      (pfSqrDistancePt3(tempvec,posture.xyz)>G_static_data->DIS_position_delta))
      { 
      // It exceeds the dead reckoning parameters, so send an update
      sendentitystate();

      //update the DRparameters
      PFCOPY_VEC3(drparam.xyz,posture.xyz);
      lastPDU = curtime;
      } // end send a new entity state PDU for DR

#ifdef DEBUG_TORP
   cerr << "Torpedo location at X " << posture.xyz[X] <<
                             ", Y " << posture.xyz[Y] << 
                             ", Z " << posture.xyz[Z] << endl;
   cerr << "Torpedo orientation in H " << posture.hpr[HEADING] <<
                                ", P " << posture.hpr[PITCH] << 
                                ", R " << posture.hpr[ROLL] << endl;
   cerr << "Torpedo velocity in X " << velocity[X] <<
                             ", Y " << velocity[Y] << 
                             ", Z " << velocity[Z] << endl;
#endif

   // Put the torpedo in the correct location in space
   pfDCSTrans(dcs,posture.xyz[X],posture.xyz[Y],posture.xyz[Z]);
   pfDCSRot(dcs,posture.hpr[HEADING],posture.hpr[PITCH],posture.hpr[ROLL]);
#ifdef DEBUG_SCSLE
   pfDCSScale(dcs,10.0f);
#endif

   return(FINEWITHME);
} // end TORPEDO_SL move()


// This function is intended to identify the correct launch effect of
//  the torpedo.  Currently, there is no effect.
void TORPEDO_SL::launcheffect()
//a virtual function that does not do anything
{
   /*do nothing, when you launch a bomb nothing visual happens*/
};


// This function returns the type of vehicle this torpedo is
mtype TORPEDO_SL::gettype() 
{ 
   return (TORPEDO_SL_TYPE);
}

// This is a special sendfire() for a torpedo, so that a initial course can
// be passed to the torpedo object

void TORPEDO_SL::sendfire(int mid,VEHICLE *platform,int newcat, PASS_DATA *,
                        float deltaspeed, int target )
//sends out the fire event for the munition;
//also fills in a lot of the fields
{

  FirePDU Fpdu;
  static teamMember = -1;
  int weapon_index;
  WEAPON_LIST *weapptr =  &(G_weaplist[mid]);
  pfMatrix orient;
  pfVec3 front = { 0.0f, 1.0f, 0.0f };
  pfVec3 right = { 1.0f, 0.0f, 0.0f };
  pfVec3 up = { 0.0f, 0.0f, 1.0f };
  long mask;
  pfCoord flashpt;
  JACK_TEAM_VEH *jack_team;
  static pfVec3 flashpt_xyz;

  if(platform->gettype() == JACK_TEAM) {
     jack_team = (JACK_TEAM_VEH *) platform;
     teamMember = (teamMember + 1) % jack_team->getTeamCount();
  }


  //do a little house cleaning first
  dcs = weapptr->dcs; 
  pfSwitchVal(weapptr->icon,(long)newcat);

  bzero ( (char *)&Fpdu, sizeof(FirePDU) );

/*
  if ( G_dynamic_data->exercise < 0 )
     Fpdu.fire_header.exercise_ident = (unsigned char)1;
  else
     Fpdu.fire_header.exercise_ident =
        (unsigned char)G_dynamic_data->exercise;
*/
 
  /*fill in the Fire, Target (Unkown), and Munition ID*/
  Fpdu.firing_entity_id.address.site = platform->disname.address.site;
  Fpdu.firing_entity_id.address.host = platform->disname.address.host;
  Fpdu.firing_entity_id.entity = platform->disname.entity;

  firedfrom = platform->disname.entity;

  if ( gettype() == BULLET_TYPE )
     {
     /* non-tracked munition */
     Fpdu.munition_id.address.site = 0;
     Fpdu.munition_id.address.host = 0;
     Fpdu.munition_id.entity = 0;
     disname.address.site = 0;
     disname.address.host = 0;
     disname.entity = 0;
     }
  else
     {
     Fpdu.munition_id.address.site = platform->disname.address.site;
     Fpdu.munition_id.address.host = platform->disname.address.host;
     Fpdu.munition_id.entity = MUNITION_OFFSET + mid; 
     disname.address.site = platform->disname.address.site;
     disname.address.host = platform->disname.address.host;
     disname.entity = MUNITION_OFFSET + mid;
     }


  //don't admit who we are shoting at
  if ( target == -1 )
     {
     Fpdu.target_entity_id.address.host = 0; 
     Fpdu.target_entity_id.address.site = 0; 
     Fpdu.target_entity_id.entity = 0; 
     }
  else
     {
     Fpdu.target_entity_id.address.host = G_vehlist[target].DISid.address.host;
     Fpdu.target_entity_id.address.site = G_vehlist[target].DISid.address.site; 
     Fpdu.target_entity_id.entity = G_vehlist[target].DISid.entity;
     }
 
  /*the event ID*/
  Fpdu.event_id.address.site = disname.address.site;
  Fpdu.event_id.address.host = disname.address.host;

//  Fpdu.event_id.event = ++event_cnt;

  G_weaplist[mid].eventid = Fpdu.event_id.event = ++event_cnt;

  weapon_index = G_vehtype[platform->icontype].fireinfo[newcat].munition;

//cerr << "Sending a fire " << weapon_index << " from " << platform->icontype
//     << endl;

  /*note: we are using the SW corner of the Database as the origin, NOT
    WGS 84 as it calls for in the DIS Spec*/

  // For DIs flashpt at end of rifle but bullet goes in where DI is looking
  pfVec3 t_front,t_right,t_up;

  
     //pfVec3 eye_point;
     //pfVec3 aim_direction;
     //pfVec3 aim_point;
     //int aim_point_valid;

     posture = platform->posture;
  
     pfMakeEulerMat ( orient, posture.hpr[HEADING],
                           posture.hpr[PITCH],
                           posture.hpr[ROLL]);
     pfFullXformPt3 ( t_front, front, orient );
     pfFullXformPt3 ( t_right, right, orient );
     pfFullXformPt3 ( t_up, up, orient );

pfScaleVec3 ( t_front,
                G_vehtype[platform->icontype].fireinfo[newcat].firepoint[Y],
                t_front );
     pfScaleVec3 ( t_right,
                G_vehtype[platform->icontype].fireinfo[newcat].firepoint[X],
                t_right );
     pfScaleVec3 ( t_up,
                G_vehtype[platform->icontype].fireinfo[newcat].firepoint[Z],
                t_up);

     pfAddVec3 (posture.xyz, posture.xyz, t_front);
     pfAddVec3 (posture.xyz, posture.xyz, t_up);
     pfAddVec3 (posture.xyz, posture.xyz, t_right);

     flashpt = posture;

     

   

  

  
  // compute flashpt and posture for muntions from other team members
  
     Fpdu.location_in_world.x = posture.xyz[X];
     Fpdu.location_in_world.y = posture.xyz[Y];
     Fpdu.location_in_world.z = posture.xyz[Z];
  

  pfDCSTrans(dcs,posture.xyz[X],posture.xyz[Y],posture.xyz[Z]);

  mask = PRUNE_MASK;
  set_intersect_mask((pfGroup *)weapptr->dcs,
                     mask, COLLIDE_DYNAMIC);
  pfSync();
  pfAddChild ( G_moving, (pfGroup *)weapptr->dcs );
 
//  platform->get_firepoint(this->posture.xyz);

  force = platform->force;

  /*what did we shoot*/
  type = G_vehtype[weapon_index].distype;
  type.specific = (unsigned char)G_dynamic_data->specific;
  Fpdu.burst_descriptor.munition =  type;
  Fpdu.burst_descriptor.warhead = WarheadMunition_HighExp;
  Fpdu.burst_descriptor.fuze = FuzeMunition_Contact;
  Fpdu.burst_descriptor.quantity = 1; /*One at a time*/
  Fpdu.burst_descriptor.rate = 0;
/*
  mask = (long(type.entity_kind) << ENTITY_KIND_SHIFT);
  mask = mask | (long(type.domain) << ENTITY_DOMAIN_SHIFT);
  mask = mask | (long(force) << ENTITY_FORCE_SHIFT);
*/

#ifdef DEBUG
cerr << "Cat " << cat << " Index " << cat+START_WEAPON 
     << " " << G_vehtype[cat+START_WEAPON].name << "\n";
cerr << "DIS Type "<< 0 +type.entity_kind <<" "<< 0 +type.domain
        <<" "<<type.country<<" "<<0 +type.category<<" ";
cerr <<0 +type.subcategory<<" "<<0 +type.specific<<" Firesend\n";
#endif


  pfCopyVec3 ( velocity, platform->velocity );
  set_speed(deltaspeed);
  /*ment parameters*/
  Fpdu.velocity.x = velocity[X];
  Fpdu.velocity.y = velocity[Y];
  Fpdu.velocity.z = velocity[Z];

  /*how far can it go*/
  Fpdu.range = WEAP_RANGE;

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          /*send it*/
          if (G_net->write_pdu((char *) &Fpdu, (PDUType )FirePDU_Type) == FALSE)
            cerr << "write_pdu() failed\n";
          break;
       }

  //identify_effect ( &type, &effect );

   if ( G_static_data->sound )
      {
      LocationRec dummy_snd_loc;
      dummy_snd_loc.x = posture.xyz[X];
      dummy_snd_loc.y = posture.xyz[Y];
      dummy_snd_loc.z = posture.xyz[Z];
      playsound(&dummy_snd_loc,&dummy_snd_loc,effect.fire_local_snd,-1);
      }

  if(((platform->gettype() == JACK_TEAM) && (teamMember > 0))) {
     // create rifle puffs for each of the other team members
     play_effect(effect.fire_effect, flashpt_xyz, flashpt.hpr, FALSE);
  } // end if JACK_TEAM/UNIT and not main member
  else {
     play_effect(effect.fire_effect, flashpt.xyz,flashpt.hpr, FALSE);
  }

#ifdef DEBUG
   cout << "Sent a FirePDU\n";
   cout << "site " << Fpdu.munition_id.address.site
        << " host " << Fpdu.munition_id.address.host
        << " entity " << Fpdu.munition_id.entity << endl;
#endif
}


//----------------------------------------------------------------------------
// *******************************************************************
// This section to end added by Dan Bacon //NEW
//
// This function is the constructor for the sub launched missile.  Its purpose
//  is to initialize variables and build an array of eligible targets,
//  namely ground vehicles or rotary winged vehicles, that are in the simulation
//  The missile is launched straight into the air, once 10M above the water
//  the missile pitches over to 0 pitch. Once the missile travels a total
//  distance  of 500M the missile turns to the initial course preset. The missile
//  then will dive to an altitude of 50m above the ground, maintaining this 
//  altitude, while looking for any targets within 1000M radius.

MISSILE_SL::MISSILE_SL(int mid, float launch_course) : MUNITION (mid)
{
   // Initialize variables for calculation of trajectory
   travelled_dist = 0.0f;
   speed = 40.0f;
   is_underwater = 1;
   is_cruising = 0;
   is_on_initial_course =0;
   
   if ( (launch_course > 0) && (launch_course < 180) )
      initial_course = - launch_course;
   else
      initial_course = 360.0 - launch_course;

   // Initialize variables for array of eligible ships and subs
   int target_num = 0;
   int ix = 0;

   // Initialize variables for targetting info for the torp
   target_acquired = 0;
   target_id = -1;

   // Make an array of maritime vehicles that are eligible to be hit by the
   //  missile.  The closest one at water impact will be the target.
   while ( target_num < 249 )
      {
      // Initailize all array elements to 0
      shore_targets[target_num] = 0;

      // If it is a ship or sub, add its id to the array
      if (( G_vehlist[target_num].vehptr != NULL) &&
          (( G_vehlist[target_num].vehptr->gettype() == ROTARY_WING ) || 
           ( G_vehlist[target_num].vehptr->gettype() == GND_VEH )))
         {
         // id is eligible, so add to array
         shore_targets[ix] = G_vehlist[target_num].id;

         // Increment counter to next array element
         ix++;

         // Increment counter to search all active vehicles
         target_num++;
         } // end if ship or sub

      else // not a ship or sub, so try the next vehicle
         target_num++;
      } // end of searching through vehicle list

   // Video missile was selected, so turn it on
#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   if (G_dynamic_data->visual_mode)
      {
      G_dynamic_data->visual_active = 1;
      G_dynamic_data->visual_mode = 0;
      video = 1;
      pfChanTravMode(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                     PFTRAV_DRAW, PFDRAW_ON);
      pfChanView(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                 posture.xyz, posture.hpr);
      } // end if video missile selected

   else // video missile not selected
      video = 0;

#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
} // end MISSILE_SL constructor


// This function kills the missile, and sends an entity state 
//  PDU to kill it from the simulation.  It also turns off video
//  missile.

void MISSILE_SL::deactivate_missile ()
{
#ifdef DIS_2v4
   unsigned int missile_al_disappear = getstatus();
   missile_al_disappear |= Appearance_Munition_Status_Deactivated;
#endif

   // If video missile was turned on, turn it off
   if (video) {
#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
      pfChanTravMode(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                    PFTRAV_DRAW, PFDRAW_OFF);
      G_dynamic_data->visual_active = 0;
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
   } // end if video

#ifdef DIS_2v4
   changestatus(missile_al_disappear);
   sendentitystate();
#endif
#ifdef DEBUG_TORP
   cerr << "Torpedo destroyed at X " << posture.xyz[X] <<
                              ", Y " << posture.xyz[Y] << 
                              ", Z " << posture.xyz[Z] << endl;
#endif
}


// This function determines if the missile hit anything in this frame.  
//  Functionality has been added to allow the missile to hit the water 
//  and remain alive.  Functionality has also been added to simulate 
//  that the missile stays in the confines of the water if it is in 
//  the water.

movestat MISSILE_SL::impactdetermination(pfVec3 startpnt, pfVec3 endpnt)
{
   long isect; //did we hit anything
   pfSegSet seg;
   pfHit    **hits[32];
   pfVec3   pnt;
   pfMatrix xform;
   pfVec3   normal;
   long     flags;
//   pfNode   *node;
   pfGeoSet *gset;
   pfVec3 hpr;

   // Set up the segment representing the path of the torp in this frame
   pfSetVec3(seg.segs[0].dir,
         (endpnt[X] - startpnt[X]),
         (endpnt[Y] - startpnt[Y]),
         (endpnt[Z] - startpnt[Z]));
   seg.segs[0].length = pfLengthVec3(seg.segs[0].dir);
   PFCOPY_VEC3(seg.segs[0].pos,startpnt);
   //fix a performer bug
   if(seg.segs[0].dir[Y] == 0.0f) seg.segs[0].dir[Y] = 0.01f;
   if(seg.segs[0].dir[X] == 0.0f) seg.segs[0].dir[X] = 0.01f;
   pfNormalizeVec3(seg.segs[0].dir);

   // See if the torp hit a vehicle
   if(pfGetNumChildren(G_moving))
      {
      seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      seg.userData = NULL;
      seg.activeMask = 0x01;
      seg.isectMask = ENTITY_MASK;
      seg.bound = NULL;
      seg.discFunc = vehiclehit;
      isect = pfNodeIsectSegs(G_moving, &seg, hits);

      // We hit a vehicle, so blow up the torp and the vehicle
      if (isect) 
         {
         //set the point at the intersection point in the world COORDINATES
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit vehicle" << endl;
#endif
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
         pfFullXformPt3(endpnt, pnt, xform);
         senddetonation(endpnt,pnt,hitveh());
#ifdef DEBUG_X
     cout << "\n\nVehicle hit at " << endpnt[X] << " "
                               << endpnt[Y] << " "
                               << endpnt[Z] << endl;
#endif
         // Score is being kept, so increment the number of hits
         if ( G_static_data->scoring )
            {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->dynamic_hits += 1;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data ); 
#endif
            } // end if scoring

         // A vehicle was hit, so return to caller that we did
         return(HITSOMETHING);
         } // end if hit vehicle

      } // end if G_moving

   // Intersect test against the fixed objects.  Only tried if the torp
   //  is 1000 meters or lower
   if(endpnt[Z] < 1000.0f)
      {
      seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      seg.userData = NULL;
      seg.activeMask = 0x01;
      seg.isectMask = DESTRUCT_MASK;
      seg.bound = NULL;
      seg.discFunc = buildinghit;
      isect = pfNodeIsectSegs(G_fixed, &seg, hits);

      // The torp hit a building
      if (isect)
         {
         // Set the point at the intersection point in the world COORDINATES
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit building" << endl;
#endif
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
//       pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
//       pfFullXformPt3(endpnt, pnt, xform);
#ifdef DEBUG_X
   cout << "\n\nBuilding hit by weapon at " << endpnt[X] << " "
                               << endpnt[Y] << " "
                               << endpnt[Z] << endl;
#endif
         // If the desired effect is an explosion, do it
         if ( effect.det_effect == EXPLOSION )
            {
            use_next_fire_plume(pnt[X],pnt[Y],pnt[Z], 20.);
            } // end if EXPLOSION

         // Send a detonation PDU
         senddetonation(pnt,pnt);

         // Score is being kept, so increment the number of hits
         if ( G_static_data->scoring )
            {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->static_hits += 1;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
            } // end if scoring

         // We hit a building, so we can return from this function
         return(HITSOMETHING);
         } // end if hit a building

      } // end if Z less than 1000

      // Intersect test against the ground
      seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      seg.userData = NULL;
      seg.activeMask = 0x01;
      seg.isectMask = PERMANENT_MASK;
      seg.bound = NULL;
      seg.discFunc = NULL;
      if (pfNodeIsectSegs(G_fixed, &seg, hits))
         {
         // Find the orientation of the crater
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit ground" << endl;
#endif
         long the_mask;
//         pfQueryHit(*hits[0], PFQHIT_NODE, &node);
//         the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
         pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
         the_mask = pfGetGSetIsectMask(gset);

         // The torp hit the ground
         if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
              == PERMANENT_DIRT )
            {
            pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
            pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
            pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
            if (flags & PFHIT_NORM)
               {
               // Figure out the orientation of the crater
               pfVec3 head, head90;
               float dotp;
               hpr[HEADING]= 0.0f;
               head[0] = 0.0f;
               head[1] = 1.0f;
               head[2] = 0.0f;
               dotp = PFDOT_VEC3(head, normal);
               hpr[PITCH] = pfArcCos(dotp) - 90.0f;
               head90[0] =  1.0f;
               head90[1] = 0.0f;
               head90[2] = 0.0f;
               dotp = PFDOT_VEC3(head90, normal);
               hpr[ROLL]  = 90.0f - pfArcCos(dotp);
#ifdef DEBUG
           cerr << "Crater 1 " << hpr[X] << "  "<<hpr[Y]<<"  " <<hpr[Z] <<"\n";
#endif
               } // end if flags & PFHIT_NORM

            /* Leave a creater at that location*/
#ifdef DEBUG_X
       cout << "\n\nGround hit at " << pnt[X] << " "
                            << pnt[Y] << " "
                            << pnt[Z] << endl;

#endif
            // If the desired effect was an explosion, do it and leave a
            //  crater.
            if ( effect.det_effect == EXPLOSION )
               {
               makecrater(pnt,hpr);

               // Generate smoke
               use_next_fire_plume(pnt[X], pnt[Y], pnt[Z],20.0f);
               } // end if EXPLOSION

            // Send detonation PDU
            senddetonation(pnt);
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit dirt" << endl;
#endif
            // The torp impacted the ground, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_DIRT   

         // We hit the water, so change the status of the missile so
         //  it trajectory will be calculated by different methods
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_WATER )
            {
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit water" << endl;
#endif
            
            is_underwater = 0;

            // Still lives if hit water, so do not return that anything was hit
            return(FINEWITHME);
            } // end if PERMANENT_WATER

         // The torp hit the trees
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_FOILAGE)
            {
            // Make an explosion if needed
            if ( effect.det_effect == EXPLOSION )
               {
               // Generate smoke
               use_next_fire_plume(pnt[X], pnt[Y], pnt[Z],20.0f);
               } // end if EXPLOSION

            // Send detonation PDU
            senddetonation(DetonationResult(DetResult_Detonation));
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit foilage" << endl;
#endif
            // The torp impacted trees, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_FOILAGE

         // The torp hit tunnel
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_TUNNEL)
            {
            // Send detonation PDU
            senddetonation(DetonationResult(DetResult_Detonation));
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit tunnel" << endl;
#endif
            // The torp impacted tunnel, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_TUNNEL

         // The torp hit bridge
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_BRIDGE )
            {
            // Send detonation PDU
            senddetonation(DetonationResult(DetResult_Detonation));
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit bridge" << endl;
#endif
            // The torp impacted bridge, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_BRIDGE

         } // end if PERMANENT_MASK

      // Torp stays alive in the boundary of the water.  If any part of
      //  the testing segment touches ground in the +Z direction, this
      //  is considered the same as impacting the ground underwater.
      if (is_underwater)
         {
         // Set up a long segment for intersection testing straight
         //  up from the torp in the +Z direction.
         PFSET_VEC3(seg.segs[0].dir, 0.0f, 0.0f, 1.0f);
         pfNormalizeVec3(seg.segs[0].dir);
         seg.segs[0].length = 20000.0f;

         // Test to see if the segment intersected anything fixed
         if (pfNodeIsectSegs(G_fixed, &seg, hits))
            {
            long the_mask;
//            pfQueryHit(*hits[0], PFQHIT_NODE, &node);
//            the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
            pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
            the_mask = pfGetGSetIsectMask(gset);
 
            // If the segment intersected anything but water, detonate it
            if ( int((the_mask & PERMANENT_MASK) >> PERMANENT_SHIFT)
                != PERMANENT_WATER)
               {   
               // Send detonation PDU
               senddetonation(posture.xyz);               
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit ground underwater" << endl;
#endif
               // The torp impacted the ground, so return that we did
               return(HITSOMETHING);
               } // end if we hit something other than water

            } // end if we have intersected something fixed

         } // end if underwater collision detection 

   // If we got here, it did not hit anything, so return that it didn't
   return(FINEWITHME);

} // end MISSILE_SL :: impactdetermination()


// This is the function that determines the movement of the missile in 
//  each frame of the simulation.  If the missile is in the air, it follows
//  the same ballistic path of the bomb in BOMB:move().  Once underwater,
//  the missile follows a straight path to the closest target in its range.

movestat MISSILE_SL::move(float deltatime,float curtime)
{
   float cumtime;  //the time since the last pdu was sent out
   pfVec3 oldpnt,oldvel;
   //pfVec3 accel;
   pfVec3 pitchposture= { 0.0f, 0.0f, 0.0f };
   //float  time2;
   pfVec3 tempvec;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 offset;

   // New variables for target following underwater
   pfVec3 move_dist;
   float entity_x,entity_y,entity_z;
   pfVec3 target_loc;
   float bearing_angle,pitch_angle;
   float old_heading = 0.0f;
   float old_pitch = 0.0f;
   float target_dist;
   pfMatrix orient;
   float future_altitude;
   float current_altitude;
   float delta_altitude;

#ifdef TRACE
   cerr <<"Moved -- missile_al\n";
#endif

   // Test to see if the missile stopped.  If so, deactive it.
   if((velocity[X] == 0.0f) &&
      (velocity[Y] == 0.0f) &&
      (velocity[Z] == 0.0f))
      {
      deactivate_missile();
      return(DEACTIVE);
      } // end if missile stopped

   // This is the old position of missile
   PFCOPY_VEC3(oldpnt,posture.xyz);
   PFCOPY_VEC3(oldvel,velocity);


//compute the new positoin if the missile is underwater----------------------
  if (is_underwater)
    { 
     posture.hpr[HEADING] += 0.0f;
     posture.hpr[PITCH]   += 0.0f;
     posture.hpr[ROLL]    += 0.0f;
    }

//compute the new positoin if the missile is NOT underwater----------------------
  if (!is_underwater)
    {
      
//CUTcerr<<"MISSILE NOT UNDERWATER"<<endl;
//First get to cruise altitiude--------------------------------------------
     if (!is_cruising)
       {
//CUTcerr<<"MISSILE NOT CRUISING"<<endl;
        current_altitude = get_altitude( posture.xyz );
 
        //missile roll over code
        if ( (current_altitude > 10) && (!is_cruising))
          posture.hpr[PITCH] -= 0.5f;

        if (posture.hpr[PITCH] <= 0.0f) //once missile at 0 pitch
          {                             // it is done with roll over
           is_cruising = 1; 
           posture.hpr[PITCH] = 0.0f;   
          }        
       } //end !is_cruising

//Once at cruise altitude, turn to initial course-----------------------
     if ( (is_cruising) && (!is_on_initial_course) && (travelled_dist > 500.0) )
       {
        old_heading =  posture.hpr[HEADING];

            // Calculate bearing to intitial_course
        float temp_heading;
        //float temp_initial_course;

        if ( posture.hpr[HEADING] <= 0) 
          temp_heading = - posture.hpr[HEADING];
        else
          temp_heading = 360.0 - posture.hpr[HEADING];
     
        bearing_angle = initial_course - temp_heading ;

        // Limit bearing angle between 0 to 359 degrees
        while (bearing_angle < 0.0f)
             {
              bearing_angle += 360.0f;
             }

           // Missile is almost pointed at the target, so reset to new bearing
            if ( (bearing_angle >= 358.0f) || (bearing_angle <= 2.0f) )
               { 
                if ( initial_course >= 180.0) 
                  old_heading = -initial_course;
                else
                  old_heading = 360.0 - initial_course;
               
               is_on_initial_course = 1;
             } // end of heading and bearing being close

            // Missile must turn left to initial course
            else if ( (bearing_angle > 180.0f) || ((bearing_angle < 0.0f) && 
                                 (bearing_angle > -180.0f)) )
               {
               old_heading += 1.0f;
             } // end of left turn
            // Missile must turn right to initial course
            else
               {
            
               old_heading -= 1.0f;

               } // end of right turn

            if ( old_heading <= -180.0) 
                  old_heading = 360.0 + old_heading;
            if ( old_heading > 180.0) 
                  old_heading =  - (360 - old_heading);

            //set torpedo heading
            posture.hpr[HEADING] =   old_heading;

       
       } //end (is_cruising) && (!is_on_initial_course) && (travelled_dist > 500.0)



//once you are cruising and on inital course look for tgt to aquire----------
       if ( (is_cruising) && (is_on_initial_course) && (!target_acquired) )
         {
          // Initialize the counter of the array
          int ix = 0;

          // Initialize distance to maximum acquisition range
             float dist_to_target = (MAX_MISSLE_RANGE);

             // No target designated yet
             target_id = 0;
#ifdef DEBUG_ACQUIRE
   cerr << "Missile: trying to acquire" << endl;
#endif
            // Search target array until no more targets in the array
            //  (empty elements are initialized to 0).
            while (shore_targets[ix] != 0)
             {
#ifdef DEBUG_ACQUIRE
   cerr << "Missile acquisition loop" << endl;
#endif
             // Check to see if the id pointed to is valid and closest
             if ((G_vehlist[shore_targets[ix]].vehptr == NULL) ||
                 ( pfDistancePt3( posture.xyz, 
                      G_vehlist[shore_targets[ix]].vehptr->getposition())
                  > dist_to_target))
               {
               // Target is invalid or not the closest, so try keep trying
               ix++;
#ifdef DEBUG_ACQUIRE
   cerr << "No vehicle or too far away" << endl;
#endif
               } // end if NULL or farther than the last one

             // Target is valid and closer than the last one
             else
               {
               
                 if ( pfDistancePt3( posture.xyz, 
                        G_vehlist[shore_targets[ix]].vehptr->getposition()) <= 1000.0)
                   {
                    // Make the distance the new shorter one so we find the
                    //  shortest once we have tested all eligible vehicles
                    dist_to_target = pfDistancePt3( posture.xyz, 
                        G_vehlist[shore_targets[ix]].vehptr->getposition());

                    // Set the target id to this closer target
                    target_id = shore_targets[ix];

                    }

                 // Increment counter to test next available target
                 ix++;

                 // We have acquired at least one target
                 target_acquired = 1;
               } // end valid and closer target

            } // end while shore_targets[ix] != 0

          } //end (is_cruising) && (is_on_initial_course) && (!target_acquired)



//tgt aquired so move toward (heading)--------------------------------------------
      if (target_acquired)
        {

         // Test to see if the target has died or moved out of range.
         //  If so, we need to acquire another target.
         if ((G_vehlist[target_id].vehptr == NULL) ||
             ( pfDistancePt3( posture.xyz, 
                              G_vehlist[target_id].vehptr->getposition())
               > (MAX_MISSLE_RANGE  )))
            {
            target_acquired = 0;
#ifdef DEBUG_HOMING
   cerr << "Missile target now out of range" << endl;
#endif
            } // end target out of range

         // Target still in range, so move towards it
         else
            {
#ifdef DEBUG_HOMING
   cerr << "Missile homing to target" << endl;
#endif

            // Initialize missile's HPR and XYZ from last frame
            old_heading = posture.hpr[HEADING];
            old_pitch = posture.hpr[PITCH];
            entity_x = posture.xyz[X];
            entity_y = posture.xyz[Y];
            entity_z = posture.xyz[Z];

            // Find the location and distance of the current target
            pfCopyVec3(target_loc, G_vehlist[target_id].vehptr->getposition());
            target_dist = pfDistancePt3(target_loc, posture.xyz);

            // Calculate bearing to target
            float bearing_angle_tgt;
            bearing_angle_tgt = RAD_TO_DEG * (atan2f((target_loc[Y] - entity_y),
                                   (target_loc[X] - entity_x)) - HALFPI);

            // Limit bearing angle between 0 to 359 degrees
            while (bearing_angle >= 360.0f)
               bearing_angle_tgt -= 360.0f;
            while (bearing_angle < 0.0f)
               bearing_angle_tgt += 360.0f;

#ifdef DEBUG_HOMING
   cerr << "Missile homing: bearing angle is " << bearing_angle << endl;
   cerr << "Missile homing: missile heading is " << old_heading << endl;
#endif
            // Missile is almost pointed at the target, so reset to new bearing
            if (((old_heading - bearing_angle_tgt) >= -2.0f) &&
                ((old_heading - bearing_angle_tgt) <= 2.0f))
               {
               old_heading = bearing_angle_tgt;
#ifdef DEBUG_HOMING
   cerr << "Missile homing: heading in limits, so just turn towards" << endl;
#endif
               } // end of heading and bearing being close

            // Missile must turn right to target
            else if ((old_heading - bearing_angle_tgt) < -2.0f)
               {
               old_heading += 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Missile homing: heading out of limits, so add" << endl;
#endif
               } // end of right turn

            // Missile must turn left to target
            else
               {
               old_heading -= 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Missile homing: heading out of limits, so subtract" << endl;
#endif
               } // end of left turn

            // Limit missile heading between 0 to 359 degrees
            while (old_heading >= 360.0f)
               old_heading -= 360.0f;
            while (old_heading < 0.0f)
               old_heading += 360.0f;

            // Set the missile's new heading
            posture.hpr[HEADING] = old_heading;

          } //end target still in range so move towards it

        } //end (tgt_aquired)


//if you are cruising and don't have a target you must control height----------
    if ( (is_cruising) && (!target_acquired) && (is_on_initial_course) )
      {
        pfMatrix pitch_orient;
        pfVec3 pitch_direction = { 0.0f, 1.0f, 0.0f };
        if (speed < 250.0f)
        speed += 2.0;


       // Calculate orientation of missile based on new HPR
       pfMakeEulerMat ( pitch_orient, posture.hpr[HEADING],
                                      posture.hpr[PITCH],
                                      posture.hpr[ROLL]);

      // Calculate vector out of the nose of the missile
      pfFullXformPt3 (pitch_direction, pitch_direction, pitch_orient );

      // Normalize direction vector of missile to length 1
      pfNormalizeVec3 ( pitch_direction );
  
      // Set the velocity vector for calculating distance in this frame
      pfScaleVec3 ( velocity, speed, pitch_direction);

      // Figure out the distance we traveled in the next  frame
      pfScaleVec3 ( move_dist, (2 * deltatime), velocity );

      // Move missile from last XYZ posit to new XYZ posit
      pfSetVec3( pitchposture ,0.0, 0.0, 0.0);
      pfAddVec3 ( pitchposture, posture.xyz, move_dist );

      future_altitude = get_altitude( pitchposture ) ;
      delta_altitude = 50.0 - future_altitude;

      if (abs(delta_altitude) <= 4.0) //within dead band so level out
        {
         if (posture.hpr[PITCH] > 0.0)
           posture.hpr[PITCH] -= 1.0;
         else if(posture.hpr[PITCH] < 0.0)
           posture.hpr[PITCH] += 1.0;
        }
    
      else if (abs(delta_altitude) <= 30.0) 
           {
           if (delta_altitude > 0.0)
             posture.hpr[PITCH] += 1.0;
           else
             posture.hpr[PITCH] -= 1.0;
           }
      else 
         {
           if (delta_altitude > 0.0)
             posture.hpr[PITCH] += 2.0;
           else
             posture.hpr[PITCH] -= 2.0;
        }
     
      //clamp posture.hpr[PITCH] to +- 10.0
      if (posture.hpr[PITCH] > 10.0)
          posture.hpr[PITCH] = 10.0;
      if (posture.hpr[PITCH] < -10.0)
          posture.hpr[PITCH] = -10.0;

      } //end (is_cruising) && (!target_acquired) && (is_on_initial_course)

//if you are cruising and you do have a tgt aquired, control height and-------------
 //course normally-----------------------------------------------------------------
    if ( (is_cruising) && (target_acquired) && (is_on_initial_course) )
      {
       if (speed < 150.0f)
        speed += 2.0;
       
        // Test to see if the target has died or moved out of range.
         //  If so, we need to acquire another target.
         if ((G_vehlist[target_id].vehptr == NULL) ||
             ( pfDistancePt3( posture.xyz, 
                              G_vehlist[target_id].vehptr->getposition())
               > (MAX_TORP_RANGE + 1000.0f)))
            {
            target_acquired = 0;
          } // end target out of range

         // Target still in range, so move towards it
         else
          {
          // Initialize missileedo's HPR and XYZ from last frame
            old_heading = posture.hpr[HEADING];
            old_pitch = posture.hpr[PITCH];
            entity_x = posture.xyz[X];
            entity_y = posture.xyz[Y];
            entity_z = posture.xyz[Z];

            // Find the location and distance of the current target
            pfCopyVec3(target_loc, G_vehlist[target_id].vehptr->getposition());
            target_dist = pfDistancePt3(target_loc, posture.xyz);

            // Calculate bearing to target
            bearing_angle = RAD_TO_DEG * (atan2f((target_loc[Y] - entity_y),
                                   (target_loc[X] - entity_x)) - HALFPI);

            // Limit bearing angle between 0 to 359 degrees
            while (bearing_angle >= 360.0f)
               bearing_angle -= 360.0f;
            while (bearing_angle < 0.0f)
               bearing_angle += 360.0f;

       // Missile is almost pointed at the target, so reset to new bearing
            if (((old_heading - bearing_angle) >= -2.0f) &&
                ((old_heading - bearing_angle) <= 2.0f))
               {
               old_heading = bearing_angle;
                } // end of heading and bearing being close

            // Missile must turn right to target
            else if ((old_heading - bearing_angle) < -2.0f)
               {
               old_heading += 1.0f;
               } // end of right turn
            // Missile must turn left to target
            else
               {
               old_heading -= 1.0f;
               } // end of left turn

            // Limit missileedo heading between 0 to 359 degrees
            while (old_heading >= 360.0f)
               old_heading -= 360.0f;
            while (old_heading < 0.0f)
               old_heading += 360.0f;

            // Set the missile='s new heading
            posture.hpr[HEADING] = old_heading;
               
            // Calculate pitch angle to target
            pitch_angle = RAD_TO_DEG * asinf((target_loc[Z] - 
                                              entity_z + 1.0f)/target_dist);

            // Missile is almost pointed at the target, so reset to new pitch
            if (((old_pitch - pitch_angle) >= -2.0f) &&
                ((old_pitch - pitch_angle) <= 2.0f))
               {
               posture.hpr[PITCH] = pitch_angle;
               } // end if pitch to target and current pitch are similar

            // Missile is lower than target, so pitch up
            else if ((old_pitch - pitch_angle) < -2.0f)
               {
               posture.hpr[PITCH] += 1.0f;
              } 

            // Missile is higher than target, so pitch down
            else
               {
               posture.hpr[PITCH] -= 1.0f;
               }


            } // end else target in range
     
      } //end (is_cruising) && (target_acquired) && (is_on_initial_course) 
 

    } //end !is_underwater


// update Missile position---------------------------------------------------
    // Roll not needed for missile underwater, so set to 0
      posture.hpr[ROLL] = 0.0f;

      // Calculate orientation of missile based on new HPR
      pfMakeEulerMat ( orient, posture.hpr[HEADING],
                               posture.hpr[PITCH],
                               posture.hpr[ROLL]);
//cerr<<"missile heading at update "<< posture.hpr[HEADING]<<endl;
//cerr<<"missile pitch at update "<< posture.hpr[PITCH]<<endl;
//cerr<<"missile roll at update "<< posture.hpr[ROLL]<<endl;
      // Calculate vector out of the nose of the missile
      pfFullXformPt3 ( direction, direction, orient );

      // Normalize direction vector of missile to length 1
      pfNormalizeVec3 ( direction );

//CUTcerr<<"MISSILE SPEED "<<speed<<endl; 
      // Set the velocity vector for calculating distance in this frame
      pfScaleVec3 ( velocity, speed, direction);

      // Figure out the distance we traveled in this frame
      pfScaleVec3 ( move_dist, deltatime, velocity );

      // Move missile from last XYZ posit to new XYZ posit
      pfAddVec3 ( posture.xyz, posture.xyz, move_dist );

      // Add the amount we moved in this frame to the total distance 
      //  traveled since being underwater
      travelled_dist += pfLengthVec3(move_dist);

      // Once we have traveled our maximum range, kill the missile
      if (travelled_dist > MAX_MISSLE_RANGE)
         {
         deactivate_missile();
#ifdef DEBUG_TORP
   cerr << "Missile died: end of run" << endl;
#endif
         return(DEACTIVE);
         } // end kill missile at its max underwater range



   // Video missile was selected, so calculate where to look and open the 
   //  viewing channel
   if (video)
      {
      // Offset the direction to be out in front of the missile
      pfScaleVec3 ( offset, -5.0f, direction );

      // Add offset to current XYZ to look from the correct point in space
      pfAddVec3 ( offset, offset, posture.xyz );

      // Open the vidoe missile/torp channel
      pfChanView(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                 offset, posture.hpr);
#ifdef DEBUG_TORP
   cerr << "Video Missile is on." << endl;
#endif
      } // end video missile calculation

   // If the missile is out of bounds, kill it
   if(!inplay(posture.xyz))
      {
      deactivate_missile();
      return(DEACTIVE);
      } // end if missile out-of-bounds
     
   // Test to see if the missile hit something.  If it did, kill it
   if(impactdetermination(oldpnt,posture.xyz) == HITSOMETHING)
      {
      // missile deo hit something -- call its own member function
      deactivate_missile();
      return(DEACTIVE);
      } // end if missile impacted anything

   // Find out if we have to sent out an entity state PDU
   cumtime = curtime - lastPDU;
   pfScaleVec3(tempvec,cumtime,oldvel);
   pfAddVec3(tempvec,drparam.xyz,tempvec);
   if((cumtime > G_static_data->DIS_heartbeat_delta) ||
      (pfSqrDistancePt3(tempvec,posture.xyz)>G_static_data->DIS_position_delta))
      { 
      // It exceeds the dead reckoning parameters, so send an update
      sendentitystate();

      //update the DRparameters
      PFCOPY_VEC3(drparam.xyz,posture.xyz);
      lastPDU = curtime;
      } // end send a new entity state PDU for DR

#ifdef DEBUG_TORP
   cerr << "Missile location at X " << posture.xyz[X] <<
                             ", Y " << posture.xyz[Y] << 
                             ", Z " << posture.xyz[Z] << endl;
   cerr << "Missile orientation in H " << posture.hpr[HEADING] <<
                                ", P " << posture.hpr[PITCH] << 
                                ", R " << posture.hpr[ROLL] << endl;
   cerr << "Missile velocity in X " << velocity[X] <<
                             ", Y " << velocity[Y] << 
                             ", Z " << velocity[Z] << endl;
#endif

   // Put the missile in the correct location in space
   pfDCSTrans(dcs,posture.xyz[X],posture.xyz[Y],posture.xyz[Z]);
   pfDCSRot(dcs,posture.hpr[HEADING],posture.hpr[PITCH],posture.hpr[ROLL]);
#ifdef DEBUG_SCALE
   pfDCSScale(dcs,10.0f);
#endif

   return(FINEWITHME);
} // end MISSILE_SL move()




// This function is intended to identify the correct launch effect of
//  the missile.  Currently, there is no effect.
void MISSILE_SL::launcheffect()
//a virtual function that does not do anything
{
   /*do nothing, when you launch a bomb nothing visual happens*/
}


// This function returns the type of vehicle this missile is
mtype MISSILE_SL::gettype() 
{ 
   return (MISSILE_SL_TYPE);
}


void MISSILE_SL::sendfire(int mid,VEHICLE *platform,int newcat, PASS_DATA *,
                        float deltaspeed, int target )
//sends out the fire event for the munition;
//also fills in a lot of the fields
{

  FirePDU Fpdu;
  static teamMember = -1;
  int weapon_index;
  WEAPON_LIST *weapptr =  &(G_weaplist[mid]);
  pfMatrix orient;
  pfVec3 front = { 0.0f, 1.0f, 0.0f };
  pfVec3 right = { 1.0f, 0.0f, 0.0f };
  pfVec3 up = { 0.0f, 0.0f, 1.0f };
  long mask;
  pfCoord flashpt;
  JACK_TEAM_VEH *jack_team;
  static pfVec3 flashpt_xyz;

  if(platform->gettype() == JACK_TEAM) {
     jack_team = (JACK_TEAM_VEH *) platform;
     teamMember = (teamMember + 1) % jack_team->getTeamCount();
  }


  //do a little house cleaning first
  dcs = weapptr->dcs; 
  pfSwitchVal(weapptr->icon,(long)newcat);

  bzero ( (char *)&Fpdu, sizeof(FirePDU) );

/*
  if ( G_dynamic_data->exercise < 0 )
     Fpdu.fire_header.exercise_ident = (unsigned char)1;
  else
     Fpdu.fire_header.exercise_ident =
        (unsigned char)G_dynamic_data->exercise;
*/
 
  /*fill in the Fire, Target (Unkown), and Munition ID*/
  Fpdu.firing_entity_id.address.site = platform->disname.address.site;
  Fpdu.firing_entity_id.address.host = platform->disname.address.host;
  Fpdu.firing_entity_id.entity = platform->disname.entity;

  firedfrom = platform->disname.entity;

  if ( gettype() == BULLET_TYPE )
     {
     /* non-tracked munition */
     Fpdu.munition_id.address.site = 0;
     Fpdu.munition_id.address.host = 0;
     Fpdu.munition_id.entity = 0;
     disname.address.site = 0;
     disname.address.host = 0;
     disname.entity = 0;
     }
  else
     {
     Fpdu.munition_id.address.site = platform->disname.address.site;
     Fpdu.munition_id.address.host = platform->disname.address.host;
     Fpdu.munition_id.entity = MUNITION_OFFSET + mid; 
     disname.address.site = platform->disname.address.site;
     disname.address.host = platform->disname.address.host;
     disname.entity = MUNITION_OFFSET + mid;
     }


  //don't admit who we are shoting at
  if ( target == -1 )
     {
     Fpdu.target_entity_id.address.host = 0; 
     Fpdu.target_entity_id.address.site = 0; 
     Fpdu.target_entity_id.entity = 0; 
     }
  else
     {
     Fpdu.target_entity_id.address.host = G_vehlist[target].DISid.address.host;
     Fpdu.target_entity_id.address.site = G_vehlist[target].DISid.address.site; 
     Fpdu.target_entity_id.entity = G_vehlist[target].DISid.entity;
     }
 
  /*the event ID*/
  Fpdu.event_id.address.site = disname.address.site;
  Fpdu.event_id.address.host = disname.address.host;

//  Fpdu.event_id.event = ++event_cnt;

  G_weaplist[mid].eventid = Fpdu.event_id.event = ++event_cnt;

  weapon_index = G_vehtype[platform->icontype].fireinfo[newcat].munition;

//cerr << "Sending a fire " << weapon_index << " from " << platform->icontype
//     << endl;

  /*note: we are using the SW corner of the Database as the origin, NOT
    WGS 84 as it calls for in the DIS Spec*/

  // For DIs flashpt at end of rifle but bullet goes in where DI is looking
  pfVec3 t_front,t_right,t_up;

 
     //pfVec3 eye_point;
     //pfVec3 aim_direction;
     //pfVec3 aim_point;
     //int aim_point_valid;

     posture = platform->posture;
     posture.hpr[HEADING] =  platform->posture.hpr[HEADING];

     
//cerr<<"SUB HEADING "<<platform->posture.hpr[HEADING]<<endl;
//cerr<<"SUB PITCH "<<platform->posture.hpr[PITCH]<<endl;
//cerr<<"MISSILE HEADING "<<posture.hpr[HEADING]<<endl;
//cerr<<"MISSILE PITCH "<<posture.hpr[PITCH]<<endl<<endl;

     pfMakeEulerMat ( orient, posture.hpr[HEADING],
                           posture.hpr[PITCH],
                           posture.hpr[ROLL]);
     pfFullXformPt3 ( t_front, front, orient );
     pfFullXformPt3 ( t_right, right, orient );
     pfFullXformPt3 ( t_up, up, orient );

pfScaleVec3 ( t_front,
                G_vehtype[platform->icontype].fireinfo[newcat].firepoint[Y],
                t_front );
     pfScaleVec3 ( t_right,
                G_vehtype[platform->icontype].fireinfo[newcat].firepoint[X],
                t_right );
     pfScaleVec3 ( t_up,
                G_vehtype[platform->icontype].fireinfo[newcat].firepoint[Z],
                t_up);

     pfAddVec3 (posture.xyz, posture.xyz, t_front);
     pfAddVec3 (posture.xyz, posture.xyz, t_up);
     pfAddVec3 (posture.xyz, posture.xyz, t_right);

//cerr<<"MISSILE HEADING "<<posture.hpr[HEADING]<<endl;
//cerr<<"MISSILE PITCH "<<posture.hpr[PITCH]<<endl<<endl;

     posture.hpr[PITCH] += 90.0;

//cerr<<"MISSILE HEADING "<<posture.hpr[HEADING]<<endl;
//cerr<<"MISSILE PITCH "<<posture.hpr[PITCH]<<endl<<endl;

     flashpt = posture;

  
 

 
     Fpdu.location_in_world.x = posture.xyz[X];
     Fpdu.location_in_world.y = posture.xyz[Y];
     Fpdu.location_in_world.z = posture.xyz[Z];
  

  pfDCSTrans(dcs,posture.xyz[X],posture.xyz[Y],posture.xyz[Z]);

  mask = PRUNE_MASK;
  set_intersect_mask((pfGroup *)weapptr->dcs,
                     mask, COLLIDE_DYNAMIC);
  pfSync();
  pfAddChild ( G_moving, (pfGroup *)weapptr->dcs );
 
//  platform->get_firepoint(this->posture.xyz);

  force = platform->force;

  /*what did we shoot*/
  type = G_vehtype[weapon_index].distype;
  type.specific = (unsigned char)G_dynamic_data->specific;
  Fpdu.burst_descriptor.munition =  type;
  Fpdu.burst_descriptor.warhead = WarheadMunition_HighExp;
  Fpdu.burst_descriptor.fuze = FuzeMunition_Contact;
  Fpdu.burst_descriptor.quantity = 1; /*One at a time*/
  Fpdu.burst_descriptor.rate = 0;
/*
  mask = (long(type.entity_kind) << ENTITY_KIND_SHIFT);
  mask = mask | (long(type.domain) << ENTITY_DOMAIN_SHIFT);
  mask = mask | (long(force) << ENTITY_FORCE_SHIFT);
*/

#ifdef DEBUG
cerr << "Cat " << cat << " Index " << cat+START_WEAPON 
     << " " << G_vehtype[cat+START_WEAPON].name << "\n";
cerr << "DIS Type "<< 0 +type.entity_kind <<" "<< 0 +type.domain
        <<" "<<type.country<<" "<<0 +type.category<<" ";
cerr <<0 +type.subcategory<<" "<<0 +type.specific<<" Firesend\n";
#endif


  pfCopyVec3 ( velocity, platform->velocity );
  set_speed(deltaspeed);
  /*movement parameters*/
  Fpdu.velocity.x = velocity[X];
  Fpdu.velocity.y = velocity[Y];
  Fpdu.velocity.z = velocity[Z];

  /*how far can it go*/
  Fpdu.range = WEAP_RANGE;

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          /*send it*/
          if (G_net->write_pdu((char *) &Fpdu, (PDUType )FirePDU_Type) == FALSE)
            cerr << "write_pdu() failed\n";
          break;
       }

  //identify_effect ( &type, &effect );

   if ( G_static_data->sound )
      {
      LocationRec dummy_snd_loc;
      dummy_snd_loc.x = posture.xyz[X];
      dummy_snd_loc.y = posture.xyz[Y];
      dummy_snd_loc.z = posture.xyz[Z];
      playsound(&dummy_snd_loc,&dummy_snd_loc,effect.fire_local_snd,-1);
      }

  if(((platform->gettype() == JACK_TEAM) && (teamMember > 0))) {
     // create rifle puffs for each of the other team members
     play_effect(effect.fire_effect, flashpt_xyz, flashpt.hpr, FALSE);
  } // end if JACK_TEAM/UNIT and not main member
  else {
     play_effect(effect.fire_effect, flashpt.xyz,flashpt.hpr, FALSE);
  }

#ifdef DEBUG
   cout << "Sent a FirePDU\n";
   cout << "site " << Fpdu.munition_id.address.site
        << " host " << Fpdu.munition_id.address.host
        << " entity " << Fpdu.munition_id.entity << endl;
#endif
}
#endif // NOSUB



// *******************************************************************
// This section to end added by Fred Lentz
//
// This function is the constructor for the torpedo.  Its purpose
//  is to initialize variables and build an array of eligible targets,
//  namely ships or submarines, that are in the simulation

TORPEDO_AL::TORPEDO_AL(int mid) : MUNITION (mid)
{
   // Initialize variables for calculation of trajectory
   travelled_dist = 0.0f;
   is_underwater = 0;

   // Initialize variables for array of eligible ships and subs
   int target_num = 0;
   int ix = 0;

   // Initialize variables for targetting info for the torp
   target_acquired = 0;
   target_id = -1;

   // Make an array of maritime vehicles that are eligible to be hit by the
   //  torpedo.  The closest one at water impact will be the target.
   while ( target_num < 249 )
      {
      // Initailize all array elements to 0
      maritime_targets[target_num] = 0;

      // If it is a ship or sub, add its id to the array
      if (( G_vehlist[target_num].vehptr != NULL) &&
          (( G_vehlist[target_num].vehptr->gettype() == SUBMERSIBLE ) || 
           ( G_vehlist[target_num].vehptr->gettype() == SHIP )))
         {
         // id is eligible, so add to array
         maritime_targets[ix] = G_vehlist[target_num].id;

         // Increment counter to next array element
         ix++;

         // Increment counter to search all active vehicles
         target_num++;
         } // end if ship or sub

      else // not a ship or sub, so try the next vehicle
         target_num++;
      } // end of searching through vehicle list

   // Video torpedo was selected, so turn it on
#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   if (G_dynamic_data->visual_mode)
      {
      G_dynamic_data->visual_active = 1;
      G_dynamic_data->visual_mode = 0;
      video = 1;
      pfChanTravMode(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                     PFTRAV_DRAW, PFDRAW_ON);
      pfChanView(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                 posture.xyz, posture.hpr);
      } // end if video torpedo selected

   else // video torpedo not selected
      video = 0;

#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
} // end TORPEDO_AL constructor


// This function kills the torpedo, and sends an entity state 
//  PDU to kill it from the simulation.  It also turns off video
//  torpedo.

void TORPEDO_AL::deactivate_torpedo ()
{
#ifdef DIS_2v4
   unsigned int torpedo_al_disappear = getstatus();
   torpedo_al_disappear |= Appearance_Munition_Status_Deactivated;
#endif

   // If video torpedo was turned on, turn it off
   if (video) {
#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
      pfChanTravMode(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                    PFTRAV_DRAW, PFDRAW_OFF);
      G_dynamic_data->visual_active = 0;
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
   } // end if video

#ifdef DIS_2v4
   changestatus(torpedo_al_disappear);
   sendentitystate();
#endif
#ifdef DEBUG_TORP
   cerr << "Torpedo destroyed at X " << posture.xyz[X] <<
                              ", Y " << posture.xyz[Y] << 
                              ", Z " << posture.xyz[Z] << endl;
#endif
}


// This function determines if the torpedo hit anything in this frame.  
//  Functionality has been added to allow the torpedo to hit the water 
//  and remain alive.  Functionality has also been added to simulate 
//  that the torpedo stays in the confines of the water if it is in 
//  the water.

movestat TORPEDO_AL::impactdetermination(pfVec3 startpnt, pfVec3 endpnt)
{
   long isect; //did we hit anything
   pfSegSet seg;
   pfHit    **hits[32];
   pfVec3   pnt;
   pfMatrix xform;
   pfVec3   normal;
   long     flags;
//   pfNode   *node;
   pfGeoSet *gset;
   pfVec3 hpr;

   // Set up the segment representing the path of the torp in this frame
   pfSetVec3(seg.segs[0].dir,
         (endpnt[X] - startpnt[X]),
         (endpnt[Y] - startpnt[Y]),
         (endpnt[Z] - startpnt[Z]));
   seg.segs[0].length = pfLengthVec3(seg.segs[0].dir);
   PFCOPY_VEC3(seg.segs[0].pos,startpnt);
   //fix a performer bug
   if(seg.segs[0].dir[Y] == 0.0f) seg.segs[0].dir[Y] = 0.01f;
   if(seg.segs[0].dir[X] == 0.0f) seg.segs[0].dir[X] = 0.01f;
   pfNormalizeVec3(seg.segs[0].dir);

   // See if the torp hit a vehicle
   if(pfGetNumChildren(G_moving))
      {
      seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      seg.userData = NULL;
      seg.activeMask = 0x01;
      seg.isectMask = ENTITY_MASK;
      seg.bound = NULL;
      seg.discFunc = vehiclehit;
      isect = pfNodeIsectSegs(G_moving, &seg, hits);

      // We hit a vehicle, so blow up the torp and the vehicle
      if (isect) 
         {
         //set the point at the intersection point in the world COORDINATES
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit vehicle" << endl;
#endif
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
         pfFullXformPt3(endpnt, pnt, xform);
         senddetonation(endpnt,pnt,hitveh());
#ifdef DEBUG_X
     cout << "\n\nVehicle hit at " << endpnt[X] << " "
                               << endpnt[Y] << " "
                               << endpnt[Z] << endl;
#endif
         // Score is being kept, so increment the number of hits
         if ( G_static_data->scoring )
            {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->dynamic_hits += 1;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data ); 
#endif
            } // end if scoring

         // A vehicle was hit, so return to caller that we did
         return(HITSOMETHING);
         } // end if hit vehicle

      } // end if G_moving

   // Intersect test against the fixed objects.  Only tried if the torp
   //  is 1000 meters or lower
   if(endpnt[Z] < 1000.0f)
      {
      seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      seg.userData = NULL;
      seg.activeMask = 0x01;
      seg.isectMask = DESTRUCT_MASK;
      seg.bound = NULL;
      seg.discFunc = buildinghit;
      isect = pfNodeIsectSegs(G_fixed, &seg, hits);

      // The torp hit a building
      if (isect)
         {
         // Set the point at the intersection point in the world COORDINATES
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit building" << endl;
#endif
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
//       pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
//       pfFullXformPt3(endpnt, pnt, xform);
#ifdef DEBUG_X
   cout << "\n\nBuilding hit by weapon at " << endpnt[X] << " "
                               << endpnt[Y] << " "
                               << endpnt[Z] << endl;
#endif
         // If the desired effect is an explosion, do it
         if ( effect.det_effect == EXPLOSION )
            {
            use_next_fire_plume(pnt[X],pnt[Y],pnt[Z], 20.);
            } // end if EXPLOSION

         // Send a detonation PDU
         senddetonation(pnt,pnt);

         // Score is being kept, so increment the number of hits
         if ( G_static_data->scoring )
            {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->static_hits += 1;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
            } // end if scoring

         // We hit a building, so we can return from this function
         return(HITSOMETHING);
         } // end if hit a building

      } // end if Z less than 1000

      // Intersect test against the ground
      seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      seg.userData = NULL;
      seg.activeMask = 0x01;
      seg.isectMask = PERMANENT_MASK;
      seg.bound = NULL;
      seg.discFunc = NULL;
      if (pfNodeIsectSegs(G_fixed, &seg, hits))
         {
         // Find the orientation of the crater
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit ground" << endl;
#endif
         long the_mask;
//         pfQueryHit(*hits[0], PFQHIT_NODE, &node);
//         the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
         
         pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
         the_mask = pfGetGSetIsectMask(gset);

         // The torp hit the ground
         if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
              == PERMANENT_DIRT )
            {
            pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
            pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
            pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
            if (flags & PFHIT_NORM)
               {
               // Figure out the orientation of the crater
               pfVec3 head, head90;
               float dotp;
               hpr[HEADING]= 0.0f;
               head[0] = 0.0f;
               head[1] = 1.0f;
               head[2] = 0.0f;
               dotp = PFDOT_VEC3(head, normal);
               hpr[PITCH] = pfArcCos(dotp) - 90.0f;
               head90[0] =  1.0f;
               head90[1] = 0.0f;
               head90[2] = 0.0f;
               dotp = PFDOT_VEC3(head90, normal);
               hpr[ROLL]  = 90.0f - pfArcCos(dotp);
#ifdef DEBUG
           cerr << "Crater 1 " << hpr[X] << "  "<<hpr[Y]<<"  " <<hpr[Z] <<"\n";
#endif
               } // end if flags & PFHIT_NORM

            /* Leave a creater at that location*/
#ifdef DEBUG_X
       cout << "\n\nGround hit at " << pnt[X] << " "
                            << pnt[Y] << " "
                            << pnt[Z] << endl;

#endif
            // If the desired effect was an explosion, do it and leave a
            //  crater.
            if ( effect.det_effect == EXPLOSION )
               {
               makecrater(pnt,hpr);

               // Generate smoke
               use_next_fire_plume(pnt[X], pnt[Y], pnt[Z],20.0f);
               } // end if EXPLOSION

            // Send detonation PDU
            senddetonation(pnt);
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit dirt" << endl;
#endif
            // The torp impacted the ground, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_DIRT   

         // We hit the water, so change the status of the torpedo so
         //  it trajectory will be calculated by different methods
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_WATER )
            {
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit water" << endl;
#endif
            
            is_underwater = 1;

            // Still lives if hit water, so do not return that anything was hit
            return(FINEWITHME);
            } // end if PERMANENT_WATER

         // The torp hit the trees
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_FOILAGE)
            {
            // Make an explosion if needed
            if ( effect.det_effect == EXPLOSION )
               {
               // Generate smoke
               use_next_fire_plume(pnt[X], pnt[Y], pnt[Z],20.0f);
               } // end if EXPLOSION

            // Send detonation PDU
            senddetonation(DetonationResult(DetResult_Detonation));
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit foilage" << endl;
#endif
            // The torp impacted trees, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_FOILAGE

         // The torp hit tunnel
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_TUNNEL)
            {
            // Send detonation PDU
            senddetonation(DetonationResult(DetResult_Detonation));
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit tunnel" << endl;
#endif
            // The torp impacted tunnel, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_TUNNEL

         // The torp hit bridge
         else if ( int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT)
               == PERMANENT_BRIDGE )
            {
            // Send detonation PDU
            senddetonation(DetonationResult(DetResult_Detonation));
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit bridge" << endl;
#endif
            // The torp impacted bridge, so return that we did
            return(HITSOMETHING);
            } // end if PERMANENT_BRIDGE

         } // end if PERMANENT_MASK

      // Torp stays alive in the boundary of the water.  If any part of
      //  the testing segment touches ground in the +Z direction, this
      //  is considered the same as impacting the ground underwater.
      if (is_underwater)
         {
         // Set up a long segment for intersection testing straight
         //  up from the torp in the +Z direction.
         PFSET_VEC3(seg.segs[0].dir, 0.0f, 0.0f, 1.0f);
         pfNormalizeVec3(seg.segs[0].dir);
         seg.segs[0].length = 20000.0f;

         // Test to see if the segment intersected anything fixed
         if (pfNodeIsectSegs(G_fixed, &seg, hits))
            {
            long the_mask;
//            pfQueryHit(*hits[0], PFQHIT_NODE, &node);
//            the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);

            pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
            the_mask = pfGetGSetIsectMask(gset);
 
            // If the segment intersected anything but water, detonate it
            if ( int((the_mask & PERMANENT_MASK) >> PERMANENT_SHIFT)
                != PERMANENT_WATER)
               {   
               // Send detonation PDU
               senddetonation(posture.xyz);               
#ifdef DEBUG_TORP
   cerr << "Torpedo impactdetermination(): hit ground underwater" << endl;
#endif
               // The torp impacted the ground, so return that we did
               return(HITSOMETHING);
               } // end if we hit something other than water

            } // end if we have intersected something fixed

         } // end if underwater collision detection 

   // If we got here, it did not hit anything, so return that it didn't
   return(FINEWITHME);

} // end TORPEDO_AL :: impactdetermination()


// This is the function that determines the movement of the torpedo in 
//  each frame of the simulation.  If the torpedo is in the air, it follows
//  the same ballistic path of the bomb in BOMB:move().  Once underwater,
//  the torp follows a straight path to the closest target in its range.

movestat TORPEDO_AL::move(float deltatime,float curtime)
{
   float cumtime;  //the time since the last pdu was sent out
   pfVec3 oldpnt,oldvel;
   pfVec3 accel;
   float  time2;
   pfVec3 tempvec;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 offset;

   // New variables for target following underwater
   pfVec3 move_dist;
   float entity_x,entity_y,entity_z;
   pfVec3 target_loc;
   float bearing_angle,pitch_angle;
   float old_heading = 0.0f;
   float old_pitch = 0.0f;
   float target_dist;
   pfMatrix orient;
   float speed;

#ifdef TRACE
   cerr <<"Moved -- torpedo_al\n";
#endif

   // Test to see if the torpedo stopped.  If so, deactive it.
   if((velocity[X] == 0.0f) &&
      (velocity[Y] == 0.0f) &&
      (velocity[Z] == 0.0f))
      {
      deactivate_torpedo();
      return(DEACTIVE);
      } // end if torpedo stopped

   // This is the old position of torpedo
   PFCOPY_VEC3(oldpnt,posture.xyz);
   PFCOPY_VEC3(oldvel,velocity);

   // Compute the new position if the torp is underwater
   if (is_underwater)
      {
      speed = pfLengthVec3(velocity);

      // No target designated, so find one is the array of ships/subs
      if ( !target_acquired )
         {
         // Initialize the counter of the array
         int ix = 0;

         // Initialize distance to maximum acquisition range
         float dist_to_target = (MAX_TORP_RANGE + 1000.0f);

         // No target designated yet
         target_id = 0;
#ifdef DEBUG_ACQUIRE
   cerr << "Torpedo: trying to acquire" << endl;
#endif

         // Search target array until no more targets in the array
         //  (empty elements are initialized to 0).
         while (maritime_targets[ix] != 0)
            {
#ifdef DEBUG_ACQUIRE
   cerr << "Torpedo acquisition loop" << endl;
#endif
            // Check to see if the id pointed to is valid and closest
            if ((G_vehlist[maritime_targets[ix]].vehptr == NULL) ||
                ( pfDistancePt3( posture.xyz, 
                      G_vehlist[maritime_targets[ix]].vehptr->getposition())
                  > dist_to_target))
               {
               // Target is invalid or not the closest, so try keep trying
               ix++;
#ifdef DEBUG_ACQUIRE
   cerr << "No vehicle or too far away" << endl;
#endif
               } // end if NULL or farther than the last one

            // Target is valid and closer than the last one, so use it
            else
               {
               // Make the distance the new shorter one so we find the
               //  shortest once we have tested all eligible vehicles
               dist_to_target = pfDistancePt3( posture.xyz, 
                      G_vehlist[maritime_targets[ix]].vehptr->getposition());

               // Set the target id to this closer target
               target_id = maritime_targets[ix];

               // Increment counter to test next available target
               ix++;

               // We have acquired at least one target
               target_acquired = 1;
               } // end valid and closer target

            } // end while maritime_targets[ix] != 0

         } // end no target acquired

      // We have already acquired our target, so move towards it
      else
         {
         // Test to see if the target has died or moved out of range.
         //  If so, we need to acquire another target.
         if ((G_vehlist[target_id].vehptr == NULL) ||
             ( pfDistancePt3( posture.xyz, 
                              G_vehlist[target_id].vehptr->getposition())
               > (MAX_TORP_RANGE + 1000.0f)))
            {
            target_acquired = 0;
#ifdef DEBUG_HOMING
   cerr << "Torpedo target now out of range" << endl;
#endif
            } // end target out of range

         // Target still in range, so move towards it
         else
            {
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing to target" << endl;
#endif
            // Initialize torpedo's HPR and XYZ from last frame
            old_heading = posture.hpr[HEADING];
            old_pitch = posture.hpr[PITCH];
            entity_x = posture.xyz[X];
            entity_y = posture.xyz[Y];
            entity_z = posture.xyz[Z];

            // Find the location and distance of the current target
            pfCopyVec3(target_loc, G_vehlist[target_id].vehptr->getposition());
            target_dist = pfDistancePt3(target_loc, posture.xyz);

            // Calculate bearing to target
            bearing_angle = RAD_TO_DEG * (atan2f((target_loc[Y] - entity_y),
                                   (target_loc[X] - entity_x)) - HALFPI);

            // Limit bearing angle between 0 to 359 degrees
            while (bearing_angle >= 360.0f)
               bearing_angle -= 360.0f;
            while (bearing_angle < 0.0f)
               bearing_angle += 360.0f;

#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: bearing angle is " << bearing_angle << endl;
   cerr << "Torpedo homing: torp heading is " << old_heading << endl;
#endif
            // Torp is almost pointed at the target, so reset to new bearing
            if (((old_heading - bearing_angle) >= -2.0f) &&
                ((old_heading - bearing_angle) <= 2.0f))
               {
               old_heading = bearing_angle;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: heading in limits, so just turn towards" << endl;
#endif
               } // end of heading and bearing being close

            // Torp must turn right to target
            else if ((old_heading - bearing_angle) < -2.0f)
               {
               old_heading += 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: heading out of limits, so add" << endl;
#endif
               } // end of right turn

            // Torp must turn left to target
            else
               {
               old_heading -= 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: heading out of limits, so subtract" << endl;
#endif
               } // end of left turn

            // Limit torpedo heading between 0 to 359 degrees
            while (old_heading >= 360.0f)
               old_heading -= 360.0f;
            while (old_heading < 0.0f)
               old_heading += 360.0f;

            // Set the torpedo's new heading
            posture.hpr[HEADING] = old_heading;
               
            // Calculate pitch angle to target
            pitch_angle = RAD_TO_DEG * asinf((target_loc[Z] - 
                                              entity_z + 1.0f)/target_dist);

            // Torp is almost pointed at the target, so reset to new pitch
            if (((old_pitch - pitch_angle) >= -2.0f) &&
                ((old_pitch - pitch_angle) <= 2.0f))
               {
               posture.hpr[PITCH] = pitch_angle;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: pitch in limits, so assign" << endl;
#endif
               } // end if pitch to target and current pitch are similar

            // Torp is lower than target, so pitch up
            else if ((old_pitch - pitch_angle) < -2.0f)
               {
               posture.hpr[PITCH] += 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: pitch out of limits, so add" << endl;
#endif
               } 

            // Torp is higher than target, so pitch down
            else
               {
               posture.hpr[PITCH] -= 1.0f;
#ifdef DEBUG_HOMING
   cerr << "Torpedo homing: pitch out of limits, so subtract" << endl;
#endif
               }

#ifdef DEBUG_HOMING
   cerr << "Torpedo end homing to target" << endl;
#endif

            } // end else target in range

         } // end if target already acquired
  
      // Roll not needed for torpedo underwater, so set to 0
      posture.hpr[ROLL] = 0.0f;

      // Calculate orientation of torpedo based on new HPR
      pfMakeEulerMat ( orient, posture.hpr[HEADING],
                               posture.hpr[PITCH],
                               posture.hpr[ROLL]);

      // Calculate vector out of the nose of the torp
      pfFullXformPt3 ( direction, direction, orient );

      // Normalize direction vector of torp to length 1
      pfNormalizeVec3 ( direction );

      // If speed of torp underwater is faster than 50, decel
      if ( speed > 50.0f )
         speed -= 2.0f;

      // If speed of torp underwater is slower than 50, accel
      else if ( speed < 45.0f )
         speed += 2.0f;

      // Speed is in an acceptable range, to make speed 50
      else
         speed = 50.0f;

      // Set the velocity vector for calculating distance in this frame
      pfScaleVec3 ( velocity, speed, direction);

      // Figure out the distance we traveled in this frame
      pfScaleVec3 ( move_dist, deltatime, velocity );

      // Move torp from last XYZ posit to new XYZ posit
      pfAddVec3 ( posture.xyz, posture.xyz, move_dist );

      // Add the amount we moved in this frame to the total distance 
      //  traveled since being underwater
      travelled_dist += pfLengthVec3(move_dist);

      // Once we have traveled our maximum range, kill the torp
      if (travelled_dist > MAX_TORP_RANGE)
         {
         deactivate_torpedo();
#ifdef DEBUG_TORP
   cerr << "Torpedo died: end of run" << endl;
#endif
         return(DEACTIVE);
         } // end kill torpedo at its max underwater range

      } // end if torpedo is underwater

   // Torp is above water, so compute ballistic motion
   else
      { 
      // Compute the accelerations.  Slow down in the horizontal plane.
      //  Drag is proportional to the square of the velocity
      accel[X] = -POSNEG(velocity[X]) *TORPEDO_DRAG* velocity[X]*velocity[X];
      accel[Y] = -POSNEG(velocity[Y]) *TORPEDO_DRAG* velocity[Y]*velocity[Y];

      // Speed up in the vertical plane
      accel[Z] = -GRAVITY -POSNEG(velocity[Z]) *TORPEDO_DRAG* velocity[Z]*velocity[Z];

      // Calculate velocities and distances using euler integration
      velocity[X] = accel[X] *deltatime + velocity[X];
      velocity[Y] = accel[Y] *deltatime + velocity[Y];
      velocity[Z] = accel[Z] *deltatime + velocity[Z];

      // Constant time and 1/2
      time2 = deltatime*deltatime*0.5;
      posture.xyz[X] = time2*accel[X]+deltatime*velocity[X] +posture.xyz[X];
      posture.xyz[Y] = time2*accel[Y]+deltatime*velocity[Y] +posture.xyz[Y];
      posture.xyz[Z] = time2*accel[Z]+deltatime*velocity[Z] +posture.xyz[Z];

      // Compute the pitch angle above water
      pfSubVec3(tempvec,posture.xyz,oldpnt);
      pfNormalizeVec3(tempvec);
      posture.hpr[PITCH] = pfArcSin(tempvec[Z]);

      // Calculate the direction of the torpedo for video torpedo
      posture.hpr[ROLL] = 0.0f;
      pfMakeEulerMat ( orient, posture.hpr[HEADING],
                               posture.hpr[PITCH],
                               posture.hpr[ROLL]);
      pfFullXformPt3 ( direction, direction, orient );
      pfNormalizeVec3 ( direction );
      } // end ballistic torpedo motion computation

   // Video torpedo was selected, so calculate where to look and open the 
   //  viewing channel
   if (video)
      {
      // Offset the direction to be out in front of the torpedo
      pfScaleVec3 ( offset, 5.0f, direction );

      // Add offset to current XYZ to look from the correct point in space
      pfAddVec3 ( offset, offset, posture.xyz );

      // Open the vidoe missile/torp channel
      pfChanView(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                 offset, posture.hpr);
#ifdef DEBUG_TORP
   cerr << "Video Torpedo is on." << endl;
#endif
      } // end video torpedo calculation

   // If the torpedo is out of bounds, kill it
   if(!inplay(posture.xyz))
      {
      deactivate_torpedo();
      return(DEACTIVE);
      } // end if torpedo out-of-bounds
     
   // Test to see if the torpedo hit something.  If it did, kill it
   if(impactdetermination(oldpnt,posture.xyz) == HITSOMETHING)
      {
      // torpdeo hit something -- call its own member function
      deactivate_torpedo();
      return(DEACTIVE);
      } // end if torpedo impacted anything

   // Find out if we have to sent out an entity state PDU
   cumtime = curtime - lastPDU;
   pfScaleVec3(tempvec,cumtime,oldvel);
   pfAddVec3(tempvec,drparam.xyz,tempvec);
   if((cumtime > G_static_data->DIS_heartbeat_delta) ||
      (pfSqrDistancePt3(tempvec,posture.xyz)>G_static_data->DIS_position_delta))
      { 
      // It exceeds the dead reckoning parameters, so send an update
      sendentitystate();

      //update the DRparameters
      PFCOPY_VEC3(drparam.xyz,posture.xyz);
      lastPDU = curtime;
      } // end send a new entity state PDU for DR

#ifdef DEBUG_TORP
   cerr << "Torpedo location at X " << posture.xyz[X] <<
                             ", Y " << posture.xyz[Y] << 
                             ", Z " << posture.xyz[Z] << endl;
   cerr << "Torpedo orientation in H " << posture.hpr[HEADING] <<
                                ", P " << posture.hpr[PITCH] << 
                                ", R " << posture.hpr[ROLL] << endl;
   cerr << "Torpedo velocity in X " << velocity[X] <<
                             ", Y " << velocity[Y] << 
                             ", Z " << velocity[Z] << endl;
#endif

   // Put the torpedo in the correct location in space
   pfDCSTrans(dcs,posture.xyz[X],posture.xyz[Y],posture.xyz[Z]);
   pfDCSRot(dcs,posture.hpr[HEADING],posture.hpr[PITCH],posture.hpr[ROLL]);
#ifdef DEBUG_SCALE
   pfDCSScale(dcs,10.0f);
#endif

   return(FINEWITHME);
} // end TORPEDO_AL move()


// This function is intended to identify the correct launch effect of
//  the torpedo.  Currently, there is no effect.
void TORPEDO_AL::launcheffect()
//a virtual function that does not do anything
{
   /*do nothing, when you launch a bomb nothing visual happens*/
};


// This function returns the type of vehicle this torpedo is
mtype TORPEDO_AL::gettype() 
{ 
   return (TORPEDO_AL_TYPE);
}


