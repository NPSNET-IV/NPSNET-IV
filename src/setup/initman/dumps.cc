// dumps.cc

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <Performer/pf.h>
#include <string.h>
#include <time.h>
#include "im.tab.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "imextern.h"
#include "imclass.h"
#include "pdu.h"

// kludge
#define PFSEQ_CYCLE     0
#define PFSEQ_SWING     1

extern struct defs *Defaults;
extern struct MC *mc;

void InitManager::dump_config_vals()
   // Print all Master Config commands and values.
{
   time_t t = time(NULL);
   dumpfile << "Dump File Created: " << asctime(localtime(&t)) << endl;

   strcpy(name_array, "Master Configuration vals:");
   dump_heading(name_array);

   mc->animations ? dumpfile << "ANIMATIONS=ON\n":
                    dumpfile << "ANIMATIONS=OFF\n";
   dumpfile << "ANIMATION_FILE=" << mc->animation_file << endl;
   dumpfile << "AUTO_FILE=" << mc->auto_file << endl;
   mc->auto_state ? dumpfile << "AUTO_STATE=ON\n":
                    dumpfile << "AUTO_STATE=OFF\n";
   mc->bdihml ? dumpfile << "BDIHML=ON\n":
                dumpfile << "BDIHML=OFF\n";
   dumpfile << "BLINKING_LIGHTS_FILE=" << mc->blinking_lights_file << endl;
   dumpfile << "BOUNDING_BOX xmin="
                   << mc->BoundingBox.xmin <<
          " xmax=" << mc->BoundingBox.xmax <<
          " ymin=" << mc->BoundingBox.ymin <<
          " ymax=" << mc->BoundingBox.ymax << endl;
   dumpfile << "CHANNEL=" << mc->channel << endl;
   dumpfile << "CLIP_PLANES near=" << mc->ClipPlanes.cp_near <<
                       " far=" << mc->ClipPlanes.cp_far << endl;
   dumpfile << "CONTROL=";
   switch (mc->control) {
      case (FCS): dumpfile << "FCS"; break;
      case (FLYBOXIN): dumpfile << "FLYBOXIN"; break;
      case (LEATHER_FLYBOX): dumpfile << "LEATHER_FLYBOX"; break;
      case (KAFLIGHT): dumpfile << "KAFLIGHT"; break;
      case (KEYBOARD): case (PAD): dumpfile << "KEYBOARD"; break;
      case (SPACEBALL): case (BALL): case (DRIVE):
                dumpfile << "SPACEBALL"; break;
      case (UNIPORT): dumpfile << "UNIPORT"; break;
      case (SHIP_PANEL): dumpfile << "SHIP_PANEL"; break;
      case (SUB_PANEL): dumpfile << "SUB_PANEL"; break;
      case (AUSA_ODT): dumpfile << "AUSA_ODT"; break;
      default: dumpfile << "<bad value>";
   }
   dumpfile << endl;
   mc->cpu_lock ? dumpfile << "CPU_LOCK=ON\n":
                  dumpfile << "CPU_LOCK=OFF\n";
   dumpfile << "CURSOR_FILE=" << mc->cursor_file << endl;
   mc->db_paging ? dumpfile << "DB_PAGING=ON\n":
                   dumpfile << "DB_PAGING=OFF\n";
   dumpfile << "DB_OFFSET=" << mc->db_offset << endl;
   dumpfile << "DB_TSG_FILE=" << mc->db_tsg_file << endl;
   dumpfile << "DB_PAGES_ALLOWED=" << mc->db_pages_allowed << endl;
   mc->dead_switch ? dumpfile << "DEAD_SWITCH=ON\n":
                     dumpfile << "DEAD_SWITCH=OFF\n";
   dumpfile << "DIGUY_SPEED_RATIO=" << mc->diguy_info.speed_ratio << endl;
   dumpfile << "DIGUY_POSITION_ERROR=" << mc->diguy_info.position_error << endl;
   dumpfile << "DIGUY_TIME_CONSTANT=" << mc->diguy_info.time_constant << endl;
   dumpfile << "DIGUY_TRANS_SMOOTHNESS=";
   switch (mc->diguy_info.trans_smoothness) {
      case (TRANS_IMMEDIATE): dumpfile << "TRANS_IMMEDIATE"; break;
      case (IMMEDIATE_TRANS): dumpfile << "IMMEDIATE_TRANS"; break;
      case (TRANS_SMOOTHEST): dumpfile << "TRANS_SMOOTHEST"; break;
      default: dumpfile << "bad value>";
   }
   dumpfile << endl;
   dumpfile << "DIGUY_POSITION_MODE=";
   switch (mc->diguy_info.position_mode) {
      case (SMOOTH_MODE): dumpfile << "SMOOTH_MODE"; break;
      case (PRECISE_MODE): dumpfile << "PRECISE_MODE"; break;
      default: dumpfile << "<bad value>";
   }
   dumpfile << endl;
   dumpfile << "DIGUY_POSITION_MECHANISM=";
   switch (mc->diguy_info.position_mechanism) {
      case (DIGUY_INTERNAL): dumpfile << "DIGUY_INTERNAL"; break;
      case (NPSNET_INTERNAL): dumpfile << "NPSNET_INTERNAL"; break;
      default: dumpfile << "<bad value>";
   }
   dumpfile << endl;
   dumpfile << "DIS_EXERCISE=" << mc->DIS.exercise << endl;
   dumpfile << "DIS_HEARTBEAT=" << mc->DIS.heartbeat << endl;
   dumpfile << "DIS_POSDELTA=" << mc->DIS.posdelta << endl;
   dumpfile << "DIS_ROTDELTA=" << mc->DIS.rotdelta << endl;
   dumpfile << "DIS_TIME_OUT=" << mc->DIS.timeout << endl;
   dumpfile << "DIS_ENUM=";
      switch (mc->dis_enum) {
         case (DISv203):
            dumpfile << "DISv203";
            break;
         case (DISv204):
            dumpfile << "DISv204";
            break;
         default:
            dumpfile << "UNKNOWN";
            break;
      }
   dumpfile << endl;
   dumpfile << "DUMP_FILE=" << mc->dump_file << endl;
   dumpfile << "DVW_PATHS=" << mc->dvw_file << endl;
   mc->dynamic_objs ?
      dumpfile << "DYNAMIC_OBJS=ON\n":
      dumpfile << "DYNAMIC_OBJS=OFF\n";
   dumpfile << "EARTH_SKY_MODE=";
   switch (mc->es_mode) {
      case (FAST): dumpfile << "FAST"; break;
      case (TAG): dumpfile << "TAG"; break;
      case (SKY): dumpfile << "SKY"; break;
      case (SKY_GROUND): dumpfile << "SKY_GROUND"; break;
      case (SKY_CLEAR): dumpfile << "SKY_CLEAR"; break;
      default: dumpfile << "<bad value>";
   }
   dumpfile << endl;
   dumpfile << "ENVIRONMENTAL=" << mc->environmental_file << endl;
   dumpfile << "ENTITY_FILE=" << mc->entity_file << endl;
   dumpfile << "ETHER_INTERF=" << mc->ether_interf << endl;
   mc->fcs.check ?
      dumpfile << "FCS_CHECK=ON\n":
      dumpfile << "FCS_CHECK=OFF\n";
   mc->fcs.mp ? dumpfile << "FCS_MP=ON\n":
                dumpfile << "FCS_MP=OFF\n";
   dumpfile << "FCS_PORT=" << mc->fcs.port << endl;
   mc->fcs.prompt ? dumpfile << "FCS_PROMPT=ON\n":
                    dumpfile << "FCS_PROMPT=OFF\n";
   dumpfile << "FIELD_OF_VIEW x=" << mc->FoV.x <<
                        " y=" << mc->FoV.y << endl;
   dumpfile << "FILTER_FILE=" << mc->filter_file << endl;
   mc->flat_world ? dumpfile << "FLAT_WORLD=ON" << endl :
                    dumpfile << "FLAT_WORLD=OFF" << endl;
   mc->flybox ?
      dumpfile << "FLYBOX=ON\n":
      dumpfile << "FLYBOX=OFF\n";
   dumpfile << "FLYBOX_PORT=" << mc->flybox_port << endl;
   mc->fog ? dumpfile << "FOG=ON\n" : dumpfile << "FOG=OFF\n";
   dumpfile << "FOG_NEAR=" << mc->fog_near << endl;
   dumpfile << "FOG_FAR=" << mc->fog_far << endl;
   dumpfile << "FOG_TYPE=";
        switch (mc->fog_type) {
           case (FOG_VTX_LIN): dumpfile << "FOG_VTX_LIN"; break;
           case (FOG_VTX_EXP): dumpfile << "FOG_VTX_EXP"; break;
           case (FOG_VTX_EXP2): dumpfile << "FOG_VTX_EXP2"; break;
           case (FOG_PIX_LIN): dumpfile << "FOG_PIX_LIN"; break;
           case (FOG_PIX_EXP): dumpfile << "FOG_PIX_EXP"; break;
           case (FOG_PIX_EXP2): dumpfile << "FOG_PIX_EXP2"; break;
           case (FOG_PIX_SPLINE): dumpfile << "FOG_PIX_SPLINE"; break;
           default: dumpfile << "<bad value>"; break;
        }
   dumpfile << endl;
   dumpfile << "FORCE=";
   switch (mc->force) {
      case (ForceID_Blue): dumpfile << "BLUE"; break;
      case (ForceID_Red): dumpfile << "RED"; break;
      case (ForceID_White): dumpfile << "WHITE"; break;
      case (ForceID_Other): dumpfile << "OTHER"; break;
      default: dumpfile << "<bad value>";
   }
   dumpfile << endl;
   dumpfile << "FRAME_RATE=" << mc->frame_rate << endl;
   dumpfile << "FS_CONFIG=" << mc->fs_config << endl;
   dumpfile << "FS_PORT=" << mc->fs_port << endl;
   dumpfile << "FS_SCALE=" << mc->fs_scale << endl;
   dumpfile << "GROUND_HEIGHT=" << mc->ground_height << endl;
   dumpfile << "HIRES=";
   switch (mc->hires) {
      case (IM_NET_OFF): dumpfile << "NET_OFF"; break;
      case (RECEIVE_ONLY): dumpfile << "RECEIVE_ONLY"; break;
      case (SEND_ONLY): dumpfile << "SEND_ONLY"; break;
      case (SEND_AND_RECEIVE): dumpfile << "SEND_AND_RECEIVE"; break;
   }
   dumpfile << endl;
         
   mc->hmd ? dumpfile << "HMD=ON\n" : dumpfile << "HMD=OFF\n";
   dumpfile << "HMD_FILE=" << mc->hmd_file << endl;
   dumpfile << "HORIZON_ANGLE=" << mc->horizon_angle << endl;
   dumpfile << "HUD_FONT_FILE=" << mc->hud_font_file << endl;
   mc->hud_level ? dumpfile << "HUD_LEVEL=ON\n" : dumpfile << "HUD_LEVEL=OFF\n";
   dumpfile << "IDENTIFY_FILE=" << mc->identify_file << endl;
   dumpfile << "INTRO_FONT_FILE=" << mc->intro_font_file << endl;
   mc->jack ?
      dumpfile << "JACK=ON" << endl:
      dumpfile << "JACK=OFF" << endl;
   dumpfile << "LANDMARK_FILE=" << mc->landmark_file << endl;
   mc->leather_flybox_check ?
      dumpfile << "LEATHER_FLYBOX_CHECK=ON\n":
      dumpfile << "LEATHER_FLYBOX_CHECK=OFF\n";
   dumpfile << "LEATHER_FLYBOX_PORT=" << mc->leather_flybox_port << endl;
   dumpfile << "LOCATION_FILE=" << mc->location_file << endl;
   dumpfile << "LOD_FADE=" << mc->lod_fade << endl;
   dumpfile << "LOD_SCALE=" << mc->lod_scale << endl;
   dumpfile << "LW_RECEIVE_PORT=" << mc->lw_rport << endl;
   dumpfile << "LW_SEND_PORT=" << mc->lw_sport << endl;
   dumpfile << "LW_MACHINE_NAME=" << mc->lw_toip << endl;
   dumpfile << "MARKING=" << mc->marking << endl;
   dumpfile << "MASTER=" << mc->master << endl;
   dumpfile << "MAX_ALTITUDE=" << mc->alt.max << endl;
   dumpfile << "MODEL_FILE=" << mc->model_file << endl;
   dumpfile << "MODSAF_MAP=" << mc->modsaf_map << endl;
   mc->multicast.enable ? dumpfile << "MULTICAST=ON\n":
                          dumpfile << "MULTICAST=OFF\n";
   dumpfile << "MULTICAST_GROUP=" << mc->multicast.group << endl;
   dumpfile << "MULTICAST_TTL=" << mc->multicast.ttl << endl;
   mc->multisample ? dumpfile << "MULTISAMPLE=ON\n":
                     dumpfile << "MULTISAMPLE=OFF\n";
   mc->networking ?
      dumpfile << "NETWORKING=ON\n":
      dumpfile << "NETWORKING=OFF\n";
   dumpfile << "NETWORK_FILE=" << mc->network_file << endl;
   dumpfile << "NOTIFY_LEVEL=";
   switch (mc->notify_level) {
      case (IM_FATAL): dumpfile << "FATAL"; break;
      case (WARN): dumpfile << "WARN"; break;
      case (NOTICE): dumpfile << "NOTICE"; break;
      case (INFO): dumpfile << "INFO"; break;
      case (IM_DEBUG): dumpfile << "DEBUG"; break;
      case (FPDEBUG): dumpfile << "FPDEBUG"; break;
      default: dumpfile << "<bad value>";
   }
   dumpfile << endl;
   dumpfile << "NPS_PIC_DIR=" << mc->nps_pic_dir << endl;
   mc->pdu_position ? dumpfile << "PDUPOSITION=ON\n":
                      dumpfile << "PDUPOSITION=OFF\n";
   dumpfile << "ODT_PORT=" << mc->odt_port << endl;
   mc->odt_mp ? dumpfile << "ODT_MP=ON\n":
                dumpfile << "ODT_MP=OFF\n";
   dumpfile << "PANEL_GROUP=" << mc->panel.group << endl;
   dumpfile << "PANEL_TTL=" << mc->panel.ttl << endl;
   dumpfile << "PANEL_PORT=" << mc->panel.port << endl;
   dumpfile << "PHASE=";
   switch (mc->phase) {
      case (ALG_FREE): dumpfile << "FREE"; break;
      case (ALG_FLOAT): dumpfile << "FLOAT"; break;
      case (ALG_LOCK): dumpfile << "LOCK"; break;
      default: dumpfile << "<bad value>";
   }
   dumpfile << endl;
   mc->pictures ? dumpfile << "PICTURES=ON\n":
                  dumpfile << "PICTURES=OFF\n";
   dumpfile << "PIPE=" << mc->pipe << endl;
   dumpfile << "PO_CTDB_FILE=" << mc->PO.ctdb_file << endl;
   dumpfile << "PO_DATABASE=" << mc->PO.database << endl;
   dumpfile << "PO_EXERCISE=" << mc->PO.exercise << endl;
   dumpfile << "PO_PORT=" << mc->PO.port << endl;
   dumpfile << "PORT=" << mc->port << endl;
   dumpfile << "PROCESS_MODE=";
   switch (mc->process_mode) {
      case (APPCULLDRAW): dumpfile << "APPCULLDRAW"; break;
      case (APP_CULLDRAW): dumpfile << "APP_CULLDRAW"; break;
      case (APPCULL_DRAW): dumpfile << "APPCULL_DRAW"; break;
      case (APP_CULL_DRAW): dumpfile << "APP_CULL_DRAW"; break;
      case (IM_DEFAULT): dumpfile << "DEFAULT"; break;
      default: dumpfile << "<bad value>";
   }
   dumpfile << endl;
   dumpfile << "RADAR_ICON_FILE=" << mc->radar_icon_file << endl;
   dumpfile << "RANGE_FILTER=" << mc->range_filter << endl;
   mc->remote_input_panel ?
      dumpfile << "REMOTE_INPUT_PANEL=ON\n":
      dumpfile << "REMOTE_INPUT_PANEL=OFF\n";
   dumpfile << "ROAD_FILE=" << mc->road_file << endl;
   dumpfile << "ROUND_WORLD_FILE=" << mc->round_world_file << endl;
   mc->scoring ? dumpfile << "SCORING=ON\n":
                 dumpfile << "SCORING=OFF\n";
   dumpfile << "SEARCH_PATH=" << mc->search_path << endl;
   mc->sound ? dumpfile << "SOUND=ON\n":
               dumpfile << "SOUND=OFF\n";
   dumpfile << "SOUND_FILE=" << mc->sound_file << endl;
   mc->static_objs ?
      dumpfile << "STATIC_OBJS=ON\n":
      dumpfile << "STATIC_OBJS=OFF\n";
   mc->stealth ? dumpfile << "STEALTH=ON\n":
                 dumpfile << "STEALTH=OFF\n";
   mc->stereo ? dumpfile << "STEREO=ON\n":
                dumpfile << "STEREO=OFF\n";
   dumpfile << "TARGET_BOUNDING_BOX";
   mc->target_bbox ?  dumpfile << "TARGET_BOUNDING_BOX=ON\n":
                      dumpfile << "TARGET_BOUNDING_BOX=OFF\n";
   dumpfile << "TARGET_ENTITY=" << mc->target_entity.site << ", "
                                << mc->target_entity.host << ", "
                                << mc->target_entity.entity << endl;
   dumpfile << "TERRAIN_FILE=" << mc->terrain_file << endl;
   dumpfile << "TESTPORT=" << mc->testport << endl;
   dumpfile << "TRAKER_FILE=" << mc->traker_file << endl;
   dumpfile << "TRAKER_PORT=" << mc->traker_port << endl;
   dumpfile << "TRANSPORT_FILE=" << mc->transport_file << endl;
   dumpfile << "TREADPORT=" << mc->treadport << endl;
   dumpfile << "UNIPORT=" << mc->uniport << endl;
   dumpfile << "UPPERBODY=" << mc->upperbody << endl;
   mc->use_round ? dumpfile << "USE_ROUND_WORLD=ON\n":
                   dumpfile << "USE_ROUND_WORLD=OFF\n";
   dumpfile << "VEHICLE_DRIVEN=" << mc->vehicle_driven << endl;
   dumpfile << "VIEW=";
   switch (mc->view) {
      case (AIRPORT): dumpfile << "AIRPORT"; break;
      case (CANYON): dumpfile << "CANYON"; break;
      case (VILLAGE): dumpfile << "VILLAGE"; break;
      case (PIER): dumpfile << "PIER"; break;
      case (IM_START): dumpfile << "START"; break;
      default: dumpfile << "<bad value>";
   }
   dumpfile << endl;
   dumpfile << "VIEWPORT xmin="
                   << mc->ViewPort.xmin <<
          " xmax=" << mc->ViewPort.xmax <<
          " ymin=" << mc->ViewPort.ymin <<
          " ymax=" << mc->ViewPort.ymax << endl;
   dumpfile << "VIEW_HPR heading="
                    << mc->ViewHPR.heading <<
          " pitch=" << mc->ViewHPR.pitch <<
          " roll="  << mc->ViewHPR.roll << endl;
   dumpfile << "VIEW_XYZ x=" << mc->ViewXYZ.x <<
                   " y=" << mc->ViewXYZ.y <<
                   " z=" << mc->ViewXYZ.z << endl;
   mc->waypoints ? dumpfile << "WAYPOINTS=ON\n":
                   dumpfile << "WAYPOINTS=OFF\n";
   mc->weapon_view ? dumpfile << "WEAPON_VIEW=ON\n":
                     dumpfile << "WEAPON_VIEW=OFF\n";
   dumpfile << "WINDOW=";
   switch (mc->window.type) {
      case (CLOSED): dumpfile << "CLOSED"; break;
      case (CUSTOM): dumpfile << "CUSTOM"; break;
      case (SVGA): dumpfile << "SVGA"; break;
      case (FULL): dumpfile << "FULL"; break;
      case (NTSC): dumpfile << "NTSC"; break;
      case (PAL): dumpfile << "PAL"; break;
      case (NORMAL): dumpfile << "NORMAL"; break;
      case (VIM): dumpfile << "VIM"; break;
      case (THREE_TV): dumpfile << "THREE_TV"; break;
      case (THREE_BUNKER): dumpfile << "THREE_BUNKER"; break;
      case (TWO_TPORT): dumpfile << "TWO_TPORT"; break;
      case (TWO_ADJACENT): dumpfile << "TWO_ADJACENT"; break;
      case (FS_MCO): dumpfile << "Fakespace Push Boom MCO option"; break;
      case (FS_ST_BUFFERS): dumpfile << "Fakespace Push Boom stereo option"; break;
      default: dumpfile << "<bad value>";
   }
   dumpfile << endl;
   dumpfile << "WINDOW_SIZE xmin="
                   << mc->window.xmin <<
          " xmax=" << mc->window.xmax <<
          " ymin=" << mc->window.ymin <<
          " ymax=" << mc->window.ymax << endl;
   dumpfile << endl;
}


