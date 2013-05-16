

#include <iostream.h>

#include "common_defs2.h"
#include "environ_input.h"
#include "environment_const.h"
#include "timeofday.h"
#include "manager.h"

#ifndef NO_DVW
#define DVW_NPSNET
#define _CSSM
#define _DVW_VERBOSE_
#define _DVW_MULTI_TASKS_
#define _CC_COMPILER_
#define PERFORMER1_2
#include <fstream.h>
#include <string.h>
#include "dvw_util.h"
#include "cloud.h" 
extern "C" void sendCloudPDUtest(void);
extern "C" void toggleHaze(void);
extern "C" void setHazeOn();
extern "C" void toggleVdustFlag(void);
extern "C" void toggleClouds(void);
extern "C" void createNewFlare(void);
extern "C" void deleteAllPlumes(void);
extern "C" void removeAllPlumeID(void);
extern "C" void deleteAllFlares(void);
extern "C" void removeAllFlareID(void);
extern "C" getFlareTexturePtrs(pfTexture **, int *);
extern "C" void getCloudTexturePtrs(pfTexture **, int *);

static void process_dvw_input(void);
extern  STATIC_DATA *G_static_data;
#endif

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile  MANAGER_REC *G_manager;
#ifndef NOSUB
extern volatile int           G_drivenveh;
extern          VEH_LIST     *G_vehlist;
#endif // NOSUB



// Static local subroutines
static void update_clouds ( CHANNEL_DATA * );
static void update_time ( CHANNEL_DATA * );
static void update_fog ( CHANNEL_DATA * );
static void update_smoke_color ();


void
modify_clouds ( CHANNEL_DATA *chan_data )
{
   pfESkyAttr ( chan_data->earth_sky, PFES_CLOUD_TOP,
                chan_data->cloud_top );
   pfESkyAttr ( chan_data->earth_sky, PFES_CLOUD_BOT,
                chan_data->cloud_bottom );
   if ( chan_data->cloud_top > chan_data->cloud_bottom ) {
      pfESkyAttr ( chan_data->earth_sky, PFES_TZONE_TOP,
                   chan_data->cloud_top+TZONE);
      pfESkyAttr ( chan_data->earth_sky, PFES_TZONE_BOT,
                   chan_data->cloud_bottom-TZONE);
   }
   else {
      pfESkyAttr ( chan_data->earth_sky, PFES_TZONE_TOP,
                   chan_data->cloud_top);
      pfESkyAttr ( chan_data->earth_sky, PFES_TZONE_BOT,
                   chan_data->cloud_bottom);
   }
}


void
modify_fog ( CHANNEL_DATA *chan_data )
{
   if ( !G_dynamic_data->fog) {
      pfESkyFog ( chan_data->earth_sky, PFES_GENERAL, NULL );
   }
   else {
      pfFogRange ( chan_data->fog_ptr, chan_data->fog_plane.near,
                   chan_data->fog_plane.far );
      pfESkyFog ( chan_data->earth_sky, PFES_GENERAL, chan_data->fog_ptr );
   }

}


