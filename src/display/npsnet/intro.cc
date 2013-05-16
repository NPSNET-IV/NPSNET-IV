// File: <intro.cc>

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


#define OTHER_CREDIT

#include <iostream.h>
#ifdef IRISGL
#include <fmclient.h>
#endif
#include <string.h>
#include <pf.h>
#include "texXLoad.h"
// #include <pfutil.h>

#include "common_defs2.h"

#include "intro.h"

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;


static pfTexture *seal_texture;
static int texture_good = FALSE;
static void display_seals ();

/*
 * this is used to download all textures for a given simulation
 * so that spurious loading of a texture does not happen during
 * real-time operation.  Assumes all textures for a simulation
 * fit in texture memory.  Also flashes textures on screen just 
 * so you can see whats going on 
 */
void DownloadTex(long n, pfTexture **texList, long drawFlag)
{
/*
pfList *theList = pfuMakeSceneTexList 
                     (G_dynamic_data->pipe[0].channel[0].scene);

pfuDownloadTexList (theList, PFUTEX_SHOW);
*/

pfList *theList = pfNewList(sizeof(pfTexture*), n, pfGetSharedArena());

for (int ix = 0; ix < n; ix++) {
   pfAdd (theList, texList[ix]);
}

pfxDownloadTexList (theList, PFXTEX_SHOW);

#if 0
    int   foo, ncomp;
    long xs, ys, i;
    float  xc, yc;
#ifdef IRISGL
    static pfMatrix ident = {{1., 0., 0., 0.}, 
			     {0., 1., 0., 0.}, 
			     {0., 0., 1., 0.}, 
			     {0., 0., 0., 1.}}; 
#endif //IRISGL
    static int which_side = TRUE;

    int texMemUsage = 0;

    for (i = 0; i < n; i++) {
         if ( texList[i] != NULL ) {
            texMemUsage += pfuGetTexSize (texList[i]);
         }
     }

    int textureSize = -1;

    pfQuerySys(PFQSYS_TEXTURE_MEMORY_BYTES, &textureSize);

    if (textureSize > 0) {
       cerr  << "     Texture Memory Usage: "
             << texMemUsage*100.0f/textureSize << "% of "
             << textureSize/(1024*1024) << "MB" << endl;
    }

    if(!drawFlag)
    {
	for (i = 0; i < n; i++) {
            if ( texList[i] != NULL ) {
	       pfApplyTex(texList[i]);
            }
        }
    }
    else
    {
    	pfPushState();
        pfBasicState();
    	pfEnable(PFEN_TEXTURE);

#ifdef IRISGL
	getsize(&xs, &ys);
	ortho2(-.5, xs-.5, -.5, ys-.5);
	viewport(0,(short)xs-1, 0, (short)ys-1);
        zfunction(ZF_ALWAYS);
 	pushmatrix();
	loadmatrix(ident);
#else //OPENGL
        //pfGetWinSize((pfWindow *)G_dynamic_data->pw, (int *)&xs, (int *)&ys);
        xs=G_dynamic_data->pipe[0].window.right;
        ys=G_dynamic_data->pipe[0].window.top;
//cerr<<"xs/ys:"<<xs<<"/"<<ys<<endl;
        pfPushIdentMatrix();

/* 
        //gluOrtho2D( 0.0f, xs, 0.0f, ys );
            // Set the viewport
        //glViewport(0,0,xs,ys);
        //glDepthFunc(GL_ALWAYS);
            // Set the projection matrix.
        //glPushMatrix();
        //glMatrixMode(GL_PROJECTION);
        //glLoadIdentity();
*/
#endif

#ifdef DEBUG
	cerr << "Downloading " << n << " textures." << endl;
#endif

        xc = xs / 2.;
        yc = ys / 2.;

                   print_next_event();
#ifdef IRISGL
                   swapbuffers();
                   RGBcolor(255,255,255); 
#else //OPENGL
                   //pfSwapPWinBuffers(G_dynamic_data->pw);
                   // Set the color we want (white) the for the text.
                   glColor3f(1.0, 1.0, 1.0);
#endif

        if ( G_dynamic_data->window_size == WIN_FS_ST_BUFFERS ) {
#ifdef IRISGL
           leftbuffer(1);
           rightbuffer(1);
#else // OPENGL
            glDrawBuffer(GL_BACK_LEFT);
            glDrawBuffer(GL_BACK_RIGHT);
#endif
        }

	for (i = 0; i < n; i++)
	{
	    int tx, ty;
	    pfVec2	v;
#ifdef IRISGL
	    static pfVec2 t[] = {{0., 0.,}, {1., 0.,}, {1., 1.,}, {0., 1.,}};
#endif //IRISGL

#ifdef DEBUG
    	    cerr << ".\n";
#endif

	    if(texList[i] == NULL)
		cerr << "NULL texture at " << i << " in list\n";
	    else
	    {
                which_side = !which_side;
		pfGetTexImage(texList[i], (unsigned int**)&foo, &ncomp, &tx, &ty, &foo);
		pfApplyTex(texList[i]);

                if (!which_side)
                   {
                   }
#ifdef IRISGL
                ortho2(-.5, xs-.5, -.5, ys-.5);
                viewport(0,(short)xs-1, 0, (short)ys-1);
	    
		bgnpolygon();
		t2f(t[0]);
#else //OPENGL
                glMatrixMode(GL_PROJECTION);
                glDisable(GL_DEPTH_TEST);
                glDisable(GL_SCISSOR_TEST);
                glLoadIdentity();
                gluOrtho2D( 0.0f, xs, 0.0f, ys );
                // Set the viewport
                glViewport(0,0,xs,ys);
                glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
                glBegin(GL_POLYGON);
                glTexCoord2f(0.0, 0.0); 
#endif
                if ( which_side )
                   PFSET_VEC2(v, (float(xs - tx)), (float(ys - ty)) );
                else 
		   PFSET_VEC2(v, 0.0f, (float(ys - ty)) ); 
#ifdef IRISGL
		v2f(v);
		t2f(t[1]);
#else //OPENGL
                glVertex2fv(v);
                glTexCoord2f(1.0, 0.0); 
#endif
                if ( which_side )
                   PFSET_VEC2(v, float(xs), (float(ys - ty)) );
                else
		   PFSET_VEC2(v, float(tx), (float(ys - ty)) );
#ifdef IRISGL
		v2f(v);
		t2f(t[2]);
#else //OPENGL
                glVertex2fv(v);
                glTexCoord2f(1.0, 1.0); 
#endif
                if ( which_side )
                   PFSET_VEC2(v, float(xs), float(ys));
                else
		   PFSET_VEC2(v, float(tx), float(ys) );
#ifdef IRISGL
		v2f(v);
		t2f(t[3]);
#else //OPENGL
                glVertex2fv(v);
                glTexCoord2f(0.0, 1.0); 
#endif
                if ( which_side )
                   PFSET_VEC2(v, (float(xs - tx)), float(ys) );
                else
		   PFSET_VEC2(v, 0.0f, float(ys) );
#ifdef IRISGL
		v2f(v);
		endpolygon();
#else //OPENGL
                glVertex2fv(v);
                glEnd();
                glEnable(GL_DEPTH_TEST);
                glEnable(GL_SCISSOR_TEST);
#endif

		if (which_side) {
#ifdef IRISGL
                   swapbuffers();
#else //OPENGL
                   pfSwapPWinBuffers(G_dynamic_data->pw);
#endif
                }
	    }
	}
#ifdef IRISGL
        zfunction(ZF_LEQUAL);
 	popmatrix();
#else //OPENGL
        //glDepthFunc(GL_ALWAYS);
        // Set the projection matrix.
        pfPopMatrix();
#endif
        pfPopState();
    }
#ifdef DEBUG
    cerr << "\ndone.\n";
#endif
#endif
}


