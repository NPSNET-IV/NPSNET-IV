// File: <person.cc> 
//

#include <math.h>
#include <string.h>
#include <bstring.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream.h>

#include "pdu.h"
#include "manager.h"
#include "input_manager.h"
#ifndef NO_IPORT
#include "iportClass.h"
#include "DI_comm.h"
#endif // NO_IPORT
#include "person.h"
#include "ground_veh.h"

#ifndef NONPSSHIP
//We will not include ship_veh.h when all vehicles have mounting capabilities
#include "ship_veh.h"
#include "ship_walk_veh.h"
#endif

#include "jack.h"
#include "human_const.h"
#include "entity_const.h"
#include "jointmods.h"
#include "common_defs2.h"
#include "macros.h"
#include "appearance.h"
#include "dirt_intersect.h"
#include "netutil.h"
#include "entity.h"
#include "munitions.h"
#include "conversion_const.h"
#include "collision_const.h"
#include "interface_const.h"
#include "environment_const.h"

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
extern volatile float            G_curtime;
extern          pfGroup         *G_fixed;
extern          pfGroup         *G_moving;
extern volatile  MANAGER_REC    *G_manager;

// Locals used by this library
const float PERSON_INTERSECT_OFFSET = 1.0f;
//static pfVec3 Zerovec = { 0.0f, 0.0f, 0.0f };

#ifndef NONPSSHIP
//static pfVec3 relposvec = { 0.0f, 0.0f, 0.0f };
//static pfVec3 oldmounthpr;
int   hit_vid;

//long mounthit(pfHit *result);
int get_isect_moving (float &iPnt, pfVec3 &c_norm, pfVec3 xyz);
#endif

//************************************************************************//
//*************** class PERSON ********************************************//
//************************************************************************//
PERSON_VEH::PERSON_VEH(int id,int atype, ForceID the_force)
: GROUND_VEH(id,atype,the_force)
{
   char tempstr[10];

   status = Appearance_LifeForm_UprightStill | Appearance_LifeForm_Weapon1_Stowed;
   Control = OTHER;
   status = Appearance_LifeForm_UprightStill | Appearance_LifeForm_Weapon1_Stowed;
   if (!*(G_static_data->marking) ) {
     strncpy ( tempstr, (const char *)G_static_data->hostname, MARKINGS_LEN-3 );
     strcpy ( (char *)G_static_data->marking, "DI" );
     strcat ( (char *)G_static_data->marking, tempstr );
   }
   movement_index = NOSIGNAL;

   JointVals = new float[MAXJOINTS];

   posture_chg_time = 0.0f;

#ifndef NONPSSHIP
   mounted_vid = vid;		// Set the mounted vid to driven_veh default
   pfSetVec3(relposvec,0.0f, 0.0f, 0.0f);
#endif

   bzero (hiResJointAngles, sizeof (fixedDatumRecord) * MIN_NUM_HIRES_JOINTS);
   hiResJointAngles[LS0].datum_id = LS0_TAG;
   hiResJointAngles[LS1].datum_id = LS1_TAG;
   hiResJointAngles[LS2].datum_id = LS2_TAG;
   hiResJointAngles[LE0].datum_id = LE0_TAG;
   hiResJointAngles[LW0].datum_id = LW0_TAG;
   hiResJointAngles[LW1].datum_id = LW1_TAG;
   hiResJointAngles[LW2].datum_id = LW2_TAG;
   hiResJointAngles[RS0].datum_id = RS0_TAG;
   hiResJointAngles[RS1].datum_id = RS1_TAG;
   hiResJointAngles[RS2].datum_id = RS2_TAG;
   hiResJointAngles[RE0].datum_id = RE0_TAG;
   hiResJointAngles[RW0].datum_id = RW0_TAG;
   hiResJointAngles[RW1].datum_id = RW1_TAG;
   hiResJointAngles[RW2].datum_id = RW2_TAG;
   hiResJointAngles[RCX].datum_id = RCX_TAG;
   hiResJointAngles[RCY].datum_id = RCY_TAG;
   hiResJointAngles[RCZ].datum_id = RCZ_TAG;
   hiResJointAngles[RCH].datum_id = RCH_TAG;
   hiResJointAngles[RCP].datum_id = RCP_TAG;
   hiResJointAngles[RCR].datum_id = RCR_TAG;
   okToSendHiRes = FALSE;
   okToUpdateHiRes = FALSE;
   numHiResJointAngles = 0;
   hiResTimeStamp = G_curtime;

   old_index = NOSIGNAL;

   gun_pos.xyz[PF_X] = 0.0f;
   gun_pos.xyz[PF_Y] = 0.0f;
   gun_pos.xyz[PF_Z] = 0.0f;
   gun_pos.hpr[PF_H] = 0.0f;
   gun_pos.hpr[PF_P] = 0.0f;
   gun_pos.hpr[PF_R] = 0.0f;

} // end PERSON_VEH::PERSON_VEH()

#ifndef NODUDE
PERSON_VEH::PERSON_VEH(int id)
: VEHICLE(id)
{
   char tempstr[10];

   status = Appearance_LifeForm_UprightStill | Appearance_LifeForm_Weapon1_Stowed;
   Control = OTHER;
   status = Appearance_LifeForm_UprightStill | Appearance_LifeForm_Weapon1_Stowed;
   if (!*(G_static_data->marking) ) {
     strncpy ( tempstr, G_static_data->hostname, MARKINGS_LEN-3 );
     strcpy ( G_static_data->marking, "DI" );
     strcat ( G_static_data->marking, tempstr );
   }
   movement_index = NOSIGNAL;

   JointVals = new float[MAXJOINTS];
   
} 
#endif // NODUDE


#ifndef NONPSSHIP
PERSON_VEH::~PERSON_VEH()
{
   dismount();
}
#endif //NONPSSHIP

void PERSON_VEH::rot_elev_head()
{
   //turn the turret
   JointVals[HEAD_H] = fmod (JointVals[HEAD_H], 360.0f );
   pfDCSRot(joint[HEAD_ANGLE].dcs, 
      JointVals[HEAD_H], JointVals[HEAD_P], JointVals[HEAD_R]);

   // raise the gun
   JointVals[HEAD_P] = fmod ( JointVals[HEAD_P], 360.0f );
   pfDCSRot(joint[HEAD_ANGLE].dcs, JointVals[HEAD_H], 
            JointVals[HEAD_P], JointVals[HEAD_R]);

} // end PERSON_VEH::rot_elev_head()


void PERSON_VEH::normalize_angles ()
{
   if ( JointVals[NECK_H] > 90.0f ) {
      JointVals[NECK_H] = 90.0f;
   }
   if ( JointVals[NECK_H] < -90.0f ) {
      JointVals[NECK_H] = -90.0f;
   }
   if ( JointVals[NECK_P] > 90.0f ) {
      JointVals[NECK_P] = 90.0f;
   }
   if ( JointVals[NECK_P] < -90.0f ) {
      JointVals[NECK_P] = -90.0f;
   }
   if ( JointVals[NECK_R] > 90.0f ) {
      JointVals[NECK_R] = 90.0f;
   }
   if ( JointVals[HEAD_H] < -90.0f ) {
      JointVals[HEAD_H] = -90.0f;
   }
   if ( JointVals[HEAD_H] > 90.0f ) {
      JointVals[HEAD_H] = 90.0f;
   }
   if ( JointVals[HEAD_H] < -90.0f ) {
      JointVals[HEAD_H] = -90.0f;
   }
   if ( JointVals[HEAD_P] > 90.0f ) {
      JointVals[HEAD_P] = 90.0f;
   }
   if ( JointVals[HEAD_P] < -90.0f ) {
      JointVals[HEAD_P] = -90.0f;
   }
   if ( JointVals[HEAD_R] > 90.0f ) {
      JointVals[HEAD_R] = 90.0f;
   }
   if ( JointVals[HEAD_R] < -90.0f ) {
      JointVals[HEAD_R] = -90.0f;
   }

} // end PERSON_VEH::normalize_angles()


