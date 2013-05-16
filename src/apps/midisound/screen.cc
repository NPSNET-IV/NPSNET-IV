// File: <screen.cc>

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

#include "screen.h"

static fmfonthandle bigfont;
static fmfonthandle middlefont;
static fmfonthandle littlefont;

short center_text ( fmfonthandle &the_font, const char *the_text )
   {
   float indent;
   float width;
   long tempx, tempy;
   long origx, origy;

   getsize (&tempx,&tempy);
   getorigin(&origx,&origy);
 
   width = float (fmgetstrwidth (the_font, the_text ));
   indent = origx + ((tempx-width)/2.0f);  
   return ( short(indent) );
   }

/**************** this routine gets the current system time********************/double get_time()
{
  struct timeval time ;
  struct timezone tzone;
  double secs, usecs;

  tzone.tz_minuteswest = 8100;
  gettimeofday(&time, &tzone);
  secs = (double)time.tv_sec;
  usecs = (double)time.tv_usec;
  usecs *= 0.000001;
  secs = secs + usecs;
  return secs;
}

void setupwin( long *win )
{
   
   foreground ();
   minsize(640,480);
   keepaspect(1279,1023);
   *win = winopen ("NPSNET 3D Sound Server Window");
   minsize(640,480);
   keepaspect(1279,1023);
   winconstraints();
   RGBmode ();
   doublebuffer();
   gconfig ();
   RGBcolor ( 0, 0, 255 );
   clear ();
   swapbuffers();
   clear ();
   qdevice (ESCKEY);
   qdevice ( OKEY );
   qdevice ( WINSHUT );
   qdevice ( WINQUIT );
   qdevice ( PADMINUS );
   qdevice ( PADPLUSKEY );
   qdevice ( DOWNARROWKEY );
   qdevice ( UPARROWKEY );
}


void draw_indicator ( short x, short y )
   {
   pushmatrix();
   translate ( x, y, 0 );
   circf ( 0, 0, 6 );
   popmatrix();
   }