#ifdef IRISGL

short center_text ( fmfonthandle &the_font, const char *the_text ) 
{
   float indent;
   float width;
   long xorigin,yorigin;

   getorigin(&xorigin,&yorigin);

   width = float (fmgetstrwidth (the_font, the_text ));
   indent = xorigin + G_dynamic_data->view.left +
      ((G_dynamic_data->view.right-G_dynamic_data->view.left-width)/2.0f);
   return ( short(indent) );
}

#else //OPENGL

short center_text ( pfuXFont *the_font, const char *the_text ) 
{
   float indent;
   float width;
   long xorigin=0; //,yorigin;

   //pfGetPWinOrigin(G_dynamic_data->pw, (int *)&xorigin, (int *)&yorigin);

   width = float (pfuGetXFontWidth(the_font, the_text));
   indent = xorigin + G_dynamic_data->view.left +
      ((G_dynamic_data->view.right-G_dynamic_data->view.left-width)/2.0f);
//cerr<<"\t\tindent:"<<indent<<" width:"<<width<<" xorigin:"<<xorigin<<endl;
   return ( short(indent) );
}

#endif


void load_big_seal_tex()
{
   seal_texture = pfNewTex(pfGetSharedArena());
   if ( seal_texture != NULL ) {
      texture_good = pfLoadTexFile ( seal_texture, "big_seal.rgb" );
   }
   else {
      texture_good = FALSE;
   } 
}


