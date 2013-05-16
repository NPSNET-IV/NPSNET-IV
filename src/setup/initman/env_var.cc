// env_var.cc

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include "im.tab.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "imextern.h"
#include "imclass.h"
#include "pdu.h"
#include "setup_const.h"

//extern "C" {
//   void perror(char*);
//   void sscanf(char*, char*, ...);
//};

extern struct MC *mc;

// Usage: setenv VARIABLE value <or>
//        setenv VARIABLE "value1 value2 etc" <quotations mandatory>
//
// note: unlike file commands, VARIABLEs cannot be abbreviated
//       (getenv() looks for an exact match)


void InitManager::process_environment_vars()
{
   cout << "PROCESSING environment vars.\n";
   process_environment_vars_filenames();
   process_environment_vars_others();
}


void InitManager::process_environment_vars_filenames()
   // Get filenames only.
{
   char		*bucket;

   //cout << "PROCESSING environment vars (filenames only).\n";

   if (bucket = getenv("ANIMATION_FILE")) {
      strncpy(mc->animation_file, bucket, STRING128);
   }
   if (bucket = getenv("AUTO_FILE")) {
      strncpy(mc->auto_file, bucket, STRING128);
   }
   if (bucket = getenv("AUTO_STATE")) {
      mc->auto_state = get_boolean(bucket);
   }
   if (bucket = getenv("BLINKING_LIGHTS_FILE")) {
      strncpy(mc->blinking_lights_file, bucket, STRING128);
   }
   if (bucket = getenv("CURSOR_FILE")) {
      strncpy(mc->cursor_file, bucket, STRING128);
   }
   if (bucket = getenv("DUMP_FILE")) {
      strncpy(mc->dump_file, bucket, STRING128);
   }
   if (bucket = getenv("DVW_PATHS")) {
      strncpy(mc->dvw_file, bucket, STRING128);
   }
   if (bucket = getenv("ENVIRONMENTAL")) {
      strncpy(mc->environmental_file, bucket, STRING128);
   }
   if (bucket = getenv("ENTITY_FILE")) {
      strncpy(mc->entity_file, bucket, STRING128);
   }
   if (bucket = getenv("FILTER_FILE")) {
      strncpy(mc->filter_file, bucket, STRING128);
   }
   if (bucket = getenv("HUD_FONT_FILE")) {
      strncpy(mc->hud_font_file, bucket, STRING128);
   }
   if (bucket = getenv("HUD_LEVEL")) {
      mc->hud_level = get_boolean(bucket);
   }
   if (bucket = getenv("IDENTIFY_FILE")) {
      strncpy(mc->identify_file, bucket, STRING128);
   }
   if (bucket = getenv("INTRO_FONT_FILE")) {
      strncpy(mc->intro_font_file, bucket, STRING128);
   }
   if (bucket = getenv("LANDMARK_FILE")) {
      strncpy(mc->landmark_file, bucket, STRING128);
   }
   if (bucket = getenv("LOCATION_FILE")) {
      strncpy(mc->location_file, bucket, STRING128);
   }
   if (bucket = getenv("LOD_FADE")) {
      mc->lod_fade = atof (bucket);
   }
   if (bucket = getenv("LOD_SCALE")) {
      mc->lod_scale = atof (bucket);
   }
   if (bucket = getenv("LW_RECIEVE_PORT")) {
      mc->lw_rport = atoi (bucket);
   }
   if (bucket = getenv("LW_SEND_PORT")) {
      mc->lw_sport = atoi (bucket);
   }
   if (bucket = getenv("LW_MACHINE_NAME")) {
      strncpy(mc->lw_toip, bucket, STRING128);
   }
   if (bucket = getenv("MODEL_FILE")) {
      strncpy(mc->model_file, bucket, STRING128);
   }
   if (bucket = getenv("NETWORK_FILE")) {
      strncpy(mc->network_file, bucket, STRING128);
   }
   if (bucket = getenv("PO_CTDB_FILE")) {
      strncpy(mc->PO.ctdb_file, bucket, STRING128);
   }
   if (bucket = getenv("RADAR_ICON_FILE")) {
      strncpy(mc->radar_icon_file, bucket, STRING128);
   }
   if (bucket = getenv("RANGE_FILTER")) {
      mc->range_filter = atof (bucket);
   }
   if (bucket = getenv("ROAD_FILE")) {
      strncpy(mc->road_file, bucket, STRING128);
   }
   if (bucket = getenv("ROUND_WORLD_FILE")) {
      strncpy(mc->round_world_file, bucket, STRING128);
   }
   if (bucket = getenv("SOUND_FILE")) {
      strncpy(mc->sound_file, bucket, STRING128);
   }
   if (bucket = getenv("TERRAIN_FILE")) {
      strncpy(mc->terrain_file, bucket, STRING128);
   }
   if (bucket = getenv("TRAKER_FILE")) {
      strncpy(mc->traker_file, bucket, STRING128);
   }
   if (bucket = getenv("TRANSPORT_FILE")) {
      strncpy(mc->transport_file, bucket, STRING128);
   }

} // process_environment_vars_filenames()


