// File: <read_config.cc>

/*
 * Copyright (c) 1995,
 *    Naval Postgraduate School
 *    Computer Graphics and Video Laboratory
 *    NPSNET Research Group
 *    npsnet@cs.nps.navy.mil
 *
 *
 *    Permission to use, copy, and modify this software and its
 * documentation for any non-commercial purpose is hereby granted
 * without fee, provided that (i) the above copyright notices and the
 * following permission notices appear in ALL copies of the software
 * and related documentation, and (ii) The Naval Postgraduate School
 * Computer Graphics and Video Laboratory and the NPSNET Research Group
 * be given written credit in your software's written documentation and
 * be given graphical credit on any start-up/credit screen your 
 * software generates.
 *    This restriction helps justify our research efforts to the
 * sponsors who fund our research.
 *
 *    Do not redistribute this code without the express written
 * consent of the NPSNET Research Group. (E-mail communication and our
 * confirmation qualifies as written permission.) As stated above, this 
 * restriction helps justify our research efforts to the sponsors who
 * fund our research.
 *
 *    This software was designed and implemented at U.S. Government
 * expense and by employees of the U.S. Government.  It is illegal
 * to charge any U.S. Government agency for its partial or full use.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * E-Mail addresses:
 *    npsnet@cs.nps.navy.mil
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */

#include <math.h>
#include <fstream.h>
#include <iostream.h>
#include <string.h>
#include <invent.h>
#include <sys/systeminfo.h>

#include <stdio.h>
#include "read_config.h"
#include "common_defs2.h"
#include "fileio.h"

#include "database_const.h"
#include "media_const.h"
#include "communication_const.h"
#include "interface_const.h"
#include "environment_const.h"
#include "setup_const.h"

#include "im.tab.h"
#include "imstructs.h"
#include "imclass.h"
#include "imdefaults.h"

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern volatile HUD_options  *G_HUD_options;
extern volatile float         G_filter_range;

extern InitManager *initman;
#ifndef NO_DVW
#include "environ_input.h"
#endif

#ifndef XMAXSCREEN
#define XMAXSCREEN 1279
#endif // XMAXSCREEN