void display_seals ()
{
   float  xc, yc;
#ifdef IRISGL
   static pfMatrix identity = {{1.0f, 0.0f, 0.0f, 0.0f},
                               {0.0f, 1.0f, 0.0f, 0.0f},
                               {0.0f, 0.0f, 1.0f, 0.0f},
                               {0.0f, 0.0f, 0.0f, 1.0f}};
   static pfVec2 t[] = {{0.0f, 0.0f,},
                        {1.0f, 0.0f,},
                        {1.0f, 1.0f,},
                        {0.0f, 1.0f,}};
#endif //IRISGL
//   static int first_time = TRUE;
   static float tx2;
   static float ty2;
   static float vp_b =
          float(G_dynamic_data->pipe[0].channel[0].viewport.bottom);
   static float vp_t =
          float(G_dynamic_data->pipe[0].channel[0].viewport.top);
   static float vp_l =
          float(G_dynamic_data->pipe[0].channel[0].viewport.left);
   static float vp_r =
          float(G_dynamic_data->pipe[0].channel[0].viewport.right);
   long xs;
   long ys;
   pfVec2      v;

   pfPushState();
   pfBasicState();
   pfEnable(PFEN_TEXTURE);


#if 0
   /* This is a workaround for a GL bug */
   static ulong        img[]   = {0xffffffff};
   static float        texps[] = {TX_MAGFILTER, TX_SHARPEN, TX_NULL};
   static float        tevps[] = {TV_NULL};
   if (G_static_data->reality_graphics) {
      texdef2d(999999, 1, 2, 2, img, 0, texps);
      texbind(TX_TEXTURE_0, 999999);

      tevdef(999999, 0, tevps);
      tevbind(0, 999999);

      texbind(TX_TEXTURE_IDLE, 999999);
      }
#endif


#ifdef IRISGL
   getsize(&xs, &ys);
   ortho2(-0.5f, xs-0.5f, -0.5f, ys-0.5f);
   viewport(0,(short)xs-1, 0, (short)ys-1);

   pushmatrix();
   loadmatrix(identity);
#else //OPENGL
   //pfGetPWinSize(G_dynamic_data->pw, (int *)&xs, (int *)&ys);
   pfWindow *w;
   w = pfGetCurWin();
   pfGetWinSize(w, (int *)&xs, (int *)&ys);
   //xs=G_dynamic_data->pipe[0].window.right;
   //ys=G_dynamic_data->pipe[0].window.top;
   //pfPushIdentMatrix();
   pfPushMatrix();
#endif
  
   xc = ((xs*vp_l)+(xs+vp_r)) / 2.0f;
   yc = ((ys*vp_b)+(ys*vp_t)) / 2.0f;

   if ( texture_good ) {
      int tx, ty, foo, ncomp;
      pfGetTexImage(seal_texture, (unsigned int**)&foo, &ncomp, &tx, &ty, &foo);
      tx2 = tx/2.0f;
      ty2 = ty/2.0f;
   }

   if ( texture_good ) {
      pfApplyTex(seal_texture);
#ifdef IRISGL
      cpack(0);
      zclear();
      cpack(0xff800000);
      bgnpolygon();
      t2f(t[0]);
      PFSET_VEC2(v, xc-tx2, yc-ty2);
      v2f(v);
      t2f(t[1]);
      PFSET_VEC2(v, xc+tx2, yc-ty2);
      v2f(v);
      t2f(t[2]);
      PFSET_VEC2(v, xc+tx2, yc+ty2);
      v2f(v);
      t2f(t[3]);
      PFSET_VEC2(v, xc-tx2, yc+ty2);
      v2f(v);
      endpolygon();
#else //OPENGL
    GLint mm;
    glGetIntegerv(GL_MATRIX_MODE, &mm);
    if (mm != GL_PROJECTION)
        glMatrixMode(GL_PROJECTION);
      //glMatrixMode(GL_PROJECTION);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_SCISSOR_TEST);
      glLoadIdentity(); 
      gluOrtho2D( 0.0f, xs, 0.0f, ys );
      glViewport(0,0,xs,ys);
      glClearColor(0.0, 0.0, 0.5, 0.0);
      glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
      glColor4f(0.0f, 0.0f, 0.5f, 1.0f);
      glBegin(GL_POLYGON);
         glTexCoord2f(0.0, 0.0); PFSET_VEC2(v, xc-tx2, yc-ty2); glVertex2fv(v);
         glTexCoord2f(1.0, 0.0); PFSET_VEC2(v, xc+tx2, yc-ty2); glVertex2fv(v);
         glTexCoord2f(1.0, 1.0); PFSET_VEC2(v, xc+tx2, yc+ty2); glVertex2fv(v);
         glTexCoord2f(0.0, 1.0); PFSET_VEC2(v, xc-tx2, yc+ty2); glVertex2fv(v);
      glEnd();
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_SCISSOR_TEST);
    if (mm != GL_PROJECTION)
            glMatrixMode(mm);
#endif
   }

