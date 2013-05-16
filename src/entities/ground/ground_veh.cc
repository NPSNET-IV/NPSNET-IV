
#include <math.h>
#include <string.h>
#include <bstring.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "ground_veh.h"
#include "common_defs2.h"
#include "macros.h"
#include "terrain.h"
#include "sound.h"
#include "disnetlib.h"
#include "netutil.h"
#include "effects.h"
#include "entity.h"
#include "munitions.h"
#include "appearance.h"
#include "conversion_const.h"
#include "interface_const.h"
#include "local_snd_code.h"
#include "collision_const.h"
#include "articulat.h"
#ifndef NO_DVW
#define NO_PFSMOKE
#define NO_NPSSMOKE
    extern "C" void processVdustVariant(EntityStatePDU*);
#endif

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
extern volatile float            G_curtime;
extern          pfGroup         *G_fixed;

// Locals used by this library
static pfVec3 ZeroVec = {0.0f,0.0f,0.0f};

//************************************************************************//
//****************** class GROUND_VEH ************************************//
//************************************************************************//


GROUND_VEH::GROUND_VEH(int N,int atype, ForceID the_force)
: VEHICLE(N,atype,the_force)
{
//   cerr << "Ground Vehicle constructor\n";
   turret = NULL;
   gun = NULL;
   turret1_angle = 0.0f;
   turret1_rate = 0.0f;
   turret1_a_change = 0;
   turret1_r_change = 0;
   gun1_angle = 0.0f;
   gun1_rate = 0.0f;
   gun1_a_change = 0;
   gun1_r_change = 0;
   DRturret1_angle = 0.0f;
   DRturret1_rate = 0.0f;
   DRgun1_angle = 0.0f;
   DRgun1_rate = 0.0f;

}

void GROUND_VEH::del_vehicle()
//delete the object
{
  this->GROUND_VEH::~GROUND_VEH();
}

GROUND_VEH::~GROUND_VEH()
{
#if 0
int i;
#ifdef __NOT_OUR_PROBLEM_YET
//  cerr << "Removing " << vid << " " << pfRemoveChild(G_moving,hull) << "  ";
//  cerr << pfDelete(models) << "  ";
//  cerr << pfDelete(hull) << "  ";
//  cerr << pfDelete(turret) << "  ";
//  cerr << pfDelete(gun) << "  ";
//   cerr << "Grnd veh destructor (6)\n";
//   pfRemoveChild(G_moving,hull);
//   pfDelete(models);
//   pfDelete(hull); // KNOWN MEMORY LEAK
//   pfDelete(turret);
//   pfDelete(gun);
#endif
  if ( hull != NULL )
     {
     pfRemoveChild(G_moving,hull);

     pfRemoveChild(hull,models);
     pfDelete(hull);
     hull = NULL;
     }
  for (i = 0; i < num_joints; i++)
     {
     joint[i].dcs = NULL;
     strcpy ( joint[i].name, "" );
     }
  num_joints = 0;
#endif

}

vtype GROUND_VEH::gettype()
{
   return (GND_VEH);
}

void GROUND_VEH::slewturret(float deltatime)
//turn the turret
{
   turret1_angle += (turret1_rate*deltatime);
   while ( turret1_angle < 0.0 )
      turret1_angle += 360.0f;
   turret1_angle = fmod ( turret1_angle, 360.0f );
   if ( ( num_joints >= 1 ) && (joint[0].dcs != NULL) ) {
      pfDCSRot(joint[0].dcs, turret1_angle, 0.0f, 0.0f );
   }

}

void GROUND_VEH::elevatgun(float deltatime)
// raise the gun
{
   gun1_angle += (gun1_rate*deltatime);
   while ( gun1_angle < -180.0f ) {
      gun1_angle += 360.0f;
   }
   while ( gun1_angle > 180.0f ) {
      gun1_angle -= 360.0f;
   }
   if ( ( num_joints >=2 ) && (joint[1].dcs != NULL) ) {
      pfDCSRot(joint[1].dcs, 0.0f, gun1_angle, 0.0f );
   }

}

void GROUND_VEH::turret_set(float amount)
{
   turret1_angle = amount;
   while ( turret1_angle < 0.0 )
      turret1_angle += 360.0f;
   turret1_angle = fmod ( turret1_angle, 360.0f );

}

void GROUND_VEH::gun_set(float amount)
{
   gun1_angle = amount;
   while ( gun1_angle < -180.0f ) {
      gun1_angle += 360.0f;
   }
   while ( gun1_angle > 180.0f ) {
      gun1_angle -= 360.0f;
   }

}

void GROUND_VEH::turret_update(float amount)
{
   turret1_angle += amount;
   while ( turret1_angle < 0.0 )
      turret1_angle += 360.0f;
   turret1_angle = fmod ( turret1_angle, 360.0f );
}

