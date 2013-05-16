
#include "string.h"
#include "leather_flybox.h"

#define JOYFACTOR_H     -2
#define JOYFACTOR_S     -5
#define JOYFACTOR_P     -2
#define JOY_HORIZ       0
#define JOY_VERT        1
#define JOY_TWIST       2

leather_flybox::leather_flybox ( const char *port )
{
      j_init = new JoystickInit;
      strcpy ( comm_port, port );
      j_init->port =  comm_port;
      valid_data = FALSE;
      j_exists = joystick_init (j_init);
      if ( j_exists ) {
         j_data = new JoystickData;
         joystick_reset(j_init);
      }
      else {
         j_data = NULL;
         delete j_init;
         j_init = NULL;
      }
}


leather_flybox::~leather_flybox ()
{
   if ( j_exists ) {
      if ( j_data != NULL ) {
         delete j_data;
         j_data = NULL;
      }
      if ( j_init != NULL ) {
         delete j_init;
         j_init = NULL;
      }
      joystick_exit();
      j_exists = FALSE;
   }

}


void
leather_flybox::get_all_inputs ( pfChannel *)
{
   valid_data = (j_exists && joystick_acquire (j_data));
}


int
leather_flybox::exists ()
{
   return this->j_exists;
}


int
leather_flybox::button_pressed ( const NPS_BUTTON button_num, int &num_presses )
{ 
   int success = FALSE;
   num_presses = 0;

   if ( valid_data ) {
      success = TRUE;
      switch ( button_num ) {
         case NPS_STCK_TRIGGER:
            // Two cases - first line is back button(switch) depressed (down)
            //             second line is back button released (up)
            num_presses = 
               ( ((j_data->digital > 65015) && (j_data->digital < 65035)) ||
                 ((j_data->digital > 65525) && (j_data->digital < 65545)) );

            break;
         default:
            num_presses = 0;
            success = FALSE;
            break;
      }
   }

   return success;
}


int
leather_flybox::valuator_set ( const NPS_VALUATOR valuator_num, float &value )
{
   int success = FALSE;

   if ( valid_data ) {
      success = TRUE;
      switch ( valuator_num ) {
         case NPS_STCK_HORIZONTAL:
            value = j_data->analog[JOY_HORIZ];
            break;
         case NPS_STCK_VERTICAL:
            value = j_data->analog[JOY_VERT];
            break;
         case NPS_STCK_TWIST:
            value = j_data->analog[JOY_TWIST];
            break;
         default:
            value = 0.0f;
            success = FALSE;
            break;
      }
   }
   return success;

}


int
leather_flybox::switch_set ( const NPS_SWITCH switch_num,
                             NPS_SWITCH_VAL &value )
{
   int success = FALSE;

   if ( valid_data ) {
      switch ( switch_num ) {
         case NPS_SWITCH_0:
            if ( ((j_data->digital > 65265) && (j_data->digital < 65285)) ||
                 ((j_data->digital > 65525) && (j_data->digital < 65545)) ) {
               value = NPS_SWITCH_DOWN;
            }
            else {
               value = NPS_SWITCH_UP;
            }
            success = TRUE;
            break;
         default:
            value = NPS_SWITCH_CENTER;
            success = FALSE;
            break;
      }
   }
   
   return success;
}


int
leather_flybox::calibrate ( const NPS_VALUATOR which_valuator,
                            const NPS_CALIBRATION which_calibration,
                            const float )
{
   int success = TRUE;

   if ( j_exists ) {
      switch ( which_valuator ) {
         case NPS_STCK:
            switch ( which_calibration ) {
               case NPS_RESTING_STATE:
                  success = joystick_reset(j_init);
                  break;
               default:
                  success = FALSE;
                  break;
            }
            break;
         default:
            success = FALSE;
            break;
      }
   }

   return success;
}

