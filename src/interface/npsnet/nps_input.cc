
#include <iostream.h>
#include <string.h>
#include "nps_input.h"
#include "manager.h"
#include "interface_const.h"
#include "communication_const.h"
#include "common_defs2.h"
#include "time_funcs.h"
#include "nps_smoke.h"
#include "effects.h"
#include "munitions.h"
#ifndef NOAHA
#include "aha.h"
#endif
#include "fcs.h"
#include "odt.h"

#ifndef NO_DVW
#define DVW_NPSNET
#define _CSSM
#define _DVW_VERBOSE_
#define _DVW_MULTI_TASKS_
#define _CC_COMPILER_
#define PERFORMER1_2
# include "cloud.h"
//# include "env_pdu.h"
# include "dvw_util.h"
extern "C" void deleteAllPlumes(void);
#endif


// Globals used by this library
extern volatile MANAGER_REC       *G_manager;
extern volatile DYNAMIC_DATA      *G_dynamic_data;
extern volatile STATIC_DATA       *G_static_data;
extern          VEH_LIST          *G_vehlist;
extern          VEHTYPE_LIST      *G_vehtype;
extern volatile int                G_drivenveh;
extern volatile float              G_curtime;


// Local defines
#define MOUSE_BUTTON_TIMER 0.5f


void
process_input_states ( pfPipeWindow *pipe, INFO_PDATA *info_pdata )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;

   pfGetPWinOrigin(pipe, (int *)&G_dynamic_data->originX,
                         (int *)&G_dynamic_data->originY);

   if ( l_input_manager != NULL ) {
      l_input_manager->query_button(counter,NPS_KEYBOARD,
                                    NPS_MOUSE_MIDDLE_BUTTON);
      if ( counter > 0 ) {
         static float middle_mouse_timer = 0.0f;
         if ( (G_curtime - middle_mouse_timer) > MOUSE_BUTTON_TIMER) {
            middle_mouse_timer = G_curtime;
            G_dynamic_data->pick_mode =
               (G_dynamic_data->pick_mode + 1) % MAX_PICK_MODES;
#ifndef NOAHA
            switch ( G_dynamic_data->pick_mode ) {
               case TWO_D_AHA_INFO:
               case TWO_D_AHA_PICK:
                  ahaSetMode(AHA_TWO_D);
                  break;
               case THREE_D_AHA_INFO:
               case THREE_D_AHA_PICK:
                  ahaSetMode(AHA_THREE_D);
                  break;
               default:
                  ahaSetMode(AHA_OFF);
               break;
            }
#endif
         }
      }

#ifndef NOAHA
     pfVec2 mouse_pos;
     switch ( G_dynamic_data->pick_mode ) {
        case TWO_D_AHA_INFO:
        case TWO_D_AHA_PICK:
        case THREE_D_AHA_INFO:
        case THREE_D_AHA_PICK:
           if ( l_input_manager->query_multival((void *)mouse_pos,
                                                NPS_KEYBOARD,
                                                NPS_MV_MOUSE_LOCATION) ) {
              ahaUpdate( mouse_pos[PF_X]-G_dynamic_data->originX,
                         mouse_pos[PF_Y]-G_dynamic_data->originY );
           }
           G_dynamic_data->nearest_radar_vid_to_mouse =
              ahaGetPreSelectedObjIndex();
           break;
        default:
           break;
     }
#endif

      switch ( G_dynamic_data->pick_mode ) {
#ifndef NOAHA
         case TWO_D_AHA_INFO:
         case THREE_D_AHA_INFO:
#endif
         case TWO_D_RADAR_INFO:
            l_input_manager->query_button(counter,NPS_KEYBOARD,
                                          NPS_MOUSE_LEFT_BUTTON);
            if ( counter > 0 ) {
               static float left_mouse_timer = 0.0f;
               if ( (G_curtime - left_mouse_timer) > MOUSE_BUTTON_TIMER) {
                  left_mouse_timer = G_curtime;
#ifndef NOAHA
                  if ( (G_dynamic_data->pick_mode == TWO_D_AHA_INFO) ||
                       (G_dynamic_data->pick_mode == THREE_D_AHA_INFO) ) {
                     ahaSelectObject();
                     G_dynamic_data->nearest_radar_vid_to_mouse =
                        ahaGetSelectedObjIndex();
                  }
#endif
                  if ( (G_dynamic_data->window_size != WIN_THREE_BUNKER) &&
                       (G_dynamic_data->window_size != WIN_TWO_TPORT) &&
                       (G_dynamic_data->window_size != WIN_FS_MCO) &&
                       (G_dynamic_data->window_size != WIN_TWO_ADJACENT) &&
                       (G_dynamic_data->window_size != WIN_THREE_TV) ) {
                     info_pdata->veh_index =
                        G_dynamic_data->nearest_radar_vid_to_mouse;
                  }
               }
            }
            l_input_manager->query_button(counter,NPS_KEYBOARD,
                                          NPS_MOUSE_RIGHT_BUTTON);
            if ( counter > 0 ) {
               static float right_mouse_timer = 0.0f;
               if ( (G_curtime - right_mouse_timer) > MOUSE_BUTTON_TIMER) {
                  right_mouse_timer = G_curtime;
#ifndef NOAHA
                  if ( (G_dynamic_data->pick_mode == TWO_D_AHA_INFO) ||
                       (G_dynamic_data->pick_mode == THREE_D_AHA_INFO) ) {
                     ahaDeselectObject();
                  }
#endif
                  if ( (G_dynamic_data->window_size != WIN_THREE_BUNKER) &&
                       (G_dynamic_data->window_size != WIN_TWO_TPORT) &&
                       (G_dynamic_data->window_size != WIN_TWO_ADJACENT) &&
                       (G_dynamic_data->window_size != WIN_FS_MCO) &&
                       (G_dynamic_data->window_size != WIN_THREE_TV) ) {
                     info_pdata->veh_index = -1;
                  }
               }
            }
            break;
         default:
            break;
      }
   }

}


