

#include "stealth_veh.h"
#include "common_defs2.h"
#include "terrain.h"
#include "interface_const.h"
#ifndef NO_IPORT
#include "iportClass.h"
#include "DI_comm.h"
#endif // NO_IPORT
#include "manager.h"
#include "view.h"
#include "input_manager.h"
#include "diguy_veh.h"
#include <math.h>
#include "transportClass.h"

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern          VEH_LIST     *G_vehlist;
extern volatile  MANAGER_REC    *G_manager;

const float STEALTH_ZOOM = 1.0f;

static pfVec3 c_norm = {0.0f, 0.0f, 1.0f};
static pfVec3 ZeroVec = {0.0f, 0.0f, 0.0f};

STEALTH_VEH::STEALTH_VEH(int id,int atype, ForceID the_force)
:VEHICLE(id,atype,the_force)
{
   pfCopyVec3 (gun_pos.xyz, ZeroVec);
   pfCopyVec3 (gun_pos.hpr, ZeroVec);
   pfCopyVec3 (m_veh.xyz, ZeroVec);
   pfCopyVec3 (m_veh.hpr, ZeroVec);
}


int STEALTH_VEH::oktofire ( wdestype designation, float curtime )
   {
cerr << "Stealth cannot fire: " << int(designation) << " "
     << curtime << endl;
   return FALSE;
   }

vtype STEALTH_VEH::gettype()
{
  return (FLYING_CARPET);
}

