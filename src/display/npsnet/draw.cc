// File: <draw.cc>

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
#include <string.h>
#include <fstream.h>
#include <iostream.h>
#include <fmclient.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <device.h>
#include <get.h>
#include <pfutil.h>

#include "draw.h"
#include "display_input.h"

#include "manager.h"
#include "common_defs2.h"
#include "appearance.h"
#include "input.h"
#include "intro.h"
#include "conversion_const.h"
#include "screen_dump.h"
#include "fontdef.h"

#ifndef XMAXSCREEN
#define XMAXSCREEN 1279
#endif

#ifndef YMAXSCREEN
#define YMAXSCREEN 1023
#endif

#include "dbPagingCallback.h"

#ifndef NOC2MEASURES
#include "highlight.h"
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
# include "cloud.h"
# include <sys/types.h>
# include <sys/sysmp.h>
# include <sys/schedctl.h>
#endif

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#endif

// Globals used by this library
extern volatile DYNAMIC_DATA  *G_dynamic_data;
extern volatile STATIC_DATA   *G_static_data;
extern volatile SUN           *G_sun;
extern volatile HUD_options   *G_HUD_options;
extern volatile  MANAGER_REC  *G_manager;
extern volatile DB_SHARE      *dbShare;

extern pfLight *G_logo_light;

static void notice ( pfChannel * );
void draw_string_at_bottom ( pfChannel *chan, char *the_string );


void hud_mode ()
{
   static int first_time = TRUE;
   static pfMatrix identity;

   if ( first_time ) {
      pfMakeIdentMat ( identity );
      first_time = FALSE;
      }

   // Save Performer state and load identity matrix
   pfPushState();
   pfBasicState();

/* GONE
   pushmaxtix ();
   loadmatrix(identity);
*/

   pfPushMatrix();
   pfLoadMatrix (identity);
#ifdef IRISGL
   zfunction(ZF_ALWAYS);
#else // OPENGL
//    glDepthFunc(GL_ALWAYS);
#endif

}


void performer_mode ()
{
   // Reset Performer state
/* GONE
   popmatrix();
*/

   pfPopMatrix ();

#ifdef IRISGL
   zfunction(ZF_LEQUAL);
#else // OPENGL
//   glDepthFunc(GL_LEQUAL);
#endif   

   pfPopState();
}


void notice ( pfChannel *chan )
{
   static int first_time = TRUE;
#ifdef IRISGL
   static char text[] = { 78, 80, 83, 78, 69, 84, 0 };
   static fmfonthandle display_font;
   static fmfonthandle original_font;
#else // OPENGL
   pfuXFont *display_font;
//   static pfuXFont original_font;
   static char text[] = { 78, 80, 83, 78, 69, 84, 0 };
#endif
   static int font_size;
   static int last_screen_x = -1.0f;
   static int last_screen_y = -1.0f;
   static float text_size;
   static float starting_x;
   static float starting_y;
   int screen_x, screen_y;
   long tp_mode, aa_mode;
   int stereo = G_dynamic_data->stereo;

   if ( first_time ) {
#ifdef IRISGL
      original_font = fmfindfont("Times-Roman");
#else // OPENGL
//      pfuLoadXFont ("-*-Times-Roman-normal--*-*-*-*-*-*-iso8859-1",
//                    &original_font);
#endif
      first_time = FALSE;
   }

   hud_mode();

   pfGetChanSize ( chan, &screen_x, &screen_y );

#ifdef IRISGL
   ortho2(0.0f, (float)screen_x, 0.0f, (float)screen_y);
#else // OPENGL
   int gl_matrix_mode;
   glGetIntegerv (GL_MATRIX_MODE, &gl_matrix_mode);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0.0f, (float)screen_x, 0.0f, (float)screen_y);
   glMatrixMode(gl_matrix_mode);
#endif
   if ( (last_screen_x != screen_x) || (last_screen_y != screen_y) ) {
      font_size = ( 0.029296875f * screen_y );
#ifdef IRISGL
      display_font = fmscalefont ( original_font, font_size );
      text_size = float(fmgetstrwidth(display_font,text));
#else // OPENGL
      npsFont(TEXTFONT);
      display_font = npsGetCurXFont();
      text_size = npsGetXFontWidth(display_font,text);
#endif

      starting_x = (screen_x - text_size)/2.0f;
      if ( stereo )
         starting_y = (screen_y - 4.0f*font_size);
      else 
         starting_y = (screen_y - 2.0f*font_size);
      last_screen_x = screen_x;
      last_screen_y = screen_y;
   }

   if ( (G_dynamic_data->window_size != WIN_TWO_TPORT) &&
        (G_dynamic_data->window_size != WIN_FULL) ) {
      tp_mode = pfGetTransparency();
      pfOverride(PFSTATE_TRANSPARENCY, PF_OFF );
      pfTransparency(PFTR_ON);
      pfOverride(PFSTATE_TRANSPARENCY, PF_ON );
   }

   aa_mode = pfGetAntialias();

