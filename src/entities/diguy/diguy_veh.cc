//#define NODIGUY
//#define DEBUG_DI_ACTION

#include <math.h>
#include <string.h>
#include <bstring.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "vehicle.h"
#include "human_constants.h"
#include "pdu.h"
#include "diguy_veh.h"
#ifndef NODIGUY
#include "libbdihml.h"
#include "libhmlchar.h"
#include "diguy.h"
#endif
#include "common_defs2.h"
#include "macros.h"
//#include "terrain.h"
#include "dirt_intersect.h"
#include "bbox.h"
#include "sound.h"
#include "disnetlib.h"
#include "netutil.h"
#include "manager.h"
#include "effects.h"
#include "entity.h"
#include "munitions.h"
#include "appearance.h"
#include "conversion_const.h"
#include "interface_const.h"
#include "local_snd_code.h"
#include "collision_const.h"
#include "articulat.h"
#include "diguyTypeLookup.h"
#include "diApperance203.h"

// Initman stuff added to ease the inialization process
// Probably doesn't belong here, but at this point it is the easiest way
// to do it. -rb

#include "im.tab.h"
#include "imstructs.h"
#include "imclass.h"
#include "imdefaults.h"

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#endif

// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile STATIC_DATA     *G_static_data;
extern          VEH_LIST        *G_vehlist;
extern          pfGroup         *G_moving;
extern volatile int              G_drivenveh;
extern          DIS_net_manager *G_net;
#ifndef NOHIRESNET
extern          DIS_net_manager *G_hr_net;
extern volatile float            G_curtime;
#endif // NOHIRESNET
extern volatile MANAGER_REC     *G_manager;
extern volatile float            G_curtime;
extern          pfGroup         *G_fixed;

#ifndef NODIGUY
extern char* diguy_bdu_m16_equipment[];
extern char* diguy_bdu_m203_equipment[];
extern char* diguy_bdu_ak47_equipment[];
extern char* diguy_dcu_m16_equipment[];
extern char* diguy_dcu_m203_equipment[];
extern char* diguy_dcu_ak47_equipment[];
extern char* diguy_land_warrior_equipment[];
#endif

// Locals used by this library
static pfVec3 ZeroVec = {0.0f,0.0f,0.0f};
//static int init;
const float PERSON_INTERSECT_OFFSET = 0.5f;
const float DI_JOG_THRESHOLD = 1.65f;
const float DI_BOX = 0.0f;

#ifndef NODIGUY
static float zHit (bdiCharacterHandle, float, float, float);
static bdiAction get_action (unsigned int, float);
#endif

static float speed_direction(float, pfVec3 &, float);
#ifdef DEBUG_DI_ACTION
static void print_action (bdiAction);
#endif



//************************************************************************//
//****************** class DIGUY_VEH ************************************//
//************************************************************************//

int DIGUY_VEH::init = FALSE;
int DIGUY_VEH::posMech = DIGUY_INTERNAL;

diguyTypeLookup *DIGUY_VEH::lookUp = (diguyTypeLookup *)NULL;

unsigned int DIGUY_VEH::diApperance = Appearance_LifeForm_State_Mask;
unsigned int DIGUY_VEH::diStill = Appearance_LifeForm_UprightStill;
unsigned int DIGUY_VEH::diWalking = Appearance_LifeForm_UprightWalking;
unsigned int DIGUY_VEH::diRunning = Appearance_LifeForm_UprightRunning;
unsigned int DIGUY_VEH::diKneeling = Appearance_LifeForm_Kneeling;
unsigned int DIGUY_VEH::diProne = Appearance_LifeForm_Prone;
unsigned int DIGUY_VEH::diCrawling = Appearance_LifeForm_Crawling;
unsigned int DIGUY_VEH::diSwimming = Appearance_LifeForm_Swimming;
unsigned int DIGUY_VEH::diParachuting = Appearance_LifeForm_Parachuting;
unsigned int DIGUY_VEH::diJumping = Appearance_LifeForm_Jumping;

unsigned int DIGUY_VEH::weaponAppearance = Appearance_LifeForm_Weapon1_Mask;
unsigned int DIGUY_VEH::weaponStowed = Appearance_LifeForm_Weapon1_Stowed;
unsigned int DIGUY_VEH::weaponDeployed = Appearance_LifeForm_Weapon1_Deployed;
unsigned int DIGUY_VEH::weaponFiring = Appearance_LifeForm_Weapon1_Firing;


DIGUY_VEH::DIGUY_VEH(int N,int atype, ForceID the_force)
: VEHICLE(N,atype,the_force)
{
   di_switch = (pfSwitch *)NULL;
   handle = -1;
   diLastTime = 0.0f;
   status = diStill | 
            weaponStowed;

   char **di_equipment = NULL;

   if (lookUp) {
      di_equipment = DIGUY_VEH::lookUp->lookUpType ((int)type.subcategory);
   }
   else {
      if (DIGUY_VEH::init) {
         cerr << "Error in DIGUY_VEH::DIGUY_VEH" << endl
              << "\tdiguyTypeLookup is not initalized." << endl;
      }
   }

#ifndef NODIGUY
   if (DIGUY_VEH::init) {
      if (!di_equipment) {
         di_equipment = diguy_bdu_m16_equipment;
         cerr << "Error in DIGUY_VEH::DIGUY_VEH" << endl
              << "\tNothing returned from diguyTypeLookup." << endl;
      }

      handle = hml_char_create (BDI_SOLDIER, 
                                di_equipment,
                                0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                                SOLDIER_WALK);

      if (DIGUY_VEH::posMech == DIGUY_INTERNAL) {
         hml_char_set_altitude_function(handle, zHit);
      }


      di_model = (pfNode *) hml_char_get_graphics_ptr (handle);

      if (di_model) {
         di_switch = pfNewSwitch ();
         pfAddChild (di_switch, di_model);
         pfAddChild (G_moving, di_switch);
         pfSwitchVal(di_switch, PFSWITCH_ON);
      }
   }

#endif
   oldstatus = status;
   hiResTimeStamp = 0.0f;
   gun_pos.xyz[PF_X] = 0.0f;
   gun_pos.xyz[PF_Y] = 0.0f;
   gun_pos.xyz[PF_Z] = 0.0f;
   gun_pos.hpr[PF_H] = 0.0f;
   gun_pos.hpr[PF_P] = 0.0f;
   gun_pos.hpr[PF_R] = 0.0f;
   target_lock = NO_LOCK;
}

void DIGUY_VEH::del_vehicle()
//delete the object
{
  this->DIGUY_VEH::~DIGUY_VEH();
}

DIGUY_VEH::~DIGUY_VEH()
{
   if (di_model) {
      pfRemoveChild (G_moving, di_switch);
      pfRemoveChild (di_switch, di_model);
      pfDelete (di_switch);
   }
#ifndef NODIGUY
   if (DIGUY_VEH::init) {
      hml_char_destroy (handle);
   }
#endif
}

vtype DIGUY_VEH::gettype()
{
   return (DI_GUY);
}

