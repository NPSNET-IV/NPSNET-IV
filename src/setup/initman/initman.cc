// initman.cc

extern "C" {
#include <bstring.h>
#include <string.h>
}
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "im.tab.h"
#include "imextern.h"
#include "imclass.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "display_const.h"
#include "environment_const.h"
#include "database_const.h"
#include "entity_const.h"
#include "interface_const.h"

extern int errno;

/* Globals needed by parser subsystem */
char current_file[128];
int  current_file_type;
int  error_reported, error_reported2;
struct defs *Defaults;
struct MC  *mc;

#define SHARE_PATH "IM_SHARE_PATH"

InitManager::InitManager()	// Constructor
   // Assign defaults
{
   // cout << "Initialization Manager installed.\n";
   char *thePath = getenv (SHARE_PATH);
   if (thePath) {
      sprintf (share_file, "%s/initman.arena.XXXXXX", thePath);
   }
   else {
      strcpy (share_file, "/tmp/initman.arena.XXXXXX");
   }
   mktemp (share_file);
   usconfig (CONF_INITSIZE, sizeof (struct defs) * 2);
   initArena = usinit (share_file);

   if (!initArena) {
      cerr << "Error: Unable to usinit: " << share_file << endl << endl
           << "\tThis may be beceause there is not enough space in:" << endl
           << endl
           << "\t\t";
      thePath ? cerr << thePath:cerr << "/tmp";
      cerr << endl << endl
           << "\tTry seting the enviornmental Variable: " << SHARE_PATH << endl
           << "\tto a path that has more space." << endl;
      exit (1);
   }

   chmod (share_file, S_IRUSR | S_IWUSR | 
                      S_IRGRP | S_IWGRP |
                      S_IROTH | S_IWOTH);

   Defaults = (struct defs *)usmalloc (sizeof (struct defs), initArena);

   unlink (share_file);

   if (!Defaults) {
      cerr << "Error: Unable to usmalloc initman struct defs." << endl << endl
           << "\tThis may be beceause there is not enough space in:" << endl
           << endl
           << "\t\t";
      thePath ? cerr << thePath:cerr << "/tmp";
      cerr << endl << endl
           << "\tTry seting the enviornmental Variable: " << SHARE_PATH << endl
           << "\tto a path that has more space." << endl;
      exit (1);
   }
   bzero((void *) Defaults, sizeof(struct defs));
   Defaults->InitComplete = FALSE;	// redundant
   mc = &(Defaults->MasterConfig);
   assign_defaults();
}

InitManager::~InitManager()	// Destructor
{
   // Do something?
   usfree ((void *)Defaults, initArena);
   usdetach (initArena);
   
}