void PERSON_VEH::apply_angles ()
{

      pfDCSRot(joint[WAIST_ANGLE].dcs,
         JointVals[WAIST_H], JointVals[WAIST_P], JointVals[WAIST_R]);

      pfDCSRot(joint[NECK_ANGLE].dcs,
         JointVals[NECK_H], JointVals[NECK_P], JointVals[NECK_R]);

      pfDCSRot(joint[HEAD_ANGLE].dcs,
         JointVals[HEAD_H], JointVals[HEAD_P], JointVals[HEAD_R]);

      pfDCSRot(joint[RARM_SHOULDER_ANGLE].dcs,
         JointVals[RIGHT_SHOULDER_H], JointVals[RIGHT_SHOULDER_P], 
         JointVals[RIGHT_SHOULDER_R]);

      pfDCSRot(joint[RARM_ELBOW_ANGLE].dcs,
         0.0f, JointVals[RIGHT_ELBOW_P], 0.0f );

      pfDCSRot(joint[RARM_WRIST_ANGLE].dcs,
         JointVals[RIGHT_WRIST_H], JointVals[RIGHT_WRIST_P], 
         JointVals[RIGHT_WRIST_R]);

      pfDCSRot(joint[LARM_SHOULDER_ANGLE].dcs,
         JointVals[LEFT_SHOULDER_H], JointVals[LEFT_SHOULDER_P], 
         JointVals[LEFT_SHOULDER_R]);

      pfDCSRot(joint[LARM_ELBOW_ANGLE].dcs,
         0.0f, JointVals[LEFT_ELBOW_P], 0.0f );

      pfDCSRot(joint[LARM_WRIST_ANGLE].dcs,
         JointVals[LEFT_WRIST_H], JointVals[LEFT_WRIST_P], 
         JointVals[LEFT_WRIST_R]);

      pfDCSRot(joint[RLEG_HIP_ANGLE].dcs,
         JointVals[RIGHT_HIP_H], JointVals[RIGHT_HIP_P], JointVals[RIGHT_HIP_R]);

      pfDCSRot(joint[RLEG_KNEE_ANGLE].dcs,
         0.0f, JointVals[RIGHT_KNEE_P], 0.0f );

      pfDCSRot(joint[RLEG_ANKLE_ANGLE].dcs,
         JointVals[RIGHT_ANKLE_H], JointVals[RIGHT_ANKLE_P], 
         JointVals[RIGHT_ANKLE_R]);

      pfDCSRot(joint[RLEG_TOE_ANGLE].dcs,
         0.0f, JointVals[RIGHT_TOE_P], 0.0f );

      pfDCSRot(joint[LLEG_HIP_ANGLE].dcs,
         JointVals[LEFT_HIP_H], JointVals[LEFT_HIP_P], JointVals[LEFT_HIP_R]);

      pfDCSRot(joint[LLEG_KNEE_ANGLE].dcs,
         0.0f, JointVals[LEFT_KNEE_P], 0.0f );

      pfDCSRot(joint[LLEG_ANKLE_ANGLE].dcs,
         JointVals[LEFT_ANKLE_H], JointVals[LEFT_ANKLE_P], 
         JointVals[LEFT_ANKLE_R]);

      pfDCSRot(joint[LLEG_TOE_ANGLE].dcs,
         0.0f, JointVals[LEFT_TOE_P], 0.0f );
} // end PERSON_VEH::apply_angles()


