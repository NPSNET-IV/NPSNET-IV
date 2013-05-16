%{ /* parser.y -- Grammar specification */

#include <stdio.h>
#include <errno.h>
extern int errno;

#include <Performer/pf.h>

#include "pdu.h"
#include "mangle.h"
#include "setup_const.h"

#include "imstructs.h"
#include "imdefaults.h"
extern struct defs *Defaults;
extern struct MC *mc;

#ifdef __cplusplus
extern "C" {
#endif
extern int yylex();
extern int yyparse();
extern void yyerror();
#ifdef __cplusplus
}
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

int boolean;
int i, j, index, len;
char tempstr[MAX_SEARCH_PATH];
extern int error_reported, error_reported2;
%}

%union {
   int		cmd;
   char		string[128];
   char		single;
   int		ivalue;
   float	fvalue;
}

%token <cmd> INT
%token <string> STRING
%token <fvalue> FLOAT

	/* Note "IM_" prefix added where symbols conflict in npsnet */
%token IM_ON IM_OFF
%token IM_NONE IM_EMPTY

	/* Master Configuration File commands */
%token <cmd> BLINKING_LIGHTS_FILE IM_BOUNDING_BOX BDIHML C2_FILE
%token <cmd> DIGUY_SPEED_RATIO DIGUY_POSITION_ERROR DIGUY_TIME_CONSTANT
%token <cmd> DIGUY_TRANS_SMOOTHNESS TRANS_IMMEDIATE IMMEDIATE_TRANS
%token <cmd> TRANS_SMOOTHEST DIGUY_POSITION_MODE SMOOTH_MODE PRECISE_MODE
%token <cmd> DIGUY_POSITION_MECHANISM DIGUY_INTERNAL NPSNET_INTERNAL
%token <cmd> CHANNEL CLIP_PLANES IM_CONTROL CPU_LOCK DB_OFFSET DB_PAGES_ALLOWED
%token <cmd> DB_TSG_FILE USE_ROUND_WORLD DUMP_FILE EARTH_SKY_MODE DEAD_SWITCH
%token <cmd> ENVIRONMENTAL ENTITY_FILE DIS_EXERCISE FCS_MP FCS_PORT FCS_PROMPT
%token <cmd> DIS_ENUM DISv203 DISv204
%token <cmd> FILTER_FILE FOG FOG_NEAR FOG_FAR FOG_TYPE
%token <cmd> FOG_VTX_LIN FOG_VTX_EXP FOG_VTX_EXP2 FOG_PIX_LIN FOG_PIX_EXP
%token <cmd> FOG_PIX_EXP2 FOG_PIX_SPLINE
%token <cmd> FORCE FOV IM_FRAME_RATE GROUND_HEIGHT HMD
%token <cmd> HEAD_MOUNTED_DISPLAY HORIZON_ANGLE
%token <cmd> HUD_FONT_FILE HUD_LEVEL INTRO_FONT_FILE LANDMARK_FILE
%token <cmd> LOCATION_FILE MARKING MASTER MODEL_FILE MULTISAMPLE NETWORK_FILE
%token <cmd> NOTIFY_LEVEL PDUPOSITION PHASE PICTURES PIPE PROCESS_MODE
%token <cmd> ROAD_FILE ROUND_WORLD_FILE SCORING IM_SOUND SOUND_FILE STEALTH
%token <cmd> STEREO TARGET_ENTITY IM_TERRAIN_FILE TRAKER_FILE TRAKER_PORT
%token <cmd> TARGET_BOUNDING_BOX
%token <cmd> TRANSPORT_FILE VEHICLE_DRIVEN VIEW VIEWPORT VIEW_HPR VIEW_XYZ
%token <cmd> WAYPOINTS IM_WINDOW LOD_SCALE LOD_FADE IM_WEAPON_VIEW
%token <cmd> LW_RECEIVE_PORT LW_SEND_PORT LW_MACHINE_NAME

	/* Master Configuration File new (npsnet4.8) tokens */
%token <cmd> ANIMATIONS DIS_HEARTBEAT DIS_POSDELTA DIS_ROTDELTA DIS_TIME_OUT
%token <cmd> DYNAMIC_OBJS ETHER_INTERF FCS_CHECK FLYBOX MULTICAST
%token <cmd> MULTICAST_GROUP MULTICAST_TTL NETWORKING IM_PORT
%token <cmd> PANEL_GROUP PANEL_TTL PANEL_PORT
%token <cmd> REMOTE_INPUT_PANEL STATIC_OBJS WINDOW_SIZE SEARCH_PATH
%token <cmd> IM_PO_EXERCISE IM_PO_PORT IM_PO_DATABASE PO_CTDB_FILE
%token <cmd> RADAR_ICON_FILE TREADPORT UNIPORT TESTPORT UPPERBODY MODSAF_MAP
%token <cmd> IDENTIFY_FILE FLYBOX_PORT NPS_PIC_DIR HIRES
%token <cmd> IM_NET_OFF RECEIVE_ONLY SEND_ONLY SEND_AND_RECEIVE
%token <cmd> LEATHER_FLYBOX_PORT LEATHER_FLYBOX_CHECK DVW_PATHS
%token <cmd> HMD_FILE ANIMATION_FILE IM_JACK AUTO_FILE AUTO_STATE
%token <cmd> RANGE_FILTER IM_MAX_ALTITUDE

	/* Master Configuration File additional tokens */
%token <cmd> CURSOR_FILE FS_CONFIG FS_PORT FS_SCALE
%token <cmd> FCS FLYBOXIN KAFLIGHT KEYBOARD SPACEBALL LEATHER_FLYBOX DRIVE PAD
%token <cmd> BALL SHIP_PANEL SUB_PANEL AUSA_ODT
%token <cmd> FAST TAG SKY SKY_GROUND SKY_CLEAR
%token <cmd> IM_BLUE IM_RED IM_WHITE IM_OTHER
%token <cmd> IM_FATAL WARN NOTICE INFO IM_DEBUG FPDEBUG
%token <cmd> ALG_FREE ALG_FLOAT ALG_LOCK
%token <cmd> APPCULLDRAW APP_CULLDRAW APPCULL_DRAW APP_CULL_DRAW IM_DEFAULT
%token <cmd> AIRPORT CANYON VILLAGE PIER IM_START
%token <cmd> CLOSED CUSTOM SVGA FULL NTSC PAL NORMAL VIM THREE_TV
%token <cmd> THREE_BUNKER TWO_TPORT TWO_ADJACENT FS_MCO FS_ST_BUFFERS ODT_PORT ODT_MP

	/* Animation File commands */
%token <cmd> ANIMATION_DEF SEQ_CYCLE SEQ_SWING
%token <cmd> BB_SEQ MODEL_SEQ

	/* Blinking Lights File commands */
%token <cmd> LIGHT

	/* Filter File commands */
%token <cmd> BLOCK_ESPDU FORCE_APPEARANCE

	/* Landmark File commands */
%token <cmd> LANDMARK_ICON

	/* Location File commands */
%token <cmd> MODEL_DEF1 IM_POSITION

	/* Model File commands */
%token <cmd> MODEL_DEF2 JACK_DEF WEAPON_DEF
%token <cmd> DUDE_DEF JADE_DEF DIGUY_DEF

	/* Network File commands */
%token <cmd> IM_HOST

	/* Radar Icon File commands */
%token <cmd> RADAR_ICON

	/* Round World File commands */
%token <cmd> UTM_MAP

	/* Sounds File commands */
%token <cmd> SOUND_DEF

	/* Terrain File commands */