void InitManager::assign_defaults()
{
   cout << "ASSIGNING defaults.\n";

   // Defaults with non-zero values
   no_master = TRUE;
   mc->animations = TRUE;
   strcpy(mc->animation_file, IM_ANIMATIONS_FILE);
   strcpy(mc->auto_file, "NONE");
   mc->auto_state = FALSE;
   mc->bdihml = TRUE;
   strcpy(mc->bird_port, "NONE");
   strcpy(mc->blinking_lights_file, "NONE");
   mc->BoundingBox.xmin = D_BBOX_MINX;
   mc->BoundingBox.xmax = D_BBOX_MAXX;
   mc->BoundingBox.ymin = D_BBOX_MINY;
   mc->BoundingBox.ymax = D_BBOX_MAXY;
   mc->ClipPlanes.cp_near = MIN_NEAR;
   mc->ClipPlanes.cp_far  = MIN_FAR;
   mc->control = KEYBOARD;
   strcpy(mc->cursor_file, "NONE");
   mc->db_paging = FALSE;
   mc->db_pages_allowed = 0;
   strncpy(mc->db_tsg_file, "NONE", STRING128);
   mc->db_offset = 0;
   mc->dead_switch = FALSE;
   mc->diguy_info.speed_ratio = 1.0f;
   mc->diguy_info.position_error = 10000.0f;
   mc->diguy_info.time_constant = 10000.0f;
   mc->diguy_info.trans_smoothness = TRANS_SMOOTHEST;
   mc->diguy_info.position_mode = SMOOTH_MODE;
   mc->diguy_info.position_mechanism = DIGUY_INTERNAL;
   mc->DIS.heartbeat = UPDATETIME;
   mc->DIS.timeout = DIS_TIMEOUT;
   mc->DIS.rotdelta = ROT_DELTA;
   mc->DIS.posdelta = DIST_DELTA;
   mc->dis_enum = DISv204;
   strncpy(mc->dump_file, DUMP_FILENAME, STRING128);
   strcpy(mc->dvw_file, D_DVW_FILE);
   mc->dynamic_objs = TRUE;
   strcpy(mc->environmental_file, "NONE");
   strcpy(mc->entity_file, "NONE");
   mc->es_mode = SKY_GROUND;
   strcpy(mc->ether_interf, "NONE");
   mc->fcs.check = FALSE;
   mc->fcs.mp = TRUE;
   strcpy(mc->fcs.port, "NONE");
   mc->fcs.prompt = FALSE;
   strcpy(mc->filter_file, "NONE");
   mc->flat_world = FALSE;
   mc->flybox = TRUE;
   strcpy(mc->flybox_port, "NONE");
   mc->FoV.x = D_FOVX;
   mc->FoV.y = D_FOVY;
   mc->fog_near = D_FOG_NEAR;
   mc->fog_far = D_FOG_FAR;
   mc->fog_type = FOG_VTX_LIN;
   mc->force = ForceID_Other;
   mc->frame_rate = D_FRAME_RATE;
   strcpy(mc->fs_port, "NONE");
   strcpy(mc->fs_config, "NONE");
   mc->fs_scale = 10.0f;
   mc->ground_height = D_GRND_HT;
   mc->hires = IM_NET_OFF;
   mc->hmd = FALSE;
   strcpy(mc->hmd_file, "NONE");
   mc->horizon_angle = D_HORIZ_ANGLE;
   strncpy(mc->hud_font_file, HUD_FONT_FILENAME, STRING128);
   mc->hud_level = 1;
   strncpy(mc->identify_file, D_IDENTIFY_FILE, STRING128);
   strcpy(mc->intro_font_file, "NONE");
   mc->jack = TRUE;
   strcpy(mc->landmark_file, "NONE");
   strncpy(mc->leather_flybox_port, D_FLYBOX_PORT, STRING32);
   strcpy(mc->location_file, "NONE");
   mc->lod_fade = 0.0f;
   mc->lod_scale = 1.0f;
   mc->lw_rport = 5000;
   mc->lw_sport = -1;
   strcpy(mc->lw_toip, "NONE");
   strcpy(mc->marking, "NONE");
   strcpy(mc->master, "NONE");
   mc->alt.max = 8000.0f;
   strncpy(mc->model_file, MODEL_FILENAME, STRING128);
   strcpy(mc->modsaf_map, "NONE");
   strncpy(mc->multicast.group, DEFAULT_MC_GROUP, STRING32);
   mc->multicast.ttl = DEFAULT_MC_TTL;
   mc->multisample = TRUE;
   mc->networking = TRUE;
   strncpy(mc->network_file, NETWORK_FILENAME, STRING128);
   mc->notify_level = IM_FATAL;
   strcpy(mc->nps_pic_dir, "/tmp/");
   strcpy(mc->odt_port, "NONE");
   mc->odt_mp = TRUE;
   strncpy(mc->panel.group, IDU_DEF_MC_GROUP, STRING32);
   mc->panel.ttl = IDU_DEF_MC_TTL;
   mc->panel.port = IDU_DEF_MC_PORT;
   mc->phase = ALG_FLOAT;
   mc->pictures = TRUE;
   strncpy(mc->PO.ctdb_file, D_PO_CTDB_FILE, STRING128);
   mc->PO.database = D_PO_DATABASE;
   mc->PO.exercise = D_PO_EXERCISE;
   mc->PO.port = D_PO_PORT;
   mc->process_mode = IM_DEFAULT;
   strncpy(mc->radar_icon_file, D_RADAR_ICON_FILE, STRING128);
   mc->range_filter = 100.0f;
   strcpy(mc->road_file, "NONE");
   strcpy(mc->round_world_file, "NONE");
   mc->scoring = TRUE;
   strcpy(mc->search_path, D_SEARCH_PATH);
   strcpy(mc->sound_file, "NONE");
   mc->static_objs = TRUE;
   mc->target_bbox = TRUE;
   mc->target_entity.site = 0;
   mc->target_entity.host = 0;
   mc->target_entity.entity = 0;
   strncpy(mc->terrain_file, "", STRING128);
   strcpy(mc->testport, "NONE");
   strcpy(mc->traker_file, "NONE");
   strcpy(mc->traker_port, "NONE");
   strcpy(mc->transport_file, "NONE");
   strcpy(mc->treadport, "NONE");
   strcpy(mc->uniport, "NONE");
   strcpy(mc->upperbody, "NONE"); 
   mc->use_round = -1;
   strncpy(mc->vehicle_driven, D_DRIVEN_FILE, STRING64);
   mc->view = IM_START;
   mc->ViewPort.xmax = D_VIEWPORT_RIGHT;
   mc->ViewPort.ymax = D_VIEWPORT_TOP;
   mc->ViewHPR.heading = DEF_VIEW_HEADING;
   mc->ViewXYZ.x = DEF_VIEW_X;
   mc->ViewXYZ.y = DEF_VIEW_Y;
   mc->ViewXYZ.z = DEF_VIEW_Z;
   mc->weapon_view = FALSE;
   mc->window.type = NORMAL;
   mc->window.xmin = D_WINDOW_LEFT;
   mc->window.xmax = D_WINDOW_RIGHT;
   mc->window.ymin = D_WINDOW_BOTTOM;
   mc->window.ymax = D_WINDOW_TOP;
}


