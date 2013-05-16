// File: <menu_funcs.h>

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

//SAK.
//Author: Sam Kirby.
//This file supports the class MENU_LEVEL which is used to create "tornado"
//menus.

#ifndef __NPS_MENU_FUNCS_H_
#define __NPS_MENU_FUNCS_H_

#include "c2_const.h"
#include "menu.h"


enum arrowTypes {DOWN_ARR,UP_ARR, RIGHT_ARR, LEFT_ARR, DOWN_HL,UP_HL, RIGHT_HL,
   LEFT_HL, QUAD_DN, QUAD};

const int NUM_ARROWS = 10;


struct callBack{
   void (*function)(MENU_LEVEL*);
   MENU_LEVEL *nodeLevel ;
   void *userData;
};

void assignCB(pfGroup * , void(*function)(MENU_LEVEL*),MENU_LEVEL* );
void assignCB(pfGroup * , void(*function)(MENU_LEVEL*),MENU_LEVEL*,void* );

void initArrowGsets();
void makeArrowGset(pfGeoSet **, pfGeoSet **,pfGeoSet **, const char *,
   const char*);
pfGroup* makeArrow(pfGeoSet *, pfGeoSet *, pfGeoSet *,pfVec3);
MENU_LEVEL* initMenu();
void menuOn(pfVec3 position);
void menuOff(MENU_LEVEL* level);
int menu_Is_On();
pfGroup*  makeDown();
pfGroup* makeUp();
pfGroup*  makeRight();
pfGroup*  makeLeft();

pfGeode* makePoGeoTri(pfVec4, const char*);
pfGeode* makePoGeoLine(pfVec4, const char*);
pfBillboard* makeBBTri(pfVec4, const char*);
pfBillboard* makeBBLine(pfVec4, const char*);
pfGroup* makePickableLnStrip(pfVec4, const char*, pfVec3);
pfGroup* makePickableTriStrip(pfVec4, const char*, pfVec3);


//**************************************************************************
//Callbacks
void downCB(MENU_LEVEL*);
void rightCB(MENU_LEVEL*);
void leftCB(MENU_LEVEL*);


//  Should really seperate these and move into another file.
void ballisticCB(MENU_LEVEL *);
void howitzerCB(MENU_LEVEL *);
void howitzermCB(MENU_LEVEL *);
void howitzerhCB(MENU_LEVEL *);
void unitCB(MENU_LEVEL *);
void doneM198CB(MENU_LEVEL*);
void doneM60CB(MENU_LEVEL*);
void done81mmCB(MENU_LEVEL*);

#endif
