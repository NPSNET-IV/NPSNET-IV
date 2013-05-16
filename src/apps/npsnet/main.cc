/*
 * Copyright (c) 1995,1996
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

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysmp.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <netdb.h>
#include <string.h>
#include <bstring.h>
#include <math.h>
#include <pf.h>
#include <pfdb/pfflt.h>
#include <pfdu.h>

#include "common_defs2.h"
#include "macros.h"
#include "manager.h"
#include "socket_struct.h"
#include "disnetlib.h"
#include "fltcallbacks.h"
#include "socket_lib.h"
#include "read_config.h"
#include "cpu.h"
#include "window.h"
#include "intro.h"
#include "cull.h"
#include "draw.h"
#include "terrain.h"
#include "entity.h"
#include "munitions.h"
#include "nps_smoke.h"
#include "environ_input.h"
#include "timeofday.h"
#include "effects.h"
#include "view.h"
#include "identify.h"
#include "sound.h"
#include "netutil.h"
#include "dbPagingCallback.h"
#include "dbDeletePageClass.h"
#include "tsg.h"
#include "tsgpf.h"

//#include "traker.h"   // mcmillan
#include "hmdClass.h"

#include "DI_comm.h"
// #include "motion.h"
#include "appearance.h"
#ifndef NODUDE
#include "dude.h"
#include "filter.h"
#include "jade.h"
#endif // NODUDE
#include "input_manager.h"
#include "vehicle.h"
#include "diguy_veh.h"
#include "nps_input.h"
#include "time_funcs.h"
#ifndef NOSUB
#include "lake.h"
#endif // NOSUB

// Initialization Manager
#include "im.tab.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "imclass.h"

#ifndef NOC2MEASURES
#include "po_interface.h"
#endif

#ifdef JACK
#include "jack.h"
#include "jacksoldier.h"
#include "jack_di_veh.h"
#endif


#ifdef VABS
#define GLOBAL
#include "vabs.h"
#undef GLOBAL
#endif

#include "interface_const.h"
#include "local_snd_code.h"
#include "display_const.h"
#include "collision_const.h"
#include "communication_const.h"
#include "conversion_const.h"

#ifndef NOAHA
#include "aha.h"
#endif

#ifdef BIRD
#include "birdlib.h"
#endif

#ifdef OPENGL
#define XMAXSCREEN              1279
#define YMAXSCREEN              1023
#endif

#ifndef NO_DVW
#define NO_PFSMOKE
#define NO_NPSSMOKE
#define DVW_NPSNET
#define _CSSM
#define _DVW_VERBOSE_
#define _DVW_MULTI_TASKS_
#define _CC_COMPILER_
#define PERFORMER1_2
#include "cloud.h"
#include "environ_input.h"
extern "C" processAtmosphericModels(const pfChannel *);
extern "C" void initializeAtmosphericModels(void*);
extern "C" void setHazeTimeOfDay(float);
extern "C" void init_vis_cloud(void); 
extern "C" void initializeFlareLights(void);

#endif

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#endif

#define MAX_TEXTURES 2000


// ***************   GLOBAL VARIABLES  ***************************
GUI_MSG_DATA *G_remote;
float G_filter_range;
DIS_net_manager *G_net;

#ifndef NOHIRESNET
DIS_net_manager *G_hr_net;
#endif // NOHIRESNET

volatile DYNAMIC_DATA *G_dynamic_data;
volatile STATIC_DATA *G_static_data;
volatile SUN *G_sun;
volatile HUD_options *G_HUD_options;
pfGroup *G_fixed;
pfGroup *G_moving;
pfGroup *G_measures;
pfGroup *G_animations;
pfGroup *G_logo;
volatile int G_drivenveh = 0;
pfLight *G_logo_light;

//          int               G_mod_num;
VEH_LIST *G_vehlist;
VEHTYPE_LIST *G_vehtype;
OBJTYPE_LIST *G_objtype;
WEAPON_LIST *G_weaplist;
volatile float G_curtime;
EntityID G_my_entity_id;
volatile MANAGER_REC *G_manager = NULL;
volatile DB_SHARE *dbShare = (DB_SHARE *)NULL;

#ifdef DATALOCK
pfDataPool *G_dynamic_pool;
#endif

#ifdef JACK
short *G_JackSoldier_active;
int G_num_soldiers = 0;
int G_tot_soldiers = 0;
int G_runjack = FALSE;
int G_runisms = FALSE;
int G_slow_speed_increment;
#endif

#ifndef NO_DVW
void *arena;
CHANNEL_DATA *viewChan;
#endif

pfEarthSky *earthSky;

// ***************   GLOBAL VARIABLES  ***************************


// static char trak_port[512]; // mcmillan

char *myopts[] =
{
#define DIS_HB		0
   "heartbeat",
#define DIS_TO		1
   "timeout",
#define DIS_RD		2
   "rotdelta",
#define DIS_PD		3
   "posdelta",
#define DIS_ID		4
   "exercise",
#define WIN_WIDTH	5
   "width",
#define WIN_X		6
   "x",
#define WIN_HEIGHT	7
   "height",
#define WIN_Y		8
   "y",
#define MAPXMIN		9
   "xsw",
#define MAPYMIN		10
   "ysw",
#define MAPXMAX		11
   "xne",
#define MAPYMAX		12
   "yne",
#define MAPFILE		13
   "file",
#ifndef NOC2MEASURES
#define PO_CTDB		14
   "poctdb",
#define PO_EXERCISE	15
   "poexercise",
#define PO_DATABASE	16
   "podatabase",
#define PO_PORT		17
   "poport",
#endif
   NULL};

InitManager *initman;

void main(int argc, char **argv)
{
   cerr << endl << "Running: " << EXACT_VERSION << endl;
  
   static int hours_expired;
   static int minutes_expired;
   static int seconds_expired;
   int esc_counter;
   int load_animations;
   int counter;
   int db_paging = FALSE;

#ifndef NOBASICPANEL
   int use_basic_panel;
#endif

#ifdef JACK
   extern int JackInit(char *datadir);
   char *jack_dir;
#endif

// int op = 0;                  // command line option

// char *options, *value;
   extern char *optarg;
   extern int optind, operr;

   CHANNEL_DATA *chan;
   PASS_DATA *draw_data;
   INFO_PDATA *info_pdata;
   //char *config_file = NULL;
   int camera;

#ifndef NOC2MEASURES
   int po_is_up = FALSE;
#endif

   CONTROL control_data;
   char ether_interf[10];

#ifdef NO_DVW
   void *arena;
#endif

   int net_is_open;

#ifndef NOHIRESNET
   int hr_net_is_open;
#endif

   NPS_Input_Device old_control;
   int exit_loop = 0;
   int nostatic;

   float target_ok;
   float posture_chg_ok;

   char driven_file[50];

   // Multicast Defaults
   //char DEFAULT_GROUP[] = "224.2.121.93";
   u_short port;
   char group[STRING32];
   u_char ttl;
   int multicast;

   float LOD_scale, LOD_fade;


#ifndef NOHIRESNET
   char HR_DEFAULT_GROUP[] = "224.252.252.5"; // "224.2.121.97";
   u_short hr_port = 0;
   char *hr_group;
   u_char hr_ttl = 63; // DEFAULT_MC_TTL;
   int hr_multicast = TRUE; //FALSE;
   hr_group = HR_DEFAULT_GROUP;
#endif // NOHIRESNET

   pfDCS *logo_dcs = NULL;
   float logo_heading = 0.0f;
   pfSphere logo_sphere;



   // QUERY INIT MAN FOR THIS!!
   int aim_view_enabled = TRUE;

   // START Initialization Manager routines
   union query_data local;
   initman = new InitManager();		// defaults assigned
   initman->process_Master_Configs(argc, argv);
   initman->process_environment_vars();
   initman->process_cl_args(argc, argv);
   initman->parse_input_files();
   initman->validate_data();
   initman->dump_inits();       // let's keep a record of what we've got
   // END Initialization Manager routines

   cerr << "Initializing...\n";

//   initman->query(TRAKER_PORT, &local, GET);
//   strncpy ( trak_port, local.string, IM_MAX_STRING_LEN );

   initman->query(IM_WEAPON_VIEW, &local, GET);
   aim_view_enabled = local.ints[0];

   initman->query(ETHER_INTERF, &local, GET);
   strncpy ( ether_interf, local.string, 10 );

   initman->query(STATIC_OBJS, &local, GET);
   nostatic = !(local.ints[0]);

   initman->query(IM_PORT, &local, GET);
   port = (u_short) local.ints[0];

   initman->query(MULTICAST, &local, GET);
   multicast = local.ints[0];
   initman->query(MULTICAST_TTL, &local, GET);
   ttl = (u_char) local.ints[0];
   initman->query(MULTICAST_GROUP, &local, GET);
   strncpy ( group, local.string, STRING32 );

   initman->query(ANIMATIONS, &local, GET);
   load_animations = local.ints[0];

#ifndef NOBASICPANEL
   initman->query(REMOTE_INPUT_PANEL, &local, GET);
   use_basic_panel = local.ints[0];
#endif

   /* Initialize performer */
   pfInit();
   pfuInitUtil();

   //zero out the clock
   pfInitClock(0);


#ifdef JACK
   char temp_jack_dir[256];

   strcpy(temp_jack_dir, "./otherlibs/Jack");
   jack_dir = &temp_jack_dir[0];
   initman->query(IM_JACK, &local, GET);
   G_runjack = local.ints[0];
   if (G_runjack) {
      cerr << "     Will be using Jack to animate humans\n";
   }
#endif

   // so load is linked in
   pfdInitConverter (".flt");

   initman->query(VEHICLE_DRIVEN, &local, GET);
   strncpy ( driven_file, local.string, 50 );

#ifdef DATALOCK

   // Create the shared stucture for data that can change during execution.
   // Creating as a Performer data poll allows locking for mutual exclusion.
   G_dynamic_pool = pfCreateDPool(sizeof(DYNAMIC_DATA), DYNAMIC_POOL_FILE);
   G_dynamic_data = (volatile DYNAMIC_DATA *) pfDPoolAlloc(G_dynamic_pool,
                                             sizeof(DYNAMIC_DATA), 1);
   bzero((char *) G_dynamic_data, sizeof(G_dynamic_data));