%token <cmd> MODEL_DEF3
%token <cmd> BRIDGE_MASK FOLIAGE_MASK TERRAIN_MASK TUNNEL_MASK WATER_MASK
%token <cmd> DB_PAGING FLAT_WORLD

	/* Transport File commands */
%token <cmd> LOCATION

%%

commands: /* terminal condition */
	| commands Acommand		/* Animation File grammar */
	| commands BLcommand		/* Blinking Lights File */
	| commands Fcommand		/* Filter File */
	| commands LMcommand		/* Landmark File */
	| commands Lcommand		/* Location File */
	| commands MCcommand		/* Master Config File */
	| commands Mcommand		/* Model File */
	| commands Ncommand		/* Network File */
	| commands RIcommand		/* Radar Icons File */
	| commands RWcommand		/* Round World File */
	| commands Scommand		/* Sounds File */
	| commands Tcommand		/* Terrain File */
	| commands TFcommand		/* Transport File */
	;

/* ANIMATION FILE */
Acommand: ANIMATION_DEF STRING INT INT SEQ_CYCLE INT
	{
	   index = $3;
	   if (i < MAX_ANIMATIONS) {
	      Defaults->Animations[index].count = $4;
	      strncpy(Defaults->Animations[index].identifier, $2, STRING32);
	      Defaults->Animations[index].mode = PFSEQ_CYCLE;
	      Defaults->Animations[index].reps = $6;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Animation index %d exceeds ", $3);
	         printf("allowable in Animation File (>%d).\n",
	                                          MAX_ANIMATIONS);
	         error_reported = TRUE;
	      }
	   }
	}
	| ANIMATION_DEF STRING INT INT SEQ_SWING INT
	{
	   index = $3;
	   if (i < MAX_ANIMATIONS) {
	      Defaults->Animations[index].count = $4;
	      strncpy(Defaults->Animations[index].identifier, $2, STRING32);
	      Defaults->Animations[index].mode = PFSEQ_SWING;
	      Defaults->Animations[index].reps = $6;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Animation index %d exceeds ", $3);
	         printf("allowable in Animation File (>%d).\n",
	                                          MAX_ANIMATIONS);
	         error_reported = TRUE;
	      }
	   }
	}
	;

Acommand: MODEL_SEQ STRING FLOAT
	{
	   i = Defaults->Animations[index].num_effects;
	   if (i < MAX_ANIM_COPIES) {
	      Defaults->Animations[index].Effect[i].effect_type = TYPE_MODEL;
	      strncpy(Defaults->Animations[index].Effect[i].filename,
	                                                      $2, STRING128);
	      Defaults->Animations[index].Effect[i].delay = $3;
	      Defaults->Animations[index].num_effects++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many effects in Animation File (>%d).\n",
	                                                   MAX_ANIM_COPIES);
	         error_reported = TRUE;
	      }
	   }
	}
	;

Acommand: BB_SEQ STRING FLOAT
	{
	   i = Defaults->Animations[index].num_effects;
	   if (i < MAX_ANIM_COPIES) {
	      Defaults->Animations[index].Effect[i].effect_type = TYPE_BB;
	      strncpy(Defaults->Animations[index].Effect[i].filename,
	                                                      $2, STRING128);
	      Defaults->Animations[index].Effect[i].delay = $3;
	      Defaults->Animations[index].num_effects++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many effects in Animation File (>%d).\n",
	                                                   MAX_ANIM_COPIES);
	         error_reported = TRUE;
	      }
	   }
	}
	;

/* SOUND FILE */
Scommand: SOUND_DEF INT STRING FLOAT
	{
	   i = $2;	/* sound number */
	   if (i < MAX_SOUND_EVENTS) {
	      j = Defaults->Sounds[i].num_events;
	      if (j < MAX_SOUNDS_PER_EVENT) {
	         strncpy(Defaults->Sounds[i].Events[j].filename, $3, STRING128);
	         Defaults->Sounds[i].Events[j].range = $4;
	         Defaults->Sounds[i].num_events++;
	      } else {
	         printf("Too many events for sound %d in Sound File (>%d).\n",
	                                   $2, MAX_SOUNDS_PER_EVENT);
	      }
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf(
	            "Sound index %d exceeds allowable in Sound File (>%d).\n",
	                                          $2, MAX_SOUND_EVENTS);
	         error_reported = TRUE;
	      }
	   }
	}
	;

/* NETWORK FILE */
Ncommand: IM_HOST STRING INT INT STRING
	{
	   i = Defaults->num_hosts;
	   if (i < MAX_HOSTS) {
	      strncpy(Defaults->Hosts[i].hostname, $2, STRING64);
	      Defaults->Hosts[i].siteno = $3;
	      Defaults->Hosts[i].hostno = $4;
	      strncpy(Defaults->Hosts[i].interf, $5, STRING32);
	      Defaults->num_hosts++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many hosts in Network File (>%d).\n", MAX_HOSTS);
	         error_reported = TRUE;
	      }
	   }
	}
	;

/* FILTER FILE */
Fcommand: BLOCK_ESPDU INT INT INT
	{
	   i = Defaults->num_filters;
	   if (i < MAX_PDU_FILTERS) {
	      Defaults->Filters[i].filter_type = TYPE_BLOCK;
	      Defaults->Filters[i].Type.Block.site = $2;
	      Defaults->Filters[i].Type.Block.host = $3;
	      Defaults->Filters[i].Type.Block.entity = $4;
	      Defaults->num_filters++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many entities in Filter File (>%d).\n",
	                         MAX_PDU_FILTERS);
	         error_reported = TRUE;
	      }
	   }
	}
	;

Fcommand: FORCE_APPEARANCE INT INT INT INT INT INT
	{
	   i = Defaults->num_filters;
	   if (i < MAX_PDU_FILTERS) {
	      Defaults->Filters[i].filter_type = TYPE_FORCE;
	      Defaults->Filters[i].Type.Force.x = $2;
	      Defaults->Filters[i].Type.Force.y = $3;
	      Defaults->Filters[i].Type.Force.z = $4;
	      Defaults->Filters[i].Type.Force.and_mask = (unsigned int) $5;
	      Defaults->Filters[i].Type.Force.or_mask = (unsigned int) $6;
	      Defaults->Filters[i].Type.Force.range = $7;
	      Defaults->num_filters++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many entities in Filter File (>%d).\n",
	                         MAX_PDU_FILTERS);
	         error_reported = TRUE;
	      }
	   }
	}
	;

/* TRANSPORT FILE */
TFcommand: LOCATION FLOAT FLOAT FLOAT FLOAT FLOAT FLOAT
	{
	   i = Defaults->num_transport_locs;
	   if (i < MAX_TRANSPORT) {
	      Defaults->TransportLocs[i].x = $2;
	      Defaults->TransportLocs[i].y = $3;
	      Defaults->TransportLocs[i].z = $4;
	      Defaults->TransportLocs[i].heading = $5;
	      Defaults->TransportLocs[i].pitch = $6;
	      Defaults->TransportLocs[i].roll = $7;
	      Defaults->num_transport_locs++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many locations in Transport File (>%d).\n",
	                                 MAX_TRANSPORT);
	         error_reported = TRUE;
	      }
	   }
	}
	;

/* ROUND WORLD FILE */
RWcommand: UTM_MAP FLOAT FLOAT INT STRING INT INT INT
	{
	   Defaults->RoundWorld.northing = $2;
	   Defaults->RoundWorld.easting = $3;
	   Defaults->RoundWorld.zone_num = $4;
	   Defaults->RoundWorld.zone_letter = $5[0];
	   Defaults->RoundWorld.map_datum = $6;
	   Defaults->RoundWorld.width = $7;
	   Defaults->RoundWorld.height = $8;
	}
	;

