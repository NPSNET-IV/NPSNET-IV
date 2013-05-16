// File: <fontdef.h>

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
/* this is an IRIS-4d Program.
   This is file fontdef.

   It contains the al declarations for routine fontdef so that
   other functions can access the font definition data arrays.
*/
#ifndef NPS_FONTDEF
#define NPS_FONTDEF

// OpenGL Static Defines
#define DEFFONT 0
#define TEXTFONT 1
#define ICONFONT 2
#define MAXFONTS 5

 void fontdef(long, char*); /* defines the fonts */

 // OpenGL Font Routines
 void npsMakeRasterXFont(char *fontName, pfuXFont *fnt);
 int npsGetXFontWidth(pfuXFont *f, const char *str);
 int npsGetXFontHeight(pfuXFont *f);
 pfuXFont *npsGetCurXFont();
 void npsCharPos(float x, float y, float z);
 void npsDrawString(const char *s);
 void npsDrawStringPos(const char *s,float x, float y, float z);
 void npsBuildFonts(float,float);
 void npsFont(int selection);

#endif