#endif

   // Create a shared arena for unchanging data sharing.  My mutual exclusion
   // so the data should not be modified once set.
   arena = pfGetSharedArena();

   dbShare = (volatile DB_SHARE *) pfCalloc(sizeof(DB_SHARE), 1, arena);
   dbShare->fixed = NULL;
   dbShare->x = 0.0f;
   dbShare->y = 0.0f;
   dbShare->h = 0.0f;
   dbShare->okToDownLoadTextures = FALSE;
   dbShare->tsgInfo = (tsgContext *) NULL;
   dbShare->dbPageBin = (dbDeletePage *) NULL;

   G_static_data = (volatile STATIC_DATA *)
      pfCalloc(sizeof(STATIC_DATA), 1, arena);

   G_dynamic_data = (volatile DYNAMIC_DATA *)
      pfCalloc(sizeof(DYNAMIC_DATA), 1, arena);
   if (G_dynamic_data == NULL) {
      cerr << "Unable to allocate enough memory for dynamic data.\n";
      exit(-1);
   }
   else
      bzero((char *) G_dynamic_data, sizeof(G_dynamic_data));

   // Allocate the Performer light structure for the Sun - global illumination
   G_sun = (SUN *) pfMalloc(sizeof(SUN), arena);
   G_sun->lt = pfNewLight(arena);
   G_sun->lm = pfNewLModel(arena);

   G_manager = (volatile MANAGER_REC *)
      pfCalloc(sizeof(MANAGER_REC), 1, arena);
   if (G_manager == NULL) {
      pfNotify(PFNFY_FATAL, PFNFY_PRINT,
               "Unable to allocate manager structure from arena.");
      exit(-1);
   }
   else {
      G_manager->ofinput = NULL;
   }

   G_HUD_options = (volatile HUD_options *)
      pfCalloc(sizeof(HUD_options), 1, arena);
   if (G_HUD_options == NULL) {
      cerr << "ERROR:\tUnable to allocate shared memeory from arena" << endl;
      cerr << "\tfor the HUD data." << endl;
      exit(-1);
   }

   /* Initialize main data structures to contain default values. */
   init_shared_info();

   if (strcasecmp((const char *)G_static_data->network_file, "NONE") != 0) {
      cerr << "     Initializing hosts..." << endl;
      get_im_hosts();
   }

   initman->query(FS_PORT,  &local, GET);
   strncpy ((char *)G_static_data->fsInit.tty, local.string, 32);
   initman->query(FS_CONFIG,  &local, GET);
   strncpy ((char *)G_static_data->fsInit.calfile, local.string, 128);

#ifndef NO_DVW
   char *filePaths[10] = {0};
   setDVWEnvPaths(filePaths);

   initializeAtmosphericModels(arena);
   init_vis_cloud();

#endif


   // Define level of notification desired for Performer error, warning and
   // debugging statements.
   pfNotifyLevel ( G_dynamic_data->notify_level );

   /* Set the file search path using the data from the configuration files */
   pfFilePath ( (char *)G_static_data->search_path );

   pfNotify ( PFNFY_NOTICE, PFNFY_PRINT,
              "The Performer file search path is:" );
   char debug_search_path[MAX_SEARCH_PATH];
   char *next_search_path;
   strcpy ( debug_search_path, (char *)G_static_data->search_path );
   next_search_path = strtok(debug_search_path,":");
   while ( next_search_path != NULL )
      {
      pfNotify ( PFNFY_NOTICE, PFNFY_PRINT,
                 "\t%s", next_search_path );
      next_search_path = strtok(NULL,":");
      } 

   /* Initialize MultiGen Flight Loader Callback function */
   init_flt_loader();
   // registerNode = myregisterfunc;
   fltRegisterNodeT pFunc =  myregisterfunc;
   void *hFunc            = &pFunc;
   pfdConverterAttr_flt (PFFLT_REGISTER_NODE, hFunc);

   if (strcmp(driven_file,""))
      {
      strcpy ( (char *)G_static_data->driven_file, driven_file );
      if ( !strncmp((char *)G_static_data->driven_file,
                    UAV_FILENAME,UAV_FILELENGTH) )
         G_static_data->UAV = TRUE;
      if ( !strcmp((char *)G_static_data->driven_file,"stealth") )
         G_static_data->stealth = TRUE;
      }

   if ( G_static_data->stealth )
      cerr << "     Stealth mode requested.\n";

   /* Read the host table and figure out which interface to use */

#ifdef TRACE
cerr << "ether_interf=" << ether_interf << endl;
#endif

   if (!G_dynamic_data->exit_flag)
      {
      char temp_interf[255];
      strcpy (temp_interf, ether_interf);
      findsitehost( ether_interf, (char *)G_static_data->hostname,
                    (SimulationAddress *)&G_static_data->DIS_address);
#ifdef TRACE
cerr << "after findsitehost\nether_interf=" << ether_interf << endl;
#endif
      if ( (strcmp(temp_interf,"NONE")) != 0 )
         strcpy ( ether_interf, temp_interf );
#ifdef TRACE
cerr << "after strcmp\nether_interf=" << ether_interf << endl;
#endif
      }


   G_static_data->num_radar_divs = 
      read_radar_icons ( (RADAR_INFO *)G_static_data->radar );

   /* Select the multiprocessing mode for Performer to use */
   initman->query (DB_TSG_FILE, &local, GET);
   if ((strcmp(local.string,"NONE")) == 0 ) {
      initman->query (DB_PAGING, &local, GET);
      db_paging = local.ints[0];
   }
   else {
      db_paging = TRUE;
   }
   if (db_paging) {
      initman->query (DB_OFFSET, &local, GET);

      dbShare->offset = local.ints[0];

      initman->query (DB_PAGES_ALLOWED, &local, GET);

      dbShare->numberAllowed = local.ints[0];
      
      if (G_static_data->process_mode == PFMP_DEFAULT) {
         cerr << "     PFMP_DEFAULT simply won't do." 
              << " Picking best MP mode..." << endl;
         long number_of_CPUs = sysmp (MP_NPROCS);

         if (number_of_CPUs == 1) {
            cerr << "\tOne processor machine. using: PFMP_APPCULLDRAW"
                 << endl;
            G_static_data->process_mode = PFMP_APPCULLDRAW;
         }
         else if (number_of_CPUs == 2) {
            cerr << "\tTwo processor machine, using: PFMP_APPCULL_DRAW"
                 << endl;
            G_static_data->process_mode = PFMP_APPCULL_DRAW;
         }
         else if (number_of_CPUs >= 3) {
            cerr << "\tMulti processor machine, using: PFMP_APP_CULL_DRAW"
                 << endl;
            G_static_data->process_mode = PFMP_APP_CULL_DRAW;
         }
         else {
            cerr << "\tUnknown CPU configuration, using: PFMP_APPCULLDRAW" 
                 << endl;
            G_static_data->process_mode = PFMP_APPCULLDRAW;
         }
      }

      G_static_data->process_mode |= PFMP_FORK_DBASE;

   }

   pfMultiprocess (G_static_data->process_mode);

   if (G_static_data->cpu) {
      npsFreeCPUs();
   }

   initman->query(HMD, &local, GET);
   if (local.ints[0]) {
      initman->query(HMD_FILE, &local, GET);
      if ( strcmp (local.string, "NONE") ) {
         char hmd_fastrak_file[50];
         strncpy ( hmd_fastrak_file, local.string, 50 );
         cerr << "Opening fastrak connection for HMD (~15 seconds)..."
            << endl;
         G_static_data->hmd_unit =
            new hmdClass(hmd_fastrak_file);

         if (G_static_data->hmd_unit->exists()) {
            G_dynamic_data->camera = 1;
         }
         else {
            delete G_static_data->hmd_unit;

            G_static_data->hmd_unit = NULL;
         }
      }
   }

#ifdef BIRD
   // BIRD, by Jiang Zhu
   if (!strcmp((char *) G_static_data->bird_port, "NONE"))
      G_static_data->bird_exists = FALSE;
   else
      G_static_data->bird_exists =
         initialize_bird((char *) G_static_data->bird_port);

   if (!G_static_data->bird_exists)
      G_dynamic_data->bird_control = FALSE;
#endif

#ifndef NOAHA
   // AHA, by Jiang Zhu
   ahaConfig();
#endif

#ifndef NO_PFSMOKE
   // If the pfu smoke stuff is used, it MUST be init'd before pfConfig
   cerr << "     Initializing PFU smoke..." << endl;
   /*initialized fire for 20 plumes blowing true north at 2 mps */
   for (int num_colors = 0; num_colors < MAX_SMOKE_COLORS; num_colors++)
      initialize_fire(5, 0.0f, 2.0f, num_colors);
