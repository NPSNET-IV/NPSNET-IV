// File: <highlight.cc>

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

#ifndef NOC2MEASURES

//  Changes made 5 Jul 1995 by Sam Kirby(SAK). Modifications to highlight
//  functions used by performer pickfly.

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <Performer/pf.h>
#include "globals.h"
#include "pfutil.h"
#include "highlight.h"


//Sets Graphics State.  This Update must be called from DRAW PROCESS.
void UpdateHighlight(Highlight *HL)
{
   if (HL->dirty > 0)
   {
      pfHlightMode(HL->hl, HL->mode);
      pfHlightColor(HL->hl, PFHL_FGCOLOR, 
         HL->color[0], HL->color[1], HL->color[2]);
      pfApplyHlight(HL->hl);
      HL->dirty--;
   }
}


void HighlightChannels(Highlight *HL, long n)
{
   if (n > 0)
   {
      HL->nChans = n;
      if (HL->dirty)
         HL->dirty = n;
    }
}


long GetHighlightChannels(Highlight *HL)
{
   if (HL)
      return HL->nChans;
   else 
      return NULL;
}


Highlight *NewHighlight(void)
{

   Highlight *HL = (Highlight *)pfMalloc(sizeof(Highlight), pfGetSharedArena());
	
   HL->nChans = 1;
   HL->mode = PFHL_FILL;
   pfSetVec3(HL->color, 1.0f, 0.0f, 1.0f);
   HL->node = NULL;
   HL->hl = pfNewHlight(pfGetSharedArena());
   HL->dirty = HL->nChans;
   return HL;
}

void HighlightMode(Highlight *HL, long mode)
{
    HL->mode = mode;
    HL->dirty = HL->nChans;
}

long GetHighlightMode(Highlight *HL)
{
    return HL->mode;
}

void HlightColor(Highlight *HL, pfVec3 c)
{
    pfCopyVec3(HL->color, c);
    HL->dirty = HL->nChans;
}

void GetHlightColor(Highlight *HL, pfVec3 dest)
{
    pfCopyVec3(dest, HL->color);
}

pfNode *GetHighlightNode(Highlight *HL)
{
   return HL->node;
}

void HighlightNode(Highlight *HL, pfNode *N)
{   
   if (HL->node != NULL) pfuTravNodeHlight(HL->node, PFHL_INHERIT);
   HL->node = N;
   if (HL->node != NULL) pfuTravNodeHlight(HL->node, PFHL_ON);
}

//  This function was built due to the the problems encounters with highlighting
//  a geoset on a billboard.  With this function I actually manipulate the
//  geoSet's attributes and change the color.  I save the old color as well
//  so that when the node is no longer highlighted I can restore its original
//  color.  The function "manually" highlights.
void poHighLight(pfGeoSet *gset){

   static pfGeoSet *old_gset = NULL;
   static void *old_alist = NULL;
   static ushort *old_ilist = NULL;
   static pfVec4 HLColor = {1.0, 0.0, 1.0, 1.0};

   //  pickedGroup is a global which is used to determine if a group is in
   //  fact picked.  It is used to ensure problem highlighting as well as 
   //  ensure pointers are handled correctly should the node be deleted.
   if (!pickedGroup){
      old_gset = NULL;
      old_alist = NULL;
      old_ilist = NULL;
   }

   if (old_gset != gset){
      if(old_gset != NULL){
         pfGSetAttr(old_gset,PFGS_COLOR4,PFGS_OVERALL,old_alist,old_ilist);
      }
      if(gset != NULL){
         pfGetGSetAttrLists(gset,PFGS_COLOR4,&old_alist,&old_ilist);
      }

      old_gset = gset;

      if(gset != NULL){
          pfGSetAttr(gset,PFGS_COLOR4,PFGS_OVERALL,HLColor,NULL);
      }

   }  // End if (old_gset != gset).
}  //  End of poHighLight.

#endif
