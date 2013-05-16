// File: <jack_sailor_veh.cc> 
//
// NPSNETIV Version 8.1  dtd 09-24-95
// Performer Version 1.2 Simulation Code
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
// 12-30-95    Official release 1.0 w/ documentation         All

#ifdef JACK

#include <iostream.h>
#include <stdlib.h>
#include <pf.h>
#include <bstring.h>

#include "manager.h"
#include "jack_sailor_veh.h"
#include "jack.h"
#include "person.h"
#include "ship_walk_veh.h"
#include "human_const.h"
#include "common_defs2.h"
#include "appearance.h"
#include "disnetlib.h"
#include "netutil.h"
#include "terrain.h"
#include "conversion_const.h"
#include "collision_const.h"
#include "munitions.h"

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
extern volatile int              G_drivenveh;
extern          DIS_net_manager *G_net;
#ifndef NOHIRESNET
extern          DIS_net_manager *G_hr_net;
extern volatile float            G_curtime;
#endif // NOHIRESNET
extern          pfGroup         *G_moving;
extern          pfGroup         *G_fixed;
extern volatile MANAGER_REC     *G_manager;

extern          short           *G_JackSoldier_active;
extern          int              G_num_soldiers;
extern          int              G_tot_soldiers;
extern          void             firebuttonhit(wdestype,int,pfCoord,pfCoord);

const float PERSON_INTERSECT_OFFSET = 1.0f;

/*
#if defined(JACK) && defined(SS_INTERFACE)
#include "idunetlib.h"
static int L_use_ss_interface = FALSE;
static IDU_net_manager *L_idu_net = NULL;

static char    *L_ss_group = "224.200.200.201";
static u_short  L_ss_port = 3201;
static u_char   L_ss_ttl = 3;
static char    *L_ss_interface = "";
static int      L_ss_loopback = TRUE;


void jack_use_ss_interface ( int mode )
{
   L_use_ss_interface = mode;
}
#endif

// Utility functions used for medic mode code
// A function to convert NPS (N=0,E=90) headings (in degrees)
// into "standard" (E=0,N=90) mathematical headings (in radians)
double nps_to_rad (double ang)
{
  return ((90.0 + ang) * DEG_TO_RAD);
}


// The reverse of the function above

double rad_to_nps (double ang)
{
  return ((ang * RAD_TO_DEG) - 90.0);
}
// Note: Given a "standard" angle A, and a distance D,
// the location (X,Y) = (D*cos(A),D*sin(A)) is at a
// distance of "D" along the heading "A"


// A function to do "standard" arc-tan computations
// Return the "standard" angle between (0,0) and (x,y)
double atanxy (double x, double y)
{
  return (atan2 (y, x));
}


// A function to determine the angle to "desire" given
// an initial angle, and a "probable" desired angle
double pangle (double ihd, double fhd)
{
    // Make 'final' angle smaller than initial
    while (fhd > ihd) fhd -= M_PI * 2.0;

    // Make 'final' angle JUST BIGGER than initial
    while (ihd > fhd) fhd += M_PI * 2.0;
    
    // Hack -- Reverse the spin direction if needed
    if (fhd - ihd > M_PI) fhd -= M_PI * 2.0;

    // Return the new final angle
    return (fhd);
}


void checkBackwardMotion(float speed, float& oldspeed,
   pfVec3& velocity, float heading)
{
   // Need to determine if going backwards
   pfVec2 vel, headingvec;
   float hdg;
   vel[X] = velocity[X];
   vel[Y] = velocity[Y];
   pfNormalizeVec2(vel);
   hdg = 90.0f+heading;
   if(hdg < 0.0f) {
      hdg += 360.0f;
   }
   else if (hdg > 360.0f) {
      hdg -= 360.0f;
   }

   pfSinCos(hdg, &headingvec[Y],&headingvec[X]);
   // heading is opposite for DR entities
   if(pfDotVec2(headingvec,vel) < 0.0f) {
      oldspeed = -speed;
   }
   else {
      oldspeed = speed;
   }
} // end checkBackwardMotion()

*/
//************************************************************************//
//*************** class JACK_SAILOR_VEH **********************************//
//************************************************************************//
JACK_SAILOR_VEH::JACK_SAILOR_VEH(int id,int atype, ForceID the_force)
: JACK_DI_VEH(id,atype,the_force)
{
   status = Appearance_LifeForm_UprightStill | Appearance_LifeForm_Weapon1_Stowed;
   Medic_update_flag = 0;
   unitMember = FALSE;
   behavior = NOBEHAVIOR; // COMBAT; 
   weapon_present = FALSE;
   weapon = JACKTTES_WEAPON_STOWED;
   icontype = JACKNORIFLE;
   targetting = FALSE;
/*
   // Temporary Fix to mount network sailors to ship
   if (id != G_drivenveh) {
      //cerr<<"temp mounting human..."<<vid<<" "<<id<<endl;
      mounted_vid = 0; //Cludge, I know that the ship is vehicle #1 in list
      ((SHIP_WALK_VEH *)G_vehlist[mounted_vid].vehptr)->mount(this,id);
      snap_switch = FALSE;
   }
*/
/*
   upperJointMods = new JointMods();
   signals = new signalClass();
   create_entity ( atype, the_force );

#if defined(JACK) && defined(SS_INTERFACE)
   if ( (id == G_drivenveh) && L_use_ss_interface ) {
      L_idu_net = new IDU_net_manager ( L_ss_group, L_ss_port, L_ss_ttl,
                                        L_ss_interface, L_ss_loopback );
      if ( !L_idu_net->net_open() ) {
         cerr << "JACK_DI_VEH constructor:\tCannot open IDU_net_manager!"
              << endl;
         L_use_ss_interface = FALSE;
         delete L_idu_net;
         L_idu_net = NULL;
      }
   }
#endif
*/
} // end JACK_SAILOR_VEH::JACK_SAILOR_VEH()