/*   if (aa_mode == PFAA_ON) {
      cerr << "AA_MODE is: ON" << endl;
   }
   else {
      cerr << "AA_MODE is: OFF" << endl;
   } */

#ifdef IRISGL
   fmsetfont(display_font);
#else
   npsFont(TEXTFONT);
#endif
   

   if ( aa_mode == PFAA_ON ) {
#ifdef IRISGL
      cpack(0x20000080);
#else // OPENGL
      //glColor4i(int(0x80),int(0x00),int(0x00),int(0x20));
      glColor4f(0.5f,0.0f,0.0f,0.125f);
#endif
   }
   else {
#ifdef IRISGL
      cpack (0x1f000080);
#else // OPENGL
      //glColor4i(int(0x80),int(0x00),int(0x00),int(0x1f));
      glColor4f(0.5f,0.0f,0.0f,0.121f);
#endif
   }
#ifdef IRISGL
   cmov2 ( starting_x + 2.0f, starting_y );
   fmprstr(text);
#else // OPENGL
   npsDrawStringPos (text, starting_x + 4.0f, starting_y, 0.0f);
#endif

   if ( aa_mode == PFAA_ON ) {
#ifdef IRISGL
      cpack (0x2000D7FF);
#else // OPENGL
      //glColor4i(int(0xFF),int(0xD7),int(0x00),int(0x20));
      glColor4f(1.0f,0.84f,0.0f,0.125f);
#endif
   }
   else {
#ifdef IRISGL
      cpack (0x1f00D7FF);
#else
      //glColor4i(int(0xFF),int(0xD7),int(0x00),int(0x1f));
      glColor4f(1.0f,0.84f,0.0f,0.121f);
#endif
   }
#ifdef IRISGL
   cmov2 ( starting_x, starting_y - 2.0f );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos (text, starting_x, starting_y - 2.0f, 0.0f);
#endif
   
   if ( (G_dynamic_data->window_size != WIN_TWO_TPORT) &&
        (G_dynamic_data->window_size != WIN_FULL) ) {
      pfOverride(PFSTATE_TRANSPARENCY, PF_OFF );
      pfTransparency(tp_mode);
      pfOverride(PFSTATE_TRANSPARENCY, PF_ON );
   }

   performer_mode();

}