void init_shared_info ( )
{
   int count;
   int count2;
   PIPE_DATA *pi;
   CHANNEL_DATA *chan;
   inventory_t *inv_pntr;
   STATIC_DATA *sdata = (STATIC_DATA *)G_static_data;
   DYNAMIC_DATA *ddata = (DYNAMIC_DATA *)G_dynamic_data;
   union query_data local;	// InitMan
 
   if ( setinvent () == -1 ) {
      cerr << "setinvent () ERROR." << endl;
      exit ( 1 );
   }

   // Initialize static ddata-> data
   initman->query(IM_TERRAIN_FILE, &local, GET);
   strncpy ( sdata->terrain_file, local.string, FILENAME_SIZE );
   initman->query(MODEL_FILE, &local, GET);
   strncpy ( sdata->model_file, local.string, FILENAME_SIZE );
   initman->query(LOCATION_FILE, &local, GET);
   strncpy ( sdata->location_file, local.string, FILENAME_SIZE );
   initman->query(SOUND_FILE, &local, GET);
   strncpy ( sdata->sound_file, local.string, FILENAME_SIZE );
   initman->query(NETWORK_FILE, &local, GET);
   strncpy ( sdata->network_file, local.string, FILENAME_SIZE );
   initman->query(FILTER_FILE, &local, GET);
   strncpy ( sdata->filter_file, local.string, FILENAME_SIZE );

   initman->query(TARGET_ENTITY, &local, GET);
   ddata->master.address.site = (u_short)local.ints[0];
   ddata->master.address.host = (u_short)local.ints[1];
   ddata->master.entity = (u_short)local.ints[2];

   initman->query(VEHICLE_DRIVEN, &local, GET);
   strncpy ( sdata->driven_file, local.string, FILENAME_SIZE );
   if ( !strncmp( sdata->driven_file, UAV_FILENAME, UAV_FILELENGTH ) )
      sdata->UAV = TRUE;

   initman->query(HUD_FONT_FILE, &local, GET);
   strncpy ( sdata->hud_font_file, local.string, FILENAME_SIZE );

   initman->query(HUD_LEVEL, &local, GET);
   if (!local.ints[0]) {
      G_HUD_options->hud_level = 0;
      G_HUD_options->veh_info = 0;
      G_HUD_options->misc_info = 0;
   }
   else {
      G_HUD_options->hud_level = ENABLE_RADAR;
      G_HUD_options->veh_info = G_HUD_options->misc_info = 1;
   }
   initman->query(INTRO_FONT_FILE, &local, GET);
   strncpy ( sdata->intro_font_file, local.string, FILENAME_SIZE );
   initman->query(LANDMARK_FILE, &local, GET);
   strncpy ( sdata->landmark_file, local.string, FILENAME_SIZE );
   initman->query(TRANSPORT_FILE, &local, GET);
   strncpy ( sdata->transport_file, local.string, FILENAME_SIZE );
   initman->query(RADAR_ICON_FILE, &local, GET);
   strncpy ( sdata->radar_icon_file, local.string, FILENAME_SIZE );
   initman->query(BLINKING_LIGHTS_FILE, &local, GET);
   strncpy ( sdata->blinking_lights_file, local.string, FILENAME_SIZE );
   initman->query(ROUND_WORLD_FILE, &local, GET);
   strncpy ( sdata->round_world_file, local.string, FILENAME_SIZE );
   initman->query(IDENTIFY_FILE, &local, GET);
   strncpy ( sdata->identify_file, local.string, FILENAME_SIZE );

   initman->query(SEARCH_PATH, &local, GET);
   strncpy ( sdata->search_path, local.string, MAX_SEARCH_PATH );

   initman->query(FLYBOX_PORT, &local, GET);
   strncpy ( sdata->flybox_port, local.string, PORTNAME_SIZE);

#ifndef NO_DVW
   strcpy ( sdata->dvw_file, D_DVW_FILE );
#endif

   sdata->flybox_exists = FALSE;
   initman->query(LEATHER_FLYBOX_PORT, &local, GET);
   strncpy ( sdata->leather_flybox_port, local.string, PORTNAME_SIZE );
   sdata->leather_flybox_exists = FALSE;

/* Change
// mcmillan
//   strcpy ( sdata->traker_port, D_TRAKER_PORT );
//   strcpy ( sdata->traker_file, D_TRAKER_FILE );
//   sdata->traker_exists = FALSE;
   strcpy ( sdata->hmd_fastrak_file, "" );
   sdata->hmd_unit = NULL;
*/
/*
   initman->query(KAFLIGHT_PORT, &local, GET);
   strncpy ( sdata->kaflight_port, local.string, PORTNAME_SIZE );
*/

   strcpy (sdata->kaflight_port, "NONE");

   initman->query(LW_RECEIVE_PORT, &local, GET);
   sdata->lwInit.r_port = local.ints[0];
   initman->query(LW_SEND_PORT, &local, GET);
   sdata->lwInit.s_port = local.ints[0];
   initman->query(LW_MACHINE_NAME, &local, GET);
   strncpy ( sdata->lwInit.name, local.string, 128);

//cerr << "sdata->lwInit.name=" << sdata->lwInit.name << endl;
//cerr << "sdata->lwInit.r_port=" << sdata->lwInit.r_port << endl;
//cerr << "sdata->lwInit.s_port=" << sdata->lwInit.s_port << endl;
 
   initman->query(RANGE_FILTER, &local, GET);
   G_filter_range = local.floats[0];
   

   initman->query(MARKING, &local, GET);
   strncpy ( sdata->marking, local.string, MARKINGS_LEN );

   sdata->num_radar_divs = 1;
   sdata->radar[0].min = 0;
   sdata->radar[0].max = MAX_VEHTYPES - 1;
   sdata->radar[0].icon_char = D_ICON_CHAR;

   sdata->kaflight_prompt = D_KAFLIGHT_PROMPT;
   sdata->num_pipes = 0;

   initman->query(PROCESS_MODE, &local, GET);
   switch (local.ints[0]) {
      case (APPCULLDRAW):
         sdata->process_mode = PFMP_APPCULLDRAW;
         break;
      case (APP_CULLDRAW):
         sdata->process_mode = PFMP_APP_CULLDRAW;
         break;
      case (APPCULL_DRAW):
         sdata->process_mode = PFMP_APPCULL_DRAW;
         break;
      case (APP_CULL_DRAW):
         sdata->process_mode = PFMP_APP_CULL_DRAW;
         break;
      case (IM_DEFAULT):
         sdata->process_mode = PFMP_DEFAULT;
         break;
      default:
         cout << "Illegal process mode.\n";
         sdata->process_mode = PFMP_DEFAULT;
   }

   initman->query(IM_FRAME_RATE, &local, GET);
   sdata->frame_rate = local.floats[0];

   initman->query(CPU_LOCK, &local, GET);
   sdata->cpu = local.ints[0];

   initman->query(PHASE, &local, GET);
   switch (local.ints[0]) {
      case (ALG_FREE):
         sdata->phase = PFPHASE_FREE_RUN;
         break;
      case (ALG_FLOAT):
         sdata->phase = PFPHASE_FLOAT;
         break;
      case (ALG_LOCK):
         sdata->phase = PFPHASE_LOCK;
         break;
      default:
         cout << "Illegal phase.\n";
         sdata->phase = D_PHASE;
   }

   while ( inv_pntr = getinvent () ) {
      if ( inv_pntr->inv_class == INV_GRAPHICS ) {
         if ( inv_pntr->inv_type == INV_RE ) {
           sdata->reality_graphics = TRUE;
         }
         else {
           sdata->reality_graphics = FALSE;
         }
      }
   }
   sdata->max_samples = 0;
   sdata->stereo_exists = FALSE;
   sdata->sball_exists = FALSE; // sbexists();

   sdata->fcs_exists = FALSE;
   initman->query(FCS_MP, &local, GET);
   sdata->fcs_mp = local.ints[0];
   initman->query(FCS_PORT, &local, GET);
   strncpy ( sdata->fcs_port, local.string, PORTNAME_SIZE);
   initman->query(FCS_PROMPT, &local, GET);
   sdata->fcs_prompt = local.ints[0];

   sdata->kaflight_exists = FALSE;
#ifndef NOBASICPANEL
   sdata->remote_exists = FALSE;
#endif
#ifndef NO_IPORT
   sdata->treadport_exists = FALSE;
   sdata->uniport_exists = FALSE;
   sdata->testport_exists = FALSE;
   initman->query(TESTPORT, &local, GET);
   strncpy ( sdata->testport_file, local.string, FILENAME_SIZE );
   initman->query(UNIPORT, &local, GET);
   strncpy ( sdata->uniport_file, local.string, FILENAME_SIZE );
   initman->query(TREADPORT, &local, GET);
   strncpy ( sdata->treadport_file, local.string, FILENAME_SIZE );
#endif // NO_IPORT

#ifndef NOUPPERBODY
   initman->query(UPPERBODY, &local, GET);
   strncpy ( sdata->upperbody_file, local.string, FILENAME_SIZE );
   sdata->upperbody_exists = FALSE;
#endif // NOUPPERBODY

   sdata->waypnt_fd = -1;
   sdata->pdupos_fd = -1;

   initman->query(WAYPOINTS, &local, GET);
   sdata->waypoints = local.ints[0];

   initman->query(ODT_MP, &local, GET);
   sdata->odt_mp = local.ints[0];

   initman->query(ODT_PORT, &local, GET);
   strncpy ( sdata->odt_port, local.string, 32);

   initman->query(PDUPOSITION, &local, GET);
   sdata->pdu_position = local.ints[0];

   initman->query(SCORING, &local, GET);
   sdata->scoring = local.ints[0];

   initman->query(PICTURES, &local, GET);
   sdata->pictures = local.ints[0];

   initman->query(STEALTH, &local, GET);
   sdata->stealth = local.ints[0];

   sdata->covert = FALSE;
   sdata->UAV = FALSE; 

   initman->query(IM_SOUND, &local, GET);
   sdata->sound = local.ints[0];

   initman->query(DYNAMIC_OBJS, &local, GET);
   sdata->load_dynamic_models = local.ints[0];

   initman->query(DIS_TIME_OUT, &local, GET);
   sdata->DIS_deactivate_delta = local.floats[0];
   initman->query(DIS_HEARTBEAT, &local, GET);
   sdata->DIS_heartbeat_delta = local.floats[0];
   initman->query(DIS_ROTDELTA, &local, GET);
   sdata->DIS_orientation_delta = local.floats[0];
   initman->query(DIS_POSDELTA, &local, GET);
   sdata->DIS_position_delta = local.floats[0];

   initman->query(NPS_PIC_DIR, &local, GET);
   strncpy ( sdata->pic_dir, local.string, IM_MAX_STRING_LEN);

#ifndef NOC2MEASURES
   initman->query(PO_CTDB_FILE, &local, GET);
   strncpy ( sdata->po_ctdb_file, local.string, FILENAME_SIZE );
   initman->query(IM_PO_EXERCISE, &local, GET);
   sdata->po_exercise = (unsigned char) local.ints[0];
   initman->query(IM_PO_DATABASE, &local, GET);
   sdata->po_database = (unsigned short) local.ints[0];
   initman->query(IM_PO_PORT, &local, GET);
   sdata->po_port = (unsigned short) local.ints[0];
#endif

#ifdef BIRD
   initman->query(BIRD_PORT, &local, GET);
   strncpy ( sdata->bird_port, local.string, PORTNAME_SIZE );
   initman->query(BIRD_CONTROL, &local, GET);
   ddata->bird_control = local.ints[0];
#endif

   // Initialize dynamic ddata-> data
   initman->query(DEAD_SWITCH, &local, GET);
   ddata->dead_switch = local.ints[0];
   
   initman->query(DIS_EXERCISE, &local, GET);
   ddata->exercise = (u_char) local.ints[0];

   initman->query(NOTIFY_LEVEL, &local, GET);
   switch (local.ints[0]) {
      case (IM_FATAL):
         ddata->notify_level = PFNFY_FATAL;
         break;
      case (WARN):
         ddata->notify_level = PFNFY_WARN;
         break;
      case (NOTICE):
         ddata->notify_level = PFNFY_NOTICE;
         break;
      case (INFO):
         ddata->notify_level = PFNFY_INFO;
         break;
      case (IM_DEBUG):
         ddata->notify_level = PFNFY_DEBUG;
         break;
      case (FPDEBUG):
         ddata->notify_level = PFNFY_FP_DEBUG;
      default:
         cout << "Illegal NOTIFY_LEVEL.\n";
         ddata->notify_level = PFNFY_FP_DEBUG;
   }

   initman->query(FOG, &local, GET);
   ddata->fog = local.ints[0];
   initman->query(FOG_TYPE, &local, GET);
   switch (local.ints[0]) {
      case FOG_VTX_LIN:
         ddata->fog_type = PFFOG_VTX_LIN;
         break;
      case FOG_VTX_EXP:
         ddata->fog_type = PFFOG_VTX_EXP;
         break;
      case FOG_VTX_EXP2:
         ddata->fog_type = PFFOG_VTX_EXP2;
         break;
      case FOG_PIX_LIN:
         ddata->fog_type = PFFOG_PIX_LIN;
         break;
      case FOG_PIX_EXP:
         ddata->fog_type = PFFOG_PIX_EXP;
         break;
      case FOG_PIX_EXP2:
         ddata->fog_type = PFFOG_PIX_EXP2;
         break;
      case FOG_PIX_SPLINE:
         ddata->fog_type = PFFOG_PIX_SPLINE;
         break;
      default:
         ddata->fog_type = PFFOG_VTX_LIN;
         break;
   }


   initman->query(IM_CONTROL, &local, GET);
   switch (local.ints[0]) {
      case (FCS):
         ddata->control= NPS_FCS;
         break;
      case (FLYBOXIN):
         ddata->control= NPS_FLYBOX;
         break;
      case (LEATHER_FLYBOX):
         ddata->control= NPS_LEATHER_FLYBOX;
         break;
      case (KAFLIGHT):
         ddata->control= NPS_KAFLIGHT;
         break;
      case (KEYBOARD):
         ddata->control= NPS_KEYBOARD;
         break;
      case (SPACEBALL):
         ddata->control= NPS_SPACEBALL;
         break;
      case (UNIPORT):
         ddata->control= NPS_UNIPORT;
         break;
      case (SHIP_PANEL):
         ddata->control= NPS_SHIP_PANEL;
         break;
      case (SUB_PANEL):
         ddata->control= NPS_SUB_PANEL;
         break;
      case (AUSA_ODT):
         ddata->control= NPS_ODT;
         break;
      default:
         cout << "System error: Illegal CONTROL option.\n";
   }

   ddata->tod = D_TIME_OF_DAY;
   ddata->clouds = D_CLOUDS;

   initman->query(STEREO, &local, GET);
   ddata->stereo = local.ints[0];

   initman->query(FS_SCALE, &local, GET);
   ddata->fs_scale = local.floats[0];


   ddata->camera = FALSE;
   ddata->camlock = FALSE;
   ddata->birdseye = FALSE;
   ddata->birdsdis = 30.0f;
   pfSetVec3 ( ddata->birdsview.xyz, 0.0f, 0.0f, 0.0f );
   pfSetVec3 ( ddata->birdsview.hpr, 0.0f, 0.0f, 0.0f );
   pfSetVec3 ( ddata->locked_view, 0.0f, 0.0f, 0.0f );

   initman->query(HMD, &local, GET);
   ddata->hmd = local.ints[0];

   ddata->paused = FALSE;
   ddata->tethered = FALSE;
   ddata->attach_dis = 30.0f;

   initman->query(MULTISAMPLE, &local, GET);
   ddata->multisample = local.ints[0];

   ddata->aspect = 1.0f;
   ddata->fovx = D_FOVX;
   ddata->fovy = -1.0f;
   pfSetVec2(ddata->mouse, 0.0f, 0.0f);
   ddata->originX = 0;
   ddata->originY = 0;
   ddata->number_of_textures = 0;
   ddata->hitveh = -1;
   ddata->init_event = INIT_PERFORMER;
   ddata->exit_flag = FALSE;
   ddata->new_vid = -1;
   ddata->new_vehicle = FALSE;
   ddata->transport = FALSE;

   initman->query(IM_BOUNDING_BOX, &local, GET);
   ddata->bounding_box.xmin = local.floats[0];
   ddata->bounding_box.xmax = local.floats[1];
   ddata->bounding_box.ymin = local.floats[2];
   ddata->bounding_box.ymax = local.floats[3];
   ddata->bounding_box.xsize = 
                  ddata->bounding_box.xmax - ddata->bounding_box.xmin;
   ddata->bounding_box.ysize = 
                   ddata->bounding_box.ymax - ddata->bounding_box.ymin;
   initman->query(TARGET_BOUNDING_BOX, &local, GET);
   ddata->use_bounding_boxes = local.ints[0];

   ddata->TDBoffsets[X] =0.0f;
   ddata->TDBoffsets[Y] =0.0f;
   ddata->dead = FALSE;
   ddata->cause_of_death = 0;
   ddata->target = -1;
   ddata->target_lock = NO_LOCK;
   ddata->targetting = FALSE;

   initman->query(FORCE, &local, GET);
   ddata->force = (ForceID) local.ints[0];

   ddata->early_warn = FALSE;
   ddata->visual_mode = FALSE;
   ddata->visual_active = FALSE;
   ddata->static_hits = 0;
   ddata->dynamic_hits = 0;
   ddata->bullets_fired = 0;
   ddata->missiles_fired = 0;
   ddata->bombs_fired = 0;

   initman->query(MULTICAST, &local, GET);
   ddata->net_mode = local.ints[0];

   ddata->specific = 0;
   ddata->pick_mode = D_PICK_MODE;
   ddata->last_selected_vid = -1;
   ddata->nearest_radar_vid_to_mouse = -1;

   initman->query(NETWORKING, &local, GET);
   if (local.ints[0] == TRUE)
      ddata->network_mode = NET_SEND_AND_RECEIVE;
   else
      ddata->network_mode = NET_OFF;

#ifndef NOHIRESNET
   initman->query(HIRES, &local, GET);
   switch (local.ints[0]) {
      case IM_NET_OFF:
// cerr << "NET_OFF" << endl;
         ddata->hr_network_mode = NET_OFF;
         break;
      case RECEIVE_ONLY:
// cerr << "NET_RECEIVE_ONLY" << endl;
         ddata->hr_network_mode = NET_RECEIVE_ONLY;
         break;
      case SEND_ONLY:
// cerr << "NET_SEND_ONLY" << endl;
         ddata->hr_network_mode = NET_SEND_ONLY;
         break;
      case SEND_AND_RECEIVE:
// cerr << "NET_SEND_AND_RECEIVE" << endl;
         ddata->hr_network_mode = NET_SEND_AND_RECEIVE;
         break;
      default:
// cerr << "default:NET_OFF" << endl;
         ddata->hr_network_mode = NET_OFF;
   }
#endif // NOHIRESNET
   ddata->hud_callback = NULL;
   ddata->identify = NULL;
#ifndef NO_DVW
   ddata->cloud_primitive_switch = 0;
#endif

#ifndef NOC2MEASURES
   ddata->HL = NULL;
#endif

/* Change
   ddata->window_size = WIN_NORMAL;
   ddata->dynamic_terrain = TRUE;
*/

   for ( count = 0; count < MAXPIPES; count++ )
      {
      pi = ddata->pipe + count;
      pi->active = FALSE;
      pi->pipe_ptr = NULL;

      pi->window.left = 0.0f;
      pi->window.bottom = 0.0f;
      initman->query(IM_WINDOW, &local, GET);
      switch (local.ints[0]) {
         case (SVGA):
            pi->window.right = SVGA_X;
            pi->window.top = SVGA_Y;
            ddata->window_size = WIN_SVGA;
            break;
         case (FULL):
            pi->window.right = XMAXSCREEN;
            pi->window.top = YMAXSCREEN;
            ddata->window_size = WIN_FULL;
            break;
         case (NTSC):
            pi->window.right = NTSC_X;
            pi->window.top = NTSC_Y;
            ddata->window_size = WIN_NTSC;
            break;
         case (PAL):
            pi->window.right = PAL_X;
            pi->window.top = PAL_Y;
            ddata->window_size = WIN_PAL;
            break;
         case (VIM):
            pi->window.right = 745.0;
            pi->window.top = 225.0;
            ddata->window_size = WIN_VIM;
            break;
         case (THREE_TV):
            pi->window.right = 1279.0;
            pi->window.top = 971.0;
            ddata->window_size = WIN_THREE_TV;
            break;
         case (THREE_BUNKER):
            pi->window.right = 1279.0;
            pi->window.top = 971.0;
            ddata->window_size = WIN_THREE_BUNKER;
            break;
         case (TWO_TPORT):
            pi->window.right = 639.0;
            pi->window.top = 971.0;
            ddata->window_size = WIN_TWO_TPORT;
            break;
         case (FS_MCO):
            pi->window.right = 639.0;
            pi->window.top = 959.0;
            ddata->window_size = WIN_FS_MCO;
            break;
         case (FS_ST_BUFFERS):
            pi->window.right = 639.0;
            pi->window.top = 479.0;
            ddata->window_size = WIN_FS_ST_BUFFERS;
            break;
         case (TWO_ADJACENT):
            pi->window.right = 1279.0;
            pi->window.top = 971.0;
            ddata->window_size = WIN_TWO_ADJACENT;
            break;
         case (NORMAL):
         default:
            pi->window.left = D_WINDOW_LEFT;
            pi->window.right = D_WINDOW_RIGHT;
            pi->window.bottom = D_WINDOW_BOTTOM;
            pi->window.top = D_WINDOW_TOP;
      }

      pi->num_channels = 0;
      for ( count2 = 0; count2 < MAXCHANPERPIPE; count2++ ) 
         {
         chan = pi->channel + count2;
         chan->active = FALSE;
         chan->chan_ptr = NULL;

         initman->query(VIEWPORT, &local, GET);
         chan->viewport.left = local.floats[0];
         chan->viewport.right= local.floats[1];
         chan->viewport.bottom = local.floats[2];
         chan->viewport.top = local.floats[3];

         initman->query(FOV, &local, GET);
         chan->field_of_view[X] = D_FOVX;
         chan->field_of_view[Y] = D_FOVY;

         initman->query(CLIP_PLANES, &local, GET);
         chan->clip_plane.near = local.floats[0];
         chan->clip_plane.far = local.floats[1];

         chan->earth_sky = NULL;

         initman->query(EARTH_SKY_MODE, &local, GET);
         switch (local.ints[0]) {
            case (FAST):
               chan->earth_sky_mode = PFES_FAST;
               break;
            case (TAG):
               chan->earth_sky_mode = PFES_TAG;
               break;
            case (SKY):
               chan->earth_sky_mode = PFES_SKY;
               break;
            case (SKY_GROUND):
               chan->earth_sky_mode = PFES_SKY_GRND;
               break;
            case (SKY_CLEAR):
               chan->earth_sky_mode = PFES_SKY_CLEAR;
               break;
            default:
               cout << "Illegal Earth-Sky Mode.\n";
         }

         initman->query(HORIZON_ANGLE, &local, GET);
         chan->horiz_angle = local.floats[0];

         chan->fog_ptr = NULL;
#ifndef NOSUB
         chan->waterfog_ptr = NULL;
#endif // NOSUB
         initman->query(FOG_NEAR, &local, GET);
         chan->fog_plane.near = local.floats[0];
         initman->query(FOG_FAR, &local, GET);
         chan->fog_plane.far = local.floats[0];
#ifndef NOSUB
         chan->waterfog_plane.near = D_FOG_NEAR;  
         chan->waterfog_plane.far = D_FOG_FAR; 
#endif // NOSUB

         initman->query(GROUND_HEIGHT, &local, GET);
         chan->ground_height = local.floats[0];

         chan->scene = NULL;
         if ( count2 == 0 )
            chan->eyeview = FORWARD_VIEW;
         else
            chan->eyeview = CHANNEL_OFF;

         if (ddata->TDBoverride) {	//put yourself in the middle
            chan->view.xyz[X] = ddata->bounding_box.xsize * 0.5f;
            chan->view.xyz[Y] = ddata->bounding_box.ysize * 0.5f;
            chan->view.xyz[Z] = 1000.0f;
         } else {
            initman->query(VIEW_XYZ, &local, GET);
            chan->view.xyz[X] = local.floats[0];
            chan->view.xyz[Y] = local.floats[1];
            chan->view.xyz[Z] = local.floats[2];
         }
         pfCopyVec3 ( sdata->init_location.xyz, chan->view.xyz );

         initman->query(VIEW_HPR, &local, GET);
         chan->view.hpr[HEADING] = local.floats[0];
         chan->view.hpr[PITCH] = local.floats[1];
         chan->view.hpr[ROLL] = local.floats[2];
         pfCopyVec3 ( sdata->init_location.hpr, chan->view.hpr );
         }
      }
}