void InitManager::dump_animation_vals()
{
   if (strcasecmp(Defaults->MasterConfig.animation_file, "NONE") == 0) {
      strcpy(name_array, "No Animation File specified.");
      dump_heading(name_array);
      dumpfile << endl;
      return;
   }

   strcpy(name_array, "Animation vals (");
   strcat(name_array, Defaults->MasterConfig.animation_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   dumpfile << "#: identifier count mode reps\n";
   dumpfile << "\tMODEL filename delay\n";
   dumpfile << "\tBB filename delay\n";
   dumpfile << "-----------------------------\n";

   for (int i = 0; i < MAX_ANIMATIONS; i++) {
      if (Defaults->Animations[i].num_effects <= 0) {
         // No effects for this animation index
         continue;
      }

      dumpfile << i << ": ";
      dumpfile << Defaults->Animations[i].identifier << " ";
      dumpfile << Defaults->Animations[i].count << " ";
      switch (Defaults->Animations[i].mode) {
         case (PFSEQ_CYCLE):
            dumpfile << "PFSEQ_CYCLE ";
            break;
         case (PFSEQ_SWING):
            dumpfile << "PFSEQ_SWING ";
            break;
         default:
            dumpfile << "?  ";
            break;
      }
      dumpfile << Defaults->Animations[i].reps << endl;

      for (int j = 0; j < Defaults->Animations[i].num_effects; j++) {
         dumpfile << "\t";
         switch (Defaults->Animations[i].Effect[j].effect_type) {
            case (TYPE_MODEL):
               dumpfile << "MODEL ";
               break;
            case (TYPE_BB):
               dumpfile << "BB ";
               break;
            default:
               dumpfile << "?  ";
               break;
         }
         dumpfile << Defaults->Animations[i].Effect[j].filename;
         dumpfile << " "  << Defaults->Animations[i].Effect[j].delay << endl;
      }
   }
   dumpfile << endl;
}


void InitManager::dump_blinking_lights_vals()
{
   if (strcasecmp(Defaults->MasterConfig.blinking_lights_file, "NONE") == 0) {
      strcpy(name_array, "No Blinking Lights File specified.");
      dump_heading(name_array);
      dumpfile << endl;
      return;
   }

   strcpy(name_array, "Blinking Light vals (");
   strcat(name_array, Defaults->MasterConfig.blinking_lights_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   dumpfile << "#: x y z r g b size rate\n";
   dumpfile << "------------------------\n";

   for (int i = 0; i < Defaults->num_blinking_lights; i++) {
      dumpfile << i+1 << ": " << Defaults->BlinkingLights[i].Position.x;
      dumpfile <<         " " << Defaults->BlinkingLights[i].Position.y;
      dumpfile <<         " " << Defaults->BlinkingLights[i].Position.z;
      dumpfile <<         " " << Defaults->BlinkingLights[i].Color.r;
      dumpfile <<         " " << Defaults->BlinkingLights[i].Color.g;
      dumpfile <<         " " << Defaults->BlinkingLights[i].Color.b;
      dumpfile <<         " " << Defaults->BlinkingLights[i].size;
      dumpfile <<         " " << Defaults->BlinkingLights[i].blink_rate;
      dumpfile << endl;
   }
   dumpfile << endl;
}

void InitManager::dump_filter_vals()
{
   if (strcasecmp(Defaults->MasterConfig.filter_file, "NONE") == 0) {
      strcpy(name_array, "No Filter File specified.");
      dump_heading(name_array);
      dumpfile << endl;
      return;
   }

   strcpy(name_array, "Filter vals (");
   strcat(name_array, Defaults->MasterConfig.filter_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   dumpfile << "#: site host entity [Block type]\n";
   dumpfile << "#: x y z AND OR range [Force type]\n";
   dumpfile << "----------------------------------\n";

   for (int i = 0; i < Defaults->num_filters; i++) {
      switch (Defaults->Filters[i].filter_type) {
         case (TYPE_BLOCK):
            dumpfile << i+1 << ": " << Defaults->Filters[i].Type.Block.site;
            dumpfile <<   " " << Defaults->Filters[i].Type.Block.host;
            dumpfile <<   " " << Defaults->Filters[i].Type.Block.entity;
            break;
         case (TYPE_FORCE):
            dumpfile << i+1 << ": " << Defaults->Filters[i].Type.Force.x;
            dumpfile <<         " " << Defaults->Filters[i].Type.Force.y;
            dumpfile <<         " " << Defaults->Filters[i].Type.Force.z;
            dumpfile << " " << Defaults->Filters[i].Type.Force.and_mask;
            dumpfile << " " << Defaults->Filters[i].Type.Force.or_mask;
            dumpfile << " " << Defaults->Filters[i].Type.Force.range;
            break;
         default:
            dumpfile << "???\n";
      }
      dumpfile << endl;
   }
   dumpfile << endl;
}

void InitManager::dump_landmark_vals()
{
   if (strcasecmp(Defaults->MasterConfig.landmark_file, "NONE") == 0) {
      strcpy(name_array, "No Landmark File specified.");
      dump_heading(name_array);
      dumpfile << endl;
      return;
   }

   strcpy(name_array, "Landmark vals (");
   strcat(name_array, Defaults->MasterConfig.landmark_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   dumpfile << "#: icon x y\n";
   dumpfile << "-----------\n";

   for (int i = 0; i < Defaults->num_landmarks; i++) {
      dumpfile << i+1 << ": " << Defaults->Landmarks[i].icon;
      dumpfile <<         " " << Defaults->Landmarks[i].x;
      dumpfile <<         " " << Defaults->Landmarks[i].y;
      dumpfile << endl;
   }
   dumpfile << endl;
}

void InitManager::dump_location_vals()
{
   struct Pos	*current;

   if (strcasecmp(Defaults->MasterConfig.location_file, "NONE") == 0) {
      strcpy(name_array, "No Location File specified.");
      dump_heading(name_array);
      dumpfile << endl;
      return;
   }

   strcpy(name_array, "Location vals (");
   strcat(name_array, Defaults->MasterConfig.location_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   dumpfile << "#: name path alive dead otype\n";
   dumpfile << "\tx y z heading pitch roll\n";
   dumpfile << "--------------------------------\n";

   for (int i = 0; i < Defaults->num_static_objs; i++) {
      dumpfile << i+1 << ": " << Defaults->SObjects[i].flightfile;
      dumpfile <<         " " << Defaults->SObjects[i].path;
      dumpfile <<         " " << Defaults->SObjects[i].alive_mod;
      dumpfile <<         " " << Defaults->SObjects[i].dead_mod;
      dumpfile <<         " " << Defaults->SObjects[i].otype;
      dumpfile << endl;
      for (int j = 0; j < Defaults->SObjects[i].num_points; j++) {
         current = &(Defaults->SObjects[i].points[j]);
         dumpfile << "\t" << current->x;
         dumpfile <<  " " << current->y;
         dumpfile <<  " " << current->z;
         dumpfile <<  " " << current->heading;
         dumpfile <<  " " << current->pitch;
         dumpfile <<  " " << current->roll;
         dumpfile << endl;
      }
   }
   dumpfile << endl;
}

void InitManager::dump_model_vals()
{
   struct Weap	*curr;

   strcpy(name_array, "Model vals (");
   strcat(name_array, Defaults->MasterConfig.model_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   dumpfile << "#: name [jack] path alive dead vtype\n";
   dumpfile << "      kind domain ctry cat subcat spec\n";
   dumpfile << "\tindex class x y z delay\n";
   dumpfile << "--------------------------------------\n";

   for (int i = 0; i < Defaults->num_dynamic_objs; i++) {
      dumpfile << i+1 << ": " << Defaults->DObjects[i].filename;
      if (Defaults->DObjects[i].mtype == TYPE_TIPS)
         dumpfile <<      " " << Defaults->DObjects[i].jack_model;
      if (Defaults->DObjects[i].mtype != TYPE_JADE)
         dumpfile <<         " " << Defaults->DObjects[i].path;
      dumpfile <<         " " << Defaults->DObjects[i].alive_mod;
      dumpfile <<         " " << Defaults->DObjects[i].dead_mod;
      dumpfile <<         " " << Defaults->DObjects[i].vtype;
      dumpfile << endl;
      dumpfile <<    "      " << (int) Defaults->DObjects[i].etype.entity_kind;
      dumpfile <<         " " << (int) Defaults->DObjects[i].etype.domain;
      dumpfile <<         " " << (int) Defaults->DObjects[i].etype.country;
      dumpfile <<         " " << (int) Defaults->DObjects[i].etype.category;
      dumpfile <<         " " << (int) Defaults->DObjects[i].etype.subcategory;
      dumpfile <<         " " << (int) Defaults->DObjects[i].etype.specific;
      dumpfile << endl;
      for (int j = 0; j < Defaults->DObjects[i].num_weapons; j++) {
         curr = &(Defaults->DObjects[i].weapons[j]);
         dumpfile << "\t" << curr->index;
         dumpfile << " "  << curr->wclass;
         dumpfile << " "  << curr->x;
         dumpfile << " "  << curr->y;
         dumpfile << " "  << curr->z;
         dumpfile << " "  << curr->delay;
         dumpfile << endl;
      }
   }
   dumpfile << endl;
}

void InitManager::dump_network_vals()
{
   struct Host	*curr;

   strcpy(name_array, "Network vals (");
   strcat(name_array, Defaults->MasterConfig.network_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   dumpfile << "host siteno hostno interf\n";
   dumpfile << "-------------------------\n";

   for (int i = 0; i < Defaults->num_hosts; i++) {
      curr = &(Defaults->Hosts[i]);
      dumpfile << curr->hostname;
      dumpfile << " " << curr->siteno;
      dumpfile << " " << curr->hostno;
      dumpfile << " " << curr->interf;
      dumpfile << endl;
   }

   dumpfile << endl;
}


void InitManager::dump_radar_icon_vals()
{
   if (strcasecmp(Defaults->MasterConfig.radar_icon_file, "NONE") == 0) {
      strcpy(name_array, "No Radar Icon File specified.");
      dump_heading(name_array);
      dumpfile << endl;
      return;
   }

   strcpy(name_array, "Radar Icon vals (");
   strcat(name_array, Defaults->MasterConfig.radar_icon_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   dumpfile << "#: min max icon\n";
   dumpfile << "---------------\n";

   for (int i = 0; i < Defaults->num_radar_icons; i++) {
      dumpfile << i+1 << ": " << Defaults->RadarIcons[i].min;
      dumpfile <<         " " << Defaults->RadarIcons[i].max;
      dumpfile <<         " " << Defaults->RadarIcons[i].ident;
      dumpfile << endl;
   }

   dumpfile << endl;
}

void InitManager::dump_round_world_vals()
{
   if (strcasecmp(Defaults->MasterConfig.round_world_file, "NONE") == 0) {
      strcpy(name_array, "No Round World File specified.");
      dump_heading(name_array);
      dumpfile << endl;
      return;
   }

   strcpy(name_array, "Round World vals (");
   strcat(name_array, Defaults->MasterConfig.round_world_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   dumpfile << "northing=" << Defaults->RoundWorld.northing;
   dumpfile << " easting=" << Defaults->RoundWorld.easting;
   dumpfile << " zone_num=" << Defaults->RoundWorld.zone_num;
   dumpfile << " zone_letter=" << Defaults->RoundWorld.zone_letter;
   dumpfile << "\nmap_datum=" << Defaults->RoundWorld.map_datum;
   dumpfile << " width=" << Defaults->RoundWorld.width;
   dumpfile << " height=" << Defaults->RoundWorld.height;
   dumpfile << "\n\n";
}

void InitManager::dump_sound_vals()
{
   if (strcasecmp(Defaults->MasterConfig.sound_file, "NONE") == 0) {
      strcpy(name_array, "No Sound File specified.");
      dump_heading(name_array);
      dumpfile << endl;
      return;
   }

   strcpy(name_array, "Sound vals (");
   strcat(name_array, Defaults->MasterConfig.sound_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   for (int i = 0; i < MAX_SOUND_EVENTS; i++) {
      if (Defaults->Sounds[i].num_events <= 0) {
         // No events for this sound index
         continue;
      }
      for (int j = 0; j < Defaults->Sounds[i].num_events; j++) {
         dumpfile << i;
         dumpfile << ": " << Defaults->Sounds[i].Events[j].filename;
         dumpfile << " "  << Defaults->Sounds[i].Events[j].range;
         dumpfile << endl;
      }
      dumpfile << endl;
   }
   dumpfile << endl;
}

void InitManager::dump_terrain_vals()
{
   struct TM	*curr;

   strcpy(name_array, "Terrain vals (");
   strcat(name_array, Defaults->MasterConfig.terrain_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   for (int i = 0; i < Defaults->num_masks; i++) {
      curr = &(Defaults->Masks[i]);
      dumpfile << curr->filename << " ";
      switch (curr->mask) {
         case BRIDGE_MASK:
            dumpfile << "BRIDGE_MASK";
            break;
         case FOLIAGE_MASK:
            dumpfile << "FOLIAGE_MASK";
            break;
         case TERRAIN_MASK:
            dumpfile << "TERRAIN_MASK";
            break;
         case TUNNEL_MASK:
            dumpfile << "TUNNEL_MASK";
            break;
         case WATER_MASK:
            dumpfile << "WATER_MASK";
            break;
      }
      dumpfile << endl;
   }
   //dumpfile << endl;
}

void InitManager::dump_transport_locs_vals()
{
   // A default transport location is guaranteed
   strcpy(name_array, "Transport vals (");
   strcat(name_array, Defaults->MasterConfig.transport_file);
   strcat(name_array, "):");
   dump_heading(name_array);

   dumpfile << "#: x y z heading pitch roll\n";
   dumpfile << "---------------------------\n";

   for (int i = 0; i < Defaults->num_transport_locs; i++) {
      dumpfile << i+1 << ": " << Defaults->TransportLocs[i].x;
      dumpfile <<         " " << Defaults->TransportLocs[i].y;
      dumpfile <<         " " << Defaults->TransportLocs[i].z;
      dumpfile <<         " " << Defaults->TransportLocs[i].heading;
      dumpfile <<         " " << Defaults->TransportLocs[i].pitch;
      dumpfile <<         " " << Defaults->TransportLocs[i].roll;
      dumpfile << endl;
   }
   dumpfile << endl;
}


void InitManager::dump_heading(char *label)
{
   dumpfile << label << endl;
   for (int i = 0, j = strlen(label); i < j; i++)
      dumpfile << "=";
   dumpfile << endl;
}


// EOF
