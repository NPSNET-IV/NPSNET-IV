
#include <math.h>

#include "fixed_wing_veh.h"
#include "common_defs2.h"
#include "terrain.h"
#include "macros.h"
#include "entity.h"
#include "conversion_const.h"
#include "interface_const.h"


// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          VEH_LIST     *G_vehlist;

// Locals used by this library
static pfVec3 ZeroVec = {0.0f,0.0f,0.0f};

//************************************************************************//
//**************** class FIXED_WING_VEH **********************************//
//************************************************************************//

FIXED_WING_VEH::FIXED_WING_VEH(int id,int atype, ForceID the_force)
:AIRCRAFT_VEH(id,atype,the_force)
{
#ifdef TRACE
   cerr << "Fixed wing constructor\n";
#endif
}


vtype FIXED_WING_VEH::gettype()
{
   return (FIXED_WING);
}


movestat FIXED_WING_VEH::move()
// move the vehicle for the next frame
{
   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   pfVec3 tempeye;
   float speed_setting = cdata.thrust * MAX_SPEED;
   //float thrust_setting = cdata.thrust * MAX_THRUST;
   float ground, pitch_factor, altitude;
   pfVec3 oldpos;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tFIXED_WING_VEH::move" << endl;
#endif

   update_time ();
   collect_inputs ();

   pfCopyVec3(oldpos,posture.xyz);

#ifdef TRACE
   cerr << "Moving a FIXED_WING_VEH\n";
#endif

   if (!(paused || tethered || cdata.rigid_body))
      {

      // Move to the location we should be at after the last time interval
      posture.xyz[X] += velocity[X]*cdata.delta_time;
      posture.xyz[Y] += velocity[Y]*cdata.delta_time;
      posture.xyz[Z] += velocity[Z]*cdata.delta_time;
      tempeye[X] = eyepoint[X] + (velocity[X] * cdata.delta_time);
      tempeye[Y] = eyepoint[Y] + (velocity[Y] * cdata.delta_time);
      tempeye[Z] = eyepoint[Z] + (velocity[Z] * cdata.delta_time);

      ground = gnd_level(posture.xyz);
      altitude = posture.xyz[Z] - ground + bounding_box.min[PF_Z];

      // Now see if we hit something in this time interval
      if (!G_static_data->stealth)
         {
         if ( check_collide( oldpos, posture.xyz,
                             posture.hpr,
                             speed, altitude) )
            return(HITSOMETHING);
         }


      // Set the speed for the next frame, pitch_factor adds or
      // subtracts from speed at level flight
      pitch_factor = (sinf(posture.hpr[PITCH] * DEG_TO_RAD));

      if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_BASIC_PANEL) || (input_control == NPS_FLYBOX) )
         {
         if ( (speed < speed_setting) && IS_SMALL(pitch_factor) )
            speed += FCS_SPEED_CHANGE - pitch_factor;
         if ( (speed > speed_setting) && IS_SMALL(pitch_factor) )
            speed -= FCS_SPEED_CHANGE - pitch_factor;
         }
      else
         {
         speed += cdata.thrust;
         cdata.thrust = 0.0f;
         }

      speed -= pitch_factor;
      if ( (speed < STALL_SPEED) && (posture.xyz[Z] > ground) )
         {
         posture.xyz[Z] -=
            ((STALL_SPEED-speed)*5.0f)/STALL_SPEED;
         posture.hpr[PITCH] -= 5.0f;
         }
      if (posture.xyz[Z] < ground)
         posture.xyz[Z] = ground;

      if (speed > MAX_SPEED)
         speed = MAX_SPEED;


      // set pitch and roll when in the air
      if ( G_static_data->stealth || (altitude > 0.0f))
         {
         posture.hpr[ROLL] =
                  fmod ( cdata.roll*4.0f + posture.hpr[ROLL], 360.0f);
         if (posture.hpr[ROLL] > 180.0f)
            posture.hpr[ROLL] -= 360.0f;
         if (posture.hpr[ROLL] < -180.0f)
            posture.hpr[ROLL] += 360.0f;

         posture.hpr[PITCH] =
            fmod ( cosf(posture.hpr[ROLL]*DEG_TO_RAD) * cdata.pitch
               * 2.0f + posture.hpr[PITCH], 180.0f);

         if (posture.hpr[PITCH] > 90.0f)
            {
            posture.hpr[PITCH] = 180.0f - posture.hpr[PITCH];
            posture.hpr[ROLL] -= 180.0f;
            posture.hpr[HEADING] -= 180.0f;
            if ( posture.hpr[HEADING] < 0.0f)
                posture.hpr[HEADING] += 360.0f;
            }
         if (posture.hpr[PITCH] < -90.0f)
            {
            posture.hpr[PITCH] = -180.0f - posture.hpr[PITCH];
            posture.hpr[ROLL] -= 180.0f;
            posture.hpr[HEADING] -= 180.0f;
            if ( posture.hpr[HEADING] < 0.0f)
                posture.hpr[HEADING] += 360.0f;
            }
          }
      else  // not in the air
          {
          // only allow the vehicle to go up when on the ground
          if (cdata.pitch < 0 )
             {
             cdata.pitch = 0.0f;
             }
         // still on the ground, so steer with stick
         posture.hpr[HEADING] -= cdata.roll;
         grnd_orient(posture,&(bounding_box));
         posture.hpr[PITCH] += cdata.pitch;
         }


      // Calculate the heading
      posture.hpr[HEADING] = fmod (( posture.hpr[HEADING] -
              ((cdata.pitch*2.0f+2.0f) * sinf(posture.hpr[ROLL] *
                DEG_TO_RAD) * speed/MAX_SPEED)),360.0f);

      if (posture.hpr[HEADING] < 0.0f)
         posture.hpr[HEADING] += 360.0f;

      // Zero control data for the next frame
      cdata.pitch = cdata.roll = 0.0f;

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

         pfXformPt3 ( direction, direction, orient );
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

   pfXformPt3 ( up, up, orient );
   pfNormalizeVec3 ( up );
   pfScaleVec3 ( up, EYE_OFFSET, up);
   pfAddVec3 ( eyepoint, posture.xyz, up );
   pfCopyVec3 ( firepoint, eyepoint );

   altitude = ::get_altitude(posture.xyz) + bounding_box.min[PF_Z];
   if ( (!tethered) && (!G_static_data->stealth) && (altitude < -0.1) )
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

#ifdef DEBUG_SCALE
   pfDCSScale(hull,2.0f);
#endif

   thrust = cdata.thrust;
   return(FINEWITHME);

}