movestat DIGUY_VEH::move()
// move the vehicle for the next frame
{
   if ((status & diApperance) == 0x0) {
      status |= diStill;
   }
   if ((tether_model_off != -1) && (tethered != 3)) {
      if (G_vehlist[tether_model_off].vehptr) {
         G_vehlist[tether_model_off].vehptr->turnon ();
      }
      tether_model_off = -1;
   }

   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   float speed_setting;
   pfVec3 oldpos;
   float altitude;
   static int old_tethered;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tDIGUY_VEH::move" << endl;
#endif

#ifdef TRACE
   cerr << "Moving a Ground Vehicle\n";
#endif

   old_tethered = tethered;

   update_time ();
   collect_inputs ();


#ifndef NOHIRESNET
   if ((vid == G_drivenveh) && (G_hr_net != NULL)) {
//      if (okToSendHiRes) {
         DataPDU hrDPDU;
         hrDPDU.orig_entity_id = G_vehlist[vid].DISid;
#ifndef DIS_2v4
         hrDPDU.orig_force_id = (ForceID)G_dynamic_data->force;
#endif
         bzero ( (void *) &hrDPDU.recv_entity_id, sizeof (EntityID) );
#ifndef DIS_2v4
         bzero ( (void *) &hrDPDU.recv_force_id, sizeof (ForceID) );
#endif
         // Need to set for id purposes
         hrDPDU.request_id = HIRES_DATA_PDU_MIMIC_TAG; 

         hrDPDU.num_datum_fixed = 13;
         hrDPDU.num_datum_variable = 0;

         hrDPDU.fixed_datum[EPX].datum_id = EPX_TAG;
         hrDPDU.fixed_datum[EPX].datum    = posture.xyz[PF_X];
         hrDPDU.fixed_datum[EPY].datum_id = EPY_TAG;
         hrDPDU.fixed_datum[EPY].datum    = posture.xyz[PF_Y];
         hrDPDU.fixed_datum[EPZ].datum_id = EPZ_TAG;
         hrDPDU.fixed_datum[EPZ].datum    = posture.xyz[PF_Z];
         hrDPDU.fixed_datum[EPH].datum_id = EPH_TAG;
         hrDPDU.fixed_datum[EPH].datum    = posture.hpr[PF_H];
         hrDPDU.fixed_datum[EPP].datum_id = EPP_TAG;
         hrDPDU.fixed_datum[EPP].datum    = posture.hpr[PF_P];
         hrDPDU.fixed_datum[EPR].datum_id = EPR_TAG;
         hrDPDU.fixed_datum[EPR].datum    = posture.hpr[PF_R];
         hrDPDU.fixed_datum[RVX].datum_id = RVX_TAG;
         hrDPDU.fixed_datum[RVX].datum    = gun_pos.xyz[PF_X];
         hrDPDU.fixed_datum[RVY].datum_id = RVY_TAG;
         hrDPDU.fixed_datum[RVY].datum    = gun_pos.xyz[PF_Y];
         hrDPDU.fixed_datum[RVZ].datum_id = RVZ_TAG;
         hrDPDU.fixed_datum[RVZ].datum    = gun_pos.xyz[PF_Z];
         hrDPDU.fixed_datum[RVH].datum_id = RVH_TAG;
         hrDPDU.fixed_datum[RVH].datum    = gun_pos.hpr[PF_H];
         hrDPDU.fixed_datum[RVP].datum_id = RVP_TAG;
         hrDPDU.fixed_datum[RVP].datum    = gun_pos.hpr[PF_P];
         hrDPDU.fixed_datum[RVR].datum_id = RXR_TAG;
         hrDPDU.fixed_datum[RVR].datum    = gun_pos.hpr[PF_R];
         switch (G_dynamic_data->target_lock) {
            case (BLUE_LOCK):
               hrDPDU.fixed_datum[HRTAR].datum_id = T_BLUE;
               break;
            case (RED_LOCK):
               hrDPDU.fixed_datum[HRTAR].datum_id = T_RED;
               break;
            case (WHITE_LOCK):
               hrDPDU.fixed_datum[HRTAR].datum_id = T_WHITE;
               break;
            case (OTHER_LOCK):
               hrDPDU.fixed_datum[HRTAR].datum_id = T_OTHER;
               break;
            default:
               hrDPDU.fixed_datum[HRTAR].datum_id = T_NONE;
               break;
         }
         switch ( G_dynamic_data->hr_network_mode ) {
            case NET_OFF:
            case NET_RECEIVE_ONLY:
               break;
            case NET_SEND_ONLY:
            case NET_SEND_AND_RECEIVE:
            default:
               if (!G_hr_net->write_pdu ( (char *)&hrDPDU, DataPDU_Type)) {
               }
               break;
         }
//         okToSendHiRes = FALSE;
//      }
   }
#endif // NOHIRESNET



   cdata.roll *= 3.0f;


   if (( old_tethered > 0 ) && ( tethered == 0 ))
      {
      if (!grnd_orient2(posture))
         posture.xyz[Z] = 3.5f;
      }
  

   pfCopyVec3(oldpos,posture.xyz);

   if (!(paused || tethered || cdata.rigid_body) )
      {
      // Vehicle pitch is determined by the ground, so roll increments will
      // be used to increment the heading. Pitch increments are ignored.
      posture.hpr[HEADING] -= cdata.roll;
      if (posture.hpr[HEADING] < 0.0f)
         posture.hpr[HEADING] += 360.0f;
  
      // If using flight controls, speed must be controlled different because
      // the throttle remains at a set location, while when using the keyboard
      // you only press a key at add or subtract speed
      if ((input_control == NPS_FCS) || 
          (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_BASIC_PANEL) || 
          (input_control == NPS_FLYBOX) || 
          (input_control == NPS_LEATHER_FLYBOX))
         {

         if (input_control == NPS_LEATHER_FLYBOX)
            {
            cdata.thrust = -cdata.pitch;
            cdata.pitch = 0.0f;
            }

         speed_setting = cdata.thrust  * MAX_PERSON_SPEED;

         if (IS_ZERO(speed_setting)) {
            speed = 0.0f;
         }
         else if (speed < speed_setting)
            speed += FCS_PERSON_SPEED_CHANGE;
         else if (speed > speed_setting) {
            if ( speed > 0.0f ) {
               speed -= (FCS_PERSON_SPEED_CHANGE * 2.0f);
            }
            else {
               speed -= FCS_PERSON_SPEED_CHANGE;
            }
         }
         cdata.roll = cdata.pitch = 0.0f;
         }
      else if (input_control == NPS_ODT)
         {
         speed = speed_direction(speed, velocity, posture.hpr[PF_H]);
         }
      else
         {
         speed_setting = cdata.thrust * 1.90; // MAX_GROUND_SPEED;
         speed += cdata.thrust;
         cdata.roll = cdata.pitch = cdata.thrust = 0.0f;
         }

      if (IS_ZERO(speed)) {
         speed = 0.0f;
         pfCopyVec3 ( velocity, ZeroVec ); 
      }

      if (((status & diApperance) ==
           diStill) ||
          ((status & diApperance) ==
           diWalking) ||
          ((status & diApperance) ==
           diRunning)) {

         // Don't want to exceed the max speed
         if (speed > MAX_PERSON_SPEED) {
            speed = MAX_PERSON_SPEED;
         }

         // Don't want to go backwards too fast
         if (speed < MAX_PERSON_REVERSE_SPEED) {
            speed = MAX_PERSON_REVERSE_SPEED;
          }

         // This should help stop movement -rb
         if (IS_ZERO(speed)) {
            status &= ~(diApperance);
            status |= diStill;
         }
         else if (((speed > 0.0f) && (speed < DI_JOG_THRESHOLD)) || 
                  (speed < 0.0f)) {
            status &= ~(diApperance);
            status |= diWalking;
         }
         else if (speed > DI_JOG_THRESHOLD) {
            status &= ~(diApperance);
            status |= diRunning;
         }
      }
      if (((status & diApperance) ==
                     diProne) && (!IS_ZERO(speed))) {

         // Don't want to exceed the max speed
         if (speed > MAX_PERSON_CRAWL_SPEED) {
            speed = MAX_PERSON_CRAWL_SPEED;
         }

         // Don't want to go backwards too fast
         if (speed < -MAX_PERSON_CRAWL_SPEED) {
            speed = -MAX_PERSON_CRAWL_SPEED;
          }
         
         status &= ~(diApperance);
         status |= diCrawling;
      }
      //move the GROUND vehicle type in the x-y plane
      posture.xyz[X] += velocity[X]*cdata.delta_time;
      posture.xyz[Y] += velocity[Y]*cdata.delta_time;
        
//      altitude = ::get_altitude(posture.xyz);

   altitude = 0.0f;
   // Now see if we are going to hit something in this time interval
   if (!G_static_data->stealth) {
      if ( check_collide(oldpos, posture.xyz,
                         posture.hpr,
                         speed, altitude) ) {
         cdata.thrust = 0.0f;
         pfSetVec3 ( velocity, 0.0f, 0.0f, 0.0f );
         return(HITSOMETHING);
      }
   }


      // Make sure we are still in bounds, if not, stop it at the boundary.
      if (at_a_boundary(posture.xyz))
         {
         posture.hpr[HEADING] =
              fmod(posture.hpr[HEADING] + 180.0f, 360.0f);
         pfNegateVec3(velocity,ZeroVec);
         }
      else  // Still in bounds
         {
         // Calculate the new velocity vector
         pfMakeEulerMat ( orient, posture.hpr[HEADING],
                                  posture.hpr[PITCH],
                                  posture.hpr[ROLL]);

         pfFullXformPt3 ( direction, direction, orient );
         pfNormalizeVec3 ( direction );
         pfScaleVec3 ( velocity, speed, direction);
         }
 

      } // End !paused !tethered !rigid_body

   // If tethered, let the other vehicle calculate our position for us
   if (tethered) {
      if ((cdata.vid > -1) && G_vehlist[cdata.vid].vehptr) {
         if ( tethered == 1) {
            G_vehlist[cdata.vid].vehptr->teleport(posture,speed);
         }
         else if ( tethered == 2) {
            G_vehlist[cdata.vid].vehptr->attach(posture,speed,
                                                G_dynamic_data->attach_dis);
         }
         else if (tethered == 3) {
            if ((tether_model_off != -1) &&
                (cdata.vid != tether_model_off)) {
               if (G_vehlist[tether_model_off].vehptr) {
                  G_vehlist[tether_model_off].vehptr->turnon();
               }
               tether_model_off = -1;
            }
            pfCopyVec3 (posture.xyz,
                        G_vehlist[cdata.vid].vehptr->getposition());
            pfCopyVec3 (posture.hpr,
                     G_vehlist[cdata.vid].vehptr->getorientation());
            G_vehlist[cdata.vid].vehptr->turnoff();
            tether_model_off = cdata.vid;
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
   }

/* // This segment of code is a hook for the implementing the
   //  rigid_body class for vehicle updates
   if (cdata.rigidbody)
      {
      if (cdata.reset)
         // make call to set rigid_body parameters to the pdata values.
      // do the normal updates to pdata
      }
*/

   pfMakeEulerMat ( orient, posture.hpr[HEADING],
                            posture.hpr[PITCH],
                            posture.hpr[ROLL]);

   pfFullXformPt3 ( up, up, orient );
   pfNormalizeVec3 ( up );
   pfScaleVec3 ( up, DI_EYE_OFFSET, up);
   pfAddVec3 ( eyepoint, posture.xyz, up );
   pfCopyVec3 ( firepoint, eyepoint );

   altitude = ::get_altitude(posture.xyz);
   if ( (!tethered) && (!G_static_data->stealth) && (altitude < -5.0) )
      {
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
   if ( ((input_control == NPS_UPPERBODY) || (input_control == NPS_UNIPORT) ||
         (input_control == NPS_ODT)) && video_switch) {
      static pfVec3 temp_gun_pos;
      pfAddVec3 ( temp_gun_pos, gun_pos.xyz, posture.xyz );
      check_targets ( temp_gun_pos, gun_pos.hpr, ZeroVec);
   }
   else {
      check_targets ( posture.xyz, posture.hpr, 
                      look.hpr );
   }

   //put the vehicle in the correct location in space
#ifndef NODIGUY
   if (DIGUY_VEH::init) {
      hml_char_set_action (handle, get_action (status, speed));
   }
#endif

//   cerr << "a posture.xyz[PF_Z]=" << posture.xyz[PF_Z] << endl;
#ifndef NODIGUY
   if (DIGUY_VEH::init) {
      if (DIGUY_VEH::posMech == NPSNET_INTERNAL) {
         posture.xyz[PF_Z] = zHit (handle,
                                   posture.xyz[PF_X],
                                   posture.xyz[PF_Y],
                                   posture.xyz[PF_Z]);
      }

      hml_char_set_desired_speed (handle, speed);
      hml_char_set_orientation (handle, posture.hpr[HEADING]);

      hml_char_set_gaze (handle, look.hpr[PF_H], look.hpr[PF_P]);
      hml_char_set_aim (handle, 
                     gun_pos.hpr[PF_H], 
                     gun_pos.hpr[PF_P]);

      float deltaTime = G_curtime - diLastTime;
      diLastTime = G_curtime;

      hml_char_set_desired_position (handle,
                                     &posture.xyz[PF_X],
                                     &posture.xyz[PF_Y],
                                     &posture.xyz[PF_Z]);

      if (DIGUY_VEH::posMech == NPSNET_INTERNAL) {
        hml_char_teleport (handle, NULL, NULL, &posture.xyz[PF_Z]);
      }
      hml_char_update (handle, deltaTime);
      if (DIGUY_VEH::posMech == DIGUY_INTERNAL) {
         hml_char_get_position (handle,
                                &posture.xyz[PF_X],
                                &posture.xyz[PF_Y],
                                &posture.xyz[PF_Z],
                                &posture.hpr[PF_H]);
      }
   }
#endif


   if (!birds_eye && !tethered) {
      posture.hpr[PF_P] = 0.0f;
      posture.hpr[PF_R] = 0.0f;

      look.xyz[PF_X] = 0.0f;
      look.xyz[PF_Y] = 0.0f;
      look.xyz[PF_Z] = 0.0f;
   }


   if(birds_eye) {
      pfSwitchVal (models, PFSWITCH_ON);
      if (di_switch) {
         pfSwitchVal (di_switch, PFSWITCH_ON);
      }
   }
   else {
      pfSwitchVal (models, PFSWITCH_OFF);
      if (di_switch) {
         pfSwitchVal(di_switch, PFSWITCH_OFF);
      }
   }

/*
   cerr << "Speed:    " << speed << endl
        << "Velocity: " << velocity[PF_X] << "  "
                        << velocity[PF_Y] << "  "
                        << velocity[PF_Z] << endl
        << "-=-=-=-=-=-=-=-=-" << endl;
*/

   pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z] + DI_BOX);
   pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);
      
   thrust = cdata.thrust;
   return(FINEWITHME);

}