void GROUND_VEH::gun_update(float amount)
{
   gun1_angle += amount;
   while ( gun1_angle < -180.0f ) {
      gun1_angle += 360.0f;
   }
   while ( gun1_angle > 180.0f ) {
      gun1_angle -= 360.0f;
   }

}

movestat GROUND_VEH::move()
// move the vehicle for the next frame
{
   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   float speed_setting;
   pfVec3 oldpos;
   float altitude;
   static int old_tethered;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tGROUND_VEH::move" << endl;
#endif

#ifdef TRACE
   cerr << "Moving a Ground Vehicle\n";
#endif

   old_tethered = tethered;

   update_time ();
   collect_inputs ();
#ifdef SIGGRAPHHACK96
   cdata.roll *= 1.75f;
#endif
   turret1_angle = look.hpr[HEADING];
   gun1_angle = look.hpr[PITCH];


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
      if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_BASIC_PANEL) || (input_control == NPS_FLYBOX) || 
          (input_control == NPS_LEATHER_FLYBOX))
         {
         if (input_control == NPS_LEATHER_FLYBOX)
            {
            cdata.thrust = -cdata.pitch;
            cdata.pitch = 0.0f;
            }
         speed_setting = cdata.thrust  * MAX_GROUND_SPEED;

         if (speed < speed_setting)
            speed += FCS_SPEED_CHANGE;
         if (speed > speed_setting)
            speed -= FCS_SPEED_CHANGE;
         cdata.roll = cdata.pitch = 0.0f;
         }
      else
         {
         speed_setting = cdata.thrust * MAX_GROUND_SPEED;

         speed += cdata.thrust;
         if ((speed < KEY_SPEED_CHANGE) && (speed > 0.0f))
            speed -= 1.0f;
         cdata.roll = cdata.pitch = cdata.thrust = 0.0f;
         }

      // Don't want to exceed the max speed
      if (speed > MAX_GROUND_SPEED)
         speed = MAX_GROUND_SPEED;
      // Don't want to go backwards too fast
      if (speed < MAX_GROUND_REVERSE)
         speed = MAX_GROUND_REVERSE;

      //move the GROUND vehicle type in the x-y plane
      posture.xyz[X] += velocity[X]*cdata.delta_time;
      posture.xyz[Y] += velocity[Y]*cdata.delta_time;
        
      altitude = ::get_altitude(posture.xyz);

