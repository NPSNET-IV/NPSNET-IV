// cl_args.cc

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "im.tab.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "imextern.h"
#include "imclass.h"
#include "pdu.h"
#include "setup_const.h"

extern struct defs *Defaults;
extern struct MC *mc;


void InitManager::process_cl_args(int argc, char **argv)
   // Command line args
{
   struct symbol	*sp;
   extern struct symbol	MasterConfSymtab[];
   char			npsnet[64];
   int			symbol;
// int			on_off;
   float		*fptr;

   cout << "PROCESSING command line args.\n";

   strcpy(npsnet, argv[0]);
   argc--, argv++;

   for (int i = 0, j; i < argc; i++) {	// for each arg i

      // cout << i << ": " << argv[i] << endl; continue;
      // The initial arg in a group must start with '-'
      if ((argv[i][0] != '-') || (strcasecmp(&(argv[i][1]), "help") == 0))
         cl_help(npsnet);

      // Search Master Config symbol table for match
      sp = MasterConfSymtab;
      symbol = 0;
#ifdef DEBUG2
      cout << "process_cl_args(): argv[i][1]=" <<
              &(argv[i][1]) << endl;
#endif
      while (TRUE) {
         if (sp->name == NULL)		// end of table
            break;
         if (sp->numchars == 0) {
            if (strcasecmp(&(argv[i][1]), sp->name) == 0)
               symbol = sp->value;
         } else {
            if (strncasecmp(&(argv[i][1]), sp->name, sp->numchars) == 0)
               symbol = sp->value; // a symbol
         }
#ifdef DEBUG2
         cout << "process_cl_args(): sp->name=" << sp->name << endl;
#endif
         if (symbol)		// found one
            break;
         sp++;			// next symbol
      }
#ifdef DEBUG2
      cout << "process_cl_args(): symbol=" << symbol << endl;
#endif
      switch (symbol) {
         case (ANIMATIONS):
            if (++i == argc)	// no more args
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->animations = TRUE;
            else
               mc->animations = FALSE;
            break;
         case (ANIMATION_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->animation_file, "NONE");
            else
               strncpy(mc->animation_file, argv[i], STRING128);
            break;
         case (AUTO_FILE):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->auto_file, "NONE");
            else
               strncpy(mc->auto_file, argv[i], STRING128);
            break;
         case (AUTO_STATE):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "ON") == 0)
               mc->auto_state = TRUE;
            else
               mc->auto_state = FALSE;
            break;
         case (BDIHML):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "ON") == 0)
               mc->bdihml = TRUE;
            else
               mc->bdihml = FALSE;
            break;
         case (DIGUY_SPEED_RATIO):
            if (++i == argc)
               break;
            mc->diguy_info.speed_ratio = atof(argv[i]);
            break;
         case (DIGUY_POSITION_ERROR):
            if (++i == argc)
               break;
            mc->diguy_info.position_error = atof(argv[i]);
            break;
         case (DIGUY_TIME_CONSTANT):
            if (++i == argc)
               break;
            mc->diguy_info.time_constant = atof(argv[i]);
            break;
         case (DIGUY_TRANS_SMOOTHNESS):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "TRANS_IMMEDIATE", 9) == 0) {
               mc->diguy_info.trans_smoothness = TRANS_IMMEDIATE;
            }
            if (strncasecmp(argv[i], "IMMEDIATE_TRANS", 9) == 0) {
               mc->diguy_info.trans_smoothness = IMMEDIATE_TRANS;
            }
            if (strncasecmp(argv[i], "TRANS_SMOOTHEST", 9) == 0) {
               mc->diguy_info.trans_smoothness = TRANS_SMOOTHEST;
            }
            break;
         case (DIGUY_POSITION_MODE):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "SMOOTH_MODE", 6) == 0) {
               mc->diguy_info.position_mode = SMOOTH_MODE;
            }
            if (strncasecmp(argv[i], "PRECISE_MODE", 7) == 0) {
               mc->diguy_info.position_mode = SMOOTH_MODE;
            }
            break;
         case (DIGUY_POSITION_MECHANISM):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "DIGUY_INTERNAL", 9) == 0) {
               mc->diguy_info.position_mechanism = DIGUY_INTERNAL;
            }
            if (strncasecmp(argv[i], "NPSNET_INTERNAL", 10) == 0) {
               mc->diguy_info.position_mechanism = NPSNET_INTERNAL;
            }
            break;
         case (BLINKING_LIGHTS_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->blinking_lights_file, "NONE");
            else
               strncpy(mc->blinking_lights_file, argv[i], STRING128);
            break;
         case (IM_BOUNDING_BOX):
            fptr = &(mc->BoundingBox.xmin);
            for (j = 0; j < 4; j++) {
               if (++i == argc)
                  break;
               *fptr++ = atof(argv[i]);
            }
            break;
         case (CHANNEL):
            if (++i == argc)
               break;
            mc->channel = atoi(argv[i]);
            break;
         case (CLIP_PLANES):
            fptr = &(mc->ClipPlanes.cp_near);
            for (j = 0; j < 2; j++) {
               if (++i == argc)
                  break;
               *fptr++ = atof(argv[i]);
            }
            break;
         case (IM_CONTROL):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "FCS") == 0) {
               mc->control = FCS;
            }
            if (strncasecmp(argv[i], "FLYBOXIN", 3) == 0) {
               mc->control = FLYBOXIN;
            }
            if (strncasecmp(argv[i], "LEATHER_FLYBOX", 3) == 0) {
               mc->control = LEATHER_FLYBOX;
            }
            if (strncasecmp(argv[i], "KAFLIGHT", 3) == 0) {
               mc->control = KAFLIGHT;
            }
            if (strncasecmp(argv[i], "KEYBOARD", 3) == 0) {
               mc->control = KEYBOARD;
            }
            if (strcasecmp(argv[i], "PAD") == 0) {
               mc->control = KEYBOARD;
            }
            if (strncasecmp(argv[i], "SPACEBALL", 3) == 0) {
               mc->control = SPACEBALL;
            }
            if (strncasecmp(argv[i], "DRIVE", 3) == 0) {
               mc->control = SPACEBALL;
            }
            if (strncasecmp(argv[i], "BALL", 3) == 0) {
               mc->control = SPACEBALL;
            }
            if (strncasecmp(argv[i], "UNIPORT", 3) == 0) {
               mc->control = UNIPORT;
            }
            if (strncasecmp(argv[i], "SHIP_PANEL", 3) == 0) {
               mc->control = SHIP_PANEL;
            }
            if (strncasecmp(argv[i], "SUB_PANEL", 3) == 0) {
                 mc->control = SUB_PANEL;
            }
            if (strncasecmp(argv[i], "AUSA_ODT", 5) == 0) {
               mc->control = AUSA_ODT;
            }
            break;
         case (CPU_LOCK):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->cpu_lock = TRUE;
            else
               mc->cpu_lock = FALSE;
            break;
         case (CURSOR_FILE):
            if (++i == argc)
               break;
            strncpy (mc->cursor_file, argv[i], STRING128);
            break;
         case (DB_OFFSET):
            if (++i == argc)
               break;
            mc->db_offset = atoi(argv[i]);
            break;
         case (DB_PAGES_ALLOWED):
            if (++i == argc)
               break;
            mc->db_pages_allowed = atoi(argv[i]);
            break;
         case (DB_TSG_FILE):
            if (++i == argc)
               break;
            strncpy (mc->db_tsg_file, argv[i], STRING128);
            break;
         case (DEAD_SWITCH):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "ON") == 0)
               mc->dead_switch = TRUE;
            else
               mc->dead_switch = FALSE;
            break;
         case (DIS_EXERCISE):
            if (++i == argc)
               break;
            mc->DIS.exercise = atoi(argv[i]);
            break;
         case (DIS_HEARTBEAT):
            if (++i == argc)
               break;
            mc->DIS.heartbeat = atof(argv[i]);
            break;
         case (DIS_POSDELTA):
            if (++i == argc)
               break;
            mc->DIS.posdelta = atof(argv[i]);
            break;
         case (DIS_ROTDELTA):
            if (++i == argc)
               break;
            mc->DIS.rotdelta = atof(argv[i]);
            break;
         case (DIS_TIME_OUT):
            if (++i == argc)
               break;
            mc->DIS.timeout = atof(argv[i]);
            break;
         case (DIS_ENUM):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "DISv203") == 0) {
               mc->dis_enum = DISv203;
            }
            else if (strcasecmp(argv[i], "DISv204") == 0) {
               mc->dis_enum = DISv204;
            }
            else {
               mc->dis_enum = DISv204;
            }
            break;
         case (DUMP_FILE):
            if (++i == argc)
               break;
            strncpy(mc->dump_file, argv[i], STRING128);
            break;
         case (DVW_PATHS):
            if (++i == argc)
               break;
            strncpy(mc->dvw_file, argv[i], STRING128);
            break;
         case (DYNAMIC_OBJS):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->dynamic_objs = TRUE;
            else
               mc->dynamic_objs = FALSE;
            break;
         case (EARTH_SKY_MODE):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "FAST", 3) == 0)
               mc->es_mode = FAST;
            else if (strcasecmp(argv[i], "TAG") == 0)
               mc->es_mode = TAG;
            else if (strncasecmp(argv[i], "SKY_GROUND", 5) == 0)
               mc->es_mode = SKY_GROUND;
            else if (strncasecmp(argv[i], "SKY_CLEAR", 5) == 0)
               mc->es_mode = SKY_CLEAR;
            else if (strcasecmp(argv[i], "SKY") == 0)
               mc->es_mode = SKY;
            break;
         case (ENVIRONMENTAL):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->environmental_file, "NONE");
            else
               strncpy(mc->environmental_file, argv[i], STRING128);
            break;
         case (ENTITY_FILE):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->entity_file, "NONE");
            else
               strncpy(mc->entity_file, argv[i], STRING128);
            break;
         case (ETHER_INTERF):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->ether_interf, "NONE");
            else
               strncpy(mc->ether_interf, argv[i], STRING32);
            break;
         case (FCS_CHECK):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->fcs.check = TRUE;
            else
               mc->fcs.check = FALSE;
            break;
         case (FCS_MP):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "ON") == 0)
               mc->fcs.mp = TRUE;
            else
               mc->fcs.mp = FALSE;
            break;
         case (FCS_PORT):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->fcs.port, "NONE");
            else
               strncpy(mc->fcs.port, argv[i], STRING32);
            break;
         case (FCS_PROMPT):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->fcs.prompt = TRUE;
            else
               mc->fcs.prompt = FALSE;
            break;
         case (FILTER_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->filter_file, "NONE");
            else
               strncpy(mc->filter_file, argv[i], STRING128);
            break;
         case (FLYBOX):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->flybox = TRUE;
            else
               mc->flybox = FALSE;
            break;
         case (FLYBOX_PORT):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->flybox_port, "NONE");
            else
               strncpy(mc->flybox_port, argv[i], STRING32);
            break;
         case (FOG):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->fog = TRUE;
            else
               mc->fog = FALSE;
            break;
         case (FOG_NEAR):
            if (++i == argc)
               break;
            mc->fog_near = atof (argv[i]);
            break;
         case (FOG_FAR):
            if (++i == argc)
               break;
            mc->fog_far = atof (argv[i]);
            break;
         case (FOG_TYPE):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "FOG_VTX_LIN") == 0)
               mc->fog_type = FOG_VTX_LIN;
            else if (strcasecmp(argv[i], "FOG_VTX_EXP") == 0)
               mc->fog_type = FOG_VTX_EXP;
            else if (strcasecmp(argv[i], "FOG_VTX_EXP2") == 0)
               mc->fog_type = FOG_VTX_EXP2;
            else if (strcasecmp(argv[i], "FOG_PIX_LIN") == 0)
               mc->fog_type = FOG_PIX_LIN;
            else if (strcasecmp(argv[i], "FOG_PIX_EXP") == 0)
               mc->fog_type = FOG_PIX_EXP;
            else if (strcasecmp(argv[i], "FOG_PIX_EXP2") == 0)
               mc->fog_type = FOG_PIX_EXP2;
            else if (strcasecmp(argv[i], "FOG_PIX_SPLINE") == 0)
               mc->fog_type = FOG_PIX_SPLINE;
            else {
               cout << "Warning: Unknown fog type: " << argv[i]
                    << "\tUsing: FOG_VTX_LIN" << endl;

               mc->fog_type = FOG_VTX_LIN;
            }
            break;
         case (FORCE):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "BLUE", 3) == 0)
               mc->force = ForceID_Blue;
            else if (strcasecmp(argv[i], "RED") == 0)
               mc->force = ForceID_Red;
            else if (strncasecmp(argv[i], "WHITE", 3) == 0)
               mc->force = ForceID_White;
            else if (strncasecmp(argv[i], "OTHER", 3) == 0)
               mc->force = ForceID_Other;
            break;
         case (FOV):
            fptr = &(mc->FoV.x);
            for (j = 0; j < 2; j++) {
               if (++i == argc)
                  break;
               *fptr++ = atof(argv[i]);
            }
            break;
         case (IM_FRAME_RATE):
            if (++i == argc)
               break;
            mc->frame_rate = atof(argv[i]);
            break;
         case (FS_CONFIG):
            if (++i == argc)
               break;
            strncpy(mc->fs_config, argv[i], STRING128);
            break;
         case (FS_PORT):
            if (++i == argc)
               break;
            strncpy(mc->fs_port, argv[i], STRING32);
            break;
         case (FS_SCALE):
            if (++i == argc)
               break;
            mc->fs_scale = atof (argv[i]);
            break;
         case (GROUND_HEIGHT):
            if (++i == argc)
               break;
            mc->ground_height = atof(argv[i]);
            break;
         case (HIRES):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "NET_OFF") == 0)
               mc->hires = IM_NET_OFF;
	    else if (strcasecmp(argv[i], "RECEIVE_ONLY") == 0)
               mc->hires = RECEIVE_ONLY;
            else if (strcasecmp(argv[i], "SEND_ONLY") == 0)
               mc->hires = SEND_ONLY;
            else if (strcasecmp(argv[i], "SEND_AND_RECEIVE") == 0)
               mc->hires = SEND_AND_RECEIVE;
            else
               mc->hires = IM_NET_OFF;
            break;
         case (HMD):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->hmd = TRUE;
            else
               mc->hmd = FALSE;
            break;
         case (HMD_FILE):
            if (++i == argc)
               break;
            else
               strncpy(mc->hmd_file, argv[i], STRING128);
            break;
         case (HORIZON_ANGLE):
            if (++i == argc)
               break;
            mc->horizon_angle = atof(argv[i]);
            break;
         case (HUD_FONT_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->hud_font_file, "NONE");
            else
               strncpy(mc->hud_font_file, argv[i], STRING128);
            break;
         case (HUD_LEVEL):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "ON") == 0)
               mc->hud_level = TRUE;
            else
               mc->hud_level = FALSE;
            break;
         case (IDENTIFY_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->identify_file, "NONE");
            else
               strncpy(mc->identify_file, argv[i], STRING128);
            break;
         case (INTRO_FONT_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->intro_font_file, "NONE");
            else
               strncpy(mc->intro_font_file, argv[i], STRING128);
            break;
         case (IM_JACK):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "ON") == 0)
               mc->jack = TRUE;
            else
               mc->jack = FALSE;
            break;
         case (LANDMARK_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->landmark_file, "NONE");
            else
               strncpy(mc->landmark_file, argv[i], STRING128);
            break;
         case (LEATHER_FLYBOX_CHECK):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->leather_flybox_check = TRUE;
            else
               mc->leather_flybox_check = FALSE;
            break;
         case (LEATHER_FLYBOX_PORT):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->leather_flybox_port, "NONE");
            else
               strncpy(mc->leather_flybox_port, argv[i], STRING32);
            break;
         case (LOCATION_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->location_file, "NONE");
            else
               strncpy(mc->location_file, argv[i], STRING128);
            break;
         case (LOD_FADE):
            if (++i == argc)
               break;
            mc->lod_fade = atof (argv[i]);
            break;
         case (LOD_SCALE):
            if (++i == argc)
               break;
            mc->lod_scale = atof (argv[i]);
            break;
         case (LW_RECEIVE_PORT):
            if (++i == argc)
               break;
            mc->lw_rport = atoi (argv[i]);
            break;
         case (LW_SEND_PORT):
            if (++i == argc)
               break;
            mc->lw_sport = atoi (argv[i]);
            break;
         case (LW_MACHINE_NAME):
            if (++i == argc)
               break;
            strcpy (mc->lw_toip, argv[i]);
            break;
         case (MARKING):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->marking, "NONE");
            else
               strncpy(mc->marking, argv[i], MARKINGS_LEN);
            break;
         case (MASTER):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->master, "NONE");
            else
               strncpy(mc->master, argv[i], STRING64);
            break;
         case (IM_MAX_ALTITUDE):
            if (++i == argc)
               break;
            mc->alt.max = atof (argv[i]);
            break;
         case (MODEL_FILE):
            if (++i == argc)
               break;
            strncpy(mc->model_file, argv[i], STRING128);
            break;
         case (MODSAF_MAP):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->modsaf_map, "NONE");
            else
               strncpy(mc->modsaf_map, argv[i], STRING128);
            break;
         case (MULTICAST):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->multicast.enable = TRUE;
            else
               mc->multicast.enable = FALSE;
            break;
         case (MULTICAST_GROUP):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->multicast.group, "NONE");
            else
               strncpy(mc->multicast.group, argv[i], STRING32);
            break;
         case (MULTICAST_TTL):
            if (++i == argc)
               break;
            mc->multicast.ttl = atoi(argv[i]);
            break;
         case (MULTISAMPLE):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->multisample = TRUE;
            else
               mc->multisample = FALSE;
            break;
         case (NETWORKING):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->networking = TRUE;
            else
               mc->networking = FALSE;
            break;
         case (NETWORK_FILE):
            if (++i == argc)
               break;
            strncpy(mc->network_file, argv[i], STRING128);
            break;
         case (NOTIFY_LEVEL):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "FATAL", 3) == 0)
               mc->notify_level = IM_FATAL;
            else if (strncasecmp(argv[i], "WARN", 3) == 0)
               mc->notify_level = WARN;
            else if (strncasecmp(argv[i], "NOTICE", 5) == 0)
               mc->notify_level = NOTICE;
            else if (strncasecmp(argv[i], "INFO", 3) == 0)
               mc->notify_level = INFO;
            else if (strncasecmp(argv[i], "DEBUG", 3) == 0)
               mc->notify_level = IM_DEBUG;
            else if (strncasecmp(argv[i], "FPDEBUG",3) == 0)
               mc->notify_level = FPDEBUG;
            break;
         case (NPS_PIC_DIR):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->nps_pic_dir, "NONE");
            else
               strncpy(mc->nps_pic_dir, argv[i], STRING128);
            break;
         case (ODT_MP):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "ON") == 0)
               mc->odt_mp = TRUE;
            else
               mc->odt_mp = FALSE;
            break;
         case (ODT_PORT):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->odt_port, "NONE");
            else
               strncpy(mc->odt_port, argv[i], STRING32);
            break;
         case (PANEL_GROUP):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->panel.group, "NONE");
            else
               strncpy(mc->panel.group, argv[i], STRING32);
            break;
         case (PANEL_TTL):
            if (++i == argc)
               break;
            mc->panel.ttl = atoi(argv[i]);
            break;
         case (PANEL_PORT):
            if (++i == argc)
               break;
            mc->panel.port = atoi(argv[i]);
            break;
         case (PDUPOSITION):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->pdu_position = TRUE;
            else
               mc->pdu_position = FALSE;
            break;
         case (PHASE):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "FREE", 3) == 0)
               mc->phase = ALG_FREE;
            else if (strncasecmp(argv[i], "FLOAT", 3) == 0)
               mc->phase = ALG_FLOAT;
            else if (strncasecmp(argv[i], "LOCK", 3) == 0)
               mc->phase = ALG_LOCK;
            break;
         case (PICTURES):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->pictures = TRUE;
            else
               mc->pictures = FALSE;
            break;
         case (PIPE):
            if (++i == argc)
               break;
            mc->pipe = atoi(argv[i]);
            break;
         case (PO_CTDB_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->PO.ctdb_file, "NONE");
            else
               strncpy(mc->PO.ctdb_file, argv[i], STRING128);
            break;
         case (IM_PO_DATABASE):
            if (++i == argc)
               break;
            mc->PO.database = atoi(argv[i]);
            break;
         case (IM_PO_EXERCISE):
            if (++i == argc)
               break;
            mc->PO.exercise = atoi(argv[i]);
            break;
         case (IM_PO_PORT):
            if (++i == argc)
               break;
            mc->PO.port = atoi(argv[i]);
            break;
         case (IM_PORT):
            if (++i == argc)
               break;
            mc->port = atoi(argv[i]);
            break;
         case (PROCESS_MODE):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "APPCULLDRAW", 9) == 0)
               mc->process_mode = APPCULLDRAW;
            else if (strncasecmp(argv[i], "APP_CULLDRAW", 9) == 0)
               mc->process_mode = APP_CULLDRAW;
            else if (strncasecmp(argv[i], "APPCULL_DRAW", 9) == 0)
               mc->process_mode = APPCULL_DRAW;
            else if (strncasecmp(argv[i], "APP_CULL_DRAW", 9) == 0)
               mc->process_mode = APP_CULL_DRAW;
            else if (strncasecmp(argv[i], "DEFAULT", 3) == 0)
               mc->process_mode = IM_DEFAULT;
            break;
         case (RADAR_ICON_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->radar_icon_file, "NONE");
            else
               strncpy(mc->radar_icon_file, argv[i], STRING128);
            break;
         case (RANGE_FILTER):
            if (++i == argc)
               break;
            mc->range_filter = atof (argv[i]);
            break;
         case (REMOTE_INPUT_PANEL):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->remote_input_panel = TRUE;
            else
               mc->remote_input_panel = FALSE;
            break;
         case (ROAD_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->road_file, "NONE");
            else
               strncpy(mc->road_file, argv[i], STRING128);
            break;
         case (ROUND_WORLD_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->round_world_file, "NONE");
            else
               strncpy(mc->round_world_file, argv[i], STRING128);
            break;
         case (SCORING):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->scoring = TRUE;
            else
               mc->scoring = FALSE;
            break;
         case (SEARCH_PATH):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->search_path, "NONE");
            else {
//               strncpy(mc->search_path, argv[i], MAX_SEARCH_PATH);
               if ((strlen (mc->search_path) +
                    strlen (argv[i])) <= MAX_SEARCH_PATH) {
                  sprintf (mc->search_path, "%s:%s",argv[i],mc->search_path);
               }
               else {
                  cerr << "Error: Searchpath too long." << endl
                       << "\tNot adding: " << argv[i] << endl;
               }
            }
            break;
         case (IM_SOUND):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->sound = TRUE;
            else
               mc->sound = FALSE;
            break;
         case (SOUND_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->sound_file, "NONE");
            else
               strncpy(mc->sound_file, argv[i], STRING128);
            break;
         case (STATIC_OBJS):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->static_objs = TRUE;
            else
               mc->static_objs = FALSE;
            break;
         case (STEALTH):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->stealth = TRUE;
            else
               mc->stealth = FALSE;
            break;
         case (STEREO):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->stereo = TRUE;
            else
               mc->stereo = FALSE;
            break;
         case (TARGET_ENTITY):
            if (++i == argc)
               break;
            mc->target_entity.site = atoi(argv[i]);
            if (++i == argc)
               break;
            mc->target_entity.host = atoi(argv[i]);
            if (++i == argc)
               break;
            mc->target_entity.entity = atoi(argv[i]);
            break;
         case (TARGET_BOUNDING_BOX):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "ON") == 0)
               mc->target_bbox = TRUE;
            else
               mc->target_bbox = FALSE;
            break;
         case (IM_TERRAIN_FILE):
            if (++i == argc)
               break;
            strncpy(mc->terrain_file, argv[i], STRING128);
            break;
         case (TESTPORT):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->testport, "NONE");
            else
               strncpy(mc->testport, argv[i], STRING32);
            break;
         case (TRAKER_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->traker_file, "NONE");
            else
               strncpy(mc->traker_file, argv[i], STRING128);
            break;
         case (TRAKER_PORT):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->traker_port, "NONE");
            else
               strncpy(mc->traker_port, argv[i], STRING32);
            break;
         case (TRANSPORT_FILE):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->transport_file, "NONE");
            else
               strncpy(mc->transport_file, argv[i], STRING128);
            break;
         case (TREADPORT):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->treadport, "NONE");
            else
               strncpy(mc->treadport, argv[i], STRING32);
            break;
         case (UNIPORT):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->uniport, "NONE");
            else
               strncpy(mc->uniport, argv[i], STRING32);
            break;
         case (UPPERBODY):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->upperbody, "NONE");
            else
               strncpy(mc->upperbody, argv[i], STRING32);
            break;
         case (USE_ROUND_WORLD):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "ON") == 0)
               mc->use_round = TRUE;
            else
               mc->use_round = FALSE;
            break;
         case (VEHICLE_DRIVEN):
            if (++i == argc)
               break;
	    if (strncasecmp(argv[i], "NONE", 3) == 0)
               strcpy(mc->vehicle_driven, "NONE");
            else
               strncpy(mc->vehicle_driven, argv[i], STRING32);
            break;
         case (VIEWPORT):
            fptr = &(mc->ViewPort.xmin);
            for (j = 0; j < 4; j++) {
               if (++i == argc)
                  break;
               *fptr++ = atof(argv[i]);
            }
            break;
         case (VIEW_HPR):
            fptr = &(mc->ViewHPR.heading);
            for (j = 0; j < 3; j++) {
               if (++i == argc)
                  break;
               *fptr++ = atof(argv[i]);
            }
            break;
         case (VIEW_XYZ):
            fptr = &(mc->ViewXYZ.x);
            for (j = 0; j < 3; j++) {
               if (++i == argc)
                  break;
               *fptr++ = atof(argv[i]);
            }
            break;
         case (VIEW):
            if (++i == argc)
               break;
            if (strncasecmp(argv[i], "AIRPORT", 3) == 0)
               mc->view = AIRPORT;
            else if (strncasecmp(argv[i], "CANYON", 3) == 0)
               mc->view = CANYON;
            else if (strncasecmp(argv[i], "VILLAGE", 3) == 0)
               mc->view = VILLAGE;
            else if (strncasecmp(argv[i], "PIER", 3) == 0)
               mc->view = PIER;
            else if (strncasecmp(argv[i], "START", 3) == 0)
               mc->view = IM_START;
            break;
         case (WAYPOINTS):
            if (++i == argc)
               break;
	    if (strcasecmp(argv[i], "ON") == 0)
               mc->waypoints = TRUE;
            else
               mc->waypoints = FALSE;
            break;
         case (IM_WEAPON_VIEW):
            if (++i == argc) break;
            if (strcasecmp(argv[i], "ON") == 0) mc->weapon_view = TRUE;
            else mc->weapon_view = FALSE;
            break;
         case (IM_WINDOW):
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "CLOSED") == 0)
               mc->window.type = CLOSED;
            else if (strncasecmp(argv[i], "CUSTOM", 3) == 0)
               mc->window.type = CUSTOM;
            else if (strncasecmp(argv[i], "SVGA", 3) == 0)
               mc->window.type = SVGA;
            else if (strncasecmp(argv[i], "FULL", 3) == 0)
               mc->window.type = FULL;
            else if (strncasecmp(argv[i], "NTSC", 3) == 0)
               mc->window.type = NTSC;
            else if (strcasecmp(argv[i], "PAL") == 0)
               mc->window.type = PAL;
            else if (strncasecmp(argv[i], "NORMAL", 3) == 0)
               mc->window.type = NORMAL;
            else if (strcasecmp(argv[i], "VIM") == 0)
               mc->window.type = VIM;
            else if (strcasecmp(argv[i], "THREE_TV") == 0)
               mc->window.type = THREE_TV;
            else if (strcasecmp(argv[i], "THREE_BUNKER") == 0)
               mc->window.type = THREE_BUNKER;
            else if (strcasecmp(argv[i], "TWO_TPORT") == 0)
               mc->window.type = TWO_TPORT;
            else if (strcasecmp(argv[i], "TWO_ADJACENT") == 0)
               mc->window.type = TWO_ADJACENT;
            else if (strcasecmp(argv[i], "FS_MCO") == 0)
               mc->window.type = FS_MCO;
            else if (strcasecmp(argv[i], "FS_ST_BUFFERS") == 0)
               mc->window.type = FS_ST_BUFFERS;
            break;
         case (WINDOW_SIZE):
            fptr = &(mc->window.xmin);
            for (j = 0; j < 4; j++) {
               if (++i == argc)
                  break;
               *fptr++ = atof(argv[i]);
            }
            break;
         default:
            if (symbol)
               cl_help(npsnet);
      }
