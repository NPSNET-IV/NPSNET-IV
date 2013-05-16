
#ifndef __NPS_DISPLAY_CONST__
#define __NPS_DISPLAY_CONST__

// Startup Const

// if you are affended by this then you are stupid...
#define WINDOW_NAME "NPSNET-IV The Resurrection"

#define D_PICTURES TRUE
#define D_STEREO OFF
#define D_HMD OFF
#define D_INTRO_FONT_FILE ""

#define D_PHASE PFPHASE_FLOAT
#define D_FRAME_RATE 30.0f
#define D_MULTISAMPLE FALSE

#define LAST_INIT_EVENT 10
#define INIT_PERFORMER -2
#define LOADING_SOUNDS -1
#define DRAW_FULLY_DEFINED 0
#define LOADING_TERRAIN 2
#define LOADING_SMODELS 3
#define LOADING_DMODELS 4
#define LOADING_ANIMATIONS 5
#define OPENING_NETWORK 6
#define OPENING_IPORT 7
#define STARTING_DRAW 8
#define LOADING_TEXTURES 9
#define DONE_ALL_EVENTS 20

// Land Mark Consts
#define MAX_LANDMARKS 10
#define D_LANDMARK_FILE "datafiles/landmark.dat"

// Color Const
#define NUMCOLORS 4
#define COLOR1 0
#define COLOR2 1
#define COLOR3 2
#define COLOR4 3

// View Const
#define CHANNEL_OFF  -1
#define FORWARD_VIEW  0
#define CAMERA_VIEW   1
#define MUNITION_VIEW 2
#define WEAPON_VIEW   3
#define BIRDSEYE_VIEW 4
#define VEHICLE_VIEW  5
#define MODULO_VIEW   6
#define INFORMATION_VIEW 7

// Transparency Const
#define MIN_ALPHA 0x0f
#define MAX_ALPHA 0xff
#define ALPHA_DELTA 4
#define D_ALPHA 0xff

// Field-of-view Const
#define MIN_FOVX 10.0f
#define MAX_FOVX 150.0f
#define MIN_FOVY -1.0f
#define MAX_FOVY 180.0f
#define DELTA_FOV 1.0f
#define D_FOVX 60.0f
#define D_FOVY -1.0f

// Clipping Plane Const
#ifdef JACK
#define MIN_NEAR 0.05f
#else
#define MIN_NEAR 0.1f
#endif
#define MAX_NEAR 25000.0f
#define MIN_FAR 2.0f
#define MAX_FAR 50000.0f
#define D_CLIP_NEAR 1.0f
#define D_CLIP_FAR 7501.0f

// Radar Const
#define D_RADAR_ICON_FILE "datafiles/radar_icons.dat"
#define MAX_RADAR_RANGE 10000.0f
#define MIN_RADAR_RANGE 500.0f
#define MAX_RADAR_DIVS 30
#define RADAR_DELTA 250.0f

// Heads-up Display Const
#define HUD_MAX_VEH 25

#define MAX_HUD_OPTIONS 5
#define ENABLE_RADAR 2
#define ENABLE_CROSS 1
#define ENABLE_RINGS 3
#define ENABLE_LANDMARKS 4
#define ENABLE_BOUNDARY 5

#define D_HUD_MODE ENABLE_CROSS
#define D_ROT_MODE TRUE
#define D_COLOR_MODE TRUE
#define D_ICON_MODE TRUE
#define D_ICON_CHAR '?'
#define D_SCORING TRUE

#define D_HUD_FONT_FILE "fonts/trg.font"
#define D_FONT_WIDTH 9.0f


// Blinking Lights Const
#define MAX_BLINKING_LIGHTS 10
#define D_BLINKING_LIGHTS_FILE "datafiles/blinklights.dat"

// Animation Const
#define MAX_ANIM_COPIES 25
#define MAX_ANIMATIONS 15
#define SMALL_FLASH 0
#define MEDIUM_FLASH 1
#define LARGE_FLASH 2
#define SMALL_GUN 3
#define MEDIUM_GUN 4
#define LARGE_GUN 5
#define SMALL_SMOKE 6
#define MEDIUM_SMOKE 7
#define LARGE_SMOKE 8
#define EXPLOSION 9
#define FIREBALL 10
#define AIR_BURST 11
#define GROUND_HIT 12
#define IMPACT 13


// Window and Viewport Const
#define SVGA_X 639.0f
#define SVGA_Y 479.0f
#define NTSC_X 645.0f
#define NTSC_Y 484.0f
#define PAL_X 767.0f
#define PAL_Y 574.0f
#define D_WINDOW_LEFT 0.0f
#define D_WINDOW_RIGHT 959.0f
#define D_WINDOW_BOTTOM 0.0f
#define D_WINDOW_TOP 679.0f
#define D_VIEWPORT_LEFT 0.0f
#define D_VIEWPORT_RIGHT 1.0f
#define D_VIEWPORT_BOTTOM 0.0f
#define D_VIEWPORT_TOP 1.0f

typedef struct {
  float aspect;
  float range;
  int hud_level;
  int veh_info;
  int misc_info;
  float eyesep;
  int num_samples;
  int full_screen;
  int color;
  int metric;
  unsigned int talpha;
  int rotmode;
  int colormode;
  int iconmode;
  int stats;
  } HUD_options;

#endif