/* RADAR ICON FILE */
RIcommand: RADAR_ICON INT INT STRING
	{
	   i = Defaults->num_radar_icons;
	   if (i < MAX_RADAR_DIVS) {
	      Defaults->RadarIcons[i].min = $2;
	      Defaults->RadarIcons[i].max = $3;
	      Defaults->RadarIcons[i].ident = $4[0];
	      Defaults->num_radar_icons++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many icons in Radar Icons File (>%d).\n",
	                           MAX_RADAR_DIVS);
	         error_reported = TRUE;
	      }
	   }
	}
	;

/* MASTER CONFIGURATION FILE */
MCcommand: ANIMATIONS on_off
	{
	   mc->animations = boolean;
	}
	;

MCcommand: ANIMATION_FILE STRING
	{
	   strncpy(mc->animation_file, $2, STRING128);
	}
	| ANIMATION_FILE IM_NONE
	{
	   strcpy(mc->animation_file, "NONE");
	}
	;

MCcommand: AUTO_FILE STRING
        {
           strncpy(mc->auto_file, $2, STRING128);
        }
        | AUTO_FILE IM_NONE
        {
           strcpy(mc->auto_file, "NONE");
        }
        ;

MCcommand: AUTO_STATE on_off
        {
           mc->auto_state =  boolean;
        }
        ;

MCcommand: BDIHML on_off
        {
           mc->bdihml = boolean;
        }
        ;

MCcommand: DIGUY_SPEED_RATIO FLOAT
        {
           mc->diguy_info.speed_ratio = $2;
        }
        ;

MCcommand: DIGUY_POSITION_ERROR FLOAT
        {
           mc->diguy_info.position_error = $2;
        }
        ;

MCcommand: DIGUY_TIME_CONSTANT FLOAT
        {
           mc->diguy_info.time_constant = $2;
        }
        ;

MCcommand: DIGUY_TRANS_SMOOTHNESS TRANS_IMMEDIATE
        {
           mc->diguy_info.trans_smoothness = TRANS_IMMEDIATE;
        }
        | DIGUY_TRANS_SMOOTHNESS IMMEDIATE_TRANS
        {
           mc->diguy_info.trans_smoothness = IMMEDIATE_TRANS;
        }
        | DIGUY_TRANS_SMOOTHNESS TRANS_SMOOTHEST
        {
           mc->diguy_info.trans_smoothness = TRANS_SMOOTHEST;
        }
        ;

MCcommand: DIGUY_POSITION_MODE SMOOTH_MODE
        {
           mc->diguy_info.position_mode = SMOOTH_MODE;
        }
        | DIGUY_POSITION_MODE PRECISE_MODE
        {
           mc->diguy_info.position_mode = PRECISE_MODE;
        }
        ;

MCcommand: DIGUY_POSITION_MECHANISM DIGUY_INTERNAL
        {
           mc->diguy_info.position_mechanism = DIGUY_INTERNAL;
        }
        | DIGUY_POSITION_MECHANISM NPSNET_INTERNAL
        {
           mc->diguy_info.position_mechanism = NPSNET_INTERNAL;
        }
        ;

MCcommand: BLINKING_LIGHTS_FILE STRING
	{
	   strncpy(mc->blinking_lights_file, $2, STRING128);
	}
	| BLINKING_LIGHTS_FILE IM_NONE
	{
	   strcpy(mc->blinking_lights_file, "NONE");
	}
	;

MCcommand: IM_BOUNDING_BOX FLOAT FLOAT FLOAT FLOAT
	{
	   mc->BoundingBox.xmin = $2;
	   mc->BoundingBox.xmax = $3;
	   mc->BoundingBox.ymin = $4;
	   mc->BoundingBox.ymax = $5;
	}
	;

MCcommand: CHANNEL INT
	{
	   mc->channel = $2;
	}
	;

MCcommand: CLIP_PLANES FLOAT FLOAT
	{
	   mc->ClipPlanes.cp_near = $2;
	   mc->ClipPlanes.cp_far = $3;
	}
	;

MCcommand: C2_FILE STRING
	{
	   printf("C2_FILE command no longer supported.\n");
	}
	| C2_FILE IM_NONE
	{
	   /* No action */
	}
	;

MCcommand: IM_CONTROL FCS
	{
	   mc->control = FCS;
	}
	|  IM_CONTROL FLYBOXIN
	{
	   mc->control = FLYBOXIN;
	}
	|  IM_CONTROL LEATHER_FLYBOX
	{
	   mc->control = LEATHER_FLYBOX;
	}
	|  IM_CONTROL KAFLIGHT
	{
	   mc->control = KAFLIGHT;
	}
	|  IM_CONTROL KEYBOARD
	{
	   mc->control = KEYBOARD;
	}
	|  IM_CONTROL PAD
	{
	   mc->control = KEYBOARD;
	}
	|  IM_CONTROL SPACEBALL
	{
	   mc->control = SPACEBALL;
	}
	|  IM_CONTROL DRIVE
	{
	   mc->control = SPACEBALL;
	}
	|  IM_CONTROL BALL
	{
	   mc->control = SPACEBALL;
	}
	|  IM_CONTROL UNIPORT
	{
	   mc->control = UNIPORT;
	}
        |  IM_CONTROL SHIP_PANEL
        {
           mc->control = SHIP_PANEL;
        }
        |  IM_CONTROL SUB_PANEL
        {
           mc->control = SUB_PANEL;
        }
        |  IM_CONTROL AUSA_ODT
        {
           mc->control = AUSA_ODT;
        }
	;

MCcommand: CPU_LOCK on_off
	{
	   mc->cpu_lock = boolean;
	}
	;

MCcommand: CURSOR_FILE STRING
        {
            strncpy(mc->cursor_file, $2, STRING128);
        }
        | CURSOR_FILE IM_NONE
        {
           strcpy(mc->cursor_file, "NONE");
        }
        ;

MCcommand: DB_OFFSET INT
        {
           mc->db_offset = $2;
        }
        ;

MCcommand: DB_PAGES_ALLOWED INT
        {
           mc->db_pages_allowed = $2;
        }
        ;

MCcommand: DB_TSG_FILE STRING
	{
	   strncpy(mc->db_tsg_file, $2, STRING128);
        }
        | DB_TSG_FILE IM_NONE
        {
           strcpy(mc->db_tsg_file, "NONE");
        }
        ;

MCcommand: DEAD_SWITCH on_off
        {
           mc->dead_switch = boolean;
        }
        ;

MCcommand: DIS_EXERCISE INT
	{
	   mc->DIS.exercise = $2;
	}
	;

MCcommand: DIS_HEARTBEAT FLOAT
	{
	   mc->DIS.heartbeat = $2;
	}
	;

MCcommand: DIS_POSDELTA FLOAT
	{
	   mc->DIS.posdelta = $2;
	}
	;

MCcommand: DIS_ROTDELTA FLOAT
	{
	   mc->DIS.rotdelta = $2;
	}
	;

MCcommand: DIS_TIME_OUT FLOAT
	{
	   mc->DIS.timeout = $2;
	}
	;

MCcommand: DIS_ENUM DISv203
        {
           mc->dis_enum = DISv203;
        }
        | DIS_ENUM DISv204
        {
           mc->dis_enum = DISv204;
        }
        ;