void PERSON_VEH::walk(float)
{
   static int first_time=1; 
   static float cyl[MAX_VEH], stp[MAX_VEH];
   float angle = 30.0f;
   float steps = 1.0f;
   float factor = 3;   // slower motion use factor=5

   if(first_time) {
        for(int i = 0; i < MAX_VEH; i++) {
           stp[i] = 0.0f;
           cyl[i] = 1.0f;
        }
        first_time = 0;
   }

   steps *= factor;
   angle /= factor;

   // Orig from Ming Lin's test program ~mlin/DI_GUY/ISMS/main.C 
   // Assume that all angles are zero degree at the initial position 
   // Make modification accordingly if the assumption is not true 
    
   // start walking cycle here, consisted of 4 stages:
   // cyl = 1.0:  left leg step out and returned to the original pos 
   // cyl = -1.0: right leg step out and returned to the original pos 
  
   if (stp[vid] <= steps) {

       // cyl = 1: RIGHT LEG  going back 
        JointVals[RIGHT_TOE_P] = 0.0f;
        JointVals[RIGHT_ANKLE_H] = 0.0f;
        JointVals[RIGHT_ANKLE_R] = 0.0f;
        JointVals[RIGHT_HIP_H] = 0.0f;
        JointVals[RIGHT_HIP_R] = 0.0f;

       if(cyl[vid] == -1) { // power kick forward 
          JointVals[RIGHT_HIP_P] =  (stp[vid]) * (-angle/2) * cyl[vid]+20.0f;
          JointVals[RIGHT_KNEE_P] = (stp[vid]+1.0f) * (-angle) -50.0f;
          JointVals[RIGHT_ANKLE_P] =  0.0f;  // pitch
          JointVals[RIGHT_TOE_P] =  0.0f;
       }
       else {
          JointVals[RIGHT_KNEE_P] =  (stp[vid]+1.0f) * (-angle/2) ;          
          JointVals[RIGHT_HIP_P] = (stp[vid]) * (-angle/2) * cyl[vid];
          JointVals[RIGHT_ANKLE_P] = 10.0f;  // pitch
          JointVals[RIGHT_TOE_P] = 10.0f;
       }
       // Right arm 
       JointVals[RIGHT_SHOULDER_H] =  0.0f;
       JointVals[RIGHT_SHOULDER_R] =  10.0f*(-1.0f);
       JointVals[RIGHT_SHOULDER_P] =  cyl[vid]*(angle/1.5)*stp[vid];
       JointVals[RIGHT_ELBOW_P] =  30.0f;

       // cyl = 1: LEFT  LEG going forward 
       JointVals[LEFT_TOE_P] = 0.0f;
       JointVals[LEFT_ANKLE_H] = 0.0f;
       JointVals[LEFT_ANKLE_R] = 0.0f;
       JointVals[LEFT_HIP_H] = 0.0f;
       JointVals[LEFT_HIP_R] = 0.0f;

       if(cyl[vid] == 1) { // power kick forward 
          JointVals[LEFT_HIP_P] =  (stp[vid])* angle/2 * cyl[vid]+20.0f;
          JointVals[LEFT_KNEE_P] = (stp[vid]+1.0f) *(-1.0f)* angle-50.0f;       
          JointVals[LEFT_ANKLE_P] = 0.0f;
          JointVals[LEFT_TOE_P] =  0.0f;
       }
       else {
          JointVals[LEFT_HIP_P] =  (stp[vid])* angle/2 * cyl[vid];
          JointVals[LEFT_KNEE_P] =  (stp[vid]+1.0f) * angle/2 *(-1.0f) ;
          JointVals[LEFT_ANKLE_P] = 10.0f;
          JointVals[LEFT_TOE_P] = 10.0f;
          posture.xyz[Z] -= 0.5f;
       }
       // Left arm 
       JointVals[LEFT_SHOULDER_H] = 0.0f;
       JointVals[LEFT_SHOULDER_R] = 10.0f;
       JointVals[LEFT_SHOULDER_P] = cyl[vid]*(-angle/1.5)*stp[vid];
       JointVals[LEFT_ELBOW_P] = 30.0f;

   }  // end of for loop of walking step 1 

   // cyl = 1: Return movements 
   else if (stp[vid] > steps)  {

       // cyl = 1: RIGHT LEG returning forward from backward position 
       JointVals[RIGHT_TOE_P] = 0.0f;
       JointVals[RIGHT_ANKLE_H] = JointVals[RIGHT_ANKLE_R] = 0.0f;
       JointVals[RIGHT_ANKLE_P]= 0.0f ;
       JointVals[RIGHT_TOE_P]= 5.0f;
       JointVals[RIGHT_HIP_H] = JointVals[RIGHT_HIP_R] = 0.0f;
       if(cyl[vid] == 1) {
          JointVals[RIGHT_KNEE_P] = ((stp[vid]-(steps)) * angle - 80.0f);
          JointVals[RIGHT_HIP_P] =
                ((stp[vid]-(steps)) * angle - 20.0f) * cyl[vid];
       }
       else {
          JointVals[RIGHT_KNEE_P] = ((stp[vid]-(steps)) * angle - 40.0f);
          JointVals[RIGHT_HIP_P] = 
                ((stp[vid]-(steps)) * angle - 40.0f) * cyl[vid];
       }
       // Right arm 
       JointVals[RIGHT_SHOULDER_H]= 0.0f;
       JointVals[RIGHT_SHOULDER_R] = -10.0f;
       JointVals[RIGHT_SHOULDER_P] = cyl[vid]*(angle/1.5*(2*steps-stp[vid]));
       JointVals[RIGHT_ELBOW_P] = 30.0f;

       // cyl = 1: LEFT LEG returning from forward position 
       JointVals[LEFT_TOE_P] = 0.0f;
       JointVals[LEFT_ANKLE_H] = JointVals[LEFT_ANKLE_R]=  0.0f;
       JointVals[LEFT_ANKLE_P]= 0.0f ;
       JointVals[LEFT_TOE_P]= 5.0f;
       JointVals[LEFT_HIP_H] = JointVals[LEFT_HIP_R] = 0.0f;
       if(cyl[vid] == 1) {
          JointVals[LEFT_KNEE_P] = -(40.0f - (stp[vid]-(steps)) * angle);
          JointVals[LEFT_HIP_P] =
                (40.0f - (stp[vid]-(steps)) * angle) * cyl[vid];
       }
       else {
          JointVals[LEFT_KNEE_P] = -(80.0f - (stp[vid]-(steps)) * angle);
          JointVals[LEFT_HIP_P] =
                (20.0f - (stp[vid]-(steps)) * angle) * cyl[vid];
       }
       // Left arm 
       JointVals[LEFT_SHOULDER_H] = 0.0f;
       JointVals[LEFT_SHOULDER_R] = 10.0f;
       JointVals[LEFT_SHOULDER_P] = cyl[vid]*(-angle/1.5*(2*steps-stp[vid]));
       JointVals[LEFT_ELBOW_P] = 30.0f;


   }  // end of for loop of walking step 2 

   stp[vid] += 1.0f;
   if(stp[vid] == 2*steps+1.0f) {
      stp[vid] = 1.0f;
      cyl[vid] = -cyl[vid];
   }
} // end PERSON_VEH::walk()


void PERSON_VEH::initMove(int& old_tethered, int& tethered)
{
   old_tethered = tethered;

   head[ROT_ANGLE] = look.hpr[HEADING];
   head[ELEV_ANGLE] = look.hpr[PITCH];
   if (( old_tethered > 0 ) && ( tethered == 0 )) {

      if (!grnd_orient2(posture)) {
         posture.xyz[Z] = 3.5f;
      }
   }
} // end PERSON_VEH::initMove()


void PERSON_VEH::determineSpeed(CONTROL &cdata, int ) 
{
   float speed_setting;
   //float newmountheading;
 
//   oldspeed = speed;

   // Vehicle pitch is determined by the ground, so roll increments will
   // be used to increment the heading. Pitch increments are ignored.
   posture.hpr[HEADING] -= 3.0f*cdata.roll;

   if (posture.hpr[HEADING] < 0.0f) {
      posture.hpr[HEADING] += 360.0f;
   }

   // If using flight controls, speed must be controlled different because 
   // the throttle remains at a set location, while when using the keyboard
   // you only press a key at add or subtract speed
   if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) ||
       (input_control == NPS_BASIC_PANEL) || (input_control == NPS_FLYBOX) ||
       (input_control == NPS_LEATHER_FLYBOX) ) {

      if (input_control == NPS_LEATHER_FLYBOX) {
         cdata.thrust = -cdata.pitch;
         cdata.pitch = 0.0f;
      }
      speed_setting = cdata.thrust * MAX_PERSON_SPEED;

      if (speed < speed_setting) {
         speed = float(int(speed_setting));
      }
      else if (speed > speed_setting) {
         speed = float(int(speed_setting));
      }
      cdata.roll = cdata.pitch = 0.0f;
/*
      if ((speed < KEY_PERSON_SPEED_CHANGE) && (speed > 0.0f)) {
         speed = 0.0f; // no drifting allowed
         speed_setting = 0.0f;
      }
      else if((-speed < KEY_PERSON_SPEED_CHANGE) &&
         (-speed > 0.0f)) {
         speed = 0.0f; // no drifting allowed
         speed_setting = 0.0f;
      }
*/
   } // end if

   else if ( (input_control == NPS_TESTPORT) ||
             (input_control == NPS_UNIPORT) ||
             (input_control == NPS_TREADPORT) ) {

      speed = pfLengthVec3 (velocity);

      pfVec2 vel, headingvec;

      float hdg;

      vel[X] = velocity[X];
      vel[Y] = velocity[Y];
      pfNormalizeVec2(vel);
      hdg = 90.0f+posture.hpr[HEADING];
      if(hdg < 0.0f) {
         hdg += 360.0f;
      }
      else if (hdg > 360.0f) {
         hdg -= 360.0f;
      }

      pfSinCos(hdg, &headingvec[Y],&headingvec[X]);
      // heading is opposite for DR entities
      if(pfDotVec2(headingvec,vel) < 0.0f) {
         speed = -speed;
      }

//      if ((speed < KEY_PERSON_SPEED_CHANGE)
//         && (speed > 0.0f)) {
//         speed = 0.0f; // no drifting allowed
//      }

   }

   else {

      speed += cdata.thrust;

      if ((speed < KEY_PERSON_SPEED_CHANGE)
         && (speed > 0.0f)) {
         speed = 0.0f; // no drifting allowed
      }

      cdata.roll = cdata.pitch = cdata.thrust = 0.0f;
   }

   // Don't want to exceed the max speed
   if((status & Appearance_LifeForm_State_Mask)
      != Appearance_LifeForm_Crawling) {
      if (speed > MAX_PERSON_SPEED) {
         speed = MAX_PERSON_SPEED;
      }
      // Don't want to go backwards too fast
      else if (speed < MAX_PERSON_REVERSE_SPEED) {
         speed = MAX_PERSON_REVERSE_SPEED;
      }
   }
   else if (speed > MAX_PERSON_CRAWL_SPEED) {
      speed = MAX_PERSON_CRAWL_SPEED;
   }

} // end PERSON_VEH::determineSpeed()

