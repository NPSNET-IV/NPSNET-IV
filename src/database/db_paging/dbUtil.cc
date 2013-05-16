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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "dbUtil.h"

static int db_gen_multi_tag (char *, int, int, ...);

int db_num_digits (int theNum)
{
   int theCount = 0;

   // I'm count zero as having one digit.  I'm not sure
   // if it should be hanlded this way.

   if (theNum == 0) {
      return 1;
   }

   while (theNum != 0) {
      theNum = int (theNum / 10);
      theCount++;
   }

   return theCount;
}

char *db_gen_tag (int theX, int maxX, int theY, int maxY)
{
   int theSize = db_num_digits (maxX) + db_num_digits (maxY) + 1;
   char *theStr = new char[theSize];
   db_gen_multi_tag (theStr, theSize, 2, theX, maxX, theY, maxY);
   return theStr;
}

// This is a low level function that generate a unique tag for
// any dimention

int db_gen_multi_tag (char *theTag, int tagSize, int theDim, ...)
{
   va_list ap;
   int charCount = 0;
   va_start (ap, theDim);

   // Make certain that theTag is a NULL string so that
   // no junk in place at the front of the tag, the first
   // time that sprintf is called. -rb
 
   theTag[0] = '\0';

   for (int ix = 0; ix <= theDim - 1; ix++) {
      int tagIndex = va_arg (ap, int);
      int tagMaxIndex = va_arg (ap, int);

      if (tagIndex > tagMaxIndex) {
         cerr << "ERROR: in db_gen_multi_tag,"
              << " tagIndex > thaMaxIndex"
              << endl;
         return FALSE;
      }

      int tagIndexSize = db_num_digits (tagIndex);
      int tagMaxIndexSize = db_num_digits (tagMaxIndex);
      charCount += tagMaxIndexSize;

      if (charCount >= tagSize) {
         cerr << "ERROR: in db_gen_multi_tag,"
              << " charCount >= tagSize"
              << endl;
         return FALSE;
      }

      int tagZeroBuffer = tagMaxIndexSize - tagIndexSize;
      char *tagZeroBufferArray = new char[tagZeroBuffer + 1];
      for (int jx = 0; jx <= tagZeroBuffer - 1; jx++) {
         tagZeroBufferArray[jx] = '0';
      }
      tagZeroBufferArray[jx] = '\0';

      sprintf (theTag, "%s%s%d", theTag, tagZeroBufferArray, tagIndex);

      delete []tagZeroBufferArray;
   }

   return TRUE;
}

char *dbCpyStr (char *s)
{
   char *tempStr = new char[strlen (s) + 1];
   strcpy (tempStr, s);
   return tempStr;
}