MCcommand: DUMP_FILE STRING
	{
	   /* Required */
	   strncpy(mc->dump_file, $2, STRING128);
	}
	;

MCcommand: DVW_PATHS STRING
	{
	   strncpy(mc->dvw_file, $2, STRING128);
	}
	| DVW_PATHS IM_NONE
	{
	   strcpy(mc->dvw_file, "NONE");
	}
	;

MCcommand: DYNAMIC_OBJS on_off
	{
	   mc->dynamic_objs = boolean;
	}
	;

MCcommand: EARTH_SKY_MODE FAST
	{
	   mc->es_mode = FAST;
	}
	|  EARTH_SKY_MODE TAG
	{
	   mc->es_mode = TAG;
	}
	|  EARTH_SKY_MODE SKY
	{
	   mc->es_mode = SKY;
	}
	|  EARTH_SKY_MODE SKY_GROUND
	{
	   mc->es_mode = SKY_GROUND;
	}
	|  EARTH_SKY_MODE SKY_CLEAR
	{
	   mc->es_mode = SKY_CLEAR;
	}
	;

MCcommand: ENVIRONMENTAL STRING
	{
	   strncpy(mc->environmental_file, $2, STRING128);
	}
	| ENVIRONMENTAL IM_NONE
	{
	   strcpy(mc->environmental_file, "NONE");
	}
	;

MCcommand: ENTITY_FILE STRING
        {
           strncpy(mc->entity_file, $2, STRING128);
        }
        | ENTITY_FILE IM_NONE
        {
           strcpy(mc->entity_file, "NONE");
        }
        ;

MCcommand: ETHER_INTERF STRING
	{
	   strncpy(mc->ether_interf, $2, STRING32);
	}
	| ETHER_INTERF IM_NONE
	{
	   strcpy(mc->ether_interf, "NONE");
	}
	;

MCcommand: FCS_CHECK on_off
	{
	   mc->fcs.check = boolean;
	}
	;

MCcommand: FCS_MP on_off
        {
           mc->fcs.mp  = boolean;
        }
        ;

MCcommand: FCS_PORT STRING
	{
	   strncpy(mc->fcs.port, $2, STRING32);
	}
	| FCS_PORT IM_NONE
	{
	   strcpy(mc->fcs.port, "NONE");
	}
	;

MCcommand: FCS_PROMPT on_off
	{
	   mc->fcs.prompt = boolean;
	}
	;

MCcommand: FOV FLOAT FLOAT
	{
	   mc->FoV.x = $2;
	   mc->FoV.y = $3;
	}
	;

MCcommand: FILTER_FILE STRING
	{
	   strncpy(mc->filter_file, $2, STRING128);
	}
	| FILTER_FILE IM_NONE
	{
	   strcpy(mc->filter_file, "NONE");
	}
	;

MCcommand: FLYBOX on_off
	{
	   mc->flybox = boolean;
	}
	;

MCcommand: FLYBOX_PORT STRING
	{
	   strncpy(mc->flybox_port, $2, STRING32);
	}
	| FLYBOX_PORT IM_NONE
	{
	   strcpy(mc->flybox_port, "NONE");
	}
	;

MCcommand: FOG on_off
	{
	   mc->fog = boolean;
	}
	;

MCcommand: FOG_NEAR FLOAT
        {
           mc->fog_near = $2;
        }
        ;

MCcommand: FOG_FAR FLOAT
        {
           mc->fog_far = $2;
        }
        ;

MCcommand: FOG_TYPE FOG_VTX_LIN
        {
           mc->fog_type = FOG_VTX_LIN;
        }
        | FOG_TYPE FOG_VTX_EXP
        {
           mc->fog_type = FOG_VTX_EXP;
        }
        | FOG_TYPE FOG_VTX_EXP2
        {
           mc->fog_type = FOG_VTX_EXP2;
        }
        | FOG_TYPE FOG_PIX_LIN
        {
           mc->fog_type = FOG_PIX_LIN;
        }
        | FOG_TYPE FOG_PIX_EXP
        {
           mc->fog_type = FOG_PIX_EXP;
        }
        | FOG_TYPE FOG_PIX_EXP2
        {
           mc->fog_type = FOG_PIX_EXP2;
        }
        | FOG_TYPE FOG_PIX_SPLINE
        {
           mc->fog_type = FOG_PIX_SPLINE;
        }
        ;

MCcommand: FORCE IM_BLUE
	{
	   mc->force = ForceID_Blue;
	}
	|  FORCE IM_RED
	{
	   mc->force = ForceID_Red;
	}
	|  FORCE IM_WHITE
	{
	   mc->force = ForceID_White;
	}
	|  FORCE IM_OTHER
	{
	   mc->force = ForceID_Other;
	}
	;

MCcommand: IM_FRAME_RATE FLOAT
	{
	   mc->frame_rate = $2;
	}
	;

MCcommand: FS_CONFIG STRING
	{
	   strncpy(mc->fs_config, $2, STRING128);
	}
	;

MCcommand: FS_PORT STRING
	{
	   strncpy(mc->fs_port, $2, STRING32);
	}
	;

MCcommand: FS_SCALE FLOAT
        {
           mc->fs_scale = $2;
        }
        ;

MCcommand: GROUND_HEIGHT FLOAT
	{
	   mc->ground_height = $2;
	}
	;

MCcommand: HIRES IM_NET_OFF
	{
	   mc->hires = IM_NET_OFF;
	}
        | HIRES RECEIVE_ONLY
        {
           mc->hires = RECEIVE_ONLY;
        }
        | HIRES SEND_ONLY
        {
           mc->hires = SEND_ONLY;
        }
        | HIRES SEND_AND_RECEIVE
        {
           mc->hires = SEND_AND_RECEIVE;
        }
	;

MCcommand: HMD on_off
	{
	   mc->hmd = boolean;
	}
	;

MCcommand: HMD_FILE STRING
        {
           strncpy(mc->hmd_file, $2, STRING128);
        }
	|  HMD_FILE IM_NONE
	{
	   strcpy(mc->hmd_file, "NONE");
	}
        ;

MCcommand: HORIZON_ANGLE FLOAT
	{
	   mc->horizon_angle = $2;
	}
	;

MCcommand: HUD_FONT_FILE STRING
	{
	   strncpy(mc->hud_font_file, $2, STRING128);
	}
	| HUD_FONT_FILE IM_NONE
	{
	   strcpy(mc->hud_font_file, "NONE");
	}
	;

MCcommand: HUD_LEVEL on_off
        {
           mc->hud_level = boolean;
        }
	;

MCcommand: IDENTIFY_FILE STRING
	{
	   strncpy(mc->identify_file, $2, STRING128);
	}
	| IDENTIFY_FILE IM_NONE
	{
	   strcpy(mc->identify_file, "NONE");
	}
	;

MCcommand: INTRO_FONT_FILE STRING
	{
	   strncpy(mc->intro_font_file, $2, STRING128);
	}
	| INTRO_FONT_FILE IM_NONE
	{
	   strcpy(mc->intro_font_file, "NONE");
	}
	;

MCcommand: IM_JACK on_off
        {
           mc->jack = boolean;
        }
        ;

MCcommand: LANDMARK_FILE STRING
	{
	   strncpy(mc->landmark_file, $2, STRING128);
	}
	| LANDMARK_FILE IM_NONE
	{
	   strcpy(mc->landmark_file, "NONE");
	}
	;

MCcommand: LEATHER_FLYBOX_CHECK INT
	{
	   mc->leather_flybox_check = $2;
	}
	;