void draw_process ( pfChannel *chan, void *data )
{
   static int hmd;
   static int hud;
   static int veh_info;
   static int misc_info;
//   static int num_samples = 0; 
   static int full_screen = FALSE;
   static int first_time = 1;
//   static int done = FALSE;
   static long mypid;
   static int show_textures;
   static int aa_mode;
   static int wf_mode;
   static int tx_mode;
   PASS_DATA *pdata = (PASS_DATA *)data;
   input_manager *l_input_manager = G_manager->ofinput;
#ifndef NO_PFSMOKE
   pfCoord eye;
#endif
#ifndef NO_DVW
   int  cloud_primitive_switch;
#endif

   if (first_time)
      {
      first_time = FALSE;
      mypid = getpid();

#ifndef NOC2MEASURES
      G_dynamic_data->HL = NewHighlight();
#endif

#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
      hmd = G_dynamic_data->hmd;
      full_screen = ((G_dynamic_data->view.left == 0) &&
                     (G_dynamic_data->view.right >= XMAXSCREEN) &&
                     (G_dynamic_data->view.bottom == 0) &&
                     (G_dynamic_data->view.top >= YMAXSCREEN));
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

      hud = D_HUD_MODE;
      veh_info = (hmd == FALSE);
      misc_info = (hmd == FALSE);

//      num_samples = int(getgconfig(GC_MS_SAMPLES));
    
      pfNotify ( PFNFY_NOTICE, PFNFY_PRINT,
                 "Draw Process PID is: %d\n", mypid );

      if ( G_static_data->waypoints )
         {
         umask(000);
         G_static_data->waypnt_fd = creat ( "/usr/tmp/NPSNETIV_waypoints",
                                                       00666 );
         if ( G_static_data->waypnt_fd < 0 )
            cerr << "     Waypoint file, /usr/tmp/NPSNETIV_waypoints, could not"
                 << " be created.\n";
         else
            write(G_static_data->waypnt_fd,
                  "#   X        Y        Z    HEADING   PITCH    ROLL\n",
                  51);
         }

/* New Opening Screen (4/10/96) Only execute after everything loaded
      done = FALSE;
//      while ( !done )
//         {
//         done = print_next_event();
//         }
      int event = INIT_PERFORMER;
      while ((G_dynamic_data->init_event < STARTING_DRAW) &&
             (G_static_data->process_mode & PFMP_FORK_DRAW)) {
         if (event != (int)G_dynamic_data->init_event) {
            print_next_event();
            event = (int)G_dynamic_data->init_event;
         }
         else {
cerr<<"Draw Waiting...\r";
         }
      }
*/
      // preload the textures into the texture memory if on a reality engine

      int tmp;
      pfQueryFeature(PFQFTR_TEXTURE, &tmp);

      if ( tmp == PFQFTR_FAST ) {

#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         G_dynamic_data->init_event = LOADING_TEXTURES;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

         //print_next_event();

         aa_mode = ( (pfGetAntialias()) == PFAA_ON );
         wf_mode = ( (pfGetEnable(PFEN_WIREFRAME)) == PF_ON );

         pfEnable(PFEN_TEXTURE);
         tx_mode = TRUE;

//         else if (!(tmp == PFQFTR_FAST) && (tx_mode)) {
//            pfDisable(PFEN_TEXTURE);
//            tx_mode = FALSE;
//         }

         show_textures = (G_static_data->pictures && 
                          tx_mode &&
                          full_screen);
         if (dbShare->okToDownLoadTextures) {
            int textNum = pfGetNum (dbShare->tsgInfo->texList);
            for (int ix = 0; ix < textNum; ix++) {
                G_dynamic_data->
                   texture_list_ptr[G_dynamic_data->number_of_textures++]
                = (pfTexture *) pfGet (dbShare->tsgInfo->texList, ix);
            }
         }

         DownloadTex(G_dynamic_data->number_of_textures,
                     G_dynamic_data->texture_list_ptr, show_textures);
//         if ((show_textures) && dbShare->okToDownLoadTextures) {
//            pfuDownloadTexList (dbShare->tsgInfo->texList, PFUTEX_SHOW);
//         }
       }
#ifdef IRISGL
       RGBcolor(0,0,128);
       clear();
       swapbuffers();
       RGBcolor(0,0,128);
       clear();
       swapbuffers();
#else // OPENGL
//       glClearColor (0.0f, 0.0f, float(128/255), 0.0f);
//       glClear (GL_COLOR_BUFFER_BIT);
//       pfSwapPWinBuffers (G_dynamic_data->pw);
//       glClearColor (0.0f, 0.0f, float(128/255), 0.0f);
//       glClear (GL_COLOR_BUFFER_BIT);
//       pfSwapPWinBuffers (G_dynamic_data->pw);
#endif
        
      
      // Flag the application process to continue
#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
      G_dynamic_data->init_event = LAST_INIT_EVENT;  
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

      // This is a fix for the Performer smoke/flames routines that
      // Come up textured on non-capable platforms no matter what
      // Performer has already decided the basic state should be.
      if ( !tx_mode )
         {
         pfDisable(PFEN_TEXTURE);
         pfOverride(PFSTATE_ENTEXTURE|PFSTATE_TEXTURE, 1);
         }

      pfNotify ( PFNFY_DEBUG, PFNFY_PRINT,
                 "Draw Process -- END first time code." );

      } /* first time */
   else {

   if ( G_dynamic_data->window_size == WIN_FS_ST_BUFFERS ) {
#ifdef IRISGL
      leftbuffer(1);
      rightbuffer(0);
#else // OPENGL
       glDrawBuffer(GL_BACK_LEFT);
#endif
   }

#ifndef NOC2MEASURES
      //SAK.  Need to update the highlight.  Must be doen here in the draw
      // process.

      UpdateHighlight(G_dynamic_data->HL);                //SAK.
#endif

#ifndef NO_DVW
#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
      cloud_primitive_switch = G_dynamic_data->cloud_primitive_switch;
      G_dynamic_data->cloud_primitive_switch = 0;
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
#endif

 
      if ( G_sun->changed )
         {
         pfLightPos(G_sun->lt, G_sun->px, G_sun->py, G_sun->pz, 0.0f);
//         pfLightAmbient(G_sun->lt, G_sun->ltamb, G_sun->ltamb, G_sun->ltamb);
         pfLightColor (G_sun->lt, PFMTL_AMBIENT,
                       G_sun->lmamb, G_sun->lmamb, G_sun->lmamb);
         pfLModelAmbient(G_sun->lm, G_sun->lmamb, G_sun->lmamb, G_sun->lmamb);
         
         pfLightColor(G_sun->lt, PFLT_AMBIENT,
                      G_sun->ltcol, G_sun->ltcol, G_sun->ltcol);
         pfApplyLModel ( G_sun->lm );
         pfNotify ( PFNFY_DEBUG, PFNFY_PRINT,
                    "Draw Process -- updated the SUN." );
         G_sun->changed = FALSE;
         }
      
      pfLightOn ( G_sun->lt );

      if (pdata->antialias)
         {
         aa_mode = !aa_mode;
         if ( aa_mode ) {
            pfAntialias(PFAA_ON);
         }
         else {
            pfAntialias(PFAA_OFF);
         }
//         num_samples = int(getgconfig(GC_MS_SAMPLES));
         pfNotify ( PFNFY_DEBUG, PFNFY_PRINT,
                    "Draw Process -- changed antialias mode." );
         }

      if (pdata->wireframe)
         {
         wf_mode = !wf_mode;
         if ( wf_mode )
            pfEnable(PFEN_WIREFRAME);
         else
            pfDisable(PFEN_WIREFRAME);
         pfNotify ( PFNFY_DEBUG, PFNFY_PRINT,
                    "Draw Process -- changed wireframe mode." );
         }

      if ( wf_mode )
         pfClear ( PFCL_COLOR, NULL );

      if (pdata->texture)
         {
         tx_mode = !tx_mode;
         if ( tx_mode )
            {
            pfOverride(PFSTATE_ENTEXTURE|PFSTATE_TEXTURE, 0);
            pfEnable(PFEN_TEXTURE);
            } 
         else
            {
            pfDisable(PFEN_TEXTURE);
            pfOverride(PFSTATE_ENTEXTURE|PFSTATE_TEXTURE, 1);
            }
         pfNotify ( PFNFY_DEBUG, PFNFY_PRINT,
                    "Draw Process -- changed texture mode." );
         }

#ifdef IRISGL
//      leftbuffer(TRUE);
//      rightbuffer(TRUE);
#else // OPENGL
//      glDrawBuffer(GL_LEFT);
//      glDrawBuffer(GL_RIGHT);
#endif

      pfClearChan ( chan );
      pfDraw();

#ifndef NO_DVW
      {
         float fov[2];
   
         fov[0] = G_dynamic_data->fovx;
         fov[1] = G_dynamic_data->fovy;
         draw_vis_cloud( cloud_primitive_switch, pdata->posture.xyz,
                        (float) G_dynamic_data->clip_plane.far, fov );
      }
#endif

#ifndef NO_PFSMOKE
          pfGetChanView(chan,eye.xyz,eye.hpr);
          pfuDrawSmokes(eye.xyz);
#endif

      if ( G_HUD_options->stats )
         {
         pfDrawChanStats ( chan );
         }


      if ( G_dynamic_data->hud_callback != NULL ) {
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         hud_mode();
         G_dynamic_data->hud_callback ( chan, data );
         performer_mode();
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
      }
   
      notice ( chan ) ;
   
      if ( pdata->printscreen ) {
         print_gl_window_to_file ((char *)G_static_data->pic_dir,
                                  "NPSNET_SCREEN",1);
      }

   } // Not first time - general case

}


