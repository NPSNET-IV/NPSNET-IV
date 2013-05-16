// File: <menu.h>

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

#ifndef __NPS_MENU_H_
#define __NPS_MENU_H_

#include "c2_const.h"
#include <math.h>
//#include "menu_funcs.h"
                          

const float BRANCH_ANGLE = M_PI / 16;
const float BRANCH_Z = 2;
const float BRANCH_LENGTH = BRANCH_Z / sin(BRANCH_ANGLE);
const float BRANCH_SHADOW = BRANCH_LENGTH * cos(BRANCH_ANGLE);


const int SWITCH_ON = -1;
const int SWITCH_OFF = -2;


const float BRANCH_THICK = 1.0;

const int MAX_CHILDREN = 20;

const float ROTATION_INCREMENT = 5.0;



class MENU_LEVEL{

public:
   pfDCS *root;
      pfGroup *nextLevelGroup;             //Will use this and build my own
                                           //switch.
      //pfSwitch *nextLevelSwitch;         //Documented BUG. Performer.
                                           //Cant't pick through switch!
         pfGroup *nodeGroup;
         pfDCS *rotateDCS;
            pfGroup *edgesGroup;
         pfGroup *downGroup;     
         pfGroup *rightGroup;  
         pfGroup *leftGroup;
         
   pfVec3   *points;                       //  The displacements of the next 
                                           //  level points.          
   int numChildren;
   int childNumber;

   MENU_LEVEL* nextLevel[MAX_CHILDREN];
   MENU_LEVEL *previousLevel;

   float cumRotation;

   void simSwitch(int);                     //Since performer can't pick through                                        //a switch I will simulate it using                                        //a group and pfRemoveChild.

   MENU_LEVEL(pfGroup *node, void(*function)(MENU_LEVEL*) );
   MENU_LEVEL(pfGroup *node, void(*function)(MENU_LEVEL*), void* );
   ~MENU_LEVEL();
   void addMenuChild(MENU_LEVEL *child);

};

#endif
