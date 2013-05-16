//header file for the NPSnet interface to the flybox
#ifndef __NPS_FLYBOX_H_
#define __NPS_FLYBOX_H_

#include <sys/schedctl.h>
#include "lv3.h"
#include "input_device.h"
#include "nps_device.h"

//the Default port for the fly box
#define FLYPORT "/dev/ttyd2"

//the max and min value the flybox can return
#define FLY_MAX  1.0f
#define FLY_MIN -1.0f

//These are on digital 0
#define FB_TOPLEFT    0x01
#define FB_TOPRIGHT   0x02
#define FB_MIDLEFT    0x04
#define FB_MIDRIGHT   0x08
#define FB_BOTLEFT    0x10
#define FB_BOTRIGHT   0x20
#define FB_MOMLEFT    0x40
#define FB_MOMRIGHT   0x80

//this one is on digital 1
#define FB_TRIGGER    0x01

//The analog outputs
#define FB_JOYROLL    0
#define FB_JOYPITCH   1
#define FB_JOYTWIST   2
#define FB_LEFTLEVER  3
#define FB_RIGHTLEVER 4

class flyboxClass : public input_device {

protected:
   bglv bgdata;
   struct itimerval itv;
   char valid;
   float deadzone;

public:
   flyboxClass(const char *port,const int sampleRate=50);
   flyboxClass() { flyboxClass(FLYPORT);};  //just in case
   ~flyboxClass(); //shut it down in a nice way

   int fb_request(); //ask for data from the fly box
   int fb_read();
   int fb_read(float analog[8] ,int digital[2]);
   int isvalid() { return(valid); } ; // is this valid data?
   void setvalid(int flag) { valid = flag; } ; // is this valid data?
   //set the value of the deadzone
   void setdeadzone(float range);

   //value of the switchs
   int topleft()  { return(bgdata.din[0] & FB_TOPLEFT); } ;
   int topright() { return(bgdata.din[0] & FB_TOPRIGHT); } ;
   int midleft()  { return(bgdata.din[0] & FB_MIDLEFT); } ;
   int midright() { return(bgdata.din[0] & FB_MIDRIGHT); } ;
   int botleft()  { return(bgdata.din[0] & FB_BOTLEFT); } ;
   int botright() { return(bgdata.din[0] & FB_BOTRIGHT); } ;
   int momleft()  { return(bgdata.din[0] & FB_MOMLEFT); } ;
   int momright() { return(bgdata.din[0] & FB_MOMRIGHT); } ;
   int trigger()  { return(bgdata.din[1] & FB_TRIGGER); } ;
   int momleftEdge();
   int momrightEdge();
   int triggerEdge();

   //the Joystick values
   float joyroll() ;
   float joypitch();
   float joytwist();

   //the Levers
   float leftlever();
   float rightlever();

   // For abstract base class input_device
   void get_all_inputs ( pfChannel * );
   int button_pressed ( const NPS_BUTTON, int & );
   int button_pressed ( const char, int &num_presses )
      { num_presses = 0; return FALSE; }
   int valuator_set ( const NPS_VALUATOR, float & );
   int switch_set ( const NPS_SWITCH, NPS_SWITCH_VAL & );
   int exists ();
   int calibrate ( const NPS_VALUATOR, const NPS_CALIBRATION,
                   const float value = 0.0f );

};

#endif
