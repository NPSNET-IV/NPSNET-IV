// File: <intro2.cc>

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
#include <fmclient.h>
#include <gl.h>
#include <string.h>

#include "intro.h"

#include "common_defs2.h"

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;


/*
 * this is used to download all textures for a given simulation
 * so that spurious loading of a texture does not happen during
 * real-time operation.  Assumes all textures for a simulation
 * fit in texture memory.  Also flashes textures on screen just 
 * so you can see whats going on 
 */
void DownloadTex(long n, pfTexture **texList, long drawFlag)
{
    long   foo, ncomp, xs, ys, i;
    float  xc, yc;
    static pfMatrix ident = {{1., 0., 0., 0.}, 
			     {0., 1., 0., 0.}, 
			     {0., 0., 1., 0.}, 
			     {0., 0., 0., 1.}}; 
    static int which_side = TRUE;


//#if GFX == 1 
    /* This is a workaround for a GL bug */
    static ulong        img[] = {0xffffffff};
    static float        texps[] = {TX_MAGFILTER, TX_SHARPEN, TX_NULL};
    static float        tevps[] = {TV_NULL};

    if (G_static_data->reality_graphics)
    {
	texdef2d(999999, 1, 2, 2, img, 0, texps);
	texbind(TX_TEXTURE_0, 999999);

	tevdef(999999, 0, tevps);
	tevbind(0, 999999);

	texbind(TX_TEXTURE_IDLE, 999999);
    }
//#endif

    if(!drawFlag)
    {
	for (i = 0; i < n; i++)
	    pfApplyTex(texList[i]);
    }
    else
    {
	getsize(&xs, &ys);
	ortho2(-.5, xs-.5, -.5, ys-.5);
	viewport(0,(short)xs-1, 0, (short)ys-1);
	xc = xs / 2.;
	yc = ys / 2.;

    	pfPushState();
        pfBasicState();
    	pfEnable(PFEN_TEXTURE);
        zfunction(ZF_ALWAYS);

 	pushmatrix();
	loadmatrix(ident);

#ifdef DEBUG
	cerr << "Downloading " << n << " textures\n";
#endif

	for (i = 0; i < n; i++)
	{
	    long	tx, ty;
	    pfVec2	v;
	    static pfVec2 t[] = {{0., 0.,}, {1., 0.,}, {1., 1.,}, {0., 1.,}};

#ifdef DEBUG
    	    cerr << ".\n";
#endif

	    if(texList[i] == NULL)
		cerr << "NULL texture at " << i << " in list\n";
	    else
	    {
                which_side = !which_side;
		pfGetTexImage(texList[i], (ulong**)&foo, &ncomp, &tx, &ty, &foo);
		pfApplyTex(texList[i]);

                if (!which_side)
                   {
                   print_next_event();
                   swapbuffers();
                   RGBcolor(255,255,255); 
                   }
	    
		bgnpolygon();
		t2f(t[0]);
                if ( which_side )
                   PFSET_VEC2(v, (float(xs - tx)), (float(ys - ty)) );
                else 
		   PFSET_VEC2(v, 0.0f, (float(ys - ty)) ); 
		v3f(v);
		t2f(t[1]);
                if ( which_side )
                   PFSET_VEC2(v, float(xs), (float(ys - ty)) );
                else
		   PFSET_VEC2(v, float(tx), (float(ys - ty)) );
		v3f(v);
		t2f(t[2]);
                if ( which_side )
                   PFSET_VEC2(v, float(xs), float(ys));
                else
		   PFSET_VEC2(v, float(tx), float(ys) );
		v3f(v);
		t2f(t[3]);
                if ( which_side )
                   PFSET_VEC2(v, (float(xs - tx)), float(ys) );
                else
		   PFSET_VEC2(v, 0.0f, float(ys) );
		v3f(v);
		endpolygon();

		if (which_side)
                   swapbuffers();
	    }
	}
        zfunction(ZF_LEQUAL);
 	popmatrix();
        pfPopState();
    }
#ifdef DEBUG
    cerr << "\ndone.\n";
#endif
}


short center_text ( fmfonthandle &the_font, const char *the_text )
   {
   float indent;
   float width;

   width = float (fmgetstrwidth (the_font, the_text ));
   indent = G_dynamic_data->view.left +
            ((G_dynamic_data->view.right-G_dynamic_data->view.left-width)/2.0f);  
   return ( short(indent) );
   }

