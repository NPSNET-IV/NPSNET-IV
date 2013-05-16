
#ifndef __NPS__ENVIRONMENT_CONST__
#define __NPS__ENVIRONMENT_CONST__

// Time-of-Day Const
#define D_TIME_OF_DAY 0.7f
#define TOD_MIN 0.0f
#define TOD_MAX 1.0f
#define TOD_DELTA 0.05f

// Horizon Angle Const
#define D_HORIZ_ANGLE 7.0f
#define MIN_HORIZ_ANGLE 0.0f
#define MAX_HORIZ_ANGLE 45.0f
#define D_HORIZ_ANGLE 7.0f

// Fog Const
#define D_FOG OFF
#define MAX_FOG 25000.0f
#define DELTA_FOG 100.0f
#define D_FOG_FAR 10000.0f
#define D_FOG_NEAR 1.0f

// Cloud Const
#define D_CLOUDS FALSE
#define CLOUD_HT_CHANGE  200.0f
#define CLOUD_THICK_CHANGE 50.0f
#define CLOUD_THICKNESS 1000.0f
#define TZONE 150.0f

// Smoke Const
#define MAX_SMOKE_COLORS 5
#define NO_SMOKE        -1
#define BLACK_SMOKE     0
#define RED_SMOKE       1
#define GREEN_SMOKE     2
#define YELLOW_SMOKE    3
#define WHITE_SMOKE     4

// Ground Const
#define MIN_GROUND_HEIGHT -600.0f
#define MAX_GROUND_HEIGHT 300.0f
#define D_ES_MODE PFES_SKY_GRND
#ifndef NOSUB
#define D_GRND_HT -1000.0f
#else
#define D_GRND_HT -300.0f
#endif // NOSUB

#define D_DVW_FILE "./otherlibs/DVW/libdvw/dvw_paths.dat"

#endif
