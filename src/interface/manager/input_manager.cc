
#include "input_manager.h"
#include "keyboard.h"
#include "fcs.h"
#include "flybox.h"
#include "leather_flybox.h"
#include "kaflight.h"
#ifndef NO_IPORT
#include "iportClass.h"
#include "testportClass.h"
#include "uniportClass.h"
#endif // NO_IPORT
#ifndef NOUPPERBODY 
#include "UpperBodyClass.h"
#endif // NOUPPERBODY
#ifndef NOBASICPANEL
#include "basic_panel.h"
#endif
#include "fakespaceClass.h"
#include "odt.h"
#include "lw_tcpip.h"
#include <iostream.h>
#include <assert.h>
#include <string.h>

extern "C" {
//   int	strcasecmp(void*, void*);
};

// Private functions
// **********************************************************************



// Public functions
// **********************************************************************

// Constructor
input_manager::input_manager ()
{
   for ( int next_dev = NPS_NO_INPUT; 
         next_dev < NPS_NUM_DEVICE_TYPES; next_dev++ ) {
      dev_list[next_dev].active_mask = 0x0;
      for ( int count = 0; count < MAX_SAME_DEVICE; count++ ) {
         dev_list[next_dev].device[count] = NULL;
      }
   } 
}


// Destructor
input_manager::~input_manager ()
{
   exit_input();
}


void *
input_manager::operator new ( size_t size )
{
   register input_manager *temp = NULL;
   void *arena;
 
   arena = pfGetSharedArena();
   if ( arena != NULL ) {
      temp = (input_manager *)pfCalloc(size,1,arena); 
   }

   return temp;
}


void
input_manager::operator delete ( void *p, size_t )
{
   pfFree ( p );
}


void
input_manager::get_all_inputs ( pfChannel *chan )
{
   short temp_mask;
   int counter;

   for ( int next_dev = NPS_NO_INPUT;
         next_dev < NPS_NUM_DEVICE_TYPES; next_dev++ ) {
      temp_mask = dev_list[next_dev].active_mask;
      counter = 0;
      while ( (temp_mask != 0x0) && (counter < MAX_SAME_DEVICE) ) {
         if ( ((temp_mask&0x1) != 0) && 
              (dev_list[next_dev].device[counter] != NULL) ) {
            dev_list[next_dev].device[counter]->get_all_inputs(chan);
         }
         temp_mask = (temp_mask >> 1);
         counter++;
      }
   }
}


void
input_manager::display_inputs ()
{
}


void
input_manager::exit_input ()
{
   for ( int next_dev = NPS_NO_INPUT; 
         next_dev < NPS_NUM_DEVICE_TYPES; next_dev++ ) {
      dev_list[next_dev].active_mask = 0x0;
      for ( int count = 0; count < MAX_SAME_DEVICE; count++ ) {
         if ( dev_list[next_dev].device[count] != NULL ) {
            delete dev_list[next_dev].device[count];
            dev_list[next_dev].device[count] = NULL;
          }
      }
   }
}


int
input_manager::manage ( const NPS_Input_Device device_kind,
                        const char *init_string,
                        const int device_num )
{
   int success = FALSE;

   if ( (0 <= device_num) && (device_num < MAX_SAME_DEVICE) &&
        (NPS_NO_INPUT < device_kind) && (device_kind < NPS_NUM_DEVICE_TYPES)) {

      if ( (((dev_list[device_kind].active_mask>>device_num) & 0x1) == 0) &&
            dev_list[device_kind].device[device_num] == NULL ) {
         switch ( device_kind ) {
            case NPS_KEYBOARD:
               dev_list[device_kind].device[device_num] = new keyboard;
               if ( dev_list[device_kind].device[device_num] != NULL ) {
                  success = TRUE;
                  cerr << "\tManaging KEYBOARD for input." << endl;
               }
               break;
            case NPS_FCS:
               fcsInit *tmpInit = (fcsInit *)init_string;
               if (strcasecmp((const char *)tmpInit->port_name, "NONE") == 0) {
                  break;
               }
               dev_list[device_kind].device[device_num] =
                  new fcs(tmpInit->port_name, 0, tmpInit->multi);
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  cerr << "\tManaging FCS on port " << init_string 
                       << " for input." << endl;
               }
               break;
            case NPS_KAFLIGHT:
               dev_list[device_kind].device[device_num] =
                  new kaflight(init_string,0);
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  cerr << "\tManaging KAFLIGHT on port " << init_string
                       << " for input." << endl;
               }
               break;
            case NPS_FLYBOX:
               dev_list[device_kind].device[device_num] =
                  new flyboxClass(init_string,50);
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  cerr << "\tManaging BG FLYBOX on port " << init_string
                       << " for input." << endl;
               }
               break;
            case NPS_LEATHER_FLYBOX:
               dev_list[device_kind].device[device_num] =
                  new leather_flybox(init_string);
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  cerr << "\tManaging Leather Net FLYBOX on port " << init_string
                       << " for input." << endl;
               }
               break;
