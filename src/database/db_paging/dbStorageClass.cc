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

#include <iostream.h>
#include <fstream.h>
#include <Performer/pf.h>
#include <Performer/pfutil.h>
#include <Performer/pfdu.h>
#include "dbUtil.h"
#include "dbStorageClass.h"
#include "tsg.h"
#include "tsgpf.h"

#ifdef DB_NODE_COUNT
static int nodeCount = 0;
#endif

ostream &operator<< (ostream &, dbPageElement *);

ostream &operator<< (ostream &dump, dbPageElement *thePage)
{
   dump << "X: " << thePage->dbX
        << "  Y: " << thePage->dbY << endl;
   return dump;
}

dbStorage::dbStorage (int theMinX, int theMinY, int theMaxX, int theMaxY,
                      tsgContext *theInfo,
                      dbDeletePage *dbPageDeleter,
                      pfGroup *theRoot)
{
   dbMinX = theMinX;
   dbMinY = theMinY;
   dbMaxX = theMaxX;
   dbMaxY = theMaxY;
   if (!theInfo) {
      cerr << "Fatal Error in dbStorage: tsgInfo is NULL." << endl;
   }

   tsgInfo = theInfo;

   dbPageBin = dbPageDeleter;

   dbPageElement *tempPage = (dbPageElement *) NULL;

   storageSize = (dbMaxX - dbMinX + 1) * (dbMaxY - dbMinY + 1);


   dbPages = new dbPageElement[storageSize];
   llTail = llHead = new dbPageElement;

   llHead->pNode = (dbPageElement *)NULL;
   llHead->cNode = (dbPageElement *)NULL;

   for (int sx = dbMinX; sx <= dbMaxX; sx++) {
      for (int sy = dbMinY; sy <= dbMaxY; sy++ ) {
         tempPage = get_pointer (sx, sy);
         tempPage->parent = NULL;
      }
   }

   build_quadtree (theRoot);

   for (int ix = dbMinX; ix <= dbMaxX; ix++) {
      for (int jy = dbMinY; jy <= dbMaxY; jy++ ) {
         tempPage = get_pointer (ix, jy);
         tempPage->tile = (pfGroup *) NULL;
         if (!tempPage->parent) {
            cerr << "Warning, Parent node was not assigned..." << endl;
         }
         tempPage->dbX = ix;
         tempPage->dbY = jy;
         tempPage->pNode = (dbPageElement *) NULL;
         tempPage->cNode = (dbPageElement *) NULL;
      }
   }
#ifdef DB_NODE_COUNT
   cerr << "Node Count: " << nodeCount << endl;
#endif
}

dbStorage::~dbStorage ()
{
   delete []dbPages;
   delete llHead;
}

int dbStorage::index_hit (int theX, int theY)
{
   int theResult = TRUE;

   dbPageElement *tempPage = get_pointer (theX, theY);

   if (tempPage) {

      if (!tempPage->tile) { 
         tempPage->tile = (pfGroup *) tsgReadTile (tsgInfo, theX, theY);
//         set_intersect_mask(tempPage->tile,
//                     (PERMANENT_DIRT<<PERMANENT_SHIFT), COLLIDE_DYNAMIC);
         pfBufferAdd (tempPage->parent, tempPage->tile);
#ifdef WA53
         pfMergeBuffer ();
#endif

         llTail->cNode = tempPage;
         tempPage->pNode = llTail;
         tempPage->cNode = (dbPageElement *) NULL;
         llTail = tempPage;
         theResult = FALSE;
      }
      else {
         if (tempPage != llTail) {
            tempPage->cNode->pNode = tempPage->pNode;
            tempPage->pNode->cNode = tempPage->cNode;
            llTail->cNode = tempPage;
            tempPage->pNode = llTail;
            llTail = tempPage;
         }
      }
   }

   return theResult;
}

int dbStorage::delete_next_page ()
{
   //cerr << "Delete" << endl;
   int theResult = FALSE;

   dbPageElement *tempPage = llHead->cNode;

   if (tempPage) {
      if (tempPage == llTail) {
         llHead->cNode = (dbPageElement *) NULL;
         llTail = llHead;                                   
      }
      else {
         llHead->cNode = tempPage->cNode;
         tempPage->cNode->pNode = llHead;
      }
   }

   if (tempPage) {
      if (tempPage->tile) {

         theResult = TRUE;

         pfBufferRemove (tempPage->parent, tempPage->tile);
#ifdef WA53
         pfMergeBuffer ();
#endif

         //cerr << "Deleting X: " << tempPage->dbX
         //     << "  Y: " << tempPage->dbY << endl;

         //pfAsyncDelete (tempPage->tile);

         dbPageBin->deletePage ((pfNode *) tempPage->tile);

         tempPage->pNode = (dbPageElement *) NULL;
         tempPage->cNode = (dbPageElement *) NULL;

         tempPage->tile = (pfGroup *) NULL;
      }
   }

   return theResult;
}