void
basic_draw ( pfChannel *chan, void * )
{
   if ( G_dynamic_data->window_size == WIN_FS_ST_BUFFERS ) {
#ifdef IRISGL
      leftbuffer(0);
      rightbuffer(1);
#else // OPENGL
       glDrawBuffer(GL_BACK_RIGHT);
#endif
   }

   pfLightOn ( G_sun->lt );
   pfClearChan ( chan );
   pfDraw();
#ifndef NO_PFSMOKE
   pfCoord eye;
   pfGetChanView(chan,eye.xyz,eye.hpr);
   pfuDrawSmokes(eye.xyz);
#endif
   notice ( chan );
}

void
draw_string_at_bottom ( pfChannel *chan, char *the_string )
{
   int screen_x, screen_y;
   int font_height;
   float temp_size;
#ifdef IRISGL
   static fmfonthandle deadfont;
   static int first_time = 1;
#endif
#ifdef OPENGL
  pfuXFont *display_font;
  int gl_matrix_mode;
#endif

   hud_mode();
   pfGetChanSize ( chan, &screen_x, &screen_y );

   font_height = (45.0f * (screen_y/1024.0f));

#ifdef IRISGL
   if ( first_time ) {
      deadfont = fmscalefont(fmfindfont("Times-Roman"), font_height);
      first_time = 0;
   }
   ortho2(0.0f, (float)screen_x, 0.0f, (float)screen_y);

   fmsetfont(deadfont);
   temp_size = float(fmgetstrwidth(deadfont,the_string));
   cmov2 ( (screen_x - temp_size)/2.0f + 2.0f,
           (font_height + 7.0f) );
   RGBcolor(0,0,128);
   fmprstr(the_string);
   cmov2 ( (screen_x - temp_size)/2.0f,
           (font_height + 5.0f) );
   RGBcolor(255,215,0);
   fmprstr(the_string);
#else //OPENGL
   glGetIntegerv (GL_MATRIX_MODE, &gl_matrix_mode);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0.0f, (float)screen_x, 0.0f, (float)screen_y);
   glMatrixMode(gl_matrix_mode);
   npsFont(TEXTFONT);
   display_font = npsGetCurXFont();
   temp_size = npsGetXFontWidth(display_font,the_string);
   glColor4f(0.0f, 0.0f, 0.5f, 0.0f);
   npsDrawStringPos(the_string, (screen_x - temp_size)/2.0f + 2.0f,
                          (font_height + 7.0f), 0.0f);
   glColor4f(1.0f, 0.84f, 0.0f, 0.0f);
   npsDrawStringPos(the_string, (screen_x - temp_size)/2.0f,
                          (font_height + 5.0f), 0.0f);
   npsFont(DEFFONT);

