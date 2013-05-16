/* defaults.h */

#include "setup_const.h"
#include "communication_const.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define PUT 197
#define GET 198

/* Default file locations and type designations */

	/* write out system defaults */
#define DUMP_FILENAME		"./dump"

#define ANIMATION_FILETYPE	1
/*
#define ANIMATION_DIR		"datafiles/animations/"
*/
#define IM_ANIMATIONS_FILE	"datafiles/animations.dat"

#define BLINKING_FILETYPE	2
#define BLINKING_FILENAME	""

/* #ifndef D_DVW_FILE
#define D_DVW_FILE		"NONE"
#endif */

#define FILTER_FILETYPE		3
#define FILTER_FILENAME		""

#define HUD_FONT_FILENAME	"datafiles/npsnetIV.font"

#define LANDMARK_FILETYPE	4
#define LANDMARK_FILENAME	""

#define LOCATION_FILETYPE	5
#define LOCATION_FILENAME	""

#define MASTER_CONFIG_FILETYPE	6
#define CONFIG_DIR		"config/"

#ifndef D_MODEL_FILE
#define D_MODEL_FILE		"datafiles/dynamic.dat"
#endif
#define MODEL_FILETYPE		7
#define MODEL_FILENAME		D_MODEL_FILE

#define NETWORK_FILETYPE	8
#define NETWORK_FILENAME	D_NETWORK_FILE

#define RADAR_ICON_FILETYPE	9
#define RADAR_ICON_FILENAME	""

#define ROUND_WORLD_FILETYPE	10
#define ROUND_WORLD_FILENAME	""

#define SOUNDS_FILETYPE		11
#define SOUNDS_FILENAME		""

#ifndef D_TERRAIN_FILE
#define D_TERRAIN_FILE		"datafiles/terrain.dat"
#endif
#define TERRAIN_FILETYPE	12
#define TERRAIN_FILENAME	D_TERRAIN_FILE

#define TRANSPORT_FILETYPE	13
#define TRANSPORT_FILENAME	""

/* Default values */

#define DEF_VIEW_HEADING	180.0

#define DEF_VIEW_X		17407.0
#define DEF_VIEW_Y		11898.0
#define DEF_VIEW_Z		5.0

#ifndef DEFAULT_MC_GROUP
#define DEFAULT_MC_GROUP	"224.2.121.93"
#endif
#ifndef DEFAULT_MC_TTL
#define DEFAULT_MC_TTL		3
#endif

#ifndef IDU_DEF_MC_GROUP
#define IDU_DEF_MC_GROUP "224.200.200.200"
#endif
#ifndef IDU_DEF_MC_TTL
#define IDU_DEF_MC_TTL 3
#endif
#ifndef IDU_DEF_MC_PORT
#define IDU_DEF_MC_PORT 3200
#endif

#ifndef MONITOR_HERTZ
#define MONITOR_HERTZ		60
#endif

#ifndef D_PO_CTDB_FILE
#define D_PO_CTDB_FILE		"NONE"
#endif
#ifndef D_PO_DATABASE
#define D_PO_DATABASE		1
#endif
#ifndef D_PO_EXERCISE
#define D_PO_EXERCISE		1
#endif
#ifndef D_PO_PORT
#define D_PO_PORT		3002
#endif

#ifndef YMAXSCREEN
#define YMAXSCREEN		1023
#endif

/* Animation types */
#define TYPE_MODEL		1
#define TYPE_BB			2

/* Model Types */
#define TYPE_FLT                1
#define TYPE_TIPS               2
#define TYPE_JCD                3
#define TYPE_JADE               4
#define TYPE_DIGUY              5

/* Filter types */
#define TYPE_BLOCK		1
#define TYPE_FORCE		2

/* EOF */