int show_picture ( char *picfile, int side, short picsize, 
                   short left, short right, short bottom, short top )
   {
   pfTexture *pictex;
   long   foo, ncomp;
   static pfMatrix ident = {{1., 0., 0., 0.},
                            {0., 1., 0., 0.},
                            {0., 0., 1., 0.},
                            {0., 0., 0., 1.}};
   long tx,ty;
   pfVec2 vrtx;
   static pfVec2 tex[] = {{0., 0.,}, {1., 0.,}, {1., 1.,}, {0., 1.,}};
 
   pfPushState(); 
   pfBasicState();
   pfEnable(PFEN_TEXTURE);
   pushmatrix();
   loadmatrix(ident);
   ortho2 ( left-0.5f, right-0.5f, bottom-0.5f, top-0.5f );
   viewport ( left, right, bottom, top );

//#if GFX == 1 
    /* This is a workaround for a GL bug */
    if (G_static_data->reality_graphics)
       {
       static ulong        img[] = {0xffffffff};
       static float        texps[] = {TX_MAGFILTER, TX_SHARPEN, TX_NULL};
       static float        tevps[] = {TV_NULL};
   
       texdef2d(999999, 1, 2, 2, img, 3, texps);
       texbind(TX_TEXTURE_0, 999999);
   
       tevdef(999999, 1, tevps);
       tevbind(TV_ENV0, 999999);
   
       texbind(TX_TEXTURE_IDLE, 999999);
       }
//#endif
 
   pictex = pfNewTex(NULL);
   if (pfLoadTexFile (pictex, picfile))
      {
      pfGetTexImage(pictex, (ulong**)&foo, &ncomp, &tx, &ty, &foo);
      pfApplyTex(pictex);

      cpack(0xffffffff);
      bgnpolygon();
      t2f(tex[0]);
      if (!side)
         PFSET_VEC2(vrtx, float(left), float(top-picsize));
      else
         PFSET_VEC2(vrtx, float(right-picsize), float(top-picsize));
      v2f(vrtx);
      t2f(tex[1]);
      if (!side)
         PFSET_VEC2(vrtx, float(left+picsize), float(top-picsize));
      else
         PFSET_VEC2(vrtx, float(right), float(top-picsize));
      v2f(vrtx);
      t2f(tex[2]);
      if (!side)
         PFSET_VEC2(vrtx, float(left+picsize), float(top));
      else
         PFSET_VEC2(vrtx, float(right), float(top));
      v2f(vrtx);
      t2f(tex[3]);
      if (!side)
         PFSET_VEC2(vrtx, float(left), float(top));
      else
         PFSET_VEC2(vrtx, float(right-picsize), float(top));
      v2f(vrtx);
      endpolygon();
      pfIdleTex(pictex);
//      pfApplyTex(NULL);
      popmatrix();
      pfPopState(); 
      return(TRUE);
      }
   else
      {
      pfIdleTex(pictex);
      popmatrix();
      pfPopState();
      return(FALSE);
      }
   }

 
