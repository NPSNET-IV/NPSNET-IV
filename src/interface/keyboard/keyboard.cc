#include <Performer/pf.h>
#include "common_defs.h"
#include "nps_device.h"
#include "keyboard.h"
#include <iostream.h>
#include <bstring.h>
#include <string.h>

extern volatile DYNAMIC_DATA *G_dynamic_data;

// Public functions
// **********************************************************************


// Constructor
keyboard::keyboard ()
{
   mouse_open = FALSE;
   mouse_press = FALSE;
   keyState = 0x0;
   mouseButtonState = 0x0;
   first_query = TRUE;
   Xpid = 0;
   last_clickDownX = 0;
   last_clickDownX = 0;
   keyboard_open = FALSE;

   fork_sema = usnewsema (pfGetSemaArena(), 0);
   if ( fork_sema == NULL ) {
      cerr << "ERROR:\tkeyboard unable to get new semaphore from pf arena."
           << endl;
   }

   mouse_lock = usnewlock ( pfGetSemaArena() );
   if ( mouse_lock == NULL ) {
      cerr << "ERROR:\tkeyboard unable to get new lock from pf arena."
           << endl;
   }

   memset (keyPressCounter, 0, sizeof (int) * MAX_KEY_NUM);
   memset (lastKeyPressCount, 0, sizeof (int) * MAX_KEY_NUM);

   int thepid = fork ();

   if (thepid < 0) {
      cerr << "Error: In keyboard.  Unable to fork." << endl;
   }
   else if (thepid == 0) {
      collect_X_input ();
   }
   else if (thepid > 0) {
      Xpid = thepid;
   }
}

int keyboard::initXProcess ()
{
   first_query = FALSE;
   usvsema (fork_sema);
   return !(first_query);

}


// Destructor
keyboard::~keyboard ()
{
   usfreesema ( fork_sema, pfGetSemaArena() );
   usfreelock ( mouse_lock, pfGetSemaArena() );
}


void *
keyboard::operator new ( size_t size )
{
   register keyboard *temp = NULL;
   void *arena;
 
   arena = pfGetSharedArena();
   if ( arena != NULL ) {
      temp = (keyboard *)pfCalloc(size,1,arena); 
   }

   return temp;
}



void
keyboard::operator delete ( void *p, size_t )
{
   pfFree ( p );
}


void
keyboard::get_all_inputs ( pfChannel * )
{
   // display_inputs ();
   if (first_query) {
      initXProcess ();
   }
}