#ifdef IRISGL
   popmatrix();
#else //OPENGL
   pfPopMatrix();
#endif

   pfPopState();

}


int show_picture ( char *picfile, int side, short picsize, 
                   short left, short right, short bottom, short top )
   {
   pfTexture *pictex;
   int   foo, ncomp;
#ifdef IRISGL
   static pfMatrix ident = {{1., 0., 0., 0.},
                            {0., 1., 0., 0.},
                            {0., 0., 1., 0.},
                            {0., 0., 0., 1.}};
   static pfVec2 tex[] = {{0., 0.,}, {1., 0.,}, {1., 1.,}, {0., 1.,}};
#endif //IRISGL
   pfVec2 vrtx;
   int tx,ty;
 
   pfPushState(); 
   pfBasicState();
   pfEnable(PFEN_TEXTURE);
#ifdef IRISGL
   pushmatrix();
   loadmatrix(ident);
   ortho2 ( left-0.5f, right-0.5f, bottom-0.5f, top-0.5f );
   viewport ( left, right, bottom, top );
#else //OPENGL
   // Set the projection matrix.
   glPushMatrix();
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D( left-0.5f, right-0.5f, bottom-0.5f, top-0.5f );
   // Set the viewport
   glViewport( left, right, bottom, top );
#endif

//#if GFX == 1 
    /* This is a workaround for a GL bug */
    if (G_static_data->reality_graphics)
       {
#ifdef IRISGL
       static ulong        img[] = {0xffffffff};
       static float        texps[] = {TX_MAGFILTER, TX_SHARPEN, TX_NULL};
       static float        tevps[] = {TV_NULL};
#else
#endif

#ifdef IRISGL   
       texdef2d(999999, 1, 2, 2, img, 3, texps);
       texbind(TX_TEXTURE_0, 999999);
   
       tevdef(999999, 1, tevps);
       tevbind(TV_ENV0, 999999);
   
       texbind(TX_TEXTURE_IDLE, 999999);
#else
#endif
       }
//#endif
 
   pictex = pfNewTex(NULL);
   if (pfLoadTexFile (pictex, picfile))
      {
      pfGetTexImage(pictex, (unsigned int**)&foo, &ncomp, &tx, &ty, &foo);
      pfApplyTex(pictex);
#ifdef IRISGL
      cpack(0xffffffff);
      bgnpolygon();
      t2f(tex[0]);
#else //OPENGL
#endif
      if (!side)
         PFSET_VEC2(vrtx, float(left), float(top-picsize));
      else
         PFSET_VEC2(vrtx, float(right-picsize), float(top-picsize));
#ifdef IRISGL
      v2f(vrtx);
      t2f(tex[1]);
#else //OPENGL
#endif
      if (!side)
         PFSET_VEC2(vrtx, float(left+picsize), float(top-picsize));
      else
         PFSET_VEC2(vrtx, float(right), float(top-picsize));
#ifdef IRISGL
      v2f(vrtx);
      t2f(tex[2]);
#else //OPENGL
#endif
      if (!side)
         PFSET_VEC2(vrtx, float(left+picsize), float(top));
      else
         PFSET_VEC2(vrtx, float(right), float(top));
#ifdef IRISGL
      v2f(vrtx);
      t2f(tex[3]);
#else //OPENGL
#endif
      if (!side)
         PFSET_VEC2(vrtx, float(left), float(top));
      else
         PFSET_VEC2(vrtx, float(right-picsize), float(top));
#ifdef IRISGL
      v2f(vrtx);
      endpolygon();
      pfIdleTex(pictex);
//      pfApplyTex(NULL);
      popmatrix();
#else
      pfPopMatrix();
#endif
      pfPopState(); 
      return(TRUE);
      }
   else
      {
      pfIdleTex(pictex);
#ifdef IRISGL
      popmatrix();
#else //OPENGL
      pfPopMatrix();
#endif
      pfPopState();
      return(FALSE);
      }
   }


 
