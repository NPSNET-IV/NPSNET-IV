
#ifndef __NPS_INPUT_DEVICE__
#define __NPS_INPUT_DEVICE__

#include <pf.h>
#include "nps_device.h"

class input_device {

public:

   input_device(){;}
   virtual ~input_device(){;}

   virtual void get_all_inputs ( pfChannel *chan ) = 0;

   virtual int exists () { return FALSE; }

   virtual int button_pressed ( const NPS_BUTTON, int &num_presses )
      { num_presses = 0; return FALSE; }
   virtual int button_pressed ( const char, int &num_presses )
      { num_presses = 0; return FALSE; }
   virtual int valuator_set ( const NPS_VALUATOR, float &value )
      { value = 0.0f; return FALSE; }
   virtual int switch_set ( const NPS_SWITCH, NPS_SWITCH_VAL &value )
      { value = NPS_SWITCH_CENTER; return FALSE; }
   virtual int get_state ( const NPS_STATE, int &value )
      { value = 1; return FALSE; }
   virtual int calibrate ( const NPS_VALUATOR, const NPS_CALIBRATION, 
                           const float value = 0.0f )
      { if (value == 0.0f) return FALSE; else return FALSE; }
   virtual int get_multival ( const NPS_MULTIVAL, void * )
      { return FALSE; }

   virtual int set_multival ( const NPS_MULTIVAL, void * )
      { return FALSE; }

   /* Currently needed only for GL devices */

};

#endif