void InitManager::validate_data()
   // Data is parsed according to type only. This procedure performs
   //   other validations, e.g. enforcing min and max limits. It also
   //   prints a message to stdout when a value has been changed.
{
   int	i;

   cout << "VALIDATING data.\n";

   // BOUNDING_BOX
   if (mc->BoundingBox.xmin < MIN_BBOX) {
      mc->BoundingBox.xmin = MIN_BBOX;
      cout << "\tRaised BOUNDING_BOX xmin to the minimum.\n";
   } else if (mc->BoundingBox.xmin > MAX_BBOX) {
      mc->BoundingBox.xmin = MAX_BBOX;
      cout << "\tLowered BOUNDING_BOX xmin to the maximum.\n";
   }

   if (mc->BoundingBox.xmax < MIN_BBOX) {
      mc->BoundingBox.xmax = MIN_BBOX;
      cout << "\tRaised BOUNDING_BOX xmax to the minimum.\n";
   } else if (mc->BoundingBox.xmax > MAX_BBOX) {
      mc->BoundingBox.xmax = MAX_BBOX;
      cout << "\tLowered BOUNDING_BOX xmax to the maximum.\n";
   }

   if (mc->BoundingBox.xmax <= mc->BoundingBox.xmin) {
      mc->BoundingBox.xmax = (mc->BoundingBox.xmin + 1.0f);
      cout << "\tBOUNDING_BOX xmax <= xmin; changed xmax to (xmin+1).\n";
   }

   if (mc->BoundingBox.ymin < MIN_BBOX) {
      mc->BoundingBox.ymin = MIN_BBOX;
      cout << "\tRaised BOUNDING_BOX ymin to the minimum.\n";
   } else if (mc->BoundingBox.ymin > MAX_BBOX) {
      mc->BoundingBox.ymin = MAX_BBOX;
      cout << "\tLowered BOUNDING_BOX ymin to the maximum.\n";
   }

   if (mc->BoundingBox.ymax < MIN_BBOX) {
      mc->BoundingBox.ymax = MIN_BBOX;
      cout << "\tRaised BOUNDING_BOX ymax to the minimum.\n";
   } else if (mc->BoundingBox.ymax > MAX_BBOX) {
      mc->BoundingBox.ymax = MAX_BBOX;
      cout << "\tLowered BOUNDING_BOX ymax to the maximum.\n";
   }

   if (mc->BoundingBox.ymax <= mc->BoundingBox.ymin) {
      mc->BoundingBox.ymax = (mc->BoundingBox.ymin + 1.0f);
      cout << "\tBOUNDING_BOX ymax <= ymin; changed ymax to (ymin+1).\n";
   }

   // CLIP_PLANES
   if (mc->ClipPlanes.cp_near < MIN_NEAR) {
      mc->ClipPlanes.cp_near = MIN_NEAR;
      cout << "\tRaised CLIP_PLANES near to the minimum.\n";
   } else if (mc->ClipPlanes.cp_near > MAX_NEAR) {
      mc->ClipPlanes.cp_near = MAX_NEAR;
      cout << "\tLowered CLIP_PLANES near to the maximum.\n";
   }
   if (mc->ClipPlanes.cp_far < MIN_FAR) {
      mc->ClipPlanes.cp_far = MIN_FAR;
      cout << "\tRaised CLIP_PLANES far to the minimum.\n";
   } else if (mc->ClipPlanes.cp_far > MAX_FAR) {
      mc->ClipPlanes.cp_far = MAX_FAR;
      cout << "\tLowered CLIP_PLANES far to the maximum.\n";
   }

   // DIS_EXERCISE
   if (mc->DIS.exercise < 0) {
      mc->DIS.exercise = 0;
      cout << "\tRaised DIS_EXERCISE to 0.\n";
   }

   // DYNAMIC OBJECTS (Model File)
   for (i = 0; i < Defaults->num_dynamic_objs; i++) {
      if ((Defaults->DObjects[i].alive_mod > MAX_VEHTYPES) ||
          (Defaults->DObjects[i].dead_mod > MAX_VEHTYPES)) {
         // tell 'em about it
         cout << "\tRejecting dynamic object " << Defaults->DObjects[i].filename
              << ", alive model#=" << Defaults->DObjects[i].alive_mod
              << ", dead model#=" << Defaults->DObjects[i].dead_mod
              << ": max allowable value for either is "
              << MAX_VEHTYPES << ".\n";
         // delete from array
         int rows_to_move = (Defaults->num_dynamic_objs - (i+1));
         if (rows_to_move > 0) {	// not last record
            int bytes_to_move = (rows_to_move * sizeof(struct Dyn));
            bcopy((void*) &(Defaults->DObjects[i+1]),	// from
                  (void*) &(Defaults->DObjects[i]),	// to
                  bytes_to_move);
         }
         Defaults->num_dynamic_objs--;
         bzero(&(Defaults->DObjects[Defaults->num_dynamic_objs]),
               sizeof(struct Dyn));	// clear redundant last record
      }
   }
   if ((mc->dynamic_objs == FALSE) &&
       strcasecmp(mc->vehicle_driven, "defmodel")) {
      // No dynamic objs and NOT default model specified
      strcpy(mc->vehicle_driven, "defmodel");
      cout << "\tDYNAMIC_OBJS=OFF, VEHICLE_DRIVEN reset to default model.\n";
   }

   // FCS
   if (strcasecmp(mc->fcs.port, "NONE") == 0)
      mc->fcs.check = FALSE;
   if (mc->fcs.check == FALSE) {
      strcpy(mc->fcs.port, "NONE");
      if (mc->control == FCS)
         mc->control = KEYBOARD;
   }

   // FIELD_OF_VIEW
   if (mc->FoV.x < MIN_FOVX) {
      mc->FoV.x = MIN_FOVX;
      cout << "\tRaised FIELD_OF_VIEW X to the minimum.\n";
   }
   if (mc->FoV.x > MAX_FOVX) {
      mc->FoV.x = MAX_FOVX;
      cout << "\tLowered FIELD_OF_VIEW X to the maximum.\n";
   }
   if (mc->FoV.y < MIN_FOVY) {
      mc->FoV.y = MIN_FOVY;
      cout << "\tRaised FIELD_OF_VIEW Y to the minimum.\n";
   }
   if (mc->FoV.y > MAX_FOVY) {
      mc->FoV.y = MAX_FOVY;
      cout << "\tLowered FIELD_OF_VIEW Y to the maximum.\n";
   }

   // FLYBOX
   if (mc->flybox == FALSE) {
      strcpy(mc->flybox_port, "NONE");
      if (mc->control == FLYBOXIN)
         mc->control = KEYBOARD;
   }

   // FRAME_RATE
   if ((MONITOR_HERTZ % int(mc->frame_rate)) != 0) {
      mc->frame_rate = D_FRAME_RATE;
      cout << "\tInvalid FRAME_RATE of " << mc->frame_rate;
      cout << "; must be a whole divisor of " << MONITOR_HERTZ << ";\n";
      cout << "\treset to default of " << D_FRAME_RATE << ".\n";
   }

   // GROUND_HEIGHT
   if (mc->ground_height < MIN_GROUND_HEIGHT) {
      mc->ground_height = MIN_GROUND_HEIGHT;
      cout << "\tRaised GROUND_HEIGHT to the minimum.\n";
   } else if (mc->ground_height > MAX_GROUND_HEIGHT) {
      mc->ground_height = MAX_GROUND_HEIGHT;
      cout << "\tLowered GROUND_HEIGHT to the maximum.\n";
   }

   // HMD
   if (mc->hmd) {
      if (mc->hmd_file[0] == '\0') {
         mc->hmd = FALSE;
         cout << "\tHMD turned OFF, HMD_FILE not specified.\n";
      }
      else if (mc->stereo) {
         mc->stereo = FALSE;
         cout << "\tHMD ON forced STEREO OFF.\n";
      }
   }

   // HORIZON_ANGLE
   if (mc->horizon_angle < MIN_HORIZ_ANGLE) {
      mc->horizon_angle = MIN_HORIZ_ANGLE;
      cout << "\tRaised HORIZON_ANGLE to the minimum.\n";
   } else if (mc->horizon_angle > MAX_HORIZ_ANGLE) {
      mc->horizon_angle = MAX_HORIZ_ANGLE;
      cout << "\tLowered HORIZON_ANGLE to the maximum.\n";
   }

   // LEATHER_FLYBOX
   if (mc->leather_flybox_check == FALSE) {
      strcpy(mc->leather_flybox_port, "NONE");
      if (mc->control == LEATHER_FLYBOX)
         mc->control = KEYBOARD;
   }

   // PO_*
   if (mc->PO.database < 0) {
      mc->PO.database = 0;
      cout << "\tRaised PO_DATABASE to 0.\n";
   }
   if (mc->PO.exercise < 0) {
      mc->PO.exercise = 0;
      cout << "\tRaised PO_EXERCISE to 0.\n";
   }
   if (mc->PO.port < 0) {
      mc->PO.port = 0;
      cout << "\tRaised PO_PORT to 0.\n";
   }

   // RADAR ICONS
   for (i = 0; i < Defaults->num_radar_icons; i++) {
      if ((Defaults->RadarIcons[i].min < 0) ||
          (Defaults->RadarIcons[i].max >= MAX_VEHTYPES) ||
          (Defaults->RadarIcons[i].min > Defaults->RadarIcons[i].max)) {
         // tell 'em about it
         cout << "\tRejecting Radar Icon min="
              << Defaults->RadarIcons[i].min << ",max="
              << Defaults->RadarIcons[i].max
              << ": valid range is from min=0 to max="
              << MAX_VEHTYPES-1 << ", max must be > min.\n";
         // delete from array
         int rows_to_move = (Defaults->num_radar_icons - (i+1));
         if (rows_to_move > 0) {	// not last record
            int bytes_to_move = (rows_to_move * sizeof(struct RI));
            bcopy((void*) &(Defaults->RadarIcons[i+1]),	// from
                  (void*) &(Defaults->RadarIcons[i]),	// to
                  bytes_to_move);
         }
         Defaults->num_radar_icons--;
         bzero(&(Defaults->RadarIcons[Defaults->num_radar_icons]),
               sizeof(struct RI));	// clear redundant last record
      }
   }
   // Guarantee a minimum of one radar icon
   if (Defaults->num_radar_icons == 0) {
      Defaults->RadarIcons[0].min = 0;
      Defaults->RadarIcons[0].max = MAX_VEHTYPES-1;
      Defaults->RadarIcons[0].ident = '?';
      Defaults->num_radar_icons = 1;
   }

   // STATIC OBJECTS (Location File)
   for (i = 0; i < Defaults->num_static_objs; i++) {
      if ((Defaults->SObjects[i].alive_mod > MAX_STATIC_OBJ_TYPES) ||
          (Defaults->SObjects[i].dead_mod > MAX_STATIC_OBJ_TYPES)) {
         // tell 'em about it
         cout << "\tRejecting static object " << Defaults->SObjects[i].flightfile
              << ", alive model#=" << Defaults->SObjects[i].alive_mod
              << ", dead model#=" << Defaults->SObjects[i].dead_mod
              << ": max allowable value for either is "
              << MAX_STATIC_OBJ_TYPES << ".\n";
         // delete from array
         int rows_to_move = (Defaults->num_static_objs - (i+1));
         if (rows_to_move > 0) {	// not last record
            int bytes_to_move = (rows_to_move * sizeof(struct Model));
            bcopy((void*) &(Defaults->SObjects[i+1]),	// from
                  (void*) &(Defaults->SObjects[i]),	// to
                  bytes_to_move);
         }
         Defaults->num_static_objs--;
         bzero(&(Defaults->SObjects[Defaults->num_static_objs]),
               sizeof(struct Model));	// clear redundant last record
      }
   }

   // STEREO
   if (mc->stereo) {
      mc->hmd = FALSE;
      cout << "\tSTEREO ON forced HMD OFF.\n";
   }

   // TRANSPORT LOCATIONS (must be done after BOUNDING_BOX)
   // Within Bounding Box?

// Commented transport validation so that ship transport will work

/*   for (i = 0; i < Defaults->num_transport_locs; i++) {
      if ((Defaults->TransportLocs[i].x > mc->BoundingBox.xmax) ||
          (Defaults->TransportLocs[i].x < mc->BoundingBox.xmin) ||
          (Defaults->TransportLocs[i].y > mc->BoundingBox.ymax) ||
          (Defaults->TransportLocs[i].y < mc->BoundingBox.ymin)) {
         // tell 'em about it
         cout << "\tRejecting Transport Location x="
              << Defaults->TransportLocs[i].x << ",y="
              << Defaults->TransportLocs[i].y << ": outside Bounding Box.\n";
         // delete from array
         int rows_to_move = (Defaults->num_transport_locs - (i+1));
         if (rows_to_move > 0) {	// not last record
            int bytes_to_move = (rows_to_move * sizeof(struct TP));
            bcopy((void*) &(Defaults->TransportLocs[i+1]),	// from
                  (void*) &(Defaults->TransportLocs[i]),		// to
                  bytes_to_move);
         }
         Defaults->num_transport_locs--;
         bzero(&(Defaults->TransportLocs[Defaults->num_transport_locs]),
               sizeof(struct TP));	// clear redundant last record
      }
   }*/
   // Guarantee a minimum of one transport location
   if (Defaults->num_transport_locs == 0) {
      Defaults->TransportLocs[0].x =
                    ((mc->BoundingBox.xmin + mc->BoundingBox.xmax) / 2.0f);
      Defaults->TransportLocs[0].y =
                    ((mc->BoundingBox.ymin + mc->BoundingBox.ymax) / 2.0f);
      Defaults->TransportLocs[0].z = 0.0f;
      Defaults->TransportLocs[0].heading = 0.0f;
      Defaults->TransportLocs[0].pitch = 0.0f;
      Defaults->TransportLocs[0].roll = 0.0f;
      Defaults->num_transport_locs = 1;
   }

   // VIEWPORT
   if ((mc->ViewPort.xmin < 0.0) || (mc->ViewPort.xmin > 1.0)) {
      mc->ViewPort.xmin = D_VIEWPORT_LEFT;
      cout << "\tChanged invalid VIEWPORT xmin to the default.\n";
   }
   if ((mc->ViewPort.xmax < 0.0) || (mc->ViewPort.xmax > 1.0)) {
      mc->ViewPort.xmax = D_VIEWPORT_RIGHT;
      cout << "\tChanged invalid VIEWPORT xmax to the default.\n";
   }
   if (mc->ViewPort.xmin >= mc->ViewPort.xmax) {
      mc->ViewPort.xmin = D_VIEWPORT_LEFT;
      mc->ViewPort.xmax = D_VIEWPORT_RIGHT;
      cout << "\tVIEWPORT xmin >= xmax; xmin, xmax changed to the defaults.\n";
   }
   if ((mc->ViewPort.ymin < 0.0) || (mc->ViewPort.ymin > 1.0)) {
      mc->ViewPort.ymin = D_VIEWPORT_BOTTOM;
      cout << "\tChanged invalid VIEWPORT ymin to the default.\n";
   }
   if ((mc->ViewPort.ymax < 0.0) || (mc->ViewPort.ymax > 1.0)) {
      mc->ViewPort.ymax = D_VIEWPORT_TOP;
      cout << "\tChanged invalid VIEWPORT ymax to the default.\n";
   }
   if (mc->ViewPort.ymin >= mc->ViewPort.ymax) {
      mc->ViewPort.ymin = D_VIEWPORT_BOTTOM;
      mc->ViewPort.ymax = D_VIEWPORT_TOP;
      cout << "\tVIEWPORT ymin >= ymax; ymin, ymax changed to the defaults.\n";
   }

   // WINDOW_SIZE
   if ((mc->window.xmin < 0.0) || (mc->window.xmin > 2000.0)) {
      mc->window.xmin = D_WINDOW_LEFT;
      cout << "\tChanged invalid WINDOW_SIZE xmin to the default.\n";
   }
   if ((mc->window.xmax < 0.0) || (mc->window.xmax > 2000.0)) {
      mc->window.xmax = D_WINDOW_RIGHT;
      cout << "\tChanged invalid WINDOW_SIZE xmax to the default.\n";
   }
   if (mc->window.xmin >= mc->window.xmax) {
      mc->window.xmin = D_WINDOW_LEFT;
      mc->window.xmax = D_WINDOW_RIGHT;
      cout <<
          "\tWINDOW_SIZE xmin >= xmax; xmin, xmax changed to the defaults.\n";
   }
   if ((mc->window.ymin < 0.0) || (mc->window.ymin > YMAXSCREEN)) {
      mc->window.ymin = D_WINDOW_BOTTOM;
      cout << "\tChanged invalid WINDOW_SIZE ymin to the default.\n";
   }
   if ((mc->window.ymax < 0.0) || (mc->window.ymax > YMAXSCREEN)) {
      mc->window.ymax = D_WINDOW_TOP;
      cout << "\tChanged invalid WINDOW_SIZE ymax to the default.\n";
   }
   if (mc->window.ymin >= mc->window.ymax) {
      mc->window.ymin = D_WINDOW_BOTTOM;
      mc->window.ymax = D_WINDOW_TOP;
      cout <<
          "\tWINDOW_SIZE ymin >= ymax; ymin, ymax changed to the defaults.\n";
   }

   Defaults->InitComplete = TRUE;
}