JACK_SAILOR_VEH::~JACK_SAILOR_VEH()
{
/*
#if defined(JACK) && defined(SS_INTERFACE)
   if ( (vid == G_drivenveh) && L_use_ss_interface && (L_idu_net != NULL) ) {
      L_use_ss_interface = FALSE;
      delete L_idu_net;
      L_idu_net = NULL;
   }
#endif

   delete upperJointMods;
   delete signals;
*/
}


#ifndef NONPSSHIP
void JACK_SAILOR_VEH::updateRelPosVec()
{
   // Update relative position based on mounted vehicle's
   // velocity and position
   if (mounted_vid != vid) {
      float  *m_vid_pos;
      pfVec3  m_vid;

      //Get new relative location vector
      m_vid_pos = G_vehlist[mounted_vid].vehptr->getposition();
      pfSetVec3(m_vid,m_vid_pos[X],m_vid_pos[Y],m_vid_pos[Z]);
      pfSubVec3(relposvec, posture.xyz, m_vid);
//cerr<<"relposvec:"<<relposvec[0]<<" "<<relposvec[1]<<" "<<relposvec[2]<<endl;
   }
}

void JACK_SAILOR_VEH::reset_on_mounted_vid()
{
   pfMatrix orient;
   pfVec3  calc_vec, m_vid, new_m_hpr, m_delta_hpr;
   float relposlen=pfLengthVec3(relposvec);
   float *m_vid_pos = G_vehlist[mounted_vid].vehptr->getposition();
   pfSetVec3(m_vid,m_vid_pos[X],m_vid_pos[Y],m_vid_pos[Z]);

   new_m_hpr[HEADING]=G_vehlist[mounted_vid].vehptr->getorientation()[HEADING];
   new_m_hpr[PITCH]=G_vehlist[mounted_vid].vehptr->getorientation()[PITCH];
   new_m_hpr[ROLL]=G_vehlist[mounted_vid].vehptr->getorientation()[ROLL];

   m_delta_hpr[HEADING] = oldmounthpr[HEADING] - new_m_hpr[HEADING];
   m_delta_hpr[PITCH] = oldmounthpr[PITCH] - new_m_hpr[PITCH];
   m_delta_hpr[ROLL] = oldmounthpr[ROLL] - new_m_hpr[ROLL];

   pfCopyVec3(oldmounthpr, new_m_hpr);
   //oldmounthpr = new_m_heading;

   //Rotate based on mounted_vid's delta heading
//cerr<<"Before:"<<posture.xyz[0]<<" "
//               <<posture.xyz[1]<<" "
//               <<posture.xyz[2]<<" ";
   posture.hpr[HEADING] -= m_delta_hpr[HEADING];
   //pfSubVec3(posture.hpr, posture.hpr, m_delta_hpr);
   pfMakeEulerMat ( orient, -m_delta_hpr[HEADING], 0.0f, 0.0f);
   //pfMakeEulerMat ( orient, -m_delta_hpr[HEADING],
   //                         -m_delta_hpr[PITCH],
   //                         -m_delta_hpr[ROLL]);
   pfCopyVec3(calc_vec, relposvec);
   pfNormalizeVec3(calc_vec);
   pfXformVec3(calc_vec, calc_vec, orient);
   pfScaleVec3(calc_vec, relposlen, calc_vec);

   //Translate to proper angular position
   pfAddVec3(posture.xyz,m_vid,calc_vec);
//cerr<<"After:"<<posture.xyz[0]<<" "
//              <<posture.xyz[1]<<" "
//              <<posture.xyz[2]<<endl;
   // Change Hull DCS if mounted_veh is after this person in vehicle list
   if(mounted_vid > vid) placeVehicle();
   drpos=posture;

} // end JACK_SAILOR_VEH::reset_on_mounted_vid()
#endif



