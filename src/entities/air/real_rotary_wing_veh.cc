#include <math.h>
#include <string.h>

#include "real_rotary_wing_veh.h"
#include "rotary_wing_veh.h"
#include "common_defs2.h"
#include "terrain.h"
#include "macros.h"
#include "entity.h"
#include "conversion_const.h"
#include "interface_const.h"
#include "fcs.h"

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          VEH_LIST     *G_vehlist;
extern          pfGroup      *G_moving;

// Locals used by this library
static pfVec3 ZeroVec = {0.0f,0.0f,0.0f};

//********************************************************************//
//*************** class REAL_ROTARY_WING_VEH *************************//
//********************************************************************//

REAL_ROTARY_WING_VEH::REAL_ROTARY_WING_VEH(int id,int atype, ForceID the_force)
:ROTARY_WING_VEH(id,atype,the_force)
{
   subpartscnt = 0;
   mainrotor = NULL;
   tailrotor = NULL;
   mrangle = 0.0f;
   trangle = 0.0f;
   mrrate = 2400.0f;
   trrate = 6000.0f;
}


void REAL_ROTARY_WING_VEH::del_vehicle()
//delete the object
{
  this->REAL_ROTARY_WING_VEH::~REAL_ROTARY_WING_VEH();
}


REAL_ROTARY_WING_VEH::~REAL_ROTARY_WING_VEH()
{
int i;
//   pfDelete(mainrotor);
//   pfDelete(tailrotor);
//   cerr << "Real Rotor Wing destructor\n";
  if ( hull != NULL )
     {
     pfRemoveChild(G_moving,hull);

     pfRemoveChild(hull,models);
//     pfDelete(hull);
     hull = NULL;
     }
  for (i = 0; i < num_joints; i++)
     {
     joint[i].dcs = NULL;
     strcpy ( joint[i].name, "" );
     }
  num_joints = 0;

}


vtype REAL_ROTARY_WING_VEH::gettype()
{
   return (PB_ROTARY_WING);
}


