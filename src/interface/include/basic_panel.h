
#ifndef __NPS_BASIC_PANEL_H__
#define __NPS_BASIC_PANEL_H__

#include <pf.h>
#include "socket_struct.h"
#include "input_device.h"

#define DEF_BPANEL_SEND_PORT (u_short)3200
#define DEF_BPANEL_RECV_PORT DEF_BPANEL_SEND_PORT+1

class basic_panel : public input_device {

public:

   basic_panel ( const char *,
                 unsigned short send_port = DEF_BPANEL_SEND_PORT,
                 unsigned short recv_port = DEF_BPANEL_RECV_PORT,
                 int buffer_len = 50 );

   virtual ~basic_panel ();

   virtual void get_all_inputs ( pfChannel * );
   virtual int exists ();
   virtual int button_pressed ( const NPS_BUTTON, int & );
   virtual int button_pressed ( const char, int &num_presses )
      { num_presses = 0; return FALSE; }
   virtual int valuator_set ( const NPS_VALUATOR, float & );
   virtual int switch_set ( const NPS_SWITCH, NPS_SWITCH_VAL & );
   virtual int get_multival ( const NPS_MULTIVAL, void * );

protected:

   int valid;
   char *ether_interf;
   int buffer_size;

private:

   int in_data_valid;
   int out_data_valid;
   GUI_MSG_DATA incoming_packet;
   GUI_MSG_DATA outgoing_packet;

};

#endif
