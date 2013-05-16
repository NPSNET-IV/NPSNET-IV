
#include <string.h>
#include <iostream.h>
#include <math.h>
#include "manager.h"
#include "common_defs2.h"
#include "vehicle.h"
#include "munitions.h"
#include "terrain.h"
#include "interface_const.h"
#include "keyboard.h"
#ifndef NOAHA
#include "aha.h"
#endif

// Globals used by this library
extern          VEH_LIST          *G_vehlist;
extern volatile MANAGER_REC       *G_manager;
extern volatile DYNAMIC_DATA      *G_dynamic_data;
extern volatile STATIC_DATA       *G_static_data;
extern volatile float              G_curtime;

static float heading_correction = 0.0f;
static float pitch_correction = 0.0f;

void
VEHICLE::collect_inputs ()
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;
   int shift_pressed = 0;
   int control_pressed = 0;
#ifndef NOHIRESNET
   set_net_type (disNet);
#endif // NOHIRESNET
   jack_rifle_on ();

   

   if (l_input_manager != NULL) {

      if ( G_dynamic_data->dead == ALIVE ) {
         switch (input_control) {
            case NPS_SHIP_PANEL:
            case NPS_SUB_PANEL:
            case NPS_KEYBOARD:
               process_keyboard_input();
               break;
            case NPS_FCS:
               process_keyboard_input();
               process_fcs_input();
               break;
            case NPS_KAFLIGHT:
               process_keyboard_input();
               process_kaflight_input();
               break;
            case NPS_FLYBOX:
               process_keyboard_input();
               process_flybox_input();
               break;
            case NPS_LEATHER_FLYBOX:
               process_keyboard_input();
               process_leather_flybox_input();
               break;
            case NPS_SPACEBALL:
               process_keyboard_input();
               process_spaceball_input();
               break;
            case NPS_BASIC_PANEL:
               process_keyboard_input();
               process_basic_panel_input();
               break;
#ifndef NO_IPORT
            case NPS_TESTPORT:
               process_keyboard_input();
               process_testport_input ();
               break;
            case NPS_UNIPORT:
               process_keyboard_input();
#ifndef NOUPPERBODY
               process_upperbody_input ();
#endif // NOUPPERBODY
               process_uniport_input ();
               break;
#endif // NO_IPORT
#ifndef NOUPPERBODY
            case NPS_UPPERBODY:
               process_keyboard_input();
               process_upperbody_input ();
               break;
#endif // NOUPPERBODY
            case NPS_FAKESPACE:
               process_keyboard_input();
               process_fakespace_input();
               break;
            case NPS_LWS:
               process_keyboard_input();
               process_lws_input();
               break;
            case NPS_ODT:
               process_keyboard_input();
               process_odt_input();
               process_lws_input();
#ifndef NOUPPERBODY
               process_upperbody_input ();
#endif // NOUPPERBODY
            case NPS_NO_INPUT:
            default:
               break;
         }
      }

      if ( l_input_manager->verify_device(NPS_KEYBOARD) ) {
         l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_SHIFTKEY);
         shift_pressed = (counter > 0);
         l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_CONTROLKEY);
         control_pressed = (counter > 0);
      }

      process_state(shift_pressed,control_pressed);
      process_transport(shift_pressed,control_pressed);
      process_birdseye(shift_pressed,control_pressed);

      if ( G_dynamic_data->dead == ALIVE ) {
         process_input_control(shift_pressed,control_pressed);
         process_tether(shift_pressed,control_pressed);
         process_pause(shift_pressed,control_pressed);
         process_targetting(shift_pressed,control_pressed);
      }

   }
}

void
VEHICLE::process_keyboard_speed_settings ()
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
      change_thrust ( float(counter)*KEY_SPEED_CHANGE );
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'d');
   if ( counter > 0 ) {
      change_thrust ( float(-counter)*KEY_SPEED_CHANGE );
   }
}