void check_for_query ( const char *ether_interf,
                       const u_short port,
                       const int multicast,
                       const char *group,
                       const u_char ttl )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;
   int hours_expired;
   int minutes_expired;
   int seconds_expired;

   if ( (l_input_manager != NULL) &&
        (l_input_manager->verify_device(NPS_KEYBOARD)) ) {
      l_input_manager->query_button(counter,NPS_KEYBOARD,'?');
      if ( counter > 0 ) {
         static int entity_type_index;
         cerr.precision(2);
         cerr << endl
              << "+===============================================+"
              << endl;
         cerr << "+ Current Parameter Set-up:" << endl;
         cerr << "+" << endl;
         seconds_to_hms(G_curtime,
                        seconds_expired,minutes_expired,hours_expired);
         cerr << "+    System up-time:         "
              << hours_expired << " hours, "
              << minutes_expired << " minutes, "
              << seconds_expired << " seconds." << endl;
         cerr << "+" << endl;
         cerr << "+    DIS Exercise ID:        ";
         if ( (int)G_dynamic_data->exercise > 0 )
            cerr << (int)G_dynamic_data->exercise << endl;
         else
            cerr << "Send on 1, Receive all." << endl;
         cerr << "+    Ethernet Interface:     ";
         if ( (strcmp(ether_interf,"")) == 0)
            cerr << "Default" << endl;
         else
            cerr << ether_interf << endl;
         cerr << "+    UDP/IP Port:            "
              << (int)port << endl;
         if ( multicast )
            {
            cerr << "+    Network Mode:           "
                 << "Multicast" << endl;
            cerr << "+    Multicast Group:        "
                 << group << endl;
            cerr << "+    Multicast time-to-live: "
                 << (int)ttl << endl;
            }
         else
            cerr << "+    Network Mode:           "
                 << "Broadcast" << endl;
         cerr << "+    Coordinates:            ";
         if ( (strcmp((char *)G_static_data->round_world_file,
                     D_ROUND_WORLD_FILE)) == 0 )
            cerr << "SIMNET 'flat-world'." << endl;
         else
            {
            cerr << "DIS 'round-world' WGS84 coordinates,"
                 << endl;
            cerr << "+       Defined from file:   "
                 << (char *)G_static_data->round_world_file << endl;
            }
         cerr << "+    Network State:          ";
         switch ( G_dynamic_data->network_mode )
            {
            case NET_OFF:
               cerr << "OFF" << endl;
               break;
            case NET_SEND_ONLY:
               cerr << "SEND ONLY" << endl;
               break;
            case NET_RECEIVE_ONLY:
               cerr << "RECEIVE ONLY" << endl;
               break;
            case NET_SEND_AND_RECEIVE:
               cerr << "SEND and RECEIVE" << endl;
               break;
            default:
               cerr << "ERROR STATE" << endl;
               break;
            }
         cerr << "+" << endl;
         entity_type_index = G_vehlist[G_drivenveh].type;
         cerr << "+    Driven Vehicle:         "
              << G_vehtype[entity_type_index].name << endl;
         cerr << "+" << endl;
         cerr << "+    DIS Deactivate Timeout: ";
         cerr << G_static_data->DIS_deactivate_delta
              << " seconds." << endl;;
         cerr << "+    DIS Hearbeat Threshold: "
              << G_static_data->DIS_heartbeat_delta
              << " seconds." << endl;;
         cerr << "+    DIS Rotation Threshold: "
              << G_static_data->DIS_orientation_delta
              << " degrees." << endl;
         cerr << "+    DIS Position Threshold: "
              << G_static_data->DIS_position_delta
              << " meters." << endl;
         cerr << "+" << endl;
         cerr << "+===============================================+"
              << endl << endl;;
         }
      }
}


