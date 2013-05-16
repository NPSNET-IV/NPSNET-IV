#ifndef _SHIP_DEFS_H
#define _SHIP_DEFS_H

#define UPDATE_MODES 0x1
#define REDRAW_WINDOW 0x2
#define X 0
#define Y 1
#define Z 2
#define HEADING 0
#define ROLL 2
#define PITCH 1
#define FLY 10
#define BRIDGE 12
#define LEFT_WING 14
#define RIGHT_WING 15

/* General */
#define VERSION "NPSNET-IV.6+"
#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.29577951f
#define HALFPI M_PI/2.0f
#define METER_TO_INCH 39.370079 
#define INCH_TO_METER 0.0254f
#define MILE_TO_KILOMETER 1.6093471f
#define METER_TO_FOOT 3.2808f
#define MPS_TO_KNOTS 1.94f
#define MAX_OBJECTS 250
#define REAL_SMALL 0.0001f
#define SMALL 0.5


/* DIS tolerances */
#define DISDELTAW 2.0f
#define ROT_DELTA       3.0f    /*Max delta for rotational delta*/
#define DIST_DELTA      3.0f    /*Max delta for distance delta*/ 
#define UPDATETIME      5.0f

/*Object stuff */
#define LAND_OBJECT 0
#define OFFSHORE_OBJECT 1
#define BUOY_OBJECT 3
#define BRIDGE_OBJECT 2

#define MAX_SHIPS 250
#define MAX_SHIPTYPES 50

#define MAXPIPES 2
#define D_DRIVEN_FILE "cg52+a"
#define D_TERRAIN_FILE "bayNorth.flt"
#define D_MODEL_FILE "/cm/npsnet/datafiles/shipsim/ship_models.dat"
#define D_NETWORK_FILE "/cm/npsnet/datafiles/hosts.dat"
#define D_OBJECT_FILE "/cm/npsnet/datafiles/shipsim/sfbayModels.dat"


#define NORTH_BAY_X 43367.914f
#define NORTH_BAY_Y 89900.55f
#define NORTH_BAY_Z 2.0f
#define D_BBOX_XMIN 0.0f
#define D_BBOX_YMIN 0.0f
#define D_BBOX_XMAX 25000.0f
#define D_BBOX_YMAX 50000.0f
#define D_ESKY PFES_SKY
#define D_GROUND_HEIGHT -600.0f
#define D_NEARCLIP 1.0f
#define D_FARCLIP 17000.0f
#define D_FOVX 60.0f
#define D_FOVY -1.0f


#define NOT_HERE -1
#define CG52_BRIDGEPOSIT_X 0.0f
#define CG52_BRIDGEPOSIT_Y 40.0f
#define CG52_BRIDGEPOSIT_Z 16.5f
#define CG52_RIGHTWING_X 9.0f
#define CG52_RIGHTWING_Y 34.0f
#define CG52_RIGHTWING_Z 20.3f


#define DNOTIFY PFNFY_INFO
#define DMULTPROCESS PFMP_DEFAULT

#define FILENAME_SIZE 20


//Offsets for intersection testing
#define HULL_LENGTH 80.0f
#define HULLOFFSET 8.0f
#define HALFHULLOFFSET 4.0f 
#define HALFBEAM_LENGTH 30.0f

//define masks for intersection testing
#define ENTITY_MASK 0x01
#define WATER_MASK 0x02
#define LAND_OBJ_MASK 0x08
#define BRIDGE_MASK 0x04
#define BUOY_MASK 0x10
#define OFFSHORE_MASK 0x14
#define ALL_MASK        00FFFFFF
#define PERMANENT_WATER 2
#define PERMANENT_SHIFT 0
#define ENTITY_KIND_SHIFT     0
#define ENTITY_FORCE_SHIFT    6
#define DESTRUCT_MASK         0x00FF0000 /* Third byte */

//Max number of course changes for playback
#define MAXCOURSECHANGES 500
#endif
