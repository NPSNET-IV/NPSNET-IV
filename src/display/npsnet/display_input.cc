

#include "manager.h"
#include "display_input.h"
#include "draw.h"
#include "entity_const.h"

// Globals used by this library
extern volatile HUD_options   *G_HUD_options;
extern volatile DYNAMIC_DATA  *G_dynamic_data;
extern volatile STATIC_DATA   *G_static_data;
extern volatile MANAGER_REC   *G_manager;




static inline int negate ( volatile int & );


inline int
negate ( volatile int &a_boolean )
{
   if ( a_boolean ) {
      a_boolean = FALSE;
      return FALSE;
   }
   else {
      a_boolean = TRUE;
      return TRUE;
   }
}


void
process_display_input ( pfChannel *chan, int &stat_cycle,
                        int &wireframe, int &texture,
                        int &antialias, int &printscreen )
{
   int counter = 0;
   int shift = FALSE;
   int control = FALSE;
   input_manager *l_input_manager = G_manager->ofinput;

   if ( l_input_manager != NULL ) {

      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_SHIFTKEY);
      shift = (counter > 0);

      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_CONTROLKEY);
      control = (counter > 0);
 
      l_input_manager->query_button(counter,NPS_KEYBOARD,'m');
      if ( counter > 0 ) {
         negate(G_HUD_options->metric);
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'0');
      if ( counter > 0 ) {
         G_HUD_options->color += 1;
         G_HUD_options->color %= NUMCOLORS;
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'1');
      if ( counter > 0 ) {
         negate(G_HUD_options->rotmode);
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'2');
      if ( counter > 0 ) {
         negate(G_HUD_options->colormode);
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'3');
      if ( counter > 0 ) {
         negate(G_HUD_options->iconmode);
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F1KEY);
      if (counter > 0) {
         if (control) {
            pfFStatsClass ( pfGetChanFStats(chan), PFSTATS_ALL,
                           PFSTATS_OFF );
            stat_cycle = 1;
            G_HUD_options->stats = FALSE;
         }
         else {
            G_HUD_options->stats = TRUE;
            if(stat_cycle == 0 ) {
               pfFStatsClass ( pfGetChanFStats(chan), PFSTATS_ALL,
                              PFSTATS_OFF );
               G_HUD_options->stats = FALSE;
               stat_cycle++;
            }
            else if(stat_cycle == 1) {
               pfFStatsClass ( pfGetChanFStats(chan),
                              PFFSTATS_ENPFTIMES,
                              PFSTATS_ON );
               stat_cycle++;
            }
            else if(stat_cycle == 2) {
               pfFStatsClass ( pfGetChanFStats(chan),
                               PFSTATSHW_ENCPU | PFFSTATS_ENPFTIMES,
                               PFSTATS_ON);
               stat_cycle++;
            }
            else if(stat_cycle == 3) {
                pfFStatsClass ( pfGetChanFStats(chan),
                              PFSTATS_ENGFX | PFFSTATS_ENDB |
                              PFFSTATS_ENCULL | PFFSTATS_ENPFTIMES,
                              PFSTATS_ON);
               stat_cycle++;
            }
            else if (stat_cycle == 4) {
               pfFStatsClass ( pfGetChanFStats(chan),
                               PFFSTATS_ENPFTIMES |
                               PFSTATSHW_GFXPIPE_FILL_DEPTHCMP |
                               PFSTATSHW_GFXPIPE_FILL_TRANSPARENT,
                               PFSTATS_ON);
               stat_cycle++;
            }
            else {
               pfFStatsClassMode(pfGetChanFStats(chan), PFSTATS_GFX,
                                    PFSTATS_GFX_ATTR_COUNTS |
                                    PFSTATS_GFX_TSTRIP_LENGTHS,
                                    PFSTATS_ON);
               stat_cycle = 0;
            }
         }
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F2KEY);
      if ( counter > 0 ) {
         negate(G_HUD_options->veh_info);
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F3KEY);
      if ( counter > 0 ) {
         negate(G_HUD_options->misc_info);
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F4KEY);
      if ( counter > 0 ) {
         if ( shift ) {
            if ( G_HUD_options->hud_level > 0 ) {
               G_HUD_options->hud_level = 0;
            }
            else {
               G_HUD_options->hud_level = MAX_HUD_OPTIONS;
            }
         }
         else {
            if ( G_HUD_options->hud_level < MAX_HUD_OPTIONS ) {
               G_HUD_options->hud_level += 1;
            }
            else {
               G_HUD_options->hud_level = 0;
            }
         }
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'-');
      if ( counter > 0 ) {
         if ( (G_HUD_options->hud_level >= ENABLE_RINGS) &&
              (G_HUD_options->range > MIN_RADAR_RANGE) ) {
            G_HUD_options->range -= counter*RADAR_DELTA;
         }
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'+');
      if ( counter > 0 ) {
         if ( (G_HUD_options->hud_level >= ENABLE_RINGS) &&
              (G_HUD_options->range < MAX_RADAR_RANGE) ) {
            G_HUD_options->range += counter*RADAR_DELTA;
         }
      }