#ifndef NO_IPORT
            case NPS_UNIPORT:
               dev_list[device_kind].device[device_num] =
                  new uniportClass();
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  cerr << "\tManaging Uni-Port for input." << endl;
               }
               break;
            case NPS_TESTPORT:
               dev_list[device_kind].device[device_num] =
                  new testportClass(init_string);
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  cerr << "\tManaging Test-Port for input." << endl;
               }
               break;
#endif NO_IPORT
#ifndef NOUPPERBODY
            case NPS_UPPERBODY:
               dev_list[device_kind].device[device_num] =
                  new UpperBodyClass(init_string);
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  dev_list[device_kind].device[device_num]->calibrate
                                                  (NPS_VALUATOR_0, NPS_CAL);
                  cerr << "\tManaging Upperbody Sensors for input." << endl;
               }
               break;
#endif // NOUPPERBODY
#ifndef NOBASICPANEL
            case NPS_BASIC_PANEL:
               dev_list[device_kind].device[device_num] =
                  new basic_panel(init_string);
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  cerr << "\tManaging Basic Remote Panel on interface " 
                       << init_string << " for input." << endl;
               }
               break;
#endif
            case NPS_SHIP_PANEL:
               dev_list[device_kind].device[device_num] = new keyboard;
               if ( dev_list[device_kind].device[device_num] != NULL ) {
                  success = TRUE;
                  cerr << "\tNot Managing SHIP_PANEL for input." << endl;
               }
               break;
            case NPS_SUB_PANEL:
               dev_list[device_kind].device[device_num] = new keyboard;
               if ( dev_list[device_kind].device[device_num] != NULL ) {
                  success = TRUE;
                  cerr << "\tNot Managing SUB_PANEL for input." << endl;
               }
               break;
            case NPS_FAKESPACE:
               char *tmpChar = (char *)init_string;
               dev_list[device_kind].device[device_num] = 
                  new fakespaceClass (tmpChar);
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  cerr << "\tManaging Fakespace device for input." << endl;
               }
               break;
            case NPS_ODT:
               odtInit *odt_tmp = (odtInit *)init_string;
               dev_list[device_kind].device[device_num] =
                  new odt (odt_tmp->port_name, 1, odt_tmp->mp);
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  cerr << "\tManaging ODT device for input." << endl;
               }
               break;
            case NPS_LWS:
               lwStruct *lw_tmp = (lwStruct *)init_string;
               dev_list[device_kind].device[device_num] =
                  new lw_tcpip ((unsigned short)lw_tmp->r_port, "", 
                                lw_tmp->name,
                                (unsigned short)lw_tmp->s_port, "", 
                                lw_tmp->name);
               if ( dev_list[device_kind].device[device_num]->exists() ) {
                  success = TRUE;
                  cerr << "\tManaging Land Warrior System for input." << endl;
               }
               break;
            default:
               break;
         }
         if ( success ) {
            dev_list[device_kind].active_mask =
               dev_list[device_kind].active_mask | (0x1 << device_num);
         }
      }
   }

   return success;
}


int
input_manager::query_button ( int &num_presses,
                              const NPS_Input_Device device_kind, 
                              const NPS_BUTTON button_num,
                              const int device_num )
{
   int success = FALSE;
   num_presses = 0;

   if ( (0 <= device_num) && (device_num < MAX_SAME_DEVICE) &&
     (NPS_NO_INPUT < device_kind) && (device_kind < NPS_NUM_DEVICE_TYPES) &&
     (((dev_list[device_kind].active_mask>>device_num) & 0x1) == 1) ) {
      success = dev_list[device_kind].device[device_num]->
                button_pressed ( button_num, num_presses );
   }
   return success;
}

int
input_manager::query_button ( int &num_presses,
                              const NPS_Input_Device device_kind,
                              const char button_char,
                              const int device_num )
{
   int success = FALSE;
   num_presses = 0;

   if ( (0 <= device_num) && (device_num < MAX_SAME_DEVICE) &&
     (NPS_NO_INPUT < device_kind) && (device_kind < NPS_NUM_DEVICE_TYPES) &&
     (((dev_list[device_kind].active_mask>>device_num) & 0x1) == 1) ) {
      success = dev_list[device_kind].device[device_num]->
                button_pressed ( button_char, num_presses );
   }
   return success;
}


int 
input_manager::query_valuator ( float &value,
                                const NPS_Input_Device device_kind,
                                const NPS_VALUATOR valuator_num,
                                const int device_num )
{
   int success = FALSE;
   value = 0.0f;

   if ( (0 <= device_num) && (device_num < MAX_SAME_DEVICE) &&
     (NPS_NO_INPUT < device_kind) && (device_kind < NPS_NUM_DEVICE_TYPES) &&
     (((dev_list[device_kind].active_mask>>device_num) & 0x1) == 1) ) {
      success = dev_list[device_kind].device[device_num]->
                valuator_set ( valuator_num, value );
   }
   return success;

}


