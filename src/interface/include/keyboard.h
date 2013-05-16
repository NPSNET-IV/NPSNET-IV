
#ifndef __NPS_KEYBOARD__
#define __NPS_KEYBOARD__

#include <gl/device.h>
#include <pf.h>
#include <pfutil.h>
#include <stddef.h>
#include <ulocks.h>

#include "input_device.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// NPS extended keyboard definitions
#define NPS_MIN_EVENT_KEY	NPS_MIN_KEY
#define NPS_MAX_EVENT_KEY	NPS_MAX_KEY

#define NPS_MIN_ALPHA_NUM_KEY	0
#define NPS_MAX_ALPHA_NUM_KEY	PFUDEV_KEY_MAP_SIZE-1

#define NPS_LEFT_MOUSE		NPS_MOUSE_LEFT_BUTTON
#define NPS_RIGHT_MOUSE		NPS_MOUSE_RIGHT_BUTTON
#define NPS_MIDDLE_MOUSE	NPS_MOUSE_MIDDLE_BUTTON

#define NPS_MAX_EXTRA_DEV	((GL_MAX_DEV - GL_MIN_DEV) + 1)

const int MAX_KEY_NUM = 500;

const int LEFT_SHIFT_KEY_MASK = 0x01;
const int RIGHT_SHIFT_KEY_MASK = 0x02;
const int LEFT_CONTROL_KEY_MASK = 0x04;
const int RIGHT_CONTROL_KEY_MASK = 0x08;
const int LEFT_ALT_KEY_MASK = 0x10;
const int RIGHT_ALT_KEY_MASK = 0x20;

const int LEFT_MOUSE_BUTTON_MASK = 0x01;
const int MIDDLE_MOUSE_BUTTON_MASK = 0x02;
const int RIGHT_MOUSE_BUTTON_MASK = 0x04;

const int ROLL_OVER_VALUE = 6000;

const int NPS_KEYOFFSET = 256;

typedef struct {
   int   count;
   void *val;
   } nps_input_rec;


class keyboard : public input_device {

public:

   keyboard();
   ~keyboard();

   // Overload new and delete so member variables are allocated out
   // of Performer shared memory.
   void *operator new(size_t);
   void operator delete(void *, size_t);

   void get_all_inputs ( pfChannel * );
   void display_inputs ();

   int button_pressed ( const NPS_BUTTON, int & );
   int button_pressed ( const char, int & );

   int function_key_pressed ( const int );
   int ascii_char_pressed ( const char );

   int get_multival ( const NPS_MULTIVAL, void * );

   int mouse_button_is_pressed ( const int );
   int mouse_button_was_pressed ();
   int mouse_press_screen_coord ( long &, long & );
   int mouse_press_norm_coord ( float &, float & );
   int shift_button_pressed ();
   int control_button_pressed ();
   int caps_lock_pressed ();


private:

   void collect_X_input (void);
   int initXProcess (void);

   int keyState;
   int mouseButtonState;

   int first_query;
   int Xpid;

   int keyPressCounter[MAX_KEY_NUM];
   int lastKeyPressCount[MAX_KEY_NUM];
   float theMouse[2];

   int keyboard_open;
   int mouse_open;
   int mouse_press;
   long last_clickDownX;
   long last_clickDownY;

   ulock_t mouse_lock;
   usema_t *fork_sema;
//   ulock_t fork_lock;

};

#endif