movestat DIGUY_VEH::moveDR(float deltatime,float curtime)
//move routine for DR vehciles simple move
{
   float accelfac;
//   float altitude = 0.0f;
   static float oldspeed = 0.0f;

#ifdef TRACE
   cerr << "Dead Recon a Ground Vehicle\n";
#endif

#ifndef NO_PFSMOKE
   update_pfsmokes();
#endif
/*
if (vid == G_drivenveh) {
       cerr << "Driven" << endl;
    }
    else {
       cerr << "Not Driven" << endl;
    }

 if (curtime == lastPDU) {
    cerr << "No DR" << endl;
cerr << "-=-=-=-=-=-=-=-=-=-=-" << endl << endl;
    return(FINEWITHME);
 }
 else {
    cerr << "Doing DR" << endl;
cerr << "-=-=-=-=-=-=-=-=-=-=-" << endl << endl;
 }
*/

 if ( ((curtime - lastPDU) > G_static_data->DIS_deactivate_delta) &&
      (vid != G_drivenveh))
  {
    //Deactivate the puppy
//    cerr <<"It has been to long since the last update DIGUY_VEH\n";
    //not us, so we should deactiveate this vehcile
    return(DEACTIVE);
  }
/*
  if ( snap_switch == TRUE )
    {
    //cerr << "Veh# " << vid << " is close enough to snap..." << endl;
    altitude = ::get_altitude(drpos.xyz);
    }
*/

  switch(dralgo)
  {
    case DRAlgo_Static: //it is fixed, so it does not move
      break;
    case DRAlgo_DRM_FPW: //Fixed orientation,Constant Speed
      // Move in the X-Y plane
      drpos.xyz[X] += drparam.xyz[X]*deltatime;
      drpos.xyz[Y] += drparam.xyz[Y]*deltatime;
      // Now put it on the ground and set HPR
/*         if ( (altitude > -5.0f) && (altitude < 5.0f)  )
           {
           if ( snap_switch == TRUE )
              {
              //cerr << "A Snapping veh# " << vid << endl;
              grnd_orient(drpos);
              }

           }
        else
           {
*/
           drpos.xyz[Z] += drparam.xyz[Z] * deltatime;
//           }
      break;
    case DRAlgo_DRM_FVW: //Constant rotation,varable velocity
      accelfac = 0.5f*deltatime*deltatime;
      drpos.xyz[X] += velocity[X]*deltatime+accelfac*velocity[X];
      drpos.xyz[Y] += velocity[Y]*deltatime+accelfac*velocity[Y];
      drparam.xyz[X] += accelfac*velocity[X];
      drparam.xyz[Y] += accelfac*velocity[Y];
      drparam.xyz[Z] += accelfac*velocity[Z];
      // Now put it on the ground and set HPR
 /*     if ( (altitude > -5.0f) && (altitude < 5.0f) )
         {
         if ( snap_switch == TRUE )
            {
            //cerr << "B Snapping veh# " << vid << endl;
            grnd_orient(drparam);
            }
         }
      else */
         drpos.xyz[Z] += velocity[Z]*deltatime+accelfac*velocity[Z];
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
/*      if ( (altitude > -5.0f) && (altitude < 5.0f) )
         {
         if ( snap_switch == TRUE )
            {
            if (!grnd_orient(drpos))
               drpos.xyz[Z] = 3.5f;
            //cerr << "C Snapping veh# " << vid << endl;
            }
         }
      else
*/
         drpos.xyz[Z] += velocity[Z]*deltatime;

      break;
  }

  if (vid != G_drivenveh){
     speed = pfLengthVec3(velocity);
     speed = speed_direction(speed, velocity, posture.hpr[PF_H]);
     // This should help stop movement -rb
     if ((speed < 0.001) && (speed > 0.0f)) {
        speed = 0.0;
        velocity[PF_X] = 0.0f;
        velocity[PF_Y] = 0.0f;
     }
     if ( ((G_curtime - hiResTimeStamp) > HI_RES_TIMEOUT) &&
        (vid != G_drivenveh)) {
        posture = drpos;
     }
     //put the vehicle in the correct location in space
/*
cerr << "posture: " << posture.xyz[X] << "  "
                    << posture.xyz[Y] << "  "
                    << posture.hpr[HEADING] << endl
     << "speed:   " << speed << endl;
int val = pfGetSwitchVal (models);
if (val == PFSWITCH_ON) {
   cerr << "PFSWITCH_ON" << endl;
}
else if (val == PFSWITCH_OFF) {
   cerr << "PFSWITCH_OFF" << endl;
}
else {
   cerr << "SwitchValue = " << val << endl;
}
*/
#ifndef NODIGUY
   if (DIGUY_VEH::init) {
      hml_char_set_desired_position (handle,
                                  &posture.xyz[PF_X],
                                  &posture.xyz[PF_Y],
                                  &posture.xyz[PF_Z]);
      if (snap_switch) {
         if (((G_curtime - hiResTimeStamp) > HI_RES_TIMEOUT) &&
             (vid != G_drivenveh)) {
            if (DIGUY_VEH::posMech == NPSNET_INTERNAL) {
               posture.xyz[PF_Z] = zHit (handle,
                                         posture.xyz[PF_X], 
                                         posture.xyz[PF_Y],
                                         posture.xyz[PF_Z]);
            }
         }
      }

      hml_char_set_action (handle, get_action (status, speed));
      hml_char_set_desired_speed (handle, speed);
      hml_char_set_orientation (handle, posture.hpr[HEADING]);

      hml_char_set_gaze (handle, look.hpr[PF_H], look.hpr[PF_P]);
      hml_char_set_aim (handle,
                        gun_pos.hpr[PF_H],
                        gun_pos.hpr[PF_P]);

      if (DIGUY_VEH::posMech == NPSNET_INTERNAL) {
         hml_char_teleport (handle, NULL, NULL, &posture.xyz[PF_Z]);
      }

      hml_char_update (handle, deltatime);

      if (DIGUY_VEH::posMech == DIGUY_INTERNAL) {
         hml_char_get_position (handle,
                                &posture.xyz[PF_X],
                                &posture.xyz[PF_Y],
                                &posture.xyz[PF_Z],
                                &posture.hpr[PF_H]);
      }

/*
bdiAction current_action;
bdiAction desired_action;
float time_to_transition;

hml_char_get_action(handle,
        &current_action,
        &desired_action,
        &time_to_transition);

int test_lod = hml_char_get_motion_lod(handle);

cerr << "-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
cerr << "current_action: ";
print_action (current_action);
cerr << "desired_action";
print_action (desired_action);
cerr << "time_to_transition: " << time_to_transition << endl;
cerr << "LOD: " << test_lod << endl;
cerr << "-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
*/
   }
#endif

   pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y],posture.xyz[Z] + DI_BOX);
   pfDCSRot(hull, posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);
  }
  else if ( !G_static_data->stealth )
  {
//    speed = pfLengthVec3(velocity);
    speed = speed_direction(speed, velocity, posture.hpr[PF_H]);
    // This is us, have we exceeded the DR error or time limit?
    if ((G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) ||
        (delta_send(&posture,&drpos)))
    {

#ifdef DEBUG
       cerr <<"X       " << posture.xyz[HEADING] << " " <<drpos.xyz[HEADING] <<
"\n";
       cerr <<"Y       " << posture.xyz[PITCH] << " " <<drpos.xyz[PITCH] << "\n";
       cerr <<"Z       " << posture.xyz[ROLL] << " " <<drpos.xyz[ROLL] << "\n";
       cerr <<"HEADING " << posture.hpr[HEADING] << " " <<drpos.hpr[HEADING] <<
"\n";
       cerr <<"PITCH   " << posture.hpr[PITCH] << " " <<drpos.hpr[PITCH] << "\n";
       cerr <<"ROLL    " << posture.hpr[ROLL] << " " <<drpos.hpr[ROLL] << "\n";
       cerr << "send a delta pdu\n";
#endif
       // Sufficient difference in rotation,location, or time to require a PDU
       sendentitystate();
       oldspeed = speed;
    }
    else if (oldstatus != status) {
       oldstatus = status;
       sendentitystate();
    }
    else if (speed != oldspeed) {
       if ( (!IS_ZERO(oldspeed)) && (IS_ZERO(speed)) ) {
          sendentitystate();
          oldspeed = speed;
       }
       else if ( (IS_ZERO(oldspeed)) && (!IS_ZERO(speed)) ) {
          sendentitystate();
          oldspeed = speed;
       }
       else if ( fabs(oldspeed-speed) > 0.1f ) {
          sendentitystate();
          oldspeed = speed;
       }
    }
  }
      return(FINEWITHME);
}