#ifndef NONPSSHIP
void PERSON_VEH::mount(int isect_vid) 
{
   if( isect_vid != vid  && mounted_vid == vid) { //&& isect_vid > 0 
//cerr<<"Mounting..."<<isect_vid<<endl;
      mounted_vid = isect_vid; 
      ((SHIP_VEH *)G_vehlist[mounted_vid].vehptr)->mount(this,vid);
      oldmounthpr[HEADING]=G_vehlist[mounted_vid].vehptr->getorientation()[HEADING];
      oldmounthpr[PITCH]=G_vehlist[mounted_vid].vehptr->getorientation()[PITCH];
      oldmounthpr[ROLL]=G_vehlist[mounted_vid].vehptr->getorientation()[ROLL];
      snap_switch = FALSE;
   }
}// end PERSON_VEH::mount()


void PERSON_VEH::dismount() 
{
   if (mounted_vid != vid) {
//cerr<<"Dismounting..."<<endl;
      ((SHIP_VEH *)G_vehlist[mounted_vid].vehptr)->dismount(vid);
      mounted_vid = vid;
      snap_switch = TRUE;
   }
}// end PERSON_VEH::dismount()
#endif

void PERSON_VEH::determineZ(int &falling, pfVec3 *norm)
{
   pfVec3   t_nrm, f_nrm, c_norm;
   int      m_isect=0, t_isect=0, f_isect=0, ix, jx=-1;
   float    t_z, f_z, the_z, tmp=-20.0f, isect[3]={-20.0f,-20.0f,-20.0f};

   pfCopyVec3(c_norm, *norm);
#ifndef NONPSSHIP
   pfVec3   m_nrm = {0.0f, 0.0f, 1.0f};
   float    m_z;

   hit_vid = FALSE;  // Are we already on the mounted vehicle?
   m_isect = get_isect_moving (m_z, m_nrm, posture.xyz);
#endif

   t_isect = get_isect_terrain (t_z, t_nrm, posture.xyz);
   f_isect = get_isect_fixed (f_z, f_nrm, posture.xyz);

   the_z = posture.xyz[Z];

#ifndef NONPSSHIP

#ifdef TRACE
cerr<<"m,t,f:"<<m_isect<<","<<t_isect<<","<<f_isect<<"/"
    <<m_z<<","<<t_z<<","<<f_z<<"/"<<the_z<<endl;
#endif

   if ( ( m_isect ) && 
        ( (m_z - the_z) < MAX_STEP_HIGHT) &&
        ( (m_z - the_z) > MAX_SAFE_FALL_DISTANCE) ) {
      isect[0] = m_z;
   }
   else isect[0] = -20.0f;
#endif

   if ( ( t_isect ) && 
        ( (t_z - the_z) < MAX_STEP_HIGHT) &&
        ( (t_z - the_z) > MAX_SAFE_FALL_DISTANCE) ) {
      isect[1] = t_z;
   }
   else isect[1] = -20.0f;

   if ( ( f_isect ) && 
        ( (f_z - the_z) < MAX_STEP_HIGHT) &&
        ( (f_z - the_z) > MAX_SAFE_FALL_DISTANCE) ) {
      isect[2] = f_z;
   }
   else isect[2] = -20.0f;

   for (ix=0; ix<3; ix++) {
      if(isect[ix] > tmp) { 
         tmp = isect[ix];
         jx = ix;
      }
   }

   if (tmp > -20.0f) {
      posture.xyz[Z] = tmp;
      switch (jx) {
#ifndef NONPSSHIP
      case 0:
         pfCopyVec3 (c_norm, m_nrm);
         break;
#endif
      case 1:
         pfCopyVec3 (c_norm, t_nrm);
         break;
      case 2:
         pfCopyVec3 (c_norm, f_nrm);
         break;
      default:
         break;
      }
      falling = FALSE;      
   }
   else {
      if (m_isect || t_isect || f_isect) {
         falling = FALSE;
         c_norm[Z] = 1.0f;
         c_norm[Y] = 0.0f;
         c_norm[X] = 0.0f;
      }
   }

   if (t_isect) {
      altitude = posture.xyz[Z] - t_z;
   }

#ifndef NONPSSHIP
   // Mount the person to the vehicle
   if ((m_isect) && !jx && (mounted_vid == vid)) {
      hit_vid = TRUE; //Are we mounted to a vehicle? Now we are

      // Get the vid of the mounted vehicle.  Store the value of the global
      // target so we will not destroy its contents when searching for mounted
      // vehicle.
      int tmp_target = G_dynamic_data->target;
      m_isect = get_isect_moving (m_z, m_nrm, posture.xyz);
      int isect_vid = G_dynamic_data->target;
      G_dynamic_data->target = tmp_target;
      mount(isect_vid);
#ifndef NOHIRESNET
      ok_to_send();
#endif // NOHIRESNET
   }

   // Dismount the person from the vehicle
   if ((t_isect || f_isect) && jx  && (mounted_vid != vid)) {
      dismount();
#ifndef NOHIRESNET
      ok_to_send();
#endif // NOHIRESNET
   }
#endif

//   cerr << c_norm[Z] << " "
//        << c_norm[Y] << " "
//        << c_norm[X] << endl;

   pfNormalizeVec3 (c_norm);

   pfCopyVec3(*norm, c_norm);

}