void
VEHICLE::process_keyboard_input ()
{
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;
   int shift = 0;
   int control = 0;
   int alt = 0;

   process_keyboard_speed_settings();

   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_SHIFTKEY);
   shift = (counter > 0);
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_CONTROLKEY);
   control = (counter > 0);
   l_input_manager->query_button(alt, NPS_KEYBOARD,NPS_ALTKEY);


   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_UPARROWKEY);
   if ( counter > 0 ) {
      if (shift) {
         change_altitude ( float(counter) );
      }
      else {
         change_vertical ( float(counter*KEY_PITCH_CHANGE) );
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_DOWNARROWKEY);
   if ( counter > 0 ) {
      if (shift) {
         change_altitude ( float(-counter) );
      }
      else {
         change_vertical ( float(-counter*KEY_PITCH_CHANGE) );
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_RIGHTARROWKEY);
   if ( counter > 0 ) {
      change_horizontal ( float(counter*KEY_ROLL_CHANGE) );
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_LEFTARROWKEY);
   if ( counter > 0 ) {
      change_horizontal ( float(-counter*KEY_ROLL_CHANGE) );
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PADUPARROWKEY);
   if ( (counter > 0) && !control ) {
      change_gaze_vertical ( float(counter*KEY_PITCH_CHANGE) );
      G_dynamic_data->camera = TRUE;
      G_dynamic_data->camlock = FALSE;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PADDOWNARROWKEY);
   if ( (counter > 0) && !control ) {
      change_gaze_vertical ( float(-counter*KEY_PITCH_CHANGE) );
      G_dynamic_data->camera = TRUE;
      G_dynamic_data->camlock = FALSE;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PADRIGHTARROWKEY);
   if ( (counter > 0) && !control ) {
      change_gaze_horizontal ( float(-counter*KEY_ROLL_CHANGE) );
      G_dynamic_data->camera = TRUE;
      G_dynamic_data->camlock = FALSE;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PADLEFTARROWKEY);
   if ( (counter > 0) && !control ) {
      change_gaze_horizontal ( float(counter*KEY_ROLL_CHANGE) );
      G_dynamic_data->camera = TRUE;
      G_dynamic_data->camlock = FALSE;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PADCENTERKEY);
   if ( counter > 0 ) {
      set_gaze_horizontal ( 0.0f );
      set_gaze_vertical ( 0.0f );
      G_dynamic_data->camera = FALSE;
      G_dynamic_data->camlock = FALSE;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_DELETEKEY);
   if ( counter > 0 ) {
      pfSetVec3 ( look.xyz, 0.0f, 0.0f, 0.0f );
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_INSERTKEY);
   if ( counter > 0 ) {
      pfAddVec3 ( posture.xyz, posture.xyz, look.xyz );
      pfAddVec3 ( posture.hpr, posture.hpr, look.hpr );
      pfSetVec3 ( look.xyz, 0.0f, 0.0f, 0.0f );
      pfSetVec3 ( look.hpr, 0.0f, 0.0f, 0.0f ); 
      G_dynamic_data->camera = FALSE;
      G_dynamic_data->camlock = FALSE;
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_HOMEKEY);
   if ( counter > 0 ) {
      fire_weapon ( PRIMARY, counter );
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_ENDKEY);
   if ( counter > 0 ) {
      fire_weapon ( SECONDARY, counter );
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,' ');
   if ( !shift && (counter > 0) ) {
      fire_weapon ( TERTIARY, counter );
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'X');
   if ( counter > 0 ) {
      video_switch = !video_switch;
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,',');
   if ( counter > 0 ) {
      if (!tethered) {
         tethered = 3;
      }
      if (cdata.vid == -1) {
         cdata.vid = 0;
      }
      int current_vid = cdata.vid;
      int found = FALSE;
      cdata.vid--;
      if (cdata.vid < 0) {
         cdata.vid = MAX_VEH - 1;
      }
cerr << "current_vid:" << current_vid << endl;
      while (!found && (current_vid != cdata.vid)) {
         if ((G_vehlist[cdata.vid].vehptr) && (cdata.vid != vid) &&
            ((G_vehlist[cdata.vid].vehptr->type.entity_kind == 1) ||
             (G_vehlist[cdata.vid].vehptr->type.entity_kind == 3))) {
cerr << "found" << endl;
               found = TRUE;
         }
         else {
cerr << "Not found cdata.vid:" << cdata.vid <<  endl;
cerr << "Not found current_vid:" << current_vid << endl;
            cdata.vid--;
            if (cdata.vid < 0) {
               cdata.vid = MAX_VEH - 1;
            }
         }
      }
      if ((found == FALSE) ||
         !(G_vehlist[cdata.vid].vehptr) ||
          (cdata.vid == vid)) {
         tethered = 0;
         cdata.vid = -1;
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'.');
   if (counter > 0) {
      if (!tethered) {
         tethered = 3;
      }
      if (cdata.vid == -1) {
         cdata.vid = 0;
      }
      int current_vid = cdata.vid;
      int found = FALSE;
      cdata.vid = (cdata.vid + 1) % MAX_VEH;
//cerr << "current_vid:" << current_vid << endl;
      while (!found && (current_vid != cdata.vid)) {
         if ((G_vehlist[cdata.vid].vehptr) && (cdata.vid != vid) &&
            ((G_vehlist[cdata.vid].vehptr->type.entity_kind == 1) ||
             (G_vehlist[cdata.vid].vehptr->type.entity_kind == 3))) {
//cerr << "found" << endl;
            found = TRUE;
         }
         else {
//cerr << "Not found cdata.vid:" << cdata.vid <<  endl;
//cerr << "Not found current_vid:" << current_vid << endl;
            cdata.vid = (cdata.vid + 1) % MAX_VEH;
         }
      }
      if ((found == FALSE) ||
         !(G_vehlist[cdata.vid].vehptr) ||
          (cdata.vid == vid)) {
         tethered = 0;
         cdata.vid = -1;
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'^');
   if ( counter > 0 ) {
      G_dynamic_data->useAuto = !(G_dynamic_data->useAuto);
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'*');
   if ( counter > 0 ) {
      for (int ix = 0; ix < MAX_VEH; ix++) {
         if (G_vehlist[ix].vehptr) {
            if ((G_vehlist[ix].vehptr->disname.address.site ==
                 G_dynamic_data->master.address.site) &&
                (G_vehlist[ix].vehptr->disname.address.host ==
                 G_dynamic_data->master.address.host) &&
                (G_vehlist[ix].vehptr->disname.entity ==
                 G_dynamic_data->master.entity)) {
               tethered = 5;
               cdata.vid = ix;
            }
         }
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'&');
   if ( counter > 0 ) {
      for (int ix = 0; ix < MAX_VEH; ix++) {
         if (G_vehlist[ix].vehptr) {
            if ((G_vehlist[ix].vehptr->disname.address.site ==
                 G_dynamic_data->master.address.site) &&
                (G_vehlist[ix].vehptr->disname.address.host ==
                 G_dynamic_data->master.address.host) &&
                (G_vehlist[ix].vehptr->disname.entity ==
                 G_dynamic_data->master.entity)) {
               tethered = 4;
               cdata.vid = ix;
            }
         }
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'/');
   if (counter && (alt & LEFT_ALT_KEY_MASK) && 
       (alt & RIGHT_ALT_KEY_MASK)) {
      if (tethered && (cdata.vid != -1) && G_vehlist[cdata.vid].vehptr) {
         G_dynamic_data->master.address.site =
          G_vehlist[cdata.vid].vehptr->disname.address.site;
         G_dynamic_data->master.address.host =
          G_vehlist[cdata.vid].vehptr->disname.address.host;
         G_dynamic_data->master.entity =
          G_vehlist[cdata.vid].vehptr->disname.entity;
      }
   }
   else if ( counter > 0 ) {
      for (int ix = 0; ix < MAX_VEH; ix++) {
         if (G_vehlist[ix].vehptr) {
            if ((G_vehlist[ix].vehptr->disname.address.site == 
                 G_dynamic_data->master.address.site) && 
                (G_vehlist[ix].vehptr->disname.address.host == 
                 G_dynamic_data->master.address.host) && 
                (G_vehlist[ix].vehptr->disname.entity == 
                 G_dynamic_data->master.entity)) {
               tethered = 3;
               look.hpr[PF_H] = 0.0f;
               look.hpr[PF_P] = 0.0f;
               look.hpr[PF_R] = 0.0f;
               cdata.vid = ix;
            }
         }
      }
   }

   class_specific_keyboard();

}


void
VEHICLE::process_fcs_input ()
{
   int counter = 0;
   float value = 0.0f;
   NPS_SWITCH_VAL setting;
   input_manager *l_input_manager = G_manager->ofinput;

   l_input_manager->query_button(counter,NPS_FCS,
                                 NPS_STCK_TRIGGER,input_number);
     if ( counter > 0 ) {
      fire_weapon ( PRIMARY, counter );
   }
   l_input_manager->query_button(counter,NPS_FCS,
                                 NPS_STCK_TOP_BUTTON,input_number);
   if ( counter > 0 ) {
      fire_weapon ( SECONDARY, counter );
   }
   l_input_manager->query_button(counter,NPS_FCS,
                                 NPS_STCK_MID_BUTTON,input_number);
   if ( counter > 0 ) {
      fire_weapon ( TERTIARY, counter );
   }
   l_input_manager->query_valuator(value,NPS_FCS,
                                   NPS_STCK_HORIZONTAL,input_number);
   if ( value != 0.0f ) {
      set_horizontal(value);
   }
   l_input_manager->query_valuator(value,NPS_FCS,
                                   NPS_STCK_VERTICAL,input_number);
   if ( value != 0.0f ) {
      set_vertical(value);
   }
   l_input_manager->query_valuator(value,NPS_FCS,
                                   NPS_THRTL,input_number);
   l_input_manager->query_switch(setting,NPS_FCS,
                                 NPS_SWITCH_1,input_number);
   if ( (setting & NPS_SWITCH_UP) > 0 ) {
      set_thrust(value);
   }
   else if ( (setting & NPS_SWITCH_DOWN) > 0 ) {
      set_thrust(-value);
   }
   else {
      set_thrust(0.0f);
   }
   l_input_manager->query_valuator(value,NPS_FCS,
                                   NPS_RUDDER,input_number);
   set_rudder(value);
   l_input_manager->query_switch(setting,NPS_FCS,
                                 NPS_SWITCH_0,input_number);
   if ( setting & NPS_SWITCH_UP ) {
      change_gaze_vertical ( -2.0f );
   }
   if ( setting & NPS_SWITCH_DOWN ) {
      change_gaze_vertical ( 2.0f );
   }
   if ( setting & NPS_SWITCH_RIGHT ) {
      change_gaze_horizontal ( -2.0f );
   }
   if ( setting & NPS_SWITCH_LEFT ) {
      change_gaze_horizontal ( 2.0f );
   }




   class_specific_fcs();

}


void
VEHICLE::process_kaflight_input ()
{
   int counter = 0;
   float value = 0.0f;
   NPS_SWITCH_VAL setting = NPS_SWITCH_CENTER;
   input_manager *l_input_manager = G_manager->ofinput;
   int good_read;

   l_input_manager->query_button(counter,NPS_KAFLIGHT,
                                 NPS_STCK_TRIGGER,input_number);
   if ( counter > 0 ) {
      fire_weapon ( PRIMARY, counter );
   }
   l_input_manager->query_button(counter,NPS_KAFLIGHT,
                                 NPS_STCK_TOP_BUTTON,input_number);
   if ( counter > 0 ) {
      fire_weapon ( SECONDARY, counter );
   }
   l_input_manager->query_button(counter,NPS_KAFLIGHT,
                                 NPS_STCK_MID_BUTTON,input_number);
   if ( counter > 0 ) {
      fire_weapon ( TERTIARY, counter );
   }
   good_read = l_input_manager->query_valuator(value,NPS_KAFLIGHT,
                                               NPS_STCK_HORIZONTAL,
                                               input_number);
   if ( good_read ) {
      set_horizontal(value);
   }
   good_read = l_input_manager->query_valuator(value,NPS_KAFLIGHT,
                                               NPS_STCK_VERTICAL,
                                               input_number);
   if ( good_read ) {
      set_vertical(value);
   }
   l_input_manager->query_valuator(value,NPS_KAFLIGHT,
                                   NPS_THRTL,input_number);
   set_thrust(value);

   l_input_manager->query_switch(setting,NPS_KAFLIGHT,
                                 NPS_SWITCH_0,input_number);
   if ( setting & NPS_SWITCH_UP ) {
      change_gaze_vertical ( -2.0f );
   }
   if ( setting & NPS_SWITCH_DOWN ) {
      change_gaze_vertical ( 2.0f );
   }
   if ( setting & NPS_SWITCH_RIGHT ) {
      change_gaze_horizontal ( -2.0f );
   }
   if ( setting & NPS_SWITCH_LEFT ) {
      change_gaze_horizontal ( 2.0f );
   }

   class_specific_kaflight();

}


void
VEHICLE::process_flybox_input ()
{
   float value = 0.0f;
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;

   l_input_manager->query_button(counter,NPS_FLYBOX,
                                 NPS_STCK_TRIGGER,input_number);
     if ( counter > 0 ) {
      fire_weapon ( PRIMARY, counter );
   }
   l_input_manager->query_valuator(value,NPS_FLYBOX,
                                   NPS_STCK_HORIZONTAL,input_number);
   if ( value != 0.0f ) {
      set_horizontal(value);
   }
   l_input_manager->query_valuator(value,NPS_FLYBOX,
                                   NPS_STCK_VERTICAL,input_number);
   if ( value != 0.0f ) {
      set_vertical(-value);
   }
   l_input_manager->query_valuator(value,NPS_FLYBOX,
                                   NPS_STCK_TWIST,input_number);
   if ( value != 0.0f ) {
      change_gaze_horizontal(-value);
   }

   l_input_manager->query_valuator(value,NPS_FLYBOX,
                                   NPS_LEVER_RIGHT,input_number);
   l_input_manager->calibrate(NPS_STCK,NPS_DEAD_ZONE,NPS_FLYBOX,
                              value,input_number);
   l_input_manager->query_valuator(value,NPS_FLYBOX,
                                   NPS_LEVER_LEFT,input_number);
   set_thrust(value);


   l_input_manager->query_button(counter,NPS_FLYBOX,
                                 NPS_MOMLEFT_BUTTON,input_number);
   if ( counter > 0 ) {
      set_gaze_vertical(0.0f);
   }
   l_input_manager->query_button(counter,NPS_FLYBOX,
                                 NPS_MOMRIGHT_BUTTON,input_number);
   if ( counter > 0 ) {
      set_gaze_horizontal(0.0f);
   }

   class_specific_flybox();

}


void
VEHICLE::process_leather_flybox_input ()
{
   input_manager *l_input_manager = G_manager->ofinput;

   int back_button_pressed = FALSE;
   NPS_SWITCH_VAL setting;
   float value = 0.0f;
   int counter = 0;

   process_keyboard_speed_settings ();
   
   l_input_manager->query_switch(setting,NPS_LEATHER_FLYBOX,
                                 NPS_SWITCH_0,input_number);
   back_button_pressed = (setting == NPS_SWITCH_DOWN);

   l_input_manager->query_valuator(value,NPS_LEATHER_FLYBOX,
                                   NPS_STCK_HORIZONTAL,input_number);
   if ( back_button_pressed ) {
      change_gaze_horizontal(-value); 
   }
   else {
      change_horizontal(value);
   }

   l_input_manager->query_valuator(value,NPS_LEATHER_FLYBOX,
                                   NPS_STCK_VERTICAL,input_number);
   if ( back_button_pressed ) {
      change_gaze_vertical(-value);
   }
   else {
      change_vertical(-value);
   }
   l_input_manager->query_valuator(value,NPS_LEATHER_FLYBOX,
                                   NPS_STCK_TWIST,input_number);
   if ( back_button_pressed ) {
      change_gaze_horizontal(-value);
   }
   else {
      posture.hpr[HEADING] -= value;
   }

   l_input_manager->query_button(counter,NPS_LEATHER_FLYBOX,
                                 NPS_STCK_TRIGGER,input_number);
   if ( counter > 0 ) {
      fire_weapon ( PRIMARY, counter );
   }



}


void
VEHICLE::process_basic_panel_input()
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter;
   float value;

   l_input_manager->query_button(counter,NPS_BASIC_PANEL,
                                 NPS_FIRE1_BUTTON,input_number);
     if ( counter > 0 ) {
      fire_weapon ( PRIMARY, counter );
   }
   l_input_manager->query_button(counter,NPS_BASIC_PANEL,
                                 NPS_FIRE2_BUTTON,input_number);
   if ( counter > 0 ) {
      fire_weapon ( SECONDARY, counter );
   }
   l_input_manager->query_button(counter,NPS_BASIC_PANEL,
                                 NPS_FIRE3_BUTTON,input_number);
   if ( counter > 0 ) {
      fire_weapon ( TERTIARY, counter );
   }

   l_input_manager->query_valuator(value,NPS_BASIC_PANEL,
                                   NPS_STCK_HORIZONTAL,input_number);
   set_horizontal(value);

   l_input_manager->query_valuator(value,NPS_BASIC_PANEL,
                                   NPS_STCK_VERTICAL,input_number);
   set_vertical(value);

   l_input_manager->query_valuator(value,NPS_BASIC_PANEL,
                                   NPS_THRTL,input_number);
   set_thrust(value);
 
   class_specific_basic_panel();

}


void
VEHICLE::process_spaceball_input ()
{

   class_specific_spaceball();

}

#ifndef NO_IPORT

void
VEHICLE::process_testport_input ()
{
   input_manager *l_input_manager = G_manager->ofinput;

   pfVec3 vel;
   float  heading = 0;
   int    trigger = 0;

   l_input_manager->query_multival ( (void *) &vel, 
                                    NPS_TESTPORT, 
                                    NPS_MV_IPORT_VELOCITY);
   pfCopyVec3 (velocity, vel);

   l_input_manager->query_valuator (heading, NPS_TESTPORT,
                                    NPS_IPORT_TWIST);
   posture.hpr[PF_H] = heading;

   l_input_manager->query_button (trigger, NPS_TESTPORT,
                                  NPS_IPORT_TRIGGER);

   if (trigger > 0) {
      fire_weapon (PRIMARY, trigger);
   }

   class_specific_testport ();
}

void
VEHICLE::process_fakespace_input()
{
   int trigger = 0;
   static pfCoord tmpCoord;
   static pfCoord lastCoord;
   static pfCoord deltaCoord;

   input_manager *l_input_manager = G_manager->ofinput;

   l_input_manager->query_multival ( (void *) &tmpCoord, NPS_FAKESPACE,
                                    NPS_MV_FS_COORD);

   pfSubVec3 (deltaCoord.xyz, tmpCoord.xyz, lastCoord.xyz);
   pfScaleVec3 (deltaCoord.xyz, G_dynamic_data->fs_scale, deltaCoord.xyz);
   pfScaleVec3 (velocity, 1/cdata.delta_time, deltaCoord.xyz);

   speed = pfLengthVec3 (velocity);

// cerr << "SPEED: " << speed << endl;
// cerr << "H:     " << posture.hpr[PF_H] << endl;

//   pfAddVec3 (posture.xyz, posture.xyz, deltaCoord.xyz);

   pfCopyVec3 (posture.hpr, tmpCoord.hpr);

   pfCopyVec3 (lastCoord.xyz, tmpCoord.xyz);

   l_input_manager->query_button (trigger, NPS_FAKESPACE, NPS_FS_BUTTON_1);

   if (trigger) {
      fire_weapon (PRIMARY, trigger);
   }

   trigger = 0;

   l_input_manager->query_button (trigger, NPS_FAKESPACE, NPS_FS_BUTTON_2);

   if (trigger) {
      fire_weapon (SECONDARY, trigger);
   }

}

void
VEHICLE::process_lws_input ()
{
   input_manager *l_input_manager = G_manager->ofinput;
   int trigger;
   l_input_manager->query_button (trigger, NPS_LWS, NPS_LW_TRIGGER);
      if (trigger) {
cerr << "Caught Trigger Press" << endl;
         fire_weapon (PRIMARY, trigger);
      }
   l_input_manager->query_button (trigger, NPS_LWS, NPS_LW_LASER);
      if (trigger) {
cerr << "Caught Laser Press" << endl;
         lase ();
      }
   l_input_manager->query_button (trigger, NPS_LWS, NPS_LW_VIDEO);
      if (trigger) {
         //video_switch = !video_switch;
cerr << "Caught Video Switch: " << video_switch << endl;
      }
   class_specific_lws ();
}

void
VEHICLE::process_odt_input ()
{
   input_manager *l_input_manager = G_manager->ofinput;
   static pfVec3 odtVelocity;
   static float odtHeading;

   if (l_input_manager->query_multival ((void *) odtVelocity,
                                     NPS_ODT, NPS_MV_ODT_VELOCITY)) {

      pfCopyVec3 (velocity, odtVelocity);
      velocity[PF_Z] = 0.0f;
      speed = pfLengthVec3 (velocity);
   }
   else {
      cerr << "Error: in ODT, Unable to read velocity." << endl;
   }
   if (l_input_manager->query_valuator (odtHeading, NPS_ODT,
                                        NPS_ODT_HEADING)) {
      posture.hpr[PF_H] = odtHeading;
   }
   else {
      cerr << "Error: in ODT, Unable to read heading." << endl;
   }
      
// cerr << "posture.hpr[PF_H]: " << posture.hpr[PF_H] << endl;
// cerr << "velocity: " << velocity[PF_X] << "  " << velocity[PF_Y] << endl;

   
}

void
VEHICLE::process_uniport_input ()
{
   input_manager *l_input_manager = G_manager->ofinput;

   float  heading = 0;
   int    trigger = 0;
   int    trigger_aux = 0;

   l_input_manager->query_multival ( (void *) velocity,
                                    NPS_UNIPORT,
                                    NPS_MV_IPORT_VELOCITY);

   // Add velocity change here. Velocity is a pfVec3.

//   cerr << "Velocity from iport X: "
//        << velocity[X] << "  Y: "
//        << velocity[Y] << "  Z: "
//        << velocity[Z] << endl;

   l_input_manager->query_multival ( (void *) look.hpr,
                                     NPS_UNIPORT,
                                     NPS_MV_IPORT_LOOK);

   l_input_manager->query_valuator (heading, NPS_UNIPORT,
                                    NPS_IPORT_TWIST);

   posture.hpr[PF_H] = heading;

   look.hpr[0] -= heading;
   
//   cerr << "Heading: " << posture.hpr[PF_H] << endl;

   l_input_manager->query_button (trigger, NPS_UNIPORT,
                                  NPS_IPORT_TRIGGER);
   l_input_manager->query_button (trigger_aux, NPS_UNIPORT,
                                  NPS_IPORT_TRIGGER_AUX);
   
   int temp_count = trigger - old_trigger;

   if ((temp_count <= 5) && (temp_count > 0)) {
      for (int ix = 0; ix <= temp_count; ix++) {
         fire_weapon (PRIMARY, trigger);
      }
   }

   old_trigger = trigger;

   temp_count = trigger_aux - old_trigger_aux;

   if ((temp_count <= 5) && (temp_count > 0)) {
      for (int ix = 0; ix <= temp_count; ix++) {
         fire_weapon (SECONDARY, trigger_aux);
      }
   }

   old_trigger_aux = trigger_aux;

   class_specific_uniport ();
}

#endif // NO_IPORT

#ifndef NOUPPERBODY

void
VEHICLE::process_upperbody_input ()
{
   class_specific_upperbody ();
}

#endif // NOUPPERBODY


void
VEHICLE::change_thrust ( const float amount )
{
   cdata.thrust += amount;
}


void
VEHICLE::set_thrust ( const float amount )
{
   cdata.thrust = amount;
}


void
VEHICLE::change_rudder ( const float amount )
{
   cdata.rudder += amount;
}


void
VEHICLE::set_rudder ( const float amount )
{
   cdata.rudder = amount;
}


void
VEHICLE::change_horizontal ( const float amount )
{
   if ( tethered == 2 ) {
      posture.hpr[PF_H] += amount;
   }
   else {
      cdata.roll += amount;
   }
}


void
VEHICLE::set_horizontal ( const float amount )
{
   cdata.roll = amount;
}


void
VEHICLE::change_vertical ( const float amount )
{
   if ( tethered == 2 ) {
      posture.hpr[PF_P] += amount;
   }
   else {
      cdata.pitch += amount;
   }
}


void
VEHICLE::set_vertical ( const float amount )
{
   cdata.pitch = amount;
}


void
VEHICLE::change_altitude ( const float amount )
{
   posture.xyz[Z] += amount;
}


void
VEHICLE::set_altitiude ( const float amount )
{
   float current_alt;

   current_alt = ::get_altitude(posture.xyz);
   posture.xyz[Z] += (amount - current_alt);

}


void
VEHICLE::change_gaze_vertical ( const float amount )
{
   look.hpr[PITCH] += amount;
   G_dynamic_data->camera = TRUE;
   G_dynamic_data->camlock = FALSE;
}


void
VEHICLE::set_gaze_vertical ( const float amount )
{
   look.hpr[PITCH] = amount;
}


void
VEHICLE::change_gaze_horizontal ( const float amount )
{
   look.hpr[HEADING] += amount;
   G_dynamic_data->camera = TRUE;
   G_dynamic_data->camlock = FALSE;

}


void
VEHICLE::set_gaze_horizontal ( const float amount )
{
   look.hpr[HEADING] = amount;
}


void
VEHICLE::fire_weapon ( const wdestype weapon_num, const int /* num_times */ )
{
   input_manager *l_input_manager = G_manager->ofinput;
   if ( (input_control == NPS_UPPERBODY) || (input_control == NPS_UNIPORT) ) {
      if (l_input_manager->verify_device (NPS_UPPERBODY) ) {
         pfCoord gun_look;
         l_input_manager->query_multival ( (void *) &gun_look,
                                          NPS_UPPERBODY,
                                          NPS_MV_TARGET_RIFLE);

         gun_look.hpr[PF_H] += heading_correction;
         gun_look.hpr[PF_P] += pitch_correction;

         firebuttonhit (weapon_num, vid, posture, gun_look);
      }
      else {
         firebuttonhit ( weapon_num, vid, posture, look );
      }
   }
   else {
      firebuttonhit ( weapon_num, vid, posture, look );
   }
}


int
VEHICLE::set_input_control ( const NPS_Input_Device new_control,
                             const int new_number )
{
   input_manager *l_input_manager = G_manager->ofinput;

   if ( (l_input_manager != NULL) &&
        l_input_manager->verify_device(new_control,new_number) ) {
      input_control = new_control;
      input_number = new_number;
      return TRUE;
   }
   else {
      return FALSE;
   }
}


void
VEHICLE::cycle_input_control ()
{
   input_manager *l_input_manager = G_manager->ofinput;

   if ( l_input_manager != NULL ) {
      if ( !l_input_manager->get_next_device(input_control, input_number) ) {
         input_control = NPS_NO_INPUT;
         input_number = 0;
      }
   }

}


void
VEHICLE::process_pause( int, int )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;

   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PAUSEKEY);
   if ( counter > 0 ) {
      static float old_speed;
      static pfVec3 old_velocity;
      paused = !paused;
      if ( paused ) {
         old_speed = speed;
         pfCopyVec3 ( old_velocity, velocity );
         speed = 0.0f;
         pfSetVec3(velocity,0.0f,0.0f,0.0f);
         sendentitystate();  
         l_input_manager->calibrate(NPS_OPERATION,NPS_PAUSE,
                                    input_control,input_number);
      }
      else {
         speed = old_speed;
         pfCopyVec3 ( velocity, old_velocity );
         sendentitystate();
         l_input_manager->calibrate(NPS_OPERATION,NPS_RESUME,
                                    input_control,input_number);
      }
   }

}


void
VEHICLE::process_transport ( int shift_pressed, int control_pressed )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int transport_num = -1;
   int new_transport = -1;
   int counter = 0;

   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F9KEY);
   if ( counter > 0 ) {
      if ( !shift_pressed && !control_pressed ) {
         if ( paused ) {
            new_transport = 0;
         }
         else {
            transport_num = 0;
         }
      }
      else if ( shift_pressed && !control_pressed ) {
         if ( paused ) {
            new_transport = 4;
         }
         else {
            transport_num = 4;
         }
      }
      else if ( !shift_pressed && control_pressed ) {
         if ( paused ) {
            new_transport = 8;
         }
         else {
            transport_num = 8;
         }
      }
      else {
         if ( paused ) {
            new_transport = 12;
         }
         else {
            transport_num = 12;
         }
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F10KEY);
   if ( counter > 0 ) {
      if ( !shift_pressed && !control_pressed ) {
         if ( paused ) {
            new_transport = 1;
         }
         else {
            transport_num = 1;
         }
      }
      else if ( shift_pressed && !control_pressed ) {
         if ( paused ) {
            new_transport = 5;
         }
         else {
            transport_num = 5;
         }
      }
      else if ( !shift_pressed && control_pressed ) {
         if ( paused ) {
            new_transport = 9;
         }
         else {
            transport_num = 9;
         }
      }
      else {
         if ( paused ) {
            new_transport = 13;
         }
         else {
            transport_num = 13;
         }
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F11KEY);
   if ( counter > 0 ) {
      if ( !shift_pressed && !control_pressed ) {
         if ( paused ) {
            new_transport = 2;
         }
         else {
            transport_num = 2;
         }
      }
      else if ( shift_pressed && !control_pressed ) {
         if ( paused ) {
            new_transport = 6;
         }
         else {
            transport_num = 6;
         }
      }
      else if ( !shift_pressed && control_pressed ) {
         if ( paused ) {
            new_transport = 10;
         }
         else {
            transport_num = 10;
         }
      }
      else {
         if ( paused ) {
            new_transport = 14;
         }
         else {
            transport_num = 14;
         }
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F12KEY);
   if ( counter > 0 ) {
      if ( !shift_pressed && !control_pressed ) {
         if ( paused ) {
            new_transport = 3;
         }
         else {
            transport_num = 3;
         }
      }
      else if ( shift_pressed && !control_pressed ) {
         if ( paused ) {
            new_transport = 7;
         }
         else {
            transport_num = 7;
         }
      }
      else if ( !shift_pressed && control_pressed ) {
         if ( paused ) {
            new_transport = 11;
         }
         else {
            transport_num = 11;
         }
      }
      else {
         if ( paused ) {
            new_transport = 15;
         }
         else {
            transport_num = 15;
         }
      }
   }


   if ((0 <= transport_num) && (transport_num < transport_data.current_count)) {
      pfSetVec3 ( look.xyz, 0.0f, 0.0f, 0.0f );
      pfSetVec3 ( look.hpr, 0.0f, 0.0f, 0.0f );
      transport ( transport_data.location[transport_num].xyz[X],
                  transport_data.location[transport_num].xyz[Y],
                  transport_data.location[transport_num].xyz[Z] );
      pfCopyVec3 ( posture.hpr, transport_data.location[transport_num].hpr );
      speed = 0.0f;
      pfSetVec3 ( velocity, 0.0f, 0.0f, 0.0f );
   }

   if ((0 <= new_transport) && (new_transport < MAX_TRANSPORT) ) { 
      transport_data.location[new_transport] = posture;
      transport_data.location[new_transport].xyz[Z] = this->get_altitude();
      strcpy ( transport_data.comment[new_transport],
               " // Added during runtime - change comment" );
      if ( transport_data.current_count < (new_transport+1) ) {
         transport_data.current_count = new_transport+1;
      }
      transport_data.changed = TRUE;
   }

}


void
VEHICLE::process_state ( int shift_pressed, int control_pressed )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;


   if ( l_input_manager->verify_device(NPS_KEYBOARD) ) {
      l_input_manager->query_button(counter,NPS_KEYBOARD,BACKSPACE_ASCII);
      // Backspace key
      if ( counter > 0 ) {
         if ( control_pressed && !shift_pressed ) {
            G_static_data->covert = !G_static_data->covert;
         }
         if ( shift_pressed && !control_pressed ) {
            G_dynamic_data->dead = ALIVE;
            changestatus(0x0);
            pfSetVec3 ( look.xyz, 0.0f, 0.0f, 0.0f );
            pfSetVec3 ( look.hpr, 0.0f, 0.0f, 0.0f );
            posture.hpr[PF_P] = 0.0f;
            posture.hpr[PF_R] = 0.0f;
            speed = 0.0f;
            pfSetVec3 ( velocity, 0.0f, 0.0f, 0.0f );

         }
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'B');
      if ( counter > 0 ) {
         toggle_bbox();
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'Q');
      if ( counter > 0 ) {
         quiet_switch = !(quiet_switch);
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'H');
      if ( counter > 0 ) {
         heading_correction += 0.25f;
         cerr << "\theading_correction: " << heading_correction << endl
              << "\tpitch_correction:   " << pitch_correction << endl;
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'L');
      if ( counter > 0 ) {
         heading_correction -= 0.25f;
         cerr << "\theading_correction: " << heading_correction << endl
              << "\tpitch_correction:   " << pitch_correction << endl;
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'K');
      if ( counter > 0 ) {
         pitch_correction += 0.25f;
         cerr << "\theading_correction: " << heading_correction << endl
              << "\tpitch_correction:   " << pitch_correction << endl;
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'J');
      if ( counter > 0 ) {
         pitch_correction -= 0.25f;
         cerr << "\theading_correction: " << heading_correction << endl
              << "\tpitch_correction:   " << pitch_correction << endl;
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'G');
      if ( counter > 0 ) {
         pitch_correction = 0.0f;
         heading_correction = 0.0f;
         cerr << "Reseting." << endl
              << "\theading_correction: " << heading_correction << endl
              << "\tpitch_correction:   " << pitch_correction << endl;
      }
   }
}


void
VEHICLE::process_input_control ( int shift_pressed, int )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;


   if ( l_input_manager->verify_device(NPS_KEYBOARD) ) {
      l_input_manager->query_button(counter,NPS_KEYBOARD,ENTER_ASCII);
      if ( (counter > 0) && shift_pressed ) {
         l_input_manager->calibrate(NPS_STCK,NPS_RESTING_STATE,
                                    input_control,input_number);
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'i');
      if ( counter > 0 ) {
         cycle_input_control();
      }
   }

}


void
VEHICLE::process_birdseye ( int, int )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;

   if ( l_input_manager->verify_device(NPS_KEYBOARD) ) {

      l_input_manager->query_button(counter,NPS_KEYBOARD,'6');
      if (counter > 0) {
         G_dynamic_data->birdseye =
         birds_eye_view(G_dynamic_data->birdseye);
      }

      if ( G_dynamic_data->birdseye ) {
         l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PAGEDOWNKEY);
         G_dynamic_data->birdsdis -= (2.0f * counter);
         if ( G_dynamic_data->birdsdis < 2.5f ) {
            G_dynamic_data->birdsdis = 2.5f;
         }
         l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PAGEUPKEY);
         G_dynamic_data->birdsdis += (2.0f * counter);
         if ( G_dynamic_data->birdsdis > 250.0f ) {
            G_dynamic_data->birdsdis = 250.0f;
         }
      }
         
   }

}