void InitManager::process_Master_Configs(int argc, char **argv)
   // Parse all Master Config files at once, including those specified
   //   on the command line.
{
   int		full_path;
   char		npsnet[64];
   char		filename[64];

   strcpy(npsnet, argv[0]);
   argc--, argv++;

   // Scan command line for "-f filename" args
   for (int i = 0; i < argc; i++) {
      if (argv[i][0] != '-')	// "something wrong"
         continue;

      // If help requested, go no farther
      if (strcasecmp(&(argv[i][1]), "help") == 0)
         cl_help(npsnet);
      if ((tolower(argv[i][1]) == 'h') && (argv[i][2] == '\0'))
         cl_help(npsnet);

      if ((tolower(argv[i][1]) == 'f') && (argv[i][2] == '\0')) {
         if (++i == argc)
            break;
         if (argv[i][0] == '-')
            cl_help(npsnet);
#ifdef DEBUG2
         cout << "Parsing Master Config: " << argv[i] << endl;
#endif
         // does name have full path?
         full_path = FALSE;
         for (int j = 0; argv[i][j]; j++) {
            if (argv[i][j] == '/') {
               full_path = TRUE;
               break;
            }
         }

         if (full_path == FALSE) {
            // assume it's in config dir
            strcpy(filename, CONFIG_DIR);
            strcat(filename, argv[i]);
         } else {
            strcpy(filename, argv[i]);
         }

         cout << "PROCESSING Master Config File: " << filename << endl;
         parse_file(filename, MASTER_CONFIG_FILETYPE);
         no_master = FALSE;
      }
   }

   // Do default Master Config, if necessary
   if (no_master) {
      strcpy(filename, CONFIG_DIR);
      strcat(filename, D_CONFIG_FILE);
      cout << "PROCESSING default Master Config File: "
                                           << D_CONFIG_FILE << endl;
      parse_file(filename, MASTER_CONFIG_FILETYPE);
   }
}