void PERSON_VEH::determinePosture(CONTROL &cdata,
   pfVec3& oldpos, int& hitSomething) 
{
   input_manager *l_input_manager = G_manager->ofinput;

   pfMatrix orient;
   pfVec3   direction = { 0.0f, 1.0f, 0.0f }, c_norm = {0.0f, 0.0f, 1.0f};
   int      falling = TRUE;

   // Snap human to either ground or mounted vehicle
   determineZ(falling, &c_norm);

   // Now see if we are going to hit something in this time interval
   if (!G_static_data->stealth) {

      (G_dynamic_data->birdseye) ? turn_on_heads() : turn_off_heads();

      if ( check_collide(oldpos, posture.xyz,
                         posture.hpr,
                         speed, altitude) ) {
         cdata.thrust = 0.0f;
         pfSetVec3 ( velocity, 0.0f, 0.0f, 0.0f );
         // return(HITSOMETHING);
         hitSomething = TRUE;
      } // end if
   } // end if

   if(!hitSomething) {
      switch (input_control) {
#ifndef NO_IPORT
         toIport outPack;

         case NPS_TESTPORT:
            pfCopyVec3 (outPack.normal, c_norm);
            pfCopyVec3 (outPack.xyz, posture.xyz);
            outPack.flags = IPORT_TELEPORT_FL;

            l_input_manager->set_multival ( (void *) &(outPack),
                                           NPS_TESTPORT,
                                           NPS_MV_IPORT_SEND);
            break;
         case NPS_UNIPORT:
            pfCopyVec3 (outPack.normal, c_norm);
            pfCopyVec3 (outPack.xyz, posture.xyz);
            outPack.flags = IPORT_TELEPORT_FL;

            l_input_manager->set_multival ( (void *) &(outPack),
                                           NPS_UNIPORT,
                                           NPS_MV_IPORT_SEND);
            break;
#endif // NO_IPORT

         default:
            break;
      } // end switch

      if (falling) {
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         G_dynamic_data->dead = DYING;
         G_dynamic_data->cause_of_death = FALLING_DEATH;
         hitSomething = TRUE;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
      } // end if

      // Make sure we are still in bounds, if not, stop it at the boundary.
      if (at_a_boundary(posture.xyz)) {
         posture.hpr[HEADING] =
            fmod(posture.hpr[HEADING] + 180.0f, 360.0f);
//         pfNegateVec3(velocity,Zerovec);
      } // end if
      else { // Still in bounds
         // Calculate the new velocity vector
         pfMakeEulerMat ( orient, posture.hpr[HEADING],
            posture.hpr[PITCH],
            posture.hpr[ROLL]);

         pfFullXformPt3 ( direction, direction, orient );
         pfNormalizeVec3 ( direction );
         pfScaleVec3 ( velocity, speed, direction);
      } // end else

   } // end if
   else {
      switch (input_control) {

#ifndef NO_IPORT

         toIport outPack;

         case NPS_TESTPORT:
            pfCopyVec3 (outPack.normal, c_norm);
            pfCopyVec3 (outPack.xyz, posture.xyz);
            outPack.flags = FORCE_STOP_FL;

            l_input_manager->set_multival ( (void *) &(outPack),
                                           NPS_TESTPORT,
                                           NPS_MV_IPORT_SEND);
            break;
         case NPS_UNIPORT:
            pfCopyVec3 (outPack.normal, c_norm);
            pfCopyVec3 (outPack.xyz, posture.xyz);
            outPack.flags = FORCE_STOP_FL;

            l_input_manager->set_multival ( (void *) &(outPack),
                                           NPS_UNIPORT,
                                           NPS_MV_IPORT_SEND);
            break;

#endif // NO_IPORT

         default:
            break;
      } // end switch

   } // else 

#ifndef NONPSSHIP
   updateRelPosVec();
#endif

} // end PERSON_VEH::determinePosture()


void PERSON_VEH::handleTether(CONTROL &cdata,int tethered) 
{
   if ((cdata.vid > -1) && G_vehlist[cdata.vid].vehptr) {

      if ( tethered == 1) {
         G_vehlist[cdata.vid].vehptr->teleport(posture,
            speed);
      }
      else if ( tethered == 2) {
         G_vehlist[cdata.vid].vehptr->attach(posture,
            speed, G_dynamic_data->attach_dis);
      }

   }
   else {
#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
      tethered = FALSE;
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
   }
} // end PERSON_VEH::handleTether()


void PERSON_VEH::computeFirepoint()
{

   pfMatrix orient;
   pfVec3 up = {0.0f,0.0f,1.0f};

   pfMakeEulerMat ( orient, posture.hpr[HEADING],
                            posture.hpr[PITCH],
                            posture.hpr[ROLL]);
   pfFullXformPt3 ( up, up, orient );
   pfNormalizeVec3 ( up );
      pfScaleVec3 ( up, EYE_OFFSET, up); // EYE_OFFSET = 2.0f
   pfAddVec3 ( eyepoint, posture.xyz, up );

   pfCopyVec3 ( firepoint, eyepoint );

} // end PERSON_VEH::computeFirepoint()


void PERSON_VEH::checkUnderground(int tethered) 
{

   altitude = ::get_altitude(posture.xyz);
   if ( (!tethered) && (!G_static_data->stealth) && (altitude < -5.0) ) {
#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
//      G_dynamic_data->cause_of_death = UNDER_GROUND_DEATH;
//      G_dynamic_data->dead = DYING;
      posture.xyz[PF_Z] -= altitude;
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
   }
} // end PERSON_VEH::checkUnderground()


void PERSON_VEH::placeVehicle() 
{

   pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
   pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);

} // end PERSON_VEH::placeVehicle()

movestat PERSON_VEH::move()
// move the vehicle for the next frame
{
   pfVec3 oldpos;
   int hitSomething = FALSE;
   static int old_tethered;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tPERSON_VEH::move" << endl;
#endif

#ifdef TRACE
   cerr << "Moving a PERSON_VEH\n";
#endif
   update_time ();
   collect_inputs ();

   initMove(old_tethered,tethered);
   pfCopyVec3(oldpos,posture.xyz);

   if (!(paused || tethered || cdata.rigid_body) ) {

      determineSpeed(cdata,input_control);

      //move the GROUND vehicle type in the x-y plane
      posture.xyz[X] += velocity[X]*cdata.delta_time;
      posture.xyz[Y] += velocity[Y]*cdata.delta_time;

      determinePosture(cdata,oldpos,hitSomething);
      if(hitSomething) {
         return(HITSOMETHING);
      }

      // Copy new posture values from pass_data into the vehicle's posture
      posture = posture;

   } // End !paused !tethered !rigid_body

   // If tethered, let the other vehicle calculate our position for us
   if (tethered) {
      handleTether(cdata,tethered);
   }

   computeFirepoint();

   checkUnderground(tethered);

   check_targets ( posture.xyz, posture.hpr,
                   look.hpr );

   //put the vehicle in the correct location in space
   placeVehicle();

   rot_elev_head();
   normalize_angles();
   apply_angles();

   thrust = cdata.thrust;
   oldstatus = status;
   return(FINEWITHME);

} // end PERSON_VEH::move()

int PERSON_VEH::check_collide(pfVec3 &startpnt, pfVec3 &endpnt,
                              pfVec3 &orientation,
                              float &speed, float &altitude)
{
   long     isect;
   pfSegSet seg;
   pfHit    **hits[32];
   long     flags;
//   pfNode   *node;
   pfGeoSet *gset;
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
// Barham added PFTRAV_IS_CULL_BACK at AUSA95,10/14/95.
      seg.mode = PFTRAV_IS_PRIM | PFTRAV_IS_CULL_BACK;
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
//          pfQueryHit(*hits[0], PFQHIT_NODE, &node);
          pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
          pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
          pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
          pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
          hit_index = 0;
          }
      else
          {
          pfQueryHit(*hits[1], PFQHIT_FLAGS, &flags);
//          pfQueryHit(*hits[1], PFQHIT_NODE, &node);
          pfQueryHit(*hits[1], PFQHIT_GSET, &gset);
          pfQueryHit(*hits[1], PFQHIT_NORM, &normal);
          pfQueryHit(*hits[1], PFQHIT_XFORM, &xform);
          pfQueryHit(*hits[1], PFQHIT_POINT, &pnt);
          hit_index = 1;
          }