MCcommand: LEATHER_FLYBOX_PORT STRING
	{
	   strncpy(mc->leather_flybox_port, $2, STRING32);
	}
	| LEATHER_FLYBOX_PORT IM_NONE
	{
	   strcpy(mc->leather_flybox_port, "NONE");
	}
	;

MCcommand: LOCATION_FILE STRING
	{
	   strncpy(mc->location_file, $2, STRING128);
	}
	| LOCATION_FILE IM_NONE
	{
	   strcpy(mc->location_file, "NONE");
	}
	;

MCcommand: LOD_FADE FLOAT
        {
           mc->lod_fade = $2;
        }
        ;

MCcommand: LOD_SCALE FLOAT
        {
           mc->lod_scale = $2;
        }
        ; 

MCcommand: LW_RECEIVE_PORT INT
        {
           mc->lw_rport = $2;
        }
        ;

MCcommand: LW_SEND_PORT INT
        {
           mc->lw_sport = $2;
        }
        ;

MCcommand: LW_MACHINE_NAME STRING
        {
           strncpy(mc->lw_toip, $2, STRING128);          
        }
        | LW_MACHINE_NAME IM_NONE
        {
           strcpy(mc->lw_toip, "NONE");
        }
        ;

MCcommand: MARKING STRING
	{
	   strncpy(mc->marking, $2, MARKINGS_LEN);
	}
	| MARKING IM_NONE
	{
	   strcpy(mc->marking, "NONE");
	}
	;

MCcommand: MASTER STRING
	{
	   strncpy(mc->master, $2, STRING64);
	}
	| MASTER IM_NONE
	{
	   strcpy(mc->master, "NONE");
	}
	;

MCcommand: IM_MAX_ALTITUDE FLOAT
	{
           mc->alt.max = $2;
        }
	;

MCcommand: MODEL_FILE STRING
	{
	   /* Required */
	   strncpy(mc->model_file, $2, STRING128);
	}
	;

MCcommand: MODSAF_MAP STRING
	{
	   strncpy(mc->modsaf_map, $2, STRING128);
	}
	| MODSAF_MAP IM_NONE
	{
	   strcpy(mc->modsaf_map, "NONE");
	}
	;

MCcommand: MULTICAST on_off
	{
	   mc->multicast.enable = boolean;
	}
	;

MCcommand: MULTICAST_GROUP STRING
	{
	   strncpy(mc->multicast.group, $2, STRING32);
	}
	| MULTICAST_GROUP IM_NONE
	{
	   strcpy(mc->multicast.group, "NONE");
	}
	;

MCcommand: MULTICAST_TTL INT
	{
	   mc->multicast.ttl = $2;
	}
	;

MCcommand: MULTISAMPLE on_off
	{
	   mc->multisample = boolean;
	}
	;

MCcommand: NETWORKING on_off
	{
	   mc->networking = boolean;
	}
	;

MCcommand: NETWORK_FILE STRING
	{
	   /* Required */
	   strncpy(mc->network_file, $2, STRING128);
	}
	;

MCcommand: NOTIFY_LEVEL IM_FATAL
	{
	   mc->notify_level = IM_FATAL;
	}
	|  NOTIFY_LEVEL WARN
	{
	   mc->notify_level = WARN;
	}
	|  NOTIFY_LEVEL NOTICE
	{
	   mc->notify_level = NOTICE;
	}
	|  NOTIFY_LEVEL INFO
	{
	   mc->notify_level = INFO;
	}
	|  NOTIFY_LEVEL IM_DEBUG
	{
	   mc->notify_level = IM_DEBUG;
	}
        | NOTIFY_LEVEL FPDEBUG
        {
           mc->notify_level = FPDEBUG;
        }
        ;

MCcommand: NPS_PIC_DIR STRING
	{
	   strncpy(mc->nps_pic_dir, $2, STRING128);
	   len = strlen(mc->nps_pic_dir);
	   if (mc->nps_pic_dir[len-1] != '\\') {
	      mc->nps_pic_dir[len] = '\\';
	      mc->nps_pic_dir[len+1] = '\0';
	   }
	}
	| NPS_PIC_DIR IM_NONE
	{
	   strcpy(mc->nps_pic_dir, "NONE");
	}
	;

MCcommand:ODT_MP on_off
        {
           mc->odt_mp = boolean;
        }
        ;

MCcommand:ODT_PORT STRING
        {
           strncpy(mc->odt_port, $2, STRING32);
        }
        | ODT_PORT IM_NONE
        {
           strcpy(mc->odt_port, "NONE");
        }
        ;

MCcommand: PANEL_GROUP STRING
	{
	   strncpy(mc->panel.group, $2, STRING32);
	}
	| PANEL_GROUP IM_NONE
	{
	   strcpy(mc->panel.group, "NONE");
	}
	;

MCcommand: PANEL_TTL INT
	{
	   mc->panel.ttl = $2;
	}
	;

MCcommand: PANEL_PORT INT
	{
	   mc->panel.port = $2;
	}
	;

MCcommand: PDUPOSITION on_off
	{
	   mc->pdu_position = boolean;
	}
	;

MCcommand: PHASE ALG_FREE
	{
	   mc->phase = ALG_FREE;
	}
	|  PHASE ALG_FLOAT
	{
	   mc->phase = ALG_FLOAT;
	}
	|  PHASE ALG_LOCK
	{
	   mc->phase = ALG_LOCK;
	}
	;

MCcommand: PICTURES on_off
	{
	   mc->pictures = boolean;
	}
	;

MCcommand: PIPE INT
	{
	   mc->pipe = $2;
	}
	;

MCcommand: PO_CTDB_FILE STRING
	{
	   strncpy(mc->PO.ctdb_file, $2, STRING128);
	}
	| PO_CTDB_FILE IM_NONE
	{
	   strcpy(mc->PO.ctdb_file, "NONE");
	}
	;

MCcommand: IM_PO_DATABASE INT
	{
	   mc->PO.database = $2;
	}
	;

MCcommand: IM_PO_EXERCISE INT
	{
	   mc->PO.exercise = $2;
	}
	;

MCcommand: IM_PO_PORT INT
	{
	   mc->PO.port = $2;
	}
	;

MCcommand: IM_PORT INT
	{
	   mc->port = $2;
	}
	;

MCcommand: PROCESS_MODE APPCULLDRAW
	{
	   mc->process_mode = APPCULLDRAW;
	}
	|  PROCESS_MODE APP_CULLDRAW
	{
	   mc->process_mode = APP_CULLDRAW;
	}
	|  PROCESS_MODE APPCULL_DRAW
	{
	   mc->process_mode = APPCULL_DRAW;
	}
	|  PROCESS_MODE APP_CULL_DRAW
	{
	   mc->process_mode = APP_CULL_DRAW;
	}
	|  PROCESS_MODE IM_DEFAULT
	{
	   mc->process_mode = IM_DEFAULT;
	}
	;

MCcommand: RADAR_ICON_FILE STRING
	{
	   strncpy(mc->radar_icon_file, $2, STRING128);
	}
	| RADAR_ICON_FILE IM_NONE
	{
	   strcpy(mc->radar_icon_file, "NONE");
	}
	;

MCcommand: RANGE_FILTER FLOAT
        {
           mc->range_filter = $2;
        }
        ;


MCcommand: REMOTE_INPUT_PANEL on_off
	{
	   mc->remote_input_panel = boolean;
	}
	;

MCcommand: ROAD_FILE STRING
	{
	   strncpy(mc->road_file, $2, STRING128);
	}
	| ROAD_FILE IM_NONE
	{
	   strcpy(mc->road_file, "NONE");
	}
	;