movestat STEALTH_VEH::move()
{
   if ((tether_model_off != -1) && (tethered != 3)) {
      if (G_vehlist[tether_model_off].vehptr) {
         G_vehlist[tether_model_off].vehptr->turnon ();
      }
      tether_model_off = -1;
   }

   if (G_dynamic_data->useAuto) {
      int found = FALSE;
      if (cdata.vid != -1) {
         if (G_vehlist[cdata.vid].vehptr) {
            EntityID tempID = 
               G_vehlist[cdata.vid].vehptr->getdisname ();

            if ((tempID.address.site ==
                 G_dynamic_data->master.address.site) &&
                (tempID.address.host ==
                 G_dynamic_data->master.address.host) &&
                (tempID.entity ==
                 G_dynamic_data->master.entity)) {
               found = TRUE;
            }
         }
      }
      if (!found) {
         for (int ix = 0; ix < MAX_VEH; ix++) {
            if (G_vehlist[ix].vehptr) {
               EntityID tempID = 
                  G_vehlist[ix].vehptr->getdisname ();
               if ((tempID.address.site ==
                    G_dynamic_data->master.address.site) &&
                   (tempID.address.host ==
                    G_dynamic_data->master.address.host) &&
                   (tempID.entity ==
                    G_dynamic_data->master.entity)) {
                  cdata.vid = ix;
                  found = TRUE;
               }
            }
         }
      }

      if (found) {
         pfCopyVec3 (m_veh.xyz,
                     G_vehlist[cdata.vid].vehptr->getposition());
         pfCopyVec3 (m_veh.hpr,
                     G_vehlist[cdata.vid].vehptr->getorientation());
         G_vehlist[cdata.vid].vehptr->get_weapon_look (gun_pos);
         if (G_vehlist[cdata.vid].vehptr->gettype () == DI_GUY) {
            DIGUY_VEH *tempDI = (DIGUY_VEH *)G_vehlist[cdata.vid].vehptr;
            G_dynamic_data->target_lock = tempDI->get_target_lock ();
         }
         if (VEHICLE::vehAuto) {
            npsInfo *infoPacket = (npsInfo *) NULL;
            infoPacket = VEHICLE::vehAuto->find_info (m_veh.xyz);
            if (infoPacket) {
               npsTransportInfo *tInfo;
               pfCoord tPoint;
               if (tInfo = (npsTransportInfo *)infoPacket->is_transport ()) {
                  tInfo->get_mode (tethered);
                  tInfo->get_point (tPoint);
                  pfCopyVec3 (posture.hpr, tPoint.hpr);
                  if ((tPoint.xyz[PF_X] != posture.xyz[PF_X]) &&
                      (tPoint.xyz[PF_Y] != posture.xyz[PF_Y]) &&
                      (tPoint.xyz[PF_Z] != posture.xyz[PF_Y])) {
                     transport (tPoint.xyz[PF_X],
                                tPoint.xyz[PF_Y],
                                tPoint.xyz[PF_Z]);
                  }
               }
            }
         }
      }
   }

   input_manager *l_input_manager = G_manager->ofinput;

   pfMatrix orient;
   pfVec3 forward = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   pfVec3 side = { 1.0f, 0.0f, 0.0f };

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tSTEALTH_VEH::move" << endl;
#endif

   update_time ();
   collect_inputs ();

   if (tethered) {
      if ((cdata.vid > -1) && G_vehlist[cdata.vid].vehptr) {
         if (tethered == 1) {
             G_vehlist[cdata.vid].vehptr->teleport(posture,speed);
         }
         else if (tethered == 2) {
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
            speed = 0.0f;
            pfCopyVec3 (look.hpr, ZeroVec);
            pfCopyVec3 (m_veh.xyz, 
                        G_vehlist[cdata.vid].vehptr->getposition());
            pfCopyVec3 (posture.xyz, m_veh.xyz);
            pfCopyVec3 (m_veh.hpr,
                        G_vehlist[cdata.vid].vehptr->getorientation());
            pfCopyVec3 (posture.hpr, m_veh.hpr);
            G_vehlist[cdata.vid].vehptr->get_weapon_look (gun_pos);
            G_vehlist[cdata.vid].vehptr->turnoff();
            if (G_vehlist[cdata.vid].vehptr->gettype () == DI_GUY) {
               DIGUY_VEH *tempDI = (DIGUY_VEH *)G_vehlist[cdata.vid].vehptr;
               G_dynamic_data->target_lock = tempDI->get_target_lock ();
            }
            tether_model_off = cdata.vid;
         }
         else if (tethered == 4) {
            if (cdata.vid != -1) {
               if (G_vehlist[cdata.vid].vehptr) {

                  pfCopyVec3 (m_veh.xyz,  
                        G_vehlist[cdata.vid].vehptr->getposition());

                  pfCopyVec3 (m_veh.hpr,
                        G_vehlist[cdata.vid].vehptr->getorientation());
                  if (G_vehlist[cdata.vid].vehptr->gettype () == DI_GUY) {
                     DIGUY_VEH *tempDI = 
                        (DIGUY_VEH *)G_vehlist[cdata.vid].vehptr;
                     G_dynamic_data->target_lock = tempDI->get_target_lock ();
                  }

                  G_vehlist[cdata.vid].vehptr->get_weapon_look (gun_pos);
               }
            }
         }
         else if (tethered == 5) {
            if (cdata.vid != -1) {
               if (G_vehlist[cdata.vid].vehptr) {

                  pfCopyVec3 (m_veh.xyz,  
                              G_vehlist[cdata.vid].vehptr->getposition());
                  pfCopyVec3 (m_veh.hpr,
                              G_vehlist[cdata.vid].vehptr->getorientation());

                  if (G_vehlist[cdata.vid].vehptr->gettype () == DI_GUY) {
                     DIGUY_VEH *tempDI = 
                        (DIGUY_VEH *)G_vehlist[cdata.vid].vehptr;
                     G_dynamic_data->target_lock = tempDI->get_target_lock ();
                  }

                  G_vehlist[cdata.vid].vehptr->get_weapon_look (gun_pos);

                  if (((fabs(m_veh.xyz[PF_X] - posture.xyz[PF_X])) < 
                        STEALTH_ZOOM) &&
                      ((fabs(m_veh.xyz[PF_Y] - posture.xyz[PF_Y])) < 
                        STEALTH_ZOOM) &&
                      ((fabs(m_veh.xyz[PF_Z] - posture.xyz[PF_Z])) < 
                        STEALTH_ZOOM)) {
                      tethered = 3;
                  }
                  else {
                     pfCopyVec3 (look.hpr, ZeroVec);
                     pfCopyVec3 (look.xyz, ZeroVec);
                     compute_hpr (posture.xyz, m_veh.xyz, posture.hpr);
                     speed = 60;
                  }
               }
               else {
                  cdata.vid = -1;
               }
            }
         }
      }
      else {
         tethered = FALSE;
      }
   }
   if (!tethered || (tethered == 4) || (tethered == 5)) {
      posture.hpr[ROLL] = 0.0f;
      posture.hpr[PITCH] += cdata.pitch;

      if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_BASIC_PANEL) || (input_control == NPS_FLYBOX) ||
          (input_control == NPS_LEATHER_FLYBOX))
         {
         posture.hpr[HEADING] -= cdata.roll;
         if ( input_control == NPS_LEATHER_FLYBOX )
            speed += cdata.thrust;
         else
         speed = 250.0f * cdata.thrust;
         }
