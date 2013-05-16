/*
 *  constants.h
 *	This code was written as part of the NPS Virtual
 *	Ship project and for the thesis of Perry McDowell
 *	and Tony King.
 *	This contains the global constants used by various
 *      files.
 *
 *    Authors: Perry L.McDowell & Tony King
 *    Thesis Advisors:	Dr. Dave Pratt
 *			Dr. Mike Zyda
 *    Date:  10 January 1995
 *
 */

#ifndef __SHIP_CONSTANTS_H__
#define __SHIP_CONSTANTS_H__

#define UPDATE_MODES	0x1
#define REDRAW_WINDOW	0x2


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// tokens for resetPosition  
#define CIC  	    0
#define DC          1
#define ER          2
#define BRIDGE      3
#define PLATFORM    4
#define CIC2        5

// tokens for path  
#define DCC		14
#define ERLDR		22
#define RADAR		4
#define OPS		2
#define HT		16
#define CLEAR           20

// mouse button defines 
#define DEMO_NO_MOUSE 0
#define PICK          1
#define LEFT_MOUSE    2
#define RIGHT_MOUSE   3
#define MIDDLE_MOUSE  4


// traverse mode  
#define WALK        1
#define FLY         2

// input device defines 
#define STANDBY   0 
#define MOUSE     1
#define BIRD      2

// Valve types
#define	steamValveType	0
#define	FOValveType	1

// demo TYPES
#define DEC  	       -1
#define DEMO_RESET 	0
#define INC		1
#define WALKJACK	2
#define FOLLOW		3
#define CAMERA		4

// limits and sizes  
#define	MAX_LAMPS	8

#define	TOTAL_HORIZ_ANGLE	7.0f

#define MAX_CHANS	64

#define MAX_DECKS	32

#define MAX_SAILORS	10

#define maxDCS 16

#define NUM_DBASE_OBJECTS	15 // try 16 for sounds

#define UPDATE_DOOR_0		0
#define UPDATE_DOOR_1 		1
#define UPDATE_DOOR_2 		2
#define UPDATE_DOOR_3 		3
#define UPDATE_DOOR_4 		4
#define UPDATE_DOOR_5 		5
#define UPDATE_DOOR_6 		6
#define UPDATE_DOOR_7 		7
#define UPDATE_VALVE_0 		8
#define UPDATE_NOZZLE		9
#define UPDATE_FIRE		10
#define UPDATE_HALON		11
#define UPDATE_VENT		12
#define UPDATE_STEAM		13

#define UPDATE_DB_TIME		5.0f

#define GROWTH_FACTOR		100.0f

#define maxEntities  	10
#define localID  	0
#define DIST_DELTA	3.0f
#define ROT_DELTA	3.0f
#define ROT_DELTA	3.0f
#define ROT_DELTA	3.0f

#define VALVE_SHUT_PERCENT	0.05f
#define MIN_GROWTH		0.25f
#define steamIncrementValue 	0.05f
#define WATER_DECREMENT_VALUE  -0.003 // -0.007f was just too fast
#define COLLIDE_DIST		0.1f // default

#define HEADING		0
#define PITCH		1
#define ROLL		2
#define X		0
#define Y		1
#define Z		2
#define NOSHOW		-1

// standard conversions
#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.29577951f
#define HALFPI M_PI/2.0f
#define METER_TO_INCH 39.370079 
#define INCH_TO_METER 0.0254f
#define MILE_TO_KILOMETER 1.6093471f
#define FOOT_TO_MILE .00018939f
#define MILE_TO_FOOT 5280f
#define METER_TO_FOOT 3.2808f
#define FOOT_TO_METER 0.30488
#define POUND_TO_KG 0.454
#define KG_TO_POUND 2.2026
//#define GRAVITY 9.8

#define NET_OFF			0
#define NET_SEND_ONLY		1
#define NET_RECEIVE_ONLY	2
#define NET_SEND_AND_RECEIVE 	3

#define UPDATETIME		5.0f

#define FIND_DCS		1
#define FIND_MANIP		2
#define FIND_INFO		3
#define FIND_MULT		4


#define CIC_PVS_INDEX		1
#define WATER_PVS_INDEX		18


// tokens for special fields in MultiGen Loader  
#define PVS_CB			100
#define HARD_INTERSECT_CB	101
#define MANIPULATE_CB		102
#define INFORMATION_CB		103
#define LDR_CB			104
#define DECK_CB			105
#define BLKD_CB			106
#define DEFN_MULT_CB		107
#define MULT_CB			108
#define DOOR_CB			109
#define GAUGE_CB		110
#define VALVE_CB		111

#define MAXPOINTS               2000

#define XFEED                    180.3f
#define YFEED                    -6.1f
#define ZFEED                    6.6f
#define FEEDPOINTS               1000
#define FEEDLENGTH               0.5f
#define FEEDRADIUS               0.2f
#define FEEDDIR                  -90.0f
#define FEEDPITCH                45.0f
#define FEEDSIZE                 4.0f


#define MAX_NODES               200
#define potentialMaxPVS  	32


// intersection mask  
#define  HARD_MASK       0x80010021
#define  DCS_MASK        0x80000060
#define  MANIP_MASK      0x80000020
#define  INFO_MASK       0x800000a0
#define  BULK_MASK       0x800000e0
#define  DECK_MASK       0x80000101
#define  LADDER_MASK     0x80001001
#define  GROUND_MASK     0x00000001
#define  COLLIDE_MASK    0x00000021
#define  COLLIDEOFF_MASK 0x00010000
#define  WATER_MASK      0x80000200
#define  COLLIDE_WATER   0x00000200
#define  MULT_MASK	 0x800010a0
#define  JACK_MASK       0x80010060
#define  DOOR_MASK	 0x800020a0
#define  VALVE_MASK	 0x800020b0
#define  GAUGE_MASK	 0x800020c0


#endif