int print_next_event () 
{
   int done = FALSE;
   static int first_time = 1;
#ifdef IRISGL
    static pfMatrix ident = {{1., 0., 0., 0.},
                             {0., 1., 0., 0.},
                             {0., 0., 1., 0.},
                             {0., 0., 0., 1.}};
   static fmfonthandle bigfont;
   static fmfonthandle middlefont;
   static fmfonthandle littlefont;
#else //OPENGL
   static pfuXFont fontmid;
   static pfuXFont fontbig;
          pfWindow *w;
          GLint     mm;
#endif 
   static char text[80];
   static short left = short(G_dynamic_data->view.left);
   static short bottom = short(G_dynamic_data->view.bottom);
   static short right = short(G_dynamic_data->view.right);
   static short top = short(G_dynamic_data->view.top);
   static float vp_b = 
          float(G_dynamic_data->pipe[0].channel[0].viewport.bottom);
   static float vp_t = 
          float(G_dynamic_data->pipe[0].channel[0].viewport.top);
#ifdef PICTURES
   static short height = (top - bottom);
   static short picsize = short(0.25417f*height);
#endif
//   static int the_event;
   int  ys;
   long xsize=G_dynamic_data->pipe[0].window.right;
   long ysize=G_dynamic_data->pipe[0].window.top;
   long xorigin=0;
   long yorigin=0;
   float line = 12.0f;
   ys = top - bottom;

#ifdef IRISGL
   pushmatrix();
   loadmatrix(ident);
   zfunction(ZF_ALWAYS);
   getsize(&xsize,&ysize);
   getorigin(&xorigin,&yorigin);
#else //OPENGL
   w = pfGetCurWin();
   pfPushMatrix();
    glGetIntegerv(GL_MATRIX_MODE, &mm);
    if (mm != GL_PROJECTION)
        glMatrixMode(GL_PROJECTION);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_SCISSOR_TEST);
   // glLoadIdentity();
   //glMatrixMode(GL_PROJECTION);
   //glLoadIdentity();
   pfGetWinSize(w, (int *)&xsize, (int *)&ysize);
   pfGetWinOrigin(w, (int *)&xorigin, (int *)&yorigin);
