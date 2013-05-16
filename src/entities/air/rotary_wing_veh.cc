
#include <math.h>
#include <string.h>

#include "rotary_wing_veh.h"
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
extern          pfGroup      *G_moving;

// Locals used by this library
static pfVec3 ZeroVec = {0.0f,0.0f,0.0f};

//********************************************************************//
//*************** class ROTARY_WING_VEH ******************************//
//********************************************************************//

ROTARY_WING_VEH::ROTARY_WING_VEH(int id,int atype, ForceID the_force)
:AIRCRAFT_VEH(id,atype,the_force)
{
   subpartscnt = 0;
   mainrotor = NULL;
   tailrotor = NULL;
   mrangle = 0.0f;
   trangle = 0.0f;
   mrrate = 2400.0f;
   trrate = 6000.0f;
}


void ROTARY_WING_VEH::del_vehicle()
//delete the object
{
  this->ROTARY_WING_VEH::~ROTARY_WING_VEH();
}


ROTARY_WING_VEH::~ROTARY_WING_VEH()
{
#if 0
int i;
//   pfDelete(mainrotor);
//   pfDelete(tailrotor);
//   cerr << "Rotor Wing destructor\n";
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
#endif
}


vtype ROTARY_WING_VEH::gettype()
{
   return (ROTARY_WING);
}

void ROTARY_WING_VEH::moverotorblades( float deltatime )
//move the rotorblades
{
   if ( num_joints > 0 )
      {
      mrangle = fmod ( (mrangle + (mrrate*deltatime)), 360.0f );
      pfDCSRot(joint[0].dcs, mrangle, 0.0f, 0.0f );
      }
   if ( num_joints > 1 )
      {
      trangle = fmod ( (trangle + (trrate*deltatime)), 360.0f );
      pfDCSRot(joint[1].dcs, 0.0f, trangle, 0.0f );
      }

}

movestat ROTARY_WING_VEH::move()
// move the vehicle for the next frame
{
   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   pfVec3 tempeye;
   float ground;
   pfVec3 oldpos;
   float speed_setting;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tROTARY_WING_VEH::move" << endl;
#endif

   update_time ();
   collect_inputs ();

   pfCopyVec3(oldpos,posture.xyz);

   speed_setting = cdata.thrust * MAX_ROTARY_SPEED;


#ifdef TRACE
   cerr << "Moving ROTARY_VEH\n";
#endif

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
      // Now see if we are going to hit something in this time interval
      if ( !G_static_data->stealth )
         {
         if ( check_collide( oldpos, posture.xyz,
                             posture.hpr,
                             speed, altitude) )
            return(HITSOMETHING);
         }

      // Set the new speed
      if ((input_control == NPS_FCS) || 
          (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_FLYBOX) ||
          (input_control == NPS_BASIC_PANEL))
         {
         if (speed < speed_setting)
            speed += FCS_SPEED_CHANGE;
         if (speed > speed_setting)
            speed -= FCS_SPEED_CHANGE;
         }
      else
         {
         // Not using the flight control throttle, so just add change
         speed += cdata.thrust;
         if (speed < KEY_SPEED_CHANGE)
            speed -= 1.0f;
         cdata.thrust = 0.0f;
         }
      // Don't want to exceed the max speed
      if (speed > MAX_ROTARY_SPEED)
         speed = MAX_ROTARY_SPEED;
      // Don't want to fly backwards
      if (speed < 0)
         speed = NO_SPEED;

      // DETERMINE THE NEW HEADING PITCH AND ROLL
      // If we are on the ground, we only want to be able to pitch up, and
      // our roll will be set by the terrain, and we steer with roll input

      /* only allow the vehicle to go up when on the ground */
      if ( (!G_static_data->stealth) && ( altitude < 0.1f )
            && (cdata.pitch < 0 ))
         {
         cdata.pitch = 0.0f;
         cdata.roll = 0.0f;
         }


      if ( (!G_static_data->stealth) && (altitude < 0.06f))
         // We are on the ground
         {
         grnd_orient(posture,&(bounding_box));
         if (cdata.pitch > 0.0f)
            posture.hpr[PITCH] += cdata.pitch;
         posture.hpr[HEADING] -= cdata.roll;
         }
      else  // We are not on the ground
         {
         if (!IS_ZERO(speed))
           posture.hpr[ROLL] =
                 fmod ( cdata.roll*2.0f + posture.hpr[ROLL], 360.0f);

         posture.hpr[PITCH] =
              fmod ( cosf(posture.hpr[ROLL]*DEG_TO_RAD) * cdata.pitch
                      + posture.hpr[PITCH], 180.0f);

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

         // Calculate the heading
         if (IS_ZERO(speed))
            posture.hpr[HEADING] -= cdata.roll;
         else
            posture.hpr[HEADING] = fmod (( posture.hpr[HEADING] -
                   ((cdata.pitch*2.0f+2.0f) * 8.0f *
                   sinf(posture.hpr[ROLL] *
                   DEG_TO_RAD) * speed/MAX_ROTARY_SPEED)),360.0f);

         } // End if off the ground

      // Make sure we always display heading 0 - 360
      if (posture.hpr[HEADING] < 0.0f)
         {
         posture.hpr[HEADING] += 360.0f;
         }

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

      if (altitude > G_dynamic_data->max_alt) {
         float altDelta = altitude - G_dynamic_data->max_alt;
         posture.xyz[PF_Z] = posture.xyz[PF_Z] - altDelta;
         altitude = altitude - altDelta;
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
//      G_dynamic_data->cause_of_death = UNDER_GROUND_DEATH
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

movestat ROTARY_WING_VEH::moveDR(float deltatime,float curtime)
{
  movestat result;
  result = VEHICLE::moveDR(deltatime,curtime);
  moverotorblades(deltatime);
  return(result);
}