//      the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
      the_mask = pfGetGSetIsectMask(gset);
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
         }

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
         } 

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
         }

      }      
    
      //if we got here it did not hit anything 
      return FINEWITHME;

   }


void PERSON_VEH::computeDR(float deltatime) 
{
   float accelfac, altitude;

   if ( snap_switch == TRUE ) {
     altitude = ::get_altitude(drpos.xyz);
   }

   switch(dralgo) {

      case DRAlgo_Static: //it is fixed, so it does not move
         break;
      case DRAlgo_DRM_FPW: //Fixed orientation,Constant Speed
         // Move in the X-Y plane
         drpos.xyz[X] += drparam.xyz[X]*deltatime;
         drpos.xyz[Y] += drparam.xyz[Y]*deltatime;
         // Now put it on the ground and set HPR
         if ( (altitude > -5.0f) && (altitude < 5.0f) ) {
            if ( snap_switch == TRUE ) {
               person_orient(drpos);
            }
         }
         else {
            drpos.xyz[Z] += drparam.xyz[Z] * deltatime;
         }
         break;

      case DRAlgo_DRM_FVW: //Constant rotation,varable velocity
         accelfac = 0.5f*deltatime*deltatime;
         drpos.xyz[X] += velocity[X]*deltatime+accelfac*velocity[X];
         drpos.xyz[Y] += velocity[Y]*deltatime+accelfac*velocity[Y];
         drparam.xyz[X] += accelfac*velocity[X];
         drparam.xyz[Y] += accelfac*velocity[Y];
         drparam.xyz[Z] += accelfac*velocity[Z];
         // Now put it on the ground and set HPR
         if ( (altitude > -5.0f) && (altitude < 5.0f) ) {
            if ( snap_switch == TRUE ) {
               person_orient(drparam);
            }
         }
         else {
            drpos.xyz[Z] += velocity[Z]*deltatime+accelfac*velocity[Z];
         }
         break;

      //I don't have a clue how to do the others, so we will default
      case DRAlgo_Other:
      case DRAlgo_DRM_RPW:
      case DRAlgo_DRM_RVW:
      case DRAlgo_DRM_FPB:
      case DRAlgo_DRM_RPB:
      case DRAlgo_DRM_RVB:
      case DRAlgo_DRM_FVB:
      default:
         //move the GROUND vehicle type in the x-y plane
         drpos.xyz[X] += velocity[X]*deltatime;
         drpos.xyz[Y] += velocity[Y]*deltatime;

         // Now put it on the ground and set HPR
         if ( (altitude > -5.0f) && (altitude < 5.0f) ) {
            if ( snap_switch == TRUE ) {
               if (!person_orient(drpos)) {
                  drpos.xyz[Z] = 3.5f;
               }
            }
         }
         else {
            drpos.xyz[Z] += velocity[Z]*deltatime;
         }

         drpos.hpr[PITCH] = (drpos.hpr[ROLL] = 0.0f);
         break;
   } // end switch

} // end PERSON_VEH::computeDR()


void PERSON_VEH::time2Send(float&) 
{
#ifdef DEBUG
   cerr <<"X       " << posture.xyz[HEADING] << " " <<drpos.xyz[HEADING] << "\n";
   cerr <<"Y       " << posture.xyz[PITCH] << " " <<drpos.xyz[PITCH] << "\n";
   cerr <<"Z       " << posture.xyz[ROLL] << " " <<drpos.xyz[ROLL] << "\n";
   cerr <<"HDG     " << posture.hpr[HEADING] << " " <<drpos.hpr[HEADING] << "\n";
   cerr <<"PITCH   " << posture.hpr[PITCH] << " " <<drpos.hpr[PITCH] << "\n";
   cerr <<"ROLL    " << posture.hpr[ROLL] << " " <<drpos.hpr[ROLL] << "\n";
   cerr << "send a delta pdu\n";
#endif
   // Sufficient difference in rotation,location, or time to require a PDU
   sendentitystate();
} // end PERSON_VEH::time2Send()


void PERSON_VEH::reason2Send(float& speed) 
{
   if ( speed != oldspeed ) {
      if ( (!IS_ZERO(oldspeed)) && (IS_ZERO(speed)) ) {
         sendentitystate();
      }
      else if ( (IS_ZERO(oldspeed)) && (!IS_ZERO(speed)) ) {
         sendentitystate();
      }
      else if ( fabs(oldspeed-speed) >= 1.0f ) {
         sendentitystate();
      }
   }
   else if(oldstatus != status) { // appearance has changed for DI
      sendentitystate();
   }
   else if(num_joints > 0) {

      // sufficient change in head heading
      float hdg = (fabs(fmodf(JointVals[HEAD_H]-360.0f,360.0f))
          > fabs(fmodf(JointVals[NECK_P]-360.0f,360.0f)))
         ? JointVals[HEAD_H] : JointVals[NECK_P];
      float  pitch = (fabs(fmodf(JointVals[HEAD_P]-360.0f,360.0f))
          > fabs(fmodf(JointVals[NECK_H]-360.0f,360.0f)))
         ? JointVals[HEAD_P] : JointVals[NECK_H];
      if((fabs(head[DRROT_ANGLE] - hdg) >= 3.0f)) {
         sendentitystate();
      }

      // sufficient change in head elevation
      else if ( ( num_joints > 1 ) &&
         (fabs(head[DRELEV_ANGLE] - pitch) >= 3.0f)) {
         sendentitystate();
      }

   }

} // end PERSON_VEH::reason2Send()


movestat PERSON_VEH::moveDR(float deltatime,float curtime) 
{
//move routine for DR vehciles simple move
   float speed;

#ifdef TRACE
   cerr << "Dead Recon a PLAIN DI.\n";
#endif

#ifndef NO_PFSMOKE
   update_pfsmokes();
#endif

   if(((curtime - lastPDU) > G_static_data->DIS_deactivate_delta ) && 
      (vid != G_drivenveh)) { 
      //Deactivate the puppy
      return(DEACTIVE);
   }

   computeDR(deltatime);

   if (vid != G_drivenveh){
      // Update DR entity 
      posture = drpos;

      //put the vehicle in the correct location in space
      placeVehicle();
   }
   else if ((!unitMember) &&
      (!G_static_data->stealth)) { // sendentitystate() only if G_drivenveh
      speed = pfLengthVec3(velocity);

      // This is us, have we exceeded the DR error or time limit?
      if((G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) || 
         (delta_send(&posture,&drpos))) {
         time2Send(speed); 
      }
      else {
         reason2Send(speed);
      }

      oldstatus = status;

   } // End if not G_drivenveh and not stealth

   return(FINEWITHME);

} // end PERSON_VEH::moveDR()


