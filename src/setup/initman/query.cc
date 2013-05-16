// query.cc

#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <bstring.h>
#include "im.tab.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "imextern.h"
#include "imclass.h"
#include "setup_const.h"


extern struct defs *Defaults;
extern struct MC *mc;

int InitManager::query(int request, union query_data *bucket, int mode)
   // Separate GET and PUT requests to initialization data.
{
   if (bucket == NULL)
      return (-1);

   switch (mode) {
      case (PUT):
         put_query(request, bucket);
         break;
      case (GET):
         get_query(request, bucket);
         break;
      default:
         // Invalid mode
         return (-2);
   }
   return (0);
}

int InitManager::put_query(int request, union query_data *bucket)
   // Change initialization data.
{
   switch (request) {
      case (ANIMATIONS):
         mc->animations = bucket->ints[0];
         break;
      case (BDIHML):
         cerr << "Unable to change BDIHML from app." << endl;
         break;
      case (IM_BOUNDING_BOX):
         bcopy(&(bucket->floats[0]), &(mc->BoundingBox),
                                                sizeof(mc->BoundingBox));
         break;
      case (CHANNEL):
         mc->channel = bucket->ints[0];
         break;
      case (CLIP_PLANES):
         bcopy(&(bucket->floats[0]), &(mc->ClipPlanes),
                                                sizeof(mc->ClipPlanes));
         break;
      case (IM_CONTROL):
         mc->control = bucket->ints[0];
         break;
      case (CPU_LOCK):
         cout << "CPU_LOCK cannot be changed with query(PUT).\n";
         break;
      case (CURSOR_FILE):
         strncpy (mc->cursor_file, bucket->string, STRING128);
         break;
      case (DB_PAGING):
         cout << "DB_PAGING cannot be changed with query(PUT).\n";
         break;
      case (DB_OFFSET):
         mc->db_offset = bucket->ints[0];
         break;
      case (DB_PAGES_ALLOWED):
         mc->db_pages_allowed = bucket->ints[0];
         break;
      case (DB_TSG_FILE):
         cout << "DB_TSG_FILE cannot be changed with query(PUT).\n";
         break;
      case (DIS_EXERCISE):
         cout << "DIS_EXERCISE cannot be changed with query(PUT).\n";
         break;
      case (DIS_ENUM):
         cout << "DIS_ENUM cannot be changed with query(PUT).\n";
         break;
      case (DIS_HEARTBEAT):
         cout << "DIS_HEARTBEAT cannot be changed with query(PUT).\n";
         break;
      case (DIS_POSDELTA):
         cout << "DIS_POSDELTA cannot be changed with query(PUT).\n";
         break;
      case (DIS_ROTDELTA):
         cout << "DIS_ROTDELTA cannot be changed with query(PUT).\n";
         break;
      case (DIS_TIME_OUT):
         mc->DIS.timeout = bucket->floats[0];
         break;
      case (DUMP_FILE):
         strncpy(mc->dump_file, bucket->string, STRING128);
         break;
      case (DVW_PATHS):
         strncpy(mc->dvw_file, bucket->string, STRING128);
         break;
      case (DYNAMIC_OBJS):
         mc->dynamic_objs = bucket->ints[0];
         break;
      case (EARTH_SKY_MODE):
         mc->es_mode = bucket->ints[0];
         break;
      case (ETHER_INTERF):
         strncpy(mc->ether_interf, bucket->string, STRING32);
         break;
      case (FCS_CHECK):
         mc->fcs.check = bucket->ints[0];
         break;
      case (FCS_MP):
         cout << "FCS_MP cannot be changed with query(PUT).\n";
         break;
      case (FCS_PROMPT):
         cout << "FCS_PROMPT cannot be changed with query(PUT).\n";
         break;
      case (FLAT_WORLD):
         cout << "FLAT_WORLD cannot be changed with query(PUT).\n";
         break;
      case (FLYBOX):
         mc->flybox = bucket->ints[0];
         break;
      case (FOG):
         mc->fog = bucket->ints[0];
         break;
      case (FOG_NEAR):
         mc->fog_near = bucket->floats[0];
         break;
      case (FOG_FAR):
         mc->fog_far = bucket->floats[0];
         break;
      case (FOG_TYPE):
         mc->fog_type = bucket->ints[0];
         break;
      case (FORCE):
         mc->force = bucket->ints[0];
         break;
      case (FOV):
         bcopy(&(bucket->floats[0]), &(mc->FoV), sizeof(mc->FoV));
         break;
      case (IM_FRAME_RATE):
         cout << "FRAME_RATE cannot be changed with query(PUT).\n";
         break;
      case (FS_CONFIG):
         strncpy(mc->fs_config, bucket->string, STRING128);
         break;
      case (FS_PORT):
         strncpy(mc->fs_port, bucket->string, STRING32);
         break;
      case (GROUND_HEIGHT):
         mc->ground_height = bucket->floats[0];
         break;
      case (HIRES):
         mc->hires = bucket->ints[0];
         break;
      case (HMD):
      case (HEAD_MOUNTED_DISPLAY):
         mc->hmd = bucket->ints[0];
         break;
      case (HORIZON_ANGLE):
         mc->horizon_angle = bucket->floats[0];
         break;
      case (LEATHER_FLYBOX_CHECK):
         mc->leather_flybox_check = bucket->ints[0];
         break;
      case (MARKING):
         strcpy(mc->marking, bucket->string);
         break;
      case (MASTER):
         strncpy(mc->master, bucket->string, STRING64);
         break;
      case (MODSAF_MAP):
         strncpy(mc->modsaf_map, bucket->string, STRING32);
         break;
      case (MULTICAST):
         mc->multicast.enable = bucket->ints[0];
         break;
      case (MULTICAST_GROUP):
         strncpy(mc->multicast.group, bucket->string, STRING32);
         break;
      case (MULTICAST_TTL):
         mc->multicast.ttl = bucket->ints[0];
         break;
      case (MULTISAMPLE):
         mc->multisample = bucket->ints[0];
         break;
      case (NETWORKING):
         mc->networking = bucket->ints[0];
         break;
      case (NOTIFY_LEVEL):
         mc->notify_level = bucket->ints[0];
         break;
      case (NPS_PIC_DIR):
         cout << "NPS_PIC_DIR cannot be changed with query(PUT).\n";
         break;
      case (ODT_MP):
         cout << "ODT_MP cannot be changed with query(PUT).\n";
         break;
      case (ODT_PORT):
         strncpy(mc->odt_port, bucket->string, STRING32);
         break;
      case (PANEL_GROUP):
         strncpy(mc->panel.group, bucket->string, STRING32);
         break;
      case (PANEL_TTL):
         mc->panel.ttl = bucket->ints[0];
         break;
      case (PANEL_PORT):
         mc->panel.port = bucket->ints[0];
         break;
      case (PDUPOSITION):
         cout << "PDUPOSITION cannot be changed with query(PUT).\n";
         break;
      case (PHASE):
         cout << "PHASE cannot be changed with query(PUT).\n";
         break;
      case (PICTURES):
         cout << "PICTURES cannot be changed with query(PUT).\n";
         break;
      case (PIPE):
         mc->pipe = bucket->ints[0];
         break;
      case (IM_PO_DATABASE):
         cout << "PO_DATABASE cannot be changed with query(PUT).\n";
         break;
      case (IM_PO_EXERCISE):
         cout << "PO_EXERCISE cannot be changed with query(PUT).\n";
         break;
      case (IM_PO_PORT):
         cout << "PO_PORT cannot be changed with query(PUT).\n";
         break;
      case (IM_PORT):
         mc->port = bucket->ints[0];
         break;
      case (PROCESS_MODE):
         cout << "PROCESS_MODE cannot be changed with query(PUT).\n";
         break;
      case (REMOTE_INPUT_PANEL):
         mc->remote_input_panel = bucket->ints[0];
         break;
      case (SCORING):
         cout << "SCORING cannot be changed with query(PUT).\n";
         break;
      case (SEARCH_PATH):
         strncpy(mc->search_path, bucket->string, MAX_SEARCH_PATH);
         break;
      case (IM_SOUND):
         cout << "SOUND cannot be changed with query(PUT).\n";
         break;
      case (STATIC_OBJS):
         mc->static_objs = bucket->ints[0];
         break;
      case (STEALTH):
         cout << "STEALTH cannot be changed with query(PUT).\n";
         break;
      case (STEREO):
         mc->stereo = bucket->ints[0];
         break;
      case (VEHICLE_DRIVEN):
         strncpy(mc->vehicle_driven, bucket->string, STRING32);
         break;
      case (VIEWPORT):
         bcopy(&(bucket->floats[0]), &(mc->ViewPort), sizeof(mc->ViewPort));
         break;
      case (VIEW_HPR):
         bcopy(&(bucket->floats[0]), &(mc->ViewHPR), sizeof(mc->ViewHPR));
         break;
      case (VIEW_XYZ):
         bcopy(&(bucket->floats[0]), &(mc->ViewXYZ), sizeof(mc->ViewXYZ));
         break;
      case (VIEW):
         mc->view = bucket->ints[0];
         break;
      case (WAYPOINTS):
         cout << "WAYPOINTS cannot be changed with query(PUT).\n";
         break;
      case (IM_WEAPON_VIEW):
         cout << "WEAPON_VIEW cannot be changed with query(PUT).\n";
         break;
      case (IM_WINDOW):
         mc->window.type = bucket->ints[0];
         break;
      case (WINDOW_SIZE):
         mc->window.xmin = bucket->floats[0];
         mc->window.xmax = bucket->floats[1];
         mc->window.ymin = bucket->floats[2];
         mc->window.ymax = bucket->floats[3];
         break;
      case (AUTO_FILE):
      case (ANIMATION_FILE):
      case (BLINKING_LIGHTS_FILE):
      case (ENVIRONMENTAL):
      case (ENTITY_FILE):
      case (FILTER_FILE):
      case (HUD_FONT_FILE):
      case (IDENTIFY_FILE):
      case (INTRO_FONT_FILE):
      case (IM_JACK):
      case (LANDMARK_FILE):
      case (LOCATION_FILE):
      case (LOD_FADE):
      case (LOD_SCALE):
      case (LW_RECEIVE_PORT):
      case (LW_SEND_PORT):
      case (LW_MACHINE_NAME):
      case (MODEL_FILE):
      case (NETWORK_FILE):
      case (PO_CTDB_FILE):
      case (ROAD_FILE):
      case (ROUND_WORLD_FILE):
      case (SOUND_FILE):
      case (IM_TERRAIN_FILE):
      case (TRAKER_FILE):
      case (TRANSPORT_FILE):
         cout << "Input files cannot be changed with query(PUT).\n";
         break;
      case (FCS_PORT):
      case (FLYBOX_PORT):
      case (LEATHER_FLYBOX_PORT):
      case (TESTPORT):
      case (TRAKER_PORT):
      case (TREADPORT):
      case (UNIPORT):
         cout << "Ports cannot be changed with query(PUT).\n";
         break;
      default:
         cout << "Invalid request\n";
         return (-2);
   }
   return (0);
}