int InitManager::get_boolean(char *boolstr)
{
   if (strcasecmp(boolstr, "ON") == 0)
      return (TRUE);
   return (FALSE);
}


void InitManager::process_environment_vars_others()
   // Get variables other than filenames.
{
   char		*bucket;

   //cout << "PROCESSING environment vars.\n";

   if (bucket = getenv("ANIMATIONS")) {
      mc->animations = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv ("BDIHML")) {
      mc->bdihml = get_boolean(bucket);
   }
   if (bucket = getenv("BOUNDING_BOX")) {
      sscanf(bucket, "%f %f %f %f",
        &(mc->BoundingBox.xmin),
        &(mc->BoundingBox.xmax),
        &(mc->BoundingBox.ymin),
        &(mc->BoundingBox.ymax));
      //free(bucket);
   }
   if (bucket = getenv("CHANNEL")) {
      mc->channel = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("CLIP_PLANES")) {
      sscanf(bucket, "%f %f",
        &(mc->ClipPlanes.cp_near),
        &(mc->ClipPlanes.cp_far));
      //free(bucket);
   }
   if (bucket = getenv("CONTROL")) {
      if (strcasecmp(bucket, "FCS") == 0)
         mc->control = FCS;
      else if (strncasecmp(bucket, "FLYBOXIN", 3) == 0)
         mc->control = FLYBOXIN;
      else if (strncasecmp(bucket, "LEATHER_FLYBOX", 3) == 0)
         mc->control = LEATHER_FLYBOX;
      else if (strncasecmp(bucket, "KAFLIGHT", 3) == 0)
         mc->control = KAFLIGHT;
      else if (strncasecmp(bucket, "KEYBOARD", 3) == 0)
         mc->control = KEYBOARD;
      else if (strcasecmp(bucket, "PAD") == 0)
         mc->control = KEYBOARD;
      else if (strncasecmp(bucket, "SPACEBALL", 3) == 0)
         mc->control = SPACEBALL;
      else if (strncasecmp(bucket, "BALL", 3) == 0)
         mc->control = SPACEBALL;
      else if (strncasecmp(bucket, "DRIVE", 3) == 0)
         mc->control = SPACEBALL;
      else if (strncasecmp(bucket, "UNIPORT", 3) == 0)
         mc->control = UNIPORT;
      else if (strncasecmp(bucket, "SHIP_PANEL", 3) == 0)
         mc->control = SHIP_PANEL;
      else if (strncasecmp(bucket, "SUB_PANEL", 3) == 0)
         mc->control = SUB_PANEL;
      else if (strncasecmp(bucket, "AUSA_ODT", 5) == 0)
         mc->control = AUSA_ODT;
      //free(bucket);
   }
   if (bucket = getenv("CPU_LOCK")) {
      mc->cpu_lock = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DB_OFFSET")) {
      mc->db_offset = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DB_PAGES_ALLOWED")) {
      mc->db_pages_allowed = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DB_TSG_FILE")) {
      strncpy(mc->db_tsg_file, bucket, STRING128);
   }
   if (bucket = getenv("DEAD_SWITCH")) {
      mc->dead_switch = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DIGUY_SPEED_RATIO")) {
      mc->diguy_info.speed_ratio = atof(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DIGUY_POSITION_ERROR")) {
      mc->diguy_info.position_error = atof(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DIGUY_TIME_CONSTANT")) {
      mc->diguy_info.time_constant = atof(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DIGUY_TRANS_SMOOTHNESS")) {
      if (strncasecmp(bucket, "TRANS_IMMEDIATE", 9) == 0) {
         mc->diguy_info.trans_smoothness = TRANS_IMMEDIATE;
      }
      if (strncasecmp(bucket, "IMMEDIATE_TRANS", 9) == 0) {
         mc->diguy_info.trans_smoothness = IMMEDIATE_TRANS;
      }
      if (strncasecmp(bucket, "TRANS_SMOOTHEST", 9) == 0) {
         mc->diguy_info.trans_smoothness = TRANS_SMOOTHEST;
      }
   }
   if (bucket = getenv("DIGUY_POSITION_MODE")) {
      if (strncasecmp(bucket, "SMOOTH_MODE", 6) == 0) {
         mc->diguy_info.position_mode = SMOOTH_MODE;
      }
      if (strncasecmp(bucket, "PRECISE_MODE", 7) == 0) {
         mc->diguy_info.position_mode = SMOOTH_MODE;
      }
   }
   if (bucket = getenv("DIGUY_POSITION_MECHANISM")) {
      if (strncasecmp(bucket, "DIGUY_INTERNAL", 9) == 0) {
         mc->diguy_info.position_mechanism = DIGUY_INTERNAL;
      }
      if (strncasecmp(bucket, "NPSNET_INTERNAL", 10) == 0) {
         mc->diguy_info.position_mechanism = NPSNET_INTERNAL;
      }
   }
   if (bucket = getenv("DIS_EXERCISE")) {
      mc->DIS.exercise = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DIS_ENUM")) {
      if (strcasecmp(bucket, "DISv203") == 0) {
         mc->dis_enum = DISv203;
      }
      else if (strcasecmp(bucket, "DISv204") == 0) {
         mc->dis_enum = DISv204;
      }
      else {
         mc->dis_enum = DISv204;
      }
   }
   if (bucket = getenv("DIS_HEARTBEAT")) {
      mc->DIS.heartbeat = atof(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DIS_POSDELTA")) {
      mc->DIS.posdelta = atof(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DIS_ROTDELTA")) {
      mc->DIS.rotdelta = atof(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DIS_TIME_OUT")) {
      mc->DIS.timeout = atof(bucket);
      //free(bucket);
   }
   if (bucket = getenv("DYNAMIC_OBJS")) {
      mc->dynamic_objs = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("EARTH_SKY_MODE")) {
      if (strncasecmp(bucket, "FAST", 3) == 0)
         mc->es_mode = FAST;
      else if (strcasecmp(bucket, "TAG") == 0)
         mc->es_mode = TAG;
      else if (strncasecmp(bucket, "SKY_GROUND", 5) == 0)
         mc->es_mode = SKY_GROUND;
      else if (strncasecmp(bucket, "SKY_CLEAR", 5) == 0)
         mc->es_mode = SKY_CLEAR;
      else if (strcasecmp(bucket, "SKY") == 0)
         mc->es_mode = SKY;
      //free(bucket);
   }
   if (bucket = getenv("ETHER_INTERF")) {
      strncpy(mc->ether_interf, bucket, STRING32);
      //free(bucket);
   }
   if (bucket = getenv("EXERCISE")) {
      mc->DIS.exercise = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("FCS_CHECK")) {
      mc->fcs.check = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("FCS_MP")) {
      mc->fcs.mp = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("FCS_PORT")) {
      strncpy(mc->fcs.port, bucket, STRING32);
      //free(bucket);
   }
   if (bucket = getenv("FCS_PROMPT")) {
      mc->fcs.prompt = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("FOV")) {
      sscanf(bucket, "%f %f", &(mc->FoV.x), &(mc->FoV.y));
      //free(bucket);
   }
   if (bucket = getenv("FIELD_OF_VIEW")) {
      sscanf(bucket, "%f %f", &(mc->FoV.x), &(mc->FoV.y));
      //free(bucket);
   }
   if (bucket = getenv("FLYBOX")) {
      mc->flybox = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("FLYBOX_PORT")) {
      strncpy(mc->flybox_port, bucket, STRING32);
      //free(bucket);
   }
   if (bucket = getenv("FOG")) {
      mc->fog = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("FOG_NEAR")) {
      mc->fog_near = atof (bucket);
   }
   if (bucket = getenv("FOG_FAR")) {
      mc->fog_far = atof (bucket);
   }
   if (bucket = getenv("FOG_TYPE")) {
      if (strcasecmp(bucket, "FOG_VTX_LIN") == 0)
         mc->fog_type = FOG_VTX_LIN;
      else if (strcasecmp(bucket, "FOG_VTX_EXP") == 0)
         mc->fog_type = FOG_VTX_EXP;
      else if (strcasecmp(bucket, "FOG_VTX_EXP2") == 0)
         mc->fog_type = FOG_VTX_EXP2;
      else if (strcasecmp(bucket, "FOG_PIX_LIN") == 0)
         mc->fog_type = FOG_PIX_LIN;
      else if (strcasecmp(bucket, "FOG_PIX_EXP") == 0)
         mc->fog_type = FOG_PIX_EXP;
      else if (strcasecmp(bucket, "FOG_PIX_EXP2") == 0)
         mc->fog_type = FOG_PIX_EXP2;
      else if (strcasecmp(bucket, "FOG_PIX_SPLINE") == 0)
         mc->fog_type = FOG_PIX_SPLINE;
      else {
               cout << "Warning: Unknown fog type: " << bucket
                    << "\tUsing: FOG_VTX_LIN" << endl;

               mc->fog_type = FOG_VTX_LIN;
            }
   }
   if (bucket = getenv("FORCE")) {
      if (strncasecmp(bucket, "BLUE", 3) == 0)
         mc->force = ForceID_Blue;
      else if (strcasecmp(bucket, "RED") == 0)
         mc->force = ForceID_Red;
      else if (strncasecmp(bucket, "WHITE", 3) == 0)
         mc->force = ForceID_White;
      else if (strncasecmp(bucket, "OTHER", 3) == 0)
         mc->force = ForceID_Other;
      //free(bucket);
   }
   if (bucket = getenv("FRAME_RATE")) {
      mc->frame_rate = atof(bucket);
      //free(bucket);
   }
   if (bucket = getenv("FS_CONFIG")) {
      strncpy(mc->fs_config, bucket, STRING128);
   }
   if (bucket = getenv("FS_PORT")) {
      strncpy(mc->fs_port, bucket, STRING32);
   }
   if (bucket = getenv("FS_SCALE")) {
      mc->fs_scale = atof (bucket);
   }
   if (bucket = getenv("GROUND_HEIGHT")) {
      mc->ground_height = atof(bucket);
      //free(bucket);
   }
   if (bucket = getenv("HEAD_MOUNTED_DISPLAY")) {
      mc->hmd = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("HIRES")) {
      if (strncasecmp(bucket, "NET_OFF", 3) == 0)
         mc->hires = IM_NET_OFF;
       else if (strncasecmp(bucket, "RECEIVE_ONLY", 3) == 0)
         mc->hires = RECEIVE_ONLY;
       else if (strncasecmp(bucket, "SEND_ONLY", 6) == 0)
         mc->hires = SEND_ONLY;
       else if (strncasecmp(bucket, "SEND_AND_RECEIVE", 6) == 0)
         mc->hires = SEND_AND_RECEIVE;
       else 
         mc->hires = IM_NET_OFF;
      //free(bucket);
   }
   if (bucket = getenv("HMD")) {
      mc->hmd = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("HMD_FILE")) {
      strncpy(mc->hmd_file, bucket, STRING128);
   }
   if (bucket = getenv("HORIZON_ANGLE")) {
      mc->horizon_angle = atof(bucket);
      //free(bucket);
   }
   if (bucket = getenv("JACK")) {
      mc->jack = get_boolean(bucket);
   }
   if (bucket = getenv("LEATHER_FLYBOX_CHECK")) {
      mc->leather_flybox_check = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("LEATHER_FLYBOX_PORT")) {
      strncpy(mc->leather_flybox_port, bucket, STRING32);
      //free(bucket);
   }
   if (bucket = getenv("MARKING")) {
      strncpy(mc->marking, bucket, MARKINGS_LEN);
      //free(bucket);
   }
   if (bucket = getenv("MASTER")) {
      strncpy(mc->master, bucket, STRING64);
      //free(bucket);
   }
   if (bucket = getenv("MAX_ALTITUDE")) {
      mc->alt.max = atof (bucket);
   }
   if (bucket = getenv("MODSAF_MAP")) {
      strncpy(mc->modsaf_map, bucket, STRING128);
   }
   if (bucket = getenv("MULTICAST")) {
      mc->multicast.enable = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("MULTICAST_GROUP")) {
      strncpy(mc->multicast.group, bucket, STRING32);
      //free(bucket);
   }
   if (bucket = getenv("MULTICAST_TTL")) {
      mc->multicast.ttl = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("MULTISAMPLE")) {
      mc->multisample = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("NETWORKING")) {
      mc->networking = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("NOTIFY_LEVEL")) {
      if (strncasecmp(bucket, "FATAL", 3) == 0)
         mc->notify_level = IM_FATAL;
      else if (strncasecmp(bucket, "WARN", 3) == 0)
         mc->notify_level = WARN;
      else if (strncasecmp(bucket, "NOTICE", 5) == 0)
         mc->notify_level = NOTICE;
      else if (strncasecmp(bucket, "INFO", 3) == 0)
         mc->notify_level = INFO;
      else if (strncasecmp(bucket, "DEBUG", 3) == 0)
         mc->notify_level = IM_DEBUG;
      else if (strncasecmp(bucket, "FPDEBUG", 3) == 0)
         mc->notify_level = FPDEBUG;
      //free(bucket);
   }
   if (bucket = getenv("NPS_PIC_DIR")) {
      strncpy(mc->nps_pic_dir, bucket, STRING128);
   }
   if (bucket = getenv("ODT_PORT")) {
      strncpy(mc->odt_port, bucket, STRING32);
   }
   if (bucket = getenv("ODT_MP")) {
      mc->odt_mp = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("PANEL_GROUP")) { 
      strncpy(mc->panel.group, bucket, STRING32);
      //free(bucket);
   }
   if (bucket = getenv("PANEL_TTL")) {
      mc->panel.ttl = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("PANEL_PORT")) {
      mc->panel.port = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("PDUPOSITION")) {
      mc->pdu_position = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("PHASE")) {
      if (strncasecmp(bucket, "FREE", 3) == 0)
         mc->phase = ALG_FREE;
      else if (strncasecmp(bucket, "FLOAT", 3) == 0)
         mc->phase = ALG_FLOAT;
      else if (strncasecmp(bucket, "LOCK", 3) == 0)
         mc->phase = ALG_LOCK;
      //free(bucket);
   }
   if (bucket = getenv("PICTURES")) {
      mc->pictures = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("PIPE")) {
      mc->pipe = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("PO_DATABASE")) {
      mc->PO.database = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("PO_EXERCISE")) {
      mc->PO.exercise = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("PO_PORT")) {
      mc->PO.port = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("PORT")) {
      mc->port = atoi(bucket);
      //free(bucket);
   }
   if (bucket = getenv("PROCESS_MODE")) {
      if (strncasecmp(bucket, "APPCULLDRAW", 9) == 0)
         mc->process_mode = APPCULLDRAW;
      else if (strncasecmp(bucket, "APP_CULLDRAW", 9) == 0)
         mc->process_mode = APP_CULLDRAW;
      else if (strncasecmp(bucket, "APPCULL_DRAW", 9) == 0)
         mc->process_mode = APPCULL_DRAW;
      else if (strncasecmp(bucket, "APP_CULL_DRAW", 9) == 0)
         mc->process_mode = APP_CULL_DRAW;
      else if (strncasecmp(bucket, "DEFAULT", 3) == 0)
         mc->process_mode = IM_DEFAULT;
      //free(bucket);
   }
   if (bucket = getenv("REMOTE_INPUT_PANEL")) {
      mc->remote_input_panel = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("SCORING")) {
      mc->scoring = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("SEARCH_PATH")) {
      strncpy(mc->search_path, bucket, MAX_SEARCH_PATH);
      //free(bucket);
   }
   if (bucket = getenv("SOUND")) {
      mc->sound = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("STATIC_OBJS")) {
      mc->static_objs = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("STEALTH")) {
      mc->stealth = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("STEREO")) {
      mc->stereo = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("STEREO")) {
      mc->stereo = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("TARGET_BOUNDING_BOX")) {
      mc->target_bbox = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("TARGET_ENTITY")) {
      sscanf(bucket, "%d %d %d", &(mc->target_entity.site),
                                 &(mc->target_entity.host),
                                 &(mc->target_entity.entity));
   }
   if (bucket = getenv("TESTPORT")) {
      strncpy(mc->testport, bucket, STRING32);
   }
   if (bucket = getenv("TRAKER_PORT")) {
      strncpy(mc->traker_port, bucket, STRING32);
      //free(bucket);
   }
   if (bucket = getenv("TREADPORT")) {
      strncpy(mc->treadport, bucket, STRING32);
   }
   if (bucket = getenv("UNIPORT")) {
      strncpy(mc->uniport, bucket, STRING32);
   }
   if (bucket = getenv("UPPERBODY")) {
      strncpy(mc->upperbody, bucket, STRING32);
   }
   if (bucket = getenv("USE_ROUND_WORLD")) {
      mc->use_round = get_boolean(bucket);
   }
   if (bucket = getenv("VEHICLE_DRIVEN")) {
      strncpy(mc->vehicle_driven, bucket, STRING32);
      //free(bucket);
   }
   if (bucket = getenv("VIEW")) {
      if (strncasecmp(bucket, "AIRPORT", 3) == 0)
         mc->view = AIRPORT;
      else if (strncasecmp(bucket, "CANYON", 3) == 0)
         mc->view = CANYON;
      else if (strncasecmp(bucket, "VILLAGE", 3) == 0)
         mc->view = VILLAGE;
      else if (strncasecmp(bucket, "PIER", 3) == 0)
         mc->view = PIER;
      else if (strncasecmp(bucket, "START", 3) == 0)
         mc->view = IM_START;
      //free(bucket);
   }
   if (bucket = getenv("VIEWPORT")) {
      sscanf(bucket, "%f %f %f %f",
        &(mc->ViewPort.xmin),
        &(mc->ViewPort.xmax),
        &(mc->ViewPort.ymin),
        &(mc->ViewPort.ymax));
      //free(bucket);
   }
   if (bucket = getenv("VIEW_HPR")) {
      sscanf(bucket, "%f %f %f",
        &(mc->ViewHPR.heading),
        &(mc->ViewHPR.pitch),
        &(mc->ViewHPR.roll));
      //free(bucket);
   }
   if (bucket = getenv("VIEW_XYZ")) {
      sscanf(bucket, "%f %f %f",
        &(mc->ViewXYZ.x),
        &(mc->ViewXYZ.y),
        &(mc->ViewXYZ.z));
      //free(bucket);
   }
   if (bucket = getenv("WAYPOINTS")) {
      mc->waypoints = get_boolean(bucket);
      //free(bucket);
   }
   if (bucket = getenv("WEAPON_VIEW")) {
      mc->weapon_view = get_boolean(bucket);
   }
   if (bucket = getenv("WINDOW")) {
      if (strcasecmp(bucket, "CLOSED") == 0)
         mc->window.type = CLOSED;
      else if (strncasecmp(bucket, "CUSTOM", 3) == 0)
         mc->window.type = CUSTOM;
      else if (strncasecmp(bucket, "SVGA", 3) == 0)
         mc->window.type = SVGA;
      else if (strncasecmp(bucket, "FULL", 3) == 0)
         mc->window.type = FULL;
      else if (strncasecmp(bucket, "NTSC", 3) == 0)
         mc->window.type = NTSC;
      else if (strcasecmp(bucket, "PAL") == 0)
         mc->window.type = PAL;
      else if (strncasecmp(bucket, "NORMAL", 3) == 0)
         mc->window.type = NORMAL;
      else if (strcasecmp(bucket, "VIM") == 0)
         mc->window.type = VIM;
      else if (strcasecmp(bucket, "THREE_TV") == 0)
         mc->window.type = THREE_TV;
      else if (strcasecmp(bucket, "THREE_BUNKER") == 0)
         mc->window.type = THREE_BUNKER;
      else if (strcasecmp(bucket, "TWO_TPORT") == 0)
         mc->window.type = TWO_TPORT;
      else if (strcasecmp(bucket, "TWO_ADJACENT") == 0)
         mc->window.type = TWO_ADJACENT;
      else if (strcasecmp(bucket, "FS_MCO") == 0)
         mc->window.type = FS_MCO;
      else if (strcasecmp(bucket, "FS_ST_BUFFERS") == 0)
         mc->window.type = FS_ST_BUFFERS;
      //free(bucket);
   }
   if (bucket = getenv("WINDOW_SIZE")) {
      sscanf(bucket, "%f %f %f %f",
                &(mc->window.xmin),
                &(mc->window.xmax),
                &(mc->window.ymin),
                &(mc->window.ymax));
      //free(bucket);
   }

} // process_environment_vars()

// EOF