void PERSON_VEH::sendentitystate()
//send a entity state PDU with articulated parts
{
  ArticulatParams       *APNptr;
  float temp_float;
  char tempstr[MARKINGS_LEN];
  int num_art_par = 0;
  EntityStatePDU epdu;

  bzero ( (char *)&epdu, sizeof(EntityStatePDU) );
  basicentitystate(&epdu);

  //They are all in cammies
  epdu.entity_appearance |= Appearance_CamoPaint;

  //Send hand signal Index as character in TBD field
  sprintf(tempstr,"%d",movement_index);
  strcpy(epdu.dead_reckon_params.TBD,tempstr);

  epdu.entity_marking.character_set = CharSet_ASCII;
  strcpy ( (char *)epdu.entity_marking.markings, 
          (char *)G_static_data->marking );

#ifdef DEBUG
cerr << "DIS Heading (psi) = " << epdu->entity_orientation.psi
     << "\n    Pitch (theta) = " << epdu->entity_orientation.theta
     << "\n     Roll (phi)   = " << epdu->entity_orientation.phi << endl;
#endif


   // Send articulated part info - only head rot/elev 
  if ( num_joints > 0 ) {

     // SEND HEAD AZIMUTH (heading) 
     APNptr = epdu.articulat_params+0; 
     {
        head[DRROT_ANGLE] = (fabs(fmodf(JointVals[HEAD_H]-360.0f,360.0f)) 
           > fabs(fmodf(JointVals[NECK_P]-360.0f,360.0f))) 
           ? JointVals[HEAD_H] : JointVals[NECK_P];
        APNptr->change = (unsigned short)0;
        APNptr->ID = (unsigned short)0x0;
        APNptr->parameter_type = (unsigned int)4107;
#ifdef DIS_2v4
        APNptr->parameter_type_designator = 0;
#endif
        temp_float = ((360.0f - head[DRROT_ANGLE]) * DEG_TO_RAD);
        bzero ( &(APNptr->parameter_value), 8);
        bcopy ( &temp_float, &(APNptr->parameter_value), 4);
        num_art_par++;
     }
  }

  if ( num_joints > 1 ) {

     // SEND HEAD ELEVATION 
     APNptr = epdu.articulat_params+1; 
     {
        head[DRELEV_ANGLE] = (fabs(fmodf(JointVals[HEAD_P]-360.0f,360.0f)) 
           > fabs(fmodf(JointVals[NECK_H]-360.0f,360.0f))) 
           ? JointVals[HEAD_P] : JointVals[NECK_H];
        APNptr->change = (unsigned short)0;
        APNptr->ID = (unsigned short)0x1;
        APNptr->parameter_type = (unsigned int)4429;
#ifdef DIS_2v4
        APNptr->parameter_type_designator = 0;
#endif
        temp_float = (-head[DRELEV_ANGLE] * DEG_TO_RAD);
        bzero ( &(APNptr->parameter_value), 8);
        bcopy ( &temp_float, &(APNptr->parameter_value), 4);
        num_art_par++;
     }
  }

  epdu.num_articulat_params = (unsigned char)num_art_par;

  sendEPDU(&epdu);
#ifdef DEBUG
  cerr << "sent a Person Entity State PDU\n";
#endif

  updateDRparams();

} // end PERSON_VEH::sendentitystate()


void PERSON_VEH::entitystateupdate(EntityStatePDU *epdu, sender_info &sender)
//get an entity state PDU with articulated parts
{
   ArticulatParams  *APNptr;
   float temp_float;
   int count = 0;

   //what and who I am
   disname = epdu->entity_id;
   type = epdu->entity_type;
   force = epdu->force_id;

   //How fast is it going
   velocity[X] = epdu->entity_velocity.x;
   velocity[Y] = epdu->entity_velocity.y;
   velocity[Z] = epdu->entity_velocity.z;
   speed = pfLengthVec3(velocity);

   //Where we are
   posture.xyz[X] = (float)epdu->entity_location.x;
   posture.xyz[Y] = (float)epdu->entity_location.y;
   posture.xyz[Z] = (float)epdu->entity_location.z;

   //How we are facing
//   posture.hpr[HEADING] =  (epdu->entity_orientation.psi < 0)?  
//        -(epdu->entity_orientation.psi + 2*M_PI)*RAD_TO_DEG:
//        -(epdu->entity_orientation.psi)*RAD_TO_DEG;
   posture.hpr[HEADING] =  (-(epdu->entity_orientation.psi))*RAD_TO_DEG;
   posture.hpr[PITCH]   =  epdu->entity_orientation.theta*RAD_TO_DEG;
   posture.hpr[ROLL]    =  epdu->entity_orientation.phi*RAD_TO_DEG;

   //Acceleration vector values
   acceleration[X] = epdu->dead_reckon_params.linear_accel[X];
   acceleration[Y] = epdu->dead_reckon_params.linear_accel[Y];
   acceleration[Z] = epdu->dead_reckon_params.linear_accel[Z];

   pfCopyVec3(drparam.xyz,velocity);
   pfCopyVec3(drparam.hpr,acceleration);
   drpos=posture;


   APNptr = epdu->articulat_params;
   count = 0;
   while ( count < epdu->num_articulat_params ) {

      if ( (APNptr->ID == (unsigned short)0x0) &&
#ifdef DIS_2v4
           (APNptr->parameter_type_designator == 0) &&
#endif
           (APNptr->parameter_type == (unsigned int)4107) ) {
         bcopy ( &(APNptr->parameter_value),
                 &temp_float, 4);
         JointVals[HEAD_H] = (360.0f - (temp_float*RAD_TO_DEG));
         while ( JointVals[HEAD_H] < 0.0f )
            JointVals[HEAD_H] += 360.0f;
         JointVals[HEAD_H] = fmod ( JointVals[HEAD_H], 360.0f );
      }
      else if ( (APNptr->ID == (unsigned short)0x0) &&
#ifdef DIS_2v4
                (APNptr->parameter_type_designator == 0) &&
#endif
                (APNptr->parameter_type == (unsigned int)4108) ) {
         bcopy ( &(APNptr->parameter_value),
                 &temp_float, 4);
         head[ROT_RATE] = float(-temp_float * RAD_TO_DEG);
      }
      else if ( (APNptr->ID == (unsigned short)0x1) &&
#ifdef DIS_2v4
                (APNptr->parameter_type_designator == 0) &&
#endif
                (APNptr->parameter_type == (unsigned int)4429) ) {
         bcopy ( &(APNptr->parameter_value),
                 &temp_float, 4);
         JointVals[HEAD_P] = float(-temp_float * RAD_TO_DEG);
      }
      APNptr++;
      count++;
   }

   rot_elev_head();

   if((strcmp(controller.internet_address,sender.internet_address))!=0) {
     struct hostent *entity;
     unsigned long netaddress = inet_addr(sender.internet_address);
     strcpy ( controller.internet_address, sender.internet_address );
     entity = gethostbyaddr ( (void *)&netaddress,
                              sizeof(struct in_addr), AF_INET );
     if ( entity != NULL ) {
        strcpy ( controller.host_name, entity->h_name );
     }
     else {
        strcpy ( controller.host_name, "Unknown" );
     }
   }

   //what we look like
   status = epdu->entity_appearance;

   changestatus(status);

   //Update the time of last message
   lastPDU = G_curtime;

#ifndef NONPSSHIP
   //Must update relative postion when mounted on ship
   updateRelPosVec();
#endif

} // end PERSON_VEH::entitystateupdate()


int
PERSON_VEH::posture_chg_ok()
{

   return ( (posture_chg_time + 0.5f) < G_curtime );

}

