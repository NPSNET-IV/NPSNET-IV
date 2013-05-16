

#ifndef NOBASICPANEL

#include <stdlib.h>
#include <string.h>
#include <bstring.h>
#include <iostream.h>

#include "basic_panel.h"
#include "socket_lib.h"


basic_panel::basic_panel ( const char *net_interface,
                           unsigned short send_port,
                           unsigned short recv_port,
                           int buffer_len )
{
   ether_interf = (char *)malloc(strlen(net_interface)+1);
   strcpy ( ether_interf, net_interface );
   buffer_size = buffer_len;
   in_data_valid = FALSE;
   out_data_valid = FALSE;
   valid = socket_open ( ether_interf, send_port, recv_port, buffer_len );
}


basic_panel::~basic_panel ()
{
   if ( valid ) {
      socket_close();
      free(ether_interf);
      ether_interf = NULL;
      buffer_size = 0;
      valid = FALSE;
      in_data_valid = FALSE;
      out_data_valid = FALSE;
   }
}


void
basic_panel::get_all_inputs ( pfChannel * )
{
   GUI_MSG_DATA *read_packet = NULL;
   GUI_MSG_DATA *write_packet = NULL;
   struct readstat rstat;
   int ok_to_send = TRUE;
   int good_read = FALSE;
   BYTE attach_mode = 0x0;
   BYTE weapons_mode = 0x0;
   BYTE hud_mode = 0x0;
   BYTE environ_mode = 0x0;

   if ( valid ) {

      do {
         rstat.rs_swap_buffers_flag = FALSE;
         read_packet = socket_read ( &rstat );
         if ( read_packet != NULL ) {
            good_read = TRUE;
            attach_mode |= read_packet->attachMode;
            weapons_mode |= read_packet->weaponsMode;
            hud_mode |= read_packet->hudMode;
            environ_mode |= read_packet->environmentMode;
         }
      }
      while ( (read_packet != NULL) && (!rstat.rs_swap_buffers_flag) );

      if ( good_read ) {
         bcopy ( (char *)read_packet, (char *)&incoming_packet,
                 sizeof(GUI_MSG_DATA) );
         incoming_packet.attachMode = attach_mode;
         incoming_packet.weaponsMode = weapons_mode;
         incoming_packet.hudMode = hud_mode;
         incoming_packet.environmentMode = environ_mode;
         in_data_valid = TRUE;
      }

      // If application has supplied a response packet, then send it.
      if ( good_read && out_data_valid ) {
         write_packet = &outgoing_packet;
      }
      // Otherwise, return the packet we just received.
      else if ( good_read && in_data_valid ) {
         write_packet = &incoming_packet;
      }
      // Otherwise, we can't respond with valid data.
      // This means that we have either not establised a connection with
      //    the panel or that we have never received a good packet.
      else {
         ok_to_send = FALSE;
      }

      if ( ok_to_send ) {
         if ( !socket_write((char *)write_packet) ) {
            cerr << "ERROR: Basic Remote Panel write on socket failed!"
                 << endl;
         }
      }

   }
}


int
basic_panel::exists ()
{
   return valid;
}


int
basic_panel::button_pressed ( const NPS_BUTTON button_id, int &count )
{
   int valid_button = FALSE;
   count = 0;

   if ( in_data_valid ) {
      valid_button = TRUE;
      switch ( button_id ) {
         case NPS_FIRE1_BUTTON:
            count = (incoming_packet.weaponsMode & GUI_PRIMARY);
            break;
         case NPS_FIRE2_BUTTON:
            count = (incoming_packet.weaponsMode & GUI_SECONDARY);
            break;
         case NPS_FIRE3_BUTTON:
            count = (incoming_packet.weaponsMode & GUI_TERTIARY);
            break;
         default:
            valid_button = FALSE;
            break;
      }
   }
 
   return valid_button;
}


int
basic_panel::valuator_set ( const NPS_VALUATOR valuator_id, float &val )
{
   int valid_valuator = FALSE;
   val = 0.0f;

   if ( in_data_valid ) {
      valid_valuator = TRUE;
      switch ( valuator_id ) {
         case NPS_STCK_HORIZONTAL:
            val = incoming_packet.joystickX;
            break;
         case NPS_STCK_VERTICAL:
            val = incoming_packet.joystickY;
            break;
         case NPS_THRTL:
            val = incoming_packet.throttleSetting;
            break;
         default:
            valid_valuator = FALSE;
      }
   }

   return valid_valuator;
}


int
basic_panel::switch_set ( const NPS_SWITCH , NPS_SWITCH_VAL &)
{
   if ( in_data_valid ) {
   }
   return FALSE;

}


int
basic_panel::get_multival ( const NPS_MULTIVAL, void *)
{
   if ( in_data_valid ) {
   }
   return FALSE;

}

#endif