int InitManager::get_query(int request, union query_data *bucket)
   // Requested information
{
   switch (request) {
      case (ANIMATIONS):
         bucket->ints[0] = mc->animations;
         break;
      case (ANIMATION_FILE):
         strncpy(bucket->string, mc->animation_file, IM_MAX_STRING_LEN);
         break;
      case (AUTO_FILE):
         strncpy(bucket->string, mc->auto_file, IM_MAX_STRING_LEN);
         break;
      case (AUTO_STATE):
         bucket->ints[0] = mc->auto_state;
         break;
      case (BDIHML):
         bucket->ints[0] = mc->bdihml;
         break;
      case (DIGUY_SPEED_RATIO):
         bucket->floats[0] = mc->diguy_info.speed_ratio;
         break;
      case (DIGUY_POSITION_ERROR):
         bucket->floats[0] = mc->diguy_info.position_error;
         break;
      case (DIGUY_TIME_CONSTANT):
         bucket->floats[0] = mc->diguy_info.time_constant;
         break;
      case (DIGUY_TRANS_SMOOTHNESS):
         bucket->ints[0] = mc->diguy_info.trans_smoothness;
         break;
      case (DIGUY_POSITION_MODE):
         bucket->ints[0] = mc->diguy_info.position_mode;
         break;
      case (DIGUY_POSITION_MECHANISM):
         bucket->ints[0] = mc->diguy_info.position_mechanism;
         break;
      case (BLINKING_LIGHTS_FILE):
         strncpy(bucket->string, mc->blinking_lights_file, IM_MAX_STRING_LEN);
         break;
      case (IM_BOUNDING_BOX):
         bcopy(&(mc->BoundingBox), &(bucket->floats[0]),
                                                sizeof(mc->BoundingBox));
         break;
      case (CHANNEL):
         bucket->ints[0] = mc->channel;
         break;
      case (CLIP_PLANES):
         bcopy(&(mc->ClipPlanes), &(bucket->floats[0]),
                                                sizeof(mc->ClipPlanes));
         break;
      case (IM_CONTROL):
         bucket->ints[0] = mc->control;
         break;
      case (CPU_LOCK):
         bucket->ints[0] = mc->cpu_lock;
         break;
      case (CURSOR_FILE):
         strncpy (bucket->string, mc->cursor_file, STRING128);
         break;
      case (DB_PAGING):
         bucket->ints[0] = mc->db_paging;
         break;
      case (DB_OFFSET):
         bucket->ints[0] = mc->db_offset;
         break;
      case (DB_PAGES_ALLOWED):
         bucket->ints[0] = mc->db_pages_allowed;
         break;
      case (DB_TSG_FILE):
         strncpy(bucket->string, mc->db_tsg_file, IM_MAX_STRING_LEN);
         break;
      case (DEAD_SWITCH):
         bucket->ints[0] = mc->dead_switch;
         break;
      case (DIS_EXERCISE):
         bucket->ints[0] = mc->DIS.exercise;
         break;
      case (DIS_HEARTBEAT):
         bucket->floats[0] = mc->DIS.heartbeat;
         break;
      case (DIS_POSDELTA):
         bucket->floats[0] = mc->DIS.posdelta;
         break;
      case (DIS_ROTDELTA):
         bucket->floats[0] = mc->DIS.rotdelta;
         break;
      case (DIS_TIME_OUT):
         bucket->floats[0] = mc->DIS.timeout;
         break;
      case (DIS_ENUM):
         bucket->ints[0] = mc->dis_enum;
         break;
      case (DUMP_FILE):
         strncpy(bucket->string, mc->dump_file, IM_MAX_STRING_LEN);
         break;
      case (DVW_PATHS):
         strncpy(bucket->string, mc->dvw_file, IM_MAX_STRING_LEN);
         break;
      case (DYNAMIC_OBJS):
         bucket->ints[0] = mc->dynamic_objs;
         break;
      case (EARTH_SKY_MODE):
         bucket->ints[0] = mc->es_mode;
         break;
      case (ENVIRONMENTAL):
         strncpy(bucket->string, mc->environmental_file, IM_MAX_STRING_LEN);
         break;
      case (ENTITY_FILE):
         strncpy(bucket->string, mc->entity_file, IM_MAX_STRING_LEN);
         break;
      case (ETHER_INTERF):
         strncpy(bucket->string, mc->ether_interf, IM_MAX_STRING_LEN);
         break;
      case (FCS_CHECK):
         bucket->ints[0] = mc->fcs.check;
         break;
      case (FCS_MP):
         bucket->ints[0] = mc->fcs.mp;
         break;
      case (FCS_PORT):
         strncpy(bucket->string, mc->fcs.port, IM_MAX_STRING_LEN);
         break;
      case (FCS_PROMPT):
         bucket->ints[0] = mc->fcs.prompt;
         break;
      case (FLAT_WORLD):
         bucket->ints[0] = mc->flat_world;
         break;
      case (FILTER_FILE):
         strncpy(bucket->string, mc->filter_file, IM_MAX_STRING_LEN);
         break;
      case (FLYBOX):
         bucket->ints[0] = mc->flybox;
         break;
      case (FLYBOX_PORT):
         strncpy(bucket->string, mc->flybox_port, IM_MAX_STRING_LEN);
         break;
      case (FOG):
         bucket->ints[0] = mc->fog;
         break;
      case (FOG_NEAR):
         bucket->floats[0] = mc->fog_near;
         break;
      case (FOG_FAR):
         bucket->floats[0] = mc->fog_far;
         break;
      case (FOG_TYPE):
         bucket->ints[0] = mc->fog_type;
         break;
      case (FORCE):
         bucket->ints[0] = mc->force;
         break;
      case (FOV):
         bcopy(&(mc->FoV), &(bucket->floats[0]), sizeof(mc->FoV));
         break;
      case (IM_FRAME_RATE):
         bucket->floats[0] = mc->frame_rate;
         break;
      case (FS_CONFIG):
         strncpy(bucket->string, mc->fs_config, IM_MAX_STRING_LEN);
         break;
      case (FS_PORT):
         strncpy(bucket->string, mc->fs_port, IM_MAX_STRING_LEN);
         break;
      case (FS_SCALE):
         bucket->floats[0] = mc->fs_scale;
         break;
      case (GROUND_HEIGHT):
         bucket->floats[0] = mc->ground_height;
         break;
      case (HIRES):
         bucket->ints[0] = mc->hires;
         break;
      case (HMD):
      case (HEAD_MOUNTED_DISPLAY):
         bucket->ints[0] = mc->hmd;
         break;
      case (HMD_FILE):
         strncpy(bucket->string, mc->hmd_file, IM_MAX_STRING_LEN);
         break;
      case (HORIZON_ANGLE):
         bucket->floats[0] = mc->horizon_angle;
         break;
      case (HUD_FONT_FILE):
         strncpy(bucket->string, mc->hud_font_file, IM_MAX_STRING_LEN);
         break;
      case (HUD_LEVEL):
         bucket->ints[0] = mc->hud_level;
         break;
      case (IDENTIFY_FILE):
         strncpy(bucket->string, mc->identify_file, IM_MAX_STRING_LEN);
         break;
      case (INTRO_FONT_FILE):
         strncpy(bucket->string, mc->intro_font_file, IM_MAX_STRING_LEN);
         break;
      case (IM_JACK):
         bucket->ints[0] = mc->jack;
         break;
      case (LANDMARK_FILE):
         strncpy(bucket->string, mc->landmark_file, IM_MAX_STRING_LEN);
         break;
      case (LEATHER_FLYBOX_CHECK):
         bucket->ints[0] = mc->leather_flybox_check;
         break;
      case (LEATHER_FLYBOX_PORT):
         strncpy(bucket->string, mc->leather_flybox_port, IM_MAX_STRING_LEN);
         break;
      case (LOCATION_FILE):
         strncpy(bucket->string, mc->location_file, IM_MAX_STRING_LEN);
         break;
      case (LOD_FADE):
         bucket->floats[0] = mc->lod_fade;
         break;
      case (LOD_SCALE):
         bucket->floats[0] = mc->lod_scale;
         break;
      case (LW_RECEIVE_PORT):
         bucket->ints[0] = mc->lw_rport;
         break;
      case (LW_SEND_PORT):
         bucket->ints[0] = mc->lw_sport;
         break;
      case (LW_MACHINE_NAME):
         strncpy(bucket->string, mc->lw_toip, IM_MAX_STRING_LEN);
         break;
      case (MARKING):
         strncpy(bucket->string, mc->marking, IM_MAX_STRING_LEN);
         break;
      case (MASTER):
         strncpy(bucket->string, mc->master, IM_MAX_STRING_LEN);
         break;
      case (IM_MAX_ALTITUDE):
         bucket->floats[0] = mc->alt.max;
         break;
      case (MODEL_FILE):
         strncpy(bucket->string, mc->model_file, IM_MAX_STRING_LEN);
         break;
      case (MODSAF_MAP):
         strncpy(bucket->string, mc->modsaf_map, IM_MAX_STRING_LEN);
         break;
      case (MULTICAST):
         bucket->ints[0] = mc->multicast.enable;
         break;
      case (MULTICAST_GROUP):
         strncpy(bucket->string, mc->multicast.group, IM_MAX_STRING_LEN);
         break;
      case (MULTICAST_TTL):
         bucket->ints[0] = mc->multicast.ttl;
         break;
      case (MULTISAMPLE):
         bucket->ints[0] = mc->multisample;
         break;
      case (NETWORKING):
         bucket->ints[0] = mc->networking;
         break;
      case (NETWORK_FILE):
         strncpy(bucket->string, mc->network_file, IM_MAX_STRING_LEN);
         break;
      case (NOTIFY_LEVEL):
         bucket->ints[0] = mc->notify_level;
         break;
      case (NPS_PIC_DIR):
         strncpy(bucket->string, mc->nps_pic_dir, IM_MAX_STRING_LEN);
         break;
      case (ODT_MP):
         bucket->ints[0] = mc->odt_mp;
         break;
      case (ODT_PORT):
         strncpy(bucket->string, mc->odt_port, IM_MAX_STRING_LEN);
         break;
      case (PANEL_GROUP):
         strncpy(bucket->string, mc->panel.group, IM_MAX_STRING_LEN);
         break;
      case (PANEL_TTL):
         bucket->ints[0] = mc->panel.ttl;
         break;
      case (PANEL_PORT):
         bucket->ints[0] = mc->panel.port;
         break;
      case (PDUPOSITION):
         bucket->ints[0] = mc->pdu_position;
         break;
      case (PHASE):
         bucket->ints[0] = mc->phase;
         break;
      case (PICTURES):
         bucket->ints[0] = mc->pictures;
         break;
      case (PIPE):
         bucket->ints[0] = mc->pipe;
         break;
      case (PO_CTDB_FILE):
         strncpy(bucket->string, mc->PO.ctdb_file, IM_MAX_STRING_LEN);
         break;
      case (IM_PO_DATABASE):
         bucket->ints[0] = mc->PO.database;
         break;
      case (IM_PO_EXERCISE):
         bucket->ints[0] = mc->PO.exercise;
         break;
      case (IM_PO_PORT):
         bucket->ints[0] = mc->PO.port;
         break;
      case (IM_PORT):
         bucket->ints[0] = mc->port;
         break;
      case (PROCESS_MODE):
         bucket->ints[0] = mc->process_mode;
         break;
      case (REMOTE_INPUT_PANEL):
         bucket->ints[0] = mc->remote_input_panel;
         break;
      case (RADAR_ICON_FILE):
         strncpy(bucket->string, mc->radar_icon_file, IM_MAX_STRING_LEN);
         break;
      case (RANGE_FILTER):
         bucket->floats[0] = mc->range_filter;
         break;
      case (ROAD_FILE):
         strncpy(bucket->string, mc->road_file, IM_MAX_STRING_LEN);
         break;
      case (ROUND_WORLD_FILE):
         strncpy(bucket->string, mc->round_world_file, IM_MAX_STRING_LEN);
         break;
      case (SCORING):
         bucket->ints[0] = mc->scoring;
         break;
      case (SEARCH_PATH):
         strncpy(bucket->string, mc->search_path, MAX_SEARCH_PATH);
         break;
      case (IM_SOUND):
         bucket->ints[0] = mc->sound;
         break;
      case (SOUND_FILE):
         strncpy(bucket->string, mc->sound_file, IM_MAX_STRING_LEN);
         break;
      case (STATIC_OBJS):
         bucket->ints[0] = mc->static_objs;
         break;
      case (STEALTH):
         bucket->ints[0] = mc->stealth;
         break;
      case (STEREO):
         bucket->ints[0] = mc->stereo;
         break;
      case (IM_TERRAIN_FILE):
         strncpy(bucket->string, mc->terrain_file, IM_MAX_STRING_LEN);
         break;
      case (TARGET_BOUNDING_BOX):
         bucket->ints[0] = mc->target_bbox;
         break;
      case (TARGET_ENTITY):
         bucket->ints[0] = mc->target_entity.site;
         bucket->ints[1] = mc->target_entity.host;
         bucket->ints[2] = mc->target_entity.entity;
         break;
      case (TESTPORT):
         strncpy(bucket->string, mc->testport, IM_MAX_STRING_LEN);
         break;
      case (TRAKER_FILE):
         strncpy(bucket->string, mc->traker_file, IM_MAX_STRING_LEN);
         break;
      case (TRAKER_PORT):
         strncpy(bucket->string, mc->traker_port, IM_MAX_STRING_LEN);
         break;
      case (TRANSPORT_FILE):
         strncpy(bucket->string, mc->transport_file, IM_MAX_STRING_LEN);
         break;
      case (TREADPORT):
         strncpy(bucket->string, mc->treadport, IM_MAX_STRING_LEN);
         break;
      case (UNIPORT):
         strncpy(bucket->string, mc->uniport, IM_MAX_STRING_LEN);
         break;
      case (UPPERBODY):
         strncpy(bucket->string, mc->upperbody, IM_MAX_STRING_LEN);
         break;
      case (USE_ROUND_WORLD):
         bucket->ints[0] = mc->use_round;
         break;
      case (VEHICLE_DRIVEN):
         strncpy(bucket->string, mc->vehicle_driven, IM_MAX_STRING_LEN);
         break;
      case (VIEWPORT):
         bcopy(&(mc->ViewPort), &(bucket->floats[0]), sizeof(mc->ViewPort));
         break;
      case (VIEW_HPR):
         bcopy(&(mc->ViewHPR), &(bucket->floats[0]), sizeof(mc->ViewHPR));
         break;
      case (VIEW_XYZ):
         bcopy(&(mc->ViewXYZ), &(bucket->floats[0]), sizeof(mc->ViewXYZ));
         break;
      case (VIEW):
         bucket->ints[0] = mc->view;
         break;
      case (WAYPOINTS):
         bucket->ints[0] = mc->waypoints;
         break;
      case (IM_WEAPON_VIEW):
         bucket->ints[0] = mc->weapon_view;
         break;
      case (IM_WINDOW):
         bucket->ints[0] = mc->window.type;
         break;
      case (WINDOW_SIZE):
         bucket->floats[0] = mc->window.xmin;
         bucket->floats[1] = mc->window.xmax;
         bucket->floats[2] = mc->window.ymin;
         bucket->floats[3] = mc->window.ymax;
         break;
      default:
         cout << "Invalid request\n";
         return (-2);
   }
   return (0);
}


