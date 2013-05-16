
#ifndef NOC2MEASURES

#include "po_interface.h"
#include "po_funcs.h"

#include "manager.h"
#include "common_defs2.h"
#include "interface_const.h"

#include "imstructs.h"
#include "imclass.h"

extern InitManager *initman;


// Globals used by this library
extern volatile MANAGER_REC       *G_manager;
extern volatile float              G_curtime;
extern volatile DYNAMIC_DATA      *G_dynamic_data;



int initPoMeas ( const char *ctdb_filename, // ctdb terrain file name 
                 unsigned char exercise,    // excercise ID
                 unsigned char database,    // database ID
                 unsigned short port )      // UDP PO port
{
   int success;
   struct RW rw;
   initman->get_round_world_data(&rw);

   success = initPoStuff((char *)ctdb_filename,exercise,database,port,
                         (float64)rw.northing, (float64)rw.easting);
   return success;
}


int
updatePoMeas ()
{
   updatePoStuff();
   return TRUE;
}


int
closePoMeas ()
{
   return ( closePoNet() );
}


int
po_input_mode ()
{
   return ( G_dynamic_data->pick_mode == PO_PICK );
}


int
display_po_key ()
{
   input_manager *l_input_manager = G_manager->ofinput;
   int count = 0;

   if ( l_input_manager != NULL ) {
      l_input_manager->query_button(count,NPS_KEYBOARD,'o');
   }

   return count;
}


long
get_valuator ( int dev )
{
   input_manager *l_input_manager = G_manager->ofinput;
   float mouse_pos[2];
   long ret_val = 0;

   if ( l_input_manager->query_multival((void *)mouse_pos,
           NPS_KEYBOARD,NPS_MV_MOUSE_LOCATION) ) {
      switch ( dev ) {
         case NPS_MOUSEX:
            ret_val = mouse_pos[0];
            break;
         case NPS_MOUSEY:
            ret_val = mouse_pos[1];
            break;
         default:
            break;
      }
   }

   return ret_val;
}


int
get_button ( int num )
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;

   if ( po_input_mode() ) {
      switch ( num ) {
         case NPS_LEFTMOUSE:
            l_input_manager->query_button(counter,NPS_KEYBOARD,
                                          NPS_MOUSE_LEFT_BUTTON);
            if ( counter > 0 ) {
               counter = 1;
            }
            else {
               counter = 0;
            }
            break;
         case NPS_MIDDLEMOUSE:
            counter = 0;
            break;
         case NPS_RIGHTMOUSE:
            l_input_manager->query_button(counter,NPS_KEYBOARD,
                                          NPS_MOUSE_RIGHT_BUTTON);
            if ( counter > 0 ) {
               counter = 1;
            }
            else {
               counter = 0;
            }
            break;
         default:
            break;
      }
   }

   return counter;
}

#endif
