
// File: <globals.h>

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


#ifndef __NPS_PO_GLOBALS_H__
#define __NPS_PO_GLOBALS_H__

#include <pf.h>
#include "po_meas.h"
#include "po_funcs.h"
#include "menu.h"
extern "C"{
#include "libctdb.h"        //SAK.  Modsaf ctdb stuff.
}


#ifndef __PO_LOCAL__
#define __PO_LOCAL__ extern
#endif 

__PO_LOCAL__
pfVec4 *L_color;

__PO_LOCAL__
SP_SimulationAddress slaveSim;

__PO_LOCAL__
pfGroup *L_po_meas;  // the root of the measures tree

__PO_LOCAL__
SP_PersistentObjectProtocolVersion poProtocol;

__PO_LOCAL__
TEXTBLOCK *L_textblock;

__PO_LOCAL__
int L_textcnt;

//__PO_LOCAL__
//pfTexture   *L_po_point_tex[16];

//__PO_LOCAL__
//pfTexEnv *L_po_tex_env;

//__PO_LOCAL__
//pfVec2 L_texcoords[]={ {0.0f, 0.0f},
//                       {1.0f, 0.0f},
//                       {1.0f, 1.0f},
//                       {0.0f, 1.0f} };

//__PO_LOCAL__
//pfVec3 L_verts[]    ={ {-1.0f , 0.0f, 0.0f},
//                       { 1.0f,  0.0f, 0.0f},
//                       { 1.0f,  0.0f, 1.0},
//                       {-1.0f , 0.0f, 1.0}};


__PO_LOCAL__
poLookUp poArray[1000];

__PO_LOCAL__
int poCount;                       // Number we're maintaining now.

__PO_LOCAL__
unsigned short poSite;

__PO_LOCAL__
unsigned short poHost;

__PO_LOCAL__
unsigned int object;

__PO_LOCAL__
MENU_LEVEL* L_menu_root;

__PO_LOCAL__
unsigned char L_exercise;    // excercise ID

__PO_LOCAL__
unsigned char L_database;    // database ID

__PO_LOCAL__
pfGroup *pickedGroup;

__PO_LOCAL__
CTDB L_ctdb;                       // SAK.Dirt for the measures.

#endif


