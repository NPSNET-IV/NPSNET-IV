// File: <window.cc>

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

#include <iostream.h>
#include <string.h>
#include <device.h>

#include "window.h"

#include "common_defs2.h"
#include "manager.h"
#include "cpu.h"
#include "fontdef.h"
#include "intro.h"

// Globals used by this library
extern volatile DYNAMIC_DATA  *G_dynamic_data;
extern volatile STATIC_DATA   *G_static_data;
extern volatile  MANAGER_REC  *G_manager;

static int FBAttrs[] = {
   PFFB_RGBA,
   PFFB_DOUBLEBUFFER,
   PFFB_STEREO,
   PFFB_DEPTH_SIZE, 23,
   PFFB_RED_SIZE, 1,
   PFFB_STENCIL_SIZE, 1,
   None,
};

void open_pipe_line ( pfPipeWindow *pw )
   {

   float left, right, bottom, top, xsize, ysize;
   pfLightModel    *lm;
   pfMaterial      *mtl;
   input_manager *l_input_manager = G_manager->ofinput;
   int stereo;

#ifdef DATALOCK
   pfAttachDPool ( DYNAMIC_POOL_FILE );
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   left   = G_dynamic_data->pipe[0].window.left;
   right  = G_dynamic_data->pipe[0].window.right;
   bottom = G_dynamic_data->pipe[0].window.bottom;
   top    = G_dynamic_data->pipe[0].window.top;
   xsize = right - left;
   ysize = top - bottom;
   G_dynamic_data->originX = long(left);
   G_dynamic_data->originY = long(bottom);
   G_dynamic_data->view.left = left + 
      (xsize*G_dynamic_data->pipe[0].channel[0].viewport.left);
   G_dynamic_data->view.right = left +
      (xsize*G_dynamic_data->pipe[0].channel[0].viewport.right);
   G_dynamic_data->view.bottom = bottom +
      (ysize*G_dynamic_data->pipe[0].channel[0].viewport.bottom);
   G_dynamic_data->view.top = bottom +
      (ysize*G_dynamic_data->pipe[0].channel[0].viewport.top);
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

   pfPWinType ( pw, PFPWIN_TYPE_X);

   switch ( G_dynamic_data->window_size ) {
      case WIN_FULL:
      case WIN_VIM:
      case WIN_THREE_TV:
      case WIN_THREE_BUNKER:
      case WIN_TWO_TPORT:
      case WIN_TWO_ADJACENT:
      case WIN_FS_MCO:
         pfPWinMode (pw, PFWIN_NOBORDER, TRUE);
         break;
      case WIN_FS_ST_BUFFERS:
         pfPWinMode (pw, PFWIN_NOBORDER, TRUE);
         pfPWinFBConfigAttrs( pw, FBAttrs );
         break;
      default:
         break;
   }

   pfNotify ( PFNFY_DEBUG, PFNFY_PRINT, "Creating window..." );
   pfNotify ( PFNFY_DEBUG, PFNFY_PRINT, "\tLeft = %f", left );
   pfNotify ( PFNFY_DEBUG, PFNFY_PRINT, "\tRight = %f", right );
   pfNotify ( PFNFY_DEBUG, PFNFY_PRINT, "\tBottom = %f", bottom );
   pfNotify ( PFNFY_DEBUG, PFNFY_PRINT, "\tTop = %f", top );

   pfPWinName ( pw, WINDOW_NAME );

   if (G_dynamic_data->window_size == WIN_FULL) {
      pfPWinFullScreen (pw);
   } 
   else {
      pfPWinOriginSize (G_dynamic_data->pw, 0, 0,
                        G_dynamic_data->pipe[0].window.right,
                        G_dynamic_data->pipe[0].window.top);
   }
  
   pfPWinMode (pw, PFWIN_EXIT, TRUE);

   load_big_seal_tex(); //Load the big seal texture for opening screen

   pfOpenPWin (pw);  //Open the window

   if ( G_dynamic_data->window_size == WIN_FS_ST_BUFFERS ) {
      (void)pfQueryPWin(pw, PFQWIN_STEREO, &stereo);
      if (stereo == PFQFTR_FALSE) {
         cerr << "BOOM STEREO BUFFER REQUEST failed - stereo not available"
              << endl;
      }
      else {
         cerr << "\tRunning in stereo mode for Boom." << endl;
      }
   }

   pfMaterial *default_mtl = pfNewMtl (NULL);
   pfMtlColorMode(default_mtl, PFMTL_FRONT, PFMTL_CMODE_AD);
   pfApplyMtl(default_mtl);

#ifdef OPENGL
   // Set the background color and clear the z-buffer
   glClearColor(0.0, 0.0, 0.5, 0.0);
   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
   pfSwapPWinBuffers(G_dynamic_data->pw);

   // Build All needed fonts
   npsBuildFonts(G_dynamic_data->view.top,G_dynamic_data->view.bottom);
#endif

   print_next_event();
   sleep(5); //Suspend draw process so users can read display screen
/*
   char tmp_char[20];
cerr<<"Logo Screen Done...Press <Enter> To Continue"<<endl;
   cin.getline(tmp_char,20);
cerr<<endl;
*/
   G_static_data->stereo_exists = ( G_static_data->stereo_exists &&
                                    G_static_data->reality_graphics);
   pfCullFace(PFCF_OFF);

   if ( G_static_data->stereo_exists ) 
      {   
/*
      stereobuffer();
      gconfig();
*/
      }

#ifdef IRISGL
   G_static_data->max_samples = int(getgconfig(GC_MS_SAMPLES));
#else // OPENGL
   G_static_data->max_samples = 0;
#endif

   /*
    * Set up default material in case the database does not
    * have any. Turn off lmcolor mode so that database does
    * not pick up random colors.
   */
   mtl = pfNewMtl(NULL);
   pfMtlColorMode(mtl, PFMTL_FRONT, PFMTL_CMODE_COLOR);
   pfApplyMtl(mtl);
 
   /* Set up default lighting model. */
   lm = pfNewLModel(NULL);
   pfLModelAmbient(lm, 0.0f, 0.0f, 0.0f);
   pfApplyLModel(lm);


   fontdef(1L,(char *)G_static_data->hud_font_file);

   if ( G_static_data->cpu )
      {
      if ( !npsLockCPU ( 3 , "Draw" ) )
         G_static_data->cpu = -1;
      }

   while ((G_dynamic_data->init_event < DRAW_FULLY_DEFINED) &&
          (G_static_data->process_mode & PFMP_FORK_DRAW));

   }