void DIGUY_VEH::sendentitystate()
//send a entity state PDU with articulated parts
{
  char tempstr[MARKINGS_LEN];
  EntityStatePDU epdu;

  //fill in the parameters of an entity state PDU
  //this assumes there are no articulated parameters for now

/*
  if ( G_dynamic_data->exercise < 0 )
     epdu->entity_state_header.exercise_ident = (unsigned char)1;
  else
     epdu->entity_state_header.exercise_ident =
        (unsigned char)G_dynamic_data->exercise;
*/

  bzero ( (char *)&epdu, sizeof(EntityStatePDU) );

  //ID and Type
  epdu.entity_id = disname = G_vehlist[vid].DISid;
//  epdu.entity_id.entity = disname.entity = vid + 1001;
  epdu.alt_entity_type = epdu.entity_type = type;
  //How fast is it going
  epdu.entity_velocity.x = velocity[X];
  epdu.entity_velocity.y = velocity[Y];
  epdu.entity_velocity.z = velocity[Z];
  //Where we are
  epdu.entity_location.x = posture.xyz[X];
  epdu.entity_location.y = posture.xyz[Y];
  epdu.entity_location.z = posture.xyz[Z];
  //How we are facing
  epdu.entity_orientation.psi   = -(posture.hpr[HEADING]*DEG_TO_RAD);
  epdu.entity_orientation.theta =  posture.hpr[PITCH]*DEG_TO_RAD;
  epdu.entity_orientation.phi   =  posture.hpr[ROLL]*DEG_TO_RAD;
  //Send out dr accelerations and velocities
  epdu.dead_reckon_params.linear_accel[X] = acceleration[X];
  epdu.dead_reckon_params.linear_accel[Y] = acceleration[Y];
  epdu.dead_reckon_params.linear_accel[Z] = acceleration[Z];
  // We don't calculate these at the moment but for the future
  //epdu.dead_reckon_params.angular_velocity[HEADING] = 0.0f;
  //epdu.dead_reckon_params.angular_velocity[PITCH] = 0.0f;
  //epdu.dead_reckon_params.angular_velocity[ROLL] = 0.0f;
  epdu.force_id = G_dynamic_data->force;
  //what we look like
  epdu.entity_appearance = status;
  //project our movement
  epdu.dead_reckon_params.algorithm = DRAlgo_DRM_FPW;

  epdu.entity_marking.character_set = CharSet_ASCII;

  if ( *(G_static_data->marking) )
     {
     strcpy ( (char *)epdu.entity_marking.markings,
              (char *)G_static_data->marking );
     }
  else
     {
     strncpy ( tempstr, (char *)G_static_data->hostname, MARKINGS_LEN-4 );
     strcpy ( (char *)epdu.entity_marking.markings, "NPS" );
     strcat ( (char *)epdu.entity_marking.markings, tempstr );
     }

#ifdef DEBUG
cerr << "DIS Heading (psi) = " << epdu.entity_orientation.psi
     << "\n    Pitch (theta) = " << epdu.entity_orientation.theta
     << "\n     Roll (phi)   = " << epdu.entity_orientation.phi << endl;
#endif


  // Send articulated part info

  epdu.num_articulat_params = (unsigned char)0;

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          G_net->write_pdu((char *)&epdu,EntityStatePDU_Type);
          break;
       }

  //update the DR parameters
  drpos = posture;
  pfCopyVec3(drparam.xyz,velocity); //speed
  pfCopyVec3(drparam.hpr,acceleration); //Acceleration

  lastPDU = G_curtime;

}

void DIGUY_VEH::entitystateupdate(EntityStatePDU *epdu, sender_info &sender)
//get an entity state PDU with articulated parts
{
/*
   ArticulatParams      *APNptr;
   float temp_float;
   int count;
*/

   //what and who I am
   disname = epdu->entity_id;
   type = epdu->entity_type;
   force = epdu->force_id;
   //How fast is it going
   velocity[X] = epdu->entity_velocity.x;
   velocity[Y] = epdu->entity_velocity.y;
   velocity[Z] = epdu->entity_velocity.z;
   //Where we are
  
   if ( ((G_curtime - hiResTimeStamp) > HI_RES_TIMEOUT) &&
        (vid != G_drivenveh)) { 
      posture.xyz[X] = (float)epdu->entity_location.x;
      posture.xyz[Y] = (float)epdu->entity_location.y;
      posture.xyz[Z] = (float)epdu->entity_location.z;

      //How we are facing
      posture.hpr[HEADING] =  (epdu->entity_orientation.psi < 0)?  
                              -(epdu->entity_orientation.psi + 2*M_PI)*
                                RAD_TO_DEG:
                              -(epdu->entity_orientation.psi)*RAD_TO_DEG;
      posture.hpr[PITCH]   =  epdu->entity_orientation.theta*RAD_TO_DEG;
      posture.hpr[ROLL]    =  epdu->entity_orientation.phi*RAD_TO_DEG;
   }
   //Acceleration vector values
   acceleration[X] = epdu->dead_reckon_params.linear_accel[X];
   acceleration[Y] = epdu->dead_reckon_params.linear_accel[Y];
   acceleration[Z] = epdu->dead_reckon_params.linear_accel[Z];

   pfCopyVec3(drparam.xyz,velocity);
   pfCopyVec3(drparam.hpr,acceleration);
   drpos=posture;

   update_articulations ( epdu->articulat_params, epdu->num_articulat_params );

/* Old way of updating articulated parts from networked entity

   APNptr = epdu->articulat_params;
   count = 0;
   while ( count < epdu->num_articulat_params )
      {
      if ( (APNptr->ID == (unsigned short)0x0) &&
           (APNptr->parameter_type == (unsigned int)4107) )
         {
         bcopy ( &(APNptr->parameter_value),
                 &temp_float, 4);
         turret1_angle = (360.0f - (temp_float*RAD_TO_DEG));
         while ( turret1_angle < 0.0f )
            turret1_angle += 360.0f;
         turret1_angle = fmod ( turret1_angle, 360.0f );
         }
      else if ( (APNptr->ID == (unsigned short)0x0) &&
           (APNptr->parameter_type == (unsigned int)4108) )
         {
         bcopy ( &(APNptr->parameter_value),
                 &temp_float, 4);
         turret1_rate = float(-temp_float * RAD_TO_DEG);
         }
      else if ( (APNptr->ID == (unsigned short)0x1) &&
           (APNptr->parameter_type == (unsigned int)4429) )
         {
         bcopy ( &(APNptr->parameter_value),
                 &temp_float, 4);
         }

         }
      count++;
      APNptr++;
      }
*/



   //what we look like
   status = epdu->entity_appearance;
//   changestatus(status);

  bcopy ( (char *)&epdu->entity_marking,
          (char *)&marking, sizeof(EntityMarking) );

  if ( (strcmp(controller.internet_address,sender.internet_address)) != 0 )
     {
     struct hostent *entity;
     unsigned long netaddress = inet_addr(sender.internet_address);
     strcpy ( controller.internet_address, sender.internet_address );
     entity = gethostbyaddr ( (void *)&netaddress,
                              sizeof(struct in_addr), AF_INET );
     if ( entity != NULL )
        strcpy ( controller.host_name, entity->h_name );
     else
        strcpy ( controller.host_name, "Unknown" );

     }

   //Update the time of last message
   lastPDU = G_curtime;

}