#endif

   performer_mode(); 

}

void
logo_draw ( pfChannel *chan, void * )
{
   char text[255];

   pfLightOn ( G_logo_light );
   pfLightPos ( G_logo_light, 0.0f, 0.0f, 0.0f, 0.0f );
   pfPushIdentMatrix();
   pfLightOn ( G_logo_light );
   pfPopMatrix();
   pfClearChan ( chan );
   pfDraw();
   notice ( chan );
   if ( G_dynamic_data->tethered ) {
     if ( G_dynamic_data->tethered == 1 )
        sprintf ( text, "TETHERED" );
     else if ( G_dynamic_data->tethered == 2 )
        sprintf ( text, "ATTACHED" );
     else if ( G_dynamic_data->tethered == 3 )
        sprintf ( text, "MIMIC" );
     else if ( G_dynamic_data->tethered == 4 )
        sprintf ( text, "CAMERA LOCK" );
     else if ( G_dynamic_data->tethered == 5 )
        sprintf ( text, "STEALTH ZOOM" );
     else
        sprintf ( text, "UNKOWN TETHER");
     if (G_dynamic_data->useAuto) {
        sprintf (text, "%s+", text);
     }
     draw_string_at_bottom ( chan, text ); 
   }
   else if (G_dynamic_data->useAuto) {
      sprintf (text, "Auto On");
      draw_string_at_bottom ( chan, text );
   }
}


void
aim_view_draw ( pfChannel *chan, void * )
{
   pfLightOn ( G_sun->lt );
   pfClearChan ( chan );
   pfDraw();
#ifndef NO_PFSMOKE
   pfCoord eye;
   pfGetChanView(chan,eye.xyz,eye.hpr);
   pfuDrawSmokes(eye.xyz);
#endif
   hud_mode();
   draw_aim_lines(chan);
   performer_mode();
   notice ( chan );
}


void
basic_draw_left ( pfChannel *chan, void * )
{
   pfLightOn ( G_sun->lt );
   pfClearChan ( chan );
   pfDraw();
#ifndef NO_PFSMOKE
   pfCoord eye;
   pfGetChanView(chan,eye.xyz,eye.hpr);
   pfuDrawSmokes(eye.xyz);
#endif
   notice ( chan );
}

void
basic_draw_right ( pfChannel *chan, void * )
{
   pfLightOn ( G_sun->lt );
   pfClearChan ( chan );
   pfDraw();
#ifndef NO_PFSMOKE
   pfCoord eye;
   pfGetChanView(chan,eye.xyz,eye.hpr);
   pfuDrawSmokes(eye.xyz);
#endif
   notice ( chan );
}