int read_config_file ( char *filename )
{
   char lint_suppress[4];
   strncpy(lint_suppress, filename, 2);
   // Functionality replaced by Initialization Manager
   return 1;
}

int read_transport ( TRANSPORT_DATA &transport_data )
{
   /*
   ifstream trans_file ( transport_data.filename );
   char temp[80];
   int file_open;
   */
   pfCoord	next_trans;
   struct TP	im_transport_locs[MAX_TRANSPORT];
   int		num_locs;

   initman->get_transport_locs((struct TP**) im_transport_locs, &num_locs);

   for (int i = 0; i < num_locs; i++) {
      next_trans.xyz[X] = im_transport_locs[i].x;
      next_trans.xyz[Y] = im_transport_locs[i].y;
      next_trans.xyz[Z] = im_transport_locs[i].z;
      next_trans.hpr[HEADING] = im_transport_locs[i].heading;
      next_trans.hpr[PITCH] = im_transport_locs[i].pitch;
      next_trans.hpr[ROLL] = im_transport_locs[i].roll;
      transport_data.location[i] = next_trans;
      strcpy(transport_data.comment[i], " // obsolete");
   }
   transport_data.current_count = num_locs;

   return (transport_data.current_count);
}

int write_transport ( TRANSPORT_DATA &transport_data )
{
   int success = FALSE;
   ostream &trans_file = ( ((strcmp(transport_data.filename,"cerr")) == 0) ?
       (ostream &)cerr : (ostream &)ofstream(transport_data.filename) );

   if ( trans_file.good() ) {
      trans_file.precision(2);
      trans_file.setf(ios::showpoint);
      trans_file.setf(ios::fixed,ios::floatfield);
      for ( int i = 0; i < transport_data.current_count; i++ ) {
         trans_file << "LOCATION "
                    << transport_data.location[i].xyz[X] << " "
                    << transport_data.location[i].xyz[Y] << " "
                    << transport_data.location[i].xyz[Z] << " "
                    << transport_data.location[i].hpr[HEADING] << " "
                    << transport_data.location[i].hpr[PITCH] << " "
                    << transport_data.location[i].hpr[ROLL]
                    << transport_data.comment[i] << endl;
      }
      success = TRUE;

   }

   return success;
}