int update_window ( STATE_REC &state )
   {
   int done = FALSE;
    
   static int first_time = 1;
   static char text[80];
   short left;
   short bottom; 
   short right;
   short top;
   short height;
   short width;
   short picsize;
   long tempx,tempy,origx,origy;
   static long oldsizex, oldsizey;


   getsize (&tempx,&tempy);
   getorigin(&origx,&origy);

   left = short(origx);
   bottom = short(origy);
   right =  left + short(tempx);
   top = bottom + short(tempy);
   height = short(tempy);
   width = short(tempx);
   picsize = short(0.25417f*height);

   viewport ( 0, width, 0, height );
   ortho2(left,right,bottom,top);
   

   if (first_time)
      {
      oldsizex = tempx;
      oldsizey = tempy;
      RGBcolor(0,0,128);
      clear();
      cmov2 ( left + 20.0f, top - 20.0f );
      RGBcolor(255,255,255);
      charstr("NPSNET-3D Startup.");
      cmov2 ( left + 20.0f, top - 40.0f );
      charstr("Please wait while loading fonts...");
      swapbuffers();

      fminit();
      littlefont = fmscalefont(fmfindfont("Screen-Bold"), 12.0f);
      middlefont = fmscalefont(fmfindfont("Times-Roman"), 24.0f);
      bigfont = fmscalefont(middlefont, 2.0f);
      first_time = 0;
      }
   
   if ( (tempy != oldsizey) || (tempx != oldsizex) )
      {
      reshapeviewport();
      oldsizex = tempx;
      oldsizey = tempy; 
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

   strcpy ( text, "presents" );
   cmov2 (center_text(littlefont, text), top - short(0.125f*height));
   fmprstr(text);

   RGBcolor(0,0,0);

   fmsetfont(bigfont);
   strcpy ( text,"NPSNET-3DSS");
   cmov2 (center_text(bigfont, text)-2, top - short(0.25417f*height)-2);
   fmprstr(text);

   RGBcolor(255,255,255);

   cmov2 (center_text(bigfont, text), top - short(0.25417f*height));
   fmprstr(text);

   


   fmsetfont(middlefont);
   strcpy ( text, "(3-Dimension Sound Server)" );
   cmov2 (center_text(middlefont, text), top-short(0.33333f*height));
   fmprstr(text);

   RGBcolor(238,201,0);
 
   fmsetfont(littlefont);
   strcpy ( text, "Written by: Russell Storms, John Roesli, Paul Barham, & Sussanah Bloch" );
   cmov2 (center_text(littlefont, text), top-short(0.39583f*height));
   fmprstr(text);
   strcpy ( text, "Network interface by:" );
   cmov2 (center_text(littlefont, text), top-short(0.475f*height));
   fmprstr(text);
   strcpy ( text, "Paul T. Barham, John Locke & Mike Macedonia");
   cmov2 (center_text(littlefont, text), top-short(0.5125f*height));
   fmprstr(text);

   strcpy ( text, "Faculty Support: Dr. Michael Zyda, Dr. David Pratt,");
   cmov2 (center_text(littlefont, text), top-short(0.57917f*height));
   fmprstr(text);
   strcpy ( text, "Dr. Robert McGhee & John Falby" );
   cmov2 (center_text(littlefont, text), top-short(0.61667f*height));
   fmprstr(text);
   strcpy ( text, "Thanks to our sponsors:  ARL, STRICOM, TRAC, ARPA & DMSO" );
   cmov2 (center_text(littlefont, text), top-short(0.65417*height));
   fmprstr(text);
/*
   strcpy ( text, "Distributed Interactive Simulation (DIS) Protocol" );
   cmov2 (center_text(littlefont, text), top-short(0.69167f*height));
   fmprstr(text);
*/

/*
   RGBcolor(255,0,255);
   strcpy ( text, "PDU Processing:" );
   cmov2 ( center_text(littlefont, text), top-short(0.75*height));
   fmprstr(text);
   strcpy ( text, "Entity State:          " );
   start_col = center_text(littlefont, text);
   end_col = start_col + short(fmgetstrwidth (littlefont, text )) + 5;
   line_pos = top-short(0.79*height);
   cmov2 ( start_col, line_pos );
   fmprstr(text);
   if ( type == EntityStatePDU_Type )
      RGBcolor(255,0,0);
   else
      RGBcolor(0,0,0);
   draw_indicator ( end_col, line_pos );

   RGBcolor(255,0,255);
   strcpy ( text, "Detonation:            " );
   start_col = center_text(littlefont, text);
   end_col = start_col + short(fmgetstrwidth (littlefont, text )) + 5;
   line_pos = top-short(0.83*height);
   cmov2 ( start_col, line_pos );
   fmprstr(text);
   if ( type == DetonationPDU_Type )
      RGBcolor(255,0,0);
   else
      RGBcolor(0,0,0);
   draw_indicator ( end_col, line_pos );

   RGBcolor(255,0,255);
   strcpy ( text, "Fire:                  " );
   start_col = center_text(littlefont, text);
   end_col = start_col + short(fmgetstrwidth (littlefont, text )) + 5;
   line_pos = top-short(0.87*height);
   cmov2 ( start_col, line_pos );
   fmprstr(text);
   if ( type == FirePDU_Type )
      RGBcolor(255,0,0);
   else
      RGBcolor(0,0,0);
   draw_indicator ( end_col, line_pos );
*/

   RGBcolor(255,0,255);

/*
   sprintf ( text, "Midi Bank #: %d", state.banknum );
   cmov2 ( center_text(littlefont, text), top-short(0.75*height));
   fmprstr(text);

   sprintf ( text, "Exercise ID:  %d", config.exercise );
   cmov2 ( center_text(littlefont, text), top-short(0.79*height));
   fmprstr(text);

   sprintf ( text, "Master Name:  %s", config.master_name );
   cmov2 ( center_text(littlefont, text), top-short(0.83*height));
   fmprstr(text);
*/




   RGBcolor(0,180,0);
   fmsetfont(middlefont);
   switch ( state.mode )
      {
      case loading:
         strcpy ( text, "Initializing and Loading sound bank ..." );
         break;
      case no_vehicle:
         strcpy ( text, "No DIS entities on the network ..." );
         break;
      case a_vehicle:
         strcpy ( text, "DIS entites on the network but not the same site as master ..." );
         break;
      case my_vehicle_alive:
         strcpy ( text, "Master is alive ..." );
         break;
      case my_vehicle_dead:
         strcpy ( text, "Master is dead ..." );
         break;
      case override:
         strcpy ( text, "Override mode is on ..." );
         break;
      case exiting:
         strcpy ( text, "Exiting the program ..." );
         break;
      default:
         strcpy ( text, "Unknown mode ... HELP!" );
         break;
      }
   cmov2 ( center_text (middlefont,text), top - short(0.97083f*height));
   fmprstr (text);


   swapbuffers();

  
   return done;

   }

void draw_event_map ( ENTITY_REC &master, STATE_REC &state, CONFIG_REC &config,
                      double interest_area, SOUND_EVENT_REC *sound )
   {
   int i;
   double ol, or, ob, ot;
   double vl, vr, vb, vt;
   short left;
   short bottom;
   short right;
   short top;
   short height, width;
   long tempx,tempy,origx,origy;
   char text[40];
   float current_vline, current_hline;
   double xpos, ypos, arrow_length, world_width;
   static first_time = TRUE;
   static double current_time = -1.0;
   static double last_time = -1.0;
   static double frame_rate[30];
   static int current_rate = 0;
   double avg_rate;
   float mapstart;

   if ( first_time )
      {
      current_time = get_time();
      last_time = current_time;
      first_time = FALSE;
      }

   getsize (&tempx,&tempy);
   getorigin(&origx,&origy);

   left = short(origx);
   bottom = short(origy);
   right =  left + short(tempx);
   top = bottom + short(tempy);
   height = short(tempy);
   width = short(tempx);


   ol = master.location.x - interest_area;
   or = master.location.x  + interest_area;
   ob = master.location.y - interest_area;
   ot = master.location.y  + interest_area;
   world_width = 2.0 * interest_area;

   vl = ( (1.0 - (MAP_FACTOR*(tempy/double(tempx))))/2.0 );
   vr = 1.0 - vl;
   vb = ( (1.0 - (MAP_FACTOR))/2.0 );
   vt = 1.0 - vb;
   vl *= tempx;
   vr *= tempx;
   vb *= tempy;
   vt *= tempy;

/* PRINT TEXT STUFF */
 
   RGBcolor(0,0,255);
   viewport ( 0, width, 0, height );
   ortho2(left,right,bottom,top);
   clear();


   RGBcolor ( 255, 255, 255 );

   fmsetfont(middlefont);

   sprintf ( text, "NPSNET 3D Sound Server" );
   cmov2 ( center_text(middlefont, text), top-short(0.065*height));
   fmprstr(text);

   if ( (strncmp(master.hostname,"NONE",4)) == 0)
      RGBcolor ( 0, 0, 0 );

   fmsetfont(littlefont);

   mapstart = (left+((1.0 - MAP_FACTOR)*width));

   sprintf ( text, "Current  " );
   cmov2 ( short(mapstart-(fmgetstrwidth(littlefont,text)+(0.03*width))),
           top-short(0.400*height));
   fmprstr(text);
   sprintf ( text, "Site %4d", master.entity_id.address.site );
   cmov2 ( short(mapstart-(fmgetstrwidth(littlefont,text)+(0.03*width))),
           top-short(0.450*height));
   fmprstr(text);
   sprintf ( text, "Host %4d", master.entity_id.address.host );
   cmov2 ( short(mapstart-(fmgetstrwidth(littlefont,text)+(0.03*width))),
           top-short(0.500*height));
   fmprstr(text);
   sprintf ( text, "Ent  %4d", master.entity_id.entity );
   cmov2 ( short(mapstart-(fmgetstrwidth(littlefont,text)+(0.03*width))),
           top-short(0.550*height));
   fmprstr(text);
   sprintf ( text, "%s", master.hostname );
   cmov2 ( short(mapstart-(fmgetstrwidth(littlefont,text)+(0.03*width))),
           top-short(0.600*height));
   fmprstr(text);


   sprintf ( text, "Center" );
   cmov2 ( left+((MAP_FACTOR+0.03)*width), 
           top-short(0.400*height));
   fmprstr(text);
   sprintf ( text, "X %2.0f", master.location.x );
   cmov2 ( left+((MAP_FACTOR+0.03)*width), 
           top-short(0.450*height));
   fmprstr(text);
   sprintf ( text, "Y %2.0f", master.location.y );
   cmov2 ( left+((MAP_FACTOR+0.03)*width), 
           top-short(0.500*height));
   fmprstr(text);

   sprintf ( text, "Range" ); 
   cmov2 ( left+((MAP_FACTOR+0.03)*width), 
           top-short(0.550*height));
   fmprstr(text);

   sprintf ( text, "%2.0f", interest_area );
   cmov2 ( left+((MAP_FACTOR+0.03)*width), 
           top-short(0.600*height));
   fmprstr(text);
   
   sprintf ( text, "Volume" ); 
   cmov2 ( left+((MAP_FACTOR+0.03)*width), 
           top-short(0.650*height));
   fmprstr(text);

   sprintf ( text, "%d", state.total_volume );
   cmov2 ( left+((MAP_FACTOR+0.03)*width), 
           top-short(0.700*height));
   fmprstr(text);

   RGBcolor ( 255, 255, 255 );


   sprintf ( text, "Midi Bank  : %d", state.banknum );
   cmov2 ( left+(0.1*width), top-short((MAP_FACTOR+0.125)*height));
   fmprstr(text);

   if ( config.exercise == (unsigned int)0 )
      sprintf ( text, "Exercise ID: Any" );
   else
      sprintf ( text, "Exercise ID: %d", config.exercise );
   cmov2 ( left+(0.1*width), top-short((MAP_FACTOR+0.155)*height));
   fmprstr(text);

   if ( config.multicast )
      strcpy ( text, "Multicast Mode" );
   else
      strcpy ( text, "Broadcast Mode" );

   cmov2 ( left+(0.1*width), top-short((MAP_FACTOR+0.185)*height));
   fmprstr(text);

   switch ( state.mode )
      {
      case loading:
         strcpy ( text, "Loading sound bank ..." );
         break;
      case no_vehicle:
         strcpy ( text, "No DIS entities ..." );
         break;
      case a_vehicle:
         strcpy ( text, "A DIS entity ..." );
         break;
      case my_vehicle_alive:
         strcpy ( text, "Master alive ..." );
         break;
      case my_vehicle_dead:
         strcpy ( text, "Master dead ..." );
         break;
      case override:
         strcpy ( text, "Override mode ..." );
         break;
      case exiting:
         strcpy ( text, "Exiting ..." );
         break;
      default:
         strcpy ( text, "Unknown mode" );
         break;
      }
  
   cmov2 ( left+(0.65*width), top-short((MAP_FACTOR+0.185)*height));
   fmprstr(text);

   if ( (strcmp(config.master_name,"")) == 0)
      sprintf ( text, "Master: %s(%d/%d/%d)", "Any",
                      config.entity_id.address.site,
                      config.entity_id.address.host,
                      config.entity_id.entity );
   else
      sprintf ( text, "Master: %s(%d/%d/%d)", config.master_name,
                      config.entity_id.address.site,
                      config.entity_id.address.host,
                      config.entity_id.entity );

   cmov2 ( left+(0.65*width), top-short((MAP_FACTOR+0.155)*height));
   fmprstr(text);

   if ( current_time != last_time )
      frame_rate[current_rate] = 1.0 / (current_time - last_time);
   else
      frame_rate[current_rate] = 0.0;

   current_rate = (current_rate + 1) % 30;

   last_time = current_time;
   current_time = get_time();

   avg_rate = 0.0;
   for ( i = 0; i < 30; i++ )
      avg_rate += frame_rate[i];
   avg_rate /= 30.0;

   sprintf ( text, "Frame Rate:  %2.0f", float(avg_rate) );
   cmov2 ( left+(0.65*width), top-short((MAP_FACTOR+0.125)*height));
   fmprstr(text);


/* DRAW MAP STUFF */

   viewport ( Screencoord(vl), Screencoord(vr),
              Screencoord(vb), Screencoord(vt) );
   ortho2 ( Coord(ol), Coord(or), Coord(ob), Coord(ot) );
   RGBcolor(0,0,0);
   clear();

   if ( (strncmp(master.hostname,"NONE",4)) == 0)
      {
      static int local_stage = 0;
      static int frame_count = 0;
      static char local_symbols[25];
      static float center;
      if ( local_stage )
         {
         strcpy ( local_symbols, "Searching For Master");
         RGBcolor(255,255,255);
         center = (ol+or)/2.0f;
         center -= float(fmgetstrwidth(littlefont,local_symbols)) *
                   ((or-ol)/1024.0f);
         cmov2 ( center, (ob+ot)/2.0f );
         fmprstr(local_symbols);
         }
      if ( ++frame_count > int(avg_rate) )
         {
         local_stage = !local_stage;
         frame_count = 0;
         }
      swapbuffers();
      return;
      }

/* Grid Lines */

//   RGBcolor(0,255,0);
   current_vline = (int(ol)/500)*500.0f;
   while ( current_vline < or )
      {
      if ( fmod(current_vline,1000.0f) == 0.0f )
         RGBcolor(238,201,0);
      else
         RGBcolor(0,0,255);
      move2 ( Coord(current_vline), Coord(ob) );
      draw2 ( Coord(current_vline), Coord(ot) );
      current_vline += 500.0f;
      }

   current_hline = (int(ob)/500)*500.0f;
   while ( current_hline < ot )
      {
      if ( fmod(current_hline,1000.0f) == 0.0f )
         RGBcolor(238,201,0);
      else
         RGBcolor(0,0,255);
      move2 ( Coord(ol), Coord(current_hline) );
      draw2 ( Coord(or), Coord(current_hline) );
      current_hline += 500.0f;
      }

/* Master's circle */

   RGBcolor(255,0,255);
   circf ( Coord(master.location.x),
           Coord(master.location.y),
           Coord(20.0) );

   move2 ( Coord(master.location.x),
           Coord(master.location.y) );
   arrow_length = 0.05*world_width;
   xpos = master.location.x + (arrow_length*cos(HALFPI+master.orientation.psi));
   ypos = master.location.y + (arrow_length*sin(HALFPI+master.orientation.psi));
   draw2 ( Coord(xpos), Coord(ypos) );

/* Sound event circles */

   i = state.headlist;
   while ( i != NIL )
      {
      switch ( sound[i].event_type )
         {
         case FirePDU_Type:
            RGBcolor ( 255, 0, 0 );
            cmov2 ( Coord(sound[i].event_location.x),
                    Coord(sound[i].event_location.y) );
            fmprstr ( "F" );   
            circ ( Coord(sound[i].event_location.x),
                   Coord(sound[i].event_location.y),
                   Coord(sound[i].event_radius));
            break;
         case DetonationPDU_Type:
            RGBcolor ( 255, 178, 80 );
            cmov2 ( Coord(sound[i].event_location.x),
                    Coord(sound[i].event_location.y) );
            fmprstr ( "D" );
            circ ( Coord(sound[i].event_location.x),
                   Coord(sound[i].event_location.y),
                   Coord(sound[i].event_radius));
            break;
         default:
            break;

         }
      i = sound[i].next_event;
      } 

   swapbuffers();

   }
 
