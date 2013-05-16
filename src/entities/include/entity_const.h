
#ifndef __NPS_ENTITY_CONST__
#define __NPS_ENTITY_CONST__

#include "conversion_const.h"

// Vehicle Const
#define MAX_VEH       250
#define KILL_DIST 50.0f
#define D_FORCE ForceID_Other
#define D_STEALTH FALSE

// Teleporting Const
#define T_P_DISTANCE 100.0f
#define T_P_DI_DISTANCE 20.0f

#define MAX_SNAP_DIST 100
#define MAX_FILTER_RANGE 10000.0f
#define MIN_FILTER_RANGE 0.0f

#define DI_EYE_OFFSET 1.7f
#define DI_RIFLE_Z_OFFSET 1.0f

#define EYE_OFFSET 2.0f
#define MAXJOINTS 80
#define MAXLODS 5

#define D_MODEL_FILE "datafiles/dynamic.dat"
#define D_DRIVEN_FILE "f15c+a"

#define NOSHOW -1
#define DEAD_MODEL 1

// Human Const
#define MAX_PERSON_SPEED 8.0f*MPH_TO_MPS
#define MAX_PERSON_REVERSE_SPEED -3.0f*MPH_TO_MPS
#define MAX_PERSON_WALK_SPEED 3.0f*MPH_TO_MPS
#define MAX_PERSON_CRAWL_SPEED 2.0f*MPH_TO_MPS
#define MAX_PERSON_THRUST 10000.0f
//#define KEY_PERSON_SPEED_CHANGE 1.0f
#define KEY_PERSON_SPEED_CHANGE 0.1f
#define MAX_PERSON_REVERSE -5.0f*MPH_TO_MPS
#define FCS_PERSON_SPEED_CHANGE MAX_PERSON_SPEED*0.02f


// Land Vehicle Const
#define MAX_GROUND_SPEED 60.0f*MPH_TO_MPS
#define MAX_GROUND_THRUST 1057.0f
#define MAX_GROUND_REVERSE -30.0f*MPH_TO_MPS

// Unmanned Air Vehicle Const
#define UAV_FILENAME "pioneer"
#define UAV_FILELENGTH 7
#define MAX_UAV_SPEED 100.0f
#define UAV_STALL_SPEED 10.0f

// Rotary Wing Air Vehicle Const
#define MAX_ROTARY_SPEED 350.0f*MPH_TO_MPS
#define MAX_ROTARY_THRUST 5000.0f*MPH_TO_MPS

// Air Vehicle Const
#define SAFE_ANG 10.0f

// Rigid Body Const
#define STALL_SPEED 80.0f*MPH_TO_MPS

// General Vehicle Constraints
#define MAX_ALTITUDE 8000.0f
#define MAX_SPEED 1000.0f*MPH_TO_MPS
#define NO_SPEED 0.0f
#define MAX_REVERSE -100.0f*MPH_TO_MPS

// Input Translation Constaints
#define KEY_ROLL_CHANGE 1.0f
#define KEY_PITCH_CHANGE 1.0f
#define KEY_SPEED_CHANGE 5.0f
#define STEALTH_KEY_SPEED_CHANGE 0.25f
#define MAX_THRUST 105700.0f
#define THRUST_TO_SPEED MAX_SPEED/MAX_THRUST
#define FCS_SPEED_CHANGE MAX_SPEED*0.002f

// Vehicle Type Const
#define MAX_VEHTYPES  230

// Internal NPS Entity Status
#define ALIVE 0
#define DYING 1
#define DEAD 2
#define AIRDEAD 3
#define GRNDDEAD 4

// Causes of Death
#define GROUND_DEATH 1
#define FOILAGE_DEATH 2
#define WATER_DEATH 3
#define S_OBJECT_DEATH 4
#define D_OBJECT_DEATH 5
#define MUNITION_DEATH 6
#define BRIDGE_DEATH 7
#define FALLING_DEATH 8
#define UNDER_GROUND_DEATH 9

// Timer Values
#define DEAD_TIME_DELAY 4.0f
#define GROUND_SLAM_DELAY 3.0f
#define CRASH_TIME 5.0f
#define DEAD_TIME 20.0f
#define DRIVEN_DEAD_TIME 5.0f

// Targetting Const
#define AIM_DISTANCE 10000.0f
#define AIM_OFFSETS 1.0f
#define NO_LOCK 0
#define BLUE_LOCK 1
#define RED_LOCK 2
#define WHITE_LOCK 3
#define OTHER_LOCK 4

// Munition Const
#define MAX_FIRE_INFO 3
#define MAX_MUNITIONS 30
#define MUNITION_OFFSET (MAX_VEH+MAX_VEH)
#define D_MUNITION_INDEX 201
#define START_WEAPON 200

// Physically Based Modelling Const
#define GRAVITY 9.8f
#define BOMB_DRAG 0.02f
#define TORPEDO_DRAG 0.001f

// Munition Ranges
#define MAX_MISSLE_RANGE 10000.0f
#define MAX_BULLET_RANGE 5000.0f
#define WEAP_RANGE 10000.0f
#define MAX_TORP_RANGE 2000.0f

#define D_TRANSPORT_FILE "datafiles/transport.dat"
#define MAX_TRANSPORT 16

#endif