movestat REAL_ROTARY_WING_VEH::move()
// move the vehicle for the next frame
{
   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   pfVec3 tempeye;

   // These are new variables for calculating velocity
   pfVec3 up_dir = { 0.0f, 0.0f, 1.0f };
   pfVec3 down_dir = { 0.0f, 0.0f, -1.0f };
   pfVec3 right_dir = { 1.0f, 0.0f, 0.0f };
   pfVec3 weight_vec;
   pfVec3 thrust_vec;
   pfVec3 total_lift_vec;
   pfVec3 fw_vec;
   pfVec3 lat_vec;
   pfVec3 temp_vec;
   float thrust_setting;
   float weight_setting;

   float ground;
   pfVec3 oldpos;

   // New values for forward and lateral speeds and changes
   static float lat_speed;
   static float fw_speed;
   static float lat_accel;
   static float fw_accel;
   float lat_change = 0.0f;
   float fw_change = 0.0f;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tREAL_ROTARY_WING_VEH::move" << endl;
#endif

   update_time ();
   collect_inputs ();

   pfCopyVec3(oldpos,posture.xyz);

#ifdef TRACE
   cerr << "Moving ROTARY_VEH\n";
#endif

   // This is the actual code to figure out where to move
   if (!(paused || tethered ))
      {

      // Move to the location we should be at after the last time interval
      posture.xyz[X] += velocity[X]*cdata.delta_time;
      posture.xyz[Y] += velocity[Y]*cdata.delta_time;
      posture.xyz[Z] += velocity[Z]*cdata.delta_time;
      tempeye[X] = eyepoint[X] + (velocity[X] * cdata.delta_time);
      tempeye[Y] = eyepoint[Y] + (velocity[Y] * cdata.delta_time);
      tempeye[Z] = eyepoint[Z] + (velocity[Z] * cdata.delta_time);

      // Find out what our altitude is
      ground = gnd_level(posture.xyz);
      altitude = posture.xyz[Z] - ground + bounding_box.min[PF_Z];
      thrust = cdata.thrust;
      if ( altitude < 0.0 ) 
      {
         altitude = 0.0;
         posture.xyz[Z] = ground - bounding_box.min[PF_Z];
      }

      // Now see if we are going to hit something in this time interval
      if ( !G_static_data->stealth )
      {
         if ( check_collide( oldpos, posture.xyz,
                             posture.hpr,
                             speed, altitude) )
         { // We crashed, so reset everything
            altitude = 0.0f;
            cdata.thrust = cdata.pitch = cdata.roll = cdata.rudder = 0.0f;
            fw_change = lat_change = 0.0f;
            fw_speed = speed = 0.0f;
            fw_accel = 0.0f;
            lat_speed = 0.0f;
            lat_accel = 0.0f;
            weight_setting = 0.0f;
            thrust_setting = 0.0f;
            pfCopyVec3(fw_vec, ZeroVec);
            pfCopyVec3(lat_vec, ZeroVec);
            pfCopyVec3(temp_vec, ZeroVec);
            pfCopyVec3(velocity, ZeroVec);
            pfCopyVec3(thrust_vec, ZeroVec);
            pfCopyVec3(total_lift_vec, ZeroVec);
            return(HITSOMETHING);
         }
      }

      // Set the new speed
#ifdef TRACE
   cerr << "Set speed\n";
#endif
      if ((input_control == NPS_FCS) || 
          (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_FLYBOX) ||
          (input_control == NPS_BASIC_PANEL))
         {
         // Changes based on control inputs.
         lat_change = cdata.roll * MY_FCS_SPEED_CHANGE;
         fw_change = -(cdata.pitch * MY_FCS_SPEED_CHANGE);

         // Speed values on the ground
         if ( altitude < 0.06 )
            {
            // Can't move sideways on the ground
            lat_speed = lat_accel = 0.0f;

            // Makes acceleration 0.0 if cdata.pitch is 0.0
            if (fw_change == 0.0f)
               fw_accel = 0.0f;

            // If there is pitch input, accelerate or decelerate
            else
               fw_accel += ( 0.5f * fw_change * 2.0f * cdata.thrust);

            // This decels us if faster than fastest speed on the ground
            if ( fw_speed > MAX_GND_SPEED )
               fw_accel = -(6.0f * MY_FCS_SPEED_CHANGE) + fw_change;

            // This accels us if slower than slowest speed on the ground
            else if ( fw_speed < MIN_GND_SPEED )
               fw_accel = (6.0f * MY_FCS_SPEED_CHANGE) + fw_change;

            // Limit acceleration, deceleration
            if (fw_accel > MAX_ACCEL)
               fw_accel = MAX_ACCEL;
            if (fw_accel < MAX_DECEL)
               fw_accel = MAX_DECEL;

            // Change speed by adding old speed to dampened acceleration
            fw_speed += (fw_accel * 0.5f);               
            } // end if on the ground

         // Speed values in the air
         else
            {
            // Accel/decel sideways motion depending on stick inputs
            if (fw_speed < 25.0f)
               // This gives the full amount of change if slower than 25
               lat_accel += lat_change;

            else
               // Amount of change decreases with increase in forward speed
               lat_accel += lat_change/fw_speed;

            // Adjust speed according to amount of dampened acceleration
            lat_speed += (0.1f * lat_accel);

            // Accel/decel forward motion with stick input, but reduce the
            // amount of accel when roll approaches 90 deg.
            fw_accel += (fw_change * cosf(posture.hpr[ROLL]*DEG_TO_RAD));

            // Adjust speed according to amount of dampened acceleration
            fw_speed += (0.1f * fw_accel);

            // Above speed 50, no more lateral speed
            if (fw_speed > 50.0f) 
               lat_speed = lat_accel = 0.0;

            // Limit lateral speed within programmed limits
            else if ( lat_speed > MAX_LAT_SPEED )
               lat_speed = MAX_LAT_SPEED;
            else if ( lat_speed < MIN_LAT_SPEED )
               lat_speed = MIN_LAT_SPEED;

            // Limit forward speed within programmed limits
            if ( fw_speed < MIN_FW_SPEED )
               fw_speed = MIN_FW_SPEED;
            }

         // Set speed for our airspeed indicator
         speed = fw_speed;
         } // End if using flight controls

      else
         { // Not using the flight control throttle, so just add change
         lat_speed = lat_accel = 0.0;
         fw_speed += cdata.thrust;
         speed = fw_speed;
         cdata.thrust = 0.0f;
         } // end not using flight controls

      // Calculate the magnitude of thrust vector
      // Amount of thrust available is limited by how fast we are, in
      //  effect, giving max forward airspeed in straight and level flight
      thrust_setting = (cdata.thrust * MAX_POWER) - 
                       ((HELO_WEIGHT/MAX_FW_SPEED) * fw_speed);

      // Now calculate the magnitude of weight vector: equal to thrust up 
      // to helo weight so DR'ed helo on ground looks correct
      if ( (!G_static_data->stealth) && ( altitude < 0.1f ) && 
           ( thrust_setting < HELO_WEIGHT ))
         weight_setting = thrust_setting;

      else // we are airborne or pulling more than our weight
         weight_setting = HELO_WEIGHT;


      // DETERMINE THE NEW HEADING PITCH AND ROLL
      // If we are on the ground, our pitch and roll will be set by the 
      // terrain, and we steer with roll or rudder input

#ifdef TRACE
   cerr << "New HPR\n";
#endif

      if ( (!G_static_data->stealth) && (altitude < 0.06f))
         // We are on the ground
         {
         grnd_orient(posture,&(bounding_box));
         posture.hpr[HEADING] += (cdata.rudder * 2.0f);
         } // end HPR calculation on the ground

      else  // HPR calculation when airborne
         {
         if (!IS_ZERO(speed))
           posture.hpr[ROLL] =
                 fmod ( cdata.roll + posture.hpr[ROLL], 360.0f);

         posture.hpr[PITCH] =
              fmod ( cosf(posture.hpr[ROLL]*DEG_TO_RAD) * cdata.pitch * 0.4f
                      + posture.hpr[PITCH], 180.0f);

         // This section just resets HPR to normal limits 
         if (posture.hpr[ROLL] > 180.0f)
            posture.hpr[ROLL] -= 360.0f;
         if (posture.hpr[ROLL] < -180.0f)
            posture.hpr[ROLL] += 360.0f;

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

         // Calculate the heading if slower that 25 mph
         if (speed < 25.0f)
            {
            // Allows the rudder pedals, if used, to control the heading
            posture.hpr[HEADING] += (cdata.rudder * 5.0f);
            }

         // Calculate the heading if faster that 25 mph
         else
            posture.hpr[HEADING] = fmod (( posture.hpr[HEADING] -
                   (0.5f * ((cdata.pitch*2.0f+2.0f) * 8.0f *
                   sinf(posture.hpr[ROLL] * DEG_TO_RAD) * speed/MAX_SPEED) -
                   32.0f * cdata.rudder/speed)),360.0f);

         } // End if off the ground

      // Make sure we always display heading 0 - 360
      if (posture.hpr[HEADING] < 0.0f)
         {
         posture.hpr[HEADING] += 360.0f;
         }

      // Zero control data for the next frame
      cdata.pitch = cdata.roll = cdata.rudder = 0.0f;

      // Make sure we are still in bounds, if not, stop it at the boundary.
      if (at_a_boundary(posture.xyz))
         {
         posture.hpr[HEADING] =
              fmod(posture.hpr[HEADING] + 180.0f, 360.0f);
         pfNegateVec3(velocity,ZeroVec);
         } // end out of bounds

      else  // Still in bounds
         {
         // Calculate the new velocity vector

         // 4x4 matrix based on HPR of aircraft
         pfMakeEulerMat ( orient, posture.hpr[HEADING],
                                      posture.hpr[PITCH],
                                      posture.hpr[ROLL]);

         // Calculate directions of thrust, forward, and lateral vectors
         //  based on 4x4 matrix
         pfXformPt3 ( thrust_vec, up_dir, orient );
         pfXformPt3 ( fw_vec, direction, orient );
         pfXformPt3 ( lat_vec, right_dir, orient );

         // Normalize thrust, forward, and lateral vectors
         pfNormalizeVec3 ( thrust_vec );
         pfNormalizeVec3 ( fw_vec );
         pfNormalizeVec3 ( lat_vec );

         // Calculate weight vector of aircraft (straight down to ground)
         pfScaleVec3 ( weight_vec, weight_setting, down_dir );

         // Scale thrust, forward, and lateral vectors according to associated
         //  scalar values
         pfScaleVec3 ( thrust_vec, thrust_setting, thrust_vec );
         pfScaleVec3 ( fw_vec, fw_speed, fw_vec );
         pfScaleVec3 ( lat_vec, lat_speed, lat_vec );

         // Temporary vector of forward + lateral velocity vectors
         pfAddVec3 ( temp_vec, fw_vec, lat_vec );

         // Amount of force produced by power is thrust + weight vectors
         pfAddVec3 ( total_lift_vec, thrust_vec, weight_vec );

         // Scale our force according to the mass constant of the helo (a=F/m)
         pfScaleVec3 ( total_lift_vec, HELO_MASS, total_lift_vec );

         // Scale force vector by delta-time to get velocity
         pfScaleVec3 ( velocity, cdata.delta_time, total_lift_vec );

         // This velocity + temp velocity gives us our final velocity vector.
         pfAddVec3 ( velocity, temp_vec, velocity );
         } // end in bounds

      } // End !paused !tethered

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

   // Calculate our look vector
   pfMakeEulerMat ( orient, posture.hpr[HEADING],
                            posture.hpr[PITCH],
                            posture.hpr[ROLL]);

   pfXformPt3 ( up, up, orient );
   pfNormalizeVec3 ( up );
   pfScaleVec3 ( up, EYE_OFFSET, up);
   pfAddVec3 ( eyepoint, posture.xyz, up );
   pfCopyVec3 ( firepoint, eyepoint );

   altitude = ::get_altitude(posture.xyz) + bounding_box.min[PF_Z];

   // Can't go underground, so reset altitude if below ground
   if ( (!tethered) && (!G_static_data->stealth) && (altitude < -0.1) ) {
      posture.xyz[PF_Z] -= altitude;
   }

   check_targets ( posture.xyz, posture.hpr, 
                   look.hpr );

   //put the vehicle in the correct location in space
   pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
   pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);

#ifdef DEBUG_SCALE
   pfDCSScale(hull,2.0f);
#endif

   return(FINEWITHME);
} // end real helo move()


