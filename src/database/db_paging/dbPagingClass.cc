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

#include <fstream.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Performer/pf.h>
#include "dbUtil.h"
#include "dbDeletePageClass.h"
#include "dbStorageClass.h"
#include "dbPagingClass.h"
#include "tsg.h"
#include "tsgpf.h"


int hack = 0;
#define pl cerr << hack++ << endl

dbPaging::dbPaging (float posX, float posY, float theHeading,
                    tsgContext *tsgInfo,
                    dbDeletePage *dbPageDeleter,
                    pfGroup *scene)
{
   dbStore = (dbStorage *) NULL;
   tileSize = 0.0f;
   wsOffset = 0;
   wsOldOffset = -1;
   dbMinX = 0;
   dbMinY = 0;
   dbMaxX = 0;
   dbMaxY = 0;
   numberPagesAllowed = 1;
   numberPagesInMem = 0;

   dbError = DB_ERROR_NONE;

   if (!tsgInfo) {
      dbError |= DB_ERROR_FATAL | DB_ERROR_NO_TSG_CONTEXT;
      return;
   }

   parse_header_file (tsgInfo);
 
   if (dbMinX > dbMaxX) {
      cerr << "Fatal Error in dbPaging: dbMinX > dbMaxX" << endl;
      dbError |= DB_ERROR_BAD_DIM | DB_ERROR_FATAL;
      return;
   }

   if (dbMinY > dbMaxY) {
      cerr << "Fatal Error in dbPaging: dbMinX > dbMaxX" << endl;
      dbError |= DB_ERROR_BAD_DIM | DB_ERROR_FATAL;
      return;
   }

   if (wsOffset < 0) {
      cerr << "Warning in dbPaging: " << endl
           << "\tWorking Set Offset is negative: "
           << wsOffset << endl;
      wsOffset = -wsOffset;
      cerr << "\tAdjusting to: " << wsOffset << endl;
   }


   if ( (dbMinX + (2 * wsOffset) ) > dbMaxX) {
      cerr << "Warning in dbPaging: " << endl
           << "\tWorking Set Offset for X is too large: "
           << wsOffset << endl;
      wsOffset--;
      while ( (dbMinX + (2 * wsOffset) ) > dbMaxX) {
         wsOffset--;
      }
      cerr << "\tAdjusting to: " << wsOffset << endl;
   }

   if ( (dbMinY + (2 * wsOffset) ) > dbMaxY) {
      cerr << "Warning in dbPaging: " << endl
           << "\tWorking Set Offset for Y is too large: "
           << wsOffset << endl;
      wsOffset--;
      while ( (dbMinY + (2 * wsOffset) ) > dbMaxY) {
         wsOffset--;
      }
      cerr << "\tAdjusting to: " << wsOffset << endl;
   }

   lastXPosition = posX - 1;
   lastYPosition = posY - 1;

   int minPages = (1 + (2 * wsOffset)) * (1 + (2 * wsOffset));

   if (numberPagesAllowed < minPages) {
      cerr << "Warning in dbPaging: " << endl
           << "\tThe number pages allowed < minimum pages required." << endl
           << "\tnumber pages allowed: " << numberPagesAllowed << endl
           << "\tminimum pages required:  " << minPages << endl
           << "\tsetting number pages allowed = " << minPages << endl;
      numberPagesAllowed = minPages;
   }

   dbStore = new dbStorage (dbMinX, dbMinY, dbMaxX, dbMaxY,
                            tsgInfo, dbPageDeleter, scene);

   update (posX, posY, theHeading);
}

dbPaging::~dbPaging ()
{
   delete dbStore;
}

int dbPaging::update_number_pages_allowed (int numAllowed)
{
   int theResult = TRUE;

   numberPagesAllowed = numAllowed;

   int minPages = (1 + (2 * wsOffset)) * (1 + (2 * wsOffset));

   if (numberPagesAllowed < minPages) {
      theResult = FALSE;
      cerr << "Warning in dbPaging: " << endl
           << "\tThe number pages allowed < minimum pages required." << endl
           << "\tnumber pages allowed: " << numberPagesAllowed << endl
           << "\tminimum pages required:  " << minPages << endl
           << "\tsetting number pages allowed = " << minPages << endl;
      numberPagesAllowed = minPages;
   }

   return theResult;
}
   

