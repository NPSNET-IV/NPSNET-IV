// File: <common_defs.h>

/*
 * Copyright (c) 1995,
 *    Naval Postgraduate School
 *    Computer Graphics and Video Laboratory
 *    NPSNET Research Group
 *    npsnet@cs.nps.navy.mil
 *
 *
 *    Permission to use, copy, and modify this software and its
 * documentation for any non-commercial purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Naval Postgraduate School
 * Computer Graphics and Video Laboratory and the NPSNET Research Group
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your 
 * software generates.
 *    This restriction helps justify our research efforts to the
 * sponsors who fund our research.
 *
 *    Do not redistribute this code without the express written
 * consent of the NPSNET Research Group. (E-mail communication and our
 * confirmation qualifies as written permission.) As stated above, this 
 * restriction helps justify our research efforts to the sponsors who
 * fund our research.
 *
 *    This software was designed and implemented at U.S. Government
 * expense and by employees of the U.S. Government.  It is illegal
 * to charge any U.S. Government agency for its partial or full use.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * E-Mail addresses:
 *    npsnet@cs.nps.navy.mil
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */


#ifndef __NPS_COMMON_DEFS__
#define __NPS_COMMON_DEFS__

#include <pf.h>
#include "pdu.h"
#include "rwstruct.h"
#include "setup_const.h"
#include "display_const.h"
#include "nps_device.h"
#include "highlight.h"
#include "identify.h"
#include "XCursorUtil.h"

#include "hmdClass.h"
#include "fakespaceClass.h"
#include "lw_tcpip.h"

#define MAXPIPES 1
#define MAXCHANPERPIPE 5

#ifndef NO_DVW
#define NO_PFSMOKE
#define NO_NPSSMOKE
#endif

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#include <pfutil.h>

#define MAX_PUFFS 32
#define PFUMAXSMOKES 50

typedef struct pfuPuff {
    float t, transp; double startTime; float radius; pfVec3 origin;
    pfMatrix rotMat; pfVec3 coords[4]; pfVec3 tcoords[4]; float speed;
    pfVec3 direction;
} pfuPuffStruct;

struct _pfuSmoke {
    pfVec3 bgnColor, deltaColor;
        float dissipation, density, puffInterval, duration;
    double startTime, prevTime, lastPuffTime; long type, mode;
    float frequency; pfVec3     direction; float speed, turbulence;
    pfVec3 origin; float radius, expansion, offset; pfTexture *tex;
    pfuPuff     *puffs[MAX_PUFFS]; long startPuff, numPuffs;
};
#endif



enum npsnetmode { NET_OFF,
                  NET_SEND_ONLY,
                  NET_RECEIVE_ONLY, 
                  NET_SEND_AND_RECEIVE };

//the class types of entities
enum vtype { VEH_TYPE = -1,
             FLYING_CARPET, // 0
             GND_VEH,       // 1
             AIR_VEH,       // 2
             FIXED_WING,    // 3
             ROTARY_WING,   // 4
             SHIP,          // 5
             SUBMERSIBLE,   // 6
             PERSON,        // 7
             UNMANNED,      // 8
             DI_GUY,        // 9
             JACK_DI,       // 10 
             JACK_TEAM,     // 11
             PB_ROTARY_WING,// 12
#ifndef NODUDE
             DUDE,          // 13
             JADE,          // 14
#endif // NODUDE
             };

//the types of weapons
enum mtype { 
   MINACTIVE = -1,
   MUNITION_TYPE,      // 0
   BULLET_TYPE,        // 1
   MISSILE_TYPE,       // 2
   BOMB_TYPE,          // 3
   TORPEDO_AL_TYPE    // 4
#ifndef NOSUB
   ,   
   TORPEDO_SL_TYPE,    // 5  
   MISSILE_SL_TYPE
#endif
};   // 6  


enum wdestype { NOWEAPON = -1, 
                PRIMARY,   // 0
                SECONDARY, // 1
                TERTIARY}; // 2

//result of movement functions
enum movestat {FINEWITHME, DEACTIVE,HITSOMETHING};

//the types of animations control for humans
enum ctltype {CTL_TYPE= -1, JACKML, IPORT, OTHER};

// Who controls a particular entity
enum entity_control { LOCAL_CTL, NETWORK_CTL, REMOTE_CTL };

enum window_type { WIN_CLOSED, WIN_CUSTOM, WIN_SVGA, WIN_FULL, WIN_NTSC,
                   WIN_PAL, WIN_STEREO, WIN_NORMAL,
                   WIN_VIM, WIN_THREE_TV, WIN_THREE_BUNKER,
                   WIN_TWO_TPORT, WIN_FS_MCO, WIN_FS_ST_BUFFERS,
                   WIN_TWO_ADJACENT };