void
PERSON_VEH::get_weapon_view (pfCoord &point)
{
   pfAddVec3 (point.xyz, posture.xyz, gun_pos.xyz);
   pfAddVec3 (point.hpr, posture.hpr, gun_pos.hpr);
/*
cerr << "posture: " << endl
     << "\t" << posture.xyz[PF_X] << "  "
             << posture.xyz[PF_Y] << "  "
             << posture.xyz[PF_Z] << "  "
             << posture.hpr[PF_H] << "  "
             << posture.hpr[PF_P] << "  "
             << posture.hpr[PF_R] << endl;
cerr << "gun_pos: " << endl
     << "\t" << gun_pos.xyz[PF_X] << "  "
             << gun_pos.xyz[PF_Y] << "  "
             << gun_pos.xyz[PF_Z] << "  "
             << gun_pos.hpr[PF_H] << "  "
             << gun_pos.hpr[PF_P] << "  "
             << gun_pos.hpr[PF_R] << endl;
cerr << "point: " << endl
     << "\t" << point.xyz[PF_X] << "  "
             << point.xyz[PF_Y] << "  "
             << point.xyz[PF_Z] << "  "
             << point.hpr[PF_H] << "  "
             << point.hpr[PF_P] << "  "
             << point.hpr[PF_R] << endl;
*/
}

int
PERSON_VEH::lase ()
{
   input_manager *l_input_manager = G_manager->ofinput;
   pfVec3 point;
   pfCoord pos;
   pfAddVec3 (pos.xyz, gun_pos.xyz, posture.xyz);
   pfAddVec3 (pos.hpr, gun_pos.hpr, posture.hpr);
   long isect = lase_isect (pos, point);
   float length = 0.0f;

   if (isect) {
      pfSubVec3 (point, point, gun_pos.xyz);
      length = PFLENGTH_VEC3 (point);
   }
   else {
      length = -1.0;
   }


   if (l_input_manager->verify_device (NPS_LWS)) {
      float data[3];
      data[0] = pos.hpr[PF_P];
      data[1] = pos.hpr[PF_H];
      data[2] = length;
cerr << "Lase Info: " << data[0] << "  "
                      << data[1] << "  "
                      << data[2] << endl;

      l_input_manager->set_multival ((void *) data, NPS_LWS, NPS_LW_LAZE);
   }

   return FALSE;
}


int
#ifndef NONPSSHIP
PERSON_VEH::transport ( float new_x, float new_y, float new_z )
#else
PERSON_VEH::transport ( float new_x, float new_y, float )
#endif
{
   pfVec3 temp_pos;//, viewpoint, vieworient;
   float curr_alt;

#ifndef NONPSSHIP

   pfMatrix orient;
   float temp_pos_len;
   pfBox *bbox;
   //Check transport location against mounted vehicle bounding box
   if (mounted_vid != vid) {
      bbox = &(G_vehtype[G_vehlist[mounted_vid].type].bounding_box);
//cerr<<"Transport:"<<new_x<<" "<<new_y<<" "<<new_z<<endl;
//cerr<<"Bbox:"<<bbox->min[X]<<" "<<bbox->max[X]<<" "
//             <<bbox->min[Y]<<" "<<bbox->max[Y]<<" "
//             <<bbox->min[Z]<<" "<<bbox->max[Z]<<endl;
      if ( (bbox->min[X] <= new_x) &&
           (new_x <= bbox->max[X]) &&
           (bbox->min[Y] <= new_y) &&
           (new_y <= bbox->max[Y]) ) {
         pfSetVec3(temp_pos, new_x, new_y, new_z);
         temp_pos_len = pfLengthVec3(temp_pos);
         pfMakeEulerMat ( orient, oldmounthpr[HEADING],
                                  oldmounthpr[PITCH],
                                  oldmounthpr[ROLL]);
         pfNormalizeVec3(temp_pos);
         pfXformVec3(temp_pos, temp_pos, orient);
         pfScaleVec3(relposvec, temp_pos_len, temp_pos);
         posture.xyz[Z] = new_z;
         reset_on_mounted_vid();

         //Update the view to reset the ship's cell locator
         //pfGetChanView(G_dynamic_data->pipe[0].channel[0].chan_ptr, 
         //              viewpoint, vieworient);
         pfChanView(G_dynamic_data->pipe[0].channel[0].chan_ptr,
                    posture.xyz, posture.hpr);
         ((SHIP_WALK_VEH *)G_vehlist[mounted_vid].vehptr)->update_locator();
         //pfChanView(G_dynamic_data->pipe[0].channel[0].chan_ptr,
         //           viewpoint, vieworient);
         return TRUE;
      }
   }
#endif

   //Check transport location against rest of scene
   if ( (G_dynamic_data->bounding_box.xmin <= new_x) &&
        (new_x <= G_dynamic_data->bounding_box.xmax) &&
        (G_dynamic_data->bounding_box.ymin <= new_y) &&
        (new_y <= G_dynamic_data->bounding_box.ymax) ) {
      pfCopyVec3 ( temp_pos, posture.xyz );
      temp_pos[X] = new_x;
      temp_pos[Y] = new_y;
      // leave temp_pos.xyz[Z] from old posture as good guess
      curr_alt = ::get_altitude(temp_pos);
      temp_pos[Z] -= curr_alt;
      pfCopyVec3 ( posture.xyz, temp_pos );
      return TRUE;
   }
   return FALSE;
}

#ifndef NONPSSHIP

int get_isect_moving (float &iPnt, pfVec3 &c_norm, pfVec3 xyz)
{
   static pfVec3 lastValidNrm = {0.0f, 0.0f, 1.0f};

   ulong         t_isect;
   ulong         t_flags;
   pfSegSet      segment;
   pfHit       **hits[32];
   pfVec3        pnt;
   char		*name;
   pfNode	*node;
   long		 the_mask;
   int           the_item_hit, success = FALSE;


   segment.userData = NULL;
   segment.bound = NULL;
   if (hit_vid) //Have we already mounted a vehicle ?
      segment.discFunc = mounthit;
   else
      segment.discFunc = NULL;
   segment.mode = PFTRAV_IS_PRIM | PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = ENTITY_MASK; //PERMANENT_MASK;
   pfCopyVec3(segment.segs[0].pos, xyz);
   segment.segs[0].pos[Z] += 0.5f;
   pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   segment.segs[0].length =
            fabs(xyz[Z]) + MAX_GROUND_HEIGHT + 100.0f;
 
   t_isect = pfNodeIsectSegs(G_moving, &segment, hits);

   if (t_isect) {
//cerr<<"Found Ship Deck Below...";
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &t_flags);
      if (t_flags & PFHIT_NORM) {
         pfQueryHit(*hits[0], PFQHIT_NORM, c_norm);
         pfCopyVec3(lastValidNrm, c_norm);
      }
      else {
         pfCopyVec3(c_norm, lastValidNrm);
         cerr << "Using last valid normal." << endl;
      }
      pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
      pfQueryHit(*hits[0], PFQHIT_NAME, &name); //cerr<<"Name:"<<name<<" ";
      pfQueryHit(*hits[0], PFQHIT_NODE, &node);
      the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
      the_item_hit = int(the_mask & ENTITY_MASK);
//cerr<<"Entity Mask:"<<the_item_hit<<" ";
      iPnt = pnt[Z];
//cerr<<"m_isect point:("<<pnt[X]<<","<<pnt[Y]<<","<<pnt[Z]<<")"<<endl;
      success = TRUE;
   }
   return success;
}

#endif