#ifdef DEBUG2
      cout << "process_cl_args(): symbol=" << symbol << endl;
#endif
      if (symbol)
         continue;

      // Individual characters (mostly retained for backwards compatibility)
      switch (tolower(argv[i][1])) {
         case 'f':      // Master Config file
            if (++i == argc)
               break;
            // do nothing, should already have been processed
            break;
         case 'h':      // help
            cl_help(npsnet);
            break;
/* NO LONGER SUPPORTED
         case 'a':      // Ethernet interface
            if (++i == argc)
               break;
            strncpy(mc->ether_interf, argv[i], STRING32);
            break;
         case 'b':      // Set Bird control
            mc->bird_control = TRUE;
            break;
         case 'g':      // Multicast group number
            if (++i == argc)
               break;
            strncpy(mc->multicast.group, argv[i], STRING32);
            break;
         case 'l':      // Set CPU Lock
            mc->cpu_lock = TRUE;
            break;
         case 'm':
            mc->multicast.enable = TRUE;
            break;
         case 'n':	// Disablers
            if (++i == argc)
               break;
            if (strcasecmp(argv[i], "animations") == 0) {
               mc->animations = FALSE;
            } else if (strcasecmp(argv[i], "dynamic") == 0) {
               mc->dynamic_objs = FALSE;
            } else if (strcasecmp(argv[i], "fcs") == 0) {
               mc->fcs.check = FALSE;
            } else if (strcasecmp(argv[i], "flybox") == 0) {
               mc->flybox = FALSE;
            } else if (strcasecmp(argv[i], "network") == 0) {
               mc->networking = FALSE;
            } else if (strcasecmp(argv[i], "pictures") == 0) {
               mc->pictures = FALSE;
            } else if (strcasecmp(argv[i], "sound") == 0) {
               mc->sound = FALSE;
            } else if (strcasecmp(argv[i], "static") == 0) {
               mc->static_objs = FALSE;
            }
            break;
         case 'p':      // Port number
            mc->port = atoi(argv[i]);
            break;
         case 'r':
            mc->remote_input_panel = TRUE;
            break;
         case 't':      // Multicast TTL
            if (++i == argc)
               break;
            mc->multicast.ttl = atoi(argv[i]);
            break;
         case 'v':      // Driven vehicle name
            if (++i == argc)
               break;
            strncpy(mc->vehicle_driven, argv[i], STRING32);
            break;
         case 'x':      // Set DIS Exercise ID
            if (++i == argc)
               break;
            mc->DIS.exercise = atoi(argv[i]);
            break;
         case 'z':      // Traker port
            if (++i == argc)
               break;
            strncpy(mc->traker_port, argv[i], STRING32);
            break;
*/
         default:
            cl_help(npsnet);
      }

   } // end for each command line arg

}