void
keyboard::display_inputs ()
{
   if ( keyboard_open ) {
   }
   if ( function_key_pressed(NPS_F1KEY) ) {
      cerr << "\tFunction Key -- F1 -- pressed."
           << function_key_pressed(NPS_F1KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F2KEY) ) {
      cerr << "\tFunction Key -- F2 -- pressed."
           << function_key_pressed(NPS_F2KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F3KEY) ) {
      cerr << "\tFunction Key -- F3 -- pressed."
           << function_key_pressed(NPS_F3KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F4KEY) ) {
      cerr << "\tFunction Key -- F4 -- pressed."
           << function_key_pressed(NPS_F4KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F5KEY) ) {
      cerr << "\tFunction Key -- F5 -- pressed."
           << function_key_pressed(NPS_F5KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F6KEY) ) {
      cerr << "\tFunction Key -- F6 -- pressed."
           << function_key_pressed(NPS_F6KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F7KEY) ) {
      cerr << "\tFunction Key -- F7 -- pressed."
           << function_key_pressed(NPS_F7KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F8KEY) ) {
      cerr << "\tFunction Key -- F8 -- pressed."
           << function_key_pressed(NPS_F8KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F9KEY) ) {
      cerr << "\tFunction Key -- F9 -- pressed."
           << function_key_pressed(NPS_F9KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F10KEY) ) {
      cerr << "\tFunction Key -- F10 -- pressed."
           << function_key_pressed(NPS_F10KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F11KEY) ) {
      cerr << "\tFunction Key -- F11 -- pressed."
           << function_key_pressed(NPS_F11KEY) << " times." << endl;
   }
   if ( function_key_pressed(NPS_F12KEY) ) {
      cerr << "\tFunction Key -- F12 -- pressed."
           << function_key_pressed(NPS_F12KEY) << " times." << endl;
   }
 
   if ( ascii_char_pressed('a') ) {
      cerr << "\tALPHA/NUM Key -- a -- pressed."
           << ascii_char_pressed('a') << " times." << endl;
   }
   if ( ascii_char_pressed('0') ) {
      cerr << "\tALPHA/NUM Key -- 0 -- pressed."
           << ascii_char_pressed('0') << " times." << endl;
   }
   if ( mouse_button_is_pressed(NPS_LEFT_MOUSE) ) {
      cerr << "\tLEFT mouse button is pressed." << endl;
   }
   if ( mouse_button_is_pressed(NPS_MIDDLE_MOUSE) ) {
      cerr << "\tMIDDLE mouse button is pressed." << endl;
   }
   if ( mouse_button_is_pressed(NPS_RIGHT_MOUSE) ) {
      cerr << "\tRIGHT mouse button is pressed." << endl;
   }

   if ( mouse_button_was_pressed() ) {
      long screen_x, screen_y;
      float norm_x, norm_y;
      cerr << "The mouse button was pressed." << endl;
      mouse_press_screen_coord ( screen_x, screen_y );
      mouse_press_norm_coord ( norm_x, norm_y );
      cerr << "\tScreen coords: " << screen_x << ", "
           << screen_y << endl;
      cerr << "\tNorm coords: " << norm_x << ", "
           << norm_y << endl;
   }

   if ( shift_button_pressed() ) {
      cerr << "The SHIFT button is pressed." << endl;
   }
   if ( control_button_pressed() ) {
      cerr << "The CONTROL button is pressed." << endl;
   }
   if ( caps_lock_pressed() ) {
      cerr << "The CAPS LOCK button is pressed." << endl;
   }



}



// Query functions
int
keyboard::function_key_pressed ( const int keynum )
{
   int keyPressed = FALSE;
   if ((keynum >= NPS_MIN_KEY) && (keynum <= GL_MAX_KEY)) {
      int keyCounter = keyPressCounter[keynum + NPS_KEYOFFSET];

      if (keyCounter != lastKeyPressCount[keynum + NPS_KEYOFFSET]) {
         keyPressed = TRUE;
         lastKeyPressCount[keynum + NPS_KEYOFFSET] = keyCounter;
      }
   }
   else if (keynum == NPS_SHIFTKEY) {
       keyPressed = keyState & (LEFT_SHIFT_KEY_MASK|RIGHT_SHIFT_KEY_MASK);
   }
   else if (keynum == NPS_CONTROLKEY) {
       keyPressed = keyState & (LEFT_CONTROL_KEY_MASK|RIGHT_CONTROL_KEY_MASK);
   }
   else if (keynum == NPS_ALTKEY) {
       keyPressed = keyState & (LEFT_ALT_KEY_MASK|RIGHT_ALT_KEY_MASK);
   }
   else if ((keynum >= NPS_LEFT_MOUSE) && (keynum <= NPS_MIDDLE_MOUSE)){
      switch (keynum) {
         case NPS_LEFT_MOUSE:
            keyPressed = mouseButtonState & LEFT_MOUSE_BUTTON_MASK;
            break;
         case NPS_MIDDLE_MOUSE:
            keyPressed = mouseButtonState & MIDDLE_MOUSE_BUTTON_MASK;
            break;
         case NPS_RIGHT_MOUSE:
            keyPressed = mouseButtonState & RIGHT_MOUSE_BUTTON_MASK;
            break;
         default:
            break;
      }
   }
   return keyPressed;
}


int
keyboard::ascii_char_pressed ( const char asciichar )
{
   int keyPressed = FALSE;

   int keyNumber = int (asciichar);
   int keyCounter = keyPressCounter[keyNumber];

   if (keyCounter != lastKeyPressCount[keyNumber]) {
      keyPressed = TRUE;
      lastKeyPressCount[keyNumber] = keyCounter;
   }

   return keyPressed;
}