//      l_input_manager->query_button(counter,NPS_KEYBOARD,'6');
//      if ( counter > 0 ) {
//         negate ( G_dynamic_data->birdseye );
//      }

      if ( (G_dynamic_data->window_size != WIN_THREE_BUNKER) &&
           (G_dynamic_data->window_size != WIN_TWO_TPORT) &&
           (G_dynamic_data->window_size != WIN_TWO_ADJACENT) &&
           (G_dynamic_data->window_size != WIN_THREE_TV) &&
           (G_dynamic_data->window_size != WIN_FS_MCO) ) {
         l_input_manager->query_button(counter,NPS_KEYBOARD,'v');
         if ( counter > 0 ) {
            if ( G_dynamic_data->visual_mode ) {
               G_dynamic_data->visual_mode = FALSE;
            }
            else if ( !G_dynamic_data->visual_mode &&
                      !G_dynamic_data->visual_active ) {
               G_dynamic_data->visual_mode = TRUE;
            }
         }
      }

      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_TABKEY);
      if ( counter > 0 ) {
         if ( shift ) {
            G_HUD_options->talpha = MAX_ALPHA;
         }
         else {
            G_HUD_options->talpha = (G_HUD_options->talpha + 
                                     (ALPHA_DELTA * counter)) % (MAX_ALPHA+1);
         }
      }

      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F5KEY);
      if ( counter > 0 ) {
         texture = TRUE;
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F6KEY);
      if ( counter > 0 ) {
         wireframe = TRUE;
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F7KEY);
      if ( counter > 0 ) {
         antialias = TRUE;
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_F8KEY);
      if ( counter > 0 ) {
         negate ( G_dynamic_data->stereo );
      }

      l_input_manager->query_button(counter,NPS_KEYBOARD,'n');
      if ( counter > 0 ) {
         G_dynamic_data->fovx -= DELTA_FOV;
         if ( G_dynamic_data->fovx < MIN_FOVX )
            G_dynamic_data->fovx = MIN_FOVX;
         pfChanFOV ( chan, G_dynamic_data->fovx, -1.0f );
      }
      l_input_manager->query_button(counter,NPS_KEYBOARD,'w');
      if ( counter > 0 ) {
         G_dynamic_data->fovx += DELTA_FOV;
         if ( G_dynamic_data->fovx > MAX_FOVX )
            G_dynamic_data->fovx = MAX_FOVX;
         pfChanFOV ( chan, G_dynamic_data->fovx, -1.0f );
      }

      if ( control && G_dynamic_data->birdseye ) {
         l_input_manager->query_button(counter,NPS_KEYBOARD,
                                       NPS_PADUPARROWKEY);
         G_dynamic_data->birdsview.hpr[PF_P] -= (KEY_PITCH_CHANGE*counter);
         l_input_manager->query_button(counter,NPS_KEYBOARD,
                                       NPS_PADDOWNARROWKEY);
         G_dynamic_data->birdsview.hpr[PF_P] += (KEY_PITCH_CHANGE*counter);
         l_input_manager->query_button(counter,NPS_KEYBOARD,
                                       NPS_PADRIGHTARROWKEY);
         G_dynamic_data->birdsview.hpr[PF_H] += (KEY_PITCH_CHANGE*counter);
         l_input_manager->query_button(counter,NPS_KEYBOARD,
                                       NPS_PADLEFTARROWKEY);
         G_dynamic_data->birdsview.hpr[PF_H] -= (KEY_PITCH_CHANGE*counter);
      }

      l_input_manager->query_button(counter,NPS_KEYBOARD,NPS_PRINTSCREENKEY);
      if ( counter > 0 ) {
         printscreen = TRUE;
      }


   }

}