int
input_manager::query_switch ( NPS_SWITCH_VAL &value,
                              const NPS_Input_Device device_kind,
                              const NPS_SWITCH switch_num,
                              const int device_num )
{
   int success = FALSE;
   value = NPS_SWITCH_CENTER;

   if ( (0 <= device_num) && (device_num < MAX_SAME_DEVICE) &&
     (NPS_NO_INPUT < device_kind) && (device_kind < NPS_NUM_DEVICE_TYPES) &&
     (((dev_list[device_kind].active_mask>>device_num) & 0x1) == 1) ) {
      success = dev_list[device_kind].device[device_num]->
                switch_set ( switch_num, value );
   }
   return success;

}

int
input_manager::query_state ( int &value,
                             const NPS_Input_Device device_kind,
                             const NPS_STATE state_val,
                             const int device_num )
{
   int success = FALSE;

   if ( (0 <= device_num) && (device_num < MAX_SAME_DEVICE) &&
     (NPS_NO_INPUT < device_kind) && (device_kind < NPS_NUM_DEVICE_TYPES) &&
     (((dev_list[device_kind].active_mask>>device_num) & 0x1) == 1) ) {
      success = dev_list[device_kind].device[device_num]->
                get_state ( state_val, value );
   }
   return success;
}

int
input_manager::query_multival ( void *data,
                                const NPS_Input_Device device_kind,
                                const NPS_MULTIVAL multi_val,
                                const int device_num )
{
   int success = FALSE;

   if ( (0 <= device_num) && (device_num < MAX_SAME_DEVICE) &&
     (NPS_NO_INPUT < device_kind) && (device_kind < NPS_NUM_DEVICE_TYPES) &&
     (((dev_list[device_kind].active_mask>>device_num) & 0x1) == 1) ) {
      success = dev_list[device_kind].device[device_num]->
                get_multival ( multi_val, data );
   }
   return success;
}

int
input_manager::set_multival ( void *data,
                                const NPS_Input_Device device_kind,
                                const NPS_MULTIVAL multi_val,
                                const int device_num )
{
   int success = FALSE;

   if ( (0 <= device_num) && (device_num < MAX_SAME_DEVICE) &&
     (NPS_NO_INPUT < device_kind) && (device_kind < NPS_NUM_DEVICE_TYPES) &&
     (((dev_list[device_kind].active_mask>>device_num) & 0x1) == 1) ) {
      success = dev_list[device_kind].device[device_num]->
                set_multival ( multi_val, data );
   }
   return success;
}



int
input_manager::get_next_same_device ( const NPS_Input_Device the_device,
                                      int &the_num )
{
   int success = FALSE;
   int counter = 0;
   int current;

   if ( (the_num < 0) || (the_num >= MAX_SAME_DEVICE) ) {
      the_num = -1;
   }
   current = the_num;
   while ( !success && (counter < MAX_SAME_DEVICE) ) {
      current = (current + 1) % MAX_SAME_DEVICE;
      if ( (((dev_list[the_device].active_mask >> current) & 0x1) > 0) &&
           (dev_list[the_device].device[current] != NULL) ) { 
         success = TRUE;
         the_num = current;
      }
      else {
         counter++;
      }
   }

   return success;
}


int
input_manager::get_next_device ( NPS_Input_Device &the_device,
                                 int &the_num )
{
   short temp_mask;
   int success = FALSE;
   int counter = 0;
   int current = the_device;
   int temp_num;

   if ( (the_num < 0) || (the_num >= MAX_SAME_DEVICE) ) {
      the_num = 0;
   }
   temp_num = the_num;
   temp_mask = (0x1 << the_num);
   if ( dev_list[the_device].active_mask > temp_mask ) {
      success = get_next_same_device ( (NPS_Input_Device)current, temp_num );
   }
   if ( success && (current == the_device) && (temp_num == the_num) ) {
      success = FALSE;
   }
   current = the_device;
   while ( !success && (counter < NPS_NUM_DEVICE_TYPES) ) {
      current = (current + 1) % NPS_NUM_DEVICE_TYPES;
      temp_num = -1;
      if ( get_next_same_device ( (NPS_Input_Device)current,
                                 temp_num ) ) {
         success = TRUE;
         the_device = (NPS_Input_Device)current;
         the_num = temp_num;
      }
      else {
         counter++;
      }
   }
      
   return success; 
}


int
input_manager::verify_device ( const NPS_Input_Device the_device,
                               const int the_num )
{
   return ( (dev_list[the_device].active_mask & 
             (0x1 << the_num)) > 0 );
}


int
input_manager::calibrate ( const NPS_VALUATOR which_valuator,
                           const NPS_CALIBRATION which_calibration,
                           const NPS_Input_Device the_device,
                           const float value, const int the_num )
{
   int success = FALSE;

   if ( this->verify_device(the_device,the_num) ) {
      success = dev_list[the_device].device[the_num]->
         calibrate(which_valuator,which_calibration,value);
   }
  
   return success; 

}