void draw_information ( pfChannel *chan, void *data )
   {
   static int first_time = TRUE;
   static float view_right;
   static float view_left; 
   static float view_top; 
   static float view_bottom;
   static float right; 
   static float top; 
   static float left; 
   static float bottom;
   static INFO_PDATA *info_pdata;
#ifdef IRISGL
   static char text[255];
   float real_heading;
   static fmfonthandle our_font;
#else //OPENGL
   static char text[255];
   float real_heading;
   static pfuXFont *display_font;
#endif
   float line, column;
   static float font_height;
   static float font_width;
   long orignal_trans_mode = pfGetTransparency();

   if ( first_time )
      {
      float width, height,temp;
      /* This twisted convoluted next statement is just to use */
      /* the chan parameter to keep the compiler happy */
      if ( chan == NULL )
         first_time = FALSE;
      else
         first_time = FALSE;
      width = (G_dynamic_data->view.right - G_dynamic_data->view.left);
      height = (G_dynamic_data->view.top - G_dynamic_data->view.bottom);
      view_top = G_dynamic_data->view.top;
      temp = (0.667f * height);
      view_bottom = G_dynamic_data->view.bottom + temp;
      view_right = G_dynamic_data->view.right;
      view_left = G_dynamic_data->view.right - (temp*(height/width));

      
      font_height = 
         ((view_top - view_bottom) * 14.0f) / 341.0f;
#ifdef IRISGL
      our_font = fmscalefont(fmfindfont("Fixed"), font_height);
      font_width = float(fmgetstrwidth(our_font,"1, 2, 333, 4, 55, 6"))/19.0f;
#else //OPENGL
      npsFont(DEFFONT);
      display_font = npsGetCurXFont();
      font_width = npsGetXFontWidth(display_font,"1, 2, 333, 4, 55, 6")/19.0f;
#endif
      right = (view_right - view_left)/(font_width);
      top = (view_top - view_bottom)/(1.3f * font_height);
      left = 0.0f;
      bottom = 0.0f;
      }

   info_pdata = (INFO_PDATA *)data;

   hud_mode();

#ifdef IRISGL
   ortho2 ( left, right, bottom, top );
#else // OPENGL
   // This is here because OpenGL handles the MODELVIEW & PROJECTION matrixes
   // differently then does IRISGL
   // OpenGL multiplys the matrices using gluOrtho, but GL does not.
   // Therefore must load identity on PROJECTION matrix before gluOrtho
   int gl_matrix_mode;
   glGetIntegerv (GL_MATRIX_MODE, &gl_matrix_mode);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D( left, right, bottom, top );
#endif

   pfOverride(PFSTATE_TRANSPARENCY, PF_OFF );
   pfTransparency(PFTR_ON);
   pfOverride(PFSTATE_TRANSPARENCY, PF_ON );

#ifdef IRISGL
   cpack (0x5F800000);
   rectf ( left, bottom, right, top );
#else // OPENGL
   glColor4f (0.0f, 0.0f, 0.5f, 0.37f);
   glRectf ( left, bottom, right, top );
#endif

   line = 1.0f;
   column = 3.0f;
#ifdef IRISGL
   RGBcolor (238, 201, 0);
   fmsetfont(our_font);
#else // OPENGL
   glColor3f(0.92f, 0.79f, 0.0f);
   npsFont(DEFFONT);
#endif


   sprintf ( text, "Vehicle index: %d", info_pdata->veh_index );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   line += 1.0f;
   sprintf ( text, "Type index: %d", info_pdata->veh_type_index );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   line += 1.0f;
   switch ( info_pdata->veh_class_type )
      {
      case FLYING_CARPET:
         sprintf ( text, "C++ class: Stealth" );
         break;
      case GND_VEH:
         sprintf ( text, "C++ class: Ground" );
         break;
      case AIR_VEH:
         sprintf ( text, "C++ class: Air" );
         break;
      case FIXED_WING:
         sprintf ( text, "C++ class: Fixed Wing" );
         break;
      case ROTARY_WING:
         sprintf ( text, "C++ class: Rotary Wing" );
         break;
      case SHIP:
         sprintf ( text, "C++ class: Ship" );
         break;
      case SUBMERSIBLE:
         sprintf ( text, "C++ class: Submersible" );
         break;
      case PERSON:
         sprintf ( text, "C++ class: Person" );
         break;
      case UNMANNED:
         sprintf ( text, "C++ class: UAV" );
         break;
      case DI_GUY:
         sprintf ( text, "C++ class: DI Guy(IC)" );
         break;
      case JACK_DI:
         sprintf ( text, "C++ class: JACK DI(IC)" );
         break;
      case VEH_TYPE: 
      default:
         sprintf ( text, "C++ class: Default" );
         break;
      }
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif


   line += 1.0f;
   sprintf ( text, "DIS Type: %d.%d.%d.%d.%d.%d",
                   info_pdata->matched_entity_type.entity_kind,
                   info_pdata->matched_entity_type.domain,
                   info_pdata->matched_entity_type.country,
                   info_pdata->matched_entity_type.category,
                   info_pdata->matched_entity_type.subcategory,
                   info_pdata->matched_entity_type.specific );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   line += 1.0f;
   sprintf ( text, "Model: %s", info_pdata->matched_model_name );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   line += 1.0f;
   sprintf ( text, "LOCAL REPRESENTATION:" );
#ifdef IRISGL
   cmov2 ( column-2.0f, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column-2.0f,line,0.0f);
#endif
  
   line += 2.0f;
   sprintf ( text, "DIS ID: %d/%d/%d",
                   info_pdata->veh_entity_id.address.site,
                   info_pdata->veh_entity_id.address.host,
                   info_pdata->veh_entity_id.entity );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text); 
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   line += 1.0f;
   sprintf ( text, "DIS Type: %d.%d.%d.%d.%d.%d",
                   info_pdata->veh_entity_type.entity_kind,
                   info_pdata->veh_entity_type.domain,
                   info_pdata->veh_entity_type.country,
                   info_pdata->veh_entity_type.category,
                   info_pdata->veh_entity_type.subcategory,
                   info_pdata->veh_entity_type.specific );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   if ( info_pdata->veh_marking.character_set == CharSet_ASCII )
      {
      line += 1.0f;
      sprintf ( text, "Markings: %s",
                info_pdata->veh_marking.markings );
#ifdef IRISGL
      cmov2 ( column, line );
      fmprstr(text);
#else //OPENGL
      npsDrawStringPos(text,column,line,0.0f);
#endif
      }
   
   line += 1.0f;
   sprintf ( text, "Appearance: %08x", info_pdata->veh_appearance );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   line += 1.0f;
   switch ( info_pdata->veh_force )
      {
      case ForceID_Blue:
         sprintf ( text, "Force: Blue (Friend)" );
         break;
      case ForceID_Red:
         sprintf ( text, "Force: Red (Foe)" );
         break;
      case ForceID_Other:
         sprintf ( text, "Force: White (Neutral)" );
         break;
      default:
         sprintf ( text, "Force: Other" );
         break;
      }
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   line += 1.0f;
   sprintf ( text, "Speed: %6.2f", info_pdata->veh_speed );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   real_heading = 360.f - info_pdata->veh_posture.hpr[HEADING];
   while ( real_heading > 360.0f )
      real_heading -= 360.0f;
   while ( real_heading < 0.0f )
      real_heading += 360.0f;
   line += 1.0f;
   sprintf ( text, "H %3.0f, P %3.0f, R %3.0f",
                    real_heading,
                    info_pdata->veh_posture.hpr[PITCH],
                    info_pdata->veh_posture.hpr[ROLL] );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   line += 1.0f;
   sprintf ( text, "X %6.0f, Y %6.0f, Z %6.0f",
                   info_pdata->veh_posture.xyz[X],
                   info_pdata->veh_posture.xyz[Y],
                   info_pdata->veh_posture.xyz[Z] );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   line += 1.0f;
   if ( !strcmp(info_pdata->veh_controller.host_name,"Unknown") ||
        (info_pdata->veh_controller.host_name == NULL) ||
        ((strlen(info_pdata->veh_controller.host_name)) == 0) )
      sprintf ( text, "From: (%s)", 
                info_pdata->veh_controller.internet_address );
   else
      sprintf ( text, "From: %s",
                info_pdata->veh_controller.host_name );
