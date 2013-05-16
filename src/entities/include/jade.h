// File: <jade.h> 

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

#ifndef __JADE_H__
#define __JADE_H__

#include "person.h"
#include "human_const.h"
#include "dude.h"
#include "jack_di_veh.h"

enum {JACKHUMAN, DUDEHUMAN};
enum {JADE_GOOD = 99, JADE_EVIL = 101};

#define JADERANGE 100

// class for creation for jades
class JADECLASS: public PERSON_VEH {

public:
   // jade constructor and destructor
   JADECLASS(int,int,ForceID);
   ~JADECLASS(); 

protected:

   // jade functions
   int  getHuman () {return human;}
   void switchHuman();
   virtual void entitystateupdate(EntityStatePDU *, sender_info &);
   void changestatus(unsigned int);
   movestat move();
   movestat moveDR(float, float); 
   void uploadData();
   void downloadData();
   void downloadData2Jack();

   // jade variables
   int          human;
   int          movement_index;
   CHANNEL_DATA *chan;

   // holds each new instantiation of jade
   pfNode       *body;
   pfGroup      *jade_GP;
   pfSwitch     *whichSwitch;

   // used to create a jack and dude 
   JACK_DI_VEH  *jack;
   DUDECLASS    *dude;
};

#endif 
