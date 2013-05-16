/*
 * Copyright (c) 1995,1996
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

#include <Performer/pf.h>
#include "common_defs.h"
#include "dbPagingCallback.h"
#include "dbPagingClass.h"
#include "tsg.h"
#include "tsgpf.h"

extern volatile DB_SHARE *dbShare;

void db_paging_process (void *)
{
   static dbPaging *thePager = (dbPaging *) NULL;
   static pfBuffer *buf = (pfBuffer *) NULL;

   if (buf == (pfBuffer *) NULL) {
      // cerr << "pid for dbProcess: " << getpid () << endl;
      buf = pfNewBuffer ();
      pfSelectBuffer (buf);
   }
   
   if (!thePager) {
      thePager = new dbPaging (dbShare->x, dbShare->y,
                               dbShare->h,
                               dbShare->tsgInfo,
                               dbShare->dbPageBin,
                               dbShare->fixed);
      if (dbShare->numberAllowed != 0) {
         thePager->update_number_pages_allowed (dbShare->numberAllowed);
      }

      thePager->update_ws (dbShare->offset);

   }
   else {
        thePager->update (dbShare->x, dbShare->y, dbShare->h);
//      thePager->print ();
   }
#ifndef WA53
   pfMergeBuffer ();
#endif
   pfDBase ();
}

void dbPrintTSG (tsgContext *context)
{
   cerr << "\tLoading: " << context->path << endl
//        << "\tDatabase source: " << context->source << endl
//        << "\tDatabase comments: " << context->comment << endl
        << "\tExtents: (" 
        << context->ll_x << ","
        << context->ll_y << ") -> ("
        << context->ll_x+context->tile_size*context->tiles_x << ","
        << context->ll_y+context->tile_size*context->tiles_y << ")" << endl
        << "\tZone: " << context->zone 
        << " Projection params: " << context->projinfo
        << " Datum: " << context->datum << endl
        << "\tTile size: " << context->tile_size << endl
        << "\tTiles: (" 
        << context->tiles_x << ","
        << context->tiles_y << ")" << endl;
}