#ifdef IRISGL
   cmov2 ( column, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column,line,0.0f);
#endif

   line += 1.0f;
   sprintf ( text, "ENTITY INFORMATION:" );
#ifdef IRISGL
   cmov2 ( column-2.0f, line );
   fmprstr(text);
#else //OPENGL
   npsDrawStringPos(text,column-2.0f,line,0.0f);
#endif

#ifdef OPENGL
   // This is here because OpenGL handles the MODELVIEW & PROJECTION
   // matrixes differently then does IRISGL
   glMatrixMode(gl_matrix_mode);
#endif

   pfOverride(PFSTATE_TRANSPARENCY, PF_OFF );
   pfTransparency(orignal_trans_mode);
   pfOverride(PFSTATE_TRANSPARENCY, PF_ON );
   
   performer_mode();
}


void
init_pass_data ( PASS_DATA *pdata )
{
   pfSetVec3 ( pdata->posture.xyz, 0.0f, 0.0f, 0.0f );
   pfSetVec3 ( pdata->posture.hpr, 0.0f, 0.0f, 0.0f );
   pfSetVec3 ( pdata->look.xyz, 0.0f, 0.0f, 0.0f );
   pfSetVec3 ( pdata->look.hpr, 0.0f, 0.0f, 0.0f );
   pdata->type = NOSHOW;
   pdata->speed = 0.0f;
   pdata->altitude = 0.0f;
   pdata->eyeview = FORWARD_VIEW;
   for ( int i = 0; i < HUD_MAX_VEH; i++ ) {
      pfSetVec2 ( pdata->hud_data[i].location, 0.0f, 0.0f );
      pdata->hud_data[i].elevation = 0.0f;
      pdata->hud_data[i].force = ForceID_Other;
      pdata->hud_data[i].type = NOSHOW;
      pdata->hud_data[i].status = 0x0;
      pdata->hud_data[i].vid = -1;
   }
   pdata->rng_fltr = 0.0f;
   pdata->input_control = NPS_NO_INPUT;
   pdata->input_number = -1;
   pdata->texture = FALSE;
   pdata->wireframe = FALSE;
   pdata->antialias = FALSE;
   pdata->printscreen = FALSE;
}