typedef void (*HUD_function)(pfChannel *chan, void *data);

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
   float xmin;
   float xmax;
   float xsize;
   float ymin;
   float ymax;
   float ysize;
   } BOUNDING_BOX;

typedef struct
   {
   int active;
   pfChannel *chan_ptr;
   VIEWPORT_REC viewport;
   float field_of_view[2];
   NEAR_FAR_REC clip_plane;
   pfEarthSky *earth_sky;
   pfFog *fog_ptr;
   pfVec3 fog_color;
#ifndef NOSUB
   pfFog *waterfog_ptr;   
   pfVec3 waterfog_color;
#endif
   pfVec3 cloud_bot_color;
   pfVec3 cloud_top_color;
   float cloud_thick;
   float cloud_bottom;
   float cloud_top;
   NEAR_FAR_REC fog_plane;
#ifndef NOSUB
   NEAR_FAR_REC waterfog_plane;
#endif // NOSUB
   long earth_sky_mode;
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
   CHANNEL_DATA channel[MAXCHANPERPIPE];
   } PIPE_DATA;
  
/* Define the shared data record variable to be used by all processes. */

typedef struct
   {
   pfVec2 location;
   float elevation;
   unsigned char force;
   int type;
   unsigned int status;
   int vid;
   } HUD_VEH_REC;

typedef struct
   {
   int min;
   int max;
   char icon_char;
   } RADAR_INFO;

typedef struct
   {
   int bdihml;
   char terrain_file[FILENAME_SIZE];
   char model_file[FILENAME_SIZE];
   char location_file[FILENAME_SIZE];
   char sound_file[FILENAME_SIZE];
   char network_file[FILENAME_SIZE];
   char filter_file[FILENAME_SIZE];
   char driven_file[FILENAME_SIZE];
   char hud_font_file[FILENAME_SIZE];
   char intro_font_file[FILENAME_SIZE];
   char landmark_file[FILENAME_SIZE];
   char transport_file[FILENAME_SIZE];
   char fcs_port[PORTNAME_SIZE];
   int fcs_mp;
   char flybox_port[PORTNAME_SIZE];
   char leather_flybox_port[PORTNAME_SIZE];

// mcmillan
//   char traker_port[PORTNAME_SIZE];
//   char traker_file[FILENAME_SIZE];
//   int  traker_exists;
   hmdClass *hmd_unit;
   char hmd_fastrak_file[FILENAME_SIZE];

   char kaflight_port[PORTNAME_SIZE];
   char radar_icon_file[FILENAME_SIZE];
   char blinking_lights_file[FILENAME_SIZE];
   char round_world_file[FILENAME_SIZE];
   roundWorldStruct rw;
   char identify_file[FILENAME_SIZE];
#ifndef NO_DVW
   char dvw_file[FILENAME_SIZE];
#endif
   char search_path[MAX_SEARCH_PATH];
   char marking[MARKINGS_LEN];
   RADAR_INFO radar[MAX_RADAR_DIVS]; 
   pfCoord init_location;
   int num_radar_divs;
   int fcs_prompt;
   int kaflight_prompt;
   int num_pipes;
   long process_mode;
   float frame_rate;
   long phase;
   int cpu;
   int reality_graphics;
   int max_samples;
   int stereo_exists;
   int sball_exists;
   int fcs_exists;
   int flybox_exists;
   int leather_flybox_exists;
   int kaflight_exists;
#ifndef NOBASICPANEL
   int remote_exists;
#endif
#ifndef NO_IPORT
   int treadport_exists;
   int uniport_exists;
   int testport_exists;
   char testport_file[FILENAME_SIZE];
   char uniport_file[FILENAME_SIZE];
   char treadport_file[FILENAME_SIZE];
#endif // NO_IPORT
#ifndef NOUPPERBODY
   int upperbody_exists;
   char upperbody_file[FILENAME_SIZE];
#endif // NOUPPERBODY
   int waypnt_fd;
   int pdupos_fd;
   int waypoints;
   int pdu_position;
   int scoring;
   int pictures;
   int stealth;
   int covert;
   int UAV;
   int sound;
   SimulationAddress DIS_address;
   char hostname[80];
#ifdef BIRD
   // BIRD, by Jiang Zhu
   char bird_port[PORTNAME_SIZE];
   int bird_exists;
#endif
   int load_dynamic_models;
   float DIS_deactivate_delta;
   float DIS_heartbeat_delta;
   float DIS_orientation_delta;
   float DIS_position_delta;
   char pic_dir[255];
#ifndef NOC2MEASURES
   char po_ctdb_file[FILENAME_SIZE];
   unsigned char po_exercise;
   unsigned char po_database;
   unsigned short po_port;
#endif
   fsInfoStruct fsInit;
   char odt_port[128];
   int odt_mp;
   lwStruct lwInit;
   } STATIC_DATA;