#ifndef NO_IPORT
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
         posture.hpr[PF_P] = 0.0f;
         posture.hpr[PF_R] = 0.0f;
      }
#endif // NO_IPORT
      else if (input_control == NPS_KEYBOARD) {
         posture.hpr[HEADING] -= (cdata.roll +
                                 (cdata.roll*(speed/25.0f))); // cdata.roll;
         speed += cdata.thrust;
         cdata.thrust = 0.0f;
         cdata.pitch = 0.0f;
         cdata.roll = 0.0f;
      }
      else {
         posture.hpr[HEADING] += cdata.heading;
      }
 /*
      if ( G_dynamic_data->camera )
         pfMakeEulerMat ( orient,
            posture.hpr[HEADING]+look.hpr[HEADING],
            posture.hpr[PITCH]+look.hpr[PITCH],
            posture.hpr[ROLL]+look.hpr[ROLL]);
      else
*/
         pfMakeEulerMat ( orient, posture.hpr[HEADING],
                                  posture.hpr[PITCH],
                                  posture.hpr[ROLL]);
  
      pfXformPt3 ( forward, forward, orient );
      pfNormalizeVec3 ( forward );
      pfXformPt3 ( side, side, orient );
      pfNormalizeVec3 ( side );
      pfXformPt3 ( up, up, orient );
      pfNormalizeVec3 ( up );


      if ( input_control == NPS_SPACEBALL )
         {
         pfScaleVec3 ( forward, cdata.thrust, forward );
         pfScaleVec3 ( up, cdata.elevthrust, up );
         pfScaleVec3 ( side, cdata.sidethrust, side );
         pfAddVec3 ( velocity, forward, side );
         pfAddVec3 ( velocity, velocity, up );
         speed = pfLengthVec3 ( velocity );
         }
      else if (input_control == NPS_TESTPORT) {
         posture.xyz[PF_Z] = 100.0f + gnd_level2 (posture.xyz);
      }
      else {
         pfScaleVec3 ( velocity, speed, forward);
      }

      posture.xyz[X] += velocity[X] * cdata.delta_time;
      posture.xyz[Y] += velocity[Y] * cdata.delta_time;
      posture.xyz[Z] += velocity[Z] * cdata.delta_time;

      if ((tethered == 4) && (cdata.vid != -1)) {
         if (G_vehlist[cdata.vid].vehptr) {
            pfVec3 hpr_result;
            compute_hpr_to_veh (posture.xyz, cdata.vid, hpr_result);
            pfSubVec3 (look.hpr, hpr_result, posture.hpr);
            look.hpr[PF_H] += 10.0f;
            G_dynamic_data->camera = TRUE;
         }
         else {
            cdata.vid = -1;
         }
      }



   }

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
   }
   altitude = ::get_altitude(posture.xyz);

   gun_pos.hpr[PF_R] = 0.0f;

   pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
   pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);

   thrust = cdata.thrust;

   return(FINEWITHME);

}

void
STEALTH_VEH::get_weapon_view (pfCoord &point)
{
   pfAddVec3 (point.xyz, m_veh.xyz, gun_pos.xyz);
   pfCopyVec3 (point.hpr, gun_pos.hpr);
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

movestat STEALTH_VEH::movedead()
   {
#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tSTEALTH_VEH::movedead" << endl;
#endif

   return this->move();
   }


movestat STEALTH_VEH::moveDR(float,float)
{
//   cerr << "Moving stealth:" << deltatime << " " << curtime << endl;
   return(FINEWITHME);
}

void
STEALTH_VEH::process_keyboard_speed_settings ()
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
      change_thrust ( float(counter)*STEALTH_KEY_SPEED_CHANGE );
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'A');
   if ( counter > 0 ) {
      change_thrust ( float(counter)*10);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'d');
   if ( counter > 0 ) {
      change_thrust ( float(-counter)*STEALTH_KEY_SPEED_CHANGE );
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'D');
   if ( counter > 0 ) {                                     
      change_thrust ( float(-counter)*10);
   }
}