int print_next_event ()
   {
   int done = FALSE;
    static pfMatrix ident = {{1., 0., 0., 0.},
                             {0., 1., 0., 0.},
                             {0., 0., 1., 0.},
                             {0., 0., 0., 1.}};
   static int first_time = 1;
   static fmfonthandle bigfont;
   static fmfonthandle middlefont;
   static fmfonthandle littlefont;
   static char text[80];
   static short left = short(G_dynamic_data->view.left);
   static short bottom = short(G_dynamic_data->view.bottom);
   static short right = short(G_dynamic_data->view.right);
   static short top = short(G_dynamic_data->view.top);
   static short height = (top - bottom);
   static short picsize = short(0.25417f*height);
   static int the_event;

   pushmatrix();
   loadmatrix(ident);
   pfPushState();
   pfBasicState();

   zfunction(ZF_ALWAYS);
   pfDisable(PFEN_TEXTURE);
   ortho2 ( left, right, bottom, top );

   if (first_time)
      {
      RGBcolor(0,0,128);
      clear();
      cmov2 ( left + 20.0f, top - 20.0f );
      RGBcolor(255,255,255);
      charstr("NPSNET-IV startup.");
      cmov2 ( left + 20.0f, top - 40.0f );
      charstr("Please wait while loading fonts...");
      swapbuffers();

      fminit();
      littlefont = fmscalefont(fmfindfont("Screen-Bold"), 12.0f);
      middlefont = fmscalefont(fmfindfont("Times-Roman"), 24.0f);
      bigfont = fmscalefont(middlefont, 2.0f);
      first_time = 0;
      }

   // Print Static Information

   RGBcolor(0,0,128);
   clear();

   RGBcolor(238,201,0);

   fmsetfont(littlefont);
   strcpy ( text, "The Naval Postgraduate School" );
   cmov2 (center_text(littlefont, text), top - short(0.05f*height));
   fmprstr(text);
   strcpy ( text, "Computer Graphics & Video Laboratory" );
   cmov2 (center_text(littlefont, text), top - short(0.0875f*height));
   fmprstr(text);
#ifndef DEMO
   strcpy ( text, "presents" );
#endif
#ifdef DEMO
   strcpy ( text, "DEMONSTRATES" );
#endif
   cmov2 (center_text(littlefont, text), top - short(0.125f*height));
   fmprstr(text);

   RGBcolor(0,0,0);

   fmsetfont(bigfont);
   strcpy ( text, VERSION );
   cmov2 (center_text(bigfont, text)-2, top - short(0.25417f*height)-2);
   fmprstr(text);

   RGBcolor(255,255,255);

   cmov2 (center_text(bigfont, text), top - short(0.25417f*height));
   fmprstr(text);

     
#ifndef MULTICAST
   if ( G_static_data->stealth || G_static_data->UAV )
#endif
      {
      fmsetfont(middlefont);
      if ( G_static_data->UAV )
         strcpy ( text, "UAV ");
      else
         strcpy ( text, "" );
      if ( G_static_data->stealth )
         strcat ( text, "STEALTH MODE" );
      else
         text[3] = NULL;
#ifdef MULTICAST
      strcat ( text, " MULTICAST" );
#endif
      cmov2 (center_text(middlefont, text), top-short(0.295f*height));
      RGBcolor(255,0,0);
      fmprstr(text);
      RGBcolor(255,255,255);
      }


   fmsetfont(middlefont);
   strcpy ( text, "An object-oriented, visual simulation system" );
   cmov2 (center_text(middlefont, text), top-short(0.33333f*height));
   fmprstr(text);

   RGBcolor(238,201,0);
 
   fmsetfont(littlefont);
   strcpy ( text, "Written in C++, by: Paul T. Barham, Roy D. Young," );
   cmov2 (center_text(littlefont, text), top-short(0.39583f*height));
   fmprstr(text);
   strcpy ( text, "David Pratt & Randall Barker" );
   cmov2 (center_text(littlefont, text), top-short(0.425*height));
   fmprstr(text);
   strcpy ( text, "Significant contributions by:" );
   cmov2 (center_text(littlefont, text), top-short(0.475f*height));
   fmprstr(text);
   strcpy ( text, "Michael Macedonia, Steven Zeswitz and Daniel Corbin" );
//   cmov2 (center_text(littlefont, text), top-short(0.5125f*height));
  cmov2 (center_text(littlefont, text), top-short(0.50f*height));
   fmprstr(text);
   strcpy ( text, "Enhanced by: Klaus Biggers, Bryant Eastham and Tim Moore of Sarcos Inc." );
   cmov2 (center_text(littlefont, text), top-short(0.54f*height));
   fmprstr(text);
   strcpy ( text, "John Granieri and Mike Hollick of Univ. of Pennsylvania" );
   cmov2 (center_text(littlefont, text), top-short(0.568f*height));
   fmprstr(text);

   strcpy ( text, "Utilizing:" );
   cmov2 (center_text(littlefont, text), top-short(0.62917f*height));
   fmprstr(text);
   strcpy ( text, "Silicon Graphics Performer API" );
   cmov2 (center_text(littlefont, text), top-short(0.660f*height));
   fmprstr(text);
   strcpy ( text, "Software Systems MultiGen Flight format database" );
   cmov2 (center_text(littlefont, text), top-short(0.69417*height));
   fmprstr(text);
   strcpy ( text, "Distributed Interactive Simulation (DIS) Protocol" );
   cmov2 (center_text(littlefont, text), top-short(0.72767f*height));
   fmprstr(text);

   strcpy ( text, "Faculty Support: Dr. David Pratt, Dr. Michael Zyda," );
   cmov2 (center_text(littlefont, text), top-short(0.7825f*height));
   fmprstr(text);
   strcpy ( text, "Dr. Robert McGhee & LCDR John Falby" );
   cmov2 (center_text(littlefont, text), top-short(0.82f*height));
   fmprstr(text);
   strcpy ( text, "Thanks to our sponsors:  ARL, STRICOM, TRAC & ARPA" );
   cmov2 (center_text(littlefont, text), top-short(0.89083*height));
   fmprstr(text);

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
//#if GFX == 1 
         if ( G_static_data->pictures )
            {
            show_picture ( "./pictures/paulb.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Paul T. Barham");
            show_picture ( "./pictures/davey.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Dave Young" );
            }
//#endif
         strcpy ( text, "Initializing Performer..." );
         break;
      case LOADING_TERRAIN:
//#if GFX == 1 
         if ( G_static_data->pictures )
            {
            show_picture ( "./pictures/paulb.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Paul T. Barham");
            show_picture ( "./pictures/davey.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Dave Young" );
            }
//#endif
         strcpy ( text, "Loading terrain..." );
         break;
      case LOADING_DMODELS:
//#if GFX == 1 
         if ( G_static_data->pictures )
            {
            show_picture ( "./pictures/davep.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "David Pratt" );
            show_picture ( "./pictures/randallb.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Randall Barker" );
            }
//#endif
         strcpy ( text, "Loading dynamic models..." );
         break;
      case LOADING_SMODELS:
//#if GFX == 1
         if ( G_static_data->pictures )
            {
            show_picture ( "./pictures/mikem.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Mike Macedonia" );
            show_picture ( "./pictures/stevez.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Steven Zeswitz" );
            }
//#endif
         strcpy ( text, "Loading static models..." );
         break;
      case LOADING_ANIMATIONS:
//#if GFX == 1
         if ( G_static_data->pictures )
            {
            show_picture ( "./pictures/danc.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Daniel Corbin" );
            show_picture ( "./pictures/mikez.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "Michael Zyda" );
            }
//#endif
         strcpy ( text, "Loading animations ..." );
         break;
      case OPENING_NETWORK:
//#if GFX == 1 
         if ( G_static_data->pictures )
            {
            show_picture ( "./pictures/paulb.rgb", 0, picsize,
                           left, right, bottom, top );
            cmov2 ( left, top-13 );
            charstr ( "Paul T. Barham" );
            show_picture ( "./pictures/johnf.rgb", 1, picsize,
                           left, right, bottom, top );
            cmov2 ( right-picsize, top-13 );
            charstr ( "John Falby" );
            }
//#endif
         strcpy ( text, "Opening network..." );
         break;
      case OPENING_IPORT:
//#if GFX == 1
         if ( G_static_data->pictures )
            {
            show_picture ("./pictures/crest.rgb", 0, picsize,
                          left, right, bottom, top);
            show_picture ("./pictures/seal.rgb", 1, picsize,
                          left, right, bottom, top);
            }
//#endif
         strcpy ( text, "Opening I-Port ..." );
         break;
      case STARTING_DRAW:
//#if GFX == 1 
         if ( G_static_data->pictures && !G_static_data->reality_graphics )
            {
            show_picture ("./pictures/crest.rgb", 0, picsize,
                          left, right, bottom, top);
            show_picture ("./pictures/seal.rgb", 1, picsize,
                          left, right, bottom, top);
            }
//#endif
         strcpy ( text, "Starting draw process..." );
         done = 1;
         break;
      case LOADING_TEXTURES:
         strcpy ( text, "Pre-loading textures for Reality Engine..." );
         break;
      default:
         strcpy ( text, "Doing something..." );
      }

   RGBcolor(0,180,0);
   fmsetfont(middlefont);
   cmov2 ( center_text (middlefont,text), top - short(0.97083f*height));
   fmprstr (text);
   swapbuffers();

   zfunction(ZF_LEQUAL);   
   pfPopState ();
   popmatrix();
   return done;

   }