void dbStorage::print_db ()
{
   dbPageElement *tempPage = (dbPageElement *) NULL;

   for (int ix = dbMinX; ix <= dbMaxX; ix++) {
      for (int jy = dbMinY; jy <= dbMaxY; jy++ ) {
         tempPage = get_pointer (ix, jy);

         cerr << "dbPageElement X: " << tempPage->dbX
              << " Y: " << tempPage->dbY << " is ";
         if (!tempPage->tile) {
            cerr << "NOT ";
         }

         cerr << "in memory." << endl;
      }
   }
}

void dbStorage::print_db_visual ()
{
   dbPageElement *tempPage = (dbPageElement *) NULL;

   int pagesInMem = 0;

   for (int ix = dbMinX; ix <= dbMaxX; ix++) {
      for (int jy = dbMinY; jy <= dbMaxY; jy++ ) {
         tempPage = get_pointer (ix, jy);
         if (!tempPage->tile) {
            cerr << ".";
         }
         else {
            pagesInMem++;
            cerr << "#";
         }
      }
      cerr << endl;
   }
   cerr << "Pages in Memory: " << pagesInMem << endl;
}

dbPageElement *dbStorage::get_pointer (int theX, int theY)
{
   int thePlace = ( (theY - dbMinY ) * (dbMaxX - dbMinX + 1) 
                 + (theX - dbMinX) );

   if (thePlace >= storageSize)  {
      cerr << "Warning, thePlace is too large." << endl;
      return (dbPageElement *) NULL;
   }

   if ( (theX > dbMaxX) || (theY > dbMaxY) ) {
      return (dbPageElement *) NULL;
   }

   if ( (theX < dbMinX) || (theY < dbMinY) ) {
      return (dbPageElement *) NULL;
   }

   dbPageElement *tempPage = &(dbPages[thePlace]);   

   return tempPage;
}

void dbStorage::build_quadtree (pfGroup *theRoot)
{
   pfGroup *tempGroup = pfNewGroup ();
#ifdef DB_NODE_COUNT
   nodeCount++;
#endif

   rbuild (tempGroup, dbMinX, dbMaxX, dbMinY, dbMaxY);

   int childNumber = pfGetNumChildren (tempGroup);

   if (!childNumber) {
      pfBufferAdd (theRoot, tempGroup);
#ifdef WA53
      pfMergeBuffer ();
#endif
   }
   else {
      pfNode *theChild = pfGetChild (tempGroup, 0);
      pfBufferAdd (theRoot, theChild);
#ifdef WA53
      pfMergeBuffer ();
#endif
      pfBufferRemove (tempGroup, theChild);
#ifdef WA53
      //pfMergeBuffer ();
#endif
      pfAsyncDelete (tempGroup);
   }
}

pfGroup *dbStorage::rbuild (pfGroup *theNode,
                           int theMinX, int theMaxX,
                           int theMinY, int theMaxY)
{
   pfGroup *tempGroup = (pfGroup *) NULL;
   int numXSide = theMaxX - theMinX;
   int numYSide = theMaxY - theMinY;
   
   if ((numXSide == 0) && (numYSide == 0)) {
         dbPageElement *tempPage = get_pointer(theMaxX, theMaxY);

         if (tempPage->parent) {
            cerr << "Parent already exists!" << endl;
         }
         else {
            tempPage->parent = theNode;
         }
   }
   else if ((numXSide != 0) && (numYSide == 0)) {

      tempGroup = pfNewGroup ();
#ifdef DB_NODE_COUNT
      nodeCount++;
#endif

      pfAddChild (theNode, (pfNode *)tempGroup);

      int nearXSide = numXSide/2;
      int farXSide = (numXSide/2)+1;

      rbuild (tempGroup, theMinX, theMinX + nearXSide, theMinY, theMinY);
      rbuild (tempGroup, theMinX + farXSide, theMaxX, theMinY, theMinY);


   }
   else if ((numXSide == 0) && (numYSide != 0)) {

      tempGroup = pfNewGroup ();
#ifdef DB_NODE_COUNT
      nodeCount++;
#endif

      pfAddChild (theNode, (pfNode *)tempGroup);

      int nearYSide = numYSide/2;
      int farYSide = (numYSide/2)+1;

      rbuild (tempGroup, theMinX, theMinX, theMinY, theMinY + nearYSide);
      rbuild (tempGroup, theMinX, theMinX, theMinY + farYSide, theMaxY);

   }
   else {

      tempGroup = pfNewGroup ();
#ifdef DB_NODE_COUNT
      nodeCount++;
#endif

      pfAddChild (theNode, (pfNode *)tempGroup);

      int nearXSide = numXSide/2;
      int nearYSide = numYSide/2;
      int farXSide = (numXSide/2)+1;
      int farYSide = (numYSide/2)+1;

      rbuild (tempGroup, 
              theMinX, theMinX + nearXSide, theMinY, theMinY + nearYSide);
      rbuild (tempGroup, 
              theMinX + farXSide, theMaxX, theMinY, theMinY + nearYSide);
      rbuild (tempGroup,
              theMinX, theMinX + nearXSide, theMinY + farYSide, theMaxY);
      rbuild (tempGroup,
              theMinX + farXSide, theMaxX, theMinY + farYSide, theMaxY);

   }

   return tempGroup;
}