//   altitude = 0.0f;
   // Now see if we are going to hit something in this time interval
   if (!G_static_data->stealth)
      {
      if ( check_collide(oldpos, posture.xyz,
                         posture.hpr,
                         speed, altitude) )
         {
         cdata.thrust = 0.0f;
         pfSetVec3 ( velocity, 0.0f, 0.0f, 0.0f );
         return(HITSOMETHING);
         }
      }

      if ( (altitude < 10.0f ) && (altitude > -10.0f) )
         {
         if (!grnd_orient(posture))
            {
            posture.xyz[Z] = 3.5f;
            }
         }
      else if (G_static_data->stealth)
         {
         if (!grnd_orient(posture))
            posture.xyz[Z] = 3.5f;
         }
      else
         {
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         G_dynamic_data->dead = DYING;
         G_dynamic_data->cause_of_death = FALLING_DEATH;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
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
   if (tethered)
      {
      if ((cdata.vid > -1) && G_vehlist[cdata.vid].vehptr)
         {
         if ( tethered == 1)
            G_vehlist[cdata.vid].vehptr->teleport(posture,speed);
         else if ( tethered == 2)
            G_vehlist[cdata.vid].vehptr->attach(posture,speed,
                                                G_dynamic_data->attach_dis);
         }
      else
         {
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
   pfScaleVec3 ( up, EYE_OFFSET, up);
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

   check_targets ( posture.xyz, posture.hpr, 
                   look.hpr );

   //put the vehicle in the correct location in space
   pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
   pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);
   slewturret(cdata.delta_time);
   elevatgun(cdata.delta_time);

#ifdef DEBUG_SCALE
   pfDCSScale(hull,2.0f);
#endif
   thrust = cdata.thrust;
   return(FINEWITHME);

}




movestat GROUND_VEH::moveDR(float deltatime,float curtime)
//move routine for DR vehciles simple move
{
   float accelfac;
   float altitude = 0.0f;
   static float oldspeed = 0.0f;
   float speed;

#ifdef TRACE
   cerr << "Dead Recon a Ground Vehicle\n";
#endif

#ifndef NO_PFSMOKE
   update_pfsmokes();
#endif

 if ( ((curtime - lastPDU) > G_static_data->DIS_deactivate_delta) &&
      (vid != G_drivenveh))
  {
    //Deactivate the puppy
//    cerr <<"It has been to long since the last update GROUND_VEH\n";
    //not us, so we should deactiveate this vehcile
    return(DEACTIVE);
  }
  if ( snap_switch == TRUE )
    {
    //cerr << "Veh# " << vid << " is close enough to snap..." << endl;
    altitude = ::get_altitude(drpos.xyz);
    }

  switch(dralgo)
  {
    case DRAlgo_Static: //it is fixed, so it does not move
      break;
    case DRAlgo_DRM_FPW: //Fixed orientation,Constant Speed
      // Move in the X-Y plane
      drpos.xyz[X] += drparam.xyz[X]*deltatime;
      drpos.xyz[Y] += drparam.xyz[Y]*deltatime;
      // Now put it on the ground and set HPR
         if ( (altitude > -5.0f) && (altitude < 5.0f)  )
           {
           if ( snap_switch == TRUE )
              {
              //cerr << "A Snapping veh# " << vid << endl;
              grnd_orient(drpos);
              }
           }
        else
           {
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
      if ( (altitude > -5.0f) && (altitude < 5.0f) )
         {
         if ( snap_switch == TRUE )
            {
            //cerr << "B Snapping veh# " << vid << endl;
            grnd_orient(drparam);
            }
         }
      else
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
      if ( (altitude > -5.0f) && (altitude < 5.0f) )
         {
         if ( snap_switch == TRUE )
            {
            if (!grnd_orient(drpos))
               drpos.xyz[Z] = 3.5f;
            //cerr << "C Snapping veh# " << vid << endl;
            }
         }
      else
         drpos.xyz[Z] += velocity[Z]*deltatime;

      break;
  }

  if (vid != G_drivenveh){
     posture = drpos;
     //put the vehicle in the correct location in space
  pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y],posture.xyz[Z]);
  pfDCSRot(hull, posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);
//  slewturret(deltatime);
//  elevatgun(deltatime);
#ifdef DEBUG_SCALE
  pfDCSScale(hull,2.0f);
#endif

  }
  else if ( !G_static_data->stealth )
  {
    speed = pfLengthVec3(velocity);
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
    else if ( speed != oldspeed )
       {
       if ( (!IS_ZERO(oldspeed)) && (IS_ZERO(speed)) )
          {
          sendentitystate();
          oldspeed = speed;
          }
       else if ( (IS_ZERO(oldspeed)) && (!IS_ZERO(speed)) )
          {
          sendentitystate();
          oldspeed = speed;
          }
       else if ( fabs(oldspeed-speed) > 50.0f )
          {
          sendentitystate();
          oldspeed = speed;
          }
       }
    else if ( num_joints > 0 )
       {
       if ( (fabs(DRturret1_angle - turret1_angle) >= 3.0f) ||
            (fabs(DRturret1_rate - DRturret1_rate) >= 2.0f) )
          {
          sendentitystate();
          }
       else if ( ( num_joints > 1 ) &&
                 (fabs(DRgun1_angle - gun1_angle) >= 3.0f) )
          {
          sendentitystate();
          }
       }


  }
      return(FINEWITHME);
}