#endif

   // Configure the performer environment, which also splits off any
   // processes for application, cull and draw.


   pfConfig();


   pfNotify(PFNFY_NOTICE, PFNFY_PRINT,
            "Application Process PID is: %d\n", getpid());

   G_static_data->process_mode = pfGetMultiprocess();

   pfNotify(PFNFY_NOTICE, PFNFY_PRINT,
            "Using multiprocess mode:  ");

   switch (G_static_data->process_mode & PFMP_APP_CULL_DRAW) {
      case PFMP_APPCULLDRAW:
         pfNotify(PFNFY_NOTICE, PFNFY_PRINT,
                  "\t1:Application,Cull,Draw.");
         break;
      case PFMP_APP_CULLDRAW:
         pfNotify(PFNFY_NOTICE, PFNFY_PRINT,
                  "\t1:Application 2:Cull,Draw.");
         break;
      case PFMP_APPCULL_DRAW:
         pfNotify(PFNFY_NOTICE, PFNFY_PRINT,
                  "\t1:Application,Cull 2:Draw.");
         break;
      case PFMP_APP_CULL_DRAW:
         pfNotify(PFNFY_NOTICE, PFNFY_PRINT,
                  "\t1:Application 2:Cull 3:Draw.");
         break;
      default:
         pfNotify(PFNFY_WARN, PFNFY_PRINT,
                  "\tUnknown multiprocessing mode.");
   }

   if (G_static_data->reality_graphics)
      pfNotify(PFNFY_INFO, PFNFY_PRINT,
               "Reality Engine graphics are available.\n");
   else
      pfNotify(PFNFY_INFO, PFNFY_PRINT,
               "Reality Engine graphics are NOT available.\n");


   // Create the new input manager to handle all input for the system
   G_manager->ofinput = new input_manager();
   open_input_devices(use_basic_panel, ether_interf);


   G_dynamic_data->identify =
      new IDENTIFY((char *) G_static_data->identify_file);

   if (G_static_data->cpu) {
      if (!npsLockCPU(1, "Application"))
         G_static_data->cpu = -1;
   }

   if (G_static_data->pdu_position) {
      umask(000);
      G_static_data->pdupos_fd = creat("/usr/tmp/NPSNETIV_pduposture", 00666);
      if (G_static_data->pdupos_fd < 0)
         cerr << "     PDU posture file, /usr/tmp/NPSNETIV_pduposture, could "
            << "not be created.\n";
      else
         write(G_static_data->pdupos_fd,
               "#   X        Y        Z    HEADING   PITCH    ROLL      TIMESTAMP\n",
               66);
   }

   if (G_dynamic_data->hmd && G_static_data->reality_graphics) {
      char temp_enter_string[20];

      cerr << endl << "****\tPutting the system into VIM output mode --"
         << endl;
      cerr << "\t\tPress <Enter> when ready...";
      cin.getline(temp_enter_string, 20);
      cerr << endl;
      system("/usr/gfx/setmon -n -sg -v 745x224_60");
   }

   /* Open Performer graphics pipelines. */
   G_dynamic_data->pipe[0].active = TRUE;
   G_dynamic_data->pipe[0].pipe_ptr = pfGetPipe(0);

   ifstream input_test_name;
   int file_exists;

   input_test_name.open((char *) G_static_data->hud_font_file, ios::in);
   file_exists = input_test_name.good();
   if (file_exists)
      input_test_name.close();
   input_test_name.clear();
   if (!file_exists) {
      cerr << "ERROR:\tHead's-Up Display (HUD) font file does not exists."
         << endl;
      cerr << "\tEdit your configuration file(s) to add a HUD_FONT_FILE entry."
         << endl;
      exit(0);
   }

   chan = (CHANNEL_DATA *) G_dynamic_data->pipe[0].channel;

   if ((G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
       (G_dynamic_data->window_size == WIN_TWO_ADJACENT) ||
       (G_dynamic_data->window_size == WIN_THREE_TV)) {
      chan->viewport.left = 0.5f;
      chan->viewport.right = 1.0f;
      chan->viewport.bottom = 0.0f;
      chan->viewport.top = 0.5f;
   }
   else if (G_dynamic_data->window_size == WIN_TWO_TPORT) {
      chan->viewport.left = 0.0f;
      chan->viewport.right = 1.0f;
      chan->viewport.bottom = 0.0f;
      chan->viewport.top = 0.5f;
   }
   else if ( G_dynamic_data->window_size == WIN_FS_MCO ) {
      chan->viewport.left = 0.0f;
      chan->viewport.right = 1.0f;
      chan->viewport.bottom = 0.0f;
      chan->viewport.top = 0.5f;
   }


   G_dynamic_data->init_event = INIT_PERFORMER;
   cerr << "     Initializing Performer...\n";

   /* Initialize the pipeline which opens a window for the pipeline */
   /* to draw into.                                                 */

   G_dynamic_data->pw = pfNewPWin(G_dynamic_data->pipe[0].pipe_ptr);

   pfPWinConfigFunc(G_dynamic_data->pw,open_pipe_line);
   pfConfigPWin (G_dynamic_data->pw);


   Display *theDisplay = pfGetCurWSConnection ();

   int timeOutSetting, interval, preferBlanking, allowExposure;

   XGetScreenSaver (theDisplay, &timeOutSetting, &interval,
                    &preferBlanking, &allowExposure);

   XSetScreenSaver (theDisplay, 0, interval,
                    preferBlanking, allowExposure);

   if (G_static_data->sound) {

      G_dynamic_data->init_event = LOADING_SOUNDS;
      cerr << "     Loading sounds...\n";
      if (!(G_static_data->process_mode & PFMP_FORK_DRAW)) {
         //print_next_event(); -4/10/96
      }

      if ( sound_capable () )
         {
         //init_sounds ( G_static_data->sound_file, (char *)pfGetFilePath() );
         init_sounds( (char *)pfGetFilePath() );
         cerr << "     Sound enabled." << endl;
      }
      else {
         cerr << "WARNING:\tSound requested but cannot work on "
            << "this machine." << endl;
         G_static_data->sound = FALSE;
      }
   }
   if (G_static_data->sound) {
      LocationRec dummy_snd_loc;

      dummy_snd_loc.x = 0.0;
      dummy_snd_loc.y = 0.0;
      dummy_snd_loc.z = 0.0;
      playsound(&dummy_snd_loc, &dummy_snd_loc, STARTUP_INDEX, -1);
   }



   /* Select the timing and synchronization model desired for Performer. */
   pfPhase(G_static_data->phase);
   pfFrameRate(G_static_data->frame_rate);


   /* Open a Performer graphics channel for rendering the scene. */
   chan = (CHANNEL_DATA *) G_dynamic_data->pipe[0].channel;
   chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);
   pfChanShare(chan->chan_ptr,
               PFCHAN_FOV | PFCHAN_NEARFAR | PFCHAN_SCENE |
               PFCHAN_EARTHSKY | PFCHAN_STRESS | PFCHAN_LOD |
               PFCHAN_SWAPBUFFERS | PFCHAN_CULLFUNC);
   pfChanTravFunc(chan->chan_ptr, PFTRAV_CULL, CullChannel);
   chan->eyeview = FORWARD_VIEW;

   pfVec3 hprOffsets;
   pfVec3 xyzOffsets;

   pfSetVec3(hprOffsets, 0.0f, 0.0f, 0.0f);
   pfSetVec3(xyzOffsets, 0.0f, 0.0f, 0.0f);

   //pfChanViewOffsets ( chan->chan_ptr, xyzOffsets, hprOffsets );

#ifndef NOAHA
   // AHA, by Jiang Zhu
   ahaInit(chan->chan_ptr);
   switch (G_dynamic_data->pick_mode) {
      case TWO_D_RADAR_INFO:
      case TWO_D_RADAR_PICK:
         ahaSetMode(AHA_OFF);
         break;
      case TWO_D_AHA_INFO:
      case TWO_D_AHA_PICK:
         ahaSetMode(AHA_TWO_D);
         break;
      case THREE_D_AHA_INFO:
      case THREE_D_AHA_PICK:
         ahaSetMode(AHA_THREE_D);
         break;
      default:
         ahaSetMode(AHA_OFF);
         break;
   }
#endif

/***************************************************************************/

   if ((G_dynamic_data->window_size != WIN_THREE_BUNKER) &&
       (G_dynamic_data->window_size != WIN_TWO_TPORT) &&
       (G_dynamic_data->window_size != WIN_FS_MCO) &&
       (G_dynamic_data->window_size != WIN_FS_ST_BUFFERS) &&
       (G_dynamic_data->window_size != WIN_TWO_ADJACENT) &&
       (G_dynamic_data->window_size != WIN_THREE_TV)) {

      /*Open a channel for rendering video missiles */
      chan += 1;
      chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);

      // Attach the two channels to they share most characteristics
      pfAttachChan(G_dynamic_data->pipe[0].channel[0].chan_ptr,
                   chan->chan_ptr);
      pfChanShare(chan->chan_ptr, PFCHAN_NEARFAR | PFCHAN_SCENE | 
                  PFCHAN_EARTHSKY | PFCHAN_LOD | PFCHAN_SWAPBUFFERS);

      // Turn default drawing of video missile window off initially
      pfChanTravMode(chan->chan_ptr, PFTRAV_DRAW, PFDRAW_OFF);

      // Set viewport to be upper left 1/3 of screen for video missile display
      pfChanViewport(chan->chan_ptr,
                     G_dynamic_data->pipe[0].channel[0].viewport.left,
             0.33 * G_dynamic_data->pipe[0].channel[0].viewport.right,
               0.67 * G_dynamic_data->pipe[0].channel[0].viewport.top,
                     G_dynamic_data->pipe[0].channel[0].viewport.top);

      // Set the field-of-view to 45 degrees for video missile
      pfChanFOV(chan->chan_ptr, 45.0f, -1.0f);

      chan->eyeview = MUNITION_VIEW;

   }
/***************************************************************************/

   /* Open a channel for rendering DIS information about a selected entity */

   if ((G_dynamic_data->window_size != WIN_THREE_BUNKER) &&
       (G_dynamic_data->window_size != WIN_TWO_TPORT) &&
       (G_dynamic_data->window_size != WIN_FS_MCO) &&
       (G_dynamic_data->window_size != WIN_FS_ST_BUFFERS) &&
       (G_dynamic_data->window_size != WIN_TWO_ADJACENT) &&
       (G_dynamic_data->window_size != WIN_THREE_TV)) {
      chan += 1;
      chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);

      // Turn default drawing of entity information window off initially
      pfChanTravMode(chan->chan_ptr, PFTRAV_DRAW, PFDRAW_OFF);

      float win_width = (G_dynamic_data->pipe[0].channel[0].viewport.right -
                    G_dynamic_data->pipe[0].channel[0].viewport.left);
      float win_height = (G_dynamic_data->pipe[0].channel[0].viewport.top -
                  G_dynamic_data->pipe[0].channel[0].viewport.bottom);
      float win_temp = 0.333f * win_height;
      float win_aspect = YMAXSCREEN / float (XMAXSCREEN);

      pfChanViewport(chan->chan_ptr,
                  (G_dynamic_data->pipe[0].channel[0].viewport.right -
                   (win_temp * win_aspect * (win_height / win_width))),
                     G_dynamic_data->pipe[0].channel[0].viewport.right,
                     (G_dynamic_data->pipe[0].channel[0].viewport.top -
                      win_temp),
                     G_dynamic_data->pipe[0].channel[0].viewport.top);

      // Set the field-of-view to 45 degrees for the information channel
      pfChanFOV(chan->chan_ptr, 45.0f, -1.0f);

      chan->eyeview = INFORMATION_VIEW;

      /* Define the draw callback function for our information channel */
      pfChanTravFunc(chan->chan_ptr, PFTRAV_DRAW, draw_information);

      /* Create the pass data from passing time relevant infomation down */
      /* the multiprocessing pipeline from application to cull to draw.  */
      info_pdata = (INFO_PDATA *) pfAllocChanData(chan->chan_ptr,
                                                  sizeof(INFO_PDATA));
      info_pdata->veh_index = -1;
   }
/***************************************************************************/

   if ((G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
       (G_dynamic_data->window_size == WIN_THREE_TV)) {

      /* Open a channel for the left monitor view */
      chan += 1;
      chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);

      // Attach the two channels to they share most characteristics
      pfAttachChan(G_dynamic_data->pipe[0].channel[0].chan_ptr,
                   chan->chan_ptr);

      pfChanTravFunc(chan->chan_ptr, PFTRAV_DRAW, basic_draw_left);

      // Set viewport to be bottom left quarter of screen
      pfChanViewport(chan->chan_ptr, 0.0f, 0.50f, 0.0f, 0.50f);

      chan->eyeview = FORWARD_VIEW;
   }