MCcommand:  ROUND_WORLD_FILE STRING
	{
	   strncpy(mc->round_world_file, $2, STRING128);
	}
	|   ROUND_WORLD_FILE IM_NONE
	{
	   strcpy(mc->round_world_file, "NONE");
	}
	;

MCcommand: SCORING on_off
	{
	   mc->scoring = boolean;
	}
	;

MCcommand: SEARCH_PATH STRING
	{
	   /* The search path is prepended to with repeated invocations */
	   /* of the command. */
	   if (strlen(mc->search_path) == 0)	/* First invocation */
	      strncpy(mc->search_path, $2, MAX_SEARCH_PATH);
	   else {				/* Subsequent invocations */
	      if (strcasecmp(mc->search_path, "NONE") == 0)
	         bzero(mc->search_path, 5);
	      /* Will the new path fit? */
	      len = (strlen(mc->search_path) + 1); /* 1 for the colon */
	      if ((len + strlen($2)) < MAX_SEARCH_PATH) {
	         /* adequate space */
	         strcpy(tempstr, $2);
	         strcat(tempstr, ":");
	         strcat(tempstr, mc->search_path);
	         strcpy(mc->search_path, tempstr);
	      } else {
	         printf("Inadequate storage for path \"%s\".\n", $2);
	      }
	   }
	}
	|  SEARCH_PATH IM_NONE
	{
	   strcpy(mc->search_path, "NONE");
	}
	;

MCcommand: IM_SOUND on_off
	{
	   mc->sound = boolean;
	}
	;

MCcommand: SOUND_FILE STRING
	{
	   strncpy(mc->sound_file, $2, STRING128);
	}
	|  SOUND_FILE IM_NONE
	{
	   strcpy(mc->sound_file, "NONE");
	}
	;

MCcommand: STATIC_OBJS on_off
	{
	   mc->static_objs = boolean;
	}
	;

MCcommand: STEALTH on_off
	{
	   mc->stealth = boolean;
	}
	;

MCcommand: STEREO on_off
	{
	   mc->stereo = boolean;
	}
	;

MCcommand: TARGET_BOUNDING_BOX on_off
        {
           mc->target_bbox = boolean;
        }
        ;

MCcommand: TARGET_ENTITY INT INT INT
	{
          mc->target_entity.site = $2;
          mc->target_entity.host = $3;
          mc->target_entity.entity = $4;
	}
	;

MCcommand: IM_TERRAIN_FILE STRING
	{
	   /* Required */
	   strncpy(mc->terrain_file, $2, STRING128);
	}
	;

MCcommand: TESTPORT STRING
	{
	   strncpy(mc->testport, $2, STRING32);
	}
	|  TESTPORT IM_NONE
	{
	   strcpy(mc->testport, "NONE");
	}
	;

MCcommand: TRAKER_FILE STRING
	{
	   strncpy(mc->traker_file, $2, STRING128);
	}
	|  TRAKER_FILE IM_NONE
	{
	   strcpy(mc->traker_file, "NONE");
	}
	;

MCcommand: TRAKER_PORT STRING
	{
	   strncpy(mc->traker_port, $2, STRING32);
	}
	|  TRAKER_PORT IM_NONE
	{
	   strcpy(mc->traker_port, "NONE");
	}
	;

MCcommand: TRANSPORT_FILE STRING
	{
	   strncpy(mc->transport_file, $2, STRING128);
	}
	|  TRANSPORT_FILE IM_NONE
	{
	   strcpy(mc->transport_file, "NONE");
	}
	;

MCcommand: TREADPORT STRING
	{
	   strncpy(mc->treadport, $2, STRING32);
	}
	|  TREADPORT IM_NONE
	{
	   strcpy(mc->treadport, "NONE");
	}
	;

MCcommand: UNIPORT STRING
	{
	   strncpy(mc->uniport, $2, STRING32);
	}
	|  UNIPORT IM_NONE
	{
	   strcpy(mc->uniport, "NONE");
	}
	;

MCcommand: UPPERBODY STRING
        {
           strncpy(mc->upperbody, $2, STRING32);
        }
        |  UPPERBODY IM_NONE
        {
           strcpy(mc->upperbody, "NONE");
        }
        ;

MCcommand: USE_ROUND_WORLD on_off
        {
           mc->use_round = boolean;
        }
        ;

MCcommand: VEHICLE_DRIVEN STRING
	{
	   strncpy(mc->vehicle_driven, $2, STRING32);
	}
	|  VEHICLE_DRIVEN IM_NONE
	{
	   strcpy(mc->vehicle_driven, "NONE");
	}
	;

MCcommand: VIEW AIRPORT
	{
	   mc->view = AIRPORT;
	}
	|  VIEW CANYON
	{
	   mc->view = CANYON;
	}
	|  VIEW VILLAGE
	{
	   mc->view = VILLAGE;
	}
	|  VIEW PIER
	{
	   mc->view = PIER;
	}
	|  VIEW IM_START
	{
	   mc->view = IM_START;
	}
	;

MCcommand: VIEWPORT FLOAT FLOAT FLOAT FLOAT
	{
	   mc->ViewPort.xmin = $2;
	   mc->ViewPort.xmax = $3;
	   mc->ViewPort.ymin = $4;
	   mc->ViewPort.ymax = $5;
	}
	;

MCcommand: VIEW_HPR FLOAT FLOAT FLOAT
	{
	   mc->ViewHPR.heading = $2;
	   mc->ViewHPR.pitch = $3;
	   mc->ViewHPR.roll = $4;
	}
	;

MCcommand: VIEW_XYZ FLOAT FLOAT FLOAT
	{
	   mc->ViewXYZ.x = $2;
	   mc->ViewXYZ.y = $3;
	   mc->ViewXYZ.z = $4;
	}
	;

MCcommand: WAYPOINTS on_off
	{
	   mc->waypoints = boolean;
	}
	;

MCcommand: IM_WEAPON_VIEW on_off
        {
           mc->weapon_view = boolean;
        }
        ;

MCcommand: IM_WINDOW CLOSED
	{
	   mc->window.type = CLOSED;
	}
	|  IM_WINDOW CUSTOM
	{
	   mc->window.type = CUSTOM;
	}
	|  IM_WINDOW SVGA
	{
	   mc->window.type = SVGA;
	}
	|  IM_WINDOW FULL
	{
	   mc->window.type = FULL;
	}
	|  IM_WINDOW NTSC
	{
	   mc->window.type = NTSC;
	}
	|  IM_WINDOW PAL
	{
	   mc->window.type = PAL;
	}
	|  IM_WINDOW NORMAL
	{
	   mc->window.type = NORMAL;
	}
	|  IM_WINDOW VIM
	{
	   mc->window.type = VIM;
	}
	|  IM_WINDOW THREE_TV
	{
	   mc->window.type = THREE_TV;
	}
	|  IM_WINDOW THREE_BUNKER
	{
	   mc->window.type = THREE_BUNKER;
	}
	|  IM_WINDOW TWO_TPORT
	{
	   mc->window.type = TWO_TPORT;
	}
        |  IM_WINDOW TWO_ADJACENT
        {
           mc->window.type = TWO_ADJACENT;
        }
	| IM_WINDOW FS_MCO
        {
	   mc->window.type = FS_MCO;
	}
        | IM_WINDOW FS_ST_BUFFERS
	{
           mc->window.type = FS_ST_BUFFERS;
        }
	;