/* This is the old one
int write_transport ( TRANSPORT_DATA &transport_data )
{
   int success = FALSE;
   ostream &trans_file = ( ((strcmp(transport_data.filename,"cerr")) == 0) ?
       (ostream &)cerr : (ostream &)ofstream(transport_data.filename) );

   if ( trans_file.good() ) {
      for ( int i = 0; i < transport_data.current_count; i++ ) {
         trans_file << "LOCATION "
                    << transport_data.location[i].xyz[X] << " "
                    << transport_data.location[i].xyz[Y] << " "
                    << transport_data.location[i].xyz[Z] << " "
                    << transport_data.location[i].hpr[HEADING] << " "
                    << transport_data.location[i].hpr[PITCH] << " "
                    << transport_data.location[i].hpr[ROLL]
                    << transport_data.comment[i] << endl;
      }
      success = TRUE;

   }

   return success;
}
*/

int read_radar_icons ( RADAR_INFO *icon_data)
{
   RADAR_INFO *next_icon = icon_data;
   struct RI im_icons[MAX_RADAR_DIVS];
   int num_icons;

   initman->get_radar_icons((struct RI**) im_icons, &num_icons);

   for (int i = 0; i < num_icons; i++, next_icon++) {
      next_icon->min = im_icons[i].min;
      next_icon->max = im_icons[i].max;
      next_icon->icon_char = im_icons[i].ident;
   }

   return num_icons;
}