movestat DIGUY_VEH::movedead()
{
  float gnd_elev = gnd_level(posture.xyz);
  static float dead_time;
  static float grnd_slam_time;
  static int hit;
  static int my_status;
  static int cause;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tDIGUY_VEH::movedead" << endl;
#endif

   update_time ();
   collect_inputs();

#ifdef DATALOCK
  pfDPoolLock ( (void *) G_dynamic_data );
#endif
  my_status = G_dynamic_data->dead;
  cause = G_dynamic_data->cause_of_death;

#ifdef DATALOCK
  pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

  if ( my_status == DYING )
     {
     if ( G_static_data->sound )
        {
        LocationRec dummy_snd_pos;
        dummy_snd_pos.x = 0.0;
        dummy_snd_pos.y = 0.0;
        dummy_snd_pos.z = 0.0;
        if ( cause == MUNITION_DEATH )
           playsound(&dummy_snd_pos, &dummy_snd_pos, HIT_INDEX,-1);
        else if ( cause == FALLING_DEATH )
           playsound(&dummy_snd_pos, &dummy_snd_pos, FALLING_INDEX,-1);
        else
           playsound(&dummy_snd_pos, &dummy_snd_pos, DYING_INDEX,-1);
        }

     if ( (::get_altitude(posture.xyz)) > 5.0f )
        {
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = AIRDEAD;
        my_status = AIRDEAD;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        grnd_slam_time = G_curtime;
        hit = FALSE;
        }
     else
        {
        grnd_slam_time = G_curtime;
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = GRNDDEAD;
        my_status = GRNDDEAD;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        status = Appearance_DamageDestroyed;
//        changestatus(status);
        pfCopyVec3(velocity,ZeroVec);
        speed = 0.0f;
        sendentitystate();
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        cause = G_dynamic_data->cause_of_death;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        if ( cause != MUNITION_DEATH )
           senddetonation(posture.xyz);
        if ( cause == GROUND_DEATH )
           makecrater_pos(posture.xyz);
//        use_next_fire_plume(posture.xyz[X], posture.xyz[Y],
//                            posture.xyz[Z], 20.0f );
        makefireball(posture.xyz);
        }
     }

  if ( my_status == AIRDEAD )
     {
     if (!hit)
        {
        if ( posture.hpr[PITCH] > -90.0f )
           posture.hpr[PITCH] -= 10.0f;
        if ( posture.hpr[PITCH] < -90.0f )
           posture.hpr[PITCH] = 90.0f;
        posture.hpr[ROLL] = 0.0f;
        posture.xyz[X] += velocity[X]*cdata.delta_time;
        posture.xyz[Y] += velocity[Y]*cdata.delta_time;
        posture.xyz[Z] += velocity[Z]*cdata.delta_time;
        velocity[Z] -= 3.0f;
// As we die here, we should be checking for collisions with other
// objects
        }
     else
        {
        posture.hpr[PITCH] = 90.0f;
        posture.hpr[ROLL] = 0.0f;
        }
     if (((::get_altitude(posture.xyz)) <= 0.0f )&& !hit)
        {
        posture.xyz[Z] = gnd_elev;
        speed = 0.0f;
        status = Appearance_DamageDestroyed;
//        changestatus(status);
        pfCopyVec3(velocity,ZeroVec);
        sendentitystate();
        senddetonation(posture.xyz);
        makefireball(posture.xyz);
// Can only leave a crater if we know we hit the ground (i.e. could
//   have hit water, bridge, etc.
//        makecrater_pos(posture.xyz);
//        use_next_fire_plume(posture.xyz[X], posture.xyz[Y],
//                            posture.xyz[Z], 20.0f );
        hit = TRUE;
        }
     if (hit && grnd_slam_time + GROUND_SLAM_DELAY < G_curtime)
        {
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = DEAD;
        my_status = DEAD;
        if ( G_static_data->sound )
           {
           LocationRec dummy_snd_pos;
           dummy_snd_pos.x = 0.0;
           dummy_snd_pos.y = 0.0;
           dummy_snd_pos.z = 0.0;
           playsound(&dummy_snd_pos,&dummy_snd_pos,DEAD_INDEX,-1);
           }
        G_dynamic_data->cause_of_death = 0;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        dead_time = G_curtime;
        }
     }
  else if ( my_status == GRNDDEAD )
     {
     if ( grnd_slam_time + GROUND_SLAM_DELAY < G_curtime )
        {
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = DEAD;
        my_status = DEAD;
        dead_time = G_curtime;
        G_dynamic_data->cause_of_death = 0;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        }
     }
  else if (!G_dynamic_data->dead_switch)
     {
     if (dead_time + DEAD_TIME_DELAY < G_curtime)
        {
        bzero ( (void *)&(look), sizeof(pfCoord) );
        pfCopyVec3 ( posture.hpr,
                     (float *)G_static_data->init_location.hpr );
        G_vehlist[vid].vehptr->
              transport(G_static_data->init_location.xyz[PF_X],
                        G_static_data->init_location.xyz[PF_Y],
                        G_static_data->init_location.xyz[PF_Z] );

#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = ALIVE;
        my_status = ALIVE;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        status = diStill;// ALIVE;
//        changestatus(status);
        }
     }

//  altitude = ::get_altitude(posture.xyz);
  float deltaTime = G_curtime - diLastTime;

#ifndef NODIGUY
  if (DIGUY_VEH::init) {
     hml_char_set_desired_position (handle,
                                 &posture.xyz[PF_X],
                                 &posture.xyz[PF_Y],
                                 &posture.xyz[PF_Z]);

     if (DIGUY_VEH::posMech == NPSNET_INTERNAL) {
        hml_char_teleport (handle, NULL, NULL, &posture.xyz[PF_Z]);
     }
     hml_char_teleport (handle, NULL, NULL, &posture.xyz[PF_Z]);
     hml_char_set_action (handle, get_action (status, speed));
     hml_char_set_desired_speed (handle, speed);
     hml_char_set_orientation (handle, posture.hpr[HEADING]);

     hml_char_set_gaze (handle, look.hpr[PF_H], look.hpr[PF_P]);

     hml_char_set_aim (handle,
                    gun_pos.hpr[PF_H],
                    gun_pos.hpr[PF_P]);

     hml_char_update (handle, deltaTime);

     if (DIGUY_VEH::posMech == DIGUY_INTERNAL) {
        hml_char_get_position (handle,
                                    &posture.xyz[PF_X],
                                    &posture.xyz[PF_Y],
                                    &posture.xyz[PF_Z],
                                    &posture.hpr[PF_H]);
     }
  }
#endif

  pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z] + DI_BOX);
  pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);

/*
  pfSwitchVal (models, PFSWITCH_ON);
  pfSwitchVal (di_switch, PFSWITCH_ON);
*/
  return FINEWITHME;
}

int DIGUY_VEH::check_collide(pfVec3 &startpnt, pfVec3 &endpnt,
                              pfVec3 &orientation,
                              float &speed, float &altitude)
{
/*
cerr << "startpnt" << startpnt[PF_X] << "  "
                   << startpnt[PF_Y] << "  "
                   << startpnt[PF_Z] << endl;

cerr << "endpnt" << endpnt[PF_X] << "  "
                 << endpnt[PF_Y] << "  "
                 << endpnt[PF_Z] << endl;

cerr << "orientation" << orientation[PF_X] << "  "
                   << orientation[PF_Y] << "  "
                   << orientation[PF_Z] << endl;

cerr << "speed: " << speed << endl;
cerr << "altitude: " << altitude << endl;
*/

  
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
//   pfCylinder the_barrel;
   long the_mask;
   pfVec3 incoming_vec;
   pfVec3 outgoing_vec;
   pfVec3 normal_vec;
   pfVec3 eye_vec, foot_vec;
   pfVec3 start_eye, end_eye;
   pfVec3 start_pos, end_pos;
   float temp_float;
   pfMatrix Orient;                      // Local entity orientation transform
   pfVec3 tb_vec = { 0.0f, 0.1f, 1.0f };
   pfVec3 fb_vec = { 0.0f, 0.1f, 0.0f };
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
   pfScaleVec3 ( eye_vec, DI_EYE_OFFSET, tb_vec );
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

//This cause it to crash. WHY?
//pfCylAroundSegs ( &the_barrel, (const pfSeg **)sp, 2 );

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
         pfFullXformPt3(posture.xyz, pnt, xform);

#ifdef DEBUG
         cout << "\n\nBuilding hit at " << posture.xyz[X] << " "
                                        << posture.xyz[Y] << " "
                                        << posture.xyz[Z] << endl;
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
            pfScaleVec3 ( normal, DI_EYE_OFFSET + 5.0f, normal );
         else
            pfScaleVec3 ( normal, INTERSECT_OFFSET + 5.0f, normal );

         pfAddVec3 (endpnt, pnt, normal);
         altitude = endpnt[Z] - gnd_level(endpnt);;
         temp_float= 0.75f * pfLengthVec3(velocity);
         speed *= 0.75f;
         pfScaleVec3 ( velocity, temp_float, direction);
         }

      }     

      //if we got here it did not hit anything
      return FINEWITHME;

   }

void
DIGUY_VEH::process_keyboard_speed_settings ()
{
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;

   l_input_manager->query_button(counter,NPS_KEYBOARD,'s');
   if ( counter > 0 ) {
      set_thrust ( 0.0f );
      pfSetVec3 ( velocity, 0.0f, 0.0f, 0.0f );
      pfSetVec3 ( acceleration, 0.0f, 0.0f, 0.0f );
      speed = 0.0f;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'a');
   if ( counter > 0 ) {
      change_thrust ( float(counter)*KEY_PERSON_SPEED_CHANGE );
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'d');
   if ( counter > 0 ) {
      change_thrust ( float(-counter)*KEY_PERSON_SPEED_CHANGE );
   }
}