void
VEHICLE::process_tether ( int shift_pressed, int )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int left_counter = 0;
   int right_counter = 0;
   int counter = 0;

   switch ( G_dynamic_data->pick_mode ) {
      case TWO_D_RADAR_PICK:
#ifndef NOAHA
      case TWO_D_AHA_PICK:
      case THREE_D_AHA_PICK:
#endif

         l_input_manager->query_button(left_counter,NPS_KEYBOARD,
                                       NPS_MOUSE_LEFT_BUTTON);
         l_input_manager->query_button(right_counter,NPS_KEYBOARD,
                                       NPS_MOUSE_RIGHT_BUTTON);

         if (left_counter) {
            static float left_mouse_timer = 0.0f;
            if ( (G_curtime - left_mouse_timer) > 0.5f ) {
               left_mouse_timer = G_curtime;
            }
            else {
               left_counter = FALSE;
            }
         }

         if (right_counter) {
            static float right_mouse_timer = 0.0f;
            if ( (G_curtime - right_mouse_timer) > 0.5f ) {
               right_mouse_timer = G_curtime;
            }
            else {
               right_counter = FALSE;
            }
         }

         if (left_counter && !right_counter) {
#ifndef NOAHA
            if ( (G_dynamic_data->pick_mode == TWO_D_AHA_PICK) ||
                 (G_dynamic_data->pick_mode == THREE_D_AHA_PICK) ) {
               ahaSelectObject();
               G_dynamic_data->nearest_radar_vid_to_mouse =
                  ahaGetSelectedObjIndex();
            }
#endif
            cdata.vid = G_dynamic_data->nearest_radar_vid_to_mouse;
            if ( shift_pressed || (cdata.vid == -1) ) {
               tethered = 0;
            }
            else {
               tethered = 1;
            }
         }
         else if (right_counter && !left_counter) {
#ifndef NOAHA
            if ( (G_dynamic_data->pick_mode == TWO_D_AHA_PICK) ||
                 (G_dynamic_data->pick_mode == THREE_D_AHA_PICK) ) {
               ahaSelectObject();
               G_dynamic_data->nearest_radar_vid_to_mouse =
                  ahaGetSelectedObjIndex();
            }
#endif
            cdata.vid = G_dynamic_data->nearest_radar_vid_to_mouse;
            if ( shift_pressed || (cdata.vid == -1) ) {
               tethered = 0;
            }
            else {
               tethered = 2;
            }
         }
         else if (right_counter && left_counter) {
            cdata.vid = G_dynamic_data->nearest_radar_vid_to_mouse;
            if ( shift_pressed || (cdata.vid == -1) ) {
               tethered = 0;
            }
            else {
               tethered = 3;
            }
         }
         break;
      default:
         break;
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PAGEUPKEY);
   if ( tethered == 1 ) {
      look.xyz[Z] += (KEY_PITCH_CHANGE*counter);
      G_dynamic_data->camera = TRUE;
      G_dynamic_data->camlock = FALSE;
   }
   else if ( tethered == 2 ) {
      G_dynamic_data->attach_dis = G_dynamic_data->attach_dis + counter;
      if ( G_dynamic_data->attach_dis > 250.0f ) {
         G_dynamic_data->attach_dis = 250.0f;
      }
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PAGEDOWNKEY);
   if ( tethered == 1 ) {
      look.xyz[Z] -= (KEY_PITCH_CHANGE*counter);
      G_dynamic_data->camera = TRUE;
      G_dynamic_data->camlock = FALSE;
   }
   else if ( tethered == 2 ) {
      G_dynamic_data->attach_dis = G_dynamic_data->attach_dis - counter;
      if ( G_dynamic_data->attach_dis < 2.5f ) {
         G_dynamic_data->attach_dis = 2.5f;
      }
   }

}


void
VEHICLE::process_targetting ( int, int )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;

   l_input_manager->query_button(counter,NPS_KEYBOARD,'4');
   if ( counter > 0 ) {
      targetting = !targetting;
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'$');
   if ( counter > 0 ) {
      G_dynamic_data->use_bounding_boxes = !G_dynamic_data->use_bounding_boxes;
/*
      cerr << "Bounding boxes toggled ";
      if ( G_dynamic_data->use_bounding_boxes ) {
         cerr << "ON." << endl;
      }
      else {
         cerr << "OFF." << endl;
      }
*/
   }

}
