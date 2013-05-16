
#ifndef __NPS_INTERFACE_CONST__
#define __NPS_INTERFACE_CONST__

// Shared Memory Input Const
#define INPUT_POOL_FILE "NPSNETIV_IO"
#define MAX_IN_QUEUE 256

// Stereo Const
#define DELTA_EYESEP 0.001270f
#define MIN_EYE_SEP 0.01f
#define MAX_EYE_SEP 0.9
#define D_EYE_DIS 0.06350f

// Entity Picking Modes
#define MAX_PICK_RANGE 450.0f
#define PO_PICK 0
#define TWO_D_RADAR_INFO 1
#define TWO_D_RADAR_PICK 2
#ifndef NONPSSHIP
  #ifndef NOAHA
  #define SHIP_PICK 3
  #define TWO_D_AHA_INFO 4
  #define TWO_D_AHA_PICK 5
  #define THREE_D_AHA_INFO 6
  #define THREE_D_AHA_PICK 7
  #define MAX_PICK_MODES 6
  #define D_PICK_MODE TWO_D_RADAR_INFO
  #else
  #define SHIP_PICK 3
  #define MAX_PICK_MODES 4
  #define D_PICK_MODE TWO_D_RADAR_INFO
  #endif
#else
  #ifndef NOAHA
  #define TWO_D_AHA_INFO 3
  #define TWO_D_AHA_PICK 4
  #define THREE_D_AHA_INFO 5
  #define THREE_D_AHA_PICK 6
  #define MAX_PICK_MODES 5
  #define D_PICK_MODE TWO_D_RADAR_INFO
  #else
  #define MAX_PICK_MODES 3
  #define D_PICK_MODE TWO_D_RADAR_INFO
  #endif
#endif

// SpaceBall Const
#define MIN_SBSENSE 0.1f
#define MAX_SBSENSE 1000.0f
#define D_SBSENSE 1.0f
#define SBSENSE_DELTA 0.9f
#define SB_ROT_SCALE 500.0f
#define SB_TRANS_SCALE 50.0f

// Thrust Master FCS/WCS/RCS Const
#define D_FCS_PORT ""
#define D_FCS_PROMPT ON

// Kinney Aerospace Stick/Throttle Const
#define D_KAFLIGHT_PORT ""
#define D_KAFLIGHT_PROMPT OFF

// BG Systems Flybox Const
#define D_FLYBOX_PORT ""

// Polhemus Fastrak Const
#define D_HMD_FASTRAK_FILE ""

// Ascension Bird Const
#ifdef BIRD
#define LEFTBIRD 2
#define MIDDLEBIRD 1
#define RIGHTBIRD 0
#define BIRD_BUTTON_TIME 1.0f
#define D_BIRD_PORT "NONE"
#define D_BIRD_CONTROL FALSE
#endif

#endif