void
DIGUY_VEH::class_specific_keyboard()
{
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;
   int control_pressed = FALSE;
   int shift_pressed = FALSE;

   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_CONTROLKEY);
   control_pressed = (counter > 0);
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_SHIFTKEY);
   shift_pressed = (counter > 0);

   l_input_manager->query_button(counter,NPS_KEYBOARD,'7');
   if ( counter > 0 ) {
//      changestatus(Appearance_DamageDestroyed);
      status &= ~(Appearance_Damage_Mask);
      status |= Appearance_DamageDestroyed;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'5');
   if ( counter > 0 ) {
//      toggle_weapon_status();
      if ((status & weaponAppearance) ==
                        weaponDeployed) {
         status &= ~(weaponAppearance);
         status |= weaponStowed;
      }
      else {
         status &= ~(weaponAppearance);
         status |= weaponDeployed;
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'8');
   if ( counter > 0 ) {
//      go_kneeling();
      status &= ~(diApperance);
      status |= diKneeling;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'9');
   if ( !control_pressed && (counter > 0) ) {
//      go_prone();
      status &= ~(diApperance);
      status |= diProne;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'e');
   if ( counter > 0 ) {
      status &= ~(diApperance);
      status |= diStill;
      status &= ~(Appearance_Damage_Mask);
//      go_standing();
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'Z');
   if ( counter > 0 ) {
      lase ();
   }
}

void
DIGUY_VEH::class_specific_upperbody ()
{
   pfMatrix orient;

   input_manager *l_input_manager = G_manager->ofinput;
   l_input_manager->query_multival ( (void *) &gun_pos,
                                    NPS_UPPERBODY,
                                    NPS_MV_TARGET_RIFLE);
   //gun_pos.hpr[PF_P] -= 30.0f; 
   //gun_pos.hpr[PF_H] += posture.hpr[PF_H];
   pfSetVec3 ( gun_pos.xyz, 0.0f, 0.5f, 0.0f );
   pfMakeEulerMat ( orient, posture.hpr[PF_H],
                            posture.hpr[PF_P],
                            posture.hpr[PF_R]);
   pfFullXformPt3 ( gun_pos.xyz, gun_pos.xyz, orient );
   pfNormalizeVec3 ( gun_pos.xyz );
   gun_pos.xyz[PF_Z] = DI_RIFLE_Z_OFFSET;
   gun_pos.hpr[PF_R] = 0.0f;

 
}


void
DIGUY_VEH::fire_weapon ( const wdestype weapon_num, const int /* num_times */ )
{
   input_manager *l_input_manager = G_manager->ofinput;
   if ( (input_control == NPS_UPPERBODY) || (input_control == NPS_UNIPORT) ||
        (input_control == NPS_ODT) ) {
      if (l_input_manager->verify_device (NPS_UPPERBODY) ) {
         pfCoord gun_look;
         //l_input_manager->query_multival ( (void *) &gun_look,
         //                                 NPS_UPPERBODY,
         //                                 NPS_MV_TARGET_RIFLE);

         pfCopyVec3 ( gun_look.xyz, gun_pos.xyz );
/*
cerr << "In DIGUY: Aim_direction is " 
     << gun_pos.hpr[PF_H] << "H, "
     << gun_pos.hpr[PF_P] << "P, "
     << gun_pos.hpr[PF_R] << "R." << endl;
*/
         gun_look.hpr[PF_H] = gun_pos.hpr[PF_H] - posture.hpr[PF_H];
         gun_look.hpr[PF_P] = gun_pos.hpr[PF_P] - posture.hpr[PF_P];
         gun_look.hpr[PF_R] = gun_pos.hpr[PF_R];

         firebuttonhit (weapon_num, vid, posture, gun_look);
      }
      else {
         firebuttonhit ( weapon_num, vid, posture, look );
      }
   }
   else {
      firebuttonhit ( weapon_num, vid, posture, look );
   }
#ifndef NODIGUY
   if (DIGUY_VEH::init) {
      if (weapon_num == SECONDARY) {
         hml_char_fire_weapon (handle, BDI_MUZZLE_RIFLE);
      }
      else {
         hml_char_fire_weapon (handle, BDI_MUZZLE_GRENADE_LAUNCHER);
      }
   }
#endif
   status &= ~(weaponAppearance);
   status |= weaponDeployed;
}



int
DIGUY_VEH::transport ( float new_x, float new_y, float new_z )
{
   pfVec3 temp_pos;

   if ( (G_dynamic_data->bounding_box.xmin <= new_x) &&
        (new_x <= G_dynamic_data->bounding_box.xmax) &&
        (G_dynamic_data->bounding_box.ymin <= new_y) &&
        (new_y <= G_dynamic_data->bounding_box.ymax) ) {
      pfCopyVec3 ( temp_pos, posture.xyz );
      temp_pos[X] = new_x;
      temp_pos[Y] = new_y;
      temp_pos[Z] = new_z;
      // leave temp_pos.xyz[Z] from old posture as good guess
      temp_pos[Z] += gnd_level2 (temp_pos);
#ifndef NODIGUY
      temp_pos[Z] = zHit (handle, temp_pos[X], temp_pos[Y], temp_pos[Z]);
      hml_char_teleport (handle, &temp_pos[X], &temp_pos[Y], &temp_pos[Z]);
#endif
      pfCopyVec3 ( posture.xyz, temp_pos );
      return TRUE;
   }
   return FALSE;
}

void DIGUY_VEH::init_posture(pfCoord &pos)
{
  pfCopyVec3 ( posture.xyz, pos.xyz );
  pfCopyVec3 ( posture.hpr, pos.hpr );
  pfCopyVec3 ( eyepoint, pos.xyz );
  pfCopyVec3 ( firepoint, pos.xyz );

//  hml_char_set_action (di_name, get_action (status, speed), 
//                    speed, posture.xyz[X], posture.xyz[Y],
//                    posture.hpr[HEADING]);

#ifndef NO_PFSMOKE
  pfCopyVec3(old_posture.xyz,pos.xyz); pfCopyVec3(old_posture.hpr,pos.hpr);
  pfCopyVec3(older_posture.xyz,pos.xyz); pfCopyVec3(older_posture.hpr,pos.hpr);
#endif

}

int DIGUY_VEH::birds_eye_view(int value)
{
   birds_eye = !value;
   if (birds_eye && !displaying_model) {
      pfSwitchVal(models,PFSWITCH_ON);
      if (di_switch) {
         pfSwitchVal(di_switch, PFSWITCH_ON);
      }
   }
   else if (!birds_eye && !displaying_model) {
      pfSwitchVal(models,PFSWITCH_OFF);
      if (di_switch) {
         pfSwitchVal(di_switch, PFSWITCH_OFF);
      }
   }
   else {
   }
#ifndef NO_PFSMOKE
//   changestatus(status);
#endif
   return birds_eye;
}