/***************************************************************************/

   if ((G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
       (G_dynamic_data->window_size == WIN_THREE_TV)) {

      /* Open a channel for the right monitor view */
      chan += 1;
      chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);

      // Attach the two channels to they share most characteristics
      pfAttachChan(G_dynamic_data->pipe[0].channel[0].chan_ptr,
                   chan->chan_ptr);

      pfChanTravFunc(chan->chan_ptr, PFTRAV_DRAW, basic_draw_right);

      // Set viewport to be upper right quarter of screen
      pfChanViewport(chan->chan_ptr, 0.50f, 1.0f, 0.50f, 1.0f);

      chan->eyeview = FORWARD_VIEW;
   }

/***************************************************************************/

   if (G_dynamic_data->window_size == WIN_TWO_TPORT) {

      /* Open a channel for the right monitor view */
      chan += 1;
      chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);

      pfChanTravFunc(chan->chan_ptr, PFTRAV_DRAW, basic_draw);

      // Attach the two channels to they share most characteristics
      pfAttachChan(G_dynamic_data->pipe[0].channel[0].chan_ptr,
                   chan->chan_ptr);
      pfChanShare(chan->chan_ptr, PFCHAN_NEARFAR | PFCHAN_SCENE |
                  PFCHAN_EARTHSKY | PFCHAN_FOV |
                  PFCHAN_LOD | PFCHAN_SWAPBUFFERS_HW);

      // Set viewport to be upper half of screen
      pfChanViewport(chan->chan_ptr, 0.0f, 1.0f, 0.5f, 1.0f);

      chan->eyeview = FORWARD_VIEW;
   }

/***************************************************************************/

   if (G_dynamic_data->window_size == WIN_FS_MCO) {

      /* Open a channel for the right monitor view */
      chan += 1;
      chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);

      pfChanTravFunc(chan->chan_ptr, PFTRAV_DRAW, basic_draw);

      // Attach the two channels to they share most characteristics
      pfAttachChan(G_dynamic_data->pipe[0].channel[0].chan_ptr,
                   chan->chan_ptr);
      pfChanShare(chan->chan_ptr, PFCHAN_VIEW | PFCHAN_NEARFAR | PFCHAN_SCENE |
                  PFCHAN_EARTHSKY | PFCHAN_FOV |
                  PFCHAN_LOD | PFCHAN_SWAPBUFFERS_HW);

      // Set viewport to be upper half of screen
      pfChanViewport(chan->chan_ptr, 0.0f, 1.0f, 0.5f, 1.0f);

	// Set the inter pupilary distance (space between the eyes)
	// This should come from an environment/config/boomrec structure
	// so it can be set outside the application 
	// IEM 6/96
	{
#define FS_IPD 0.5 // this is too big (should be 0.0064m)
	pfVec3 xyz;
	pfVec3 hpr;
	int c;
	for (c = 0; c < 2; c++) {
		pfSetVec3(xyz, (c!=0)?(FS_IPD/2.0):(-FS_IPD/2.0), 0.0f, 0.0f);
		pfChanViewOffsets(G_dynamic_data->pipe[0].channel[c].chan_ptr, xyz, hpr);
    	}

	}

      chan->eyeview = FORWARD_VIEW;
   }

/***************************************************************************/

/***************************************************************************/

   if (G_dynamic_data->window_size == WIN_FS_ST_BUFFERS) {

      /* Open a channel for the right monitor view */
      chan += 1;
      chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);

      pfChanTravFunc(chan->chan_ptr, PFTRAV_DRAW, basic_draw);

      // Attach the two channels to they share most characteristics
      pfAttachChan(G_dynamic_data->pipe[0].channel[0].chan_ptr,
                   chan->chan_ptr);
      pfChanShare(chan->chan_ptr, PFCHAN_VIEW | PFCHAN_NEARFAR | PFCHAN_SCENE |
                  PFCHAN_EARTHSKY | PFCHAN_FOV |
                  PFCHAN_LOD | PFCHAN_VIEWPORT);

        // Set the inter pupilary distance (space between the eyes)
        // This should come from an environment/config/boomrec structure
        // so it can be set outside the application
        // IEM 6/96
        {
#define FS_IPD 0.5 // this is too big (should be 0.0064m)
        pfVec3 xyz;
        pfVec3 hpr;
        int c;
        for (c = 0; c < 2; c++) {
                pfSetVec3(xyz, (c!=0)?(FS_IPD/2.0):(-FS_IPD/2.0), 0.0f, 0.0f);
                pfChanViewOffsets(G_dynamic_data->pipe[0].channel[c].chan_ptr, xyz, hpr);
        }

        }

      chan->eyeview = FORWARD_VIEW;
   }

/***************************************************************************/
/***************************************************************************/

pfVec3 my_xyz, my_hpr;
   if (G_dynamic_data->window_size == WIN_TWO_ADJACENT) {
      int share;

      share = pfGetChanShare(chan->chan_ptr);
//      share |= (PFCHAN_VIEW);
      pfChanShare(chan->chan_ptr,share);

      /* Open a channel for the right monitor view */
      chan += 1;
      chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);

      // Attach the two channels to they share most characteristics
      pfAttachChan(G_dynamic_data->pipe[0].channel[0].chan_ptr,
                   chan->chan_ptr);

      pfChanTravFunc(chan->chan_ptr, PFTRAV_DRAW, basic_draw_right);

      pfChanViewport(chan->chan_ptr, 0.0f, 0.5f, 0.0f, 0.5f);

      /* Set each channel's viewing offset */
      pfSetVec3(my_xyz, 0.0f, 0.0f, 0.0f);

      /* Horizontally tile the channels from left to right */
      pfSetVec3(my_hpr,
                0.5f*G_dynamic_data->pipe[0].channel[0].field_of_view[X],
                0.0f, 0.0f);
      pfChanViewOffsets(chan->chan_ptr, my_xyz, my_hpr);

      chan->eyeview = FORWARD_VIEW;


      G_logo_light = pfNewLight(arena);
      pfLightColor(G_logo_light, PFLT_AMBIENT, 1.0f, 0.0f, 0.0f);
      pfLightColor(G_logo_light, PFLT_DIFFUSE, 1.0f, 0.0f, 0.0f);
      pfLightColor(G_logo_light, PFLT_SPECULAR, 1.0f, 0.0f, 0.0f);
      pfLightPos(G_logo_light,0.0f, 0.0f, 1.0f, 0.0f);
      logo_dcs = pfNewDCS();
      logo_heading = 0.0f;
      pfDCSRot(logo_dcs,logo_heading,0.0f,0.0f);
      pfDCSTrans(logo_dcs,0.0f, 0.0f, 0.0f);
      chan += 1;
      chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);
      pfChanTravFunc(chan->chan_ptr, PFTRAV_DRAW, logo_draw);
      pfChanViewport(chan->chan_ptr, 0.5f, 1.0f, 0.5f, 1.0f);
      chan->scene = pfNewScene();
      pfAddChild(chan->scene,logo_dcs);
      G_logo = (pfGroup *)pfdLoadFile("logo.flt");
      if ( G_logo != NULL ) {
         if (0 == (pfNodeName(G_logo, "G_logo")))
            cerr << "Performer says G_logo not unique.\n";
   
         if (!pfAddChild(logo_dcs, G_logo))
            cerr << "Error occured in attaching logo subtree.\n";
         else {
            pfNodeTravMask(G_logo, PFTRAV_ISECT,
                           PRUNE_MASK,
                           PFTRAV_SELF | PFTRAV_IS_CACHE, PF_SET);
         }
      }
      pfChanScene(chan->chan_ptr, chan->scene);
      pfChanAutoAspect(chan->chan_ptr, PFFRUST_CALC_VERT);
      pfMakeSimpleChan(chan->chan_ptr, 45.0f);
      pfGetNodeBSphere(chan->scene, &logo_sphere);
      pfCopyVec3(chan->view.xyz, logo_sphere.center);
      chan->view.xyz[PF_Y] -= 2.0f*logo_sphere.radius;
      pfSetVec3(chan->view.hpr, 0.0f, 0.0f, 0.0f);
      pfChanView(chan->chan_ptr, chan->view.xyz, chan->view.hpr);


   }

