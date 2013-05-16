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

#ifndef DB_PAGING_CLASS_DOT_H
#define DB_PAGING_CLASS_DOT_H

#include <Performer/pf.h>

#include "dbUtil.h"
#include "dbStorageClass.h"
#include "tsg.h"
#include "tsgpf.h"

// Error Types.  These are bit flags which are stored in dbError
const unsigned int DB_ERROR_NONE            = 0x00000000;
const unsigned int DB_ERROR_IGNORE          = 0x00000001;
const unsigned int DB_ERROR_FATAL           = 0x00000002;
const unsigned int DB_ERROR_OFF_DB          = 0x00000004;
const unsigned int DB_ERROR_BAD_DIM         = 0x00000008;
const unsigned int DB_ERROR_NO_TSG_CONTEXT  = 0x00000010;
const unsigned int DB_ERROR_REOPEN_TSG      = 0x00000020;

const int DB_MAX_STRING_SIZE        = 100;

class dbPaging {

   private:
      dbStorage    *dbStore;

      float         lastXPosition;
      float         lastYPosition;
      float         lastHeading;
      float         tileSize;

      // the "ws" prefix is short for Working Set
      // I was just too lazy to type out workingSetDumbyVariable
      // every time... -rb
      int           wsOffset;
      int           wsOldOffset;
      int           wsCenterTileX;
      int           wsCenterTileY;

      // the "db" prefix is short for Data Base.  See above
      // reason... -rb
      int           dbMinX;
      int           dbMinY;
      int           dbMaxX;
      int           dbMaxY;

      int           numberPagesAllowed;
      int           numberPagesInMem;

      tsgContext   *tinfo;
 
      unsigned int  dbError;
      
      int parse_header_file (tsgContext *);
      int is_in_ws (int, int, int, int, int, int);

   public:
      dbPaging (float, float, float, tsgContext * = (tsgContext *) NULL, 
                dbDeletePage * = (dbDeletePage *) NULL,
                pfGroup * = (pfGroup *) NULL);

     ~dbPaging ();
      int  update_number_pages_allowed (int);
      int  update_ws (int);
      void update (float, float, float);
      void print (void);
};

#endif // DB_PAGING_CLASS_DOT_H