int InitManager::get_blinking_lights(struct BL **lights, int *num_lights)
   // Fill passed-in blinking lights array
{
   if (strcasecmp(mc->blinking_lights_file, "NONE") == 0) {
      *num_lights = 0;
      return (0);
   }

   bzero(lights, sizeof(struct BL) * MAX_BLINKING_LIGHTS);

   *num_lights = Defaults->num_blinking_lights;
   int bytes = (Defaults->num_blinking_lights * sizeof(struct BL));
   bcopy((void*) Defaults->BlinkingLights, (void*) lights, bytes);

   return (1);
}


int InitManager::get_filters(struct FIL **filters, int *num_filters)
   // Fill passed-in filters array
{
   if (strcasecmp(mc->filter_file, "NONE") == 0) {
      *num_filters = 0;
      return (0);
   }

   bzero(filters, sizeof(struct FIL) * MAX_PDU_FILTERS);

   *num_filters = Defaults->num_filters;
   int bytes = (Defaults->num_filters * sizeof(struct FIL));
   bcopy((void*) Defaults->Filters, (void*) filters, bytes);

   return (1);
}


int InitManager::get_landmarks(struct LAND **landmarks, int *num_landmarks)
   // Fill passed-in landmarks array
{
   if (strcasecmp(mc->landmark_file, "NONE") == 0) {
      *num_landmarks = 0;
      return (0);
   }

   bzero(landmarks, sizeof(struct LAND) * MAX_LANDMARKS);

   *num_landmarks = Defaults->num_landmarks;
   int bytes = (Defaults->num_landmarks * sizeof(struct LAND));
   bcopy((void*) Defaults->Landmarks, (void*) landmarks, bytes);

   return (1);
}