#endif

   pfPushState();
   pfBasicState();
   pfDisable(PFEN_TEXTURE);

   if ( G_dynamic_data->window_size == WIN_FS_ST_BUFFERS ) {
#ifdef IRISGL
      leftbuffer(1);
      rightbuffer(1);
#else // OPENGL
       glDrawBuffer(GL_BACK_LEFT);
       glDrawBuffer(GL_BACK_RIGHT);
#endif
   }

   if (first_time) {
      static char notice[255];
#ifdef IRISGL
      ortho2 ( left-0.5f, right-0.5f, 0.0f, 13.0f );
      RGBcolor(0,0,128);
      clear();
      cmov2 ( left + 20.0f, 11.0f );
      RGBcolor(255,255,255);
      sprintf ( notice, "%s startup.", VERSION );
      charstr(notice);
      cmov2 ( left + 20.0f, 10.0f );
      charstr("Please wait while loading fonts...");
      swapbuffers();

      fminit();
      bigfont = fmscalefont(fmfindfont("Times-Roman"), (ys*3.0f)/48.0f );
      littlefont = fmscalefont(bigfont, 0.25f);
      middlefont = fmscalefont(bigfont, 0.5f);
#else //OPENGL
      pfuMakeRasterXFont(NULL, &fontmid);
      glLoadIdentity();
      gluOrtho2D( left, right, 0.0f, 13.0f );

      // Set the background color and clear the z-buffer
      glClearColor(0.0, 0.0, 0.5, 0.0);
      glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

      // Set the color we want (white) the for the text.
      glColor3f(1.0, 1.0, 1.0);

      pfuCharPos(left + 20.0f, 11.0f, 0.0);
      sprintf ( notice, "%s startup.", VERSION );
      pfuDrawString(notice);
      pfuCharPos(left + 20.0f, 10.0f,  0.0);
      pfuDrawString("Please wait while loading fonts and textures...");

      //pfSwapPWinBuffers(G_dynamic_data->pw);
      pfSwapWinBuffers(w);
      char midfontName[255];
      char bigfontName[255];
      int bigsize = (int)((ys*3.0f)/48.0f);
      if(bigsize>54) bigsize=54;  //This added due to font size4 limit
      int midsize = (int)(bigsize / 2);
// cerr<<"big:"<<bigsize<<endl;
      sprintf(bigfontName,"-*-times-medium-r-normal--%d-*-*-*-p-184-iso8859-1",
                          bigsize); 
      sprintf(midfontName,"-*-utopia-medium-r-normal--%d-*-*-*-p-184-iso8859-1",
                          midsize); 
      //Setup OpenGL Fonts
      pfuMakeRasterXFont(bigfontName, &fontbig);
      pfuMakeRasterXFont(midfontName, &fontmid);
      pfSwapWinBuffers(w);
#endif
      first_time = 0;
   } // first_time

   // Print Static Information
#ifdef IRISGL
   RGBcolor(0,0,128);
   clear();
#else // OPENGL
   // Set the background color and clear the z-buffer
   //glClearColor(0.0, 0.0, 0.5, 0.0);
   //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
#endif

#ifndef PICTURES
   display_seals();
#endif

   if ( G_dynamic_data->window_size == WIN_FS_ST_BUFFERS ) {
#ifdef IRISGL
      leftbuffer(1);
      rightbuffer(1);
#else // OPENGL
       glDrawBuffer(GL_BACK_LEFT);
       glDrawBuffer(GL_BACK_RIGHT);
#endif
   }

