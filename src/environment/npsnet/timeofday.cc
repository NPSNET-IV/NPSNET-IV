// File: <timeofday.cc>

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


#include "timeofday.h"
#include "environment_const.h"

#include "common_defs2.h"

#ifndef NO_DVW
#define DVW_NPSNET
#define _CSSM
#define _DVW_VERBOSE_
#define _DVW_MULTI_TASKS_
#define _CC_COMPILER_
#define PERFORMER1_2
# include "dvw_util.h"
#endif

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern volatile SUN          *G_sun;

#ifndef NO_DVW
extern float          haze_color[3];
extern float          hazeWeight;
extern "C" int        isHazeON(void);
extern "C" int        isCloudON(void);
#endif


void inittod ( void *arena, CHANNEL_DATA *chan )
{

   /* Define the Earth/Sky model from clearing the screen between   */
   /*    refreshes.                                                 */
   chan->earth_sky = pfNewESky();
   chan->fog_ptr = pfNewFog(arena);
#ifndef NOSUB
      chan->waterfog_ptr = pfNewFog(arena);
#endif // NOSUB
   pfESkyMode ( chan->earth_sky, PFES_BUFFER_CLEAR, chan->earth_sky_mode );
   pfESkyAttr ( chan->earth_sky, PFES_GRND_HT,  chan->ground_height );
   pfESkyAttr ( chan->earth_sky, PFES_HORIZ_ANGLE, chan->horiz_angle );
   pfFogType ( chan->fog_ptr, G_dynamic_data->fog_type);
/*
   if ( (G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
        (G_dynamic_data->window_size == WIN_TWO_TPORT) ) {
      pfFogType ( chan->fog_ptr, PFFOG_PIX_LIN );
      G_dynamic_data->tod = 0.5f;
   }
   else {
      if ( G_static_data->reality_graphics ) {
         pfFogType ( chan->fog_ptr, PFFOG_PIX_SPLINE );
#ifndef NOSUB
            pfFogType ( chan->waterfog_ptr, PFFOG_PIX_SPLINE );
#endif // NOSUB         
      }
      else {
         pfFogType ( chan->fog_ptr, PFFOG_PIX_EXP2 );
#ifndef NOSUB
            pfFogType ( chan->waterfog_ptr, PFFOG_PIX_EXP2 );
#endif // NOSUB
      }
   }
*/
   pfFogRange ( chan->fog_ptr, chan->fog_plane.near, chan->fog_plane.far );
#ifndef NOSUB
      pfFogRange ( chan->waterfog_ptr, chan->waterfog_plane.near, chan->waterfog_plane.far );
#endif // NOSUB
   pfChanESky ( chan->chan_ptr, chan->earth_sky );
   if ( G_dynamic_data->fog )
      pfESkyFog ( chan->earth_sky, PFES_GENERAL, chan->fog_ptr );

   pfESkyMode ( chan->earth_sky, PFES_CLOUDS, PFES_OVERCAST );
   chan->cloud_top = 1500.0f;
//************************************************************************
   chan->cloud_thick = 700.0f;
//************************************************************************ 
   if (G_dynamic_data->clouds)
      {
//************************************************************************ 
      chan->cloud_bottom = chan->cloud_top - chan->cloud_thick;
//************************************************************************ 
      pfESkyAttr ( chan->earth_sky, PFES_CLOUD_TOP, chan->cloud_top );
      pfESkyAttr ( chan->earth_sky, PFES_CLOUD_BOT, chan->cloud_bottom );
      pfESkyAttr ( chan->earth_sky, PFES_TZONE_TOP, chan->cloud_top+TZONE);
      pfESkyAttr ( chan->earth_sky, PFES_TZONE_BOT, chan->cloud_bottom-TZONE);
      }
   else
      {
      chan->cloud_bottom = chan->cloud_top;
      pfESkyAttr ( chan->earth_sky, PFES_CLOUD_TOP, chan->cloud_top );
      pfESkyAttr ( chan->earth_sky, PFES_CLOUD_BOT, chan->cloud_bottom );
      pfESkyAttr ( chan->earth_sky, PFES_TZONE_TOP, chan->cloud_top);
      pfESkyAttr ( chan->earth_sky, PFES_TZONE_BOT, chan->cloud_bottom);
      }

   chan->cloud_bot_color[R] = 0.5f;
   chan->cloud_bot_color[G] = 0.5f;
   chan->cloud_bot_color[B] = 0.8f;
   chan->cloud_top_color[R] = 0.8f;
   chan->cloud_top_color[G] = 0.8f;
   chan->cloud_top_color[B] = 1.0f;

#ifndef NO_DVW
   if ( chan->earth_sky_mode == PFES_FAST )
   {
     chan->fog_color[R] = 0.4f;
     chan->fog_color[G] = 0.4f;
     chan->fog_color[B] = 1.0f;
   }
   else //(rsr: added extra braces in following)
#endif
   {
      if ( (G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
           (G_dynamic_data->window_size == WIN_TWO_TPORT) ) {
         chan->fog_color[R] = 0.5;
         chan->fog_color[G] = 0.6;
         chan->fog_color[B] = 0.8f;
      }
      else {
         chan->fog_color[R] = 0.8f;
         chan->fog_color[G] = 0.8f;
         chan->fog_color[B] = 1.0f;
      }
   }
#ifndef NOSUB   
      chan->waterfog_color[R] = 0.0f;  
      chan->waterfog_color[G] = 0.0f;  
      chan->waterfog_color[B] = 1.0f;  
#endif // NOSUB

}


void updatetod( float tod, CHANNEL_DATA *chan )
{
    float r,g,b;
    float s;
    SUN *sun = (SUN *)G_sun;

#ifndef NO_DVW
    if (isCloudON()) {
        pfESkyMode (chan->earth_sky, PFES_BUFFER_CLEAR, PFES_FAST);
    }
    else {
        pfESkyMode (chan->earth_sky, PFES_BUFFER_CLEAR, PFES_SKY_CLEAR);
    }
#endif

    /* make horizon band angle directly proportional to time of day */
    pfESkyAttr(chan->earth_sky, PFES_HORIZ_ANGLE,
        1.0f + (chan->horiz_angle - 1.0f)*tod);

    /* scale it for the following ramp calcs */
    s = tod * 4.0f;

    if ( (G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
         (G_dynamic_data->window_size == WIN_TWO_TPORT) ) {
       r = 0.502f;
       g = 0.6f;
       b = 0.733f;
       pfESkyColor(chan->earth_sky, PFES_SKY_TOP, r,g,b, 1.0f);
       pfESkyColor(chan->earth_sky, PFES_SKY_BOT, r,g,b, 1.0f);
       pfESkyColor(chan->earth_sky, PFES_CLEAR, r,g,b, 1.0f);

       r *= 0.05f;
       b *= 0.05f;
       g *= 0.05f;
       pfESkyColor(chan->earth_sky, PFES_HORIZ, r,g,b, 1.0f);
    }
    else {

       /* top of sky */
       r = (0.4f * s) - 1.6f;   r = PF_MAX2(0.0f, r);  r = PF_MIN2(0.6f, r);
       g = (0.4f * s) - 1.6f;   g = PF_MAX2(0.0f, g);  g = PF_MIN2(0.6f, g);
       b = (0.8f * s) - 1.6f;   b = PF_MAX2(0.0f, b);  b = PF_MIN2(1.0f, b);
       pfESkyColor(chan->earth_sky, PFES_SKY_TOP, r,g,b, 1.0f);
   
       /* bottom of sky */
       r = (0.6f * s) - 1.6f;   r = PF_MAX2(0.0f, r);  r = PF_MIN2(0.6f, r);
       g = (0.6f * s) - 1.6f;   g = PF_MAX2(0.0f, g);  g = PF_MIN2(0.6f, g);
       b = (1.0f * s) - 1.6f;   b = PF_MAX2(0.0f, b);  b = PF_MIN2(1.0f, b);
       pfESkyColor(chan->earth_sky, PFES_SKY_BOT, r,g,b, 1.0f);
   
       /* horizon */
       r = (1.45f * s) - 0.4f;   r = PF_MAX2(0.0f, r);  r = PF_MIN2(0.7f, r);
       g = (0.65f * s) - 1.4f;   g = PF_MAX2(0.0f, g);  g = PF_MIN2(0.7f, g);
       b = (0.65f * s) - 1.6f;   b = PF_MAX2(0.0f, b);  b = PF_MIN2(1.0f, b);
       pfESkyColor(chan->earth_sky, PFES_HORIZ, r,g,b, 1.0f);
    }
 
    /* ground */
    r = (0.0181f * (s - 0.8f));   r = PF_MAX2(0.0f, r);  r = PF_MIN2(0.058f, r);
    g = (0.084f * (s - 0.8f));   g = PF_MAX2(0.0f, g);  g = PF_MIN2(0.27f, g);
    b = (0.0181f * (s - 0.8f));   b = PF_MAX2(0.0f, b);  b = PF_MIN2(0.058f, b);
    pfESkyColor(chan->earth_sky, PFES_GRND_NEAR, r,g,b, 1.0f);
    pfESkyColor(chan->earth_sky, PFES_GRND_FAR, r,g,b, 1.0f);

#ifndef NO_DVW
   if (isCloudON())
   {
      if ( G_dynamic_data->fog)
      {
         r = chan->fog_color[R];
         g = chan->fog_color[G];
         b = chan->fog_color[B];
         pfESkyColor(chan->earth_sky, PFES_CLEAR, tod*r, tod*g, tod*b, 1.0f);
      }
      else
      {
         r = (0.6f * s) - 1.6f;   r = PF_MAX2(0.0f, r);  r = PF_MIN2(0.6f, r);
         g = (0.6f * s) - 1.6f;   g = PF_MAX2(0.0f, g);  g = PF_MIN2(0.6f, g);
         b = (1.0f * s) - 1.6f;   b = PF_MAX2(0.0f, b);  b = PF_MIN2(1.0f, b);
         pfESkyColor(chan->earth_sky, PFES_CLEAR,
                (tod * r), (tod * g), (tod * b), 1.0f);
      }
   }
   else
   {
      pfESkyColor(chan->earth_sky, PFES_CLEAR, r,g,b, 1.0f);
   }

/***************************************/
/* to blend the haze color to FAST SKY */
/***************************************/

    if (isHazeON() && isCloudON()) {
       r = (0.6f * s) - 1.6f;   r = PF_MAX2(0.0f, r);  r = PF_MIN2(0.6f, r);
       g = (0.6f * s) - 1.6f;   g = PF_MAX2(0.0f, g);  g = PF_MIN2(0.6f, g);
       b = (1.0f * s) - 1.6f;   b = PF_MAX2(0.0f, b);  b = PF_MIN2(1.0f, b);
       r = hazeWeight*haze_color[R] + (1-hazeWeight)*r;
       g = hazeWeight*haze_color[G] + (1-hazeWeight)*g;
       b = hazeWeight*haze_color[B] + (1-hazeWeight)*b;
       pfESkyColor(chan->earth_sky, PFES_CLEAR, tod*r, tod*g, tod*b, 1.0f);
    }
#else
    /* update clear color in case of no ground plane */
    pfESkyColor(chan->earth_sky, PFES_CLEAR, 0.25f,0.15f,1.0f, 1.0f);
#endif

    if ( (G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
         (G_dynamic_data->window_size == WIN_TWO_TPORT) ) {
       pfFogColor ( chan->fog_ptr, chan->fog_color[R],
                                   chan->fog_color[G],
                                   chan->fog_color[B] );
    }
    else {
       pfFogColor ( chan->fog_ptr, (tod * chan->fog_color[R]),
                                   (tod * chan->fog_color[G]),
                                   (tod * chan->fog_color[B]) );
    }
    pfESkyColor ( chan->earth_sky, PFES_CLOUD_BOT,
                  (tod * chan->cloud_bot_color[R]),
                  (tod * chan->cloud_bot_color[G]),
                  (tod * chan->cloud_bot_color[B]), 0.4f );
    pfESkyColor ( chan->earth_sky, PFES_CLOUD_TOP,
                  (tod * chan->cloud_top_color[R]),
                  (tod * chan->cloud_top_color[G]),
                  (tod * chan->cloud_top_color[B]), 0.8f );


    /* adjust sun ambient, color and position */
    if ( (G_dynamic_data->window_size == WIN_THREE_BUNKER) ||
         (G_dynamic_data->window_size == WIN_TWO_TPORT) ) {
       sun->ltamb = 0.4f * tod;
       sun->lmamb = 0.15f;
       sun->ltcol = 0.8f;
       sun->px = 0.0f;
       sun->pz = 1.0f;
       sun->py = -1.0f;
    }
    else {
       sun->ltamb = 0.5f * tod;
       sun->lmamb = 0.5f * tod;
       sun->ltcol = 1.0f * tod;
       sun->px = 1.0f - tod;
       sun->pz = tod;
       sun->py = 0.1f;
    }

    sun->changed = TRUE;

}

#ifndef NO_DVW
 extern "C" float set_nps_tod(float tod, double *sun_vec)
#else
 extern "C" float set_nps_tod(float tod, double *)
#endif
 {
#ifndef NO_DVW
    extern CHANNEL_DATA  *viewChan;
    SUN *sun = (SUN *)G_sun;
    updatetod( tod, viewChan );
    /* adjust sun position */
    sun->px = (float)sun_vec[0];
    sun->py = (float)sun_vec[1];
    sun->pz = (float)sun_vec[2];

 /************** WAS ****************
    sun->pz = (float)sun_vec[1];
    sun->py = (float)sun_vec[2];
 ************** END ******************/

    /* adjust sun ambient and color */
    sun->ltamb = 0.6f * (float)tod;
    sun->lmamb = 0.0f;
    sun->ltcol = 0.6f * (float)tod;

    if ( (sun->ltamb + sun->ltcol) > 1.0 )
    {
       if ( sun->ltamb > 0.5 )
       {
          sun->ltamb = 0.5;
       }
    }
#endif
    return (float)tod;
 }
