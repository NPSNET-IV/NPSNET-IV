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

#ifndef DB_DELETE_PAGE_CLASS_DOT_H
#define DB_DELETE_PAGE_CLASS_DOT_H

#include <Performer/pf.h>

const int DB_DELETE_MAX_ARRAY_SIZE = 1000;

typedef struct _delList {
   pfNode *child;
} delList;

class dbDeletePage {

   private:
      int     addListPlace;
      int     removeListPlace;
      delList pageList[DB_DELETE_MAX_ARRAY_SIZE];

      inline int get_next_place(int currentPlace)
         {return (currentPlace + 1) % DB_DELETE_MAX_ARRAY_SIZE;}

   public:
      void *operator new(size_t);
      void operator delete(void *, size_t);

      dbDeletePage ();
     ~dbDeletePage () {;}
      int deletePage (pfNode *);
      void update ();

};

#endif // DB_DELETE_PAGE_CLASS_DOT_H