int dbPaging::update_ws (int theOffset)
{
   int theResult = TRUE;

   wsOffset = theOffset;

   if (wsOffset != wsOldOffset) {

      wsOldOffset = wsOffset;

      if (wsOffset < 0) {
         cerr << "Warning in dbPaging: " << endl
              << "\tWorking Set Offset is negative: "
              << wsOffset << endl;
         wsOffset = -wsOffset;
         cerr << "\tAdjusting to: " << wsOffset << endl;
      }

      if ( (dbMinX + (2 * wsOffset) ) > dbMaxX) {
         cerr << "Warning in dbPaging: " << endl
              << "\tWorking Set Offset for X is too large: "
           << wsOffset << endl;
         wsOffset--;
         while ( (dbMinX + (2 * wsOffset) ) > dbMaxX) {
            wsOffset--;
         }
         cerr << "\tAdjusting to: " << wsOffset << endl;
      }

      if ( (dbMinY + (2 * wsOffset) ) > dbMaxY) {
         cerr << "Warning in dbPaging: " << endl
              << "\tWorking Set Offset for Y is too large: "
              << wsOffset << endl;
         wsOffset--;
         while ( (dbMinY + (2 * wsOffset) ) > dbMaxY) {
            wsOffset--;
         }
         cerr << "\tAdjusting to: " << wsOffset << endl;
      }

      int minPages = (1 + (2 * wsOffset)) * (1 + (2 * wsOffset));

      if (numberPagesAllowed < minPages) {
         cerr << "Warning in dbPaging: " << endl
              << "\tThe number pages allowed < minimum pages required." << endl
              << "\tnumber pages allowed: " << numberPagesAllowed << endl
              << "\tminimum pages required:  " << minPages << endl
              << "\tsetting number pages allowed = " << minPages << endl;
         numberPagesAllowed = minPages;
      }
   }

   return theResult;
}

void dbPaging::update (float theX, float theY, float theHeading)
{

   if (dbError & DB_ERROR_FATAL) {
      return;
   }

   if ((theX == lastXPosition) 
     && (theY == lastYPosition)
     && (theHeading == lastHeading)
                                     ) {
      return;
   }

   wsCenterTileX = int ((theX  / tileSize) + (wsOffset 
                        * sinf (-PF_DEG2RAD (theHeading))));

   if ((wsCenterTileX - wsOffset) < dbMinX) {
      wsCenterTileX = dbMinX + wsOffset;
   }
   if ((wsCenterTileX + wsOffset) > dbMaxX) {
      wsCenterTileX = dbMaxX - wsOffset;
   }

   wsCenterTileY = int ((theY / tileSize) + (wsOffset
                        * cosf (-PF_DEG2RAD (theHeading))));

   if ((wsCenterTileY - wsOffset) < dbMinY) {
      wsCenterTileY = dbMinY + wsOffset;
   }
   if ((wsCenterTileY + wsOffset) > dbMaxY) {
      wsCenterTileY = dbMaxY - wsOffset;
   }

   lastXPosition = theX;
   lastYPosition = theY;
   lastHeading = theHeading;

   for (int ix = wsCenterTileX - wsOffset; 
        ix <= wsCenterTileX + wsOffset; ix++) {
      for (int jy = wsCenterTileY - wsOffset; 
        jy <= wsCenterTileY + wsOffset; jy++) {
         if ( !(dbStore->index_hit (ix,jy) ) ) {
            numberPagesInMem++;
         }
      }
   }

   int numPagesToDelete = numberPagesInMem - numberPagesAllowed;

   for (ix = 1; ix <= numPagesToDelete; ix++) {
      if (dbStore->delete_next_page () ) {
         numberPagesInMem--;
      }
      else {
         cerr << "Error in dbPaging::update." << endl
              << "\tTried to delete a page, but none in memory."
              << endl;
      }
   }

   if (numberPagesInMem > numberPagesAllowed) {
      cerr << "Too many pages in memory." << endl;
   }

//   dbStore->print_db_visual ();

}

int dbPaging::parse_header_file (tsgContext *tsgInfo)
{
   if (!tsgReadReopen (tsgInfo)) {
      cerr << "Fatal Error in dbPaging: Unable to reopen file in DBASE process."
           << endl;
      dbError = DB_ERROR_FATAL | DB_ERROR_REOPEN_TSG;
      return FALSE;
   }

   tsgInfo->indexed = 0;

   dbMinX = 0;
   dbMinY = 0;
   dbMaxX = tsgInfo->tiles_x - 1;
   dbMaxY = tsgInfo->tiles_y - 1;
   tileSize = tsgInfo->tile_size;

   return TRUE;
}

int dbPaging::is_in_ws (int theX, int theY, int edgeX, int edgeY,
                            int ix, int jy)
{
   int theResult = FALSE;

   int theOffsetX = 0;
   int theOffsetY = 0;

         theOffsetX = wsOffset;
         theOffsetY = wsOffset;

   if ( ( (theX + ix) >= (edgeX) ) &&
        ( (theY + jy) >= (edgeY) ) &&
        ( (theX + ix) <= (edgeX + (2 * theOffsetX) ) ) &&
        ( (theY + jy) <= (edgeY + (2 * theOffsetY) ) ) ) {
      theResult = TRUE;
   }

   return theResult;
}

void dbPaging::print ()
{
   dbStore->print_db_visual ();
}