void
DIGUY_VEH::get_weapon_view (pfCoord &point)
{
   if ( ((input_control == NPS_UPPERBODY) || (input_control == NPS_UNIPORT) ||
         (input_control == NPS_ODT)) && video_switch) {
      pfAddVec3 (point.xyz, posture.xyz, gun_pos.xyz);
      pfCopyVec3 (point.hpr, gun_pos.hpr);
   }
   else {
      VEHICLE::get_weapon_view(point);
   }

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

void 
DIGUY_VEH::get_weapon_look (pfCoord &point)
{
   pfCopyVec3 (point.xyz, gun_pos.xyz);
   pfCopyVec3 (point.hpr, gun_pos.hpr);
}

int
DIGUY_VEH::get_target_lock ()
{
   return target_lock;
}

void 
DIGUY_VEH::set_hr_mimic_info (fixedDatumRecord *data, int num)
{
hiResTimeStamp = G_curtime;

   for (int ix = 0; ix < num; ix++) {
      float value = data[ix].datum;
      switch (data[ix].datum_id) {
         case EPX_TAG:
            posture.xyz[PF_X] = value;
            break;
         case EPY_TAG:
            posture.xyz[PF_Y] = value;
            break;
         case EPZ_TAG:
            posture.xyz[PF_Z] = value;
            break;
         case EPH_TAG:
            posture.hpr[PF_H] = value;
            break;
         case EPP_TAG:
            posture.hpr[PF_P] = 0.0f; //value;
            break;
         case EPR_TAG:
            posture.hpr[PF_R] = 0.0f; //value;
            break;
         case RVX_TAG:
            gun_pos.xyz[PF_X] = value;
            break;
         case RVY_TAG:
            gun_pos.xyz[PF_Y] = value;
            break;
         case RVZ_TAG:
            gun_pos.xyz[PF_Z] = value;
            break;
         case RVH_TAG:
            gun_pos.hpr[PF_H] = value;
            break;
         case RVP_TAG:
            gun_pos.hpr[PF_P] = value;
            break;
         case RXR_TAG:
            gun_pos.hpr[PF_R] = value;
            break;
         case T_NONE:
            target_lock = NO_LOCK;
            break;
         case T_BLUE:
            target_lock = BLUE_LOCK;
            break;
         case T_RED:
            target_lock = RED_LOCK;
            break;
         case T_WHITE:
            target_lock = WHITE_LOCK;
            break;
         case T_OTHER:
            target_lock = OTHER_LOCK;
            break;
         default:
            cerr << "Error: Unknown HiRes Tag: " << data[ix].datum_id << endl;
            break;
      }
   }
}

void DIGUY_VEH::turnoff()
{
   pfSwitchVal(models,PFSWITCH_OFF);
   if (di_switch) {
      pfSwitchVal(di_switch, PFSWITCH_OFF);
   }
   displaying_model = FALSE;
}


void DIGUY_VEH::turnon()
{
   pfSwitchVal(models,PFSWITCH_ON);
   if (di_switch) {
      pfSwitchVal(di_switch, PFSWITCH_ON);
   }
   displaying_model = TRUE;
}


int
DIGUY_VEH::lase ()
{
   input_manager *l_input_manager = G_manager->ofinput;
   pfVec3 point;
   pfCoord pos;

   get_weapon_view(pos);
cerr << "Lazing from point ("
     << pos.xyz[PF_X] << ", "
     << pos.xyz[PF_Y] << ", "
     << pos.xyz[PF_Z] << ") in the direction of ("
     << pos.hpr[PF_H] << ", "
     << pos.hpr[PF_P] << ", "
     << pos.hpr[PF_R] << ")." << endl;

   long isect = lase_isect (pos, point);
   float length = 0.0f;

   if (isect) {
cerr << "Aim point ("
     << pos.xyz[PF_X] << ", "
     << pos.xyz[PF_Y] << ", "
     << pos.xyz[PF_Z] << ") in the direction of ("
     << pos.hpr[PF_H] << ", "
     << pos.hpr[PF_P] << ", "
     << pos.hpr[PF_R] << ")." << endl;
cerr << "Hit point ("
     << point[PF_X] << ", "
     << point[PF_Y] << ", "
     << point[PF_Z] << ")." << endl;
      pfSubVec3 (point, point, pos.xyz);
      length = PFLENGTH_VEC3 (point);
   }
   else {
      length = -1.0;
   }

cerr << "Lase Info: " << PF_DEG2RAD (pos.hpr[PF_P]) << "  "
                      << PF_DEG2RAD (pos.hpr[PF_H]) << "  "
                      << length << endl;

   if (l_input_manager->verify_device (NPS_LWS)) {
      float data[3];
      data[0] = PF_DEG2RAD (pos.hpr[PF_P]);
      data[1] = PF_DEG2RAD (pos.hpr[PF_H]);
      data[2] = length;
cerr << "Lase Info: " << data[0] << "  " 
                      << data[1] << "  " 
                      << data[2] << endl;

      l_input_manager->set_multival ((void *) data, NPS_LWS, NPS_LW_LAZE);
   }
   
   return FALSE;
}

void
DIGUY_VEH::change_gaze_vertical ( const float amount )
{
   look.hpr[PITCH] += amount;
   gun_pos.hpr[PITCH] += amount;
   gun_pos.xyz[PF_Z] = 1.5f;
   G_dynamic_data->camera = TRUE;
   G_dynamic_data->camlock = FALSE;
}

void
DIGUY_VEH::change_gaze_horizontal ( const float amount )
{
   look.hpr[HEADING] += amount;
   gun_pos.hpr[HEADING] += amount;
   gun_pos.xyz[PF_Z] = 1.5f;
   G_dynamic_data->camera = TRUE;
   G_dynamic_data->camlock = FALSE;

}


int
DIGUY_VEH::init_diguy (InitManager *initman)
{
   union query_data local;

   initman->query(BDIHML, &local, GET);
   G_static_data->bdihml = local.ints[0];

   if (G_static_data->bdihml) {
#ifndef NODIGUY
      cerr << "     Initializing BDIHML..." << endl;

      DIGUY_VEH::init = diguy_initialize (NULL);
      if (DIGUY_VEH::init < 0) {
         DIGUY_VEH::init = FALSE;
         cerr << endl << "\tDI-Guy is not installed. " 
              << "For information on depicting realistic " << endl
              << "\tdismounted infantry with DI-Guy, contact "
              << "Boston Dynamics Inc, " << endl
              << "\t617/621-2929, di-guy@bdi.com" << endl << endl;
      }
      else if (DIGUY_VEH::init >= 0) {
         DIGUY_VEH::init = TRUE;
      }
   
      if (DIGUY_VEH::init) {
         lookUp = new diguyTypeLookup ();

         initman->query(DIGUY_SPEED_RATIO, &local, GET);
         cerr << "\thml_char_set_max_speed_ratio: " << local.floats[0] << endl;
         hml_char_set_max_speed_ratio (BDI_ALL_CHARACTERS_HANDLE,
                                       local.floats[0]);
         // 5.0f

         initman->query(DIGUY_POSITION_ERROR, &local, GET);
         cerr << "\thml_char_set_max_position_error: " << local.floats[0] 
              << endl;
         hml_char_set_max_position_error (BDI_ALL_CHARACTERS_HANDLE, 
                                          local.floats[0]);
         // 0.0f

         initman->query(DIGUY_TIME_CONSTANT, &local, GET);
         cerr << "\thml_char_set_position_time_constant: " << local.floats[0] 
              << endl;
         hml_char_set_position_time_constant (BDI_ALL_CHARACTERS_HANDLE,
                                              local.floats[0]);
         // 2.0

         initman->query(DIGUY_TRANS_SMOOTHNESS, &local, GET);
         cerr << "\thml_char_set_transition_smoothness: ";
         bdiTransitionSmoothness diTrans = BDI_TRANSITION_SMOOTHEST;
         switch (local.ints[0]) {
            case (TRANS_IMMEDIATE):
               cerr << "BDI_TRANSITION_IMMEDIATE" << endl;
               diTrans = BDI_TRANSITION_IMMEDIATE;
               break;
            case (IMMEDIATE_TRANS):
               cerr << "BDI_TRANSITION_IMMEDIATE_TRANSITION" << endl;
               diTrans = BDI_TRANSITION_IMMEDIATE_TRANSITION;
               break;
            case (TRANS_SMOOTHEST):
               cerr << "BDI_TRANSITION_SMOOTHEST" << endl;
               diTrans = BDI_TRANSITION_SMOOTHEST;
               break;
            default:
               cerr << "BDI_TRANSITION_SMOOTHEST" << endl;
               diTrans = BDI_TRANSITION_SMOOTHEST;
               break;
         }
         hml_char_set_transition_smoothness (BDI_ALL_CHARACTERS_HANDLE,
                                             diTrans);

         initman->query(DIS_ENUM, &local, GET);
         switch (local.ints[0]) {
            case (DISv203):
               cerr << "\tUsing DISv2.0.3 Human Enumerations." << endl;
               DIGUY_VEH::set203Enum ();
               break;
            case (DISv204) :
               cerr << "\tUsing DISv2.0.4 Human Enumerations." << endl;
               break;
            default:
               cerr << "\tUnkown Human Enumeration Type, Using DISv2.0.4."
                    << endl;
               break;
         }

         initman->query(DIGUY_POSITION_MECHANISM, &local, GET);
         cerr << "\tbdihml z position: ";
         switch (local.ints[0]) {
            case (DIGUY_INTERNAL):
               DIGUY_VEH::posMech = DIGUY_INTERNAL;
               cerr << "DIGUY_INTERNAL" << endl;
               break;
            default:
               DIGUY_VEH::posMech = NPSNET_INTERNAL;
               cerr << "NPSNET_INTERNAL" << endl;
               break;
          }
      }
#else
      cerr << "     DIGUY not built in." << endl;
#endif
   }
   return DIGUY_VEH::init;
}

void
DIGUY_VEH::set203Enum ()
{
   DIGUY_VEH::diApperance = Appearance_LifeForm_State_Mask_v203;
   DIGUY_VEH::diStill = Appearance_LifeForm_UprightStill_v203;
   DIGUY_VEH::diWalking = Appearance_LifeForm_UprightWalking_v203;
   DIGUY_VEH::diRunning = Appearance_LifeForm_UprightRunning_v203;
   DIGUY_VEH::diKneeling = Appearance_LifeForm_Kneeling_v203;
   DIGUY_VEH::diProne = Appearance_LifeForm_Prone_v203;
   DIGUY_VEH::diCrawling = Appearance_LifeForm_Prone_v203;
   DIGUY_VEH::diSwimming = Appearance_LifeForm_Prone_v203;
   DIGUY_VEH::diParachuting = Appearance_LifeForm_UprightStill_v203;
   DIGUY_VEH::diJumping = Appearance_LifeForm_UprightStill_v203;

   DIGUY_VEH::weaponAppearance = Appearance_LifeForm_Weapon1_Mask_v203;
   DIGUY_VEH::weaponStowed = Appearance_LifeForm_Weapon1_Stowed_v203; 
   DIGUY_VEH::weaponDeployed = Appearance_LifeForm_Weapon1_Deployed_v203;
   DIGUY_VEH::weaponFiring = Appearance_LifeForm_Weapon1_Firing_v203;
}

int
DIGUY_VEH::addType (int place, char* theType)
{
   int result = FALSE;
   if (!DIGUY_VEH::lookUp) {
      if (DIGUY_VEH::init) {
        cerr << "Error: in DIGUY_VEH::addType" << endl
             << "\tUnable to add type:" << theType <<  endl;
      }
   }
   else {
      result = DIGUY_VEH::lookUp->addType (place, theType);
   }

   return result;
}

void 
DIGUY_VEH::update_diguy ()
{
//   hml_char_update ();
}

typedef struct _holder {
   float isect;
   float diff;
   int valid;
} isectStruct;

#ifndef NODIGUY
float zHit (bdiCharacterHandle, float x, float y, float old_z)
{
   pfVec3 pos;
   pfVec3 dumby;
   float t_z, f_z;
   int   t_isect = 0, f_isect = 0;
   isectStruct iBin[2]; 
   iBin[0].valid = FALSE;
   iBin[1].valid = FALSE;
   pos[PF_X] = x;
   pos[PF_Y] = y;
   pos[PF_Z] = old_z - 1.0f;
 
//   cerr << "x: " << x << "  y: " << y << "  old_z: " << old_z << endl;

   t_isect = get_isect_terrain (t_z, dumby, pos);
   f_isect = get_isect_fixed (f_z, dumby, pos);

   if (t_isect) {
      iBin[0].isect = t_z;
      iBin[0].diff = fabsf (pos[PF_Z] + 2.0f - t_z);
      iBin[0].valid = TRUE;
   }

   if (f_isect) {
      iBin[1].isect = f_z;
      iBin[1].diff = fabsf (pos[PF_Z] + 1.0f - f_z);
      iBin[1].valid = TRUE;
   }

   isectStruct tmp;
   tmp.valid = TRUE;
   tmp.isect = 0.0f;
   // This may be bad, but I can never remember max Integer. It will
   // probably come back to bite me in the butt anyways. -rb
   tmp.diff = float(0xffffffff);
/*
cerr << "X     : " << x << endl;
cerr << "Y     : " << y << endl;
cerr << "Z     : " << old_z << endl;

cerr << "Ground: " << iBin[0].isect << endl;
cerr << "Fixed : " << iBin[1].isect << endl;
*/

   
   for (int ix = 0; ix <= 1; ix++) {
      if (iBin[ix].valid) {
         if (iBin[ix].diff < tmp.diff) {
            tmp.diff = iBin[ix].diff;
            tmp.isect = iBin[ix].isect;
         }
      }
   }
/*
cerr << "Choose: " << tmp.isect << endl;
cerr << "-=-=-=-=-=-=-=-=-=-=-" << endl;
//cerr << endl;
*/

   return tmp.isect;
      
}

bdiAction
get_action (unsigned int status, float speed)
{
//cerr << "speed: " << speed << endl
//     << (hex) << "status: " << status << (dec) << endl;

  bdiAction action = SOLDIER_STAND_READY;

  if ((status & Appearance_Damage_Mask) ==
                Appearance_DamageDestroyed) {
     action = SOLDIER_DEAD;
  }
  else if ((status & DIGUY_VEH::getDiApperance ()) == 
                     DIGUY_VEH::getDiStill ()) {
     if (speed == 0.0f) {
        if ((status & DIGUY_VEH::getWeaponAppearance ()) ==
                      DIGUY_VEH::getWeaponStowed ()){
           action = SOLDIER_STAND_READY;
        }
        else if ((status & DIGUY_VEH::getWeaponAppearance ()) ==
                           DIGUY_VEH::getWeaponDeployed ()) {
           action = SOLDIER_STAND_AIM;
        }
        else if ((status & DIGUY_VEH::getWeaponAppearance ()) ==
                           DIGUY_VEH::getWeaponFiring ()) {
        action = SOLDIER_STAND_AIM;
        }
     }
     else {
        if (speed == 0.0f) {
           action = SOLDIER_STAND_READY;
        }
        else if ((speed > 0.0f) && (speed < DI_JOG_THRESHOLD)) {
           action = SOLDIER_WALK;
        }
        else if (speed >= DI_JOG_THRESHOLD) {
           action = SOLDIER_JOG;
        }
        else if (speed < 0.0f) {
           action = SOLDIER_WALK_BACK;
        }
     }
  }
  else if ((status & DIGUY_VEH::getDiApperance ()) == 
                     DIGUY_VEH::getDiWalking ()) {
     if (speed == 0.0f) {
        action = SOLDIER_STAND_READY;
     }
     else if ((speed > 0.0f) && (speed < DI_JOG_THRESHOLD)) {
        action = SOLDIER_WALK;
     }
     else if (speed >= DI_JOG_THRESHOLD) {
        action = SOLDIER_JOG;
     }
     else if (speed < 0.0f) {
        action = SOLDIER_WALK_BACK;
     }
  }
  else if ((status & DIGUY_VEH::getDiApperance ()) == 
                     DIGUY_VEH::getDiRunning ()) { 
     if (speed == 0.0f) {
        action = SOLDIER_STAND_READY;
     }
     else if (speed > 0.0f) {
        action = SOLDIER_JOG;
     }
     else if (speed < 0.0f) {
        action = SOLDIER_WALK_LO_BACK;
     }
  }
  else if ((status & DIGUY_VEH::getDiApperance ()) == 
                     DIGUY_VEH::getDiKneeling ()) {
     if ((status & DIGUY_VEH::getWeaponAppearance ()) ==
                   DIGUY_VEH::getWeaponStowed ()) {
        action = SOLDIER_KNEEL_READY;
     }
     else if ((status & DIGUY_VEH::getWeaponAppearance ()) ==
                        DIGUY_VEH::getWeaponDeployed ()) {
        action = SOLDIER_KNEEL_AIM;
     }
     else if ((status & DIGUY_VEH::getWeaponAppearance ()) ==
                        DIGUY_VEH::getWeaponFiring ()) {
        action = SOLDIER_KNEEL_AIM;
     }
     else {
        action = SOLDIER_KNEEL_READY;
     }
  }
  else if ((status & DIGUY_VEH::getDiApperance ()) == 
                     DIGUY_VEH::getDiProne ()) {
     if ((status & DIGUY_VEH::getWeaponAppearance ()) ==
                   DIGUY_VEH::getWeaponStowed ()) {
        action = SOLDIER_PRONE_READY;
     }
     else if ((status & DIGUY_VEH::getWeaponAppearance ()) ==
                        DIGUY_VEH::getWeaponDeployed ()) {
        action = SOLDIER_PRONE_AIM;
     }
     else if ((status & DIGUY_VEH::getWeaponAppearance ()) ==
                        DIGUY_VEH::getWeaponFiring ()) {
        action = SOLDIER_PRONE_AIM;
     }
     else {
        action = SOLDIER_PRONE_READY;
     }
  }
  else if ((status & DIGUY_VEH::getDiApperance ()) == 
                     DIGUY_VEH::getDiCrawling ()) {
     if (speed == 0.0f) {
        action = SOLDIER_PRONE_READY;
     }
     else {
        action = SOLDIER_CRAWL;
     }
  }
  else if ((status & DIGUY_VEH::getDiApperance ()) == 
                     DIGUY_VEH::getDiSwimming ()) {
     if (speed == 0.0f) {
        action = SOLDIER_PRONE_READY;
     }
     else {
        action = SOLDIER_CRAWL;
     }
  }
  else if ((status & DIGUY_VEH::getDiApperance ()) == 
                     DIGUY_VEH::getDiParachuting ()) {
     action = SOLDIER_STAND_READY;
  }
  else if ((status & DIGUY_VEH::getDiApperance ()) == 
                     DIGUY_VEH::getDiJumping ()) {
     action = SOLDIER_STAND_READY;
  }
  else {
     if (speed == 0.0f) {
        action = SOLDIER_STAND_READY;
     }
     else if ((speed > 0.0f) && (speed < DI_JOG_THRESHOLD)) {
        action = SOLDIER_WALK;
     }
     else if (speed >= DI_JOG_THRESHOLD) {
        action = SOLDIER_JOG;
     }
     else if (speed < 0.0f) {
        action = SOLDIER_WALK_BACK;
     }
  }
#ifdef DEBUG_DI_ACTION
  print_action (action);
#endif
  return action;
}
#endif

#ifdef DEBUG_DI_ACTION
void print_action (bdiAction action)
{
   switch (action) {
      case SOLDIER_STAND_READY:
         cerr << "SOLDIER_STAND_READY" << endl;
         break;
      case SOLDIER_KNEEL_READY:
         cerr << "SOLDIER_KNEEL_READY" << endl;
         break;
      case SOLDIER_PRONE_READY:
         cerr << "SOLDIER_PRONE_READY" << endl;
         break;
      case SOLDIER_STAND_AIM:
         cerr << "SOLDIER_STAND_AIM" << endl;
         break;
      case SOLDIER_KNEEL_AIM:
         cerr << "SOLDIER_KNEEL_AIM" << endl;
         break;
      case SOLDIER_PRONE_AIM:
         cerr << "SOLDIER_PRONE_AIM" << endl;
         break;
      case SOLDIER_WALK:
         cerr << "SOLDIER_WALK" << endl;
         break;
      case SOLDIER_WALK_LO:
         cerr << "SOLDIER_WALK_LO" << endl;
         break;
      case SOLDIER_WALK_BACK:
         cerr << "SOLDIER_WALK_BACK" << endl;
         break;
      case SOLDIER_CRAWL:
         cerr << "SOLDIER_CRAWL" << endl;
         break;
      case SOLDIER_WALK_LO_BACK:
         cerr << "SOLDIER_WALK_LO_BACK" << endl;
         break;
      case SOLDIER_JOG:
         cerr << "SOLDIER_JOG" << endl;
         break;
      case SOLDIER_DEAD:
         cerr << "SOLDIER_DEAD" << endl;
         break;
      default:
         cerr << "Unknown Action." << endl;
         break;
   }
}
#endif

float speed_direction(float speed, pfVec3& velocity, float heading)
{
   // Need to determine if going backwards
   float newSpeed;
   pfVec2 vel, headingvec;
   float hdg;
   vel[X] = velocity[X];
   vel[Y] = velocity[Y];
   pfNormalizeVec2(vel);
   hdg = 90.0f+heading;
   while (hdg < 0.0f) {
      hdg += 360.0f;
   }
   while (hdg > 360.0f) {
      hdg -= 360.0f;
   }

   pfSinCos(hdg, &headingvec[Y],&headingvec[X]);
   if(pfDotVec2(headingvec,vel) < 0.0f) {
      newSpeed = -fabs(speed);
   }
   else {
      newSpeed = fabs(speed);
   }

   return newSpeed;
}

void DIGUY_VEH::get_firepoint ( pfVec3 &fire )
{
cerr << "DIGUY get firepoint." << endl;
   pfCopyVec3 ( fire, gun_pos.xyz );
}

