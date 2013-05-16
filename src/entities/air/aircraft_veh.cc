
#include <math.h>

#include "aircraft_veh.h"
#include "common_defs2.h"
#include "macros.h"
#include "dirt_intersect.h"
#include "entity.h"
#include "munitions.h"

#include "entity_const.h"
#include "collision_const.h"
#include "interface_const.h"
#include "conversion_const.h"

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          VEH_LIST     *G_vehlist;
extern          pfGroup      *G_fixed;


// Locals used by this library
static pfVec3 ZeroVec = {0.0f,0.0f,0.0f};



//************************************************************************//
//*************** class AIRCRAFT_VEH *************************************//
//************************************************************************//


AIRCRAFT_VEH::AIRCRAFT_VEH(int id,int atype, ForceID the_force)
:VEHICLE(id,atype,the_force)
{
#ifdef TRACE
   cerr << "Aircraft Constructor, veh no: " << id << endl;
#endif
}

AIRCRAFT_VEH::~AIRCRAFT_VEH()
{
}

vtype AIRCRAFT_VEH::gettype()
{
   return (AIR_VEH);
}


movestat AIRCRAFT_VEH::move()
// move the vehicle the next frame
{
   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   pfVec3 tempeye;
   float ground;
   float speed_setting = cdata.thrust * MAX_SPEED;
   //float thrust_setting = cdata.thrust * MAX_THRUST;
   pfVec3 oldpos;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tAIRCRAFT_VEH::move" << endl;
#endif

   update_time ();
   collect_inputs ();

   pfCopyVec3(oldpos,posture.xyz);

#ifdef TRACE
   cerr << "Moving AIRCRAFT_VEH\n";
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
      if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) || 
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
      if (speed > MAX_SPEED)
         speed = MAX_SPEED;
      // Don't want to fly backwards
      if (speed < 0)
         speed = NO_SPEED;

      // DETERMINE THE NEW HEADING PITCH AND ROLL
      // If we are on the ground, we only want to be able to pitch up, and
      // our roll will be set by the terrain, and we steer with roll input



      if ( (!G_static_data->stealth) && (altitude < 0.1f) )
         // We are on the ground
         {
         if (cdata.pitch <= 0.1f)
            cdata.pitch = 0.0f;
         grnd_orient(posture,&(bounding_box));
         if (cdata.pitch > 0.0f)
            posture.hpr[PITCH] += cdata.pitch;
         posture.hpr[HEADING] -= cdata.roll;
         }
      else  // We are not on the ground
         {
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
         posture.hpr[HEADING] = fmod (( posture.hpr[HEADING] -
                ((cdata.pitch*2.0f+2.0f) * sinf(posture.hpr[ROLL] *
                DEG_TO_RAD) * speed/MAX_SPEED)),360.0f);

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

   pfMakeEulerMat ( orient, posture.hpr[HEADING],
                            posture.hpr[PITCH],
                            posture.hpr[ROLL]);

   pfFullXformPt3 ( up, up, orient );
   pfNormalizeVec3 ( up );
   pfScaleVec3 ( up, EYE_OFFSET, up);
   pfAddVec3 ( eyepoint, posture.xyz, up );
   pfCopyVec3 ( firepoint, eyepoint );

   altitude = ::get_altitude(posture.xyz) +
                     bounding_box.min[PF_Z];
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



int AIRCRAFT_VEH::check_collide(pfVec3 &startpnt, pfVec3 &endpnt,
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
   pfMatrix Orient;
   pfVec3 tb_vec = { 0.0f, 0.0f, 1.0f };
   pfVec3 fb_vec = { 0.0f, 1.0f, 0.0f };
   int the_item_hit;

   if (int(pfEqualVec3(startpnt,endpnt))) 
      return FINEWITHME;

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
#ifdef TRACE
   cerr << "Chck Pt 2 Number of Kids " << pfGetNumChildren(G_moving) <<"\n";
#endif


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
         cout << "\n\nBuilding hit at " << posture.xyz[X] << " "
                                        << posture.xyz[Y] << " "
                                        << posture.xyz[Z] << endl;
#endif
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         G_dynamic_data->dead = DYING;
         G_dynamic_data->cause_of_death = S_OBJECT_DEATH;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         senddetonation(posture.xyz);
         return(HITSOMETHING);
         }
      }


   //******* intersect test against the fixed objects *********//
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

// cerr << "the_item_hit: " << (hex) << the_item_hit << endl;

      if ( ((the_item_hit == PERMANENT_DIRT) ||
            (the_item_hit == PERMANENT_BRIDGE)) &&
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

         //  Check to see if we are at a safe landing angle

         if ( ( ABS(posture.hpr[PITCH] - hpr[PITCH]) > SAFE_ANG ) ||
              ( ABS(posture.hpr[ROLL] - hpr[ROLL]) > SAFE_ANG ) )
            {
#ifdef DEBUG
            cout << "\n\nGround strike at " 
                 << result[hit_index].point[X] << " "
                 << result[hit_index].point[Y] << " "
                 << result[hit_index].point[Z] << endl;
#endif
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data ); 
#endif
            G_dynamic_data->dead = DYING;
            if (the_item_hit & PERMANENT_DIRT)
               G_dynamic_data->cause_of_death = GROUND_DEATH;
            else
               G_dynamic_data->cause_of_death = BRIDGE_DEATH;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
            return(HITSOMETHING);
            }   
//            else {
//               cerr << "Safe landing?" << endl;
//            }
         } // Hit the dirt or bridge 

      else if ( ( the_item_hit == PERMANENT_FOILAGE) || 
                ( the_item_hit == PERMANENT_WATER ) )
         { // Hit the water or foilage
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         G_dynamic_data->dead = DYING;
         if (the_item_hit == PERMANENT_FOILAGE)
            G_dynamic_data->cause_of_death = FOILAGE_DEATH;
         else
            G_dynamic_data->cause_of_death = WATER_DEATH;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         return(HITSOMETHING);
         } 

      else if ( (the_item_hit & PERMANENT_TUNNEL) &&  
                (flags & PFHIT_NORM) &&
                (flags & PFHIT_POINT) )
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

void air_hud_draw ( pfChannel *, void * )
   {
   cerr << "Air HUD draw called." << endl;
   }

int
AIRCRAFT_VEH::transport ( float new_x, float new_y, float new_alt )
{

   if ( (G_dynamic_data->bounding_box.xmin <= new_x) &&
        (new_x <= G_dynamic_data->bounding_box.xmax) &&
        (G_dynamic_data->bounding_box.ymin <= new_y) &&
        (new_y <= G_dynamic_data->bounding_box.ymax) ) {
      posture.xyz[PF_X] = new_x;
      posture.xyz[PF_Y] = new_y;
      posture.xyz[PF_Z] = 0.0f;
      posture.xyz[PF_Z] = new_alt + get_altitude();
      return TRUE;
   }
   return FALSE;
}

float
AIRCRAFT_VEH::get_altitude()
{
   float ground = 0.0f;
   float my_altitude;

   ground = ::get_altitude(posture.xyz);
   my_altitude = posture.xyz[Z] - ground + bounding_box.min[PF_Z];
   return my_altitude;
}

