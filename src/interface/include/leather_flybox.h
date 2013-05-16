
#ifndef __NPS_LEATHER_FLYBOX_H__
#define __NPS_LEATHER_FLYBOX_H__

#include "input_device.h"
#include "joystick.h"

class leather_flybox : public input_device {

public:
   leather_flybox ( const char * );
   ~leather_flybox ();

   void get_all_inputs ( pfChannel * );
   int exists ();
   int button_pressed ( const NPS_BUTTON, int & );
   int button_pressed ( const char, int &num_presses )
      { num_presses = 0; return FALSE; }
   int valuator_set ( const NPS_VALUATOR, float & );
   int switch_set ( const NPS_SWITCH, NPS_SWITCH_VAL & );
   int calibrate ( const NPS_VALUATOR, const NPS_CALIBRATION,
                   const float value = 0.0f );


private:

   JoystickInit *j_init;
   JoystickData *j_data;

   char comm_port[255];
   int valid_data;
   int j_exists;
};


#endif