void
update_clouds ( CHANNEL_DATA *chan_data )
{
   int counter = 0;
   int clouds_updated = FALSE;
   input_manager *l_input_manager = G_manager->ofinput;

   // **********************************************************
   // CLOUD OPTIONS
   // **********************************************************

#ifdef NO_DVW
   l_input_manager->query_button(counter,NPS_KEYBOARD,'c');
   if ( counter > 0 ) {
      clouds_updated = TRUE;
      G_dynamic_data->clouds = !G_dynamic_data->clouds;
      if ( G_dynamic_data->clouds ) {
         chan_data->cloud_bottom = 
            chan_data->cloud_top - chan_data->cloud_thick;
      } 
      else {
         chan_data->cloud_bottom = chan_data->cloud_top;
      }
   }
#endif

   l_input_manager->query_button(counter,NPS_KEYBOARD,'g');
   if ( counter > 0 ) {
      if (G_dynamic_data->clouds) {
         if ((chan_data->cloud_top + CLOUD_HT_CHANGE) <= 1980.0f) {
            chan_data->cloud_top += CLOUD_HT_CHANGE;
            chan_data->cloud_bottom += CLOUD_HT_CHANGE;
            clouds_updated = TRUE;
         }
         else {
            if (chan_data->cloud_top < 1980.0f) {
               chan_data->cloud_top = 1980.0f;
               chan_data->cloud_bottom = 1980.0f - chan_data->cloud_thick;
               clouds_updated = TRUE;
            }
         }
      }
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'h');
   if ( counter > 0 ) {
      if (G_dynamic_data->clouds) {
         if ((chan_data->cloud_bottom - CLOUD_HT_CHANGE) >= 500.0f) {
            chan_data->cloud_top -= CLOUD_HT_CHANGE;
            chan_data->cloud_bottom -= CLOUD_HT_CHANGE;
            clouds_updated = TRUE;
         }
         else {
            if (chan_data->cloud_bottom > 500.0f) {
               chan_data->cloud_top = 500.0f + chan_data->cloud_thick;
               chan_data->cloud_bottom = 500.0f;
               clouds_updated = TRUE;
            }
         }
      }
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'j');
   if ( counter > 0 ) {
      if (G_dynamic_data->clouds) {
         if ((chan_data->cloud_top + CLOUD_HT_CHANGE) <= 1980.0f) {
            chan_data->cloud_top += CLOUD_HT_CHANGE;
            chan_data->cloud_bottom += CLOUD_HT_CHANGE;
            clouds_updated = TRUE;
         }
         else
         {
            if (chan_data->cloud_top < 1980.0f) {
               chan_data->cloud_top = 1980.0f;
               chan_data->cloud_bottom = 1980.0f - chan_data->cloud_thick;
               clouds_updated = TRUE;
            }
         }
      }
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'k');
   if ( counter > 0 ) {
      if (G_dynamic_data->clouds) {
         if ((chan_data->cloud_bottom - CLOUD_HT_CHANGE) >= 500.0f) {
            chan_data->cloud_top -= CLOUD_HT_CHANGE;
            chan_data->cloud_bottom -= CLOUD_HT_CHANGE;
            clouds_updated = TRUE;
         }
         else {
            if (chan_data->cloud_bottom > 500.0f) {
               chan_data->cloud_bottom = 500.0f;
               chan_data->cloud_top = 500.0f +  chan_data->cloud_thick;
               clouds_updated = TRUE;
            }
         }
      }
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'y');
   if ( counter > 0 ) {
      if (G_dynamic_data->clouds)
      {
         if ((chan_data->cloud_thick < 1000.0f) && 
             (chan_data->cloud_top <= 1930.0f)) {
            chan_data->cloud_top += CLOUD_THICK_CHANGE;
            chan_data->cloud_bottom -= CLOUD_THICK_CHANGE;
            chan_data->cloud_thick += 2 * CLOUD_THICK_CHANGE;
            clouds_updated = TRUE;
         }
         else {
            if (chan_data->cloud_thick < 1000.0f) {
               chan_data->cloud_bottom -= (2*CLOUD_THICK_CHANGE) - 
                                     (1980.0f - chan_data->cloud_top);
               chan_data->cloud_top += (1980.0f - chan_data->cloud_top);
               chan_data->cloud_thick += 2 * CLOUD_THICK_CHANGE;
               clouds_updated = TRUE;
            }
         }
      }
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'u');
   if ( counter > 0 ) {
      if (G_dynamic_data->clouds) {
         if (chan_data->cloud_thick > 500.0f) {
            chan_data->cloud_top -= CLOUD_THICK_CHANGE;
            chan_data->cloud_bottom += CLOUD_THICK_CHANGE;
            clouds_updated = TRUE;
         }
      }
   }

   if ( clouds_updated ) {
      modify_clouds ( chan_data );
   }

}


void
update_time ( CHANNEL_DATA *chan_data )
{
   static int more_light = TRUE;
   int time_updated = FALSE;
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;

   // **********************************************************
   // TIME OPTIONS
   // **********************************************************

   l_input_manager->query_button(counter,NPS_KEYBOARD,'t');
   if ( counter > 0 ) {
      time_updated = TRUE;
      if ( more_light ) {
         G_dynamic_data->tod += TOD_DELTA;
         if ( G_dynamic_data->tod > TOD_MAX ) {
            G_dynamic_data->tod = TOD_MAX;
            more_light = FALSE;
         }
      }
      else {
         G_dynamic_data->tod -= TOD_DELTA;
         if ( G_dynamic_data->tod < TOD_MIN ) {
            G_dynamic_data->tod = TOD_MIN;
            more_light = TRUE;
         }
      }
   }

   if ( time_updated ) {
      updatetod (  G_dynamic_data->tod, chan_data );
   }

}