int InitManager::get_hosts(struct Host **hosts, int *num_hosts)
   // Fill passed-in hosts array
{
   if (strcasecmp(mc->network_file, "NONE") == 0) {
      *num_hosts = 0;
      return (0);
   }

   bzero(hosts, sizeof(struct Host) * MAX_HOSTS);

   *num_hosts = Defaults->num_hosts;
   int bytes = (Defaults->num_hosts * sizeof(struct Host));
   bcopy((void*) Defaults->Hosts, (void*) hosts, bytes);

   return (1);
}


int InitManager::get_round_world_data(struct RW *rw)
{
   if (strcasecmp(mc->round_world_file, "NONE") == 0)
      return (0);

   bcopy((void*) &(Defaults->RoundWorld), (void*) rw, sizeof(struct RW));

   return (1);
}


void InitManager::get_transport_locs(struct TP **locations, int *num_locations)
   // Fill passed-in transport locations array
{
   bzero(locations, sizeof(struct TP) * MAX_TRANSPORT);
   *num_locations = Defaults->num_transport_locs;
   int bytes = (Defaults->num_transport_locs * sizeof(struct TP));
   bcopy((void*) Defaults->TransportLocs, (void*) locations, bytes);
}


void InitManager::get_radar_icons(struct RI **icons, int *num_icons)
   // Fill passed-in radar icons array
{
   bzero(icons, sizeof(struct RI) * MAX_RADAR_DIVS);
   *num_icons = Defaults->num_radar_icons;
   int bytes = (Defaults->num_radar_icons * sizeof(struct RI));
   bcopy((void*) Defaults->RadarIcons, (void*) icons, bytes);
}