void InitManager::parse_input_files()
   // Parse input files other than Master Configs. Optional files will
   //   not be assigned defaults. If the mandatory files are not present
   //   the system will be aborted.
{
   cout << "PROCESSING input files:\n";

   if (strcasecmp(mc->blinking_lights_file, "NONE") != 0) {
      cout << "\tBlinking Lights File: " << mc->blinking_lights_file << endl;
      error_reported = FALSE;
      parse_file(mc->blinking_lights_file, BLINKING_FILETYPE);
   }

/*
   if (mc->animations) {
      for (int i = 0; i < Defaults->num_animations; i++) {
         strcpy(animation_file, ANIMATION_DIR);
         strcat(animation_file, Defaults->Animations[i].filename);
         cout << "\tAnimation File: " << animation_file << endl;
         error_reported = FALSE;
         parse_file(animation_file, ANIMATION_FILETYPE);
      }
   }
*/

   if ((strcasecmp(mc->animation_file, "NONE") != 0) && mc->animations) {
      cout << "\tAnimation File: " << mc->animation_file << endl;
      error_reported = FALSE;
      parse_file(mc->animation_file, ANIMATION_FILETYPE);
   }

   if (strcasecmp(mc->filter_file, "NONE") != 0) {
      cout << "\tFilter File: " << mc->filter_file << endl;
      error_reported = FALSE;
      parse_file(mc->filter_file, FILTER_FILETYPE);
   }

   if (strcasecmp(mc->landmark_file, "NONE") != 0) {
      cout << "\tLandmark File: " << mc->landmark_file << endl;
      error_reported = FALSE;
      parse_file(mc->landmark_file, LANDMARK_FILETYPE);
   }

   if (strcasecmp(mc->location_file, "NONE") != 0) {
      cout << "\tLocation File: " << mc->location_file << endl;
      error_reported = FALSE;
      parse_file(mc->location_file, LOCATION_FILETYPE);
   }

   if (strlen(mc->model_file)) {
      cout << "\tModel File: " << mc->model_file << endl;
      error_reported = FALSE;
      parse_file(mc->model_file, MODEL_FILETYPE);
   } else {
      // Required!
      cout << "ABORTING. No Model File specified.\n";
      exit(0);
   }

   if (strlen(mc->network_file)) {
      cout << "\tNetwork File: " << mc->network_file << endl;
      error_reported = FALSE;
      parse_file(mc->network_file, NETWORK_FILETYPE);
   } else {
      cout << "ABORTING. No Network File specified.\n";
      exit(0);
   }

   if (strcasecmp(mc->radar_icon_file, "NONE") != 0) {
      cout << "\tRadar Icon File: " << mc->radar_icon_file << endl;
      error_reported = FALSE;
      parse_file(mc->radar_icon_file, RADAR_ICON_FILETYPE);
   }

   if (strcasecmp(mc->round_world_file, "NONE") != 0) {
      cout << "\tRound World File: " << mc->round_world_file << endl;
      parse_file(mc->round_world_file, ROUND_WORLD_FILETYPE);
   }

   if (strcasecmp(mc->sound_file, "NONE") != 0) {
      cout << "\tSound File: " << mc->sound_file << endl;
      error_reported = FALSE;
      parse_file(mc->sound_file, SOUNDS_FILETYPE);
   }

   if (strlen(mc->terrain_file)) {
      cout << "\tTerrain File: " << mc->terrain_file << endl;
      error_reported = FALSE;
      parse_file(mc->terrain_file, TERRAIN_FILETYPE);
   } else {
      cout << "\tTerrain File: No Terrain File specified.\n";
   }

   if (strcasecmp(mc->transport_file, "NONE") != 0) {
      cout << "\tTransport File: " << mc->transport_file << endl;
      error_reported = FALSE;
      parse_file(mc->transport_file, TRANSPORT_FILETYPE);
   }
}

