#ifndef __IPORT_CLASS_DOT_H__
#define __IPORT_CLASS_DOT_H__

#include <ulocks.h>       // for datalock definitions

#include "input_device.h" // for input_device class definition
#include "nps_device.h"   // for enum definitions for devices
#include "DI_comm.h"      // for DIJOINTCOUNT definition

#ifndef TRUE
#define TRUE 1
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#endif // FALSE

const int BUFFSIZE = 2;
const int OUTBUFF = 0; 
const int INBUFF = 1;

typedef struct {
   pfCoord pos;
   pfCoord rifle;
   pfVec3  velocity;
// float   heading;
   float   jlist[DIJOINTCOUNT];
   pfVec3  gaze;
   int     trigger_press;
   int     trigger_press_aux;
   int     flags;
} fromIport;

typedef struct {
   pfVec3 xyz;
   pfVec3 normal;
   int    flags;
} toIport;

void i_sig_handler (int, ...);

class iportClass : public input_device {

public:

   iportClass ();   // Constructor
   virtual ~iportClass (); // Destructor

   virtual int exists () { return TRUE; } // return uniport_detect; }

   virtual  void get_all_inputs (pfChannel *);

   virtual int button_pressed (const NPS_BUTTON, int &);
   virtual int button_pressed (char, int &num_presses)
      { num_presses = 0; return FALSE; }
   virtual int valuator_set ( const NPS_VALUATOR, float &);

   virtual int get_state (const NPS_STATE, int &);

   virtual int get_multival (const NPS_MULTIVAL, void *);
   virtual int set_multival (const NPS_MULTIVAL, void *);

protected:

   fromIport recv_buffer[BUFFSIZE];
   toIport   send_buffer;
   int       child_pid;
   usptr_t  *handle;
   ulock_t   iport_send_lock;
   ulock_t   iport_recv_lock;

   virtual void print_buffers ();

};

#endif // __IPORT_CLASS_DOT_H__