typedef struct
   {
   int dead_switch;
   u_char exercise;
   PIPE_DATA pipe[MAXPIPES];
   pfPipeWindow *pw;
   ulong notify_level;
   int fog;
   int fog_type;
   NPS_Input_Device control;
   float tod;
   int clouds;
   int stereo;
   int camera;
   int camlock;
   int birdseye;
   float birdsdis;
   pfCoord birdsview;
   pfVec3 locked_view;
   int hmd;
   int paused;
   int tethered;
   float attach_dis;
   int multisample;
   float aspect;
   float fovx;
   float fovy;
   NEAR_FAR_REC clip_plane;
   VIEWPORT_REC view;
   pfVec2 mouse;
   long originX;
   long originY;
   pfTexture **texture_list_ptr;
   long number_of_textures;
   int hitveh;
   int init_event;
   int exit_flag;
   int new_vid;
   int new_vehicle;
   int transport;
   char  TDBoverride;  //override the size of the terrain in config S1k loader
   float TDBoffsets[2]; //these is is the values to normalize the location
   BOUNDING_BOX bounding_box;
   int use_bounding_boxes;
   int dead;
   int cause_of_death;
   int target;
   int target_lock;
   int targetting;
   ForceID force;
   int early_warn;
   int visual_mode;
   int visual_active;
   int static_hits;
   int dynamic_hits;
   int bullets_fired;
   int missiles_fired;
   int bombs_fired;
   int net_mode;
   int specific;
#ifdef BIRD
   int bird_control;
#endif
   int pick_mode;
   pfVec2 transport_pos;
   int last_selected_vid;
   int nearest_radar_vid_to_mouse;
   npsnetmode network_mode;
#ifndef NOHIRESNET
   npsnetmode hr_network_mode;
#endif // NOHIRESNET
   HUD_function hud_callback;
#ifndef NOC2MEASURES
   Highlight *HL;
#endif
#ifndef NO_PFSMOKE
   pfuSmoke *smokes[PFUMAXSMOKES]; // Vehicle smoke plumes
   pfuSmoke *flames[PFUMAXSMOKES]; // Vehicle fires
   pfuSmoke *msmoke[PFUMAXSMOKES]; // Missile contrails
   pfuSmoke *trails[PFUMAXSMOKES]; // Dust trails
   pfuSmoke *lsmoke[PFUMAXSMOKES]; // Land smoke plumes
   pfTexture *smokeTex, *fireTex, *missileTex;
#endif
   IDENTIFY *identify;
#ifndef NOSUB
   float inital_torpedo_course;
   float inital_tlam_course;
#endif // NOSUB
#ifndef NO_DVW
   int cloud_primitive_switch;
#endif
   window_type window_size;
   int dynamic_terrain;
   EntityID master;
   XCursorUtil *XCursor;
   int useAuto;
   float max_alt;
   float fs_scale;
   } DYNAMIC_DATA;
   
typedef struct
   {
   unsigned short site;
   unsigned short host;
   char *ether_interf;
   } NET_DATA;

typedef struct
   {
   float ltamb;
   float ltcol;
   float lmamb;
   float px, py, pz;
   pfLight *lt;
   pfLightModel *lm;
   int changed;
   } SUN;

//the node data that is attached to the pfSwitch node
typedef struct {
  pfSwitch *node;
}NODEDATA;

//the switch node data used to blow up buildings
struct switchnodedata{ 
  pfSwitch *node;
  float timeoff;
  long status;
  int data;
  switchnodedata *next;
};

typedef struct
   {
   int active;
   pfVec3 color;
   pfSequence *animation;
   pfLightPoint *light[2];
   } LIGHTREC;

typedef struct switchnodedata SWITCHNODEDATA;

typedef struct
   {
   pfDCS *dcs;
   pfSequence *sequence;
   int active;
   } ANIMATION;

typedef struct
   {
   int numloaded;
   int next;
   ANIMATION animation[MAX_ANIM_COPIES];
   } ANIMATION_LIST;

#endif