void GROUND_VEH::sendentitystate()
//send a entity state PDU with articulated parts
{
  ArticulatParams       *APNptr;
  float temp_float;
  char tempstr[MARKINGS_LEN];
  int num_art_par = 0;
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
  if ( num_joints > 0 )
     {
     /* SEND TURRET AZIMUTH (heading) */
     APNptr = epdu.articulat_params+0;
        {
        if ( DRturret1_angle != turret1_angle )
           turret1_a_change++;
        DRturret1_angle = turret1_angle;
//        APNptr->change = (unsigned short)turret1_a_change;
        APNptr->change = (unsigned short)0;
        APNptr->ID = (unsigned short)0x0;
        APNptr->parameter_type = (unsigned int)4107;
#ifdef DIS_2v4
        APNptr->parameter_type_designator = 0;
#endif
        temp_float = ((360.0f - turret1_angle) * DEG_TO_RAD);
        bzero ( &(APNptr->parameter_value), 8);
        bcopy ( &temp_float, &(APNptr->parameter_value), 4);
        num_art_par++;
        }

     /* SEND TURRET SPIN RATE */

/* COMMENTED OUT
     APNptr = epdu.articulat_params+1;
        {
        if ( DRturret1_rate != turret1_rate )
           turret1_r_change++;
        DRturret1_rate = turret1_rate;
        APNptr->change = (unsigned short)turret1_r_change;
        APNptr->ID = (unsigned short)0x0;
        APNptr->parameter_type = (unsigned int)4108;
        temp_float = -turret1_rate;
        bzero ( &(APNptr->parameter_value), 8);
        bcopy ( &temp_float, &(APNptr->parameter_value), 4);
        num_art_par++;
        }
*/
     }
  if ( num_joints > 1 )
     {
     /* SEND GUN ELEVATION */
     APNptr = epdu.articulat_params+1;
        {
        if ( DRgun1_angle != gun1_angle )
           gun1_a_change++;
        DRgun1_angle = gun1_angle;
//        APNptr->change = (unsigned short)gun1_a_change;
        APNptr->change = (unsigned short)0;
        APNptr->ID = (unsigned short)0x1;
        APNptr->parameter_type = (unsigned int)4429;
#ifdef DIS_2v4
        APNptr->parameter_type_designator = 0;
#endif
        temp_float = (gun1_angle * DEG_TO_RAD);
        bzero ( &(APNptr->parameter_value), 8);
        bcopy ( &temp_float, &(APNptr->parameter_value), 4);
        num_art_par++;
        }
     }

  epdu.num_articulat_params = (unsigned char)num_art_par;

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

void GROUND_VEH::entitystateupdate(EntityStatePDU *epdu, sender_info &sender)
//get an entity state PDU with articulated parts
{
/*
   ArticulatParams      *APNptr;
   float temp_float;
   int count;
*/

#ifndef NO_DVW
   processVdustVariant(epdu);
#endif

   //what and who I am
   disname = epdu->entity_id;
   type = epdu->entity_type;
   force = epdu->force_id;
   //How fast is it going
   velocity[X] = epdu->entity_velocity.x;
   velocity[Y] = epdu->entity_velocity.y;
   velocity[Z] = epdu->entity_velocity.z;
   //Where we are
   posture.xyz[X] = (float)epdu->entity_location.x;
   posture.xyz[Y] = (float)epdu->entity_location.y;
   posture.xyz[Z] = (float)epdu->entity_location.z;
   //How we are facing

   posture.hpr[HEADING] =  (epdu->entity_orientation.psi < 0)?  -(epdu->entity_orientation.psi + 2*M_PI)*RAD_TO_DEG:
                                                                -(epdu->entity_orientation.psi)*RAD_TO_DEG;

   posture.hpr[PITCH]   =  epdu->entity_orientation.theta*RAD_TO_DEG;
   posture.hpr[ROLL]    =  epdu->entity_orientation.phi*RAD_TO_DEG;
   //Acceleration vector values
   acceleration[X] = epdu->dead_reckon_params.linear_accel[X];
   acceleration[Y] = epdu->dead_reckon_params.linear_accel[Y];
   acceleration[Z] = epdu->dead_reckon_params.linear_accel[Z];

   pfCopyVec3(drparam.xyz,velocity);
   pfCopyVec3(drparam.hpr,acceleration);
   drpos=posture;

   update_articulations ( epdu->articulat_params, epdu->num_articulat_params );
   while ( gun1_angle < -180.0f ) {
      gun1_angle += 360.0f;
   }
   while ( gun1_angle > 180.0f ) {
      gun1_angle -= 360.0f;
   }

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
         gun1_angle = float(temp_float * RAD_TO_DEG);
         while ( gun1_angle < -180.0f ) {
            gun1_angle += 360.0f;
         }
         while ( gun1_angle > 180.0f ) {
            gun1_angle -= 360.0f;
         }

         }
      count++;
      APNptr++;
      }
*/



   //what we look like
   status = epdu->entity_appearance;
   changestatus(status);

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



movestat GROUND_VEH::movedead()
{
  float gnd_elev = gnd_level(posture.xyz);
  static float dead_time;
  static float grnd_slam_time;
  static int hit;
  static int my_status;
  static int cause;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tGROUND_VEH::movedead" << endl;
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
        changestatus(status);
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
        changestatus(status);
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
#ifndef NODEADCYCLE
  else
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
        status = ALIVE;
        changestatus(status);
        }
     }
#endif // NODEADCYCLE

  altitude = ::get_altitude(posture.xyz);

  //put the vehicle in the correct location in space
  pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
  pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);


  return FINEWITHME;
}



int GROUND_VEH::check_collide(pfVec3 &startpnt, pfVec3 &endpnt,
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
   pfCylinder the_barrel;
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
   pfScaleVec3 ( foot_vec, INTERSECT_OFFSET, tb_vec );
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

  pfCylAroundSegs ( &the_barrel, (const pfSeg **)sp, 2 );

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
            pfScaleVec3 ( normal, EYE_OFFSET + 5.0f, normal );
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

int
GROUND_VEH::transport ( float new_x, float new_y, float )
{
   pfVec3 temp_pos;
   float curr_alt;

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