MCcommand: WINDOW_SIZE INT INT INT INT
	{
	   mc->window.xmin = $2;
	   mc->window.xmax = $3;
	   mc->window.ymin = $4;
	   mc->window.ymax = $5;
	}
	;

/* TERRAIN FILE */
Tcommand: MODEL_DEF3 STRING BRIDGE_MASK
	{
	   assign_terrain_mask($2, 1);
	}
	| MODEL_DEF3 STRING FOLIAGE_MASK
	{
	   assign_terrain_mask($2, 2);
	}
	| MODEL_DEF3 STRING TERRAIN_MASK
	{
	   assign_terrain_mask($2, 3);
	}
	| MODEL_DEF3 STRING TUNNEL_MASK
	{
	   assign_terrain_mask($2, 4);
	}
	| MODEL_DEF3 STRING WATER_MASK
	{
	   assign_terrain_mask($2, 5);
	}
        ;

Tcommand: DB_PAGING on_off
        {
           mc->db_paging = boolean;
        }
	;

Tcommand: FLAT_WORLD on_off
        {
           mc->flat_world = boolean;
        }
        ;

/* boolean bucket */
on_off:   IM_ON
        {
           boolean = TRUE;
        }
        | IM_OFF
        {
           boolean = FALSE;
        }
        ;

/* MODEL FILE */
Mcommand: MODEL_DEF2 STRING STRING INT INT INT INT INT INT INT INT INT
	{
	   assign_dyn_models(TYPE_FLT, $2, "", $3, $4, $5, $6, $7, 
                             $8, $9, $10, $11, $12);
	}
	| MODEL_DEF2 INT STRING INT INT INT INT INT INT INT INT INT
	{
	   /* Convert first "INT" back to string when model name is a num */
	   char tmp[32];
	   sprintf(tmp, "%d", $2);
	   assign_dyn_models(TYPE_FLT, tmp, "", $3, $4, $5, $6, $7,
                             $8, $9, $10, $11, $12);
	}
	;

Mcommand: JACK_DEF STRING STRING STRING INT INT INT INT INT INT INT INT INT
	{
           assign_dyn_models(TYPE_TIPS, $2, $3, $4, $5, $6, $7, $8,
                             $9, $10, $11, $12, $13);
        }
	;

Mcommand: DUDE_DEF STRING STRING INT INT INT INT INT INT INT INT INT
        {
           assign_dyn_models(TYPE_JCD, $2, "", $3, $4, $5, $6, $7,
                             $8, $9, $10, $11, $12);
        }
        ;

Mcommand: JADE_DEF STRING INT INT INT INT INT INT INT INT INT
        {
           assign_dyn_models(TYPE_JADE, $2, "", "", $3, $4, $5, $6,
                             $7, $8, $9, $10, $11);
        }
        ;

Mcommand: DIGUY_DEF STRING STRING INT INT INT INT INT INT INT INT INT
        {
           assign_dyn_models(TYPE_DIGUY, $2, $3, "", $4, $5, $6, $7,
                             $8, $9, $10, $11, $12);
        }
        ;

Mcommand: WEAPON_DEF INT INT FLOAT FLOAT FLOAT FLOAT
	{
	   if (error_reported == FALSE) {
	      struct Weap *current;

	      /* Get current model */
	      i = (Defaults->num_dynamic_objs-1);
	      /* Get current munition */
	      j = Defaults->DObjects[i].num_weapons;

	      if (j < MAX_FIRE_INFO) {
	         current = &(Defaults->DObjects[i].weapons[j]);
	         current->index = $2;
	         current->wclass = $3;
	         current->x = $4;
	         current->y = $5;
	         current->z = $6;
	         current->delay = $7;
	         Defaults->DObjects[i].num_weapons++;
	      } else {
	         if (error_reported2 == FALSE) {
	            /* report error once */
	            printf("Too many weapons in Model File (>%d). ",
	                                                  MAX_FIRE_INFO);
	            printf("Model %s.\n", Defaults->DObjects[i].filename);
	            error_reported2 = TRUE;
	         }
	      }
	   }
	}
	;

/* LOCATION FILE */
Lcommand: MODEL_DEF1 STRING STRING INT INT INT
	{
	   assign_stat_models($2, $3, $4, $5, $6);
	}
	| MODEL_DEF1 INT STRING INT INT INT
	{
	   /* Convert first "INT" back to string when model name is a num */
	   char tmp[32];
	   sprintf(tmp, "%d", $2);
	   assign_stat_models(tmp, $3, $4, $5, $6);
	}
	;

Lcommand: IM_POSITION FLOAT FLOAT FLOAT FLOAT FLOAT FLOAT
	{
	   if (error_reported == FALSE) {
	      struct Pos *current;

	      /* Get current model */
	      i = (Defaults->num_static_objs-1);
	      /* Get current point */
	      j = Defaults->SObjects[i].num_points;

	      if (j < MAX_POINTS) {
	         current = &(Defaults->SObjects[i].points[j]);
	         current->x = $2;
	         current->y = $3;
	         current->z = $4;
	         current->heading = $5;
	         current->pitch = $6;
	         current->roll = $7;
	         Defaults->SObjects[i].num_points++;
	      } else {
	         if (error_reported2 == FALSE) {
	            /* report error once */
	            printf("Too many points in Location File (>%d). ",
	                                                     MAX_POINTS);
	            printf("Model %s.\n", Defaults->SObjects[i].flightfile);
	            error_reported2 = TRUE;
	         }
	      }
	   }
	}
	;

/* BLINKING LIGHTS FILE */
BLcommand: LIGHT FLOAT FLOAT FLOAT FLOAT FLOAT FLOAT FLOAT FLOAT
	{
	   i = Defaults->num_blinking_lights;
	   if (i < MAX_BLINKING_LIGHTS) {
	      Defaults->BlinkingLights[i].Position.x = $2;
	      Defaults->BlinkingLights[i].Position.y = $3;
	      Defaults->BlinkingLights[i].Position.z = $4;
	      Defaults->BlinkingLights[i].Color.r = $5;
	      Defaults->BlinkingLights[i].Color.g = $6;
	      Defaults->BlinkingLights[i].Color.b = $7;
	      Defaults->BlinkingLights[i].size = $8;
	      Defaults->BlinkingLights[i].blink_rate = $9;
	      Defaults->num_blinking_lights++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many lights in Blinking Lights File (>%d).\n",
	                                     MAX_BLINKING_LIGHTS);
	         error_reported = TRUE;
	      }
	   }
	}
	;

/* LANDMARK FILE */
LMcommand: LANDMARK_ICON STRING FLOAT FLOAT
	{
	   i = Defaults->num_landmarks;
	   if (i < MAX_LANDMARKS) {
	      Defaults->Landmarks[i].icon = $2[0];
	      Defaults->Landmarks[i].x = $3;
	      Defaults->Landmarks[i].y = $4;
	      Defaults->num_landmarks++;
	   } else {
	      if (error_reported == FALSE) {
	         /* report error once */
	         printf("Too many landmarks in Landmark File (>%d).\n",
	                                   MAX_LANDMARKS);
	         error_reported = TRUE;
	      }
	   }
	}
	;

%%

int lineno;
extern char current_file[];
extern int current_file_type;

void parser()
{
   extern FILE	*yyin;
   char		error[128];

   if (current_file == NULL) {
      printf("Error: No file specified in parser()\n");
   } else {
#ifdef DEBUG2
      printf("Parsing: %s\n", current_file);
      fflush(stdout);
#endif
   }

   if ((yyin = fopen(current_file, "r")) == NULL) {
      printf("Parser error. ");
      fflush(stdout);
      strcpy(error, "fopen(");
      strcat(error, current_file);
      strcat(error, ")");
      perror(error);
      exit(0);
   } else {
      lineno = 1;
      yyparse();
      fclose(yyin);
   }
}