void InitManager::parse_file(char *fname, int ftype)
   // Set file name and type, call parser.
{
#ifdef DEBUG2
   cout << "fname=" << fname << "; ftype " << ftype << endl;
#endif
   strcpy(current_file, fname);
   current_file_type = ftype;
   parser();
}

/*
void InitManager::init_animations()
{
   Defaults->Animations[0].index = SMALL_FLASH;
   Defaults->Animations[0].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[0].filename, "small_flash", STRING64);
   Defaults->Animations[0].mode = PFSEQ_CYCLE;
   Defaults->Animations[0].reps = 1;

   Defaults->Animations[1].index = MEDIUM_FLASH;
   Defaults->Animations[1].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[1].filename, "medium_flash", STRING64);
   Defaults->Animations[1].mode = PFSEQ_CYCLE;
   Defaults->Animations[1].reps = 1;

   Defaults->Animations[2].index = LARGE_FLASH;
   Defaults->Animations[2].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[2].filename, "large_flash", STRING64);
   Defaults->Animations[2].mode = PFSEQ_CYCLE;
   Defaults->Animations[2].reps = 1;

   Defaults->Animations[3].index = SMALL_GUN;
   Defaults->Animations[3].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[3].filename, "small_gun", STRING64);
   Defaults->Animations[3].mode = PFSEQ_CYCLE;
   Defaults->Animations[3].reps = 1;

   Defaults->Animations[4].index = MEDIUM_GUN;
   Defaults->Animations[4].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[4].filename, "medium_gun", STRING64);
   Defaults->Animations[4].mode = PFSEQ_CYCLE;
   Defaults->Animations[4].reps = 1;

   Defaults->Animations[5].index = LARGE_GUN;
   Defaults->Animations[5].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[5].filename, "large_gun", STRING64);
   Defaults->Animations[5].mode = PFSEQ_CYCLE;
   Defaults->Animations[5].reps = 1;

   Defaults->Animations[6].index = SMALL_SMOKE;
   Defaults->Animations[6].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[6].filename, "small_smoke", STRING64);
   Defaults->Animations[6].mode = PFSEQ_CYCLE;
   Defaults->Animations[6].reps = 1;

   Defaults->Animations[7].index = MEDIUM_SMOKE;
   Defaults->Animations[7].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[7].filename, "medium_smoke", STRING64);
   Defaults->Animations[7].mode = PFSEQ_CYCLE;
   Defaults->Animations[7].reps = 1;

   Defaults->Animations[8].index = LARGE_SMOKE;
   Defaults->Animations[8].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[8].filename, "large_smoke", STRING64);
   Defaults->Animations[8].mode = PFSEQ_CYCLE;
   Defaults->Animations[8].reps = 1;

   Defaults->Animations[9].index = EXPLOSION;
   Defaults->Animations[9].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[9].filename, "explosion", STRING64);
   Defaults->Animations[9].mode = PFSEQ_SWING;
   Defaults->Animations[9].reps = 2;

   Defaults->Animations[10].index = AIR_BURST;
   Defaults->Animations[10].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[10].filename, "air_burst", STRING64);
   Defaults->Animations[10].mode = PFSEQ_CYCLE;
   Defaults->Animations[10].reps = 1;

   Defaults->Animations[11].index = GROUND_HIT;
   Defaults->Animations[11].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[11].filename, "ground_hit", STRING64);
   Defaults->Animations[11].mode = PFSEQ_CYCLE;
   Defaults->Animations[11].reps = 1;

   Defaults->Animations[12].index = IMPACT;
   Defaults->Animations[12].count = MAX_ANIM_COPIES;
   strncpy(Defaults->Animations[12].filename, "impact", STRING64);
   Defaults->Animations[12].mode = PFSEQ_CYCLE;
   Defaults->Animations[12].reps = 1;

   Defaults->num_animations = 13;
}
*/


int InitManager::dump_inits()
   // Print initialization values for all input files to dumpfile.
{
   dumpfile.open(mc->dump_file, ios::out);

   cout << "DUMPING initial values to: " << mc->dump_file << "\n\n";

   if (!dumpfile) {     // open failed
      cerr << "Error: Cannot open " << mc->dump_file << " for writing\n";
      return (-1);
   }

   dump_config_vals();
   dump_animation_vals();
   dump_blinking_lights_vals();
   dump_filter_vals();
   dump_landmark_vals();
   dump_location_vals();
   dump_model_vals();
   dump_network_vals();
   dump_radar_icon_vals();
   dump_round_world_vals();
   dump_sound_vals();
   dump_transport_locs_vals();
   dump_terrain_vals();

   dumpfile.close();

   return (0);
}

// EOF