void InitManager::cl_help(char *progname)
{
   cout << "Usage: " << progname << " [command line opts]" << endl;
   cout << "Command line opts:\n";
   cout << "\t-f master_config_file\n";
   cout << "\t-h (help)\n\n";
   cout << "\t-ANIMATIONS [ON|OFF]\n";
   cout << "\t-ANIMATION_FILE [filename|NONE]\n";
   cout << "\t-AUTO_FILE [filename|NONE]\n";
   cout << "\t-AUTO_STATE [ON|OFF]\n";
   cout << "\t-BDIHML [ON|OFF]\n";
   cout << "\t-BLINKING_LIGHTS_FILE [filename|NONE]\n";
   cout << "\t-BOUNDING_BOX xmin xmax ymin ymax\n";
   cout << "\t-CHANNEL channel_number\n";
   cout << "\t-CLIP_PLANES near far\n";
   cout << "\t-CONTROL [FCS|FLYBOXIN|KAFLIGHT|KEYBOARD|SPACEBALL|UNIPORT|]\n";
   cout << "\t\t\tSHIP_PANEL|SUB_PANEL|AUSA_ODT]\n";
   cout << "\t-CPU_LOCK [ON|OFF]\n";
   cout << "\t-DB_OFFSET working_group_size\n";
   cout << "\t-DB_PAGES_ALLOWED number_pages_allowed_in_memory\n";
   cout << "\t-DB_TSG_FILE [filename|NONE]\n";
   cout << "\t-DEAD_SWITCH [ON|OFF]\n";
   cout << "\t-DIGUY_SPEED_RATIO ratio\n";
   cout << "\t-DIGUY_POSITION_ERROR error\n";
   cout << "\t-DIGUY_TIME_CONSTANT constant\n";
   cout << "\t-DIGUY_TRANS_SMOOTHNESS [TRANS_IMMEDIATE|IMMEDIATE_TRANS\n"
        << "\t\t|TRANS_SMOOTHEST]\n";
   cout << "\t-DIGUY_POSITION_MODE [SMOOTH_MODE|PRECISE_MODE]\n";
   cout << "\t-DIGUY_POSITION_MECHANISM [DIGUY_INTERNAL|NPSNET_INTERNAL]\n";
   cout << "\t-DIS_EXERCISE exercise_number\n";
   cout << "\t-DIS_HEARTBEAT time\n";
   cout << "\t-DIS_POSDELTA threshold\n";
   cout << "\t-DIS_ROTDELTA threshold\n";
   cout << "\t-DIS_TIME_OUT timeout\n";
   cout << "\t-DUMP_FILE filename\n";
   cout << "\t-DVW_PATHS filename\n";
   cout << "\t-DYNAMIC_OBJS [ON|OFF]\n";
   cout << "\t-EARTH_SKY_MODE [FAST|TAG|SKY|SKY_GROUND|SKY_CLEAR]\n";
   cout << "\t-ENVIRONMENTAL [filename|NONE]\n";
   cout << "\t-ENTITY_FILE [filename|NONE]\n";
   cout << "\t-ETHER_INTERF [name|NONE]\n";
   cout << "\t-FCS_CHECK [ON|OFF]\n";
   cout << "\t-FCS_MP [ON|OFF]\n";
   cout << "\t-FCS_PORT [portname|NONE]\n";
   cout << "\t-FCS_PROMPT [ON|OFF]\n";
   cout << "\t-FS_CONFIG [config_file|NONE]\n";
   cout << "\t-FS_PORT [port_name|NONE]\n";
   cout << "\t-FS_SCALE scale_factor\n";
   cout << "\t-[FIELD_OF_VIEW|FOV] x_fov y_fov\n";
   cout << "\t-FILTER_FILE [filename|NONE]\n";
   cout << "\t-FLYBOX [ON|OFF]\n";
   cout << "\t-FLYBOX_PORT [portname|NONE]\n";
   cout << "\t-FOG [ON|OFF]\n";
   cout << "\t-FOG_NEAR distance\n";
   cout << "\t-FOG_FAR distance\n";
   cout << "\t-FOG_TYPE [FOG_VTX_LIN|FOG_VTX_EXP|FOG_VTX_EXP2|FOG_PIX_LIN|\n";
   cout << "\t\tFOG_PIX_EXP|FOG_PIX_EXP2|FOG_PIX_SPLINE]\n";
   cout << "\t-FORCE [BLUE|RED|WHITE|OTHER]\n";
   cout << "\t-FRAME_RATE rate\n";
   cout << "\t-GROUND_HEIGHT elevation\n";
   cout << "\t-[HEAD_MOUNTED_DISPLAY|HMD] [ON|OFF]\n";
   cout << "\t-HMD_FILE filename\n";
   cout << "\t-HELP\n";
   cout << "\t-HIRES [NET_OFF|RECEIVE_ONLY|SEND_ONLY|SEND_AND_RECEIVE]\n";
   cout << "\t-HORIZON_ANGLE angle\n";
   cout << "\t-HUD_FONT_FILE [filename|NONE]\n";
   cout << "\t-HUD_LEVEL [ON|OFF]\n";
   cout << "\t-IDENTIFY_FILE [filename|NONE]\n";
   cout << "\t-INTRO_FONT_FILE [filename|NONE]\n";
   cout << "\t-JACK [ON|OFF]\n";
   cout << "\t-LANDMARK_FILE [filename|NONE]\n";
   cout << "\t-LEATHER_FLYBOX_CHECK [ON|OFF]\n";
   cout << "\t-LEATHER_FLYBOX_PORT [portname|NONE]\n";
   cout << "\t-LOCATION_FILE [filename|NONE]\n";
   cout << "\t-LOD_FADE lod_fade_ratio\n";
   cout << "\t-LOD_SCALE lod_scale_ratio\n";
   cout << "\t-LW_RECEIVE_PORT port_number\n";
   cout << "\t-LW_SEND_PORT port_number\n";
   cout << "\t-LW_MACHINE_NAME machine_name\n";
   cout << "\t-MARKING [marking_string|NONE]\n";
   cout << "\t-MASTER [machine_name|NONE]\n";
   cout << "\t-MAX_ALTITUDE max_altitude\n";
   cout << "\t-MODEL_FILE filename\n";
   cout << "\t-MODSAF_MAP [filename|NONE]\n";
   cout << "\t-MULTICAST [ON|OFF]\n";
   cout << "\t-MULTICAST_GROUP [IP_addr|NONE]\n";
   cout << "\t-MULTICAST_TTL time\n";
   cout << "\t-MULTISAMPLE [ON|OFF]\n";
   cout << "\t-NETWORKING [ON|OFF]\n";
   cout << "\t-NETWORK_FILE filename\n";
   cout << "\t-NOTIFY_LEVEL [FATAL|WARN|NOTICE|INFO|DEBUG]\n";
   cout << "\t-NPS_PIC_DIR [dirname|NONE]\n";
   cout << "\t-ODT_MP [ON|OFF]\n";
   cout << "\t-ODT_PORT [portname|NONE]\n";
   cout << "\t-PDUPOSITION [ON|OFF]\n";
   cout << "\t-PHASE [FREE|FLOAT|LOCK]\n";
   cout << "\t-PICTURES [ON|OFF]\n";
   cout << "\t-PIPE pipe_number\n";
   cout << "\t-PO_CTDB_FILE [filename|NONE]\n";
   cout << "\t-PO_DATABASE db_number\n";
   cout << "\t-PO_EXERCISE exercise\n";
   cout << "\t-PO_PORT port_number\n";
   cout << "\t-PORT port_number\n";
   cout << "\t-PROCESS_MODE [APPCULLDRAW|APP_CULLDRAW|APPCULL_DRAW|\n";
   cout << "\t\t\tAPP_CULL_DRAW|DEFAULT]\n";
   cout << "\t-RADAR_ICON_FILE [filename|NONE]\n";
   cout << "\t-RANGE_FILTER range\n";
   cout << "\t-REMOTE_INPUT_PANEL [ON|OFF]\n";
   cout << "\t-ROAD_FILE [filename|NONE]\n";
   cout << "\t-ROUND_WORLD_FILE [filename|NONE]\n";
   cout << "\t-SCORING [ON|OFF]\n";
   cout << "\t-SEARCH_PATH [pathname|NONE]\n";
   cout << "\t-SOUND [ON|OFF]\n";
   cout << "\t-SOUND_FILE [filename|NONE]\n";
   cout << "\t-STATIC_OBJS [ON|OFF]\n";
   cout << "\t-STEALTH [ON|OFF]\n";
   cout << "\t-STEREO [ON|OFF]\n";
   cout << "\t-TARGET_ENTITY site host entity\n";
   cout << "\t-TERRAIN_FILE filename\n";
   cout << "\t-TESTPORT [portname|NONE]\n";
   cout << "\t-TRAKER_FILE [filename|NONE]\n";
   cout << "\t-TRAKER_PORT [portname|NONE]\n";
   cout << "\t-TRANSPORT_FILE [filename|NONE]\n";
   cout << "\t-TREADPORT [portname|NONE]\n";
   cout << "\t-UNIPORT [portname|NONE]\n";
   cout << "\t-VEHICLE_DRIVEN [filename|NONE]\n";
   cout << "\t-VIEW [AIRPORT|CANYON|VILLAGE|PIER|START]\n";
   cout << "\t-VIEWPORT xmin xmax ymin ymax\n";
   cout << "\t-VIEW_HPR heading pitch roll\n";
   cout << "\t-VIEW_XYZ x y z\n";
   cout << "\t-USE_ROUND_WORLD [ON|OFF]\n";
   cout << "\t-WAYPOINTS [ON|OFF]\n";
   cout << "\t-WEAPON_VIEW [ON|OFF]\n";
   cout << "\t-WINDOW [CLOSED|CUSTOM|SVGA|FULL|NTSC|PAL|NORMAL|\n";
   cout << "\t\t\tVIM|THREE_TV|THREE_BUNKER|TWO_TPORT|TWO_ADJACENT|FS_MCO|FS_ST_BUFFERS]\n";
   cout << "\t-WINDOW_SIZE xmin xmax ymin ymax\n";

   exit(0);
}

// EOF