void
update_fog ( CHANNEL_DATA *chan_data )
{

   int fog_updated = FALSE;
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;
#ifndef NOSUB
   pfVec3 tempheightofeye;
   if (G_vehlist[G_drivenveh].vehptr->gettype() == SUBMERSIBLE) {
      //determine height of eye 
      pfCopyVec3(tempheightofeye,chan_data->view.xyz);
      //if height of eye is above water set for to earth sky.  
      if(tempheightofeye[Z] > 0.5)
         pfESkyFog ( chan_data->earth_sky, PFES_GENERAL, NULL );
   }
#endif // NOSUB
   // **********************************************************
   // FOG OPTIONS
   // **********************************************************

#ifdef NO_DVW
   l_input_manager->query_button(counter,NPS_KEYBOARD,'f');
   if ( counter > 0 ) {
      fog_updated = TRUE;
      G_dynamic_data->fog = !G_dynamic_data->fog;
   }
#endif

   l_input_manager->query_button(counter,NPS_KEYBOARD,'<');
   if ( counter > 0 ) {
      if ( chan_data->fog_plane.far <= MAX_FOG + DELTA_FOG ) {
         chan_data->fog_plane.far += DELTA_FOG;
         fog_updated = TRUE;
      }
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'>');
   if ( counter > 0 ) {
     if ( chan_data->fog_plane.far >= chan_data->fog_plane.near + DELTA_FOG ) {
        chan_data->fog_plane.far -= DELTA_FOG;
        fog_updated = TRUE;
     }
   }
#ifndef NOSUB
   if (G_vehlist[G_drivenveh].vehptr->gettype() == SUBMERSIBLE) {
      //height of eye is under water so set for water fog
      //fog distance gets less as a function of depth
      if (tempheightofeye[Z] < 0.5)
       {  
        chan_data->waterfog_plane.near = 3000 +(3.0 * tempheightofeye[Z]);
        if ( chan_data->waterfog_plane.near <= 1.0)
           chan_data->waterfog_plane.near = 1.0;

        chan_data->ground_height = -1200.0;
        pfFogColor ( chan_data->waterfog_ptr, 0.0, 0.4, 0.8 );
        pfFogRange ( chan_data->waterfog_ptr, chan_data->waterfog_plane.near,
                  chan_data->fog_plane.far );
        pfESkyAttr ( chan_data->earth_sky, 
                  PFES_GRND_HT, chan_data->ground_height);
           
        pfESkyMode ( chan_data->earth_sky, PFES_BUFFER_CLEAR, PFES_SKY_GRND );      
        pfESkyColor(chan_data->earth_sky, PFES_GRND_FAR, 0.7, 0.4, 0.0, 1.0);
        pfESkyColor(chan_data->earth_sky, PFES_GRND_NEAR, 1.0, 0.8, 0.0, 1.0);
        pfESkyFog ( chan_data->earth_sky, PFES_GENERAL, chan_data->waterfog_ptr );
	         
        fog_updated = TRUE;
       }
    }
#endif // NOSUB
   if ( fog_updated ) {
      modify_fog ( chan_data );
   }

}


void
update_smoke_color()
{
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;

   l_input_manager->query_button(counter,NPS_KEYBOARD,'q');
   if ( counter > 0 ) {
      G_dynamic_data->specific =
         (G_dynamic_data->specific + 1) % MAX_SMOKE_COLORS;
   }
}



void
update_environment ( CHANNEL_DATA *chan_data )
{
   update_clouds ( chan_data );
   update_time ( chan_data );
   update_fog ( chan_data );
   update_smoke_color();
#ifndef NO_DVW
   process_dvw_input();
#endif
}


#ifndef NO_DVW
static void
process_dvw_input ()
{
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;
   int shift_pressed = FALSE;


   l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_SHIFTKEY);
   shift_pressed = ( counter > 0 );

   l_input_manager->query_button(counter,NPS_KEYBOARD,'c');
   if ( counter > 0 ) {
      toggleClouds();
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'C');
   if ( counter > 0 ) {
       G_dynamic_data->cloud_primitive_switch=1; 
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'f');
   if ( counter > 0 ) {
      toggleHaze();
      setHazeOn();
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'p');
   if ( counter > 0 ) {
      TOGGLE_DVW_PRINT_SWITCH();
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'P');
   if ( counter > 0 ) {  // get rid of all combic plumes at will
      deleteAllPlumes();
      removeAllPlumeID();
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,'E');
   if ( counter > 0 ) {
      toggleVdustFlag();
   }

   l_input_manager->query_button(counter,NPS_KEYBOARD,' ');
   if ( shift_pressed && (counter > 0) ) {
      createNewFlare();
   }

}

void DVWExit()
{
   deleteAllPlumes();
   removeAllPlumeID();

   deleteAllFlares(); 
   removeAllFlareID();

   // Cloud Process 
   cloudClean();

}

void getDVWTexturePtrs()
{
   int texture_count = 0;
   const int MAX_DVW_TEXTURES = 15;
   pfTexture *texPtrs[MAX_DVW_TEXTURES] = {NULL};

   // Now get all those texture ptrs from DVW
   getCloudTexturePtrs(&texPtrs[0],  &texture_count);
   for (int ix = 0;ix< texture_count;ix++) {
      G_dynamic_data->texture_list_ptr[G_dynamic_data->number_of_textures++]
              = texPtrs[ix];
   }

   getFlareTexturePtrs(&texPtrs[0],  &texture_count);
   for (ix = 0;ix< texture_count;ix++) {
      G_dynamic_data->texture_list_ptr[G_dynamic_data->number_of_textures++]
           = texPtrs[ix];
   }

}


void setDVWEnvPaths(char **filePaths)
{
   char *envptr, env_var[FILENAME_SIZE], 
        path[FILENAME_SIZE], string[FILENAME_SIZE];
   ifstream DVWfile(G_static_data->dvw_file);

   if(!DVWfile) {
      cerr << "Unable to open DVW environment parameters file "
           << envptr << endl;
      exit(-1);
   }
   else {
      cerr << "     Reading DVW file: " << G_static_data->dvw_file << endl;
   }

   int ix = 0;
   while(DVWfile >> env_var >> path) {
      strcpy(string,env_var);
      strcat(string,"=");
      strcat(string,path);
      // allocate space here -- space deallocated before main() exits
      filePaths[ix] = new char[strlen(string) + 1];
      strcpy(filePaths[ix],string);
      putenv(filePaths[ix]);
      ix++;
   }
   DVWfile.close();

}

#endif