int
keyboard::mouse_button_is_pressed ( const int mouse_button )
{
   int is_pressed = FALSE;
   switch (mouse_button) {
      case NPS_LEFT_MOUSE:
         is_pressed = mouseButtonState & LEFT_MOUSE_BUTTON_MASK;
         break;
      case NPS_MIDDLE_MOUSE:
         is_pressed = mouseButtonState & MIDDLE_MOUSE_BUTTON_MASK;
         break;
      case NPS_RIGHT_MOUSE:
         is_pressed = mouseButtonState & RIGHT_MOUSE_BUTTON_MASK;
         break;
      default:
         break;
   }
 
   return is_pressed;
}


int
keyboard::mouse_button_was_pressed ()
{
   return ( mouse_press && mouse_open );
}


int
keyboard::mouse_press_screen_coord ( long &, long & )
{
   int success = FALSE;

   if ( mouse_open ) {
      ussetlock(mouse_lock);
      usunsetlock(mouse_lock);
   }

   return success;
}


int
keyboard::mouse_press_norm_coord ( float &, float & )
{
   int success = FALSE;

   ussetlock(mouse_lock);
   if ( mouse_open ) {
/*      pos_x = 
         (2.0f*((mouse.clickPosLast[0] - mouse.winSizeX) / float(mouse.winSizeX)))
         - 1.0f;
      pos_y = 
         (2.0f*((mouse.clickPosLast[1] - mouse.winSizeY) / float(mouse.winSizeY)))
         - 1.0f;
      success = TRUE;
*/
   }
   usunsetlock(mouse_lock);

   return success;
}


int
keyboard::shift_button_pressed ( )
{

   int is_pressed = FALSE;

   return is_pressed;
}


int
keyboard::control_button_pressed ( )
{

   int is_pressed = FALSE;

   return is_pressed;
}


int
keyboard::caps_lock_pressed ( )
{

   int is_pressed = FALSE;

   return is_pressed;
}

int 
keyboard::button_pressed ( const NPS_BUTTON button_num, int &num_presses )
{
   int success = FALSE;

   if (first_query) {
      initXProcess ();
   }

   num_presses = success = function_key_pressed(button_num);
   return success;
}

int
keyboard::button_pressed ( const char button_char, int &num_presses )
{
   int success = FALSE;

   if (first_query) {
      initXProcess ();
   }

   num_presses = this->ascii_char_pressed(button_char); 

   success = ( keyboard_open && 
               (NPS_MIN_ALPHA_NUM_KEY <= int(button_char)) &&
               (int(button_char) <= NPS_MAX_ALPHA_NUM_KEY) );

   return success;
}


int
keyboard::get_multival ( const NPS_MULTIVAL multi_val,
                         void *data )
{
   int success = FALSE;
   float *mouse_position = (float *)data;

   if (first_query) {
      initXProcess ();
   }

   switch ( multi_val ) {
      case NPS_MV_MOUSE_LOCATION:
         static int ix = 0.0f, jy = 0.0f;
         pfGetPWinSize (G_dynamic_data->pw, &ix, &jy);
         success = TRUE;
         mouse_position[0] = theMouse[0];
         mouse_position[1] = jy - theMouse[1];
         break;
      case NPS_MV_MOUSE_CHAN_LOCATION:
      case NPS_MV_MOUSE_DOWN_SCREEN_LOCATION:
         long temp[2];
         success = mouse_press_screen_coord ( temp[0], temp[1] );
         if ( success ) {
            mouse_position[0] = (float)temp[0];
            mouse_position[1] = (float)temp[1];
         }
         break;
      case NPS_MV_MOUSE_DOWN_NORM_LOCATION:
         success = mouse_press_norm_coord ( mouse_position[0],
                                            mouse_position[1] );
         break;
      default:
         break;
   }

   return success;

}