int InitManager::get_sounds(struct Sound **sounds)
   // Fill passed-in sounds array
{
   if (strcasecmp(mc->sound_file, "NONE") == 0)
      return (0);

   int bytes = (MAX_SOUND_EVENTS * sizeof(struct Sound));
   bcopy((void*) Defaults->Sounds, (void*) sounds, bytes);

   return (1);
}


int InitManager::get_terrain_masks(struct TM **masks, int *num_masks)
   // Fill passed-in terrain masks array
{
   if (strcasecmp(mc->terrain_file, "NONE") == 0) {
      *num_masks = 0;
      return (0);
   }

   bzero(masks, sizeof(struct TM) * MAX_MASKS);

   *num_masks = Defaults->num_masks;
   int bytes = (Defaults->num_masks * sizeof(struct TM));
   bcopy((void*) Defaults->Masks, (void*) masks, bytes);

   return (1);
}


int InitManager::get_animations(struct Anim **animations)
   // Fill passed-in animations array
{
   if (strcasecmp(mc->animation_file, "NONE") == 0)
      return (0);

   int bytes = (MAX_ANIMATIONS * sizeof(struct Anim));
   bcopy((void*) Defaults->Animations, (void*) animations, bytes);

   return (1);
}


int InitManager::get_models(struct Model **models, int *num_models)
   // Fill passed-in Location File static models array
{
   if (strcasecmp(mc->location_file, "NONE") == 0) {
      *num_models = 0;
      return (0);
   }

   *models = Defaults->SObjects;
   *num_models = Defaults->num_static_objs;
/*
   bzero(models, sizeof(struct Model) * MAX_STATIC_OBJ_TYPES);

cerr << "Size of Model Array: " << sizeof(struct Model) * MAX_STATIC_OBJ_TYPES
     << endl;

   *num_models = Defaults->num_static_objs;
   int bytes = (Defaults->num_static_objs * sizeof(struct Model));
   bcopy((void*) Defaults->SObjects, (void*) models, bytes);
*/
   return (1);
}


void InitManager::get_dynamic_objs(struct Dyn **models, int *num_objs)
   // Fill passed-in Model File static models array
{
   bzero(models, sizeof(struct Dyn) * MAX_VEHTYPES);

   *num_objs = Defaults->num_dynamic_objs;
   int bytes = (Defaults->num_dynamic_objs * sizeof(struct Dyn));
   bcopy((void*) Defaults->DObjects, (void*) models, bytes);
}


// EOF