void open_input_devices ( int &use_basic_panel, const char *ether_interf )
{
   if ( G_manager->ofinput != NULL ) {

      G_manager->ofinput->manage(NPS_KEYBOARD);

//    if ( strlen((char *)G_static_data->fcs_port) != 0 ) {

      if ( strcasecmp((char *)G_static_data->fcs_port, "NONE") != 0 ) {
         cerr << "\tOpening FCS controls..." << endl;
         fcsInit fcs_tmp;
         strcpy (fcs_tmp.port_name, (const char*)G_static_data->fcs_port);
         fcs_tmp.multi = G_static_data->fcs_mp;
         G_static_data->fcs_exists =
            G_manager->ofinput->manage(NPS_FCS,(char *)&fcs_tmp);
            if ( !G_static_data->fcs_exists) {
            cerr << "Unable to open FCS on port "
                 << G_static_data->fcs_port << endl;
         }
         else {
            G_manager->ofinput->calibrate(NPS_STCK_HORIZONTAL,
                                          NPS_DEAD_ZONE, NPS_FCS, 0.03f);
            G_manager->ofinput->calibrate(NPS_STCK_VERTICAL,
                                          NPS_DEAD_ZONE, NPS_FCS, 0.03f);
            G_manager->ofinput->calibrate(NPS_RUDDER,
                                          NPS_DEAD_ZONE, NPS_FCS, 0.08f);
         }
      }
      else {
         G_static_data->fcs_exists = FALSE;
      }

//    if ( strlen((char *)G_static_data->kaflight_port) != 0 ) {

      if ( strcasecmp((char *)G_static_data->kaflight_port, "NONE") != 0 ) {
         cerr << "\tOpening KA Flight controls..." << endl;
         G_static_data->kaflight_exists =
            G_manager->ofinput->manage(NPS_KAFLIGHT,
                                    (char *)G_static_data->kaflight_port);
         if ( !G_static_data->kaflight_exists) {
            cerr << "Unable to open KAFLIGHT on port "
                 << G_static_data->kaflight_port << endl;
         }
         else {
            G_manager->ofinput->calibrate(NPS_STCK_HORIZONTAL,
                                          NPS_DEAD_ZONE, NPS_KAFLIGHT, 0.03f);
            G_manager->ofinput->calibrate(NPS_STCK_VERTICAL,
                                          NPS_DEAD_ZONE, NPS_KAFLIGHT, 0.03f);
         }
      }
      else {
         G_static_data->kaflight_exists = FALSE;
      }

//    if (( strlen((char *)G_static_data->flybox_port) != 0 ) && // kludge
      if  ( strcasecmp((char *)G_static_data->flybox_port, "NONE") != 0 ) {

         cerr << "\tOpening Flybox controls..." << endl;
         G_static_data->flybox_exists =
            G_manager->ofinput->manage(NPS_FLYBOX,
                                    (char *)G_static_data->flybox_port);
         if ( !G_static_data->flybox_exists) {
            cerr << "Unable to open BG FLYBOX on port "
                 << G_static_data->flybox_port << endl;
         }
      }
      else {
         G_static_data->flybox_exists = FALSE;
      }

//    if ( strlen((char *)G_static_data->leather_flybox_port) != 0 ) {

      if ( strcasecmp((char *)G_static_data->leather_flybox_port,
                                                         "NONE") != 0 ) {
         cerr << "\tOpening Leather Net Flybox controls..." << endl;
         G_static_data->leather_flybox_exists =
            G_manager->ofinput->manage(NPS_LEATHER_FLYBOX,
                                    (char *)G_static_data->leather_flybox_port);         if ( !G_static_data->leather_flybox_exists) {
            cerr << "Unable to open BG FLYBOX on port "
                 << G_static_data->leather_flybox_port << endl;
         }
      }
      else {
         G_static_data->leather_flybox_exists = FALSE;
      }

#ifndef NO_IPORT
      if ( strncmp ( (const char *)G_static_data->testport_file, "NONE", 4 ) ) {
         G_static_data->testport_exists =
         G_manager->ofinput->manage(NPS_TESTPORT,
                                    (char *)G_static_data->testport_file);
         if ( !G_static_data->testport_exists ) {
            cerr << "Unable to create Test Port." << endl;
         }
      }

      if ( strncmp ((const char *) G_static_data->uniport_file, "NONE", 4 ) ) {
//         cerr << "\tOpening Uni-Port controls system..." << endl;
         G_static_data->uniport_exists =
         G_manager->ofinput->manage(NPS_UNIPORT,
                                    (char *)G_static_data->uniport_file);
         if ( !G_static_data->uniport_exists ) {
            cerr << "Unable to open Uni-Port." << endl;
         }
      }

      if ( strncmp ((const char *) G_static_data->treadport_file, "NONE", 4 ) ) {
//         cerr << "\tOpening Tread-Port controls system..." << endl;
         G_static_data->treadport_exists =
         G_manager->ofinput->manage(NPS_TREADPORT,
                                    (char *)G_static_data->treadport_file);
         if ( !G_static_data->treadport_exists ) {
            cerr << "Unable to open Tread-Port." << endl;
         }
      }
#endif
#ifndef NOUPPERBODY
      if ( strncmp ((const char *) G_static_data->upperbody_file, "NONE", 4) ) {
         G_static_data->upperbody_exists =
         G_manager->ofinput->manage(NPS_UPPERBODY,
                                    (char *)G_static_data->upperbody_file);
         if ( !G_static_data->upperbody_exists ) {
            cerr << "Unable to open Upperbody Sensors." << endl;
         }
      }
#endif // NOUPPERBODY
      if ( strncmp ((const char *)G_static_data->fsInit.tty, "NONE", 4) &&
           strncmp ((const char *)G_static_data->fsInit.calfile, "NONE", 4)) {
         G_manager->ofinput->manage(NPS_FAKESPACE, 
                                    (char *)&(G_static_data->fsInit));
      }
      if ( strncmp ((const char *)G_static_data->odt_port, "NONE", 4)) {
         odtInit odt_tmp;
         strcpy (odt_tmp.port_name, (const char*)G_static_data->odt_port);
         odt_tmp.mp = G_static_data->odt_mp;
         G_manager->ofinput->manage(NPS_ODT,(const char *)&odt_tmp);
      }
      if ( strncmp ((const char *) G_static_data->lwInit.name, "NONE", 4) ) {
         if (G_static_data->lwInit.s_port == -1) {
            G_static_data->lwInit.s_port = G_static_data->lwInit.r_port -1;
         }
         G_manager->ofinput->manage(NPS_LWS, (char *)&(G_static_data->lwInit));
      }
#ifndef NOBASICPANEL
      if ( use_basic_panel ) {
         cerr << "\tOpen Basic Remote Panel communications..." << endl;
         G_static_data->remote_exists =
            G_manager->ofinput->manage(NPS_BASIC_PANEL,
                                       ether_interf);
         if ( !G_static_data->remote_exists ) {
            cerr << "Unable to open Panel communications on interface "
                 << ether_interf << endl;
         }
      }
      else {
         G_static_data->remote_exists = FALSE;
      }
#endif

#ifndef NONPSSHIP
      if ( G_dynamic_data->control == NPS_SHIP_PANEL ) {
         G_manager->ofinput->manage(NPS_SHIP_PANEL,
                                    ether_interf);
         cerr<<"\tOpening SHIP PANEL for input."<<endl;
      }
#endif
#ifndef NOSUB
      if ( G_dynamic_data->control == NPS_SUB_PANEL ) {
         G_manager->ofinput->manage(NPS_SUB_PANEL,
                                    ether_interf);
      }
#endif

   }
   else {
      G_static_data->fcs_exists = FALSE;
      G_static_data->kaflight_exists = FALSE;
      G_static_data->flybox_exists = FALSE;
      G_static_data->leather_flybox_exists = FALSE;
#ifndef NO_IPORT
      G_static_data->testport_exists = FALSE;
      G_static_data->uniport_exists = FALSE;
      G_static_data->treadport_exists = FALSE;
#endif
#ifndef NOBASICPANEL
      G_static_data->remote_exists = FALSE;
#endif
   }

#ifndef NOBASICPANEL
   if ( use_basic_panel ) {
      if ( !G_static_data->remote_exists ) {
         G_dynamic_data->control = NPS_KEYBOARD;
         cerr << "** Remote Panel requested as control but "
              << "could not be opened.\n";
         cerr << "**   Preceeding without them.\n";
         use_basic_panel = FALSE;
         }
      else {
         G_dynamic_data->control = NPS_BASIC_PANEL;
      }
   }
#endif


   if ( G_dynamic_data->control == NPS_FCS )
      {
      if (!G_static_data->fcs_exists)
         {
         G_dynamic_data->control = NPS_KEYBOARD;
         cerr << "** FCS requested as control but could not be opened.\n";
         cerr << "**   Preceeding without them.\n";
         }
      }

   if ( G_dynamic_data->control == NPS_KAFLIGHT)
      {
      if (!G_static_data->kaflight_exists)
         {
         G_dynamic_data->control = NPS_KEYBOARD;
         cerr << "** KAFLIGHT requested as control but could not be opened.\n";
         cerr << "**   Preceeding without them.\n";
         }
      }

   if ( G_dynamic_data->control == NPS_FLYBOX)
      {
      if (!G_static_data->flybox_exists)
         {
         G_dynamic_data->control = NPS_KEYBOARD;
         cerr << "** FLYBOX requested as control but could not be opened.\n";
         cerr << "**   Preceeding without them.\n";
         }
      }

   if ( G_dynamic_data->control == NPS_LEATHER_FLYBOX)
      {
      if (!G_static_data->leather_flybox_exists)
         {
         G_dynamic_data->control = NPS_KEYBOARD;
         cerr << "** Leather Net Flybox requested as control but could not be opened.\n";
         cerr << "**   Preceeding without them.\n";
         }
      }

}

void
check_for_reset ()
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;

   if ( (l_input_manager != NULL) &&
        (l_input_manager->verify_device(NPS_KEYBOARD)) ) {
      l_input_manager->query_button(counter,NPS_KEYBOARD,'r');
      if ( counter > 0 ) {
         dump_fire();
         dump_craters();
         dump_smoke();
         super_lazarus();
#ifndef NO_DVW
         cloudClean();
         deleteAllPlumes();
#endif
      }
   }

}


void
update_range_filter ( float &range )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;

   if ( (l_input_manager != NULL) &&
        (l_input_manager->verify_device(NPS_KEYBOARD)) ) {
      l_input_manager->query_button(counter,NPS_KEYBOARD,'z');
      if ( counter > 0 ) {
         range -= (counter*20.0f);
         if ( range < MIN_FILTER_RANGE ) {
            range = MIN_FILTER_RANGE;
         }
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'x');
      if ( counter > 0 ) {
         range += (counter*20.0f);
         if ( range > MAX_FILTER_RANGE ) {
            range = MAX_FILTER_RANGE;
         }
      }
   }

}