void yyerror(s)
   char *s;
{
   printf("\nParser: %s, line %d: %s\n", current_file, lineno, s);
   switch (current_file_type) {
      case (ANIMATION_FILETYPE):
         printf("\tAnimation File formats:\n");
         printf("\tBB_SEQ model_file delay\n");
         printf("\tMODEL_SEQ model_file delay\n");
         printf("\t\t(token present?)\n");
         break;
      case (BLINKING_FILETYPE):
         printf("\tBlinking Lights File format:\n");
         printf("\tLIGHT x y z r g b size rate\n");
         printf("\t\t(token 'LIGHT' present?)\n");
         printf("\t\t(EOL comment preceded with '#'?)\n");
         break;
      case (FILTER_FILETYPE):
         printf("\tFilter File formats:\n");
         printf("\tBLOCK_ESPDU site host entity\n");
         printf("\tFORCE_APPEARANCE x y x ANDmask ORmask range\n");
         printf("\t\t(token present?)\n");
         break;
      case (LANDMARK_FILETYPE):
         printf("\tLandmark File format:\n");
         printf("\tLANDMARK_ICON key x y\n");
         printf("\t\t(token 'LANDMARK_ICON' present?)\n");
         printf("\t\t(EOL comment preceded with '#'?)\n");
         printf("\t\t('#' used as key is invalid.)\n");
         break;
      case (LOCATION_FILETYPE):
         printf("\tLocation File formats:\n");
         printf("\tMODEL_DEF name dir alive dead otype\n");
         printf("\tPOSITION x y z heading pitch roll\n");
         break;
      case (MODEL_FILETYPE):
         printf("\tModel File formats:\n");
         printf("\tMODEL_DEF name dir alive dead vtype ");
         printf("\tkind dom cntry cat subcat spec\n");
         printf("\tJACK_DEF name jack_model dir alive dead vtype ");
         printf("\tkind dom cntry cat subcat spec\n");
         printf("\tWEAPON_DEF index class x y z delay\n");
         break;
      case (NETWORK_FILETYPE):
         printf("\tNetwork File format:\n");
         printf("\tHOST hostname site# host# interface\n");
         printf("\t\t(token 'HOST' present?)\n");
         break;
      case (RADAR_ICON_FILETYPE):
         printf("\tRadar Icon File format:\n");
         printf("\tRADAR_ICON min max identifier\n");
         printf("\t\t(token 'RADAR_ICON' present?)\n");
         printf("\t\t(EOL comment preceded with '#'?)\n");
         break;
      case (ROUND_WORLD_FILETYPE):
         printf("\tRound World File format:\n");
         printf("\tUTM_MAP northing easting zone# zone_let ");
         printf("\tdatum width height\n");
         printf("\t\t(token 'UTM_MAP' present?)\n");
         break;
      case (SOUNDS_FILETYPE):
         printf("\tSound File format:\n");
         printf("\tSOUND_DEF identifier filename\n");
         printf("\t\t(token 'SOUND_DEF' present?)\n");
         break;
      case (TERRAIN_FILETYPE):
         printf("\tTerrain File format:\n");
         printf("\tMODEL_DEF filename maskname\n");
         printf("\t\t(token 'MODEL_DEF' present?)\n");
         printf("\tDB_PAGING ON/OFF:\n");
         printf("\t\t(token 'DB_PAGING' present?)\n");
         break;
      case (TRANSPORT_FILETYPE):
         printf("\tTransport File format:\n");
         printf("\tLOCATION x y z heading pitch roll\n");
         printf("\t\t(token 'LOCATION' present?)\n");
         printf("\t\t(EOL comment preceded with '#'?)\n");
         break;
      default:
         break;
   }

   exit(-1);
}


/* Assign "M" line vals */
assign_stat_models(name, dir, alive, dead, otype)
   char *name, *dir;
   int  alive, dead, otype;
{
   error_reported2 = FALSE;
   i = Defaults->num_static_objs;
   if (i < MAX_STATIC_OBJ_TYPES) {
      strncpy(Defaults->SObjects[i].flightfile, (char *) name, STRING128);
      strncpy(Defaults->SObjects[i].path, (char *) dir, STRING128);
      Defaults->SObjects[i].alive_mod = alive;
      Defaults->SObjects[i].dead_mod = dead;
      Defaults->SObjects[i].otype = otype;
      Defaults->num_static_objs++;
   } else {
      if (error_reported == FALSE) {
         /* report error once */
         printf("Too many static objects in Location File (>%d).\n",
                                                        MAX_STATIC_OBJ_TYPES);
         error_reported = TRUE;
      }
   }
   return 1;
}


/* Assign "D" line vals */
assign_dyn_models(mtype, name, jname, dir, alive, dead, vtype,
                  kind, domain, country, category, subcategory, specific)
   int mtype;
   char *name, *jname, *dir;
   int  alive, dead, vtype;
   int	kind, domain, country, category, subcategory, specific;
{
   error_reported2 = FALSE;
   i = Defaults->num_dynamic_objs;
   if (i < MAX_VEHTYPES) {
      strncpy(Defaults->DObjects[i].filename, (char *) name, STRING128);
      strncpy(Defaults->DObjects[i].jack_model, (char *) jname, STRING64 );
      Defaults->DObjects[i].mtype = mtype;
      strncpy(Defaults->DObjects[i].path, (char *) dir, STRING128);
      Defaults->DObjects[i].alive_mod = alive;
      Defaults->DObjects[i].dead_mod = dead;
      Defaults->DObjects[i].vtype = vtype;
      Defaults->DObjects[i].etype.entity_kind = (EntityKind) kind;
      Defaults->DObjects[i].etype.domain = (Domain) domain;
      Defaults->DObjects[i].etype.country = (Country) country;
      Defaults->DObjects[i].etype.category = (Category) category;
      Defaults->DObjects[i].etype.subcategory = (Subcategory) subcategory;
      Defaults->DObjects[i].etype.specific = (Specific) specific;
      Defaults->DObjects[i].etype.extra = (Extra) 0;	/* not used */
      Defaults->num_dynamic_objs++;
   } else {
      if (error_reported == FALSE) {
         /* report error once */
         printf("Too many dynamic objects in Model File (>%d).\n",
                                                       MAX_VEHTYPES);
         error_reported = TRUE;
      }
   }
   return 1;
}

assign_terrain_mask(filename, mask)
   char	*filename;
   int	mask;
{
   i = Defaults->num_masks;
   if (i < MAX_MASKS) {
      strncpy(Defaults->Masks[i].filename, filename, STRING128);
      switch (mask) {
         case 1:
            Defaults->Masks[i].mask = BRIDGE_MASK;
            break;
         case 2:
            Defaults->Masks[i].mask = FOLIAGE_MASK;
            break;
         case 3:
            Defaults->Masks[i].mask = TERRAIN_MASK;
            break;
         case 4:
            Defaults->Masks[i].mask = TUNNEL_MASK;
            break;
         case 5:
            Defaults->Masks[i].mask = WATER_MASK;
            break;
      }
      Defaults->num_masks++;
   } else {
      if (error_reported == FALSE) {
         /* report error once */
         printf("Too many masks in Terrain File (>%d).\n", MAX_MASKS);
         error_reported = TRUE;
      }
   }
   return 1;
}

/* EOF */
