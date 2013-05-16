
#include <string.h>
#include <iostream.h>
#include "manager.h"
#include "common_defs2.h"
//#include "vehicle.h"
#include "ship_veh.h"
#include "munitions.h"
#include "terrain.h"
#include "interface_const.h"
#ifndef NOAHA
#include "aha.h"
#endif

// Globals used by this library
extern volatile MANAGER_REC       *G_manager;
extern volatile DYNAMIC_DATA      *G_dynamic_data;
extern volatile STATIC_DATA       *G_static_data;
extern volatile float              G_curtime;

void
SHIP_VEH::process_keyboard_speed_settings ()
{
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;
   int shift = 0;
   int control = 0;

   l_input_manager->query_button(counter,NPS_KEYBOARD,'s');
   if ( counter > 0 ) {
#ifndef NONPSSHIP
      // Order ship to all stop
      ordered_port_bell = 0.0f;
      ordered_stbd_bell = 0.0f;
      ordered_rpm = 999.0f;
      //keyboard_order = TRUE;
#else
      set_thrust ( 0.0f );
      pfSetVec3 ( velocity, 0.0f, 0.0f, 0.0f );
      pfSetVec3 ( acceleration, 0.0f, 0.0f, 0.0f );
      speed = 0.0f;
#endif
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'a');
   if ( counter > 0 ) {
#ifndef NONPSSHIP
      //Order engines ahead 1 bell
      if (ordered_port_bell < 4.5f)
         ordered_port_bell += 1.0f;
      if (ordered_stbd_bell < 4.5f)
         ordered_stbd_bell += 1.0f; 
      //Make the bells the same value
      if (ordered_stbd_bell != ordered_port_bell)
         ordered_stbd_bell = ordered_port_bell;
      ordered_rpm = 999.0f;
      //keyboard_order = TRUE;
#else
      change_thrust ( float(counter)*KEY_SPEED_CHANGE );
#endif
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,'d');
   if ( counter > 0 ) {
#ifndef NONPSSHIP
      //Order engines back 1 bell
      if (ordered_port_bell > -2.5f)
         ordered_port_bell -= 1.0f;
      if (ordered_stbd_bell > -2.5f)
         ordered_stbd_bell -= 1.0f; 
      //Make the bells the same value
      if (ordered_stbd_bell != ordered_port_bell)
         ordered_stbd_bell = ordered_port_bell; 
      ordered_rpm = 999.0f;
      //keyboard_order = TRUE;
#else
      change_thrust ( float(-counter)*KEY_SPEED_CHANGE );
#endif     
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_SHIFTKEY);
   shift = (counter > 0);
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_CONTROLKEY);
   control = (counter > 0);


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
#ifndef NONPSSHIP
      if (ordered_rudder_angle < 30.0f)
         ordered_rudder_angle += 5.0f;  //increase rudder 5 degrees
      //keyboard_order = TRUE;
#else
      change_horizontal ( float(counter*KEY_ROLL_CHANGE) );
#endif
   }
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_LEFTARROWKEY);
   if ( counter > 0 ) {
#ifndef NONPSSHIP
      if (ordered_rudder_angle > -30.0f)
         ordered_rudder_angle -= 5.0f;  //decrease rudder 5 degrees
      //keyboard_order = TRUE;
#else
      change_horizontal ( float(-counter*KEY_ROLL_CHANGE) );
#endif
   }
}//process_speed_settings()


void
SHIP_VEH::process_keyboard_input ()
{
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;
   int shift = 0;
   int control = 0;

   if (input_control == NPS_KEYBOARD) {
      process_keyboard_speed_settings();
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_SHIFTKEY);
   shift = (counter > 0);
   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_CONTROLKEY);
   control = (counter > 0);


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
   if ( counter > 0 ) {
      fire_weapon ( TERTIARY, counter );
   }

   class_specific_keyboard();

}