void
init_display_options ()
{
  G_HUD_options->aspect = G_dynamic_data->aspect;
  G_HUD_options->range = MIN_RADAR_RANGE;
  G_HUD_options->veh_info = 
       ((G_dynamic_data->hmd == 0) &&
        (G_dynamic_data->window_size != WIN_THREE_BUNKER) &&
        (G_dynamic_data->window_size != WIN_TWO_TPORT) &&
        (G_dynamic_data->window_size != WIN_TWO_ADJACENT) &&
        (G_dynamic_data->window_size != WIN_THREE_TV) );
  G_HUD_options->misc_info = G_HUD_options->veh_info;
  G_HUD_options->eyesep = 2.5f;
  G_HUD_options->num_samples = 0;
  G_HUD_options->full_screen = TRUE;
  G_HUD_options->color = COLOR1;
  G_HUD_options->metric = FALSE;
  G_HUD_options->talpha = 255;
  G_HUD_options->rotmode = TRUE;
  G_HUD_options->colormode = FALSE;
  G_HUD_options->iconmode = TRUE;
  G_HUD_options->stats = FALSE;

  if ( G_dynamic_data->hmd )
     G_HUD_options->hud_level = ENABLE_CROSS;
  else if ((G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
           (G_dynamic_data->window_size == WIN_TWO_ADJACENT) ||
           (G_dynamic_data->window_size == WIN_TWO_TPORT)) 
     G_HUD_options->hud_level = 0;
  else
     G_HUD_options->hud_level = ENABLE_RADAR;

}


void
update_display_options ( pfChannel *chan, PASS_DATA *pdata )
{

   static int stat_cycle = -1;
   int wireframe = FALSE;
   int texture = FALSE;
   int antialias = FALSE;
   int printscreen = FALSE;

   if ( stat_cycle == -1 ) {
// FIXME
//      pfStatsClass ( pfGetChanFStats(chan),
//                     PFFSTATS_ENPFTIMES | PFFSTATS_PFTIMES,
//                     PFSTATS_ON );
      stat_cycle = 1;
   }

   process_display_input(chan,stat_cycle,
                         wireframe,texture,antialias,printscreen);
   pdata->texture = texture;
   pdata->wireframe = wireframe;
   pdata->antialias = antialias;
   pdata->printscreen = printscreen;


}

void draw_aim_lines ( pfChannel *chan )
{
   pfVec2 v; //vertex for drawing lines
   int screen_x, screen_y;
   int targetting = G_dynamic_data->targetting;
   int target_lock = G_dynamic_data->target_lock;


   pfGetChanSize ( chan, &screen_x, &screen_y );

#ifdef IRISGL
   ortho2(0.0f, (float)screen_x, 0.0f, (float)screen_y);
#else // OPENGL
   int gl_matrix_mode;
   glGetIntegerv (GL_MATRIX_MODE, &gl_matrix_mode);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0.0f, (float)screen_x, 0.0f, (float)screen_y);
   glMatrixMode(gl_matrix_mode);
#endif

  float halfX = screen_x / 2.0f;
  float halfY = screen_y / 2.0f;

#ifdef IRISGL
  cpack(0x00ffffff);
  linewidth(2);
  move2i((long)halfX, (long)halfY + int(0.1953125*screen_y));
  draw2i((long)halfX, (long)halfY + int(0.048828125*screen_y));
  move2i((long)halfX, (long)halfY - int(0.1953125*screen_y));
  draw2i((long)halfX, (long)halfY - int(0.048828125*screen_y));

  move2i((long)halfX + int(0.1953125*screen_x), (long)halfY);
  draw2i((long)halfX + int(0.048828125*screen_x), (long)halfY);
  move2i((long)halfX - int(0.1953125*screen_x), (long)halfY);
  draw2i((long)halfX - int(0.048828125*screen_x), (long)halfY);

  if (targetting) {
     switch (target_lock) {
        case BLUE_LOCK:
           cpack(0x00ff0000);
           break;
        case RED_LOCK:
           cpack(0x000000ff);
           break;
        case WHITE_LOCK:
           cpack(0x00ffffff);
           break;
        case OTHER_LOCK:
           cpack(0x0000ff00);
           break;
        default:
           cpack(0x00ffffff); 
           break;
     }  
  }
  else {
     cpack(0x00ffffff);
  }
  circ(halfX,halfY,screen_y*0.015);

#else //OPENGL
  glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
  glLineWidth(2);
  glBegin(GL_LINES);
     PFSET_VEC2(v, halfX, halfY + (0.1953125*screen_y)); glVertex2fv(v);
     PFSET_VEC2(v, halfX, halfY + (0.048828125*screen_y)); glVertex2fv(v);
     PFSET_VEC2(v, halfX, halfY - (0.1953125*screen_y)); glVertex2fv(v);
     PFSET_VEC2(v, halfX, halfY - (0.048828125*screen_y)); glVertex2fv(v);
     PFSET_VEC2(v, halfX + (0.1953125*screen_x), halfY); glVertex2fv(v);
     PFSET_VEC2(v, halfX + (0.048828125*screen_x), halfY); glVertex2fv(v);
     PFSET_VEC2(v, halfX - (0.1953125*screen_x), halfY); glVertex2fv(v);
     PFSET_VEC2(v, halfX - (0.048828125*screen_x), halfY); glVertex2fv(v);
  glEnd();

  GLUquadricObj *qobj = gluNewQuadric();
  pfPushMatrix();
  glTranslatef ( halfX, halfY, 0.0f );
  if (targetting) {
     switch (target_lock) {
        case BLUE_LOCK:
           glColor4f(0.0f, 0.0f, 1.0f, 0.0f);
           break;
        case RED_LOCK:
           glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
           break;
        case WHITE_LOCK:
           glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
           break;
        case OTHER_LOCK:
           glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
           break;
        default:
           glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
           break;
     }
  }
  else {
     glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
  }
  gluDisk(qobj,screen_y*0.01f,screen_y*0.015f,10,1);
  pfPopMatrix();
  gluDeleteQuadric(qobj);
  
#endif
}
