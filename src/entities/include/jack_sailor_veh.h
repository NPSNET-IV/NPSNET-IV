// File: <jack_sailor_veh.h> 

#ifndef __JACK_SAILOR_VEH_H__
#define __JACK_SAILOR_VEH_H__

#ifdef JACK
#include "jack_di_veh.h"

// demo TYPES
#define DEC  	       -1
#define DEMO_RESET 	0
#define INC		1
#define WALKJACK	2
#define FOLLOW		3
#define CAMERA		4

#define MAXNUMSCRIPTS 50
enum scriptEvent {LOAD=0, MOVE, PIVOT, STAIRS, DOOR, 
                  VALVE, HANDS, FOLLK, NOZZLE, STMLK, 
                  VENT, HALON, PLAYSND, JUMP, TEXT, 
                  NEXT, PAUSE, EXIT, RESTART};

typedef struct {
   scriptEvent event;
   pfVec3 xyz;
   //float x;
   //float y;
   //float z;
   float H; // heading
   float P; // neck pitch  -- skip roll
   float speed;
   int object; // represents -> steps, and door#
   int misc;
   int sound;
   float cx;
   float cy;
   float cz;
   float cH; // heading
   float cP; // pitch  -- skip roll
} scriptStruct;

class JACK_SAILOR_VEH: public JACK_DI_VEH {

   friend class JACK_UNIT_VEH;

protected:
   int          scriptsLoaded;
   scriptStruct script[MAXNUMSCRIPTS];
   int          demo_mode;

   void readScriptFile(int);
   void runScriptDemo(int);

public:
   JACK_SAILOR_VEH(int,int,ForceID);
   ~JACK_SAILOR_VEH();
#ifndef NONPSSHIP
   virtual void updateRelPosVec();
   virtual void reset_on_mounted_vid();
#endif

   virtual int check_collide(pfVec3 &,pfVec3 &, pfVec3 &, float &, float &);
   virtual void class_specific_keyboard();
   virtual void class_specific_fcs();
};

#endif

#endif