/***************************************************************************/
   
   if ( aim_view_enabled ) {
      static CHANNEL_DATA *my_chan;

      my_chan = (CHANNEL_DATA *)&(G_dynamic_data->pipe[0].channel[3]);

      /* Open a channel for rendering aiming view */
      my_chan->chan_ptr = pfNewChan(G_dynamic_data->pipe[0].pipe_ptr);
      // Attach the two channels to they share most characteristics
      pfAttachChan(G_dynamic_data->pipe[0].channel[0].chan_ptr,
                   my_chan->chan_ptr);
/*
      pfChanShare(my_chan->chan_ptr, PFCHAN_NEARFAR | PFCHAN_SCENE |
                  PFCHAN_EARTHSKY | PFCHAN_LOD | PFCHAN_SWAPBUFFERS);
*/
      // Turn default drawing of video missile window off initially
      pfChanTravMode(my_chan->chan_ptr, PFTRAV_DRAW, PFDRAW_OFF);
      pfChanTravFunc(my_chan->chan_ptr, PFTRAV_DRAW, aim_view_draw);
/*
      pfChanNearFar(my_chan->chan_ptr, 
                    G_dynamic_data->pipe[0].channel[0].clip_plane.near,
                    G_dynamic_data->pipe[0].channel[0].clip_plane.far );
*/
      if ((G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
          (G_dynamic_data->window_size == WIN_TWO_ADJACENT) ||
          (G_dynamic_data->window_size == WIN_THREE_TV)) {
         pfChanViewport(my_chan->chan_ptr, 0.0f, 0.5f, 0.5f, 1.0f);
      }
      else {
         pfChanViewport(my_chan->chan_ptr, 0.0f, 1.0f, 0.0f, 1.0f);
      }
/*
      // Set the field-of-view to 10 degrees for video missile
      pfChanFOV(my_chan->chan_ptr, 10.0f, -1.0f);
*/
      my_chan->eyeview = MUNITION_VIEW;

   }


   /* Switch back to the main channel and modify attributes */

   chan = (CHANNEL_DATA *)&(G_dynamic_data->pipe[0].channel[0]);

   /* Define the viewport for the scene channel. */
   pfChanViewport(chan->chan_ptr, chan->viewport.left,
                  chan->viewport.right,
                  chan->viewport.bottom,
                  chan->viewport.top);

   /* Define the near and far clipping plane distances. */
   pfChanNearFar(chan->chan_ptr, chan->clip_plane.near,
                 chan->clip_plane.far);
   G_dynamic_data->clip_plane.near = chan->clip_plane.near;
   G_dynamic_data->clip_plane.far = chan->clip_plane.far;

   //open up the main channel

   initman->query(LOD_SCALE, &local, GET);
   LOD_scale = local.floats[0];
   initman->query(LOD_FADE, &local, GET);
   LOD_fade = local.floats[0];

   cerr << "\tLOD settings are scale = " << LOD_scale << ", fade = " << LOD_fade << endl;
   pfChanLODAttr(chan->chan_ptr, PFLOD_SCALE, LOD_scale);
   pfChanLODAttr(chan->chan_ptr, PFLOD_FADE, LOD_fade);


   /* Compute the aspect ratio. */

   G_dynamic_data->aspect = (G_dynamic_data->pipe[0].window.right -
                             G_dynamic_data->pipe[0].window.left) /
      (G_dynamic_data->pipe[0].window.top -
       G_dynamic_data->pipe[0].window.bottom);

   if (G_dynamic_data->window_size == WIN_TWO_ADJACENT) {
      pfSetVec3(my_hpr,
                -0.5f*chan->field_of_view[X],
                0.0f, 0.0f);
      pfChanViewOffsets(chan->chan_ptr, my_xyz, my_hpr);
      pfChanAutoAspect(chan->chan_ptr, PFFRUST_CALC_VERT);
   }
   else {

      /* Define the field-of-view for y and x. */
      if (chan->field_of_view[Y] == -1.0f) {
         pfChanAutoAspect(chan->chan_ptr, PFFRUST_CALC_VERT);
         cerr << "     Automatically computing vertical FOV." << endl;
      }
   }

   pfChanFOV(chan->chan_ptr, chan->field_of_view[X], chan->field_of_view[Y]);

   pfGetChanFOV(chan->chan_ptr, &(chan->field_of_view[X]),
                &(chan->field_of_view[Y]));
   G_dynamic_data->fovx = chan->field_of_view[X];
   G_dynamic_data->fovy = chan->field_of_view[Y];
   cerr << "     Field of view is set to: "
      << G_dynamic_data->fovx << " X " << G_dynamic_data->fovy << endl;

   // Initialize time of day and other atmospheric effects
   // Still needed regardless of new DVW stuff
   inittod(arena, chan);
   updatetod(G_dynamic_data->tod, chan);

   /* Define the draw callback function for our the scene channel */
   pfChanTravFunc(chan->chan_ptr, PFTRAV_DRAW, draw_process);

   /* Create a new scene for the scene channel */
   chan->scene = pfNewScene();
   if (0 == (pfNodeName(chan->scene, "G_scene")))
      cerr << "Performer says G_scene not unique.\n";

   G_dynamic_data->number_of_textures = 0;

   /* Reserve enough memory in Shared Arena for pfTexture pointers */
   G_dynamic_data->texture_list_ptr = (pfTexture **) pfMalloc((unsigned int)
                              (sizeof(void *) * MAX_TEXTURES), arena);

   if (G_dynamic_data->texture_list_ptr == NULL)
      cerr << "ERROR:  Not enough memory in arena to allocate textures.\n";
   
   initman->query(IM_MAX_ALTITUDE, &local, GET);
   G_dynamic_data->max_alt = local.floats[0];
   cerr << "     Maximum Altitude: " << G_dynamic_data->max_alt << endl;

   cerr << "     Loading terrain..." << endl;;

   if (!db_paging) {

#ifdef DATALOCK
      pfDPoolLock((void *) G_dynamic_data);
#endif
      G_dynamic_data->init_event = LOADING_TERRAIN;
#ifdef DATALOCK
      pfDPoolUnlock((void *) G_dynamic_data);
#endif

      if (!(G_static_data->process_mode & PFMP_FORK_DRAW)){
//         print_next_event();
      }
      if (G_static_data->sound) {
         LocationRec dummy_snd_loc;

         dummy_snd_loc.x = 0.0;
         dummy_snd_loc.y = 0.0;
         dummy_snd_loc.z = 0.0;
         playsound(&dummy_snd_loc, &dummy_snd_loc, LOADING_INDEX, -1);
      }
   }



   /* Create and add the terrain sub-tree to the scene graph. */
   dbShare->fixed = G_fixed = pfNewGroup();
   if (0 == (pfNodeName(G_fixed, "G_fixed")))
      cerr << "Performer says G_fixed not unique.\n";

   if (!pfAddChild(chan->scene, G_fixed))
      cerr << " Error occured in attaching terrain subtree.\n";

   pfNodeTravMask(G_fixed, PFTRAV_ISECT,
                  ALL_MASK,
                  PFTRAV_SELF | PFTRAV_IS_CACHE, PF_SET);

   if (!db_paging) {
      /* Partion terrain for the craters */
      deviy_up_dirt();

      /* Read and load the terrain files */
      if (load_terrain()) {
         cerr << " Error occured in loading the terrain files\n";
      }
   }
   else {
      struct TM    im_masks[MAX_MASKS];
      int          num_masks;
      char         tsg_name[1024];

      initman->query (DB_TSG_FILE, &local, GET);

      if ((strcmp(local.string,"NONE")) == 0 ) {
         initman->get_terrain_masks((struct TM**) im_masks, &num_masks);
         strcpy (tsg_name, im_masks[0].filename);
         cerr << "Warning: Obsolete method for specifying "
              << "terrain paging file." << endl;
         cerr << "\tUse DB_TSG_FILE " << im_masks[0].filename << endl;
      }
      else {
         strcpy (tsg_name, local.string);
      }

      char file_path[1024];
      if (pfFindFile (tsg_name, file_path, R_OK)) {
         if (!(dbShare->tsgInfo = tsgReadOpen(file_path))) {
            cerr << "Can't load" << (*file_path ? file_path : argv[1])
                 << endl;
            pfExit ();
         }
         else {
            dbPrintTSG (dbShare->tsgInfo);
            G_dynamic_data->bounding_box.xmin = 0.0f;
            G_dynamic_data->bounding_box.xmax =
                  dbShare->tsgInfo->tile_size*dbShare->tsgInfo->tiles_x;
            G_dynamic_data->bounding_box.ymin = 0.0f;
            G_dynamic_data->bounding_box.ymax =
                  dbShare->tsgInfo->tile_size*dbShare->tsgInfo->tiles_y;
            G_dynamic_data->bounding_box.xsize =
                  G_dynamic_data->bounding_box.xmax 
                  - G_dynamic_data->bounding_box.xmin;
            G_dynamic_data->bounding_box.ysize =
                  G_dynamic_data->bounding_box.ymax 
                  - G_dynamic_data->bounding_box.ymin;
            cerr << "\tCalculated Bounding Box: "
                 << G_dynamic_data->bounding_box.xmax << ", "
                 << G_dynamic_data->bounding_box.ymax << endl;
            pfChanGState (chan->chan_ptr, dbShare->tsgInfo->globalmat[0]);
            pfChanGStateTable (chan->chan_ptr, dbShare->tsgInfo->materials[0]);
            dbShare->tsgInfo->indexed = 0;
            dbShare->okToDownLoadTextures = TRUE;
         }
         dbShare->dbPageBin = new dbDeletePage();
         pfDBaseFunc (db_paging_process);
      }
      else {
         cerr << "*** Error: unable to find tsg file: "
              << tsg_name << endl;
         pfExit ();
      }
   }

   /* Create and add the vehicle subtree to the scene */
   G_moving = pfNewGroup();
   if (0 == (pfNodeName(G_moving, "G_moving")))
      cerr << "Performer says G_moving not unique.\n";

   if (!pfAddChild(chan->scene, G_moving))
      cerr << " Error occured in attaching vehicle subtree.\n";

   /*add the main smoke node.  The plume nodes will be added
      to a child node in the tree based on where it is in the
      terrain db */

   //add the node for the control measures
   G_measures = pfNewGroup();
   if (0 == (pfNodeName(G_measures, "G_measures")))
      cerr << "Performer says G_lights not unique.\n";

   if (!pfAddChild(chan->scene, G_measures))
      cerr << " Error occured in attaching measures subtree.\n";

   /* Declare what scene this channel should render. */
   pfChanScene(chan->chan_ptr, chan->scene);

   /* Set the starting view position and orientation. */
   pfChanView(chan->chan_ptr, chan->view.xyz, chan->view.hpr);

   if ((G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
       (G_dynamic_data->window_size == WIN_THREE_TV)) {
      pfMatrix temp_matrix;;
      pfGetChanViewMat(chan->chan_ptr, temp_matrix);
      CHANNEL_DATA *temp_chan;

      temp_chan = chan + 1;
      pfChanViewMat(temp_chan->chan_ptr, temp_matrix);
      temp_chan++;
      pfChanViewMat(temp_chan->chan_ptr, temp_matrix);
   }

   /* Create the pass data from passing time relevant infomation down */
   /* the multiprocessing pipeline from application to cull to draw.  */
   draw_data = (PASS_DATA *) pfAllocChanData(chan->chan_ptr, sizeof(PASS_DATA));
   init_pass_data(draw_data);

   /* Initialize posture in Draw_data to posture given in configuration file */
   bcopy(&(chan->view), &(draw_data->posture), sizeof(pfCoord));

#ifdef JACK
   if (G_runjack)
      JackSoldier::jack = new JackProc();
#endif

   DIGUY_VEH::init_diguy (initman);

   /* Allocate the memory for the entity arrays, and set all to NOSHOW */
   initialize_vehicle_arrays();


#ifdef TRACE
   cerr << "Init veh arrays done\n";
#endif


#ifdef DATALOCK
   pfDPoolLock((void *) G_dynamic_data);
#endif
   G_dynamic_data->init_event = LOADING_DMODELS;
#ifdef DATALOCK
   pfDPoolUnlock((void *) G_dynamic_data);
#endif

   cerr << "     Loading dynamic models...\n";
   if (!(G_static_data->process_mode & PFMP_FORK_DRAW)) {
      //print_next_event(); -4/10/96
   }
   if (G_static_data->sound) {
      LocationRec dummy_snd_loc;

      dummy_snd_loc.x = 0.0;
      dummy_snd_loc.y = 0.0;
      dummy_snd_loc.z = 0.0;
      playsound(&dummy_snd_loc, &dummy_snd_loc, LOADING_INDEX, -1);
   }


   /* Read the dynamic models def file and constuct the tree */
   pfdConverterMode_flt (PFFLT_FLATTEN, FALSE);
   //setup_type_table((char *)G_static_data->model_file);
   setup_type_table();
   pfdConverterMode_flt (PFFLT_FLATTEN, TRUE);

#ifndef NO_DVW
   getDVWTexturePtrs();
#endif


#ifdef JACK
   if (G_runjack) {
      if (G_static_data->load_dynamic_models)
         JackInit(jack_dir);
      // Initialize table of soldiers to see who's active or not
      G_JackSoldier_active = (short *) pfMalloc(JACK_MAXSOLDIER *
                                   sizeof(short), pfGetSharedArena());

      for (int s = 0; s < JACK_MAXSOLDIER; s++)
         G_JackSoldier_active[s] = FALSE;
   }
#endif

#ifdef TRACE
   cerr << "Init types table done\n";
#endif

   /* Set up the hash table for the entity ids from the network */
   setup_entity_net_table();

#ifdef TRACE
   cerr << "Entity_net_table done\n";
#endif

#ifdef FREE_MEMORY
   init_frame_queue();
#endif


#ifdef DATALOCK
   pfDPoolLock((void *) G_dynamic_data);
#endif
   G_dynamic_data->init_event = LOADING_SMODELS;
#ifdef DATALOCK
   pfDPoolUnlock((void *) G_dynamic_data);
#endif

   if (!(G_static_data->process_mode & PFMP_FORK_DRAW)) {
      //print_next_event(); -4/10/96
   }
   if (G_static_data->sound) {
      LocationRec dummy_snd_loc;

      dummy_snd_loc.x = 0.0;
      dummy_snd_loc.y = 0.0;
      dummy_snd_loc.z = 0.0;
      playsound(&dummy_snd_loc, &dummy_snd_loc, LOADING_INDEX, -1);
   }


   if (!nostatic) {
      //read in the static objects
      //readnplacemodels((char *)G_static_data->location_file);
      cerr << "     Loading static models..." << endl;
      readnplacemodels();
#ifdef TRACE
      cerr <<"Static Models have been read in and placed\n";
#endif
   }

   cerr << "     Initializing the driven vehicle..." << endl;

   /* Initialize the driven vehicle */
   if (!G_dynamic_data->exit_flag && !G_runisms) {
      init_drivenveh(G_drivenveh,
                  (SimulationAddress *) & G_static_data->DIS_address);
   }

#ifndef NO_NPSSMOKE
   cerr << "     Initializing original NPS smoke..." << endl;
   /*initialized fire for 20 plumes blowing true north at 2 mps */
   for (int num_colors = 0; num_colors < MAX_SMOKE_COLORS; num_colors++)
      initialize_fire(5, 0.0f, 2.0f, num_colors);
#endif

//#endif

#ifdef TRACE
   cerr << "Initialized driven vehicle\n";
#endif

#ifndef NOSUB  
   //Adding moving water to the scene if a submarine
   if( (G_vehlist[G_drivenveh].vehptr != NULL) &&
       ( // (G_vehlist[G_drivenveh].vehptr->gettype() == SHIP) ||
       (G_vehlist[G_drivenveh].vehptr->gettype() == SUBMERSIBLE)) ) {            
     cerr << "Loading moving water"<<endl;
     LoadLakeDB (chan->scene);
 
#ifdef TRACE
   cerr << "Done loading moving water"<<endl;
#endif
     } 
#endif // NOSUB

   //now do the weapons
   /* Allocate the memory and set all to NOSHOW */

   cerr << "     Initializing weapons..." << endl;
   initialize_weapons_array(G_runisms);


#ifdef TRACE
   cerr << "Init weapons done\n";
#endif



   if (load_animations) {
#ifdef DATALOCK
      pfDPoolLock((void *) G_dynamic_data);
#endif
      G_dynamic_data->init_event = LOADING_ANIMATIONS;
#ifdef DATALOCK
      pfDPoolUnlock((void *) G_dynamic_data);
#endif

      if (!(G_static_data->process_mode & PFMP_FORK_DRAW)) {
         //print_next_event(); -4/10/96
      }
      if (G_static_data->sound) {
         LocationRec dummy_snd_loc;

         dummy_snd_loc.x = 0.0;
         dummy_snd_loc.y = 0.0;
         dummy_snd_loc.z = 0.0;
         playsound(&dummy_snd_loc, &dummy_snd_loc, LOADING_INDEX, -1);
      }

   }

   G_animations = pfNewGroup();
   if (0 == (pfNodeName(G_animations, "G_animations")))
      cerr << "Performer says G_animations not unique.\n";

   if (!pfAddChild(chan->scene, G_animations))
      cerr << " Error occured in attaching animations subtree.\n";
   else {
      pfNodeTravMask(G_animations, PFTRAV_ISECT,
                     PRUNE_MASK,
                     PFTRAV_SELF | PFTRAV_IS_CACHE, PF_SET);
   }

   if (load_animations) {
      cerr << "     Loading animations..." << endl;
      load_animation();
   }

   cerr << "     Initializing fireballs and craters..." << endl;
   //set up the crater and fireball array
   make_fireball_array();
   make_crater_array();

   if (strcasecmp((const char *)G_static_data->blinking_lights_file, "NONE") != 0) {
      cerr << "     Initializing blinking lights..." << endl;
      init_blinking_lights();
      read_blinking_lights();
   }

   control_data.rigid_body = FALSE;
   control_data.reset = FALSE;
   control_data.vid = 0;
   control_data.thrust = 0.0f;
   control_data.pitch = 0.0f;
   control_data.heading = 0.0f;
   control_data.roll = 0.0f;
   target_ok = 0.0f;
   posture_chg_ok = 0.0f;
   G_slow_speed_increment = FALSE;

#ifdef DATALOCK
   pfDPoolLock((void *) G_dynamic_data);
#endif
   G_dynamic_data->init_event = OPENING_NETWORK;
#ifdef DATALOCK
   pfDPoolUnlock((void *) G_dynamic_data);
#endif

   cerr << "     Opening networks...\n";
   if (!(G_static_data->process_mode & PFMP_FORK_DRAW)) {
      //print_next_event(); -4/10/96
   }
   if (G_static_data->sound) {
      LocationRec dummy_snd_loc;

      dummy_snd_loc.x = 0.0;
      dummy_snd_loc.y = 0.0;
      dummy_snd_loc.z = 0.0;
      playsound(&dummy_snd_loc, &dummy_snd_loc, LOADING_INDEX, -1);
   }

   if (strcasecmp((const char *)G_static_data->filter_file, "NONE") != 0) {
      read_filter_file();
   }

#ifdef TRACE
   cerr << "Read hosts file\n";
   cerr << "Opening the network connection on port " << ether_interf << "\n";
#endif

   G_net = NULL;

#ifndef NOHIRESNET
   G_hr_net = NULL;
#endif // NOHIRESNET

   if (!G_dynamic_data->exit_flag) {
#ifndef NOS1kLOADER
      if ((G_dynamic_data->TDBoverride) &&
          (strcmp((char *) G_static_data->round_world_file, D_ROUND_WORLD_FILE))) {
         modifyUTMfile(G_static_data->round_world_file);
      }
#endif

      G_static_data->rw.rw_valid = FALSE;

      if (strcmp((char *) G_static_data->round_world_file,
                    D_ROUND_WORLD_FILE)) {
         G_static_data->rw.rw_valid = TRUE;
      }

      if (db_paging && !(G_static_data->rw.rw_valid)) {
         G_static_data->rw.northing = dbShare->tsgInfo->ll_y;
         G_static_data->rw.easting = dbShare->tsgInfo->ll_x;
         G_static_data->rw.o_zone_num = dbShare->tsgInfo->zone;
         G_static_data->rw.o_zone_letter = dbShare->tsgInfo->projinfo[0];
         if (!(strcmp ((char *)dbShare->tsgInfo->datum, "WGS84"))) {
            G_static_data->rw.map_datum = 3;
         }
         else if (!(strcmp ((char *)dbShare->tsgInfo->datum, "NASBNAD27"))) {
            G_static_data->rw.map_datum = 5;
         }
         else {
            cerr << "ERROR: Unsuported map datum" << endl;
            G_static_data->rw.map_datum = 3;
         }
         G_static_data->rw.width = dbShare->tsgInfo->tiles_y 
                                    * dbShare->tsgInfo->tile_size;
         G_static_data->rw.height = dbShare->tsgInfo->tiles_x 
                                     * dbShare->tsgInfo->tile_size;
         G_static_data->rw.rw_valid = TRUE;
      }
      else {
         struct RW initManRW;
         initman->get_round_world_data(&initManRW);
         G_static_data->rw.northing = initManRW.northing;
         G_static_data->rw.easting = initManRW.easting;
         G_static_data->rw.o_zone_num = initManRW.zone_num;
         G_static_data->rw.o_zone_letter = initManRW.zone_letter;
         G_static_data->rw.map_datum = initManRW.map_datum;
         G_static_data->rw.width = initManRW.width;
         G_static_data->rw.height = initManRW.height;
      }
      initman->query(FLAT_WORLD, &local, GET);

      if (local.ints[0]) {
         G_static_data->rw.rw_valid = FALSE;
      }

      initman->query(USE_ROUND_WORLD, &local, GET);

      if (local.ints[0] != -1) {
         if (!local.ints[0]) {
            G_static_data->rw.rw_valid = FALSE;
         }
      }

      if (!G_static_data->rw.rw_valid) {
         cerr << "\tUsing Flat World Coordinates." << endl;
      }

      if (!multicast) {
         if (port == 0) {
            port = DEFAULT_BC_PORT;
         }
         G_net = new DIS_net_manager(G_dynamic_data->exercise,
                                     G_static_data->rw.rw_valid,
                                     (roundWorldStruct *)&(G_static_data->rw),
                                     500, ether_interf, port);
      }
      else {
         if (port == 0) {
            port = DEFAULT_MC_PORT;
         }

         G_net = new DIS_net_manager(group, ttl, port,
                                     G_dynamic_data->exercise,
                                     G_static_data->rw.rw_valid,
                                     (roundWorldStruct *)&(G_static_data->rw),                                       500, ether_interf);
      }
      net_is_open = G_net->net_open();
      if (!net_is_open) {
         G_dynamic_data->network_mode = NET_OFF;
         cerr << "ERROR:\tUnabled to open the network.  Proceeding with" << endl;
         cerr << "\tnetwork turned off." << endl;
      }
      else {
         if (!G_static_data->stealth) {
            G_net->add_to_filter_list(G_vehlist[G_drivenveh].DISid);
         }
      }

#ifndef NOHIRESNET

      if (!hr_multicast) {
         if (hr_port == 0) {
            hr_port = port - 2;
         }

         G_hr_net = new DIS_net_manager(G_dynamic_data->exercise,
                                        G_static_data->rw.rw_valid,
                                        (roundWorldStruct *)
                                         &(G_static_data->rw),
                                        500, ether_interf, hr_port);
      }
      else {
         if (hr_port == 0) {
            hr_port = port - 2;
         }
         G_hr_net = new DIS_net_manager(hr_group, hr_ttl, hr_port,
                                        G_dynamic_data->exercise,
                                        G_static_data->rw.rw_valid,
                                        (roundWorldStruct *)
                                         &(G_static_data->rw),
                                        500, ether_interf);
      }
      hr_net_is_open = G_hr_net->net_open();

      if (!hr_net_is_open) {
         G_dynamic_data->hr_network_mode = NET_OFF;
         cerr << "ERROR:\tUnabled to open the Hi-Res Human network." << endl
            << "\tProceeding with Hi-Res Human network turned off." << endl;
      }
      else {
         G_hr_net->add_to_filter_list(G_vehlist[G_drivenveh].DISid);
         G_hr_net->add_to_receive_list(DataPDU_Type);
//         G_dynamic_data->hr_network_mode = NET_SEND_AND_RECEIVE;
      }
#endif //NOHIRESNET
   }
   else {
      net_is_open = FALSE;
#ifndef NOHIRESNET
      hr_net_is_open = FALSE;
#endif // NOHIRESNET
   }




#ifndef NOC2MEASURES
   //SAK.  Initialize the CTDB database.  ModSAF stuff.
   if ((strcmp((const char *)G_static_data->po_ctdb_file, "NONE")) == 0) {
      po_is_up = FALSE;
   }
   else {
      cerr << "\tStarting PO/Measures subsystem..." << endl;
      po_is_up =
         initPoMeas((const char *)G_static_data->po_ctdb_file,
                    G_static_data->po_exercise,
                    G_static_data->po_database,
                    G_static_data->po_port);
   }
#endif


#ifdef DATALOCK
   pfDPoolLock((void *) G_dynamic_data);
#endif
   old_control = G_dynamic_data->control;
#ifdef DATALOCK
   pfDPoolUnlock((void *) G_dynamic_data);
#endif


   G_curtime = control_data.current_time = pfGetTime();
   control_data.last_time = control_data.current_time;

#ifdef TRACE
   cerr << "Start of the Main Simulation Loop \n";
#endif

   get_flt_texture_list ();

#ifdef DATALOCK
   pfDPoolLock((void *) G_dynamic_data);
#endif
   G_dynamic_data->init_event = STARTING_DRAW;
#ifdef DATALOCK
   pfDPoolUnlock((void *) G_dynamic_data);
#endif

   cerr << "     Starting Draw Process...\n";
   if (!(G_static_data->process_mode & PFMP_FORK_DRAW)) {
      //print_next_event(); -4/10/96
   }
   if (G_static_data->sound) {
      LocationRec dummy_snd_loc;

      dummy_snd_loc.x = 0.0;
      dummy_snd_loc.y = 0.0;
      dummy_snd_loc.z = 0.0;
      playsound(&dummy_snd_loc, &dummy_snd_loc, THEME_INDEX, -1);
   }

//   pfPWinConfigFunc(G_dynamic_data->pw,open_pipe_line);
//   pfConfigPWin (G_dynamic_data->pw);

// New Opening Screen (4/10/96) Only open the window after everything is loaded
   pfSync();
   pfFrame();

//   Window theWindow = pfGetPWinWSWindow(G_dynamic_data->pw);

//   initman->query(CURSOR_FILE, &local, GET);

//   G_dynamic_data->XCursor = new XCursorUtil (local.string,
//                                              theDisplay,
//                                              theWindow);

   init_display_options();

//   G_filter_range = MAX_SNAP_DIST;

   // These lines take care of a Performer problem with terrain tearing
#if 1
   cerr << "     Doing optimization on scene graph..." << endl;
   pfdMakeShared((pfNode *)chan->scene);
   pfGeoState *gs;
   gs = pfNewGState(pfGetSharedArena());
   pfCopy(gs, (pfGeoState*)pfdGetDefaultGState());
   pfSceneGState(chan->scene, gs);
   pfdMakeSharedScene(chan->scene);
   pfdCombineLayers ((pfNode*)chan->scene);
#endif

#ifdef VABS
// Put initialization of OSV here
   XEvent vabs_event;

   system("xrdb Vabs");
   printf("Starting the OSV");
   init_vabs(argc, argv);
#endif

#ifndef NO_DVW
   initializeFlareLights();
#endif // NO_DVW

   char auto_path[255];
   initman->query(AUTO_FILE, &local, GET);
   if (strcmp (local.string, "NONE") != 0) {
      if (pfFindFile (local.string, auto_path, R_OK)) {
         VEHICLE::vehAuto_init (auto_path);
      }
      else {
         cerr << "Error: Unable to find AUTO_FILE: " << local.string << endl;
         G_dynamic_data->useAuto = FALSE;
      }
   }
   else {
      G_dynamic_data->useAuto = FALSE;
   }

   initman->query(AUTO_STATE, &local, GET);
   G_dynamic_data->useAuto = local.ints[0]; 

/************************************************************************/
/******************** Main Application Simulation Loop ******************/
   while (!exit_loop) {
      pfSync();
      check_blinking_lights();
      check_animations();

      if (G_static_data->hmd_unit) {
         getHMDView(chan->view, draw_data);
      }

//      static pfVec3 zero = {0.0f, 0.0f, 0.0f};

//      if (G_dynamic_data->control == NPS_ODT) {
//         pfChanView(chan->chan_ptr, chan->view.xyz, zero);
//      }
//      else {
         pfChanView(chan->chan_ptr, chan->view.xyz, chan->view.hpr);
//      }

      if ((G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
          (G_dynamic_data->window_size == WIN_THREE_TV)) {
         static pfMatrix temp_matrix;
         static pfMatrix leftview;
         static pfMatrix rightview;
         static pfMatrix chan_matrix;
         static CHANNEL_DATA *tempchan;
         static float the_sin, the_cos;

         pfGetChanViewMat(chan->chan_ptr, chan_matrix);
         PFMAKE_IDENT_MAT(temp_matrix);
         pfSinCos(chan->field_of_view[X], &the_sin, &the_cos);
         temp_matrix[0][0] = temp_matrix[1][1] = the_cos;
         temp_matrix[1][0] = -(temp_matrix[0][1] = the_sin);
         pfMultMat(leftview, temp_matrix, chan_matrix);
         temp_matrix[1][0] *= -1;
         temp_matrix[0][1] *= -1;
         pfMultMat(rightview, temp_matrix, chan_matrix);
         PFCOPY_VEC3(leftview[3], chan->view.xyz);
         PFCOPY_VEC3(rightview[3], chan->view.xyz);
         tempchan = chan + 1;
         pfChanViewMat(tempchan->chan_ptr, leftview);
         tempchan++;
         pfChanViewMat(tempchan->chan_ptr, rightview);
      }
      if (G_dynamic_data->window_size == WIN_TWO_ADJACENT) {
         pfChanView(G_dynamic_data->pipe[0].channel[1].chan_ptr,
                    chan->view.xyz, chan->view.hpr);
      }

      draw_data->eyeview = chan->eyeview;

      if ((G_dynamic_data->window_size != WIN_THREE_BUNKER) &&
          (G_dynamic_data->window_size != WIN_TWO_TPORT) &&
          (G_dynamic_data->window_size != WIN_FS_MCO) &&
          (G_dynamic_data->window_size != WIN_TWO_ADJACENT) &&
          (G_dynamic_data->window_size != WIN_FS_ST_BUFFERS) &&
          (G_dynamic_data->window_size != WIN_THREE_TV)) {
         /* Pass the channel data for the vehicle information */
         if (info_pdata->veh_index > -1)
            pfPassChanData(G_dynamic_data->pipe[0].channel[2].chan_ptr);
      }

      if ( aim_view_enabled ) {
         static CHANNEL_DATA *another_chan;
         static pfCoord the_view;
         another_chan = chan + 3;
         if (G_vehlist[G_drivenveh].vehptr->get_video_switch ()) {
            pfChanTravMode(another_chan->chan_ptr, PFTRAV_DRAW, PFDRAW_ON);
            G_vehlist[G_drivenveh].vehptr->get_weapon_view(the_view);
            another_chan = chan + 3; 
            pfChanView(another_chan->chan_ptr, the_view.xyz, the_view.hpr);
         }
         else {
            pfChanTravMode(another_chan->chan_ptr, PFTRAV_DRAW, PFDRAW_OFF);
         }
      }

      if ( logo_dcs ) {
         logo_heading += 0.5f;
         if ( logo_heading > 360.0f ) {
            logo_heading -= 360.0f; 
         }
         pfDCSRot(logo_dcs,logo_heading,0.0f,0.0f);
      }

      pfPassChanData(chan->chan_ptr);
      pfFrame();

      /* Establish a time delta and then update last_time to now */
      G_curtime = control_data.current_time = pfGetTime();

      control_data.delta_time = control_data.current_time -
         control_data.last_time;
      control_data.last_time = control_data.current_time;

      // The following if is performed once and only once the first
      //    time through the simulation main loop
      if (G_dynamic_data->init_event != DONE_ALL_EVENTS) {
         seconds_to_hms(G_curtime,
                     seconds_expired, minutes_expired, hours_expired);
         cerr << endl << "     Load time: "
            << minutes_expired << " minutes, "
            << seconds_expired << " seconds." << endl << endl;
         while (G_dynamic_data->init_event != LAST_INIT_EVENT)
            sleep(1);
         pfFree(G_dynamic_data->texture_list_ptr);
         switch (G_dynamic_data->network_mode) {
            case NET_OFF:
            case NET_SEND_ONLY:
               break;
            case NET_RECEIVE_ONLY:
            case NET_SEND_AND_RECEIVE:
            default:
               empty_net_buffers(G_net);
               break;
         }
#ifndef NOHIRESNET
         switch (G_dynamic_data->hr_network_mode) {
            case NET_OFF:
            case NET_SEND_ONLY:
               break;
            case NET_RECEIVE_ONLY:
            case NET_SEND_AND_RECEIVE:
            default:
               empty_net_buffers(G_hr_net);
               break;
         }
#endif // NOHIRESNET
         G_dynamic_data->init_event = DONE_ALL_EVENTS;
      }

      //read the network
      switch (G_dynamic_data->network_mode) {
         case NET_OFF:
         case NET_SEND_ONLY:
            break;
         case NET_RECEIVE_ONLY:
         case NET_SEND_AND_RECEIVE:
         default:
            counter = parse_net_pdus(G_net);
            break;
      }
#ifndef NOHIRESNET
      switch (G_dynamic_data->hr_network_mode) {
         case NET_OFF:
         case NET_SEND_ONLY:
            break;
         case NET_RECEIVE_ONLY:
         case NET_SEND_AND_RECEIVE:
         default:
            counter = parse_net_pdus(G_hr_net);
            break;
      }
#endif // NOHIRESNET
      if (G_manager->ofinput != NULL) {
         G_manager->ofinput->get_all_inputs(chan->chan_ptr);
      }

      check_for_reset();

#ifndef NOC2MEASURES
      //  process the po_pdus here     //SAK.
      if (po_is_up) {
         updatePoMeas();
      }
#endif

#ifndef NONPSSHIP 
      //** NEW - Mounted ship information
      //Update picking information on mounted ships
      if (G_dynamic_data->pick_mode == SHIP_PICK) {
         update_ship_picking();
      }
#endif

      update_display_options(chan->chan_ptr, draw_data);
      if (chan->field_of_view[X] != G_dynamic_data->fovx) {
         pfGetChanFOV(chan->chan_ptr, &(chan->field_of_view[X]),
                      &(chan->field_of_view[Y]));
         G_dynamic_data->fovx = chan->field_of_view[X];
         G_dynamic_data->fovy = chan->field_of_view[Y];
         cerr << "     Field of view is set to: "
            << G_dynamic_data->fovx << " X " << G_dynamic_data->fovy << endl;
      }

      update_environment(chan);

#ifndef NO_DVW
      viewChan = chan;
      cloudSun = (CloudSun *) G_sun;
      earthSky = chan->earth_sky;
      setHazeTimeOfDay(G_dynamic_data->tod);  
      processAtmosphericModels(chan->chan_ptr);

#endif

      check_for_query(ether_interf, port, multicast, group, ttl);

#ifdef VABS
      // Get all OSV inputs
      while (XtPending()) {
         XtNextEvent(&vabs_event);
         XtDispatchEvent(&vabs_event);
      }
#endif

      //put here by corbin for smoke
      generate_all_fire();

      //move all of my munitions
      update_munitions(control_data.delta_time, control_data.current_time);

      //move all of the network entities and myself
      update_entities(draw_data, control_data);
      update_range_filter(G_filter_range);
      draw_data->rng_fltr = G_filter_range;

      process_input_states(G_dynamic_data->pw, info_pdata);

      if ((G_dynamic_data->window_size != WIN_THREE_BUNKER) &&
          (G_dynamic_data->window_size != WIN_TWO_TPORT) &&
          (G_dynamic_data->window_size != WIN_TWO_ADJACENT) &&
          (G_dynamic_data->window_size != WIN_FS_MCO) &&
          (G_dynamic_data->window_size != WIN_FS_ST_BUFFERS) &&
          (G_dynamic_data->window_size != WIN_THREE_TV)) {
         /* Get the most current info on the selected vehicle (if any) for */
         /*    displaying in the vehicle information channel               */
         acquire_veh_info(info_pdata);
      }

      // Update DBASE process if paging is active. -rb
      if (db_paging) {
         static pfCoord myPos;
         pfCopyVec3(myPos.xyz,
                    G_vehlist[G_drivenveh].vehptr->getposition());
         pfCopyVec3(myPos.hpr,
                    G_vehlist[G_drivenveh].vehptr->getorientation());
            dbShare->x = myPos.xyz[0];
            dbShare->y = myPos.xyz[1];
            dbShare->h = myPos.hpr[0];
         dbShare->dbPageBin->update ();
      }

#ifndef NOSUB
      //orient the moving water
      if( (G_vehlist[G_drivenveh].vehptr != NULL) &&
          ( // (G_vehlist[G_drivenveh].vehptr->gettype() == SHIP) ||
          (G_vehlist[G_drivenveh].vehptr->gettype() == SUBMERSIBLE)) ) { 
 
      pfVec3 posit;
      pfVec3 angle;
      
      pfCopyVec3(posit,(G_vehlist[G_drivenveh].vehptr->getposition()));
      pfCopyVec3(angle,(G_vehlist[G_drivenveh].vehptr->getorientation()));
      LakeSim(chan->chan_ptr, posit[X], posit[Y], angle[X]-100.0);
      }
#endif // NOSUB

      /* Combine veiwer offsets with posture for the channel view */
#ifdef DATALOCK
      pfDPoolLock((void *) G_dynamic_data);
#endif
      camera = G_dynamic_data->camera;
      if (camera)
         chan->eyeview = CAMERA_VIEW;
      else
         chan->eyeview = FORWARD_VIEW;
      if (G_dynamic_data->camlock == 1) {
         static pfVec3 temp_view;

         pfCopyVec3(temp_view, (float *) G_dynamic_data->locked_view);
         compute_hpr(draw_data->posture.xyz,
                     temp_view,
                     draw_data->look.hpr);
         pfSubVec3(draw_data->look.hpr, draw_data->look.hpr,
                   draw_data->posture.hpr);
      }
      else if (G_dynamic_data->camlock == 2) {
         if (G_dynamic_data->new_vid != -1) {
            control_data.vid = G_dynamic_data->new_vid;
            G_dynamic_data->new_vid = -1;
         }
         if (control_data.vid != -1) {
            compute_hpr_to_veh(draw_data->posture.xyz,
                               control_data.vid,
                               draw_data->look.hpr);
            pfSubVec3(draw_data->look.hpr, draw_data->look.hpr,
                      draw_data->posture.hpr);
         }
         else
            G_dynamic_data->camlock = FALSE;
      }
#ifdef DATALOCK
      pfDPoolUnlock((void *) G_dynamic_data);
#endif

      static int changed_direction = 0;

      if (!G_static_data->hmd_unit)
         update_view(chan->view, draw_data, changed_direction, camera);

#ifndef NONPSSHIP
      //Update Locators of all ship_walk_veh entities in vehicle array
      update_locators();
#endif

      //bring the dead things back to life
      lazarus();


#ifdef DATALOCK
      pfDPoolLock((void *) G_dynamic_data);
#endif
      if (G_manager->ofinput != NULL) {
         G_manager->ofinput->query_button(esc_counter, NPS_KEYBOARD, NPS_ESCKEY);
      }

      exit_loop = (G_dynamic_data->exit_flag ||
                   (esc_counter > 0));
#ifdef DATALOCK
      pfDPoolUnlock((void *) G_dynamic_data);
#endif

      if (exit_loop && G_static_data->sound) {
         LocationRec dummy_snd_pos;

         dummy_snd_pos.x = draw_data->posture.xyz[X];
         dummy_snd_pos.y = draw_data->posture.xyz[Y];
         dummy_snd_pos.z = draw_data->posture.xyz[Z];
         playsound(&dummy_snd_pos, &dummy_snd_pos, SHUTDOWN_INDEX, -1);
      }

   }
/******* end application main simulation loop **********/

   XSetScreenSaver (theDisplay, timeOutSetting, interval,
                    preferBlanking, allowExposure);

   // Not sure why this need to be done, but it seems the only way to
   // restore the orignal state... Maybe a bug in Xsgi ? -rb
   XGetScreenSaver (theDisplay, &timeOutSetting, &interval,
                    &preferBlanking, &allowExposure);

//   delete G_dynamic_data->XCursor;

#ifndef NO_DVW
   int iy = 0;
   while(filePaths[iy] != NULL) {
      delete [] filePaths[iy++];
   }
   DVWExit();
   sleep(2);  // time needed to shutdown DVW processes or else usually leave 
              // one behind when running on multi-processor machines
#endif


   cerr << "     Closing input manager and devices." << endl;
   if (G_manager != NULL) {
      if (G_manager->ofinput != NULL) {
         input_manager *l_input_manager = G_manager->ofinput;

         G_manager->ofinput = NULL;
         sleep(1);
         delete l_input_manager;
      }
      pfFree((void *) G_manager);
      G_manager = NULL;
   }

#ifndef NOAHA
   ahaExit();
#endif
#ifdef JACK
   if (G_runjack) {
      JackQuit();
   
      int count = 0;

      for (int ix = 0; ix < JACK_MAXSOLDIER; ix++) {
         if ((G_JackSoldier_active[ix] == TRUE) &&
             (JackSoldier::jack->table[ix])) {
            delete JackSoldier::jack->table[ix];
            count++;
         }
      }
//cerr << count << " Jack Soldiers deleted\n";
      delete G_JackSoldier_active;
   }

#endif
   deactivate_drivenveh(G_drivenveh);

   /* terminate the network processes */
   if (net_is_open) {
      G_net->net_close();
   }

#ifndef NOHIRESNET
   if (hr_net_is_open) {
      G_hr_net->net_close();
   }
#endif // NOHIRESNET

   if (G_static_data->cpu) {
      npsFreeCPUs();
   }

   if (G_static_data->hmd_unit && G_static_data->hmd_unit->exists()) {
      delete G_static_data->hmd_unit;
   }

   if (G_dynamic_data->hmd && G_static_data->reality_graphics) {
      cerr << endl << "****\tPutting the system into Console output mode --"
         << endl;
      system("/usr/gfx/setmon -n -sg -v 1280x1024_failsafe");
   }


   if (G_static_data->pdu_position && (G_static_data->pdupos_fd >= 0))
      close(G_static_data->pdupos_fd);

   if (G_static_data->sound) {
      sleep(3);
      closeaudioports();
   }

#ifdef DATALOCK
   pfReleaseDPool(G_dynamic_pool);
#endif

#ifndef NOC2MEASURES
   if (po_is_up) {
      closePoMeas();
      po_is_up = FALSE;
   }
#endif

   if (G_dynamic_data->identify != NULL) {
      delete G_dynamic_data->identify;

      G_dynamic_data->identify = NULL;
   }

   if (initman) {
      delete initman;
   }

   seconds_to_hms(pfGetTime(),
                  seconds_expired, minutes_expired, hours_expired);
   cerr << endl << "     Total run time: "
      << hours_expired << " hours, "
      << minutes_expired << " minutes, "
      << seconds_expired << " seconds." << endl << endl;

   pfuExitUtil();
   pfExit();
   exit(0);

}

void exitroutine(void)
//the closure routine for the system
{
   cerr << "Exit routine called!!!\n";
#ifdef DATALOCK
   pfDPoolLock((void *) G_dynamic_data);
#endif
   G_dynamic_data->exit_flag = TRUE;
#ifdef DATALOCK
   pfDPoolUnlock((void *) G_dynamic_data);
#endif

}