#ifdef IRISGL
//   ortho2 ( left-0.5f, right-0.5f, 0.0f, 13.0f );
   ortho2 ( float(xorigin), float(xorigin+xsize), 0.0f, 13.0f/(vp_t-vp_b) );
#else //OPENGL
   glLoadIdentity();
   gluOrtho2D( float(xorigin), float(xorigin+xsize), 0.0f, 13.0f/(vp_t-vp_b) );
#endif

   line = 12.0f;
   strcpy ( text, "The NPSNET Research Group Presents" );

#ifdef IRISGL
   RGBcolor(238,201,0);
   fmsetfont(middlefont);
   cmov2 ( center_text(middlefont,text), line );
   fmprstr(text);
#else //OPENGL
   glColor3f(0.94, 0.78, 0.0);
   pfuSetXFont(&fontmid);
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);
#endif

   line--;
   strcpy ( text, VERSION );

#ifdef IRISGL
   RGBcolor(255,255,255);
   fmsetfont(bigfont);
   cmov2 ( center_text(bigfont, text), line );
   fmprstr(text);
#else //OPENGL
   glColor3f(1.0, 1.0, 1.0);
   pfuSetXFont(&fontbig);
   pfuCharPos((float)center_text(&fontbig,text), line,  0.0);
   pfuDrawString(text);
#endif

   line -= 0.5f;
   strcpy ( text, "A DIS Visual Simulation System" );

#ifdef IRISGL
   RGBcolor(238,201,0);
   fmsetfont(middlefont);
   cmov2 ( center_text(middlefont,text), line );
   fmprstr(text);
#else //OPENGL
   glColor3f(0.94, 0.78, 0.0);
   pfuSetXFont(&fontmid);
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);
#endif

   line--;
   strcpy ( text, "Code Contributions:" );

#ifdef IRISGL
   cmov2 ( center_text(middlefont,text), line );
   fmprstr(text);
#else //OPENGL
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);
#endif

   line -= 0.5f;
   strcpy ( text, "CMU, NRaD, Loral, Sarcos Inc., TI, and UPenn HMS Lab" );

#ifdef IRISGL
   cmov2 ( center_text(middlefont,text), line );
   fmprstr(text);
#else //OPENGL
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);
#endif

   line--;
   line-= 0.5f;
   strcpy ( text, "Sponsors:" );

#ifdef IRISGL
   cmov2 ( center_text(middlefont,text), line );
   fmprstr(text);
#else //OPENGL
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);
#endif

   line-= 0.5f;
   strcpy ( text, "ARL, ARPA, DMSO, STRICOM, TEC, TRAC," );

#ifdef IRISGL
   cmov2 ( center_text(middlefont,text), line );
   fmprstr(text);
#else //OPENGL
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);
#endif

   line-= 0.5f;
   strcpy ( text, "and NPS Direct Funding" );

#ifdef IRISGL
   cmov2 ( center_text(middlefont,text), line );
   fmprstr(text);
#else //OPENGL
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);
#endif

   line--;
   line--;
   line--;
   strcpy ( text, "Contact Information:" );

#ifdef IRISGL
   cmov2 ( center_text(middlefont,text), line );
   fmprstr(text);
#else //OPENGL
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);
#endif

   line-= 0.5f;
   strcpy ( text, "npsnet@cs.nps.navy.mil" );

#ifdef IRISGL
   cmov2 ( center_text(middlefont,text), line );
   fmprstr(text);
#else //OPENGL
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);
#endif

   line-= 0.5f;
   strcpy(text, "http://www-npsnet.cs.nps.navy.mil/npsnet/"); 

#ifdef IRISGL
   cmov2 ( center_text(middlefont,text), line );
   fmprstr(text);
#else //OPENGL
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);
#endif

   line--;
   line--;

