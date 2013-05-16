// File: <po_ball_funcs.h>

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


#ifndef __NPS_PO_BALL_FUNC_H__
#define  __NPS_PO_BALL_FUNC_H__

#include <sys/types.h>                           //  SAK.
#include "c2_const.h"

#include "po_net.h"
#include "po_ball.h"
#include "menu.h"

//const float ELEV198 = 72.0;
//const float DEPRESS198 = -5.0;
//const float TRAV198 = 45.0;


struct ballCB{
   void (*function)(BALL_BOX*);
   BALL_BOX *ballBox ;
};


void initBallStuff(ballType);
void ballOn(pfVec3, ballType);
void ballOff();


void assignBallCB(pfGroup * , void(*function)(BALL_BOX*),BALL_BOX* );

pfGeode* makeArrowGeo();
pfGeode* buildTrajPaths(float, float, float, float, float, float, BALL_BOX);
pfGeode* buildTrajFan(float, float, float, float, float, float, BALL_BOX); 
void upDateBoxGeo(BALL_BOX*);
void initBallMenu(BALL_BOX*);



//Callbacks
//***************************************************************************
void downBallCB(BALL_BOX*);
void upBallCB(BALL_BOX*);
void rightBallCB(BALL_BOX*);
void leftBallCB(BALL_BOX*);
void lowRightCB(BALL_BOX*);
void upRightCB(BALL_BOX*);
void upLeftCB(BALL_BOX*);
void lowLeftCB(BALL_BOX*);
void ballDownCB(MENU_LEVEL*);
void linefanCB(MENU_LEVEL*);
void fanCB(MENU_LEVEL*);
void _1GCB(MENU_LEVEL*);
void _3GCB(MENU_LEVEL*);
void _5WCB(MENU_LEVEL*);
void _8CB(MENU_LEVEL*);
void rootBallCB(MENU_LEVEL*);

#endif
