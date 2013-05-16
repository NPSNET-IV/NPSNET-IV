
#ifndef __NPS_INPUT_MANAGER__
#define __NPS_INPUT_MANAGER__

#include <pf.h>
#include <stddef.h>

#include "nps_device.h"
#include "input_device.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_SAME_DEVICE 4

typedef struct {
   short active_mask;
   input_device *device[MAX_SAME_DEVICE];
} nps_device;


class input_manager {

public:

   input_manager();
   ~input_manager();

   // Overload new and delete so member variables are allocated out
   // of Performer shared memory.
   void *operator new(size_t);
   void operator delete(void *, size_t);

   int manage ( const NPS_Input_Device,
                const char *port_name = NULL,
                const int device_num = 0 );

   int query_button ( int &,
                      const NPS_Input_Device,
                      const NPS_BUTTON,
                      const int device_num = 0 );
   int query_button ( int &,
                      const NPS_Input_Device,
                      const char,
                      const int device_num = 0 );

   int query_valuator ( float &,
                        const NPS_Input_Device,
                        const NPS_VALUATOR,
                        const int device_num = 0 );

   int query_switch ( NPS_SWITCH_VAL &,
                      const NPS_Input_Device,
                      const NPS_SWITCH,
                      const int device_num = 0 );

   int query_state ( int &,
                     const NPS_Input_Device,
                     const NPS_STATE,
                     const int device_num = 0 );

   int query_multival ( void *,
                        const NPS_Input_Device,
                        const NPS_MULTIVAL,
                        const int device_num = 0 );

   int set_multival ( void *,
                      const NPS_Input_Device,
                      const NPS_MULTIVAL,
                      const int device_num = 0 );

   void get_all_inputs ( pfChannel * );
   void display_inputs ();

   void exit_input ();

   int get_next_same_device ( const NPS_Input_Device, int & );
   int get_next_device ( NPS_Input_Device &, int & );
   int verify_device ( const NPS_Input_Device, const int the_num = 0 );

   int calibrate ( const NPS_VALUATOR,
                   const NPS_CALIBRATION,
                   const NPS_Input_Device,
                   const float value = 0.0f, const int the_num = 0 );

private:

   nps_device dev_list[NPS_NUM_DEVICE_TYPES];

};

#endif