/* Commented Out 4/10/96 for new opening screen
   // Print Dynamic Information 
#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   the_event = G_dynamic_data->init_event;
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

   switch (the_event)
      {
      case INIT_PERFORMER:
         if ( G_static_data->pictures )
            {
#ifdef PICTURES
            show_picture ( "paulb.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Paul T. Barham");
            show_picture ( "davey.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Dave Young" );
#endif
            }
         strcpy ( text, "Initializing Performer..." );
         break;
      case LOADING_SOUNDS:
         if ( G_static_data->pictures )
            {
#ifdef PICTURES
            show_picture ( "paulb.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Paul T. Barham");
            show_picture ( "davey.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Dave Young" );
#endif
            }
         strcpy ( text, "Loading Sounds..." );
         break;
      case LOADING_TERRAIN:
         if ( G_static_data->pictures )
            {
#ifdef PICTURES
            show_picture ( "paulb.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Paul T. Barham");
            show_picture ( "davey.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Dave Young" );
#endif
            }
         strcpy ( text, "Loading terrain..." );
         break;
      case LOADING_DMODELS:
         if ( G_static_data->pictures )
            {
#ifdef PICTURES
            show_picture ( "davep.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "David Pratt" );
            show_picture ( "randallb.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Randall Barker" );
#endif
            }
         strcpy ( text, "Loading dynamic models..." );
         break;
      case LOADING_SMODELS:
         if ( G_static_data->pictures )
            {
#ifdef PICTURES
            show_picture ( "shirleyp.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Shirley Pratt" );
            show_picture ( "mikem.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Mike Macedonia" );
#endif
            }
         strcpy ( text, "Loading static models..." );
         break;
      case LOADING_ANIMATIONS:
         if ( G_static_data->pictures )
            {
#ifdef PICTURES
            show_picture ( "danc.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Daniel Corbin" );
            show_picture ( "mikez.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Michael Zyda" );
#endif
            }
         strcpy ( text, "Loading animations ..." );
         break;
      case OPENING_NETWORK:
         if ( G_static_data->pictures )
            {
#ifdef PICTURES
            show_picture ( "stevez.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Steven Zeswitz" );
            show_picture ( "johnf.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "John Falby" );
#endif
            }
         strcpy ( text, "Opening network..." );
         break;
      case OPENING_IPORT:
         if ( G_static_data->pictures )
            {
#ifdef PICTURES
            show_picture ("crest.rgb", 0, picsize,
                          left, right, bottom, top);
            show_picture ("seal.rgb", 1, picsize,
                          left, right, bottom, top);
#endif
            }
         strcpy ( text, "Opening ISMS/IPORT ..." );
         break;
      case STARTING_DRAW:
         if ( G_static_data->pictures && !G_static_data->reality_graphics )
            {
#ifdef PICTURES
            show_picture ("crest.rgb", 0, picsize,
                          left, right, bottom, top);
            show_picture ("seal.rgb", 1, picsize,
                          left, right, bottom, top);
#endif
            }
         strcpy ( text, "Many thanks to our sponsors!" );
         done = 1;
         break;
      case LOADING_TEXTURES:
         strcpy ( text, "Pre-loading textures for Reality Engine..." );
         break;
      default:
         strcpy ( text, "Doing something..." );
      }
*/

// Temporary until finalized 4/10/96
   strcpy ( text, "Many thanks to our sponsors!" );

#ifdef IRISGL

   RGBcolor(0,180,0);
   cmov2 ( center_text (middlefont,text), line );
   fmprstr (text);
   swapbuffers();

   zfunction(ZF_LEQUAL);   
#else //OPENGL
   // Set the color we want (green) the for the text.
   glColor3f(0.0, 0.7, 0.0);
   pfuCharPos((float)center_text(&fontmid,text), line,  0.0);
   pfuDrawString(text);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    if (mm != GL_PROJECTION)
            glMatrixMode(mm);

   //pfSwapPWinBuffers(G_dynamic_data->pw);
   pfSwapWinBuffers(w);

#endif

   pfPopState ();

#ifdef IRISGL
   popmatrix();
#else //OPENGL
   pfPopMatrix();
#endif

   return done;

}

