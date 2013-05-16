/*
File              :  ship_common.C
Authors           :  James F. Garrova
                  :  Joseph A. Nobles
Date              :  1 February 1995
Revised           :  1 February 1995

Thesis Advisors   :  Dr Michael A. Zyda
                  :  Dr David R. Pratt

Reference         :  NPSNETIV Ver 6.0 common.cc

*/


#ifndef _NPS_SHIP_COMMON_DEFS_
#define _NPS_SHIP_COMMON_DEFS_

#include "ship_defs.h"


#define FILENAME_SIZE 45


//Types of entities

enum vtype{VEH_TYPE = -1, FLYING_CARPET, GND_VEH, AIR_VEH, FIXED_WING,
           ROTARY_WING, SHIP, SUBMERSIBLE, PERSON, UNMANNED, DI_GUY};

//Results of movement functions
enum movestat{FINEWITHME, DEACTIVE, HITSOMETHING, HITHEADON, HITASHIP};
enum objtype{LAND_OBJ, OFFSHORE_OBJ, BRIDGE_OBJ, BUOY_OBJ};


typedef struct
{
  float left;
  float right;
  float bottom;
  float top;
} VIEWPORT_REC;

typedef struct
{
  float near;
  float far;
} NEAR_FAR_REC;


typedef struct
{
  unsigned short site;
  unsigned short host;
  char * ether_interf;
} NETDATA;


typedef struct
   {
   pfCoord posture;
   pfCoord look;
   int type;
   float speed;
   float leftengineRPM;
   float rightengineRPM;
   float rudderAngle;
   float altitude;
   float exTime;
   } PASS_DATA;

typedef struct
   {
   int active;
   pfChannel *chan_ptr;
   VIEWPORT_REC viewport;
   NEAR_FAR_REC clip_plane;
   pfEarthSky *earthSky;
   pfLightSource *sun;
   pfFog *fog_ptr;
   pfVec3 fog_color;
   long earthSkyMode;
   float horiz_angle;
   float ground_height;
   int eyeview;
   pfScene *scene;
   pfCoord view;
   } CHANNEL_DATA;

typedef struct
   {
   int active;
   pfPipe *pipe_ptr;
   VIEWPORT_REC window;
   int num_channels;
   CHANNEL_DATA channel;
   } PIPE_DATA;

typedef struct
   {
   char model_file[FILENAME_SIZE];
   char network_file[FILENAME_SIZE];
   char filter_file[FILENAME_SIZE];
   char driven_file[FILENAME_SIZE];
   char terrain_file[FILENAME_SIZE];
   char script_file[FILENAME_SIZE];
   char object_file[FILENAME_SIZE];
   pfCoord init_location;
   int num_pipes;
   float frame_rate;
   long phase;
   int reality_graphics;
   int multicast;
   long sceneSize;
   SimulationAddress DIS_address;
   char hostname[80];
   unsigned char exercise;
   long input;
   long xformerMode;
//   pfuMouse mouse;
   } STATIC_DATA;

typedef struct
   {
   unsigned char exercise;
   PIPE_DATA pipe[MAXPIPES];
   unsigned long Notify_Level;
   float tod;
   float aspect;
   float fovx;
   float fovy;
   float viewHeading;
   float horizonAngle;
   NEAR_FAR_REC clip_plane;
   VIEWPORT_REC view;
   pfList *texList;
   int init_event;
   int exitFlag;
   int new_vid;
   int new_vehicle;
   pfuXformer *xformer;
   pfuMouse mouse;
   pfuEvent events;
   ForceID force;
   int aa;
   long fog;
   float farFogRange;
   float nearFogRange;
   int viewOption;
   int updateChannels;
   int lighting;
   int texture;
   int wireframe;
   int runLightStatus;
   int stats;
   int statsEnable;
   int azCircle;
   int playBack;
   int fastForward;
   int combineShafts;
   int imageReq;
   int dispOff;
   pfuWidget *guiWidgets[PFUGUI_MAXWIDGETS];
   int drawFlag;
   int reset;
   float rudderAngle;
   float leftengineRPM;
   float rightengineRPM;
   pfVec3 bridgePosit;
   pfVec3 leftWingPosit;
   pfVec3 rightWingPosit;
   pfVec3 bridgeViewOffset;

  } DYNAMIC_DATA;

typedef struct
  {
  int vid;         // Vehicle id 
  int reset;       // Flag for rigid body object to be reset
  int rigid_body; // Flag to indicate if we are using the rigid body
  float roll;
  float pitch;
  float thrust;  // Throttle settings and keyboard speed control
  float sidethrust;
  float elevthrust;
  float heading;
  float current_time;
  float last_time;
  float delta_time;
  float rudderAngle;
  } CONTROL;

#endif