int JACK_SAILOR_VEH::check_collide(pfVec3 &startpnt, pfVec3 &endpnt,
                                   pfVec3 &orientation,
                                   float &speed, float &altitude)
{
   long     isect;
   pfSegSet seg;
   pfHit    **hits[32];
   long     flags;
   pfNode   *node;
   pfVec3   normal;
   pfVec3   pnt;
   pfMatrix xform;
   int hit_index;
   pfSeg *sp[2] ;
   // pfCylinder the_barrel;
   long the_mask;
   pfVec3 incoming_vec;
   pfVec3 outgoing_vec;
   pfVec3 normal_vec;
   pfVec3 eye_vec, foot_vec;
   pfVec3 start_eye, end_eye;
   pfVec3 start_pos, end_pos;
   float temp_float;
   pfMatrix Orient;                      // Local entity orientation transform
   pfVec3 tb_vec = { 0.0f, 0.0f, 1.0f };
   pfVec3 fb_vec = { 0.0f, 1.0f, 0.0f };
   int the_item_hit;

   // If not moving, do not check for collisions
   if (int(pfEqualVec3(startpnt,endpnt))) 
      return FINEWITHME;

   // Compute local entity axis
   pfMakeEulerMat ( Orient, orientation[HEADING],
                            orientation[PITCH],
                            orientation[ROLL] );
   pfFullXformPt3 ( tb_vec, tb_vec, Orient ); 
   pfFullXformPt3 ( fb_vec, fb_vec, Orient );
   pfScaleVec3 ( eye_vec, EYE_OFFSET, tb_vec );
   pfScaleVec3 ( foot_vec, PERSON_INTERSECT_OFFSET, tb_vec );
   pfAddVec3 ( start_eye, startpnt, eye_vec );
   pfAddVec3 ( end_eye, endpnt, eye_vec );
   pfAddVec3 ( start_pos, startpnt, foot_vec );
   pfAddVec3 ( end_pos, endpnt, foot_vec );

   if ( speed < 0.0f )
      pfScaleVec3 ( fb_vec, bounding_box.min[PF_Y], fb_vec );
   else
      pfScaleVec3 ( fb_vec, bounding_box.max[PF_Y], fb_vec );
   pfAddVec3 ( end_eye, end_eye, fb_vec );
   pfAddVec3 ( end_pos, end_pos, fb_vec );
   
   //create a segment along the path we just followed
   pfSetVec3(seg.segs[0].dir,(end_pos[X] - start_pos[X]),
                        (end_pos[Y] - start_pos[Y]),
                        (end_pos[Z] - start_pos[Z]));
   pfSetVec3(seg.segs[1].dir,(end_eye[X] - start_eye[X]),
                        (end_eye[Y] - start_eye[Y]),
                        (end_eye[Z] - start_eye[Z]));

   seg.segs[0].length = pfLengthVec3(seg.segs[0].dir);
   seg.segs[1].length = pfLengthVec3(seg.segs[1].dir);

   PFCOPY_VEC3(seg.segs[0].pos,start_pos);
   PFCOPY_VEC3(seg.segs[1].pos,start_eye);

   //fix a performer bug
   if(seg.segs[0].dir[Y] == 0.0) seg.segs[0].dir[Y] = 0.01f;
   if(seg.segs[0].dir[X] == 0.0) seg.segs[0].dir[X] = 0.01f;
   if(seg.segs[1].dir[Y] == 0.0) seg.segs[1].dir[Y] = 0.01f;
   if(seg.segs[1].dir[X] == 0.0) seg.segs[1].dir[X] = 0.01f;

   pfNormalizeVec3(seg.segs[0].dir);
   pfNormalizeVec3(seg.segs[1].dir);
 
   sp[0] = &(seg.segs[0]);
   sp[1] = &(seg.segs[1]);

//  pfCylAroundSegs ( &the_barrel, (const pfSeg **)sp, 2 );

   //*** intersect test against the fixed objects ********************//

   if (altitude < 1000.0f)
      {
      seg.mode = PFTRAV_IS_PRIM;
      seg.userData = NULL;
      seg.activeMask = 0x03;
      seg.isectMask = DESTRUCT_MASK;
      seg.bound = NULL;
      seg.discFunc = buildinghit;
      isect = pfNodeIsectSegs(G_fixed, &seg, hits);
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      if ( flags )
         {
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
         }
      else
         {
         pfQueryHit(*hits[1], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[1], PFQHIT_XFORM, &xform);
         pfQueryHit(*hits[1], PFQHIT_FLAGS, &flags);
         }
      if (isect && (flags & PFHIT_POINT))
         {
         /*set the point at the intersectin point in the world COORDINATES*/
         // pfFullXformPt3(posture.xyz, pnt, xform);
#ifdef DEBUG
         cout << "\n\nBuilding hit at " << pnt[X] << " "
                                        << pnt[Y] << " "
                                        << pnt[Z] << endl;
#endif
         speed = 0.0f;
         pfCopyVec3 ( endpnt, startpnt );
         return (HITSOMETHING);
         }
      }

   //******* intersect test against ship objects *********************//
   seg.mode = PFTRAV_IS_PRIM;
   seg.userData = NULL;
   seg.activeMask = 0x03;
   seg.isectMask = ENTITY_MASK;
   seg.bound = NULL;
   seg.discFunc = buildinghit;
   isect = pfNodeIsectSegs(G_moving, &seg, hits);
   pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
   if ( flags ) {
      pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
      pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
   }
   else {
      pfQueryHit(*hits[1], PFQHIT_POINT, &pnt);
      pfQueryHit(*hits[1], PFQHIT_XFORM, &xform);
      pfQueryHit(*hits[1], PFQHIT_FLAGS, &flags);
   }
   if (isect && (flags & PFHIT_POINT)) {
      /*set the point at the intersectin point in the world COORDINATES*/
      // pfFullXformPt3(posture.xyz, pnt, xform);
#ifdef DEBUG
      cout << "\n\nShip hit at " << pnt[X] << " "
                                 << pnt[Y] << " "
                                 << pnt[Z] << endl;
cerr<<"startpt:"<<startpnt[X]<<" "<<startpnt[Y]<<" "<<startpnt[Z]<<"/"
    <<"endpt:"<<endpnt[X]<<" "<<endpnt[Y]<<" "<<endpnt[Z]<<endl;
#endif
      speed = 0.0f;
      pfCopyVec3 ( endpnt, startpnt );
      return (HITSOMETHING);
   } //

   //******* intersect test against the ground *********************//
   seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   seg.userData = NULL;
   seg.activeMask = 0x03;
   seg.isectMask = PERMANENT_MASK;
   seg.bound = NULL;
   seg.discFunc = NULL;
   if (pfNodeIsectSegs(G_fixed, &seg, hits))
      {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      if ( flags )
          {
          pfQueryHit(*hits[0], PFQHIT_NODE, &node);
          pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
          pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
          pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
          hit_index = 0;
          }
      else
          {
          pfQueryHit(*hits[1], PFQHIT_FLAGS, &flags);
          pfQueryHit(*hits[1], PFQHIT_NODE, &node);
          pfQueryHit(*hits[1], PFQHIT_NORM, &normal);
          pfQueryHit(*hits[1], PFQHIT_XFORM, &xform);
          pfQueryHit(*hits[1], PFQHIT_POINT, &pnt);
          hit_index = 1;
          }

      the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
      the_item_hit = int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT);

      // find the orientation of the ground
      if ( ( (the_item_hit == PERMANENT_DIRT) ||
             (the_item_hit == PERMANENT_BRIDGE) ||
             (the_item_hit == PERMANENT_FOILAGE) ) &&
           (flags & PFHIT_NORM) )
         {
         pfVec3 head, head90, hpr;
         float sh, ch;
         float dotp;
         pfSinCos(posture.hpr[HEADING], &sh, &ch);
         head[0] = -sh;
         head[1] =  ch;
         head[2] = 0.0f;
         dotp = PFDOT_VEC3(head, normal);
         hpr[PITCH] = pfArcCos(dotp) - 90.0f;
         head90[0] =  ch;
         head90[1] =  sh;
         head90[2] = 0.0f;
         dotp = PFDOT_VEC3(head90, normal);
         hpr[ROLL]  = 90.0f - pfArcCos(dotp);

         if ( (hpr[PITCH] > 30.0f) || (hpr[ROLL] > 30.0f) )
            {
            speed = 0.0f;
            pfCopyVec3 ( endpnt, startpnt );
            return (HITSOMETHING);
            }
         }// if dirt/bridge/foilage hit

      else if ( the_item_hit == PERMANENT_WATER )
         {
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         G_dynamic_data->dead = DYING;
         G_dynamic_data->cause_of_death = WATER_DEATH;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         return(HITSOMETHING);
         } // if water hit

      else if ( (the_item_hit == PERMANENT_TUNNEL) &&  
                (flags & PFHIT_NORM) 
               && (flags & PFHIT_POINT) )
         {
         float ordered_heading;
         float pitch_angle, roll_angle, bearing_angle;
         pfMatrix orient;
         pfVec3 direction = { 0.0f, 1.0f, 0.0f };
         pfSubVec3 ( incoming_vec, startpnt, endpnt);
         pfNormalizeVec3 ( incoming_vec );
           normal_vec[X] = normal[X];
           normal_vec[Y] = normal[Y];
           normal_vec[Z] = normal[Z];
         
         pfNormalizeVec3 ( normal_vec );
         temp_float = 1.25f * pfDotVec3 ( normal_vec, incoming_vec );
         pfScaleVec3 ( outgoing_vec, temp_float, normal_vec );
         pfSubVec3 ( outgoing_vec, outgoing_vec, incoming_vec );
         pfNormalizeVec3 ( outgoing_vec );
         
         bearing_angle = pfArcTan2( -outgoing_vec[Y], -outgoing_vec[X] );
         pitch_angle = 1.5f * pfArcSin ( outgoing_vec[Z] );
         if (pitch_angle >= 90.0f)
            pitch_angle -= 90.0f;
         if (pitch_angle <= -90.0f)
            pitch_angle += 90.0f;
         roll_angle = 0.0f; 
         ordered_heading = (bearing_angle + 90.0f);
         if (ordered_heading >= 360.0)
            ordered_heading -= 360.0f;
         if (ordered_heading < 0.0)
            ordered_heading += 360.0f;
         
         pfSetVec3 ( orientation, ordered_heading, pitch_angle, roll_angle );

         pfMakeEulerMat ( orient, orientation[HEADING],
                                  orientation[PITCH],
                                  orientation[ROLL]);
         pfFullXformPt3 ( direction, direction, orient );
         pfNormalizeVec3 ( direction );
         if ( hit_index == 1 )
            pfScaleVec3 ( normal, EYE_OFFSET + 5.0f, normal );
         else                 
            pfScaleVec3 ( normal, PERSON_INTERSECT_OFFSET + 5.0f, normal );

         pfAddVec3 (endpnt, pnt, normal);
//         altitude = endpnt[Z] - gnd_level(endpnt);;
         temp_float= 0.75f * pfLengthVec3(velocity);
         speed *= 0.75f;
         pfScaleVec3 ( velocity, temp_float, direction);
         }// if tunnel hit

      } //if ground pfSegs 
    
      //if we got here it did not hit anything 
      return FINEWITHME;

} //check_collide

void JACK_SAILOR_VEH::class_specific_keyboard()
{
   PERSON_VEH::class_specific_keyboard();
}// class_specific_keyboard()


void JACK_SAILOR_VEH::class_specific_fcs()
{
   PERSON_VEH::class_specific_fcs();
}// class_specific_fcs()

#endif  // ifdef JACK


