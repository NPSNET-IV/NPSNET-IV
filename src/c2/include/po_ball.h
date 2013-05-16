// File: <po_ball.h>

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

//SAK.  This header file is for po_ballistics.  It is used to visualize the 
//ballistic paths of various weapons.  It is not used for real time firing,
//rather it is used to visually depict probably paths of weapons using
//physically based methods.

#ifndef __NPS_PO_BALL_H_
#define __NPS_PO_BALL_H_

#include "c2_const.h"

enum ballType {M198, M60, Mort81mm};
enum active {NONE = -1, LR, UR, UL, LL, NO_ARROW};

const float ARROW_STEM = 11.0;
const float ARROW_LENGTH = 12.0;
const int BOX_ARROWS = 4;
const float INCREMENT = 5.0;
const pfVec3 ARROW = {ARROW_STEM, 0.0, 0.0};

class COEF{
public:
   float ELEV_INC;
   float TRAV_INC;

   float MAX_ELEV;
   float MAX_DEPRESS;
   float MAX_TRAV;
   float MUZZLE_V;
   float BARREL_L;
   
   float DRAG;

   float _1G;
   float _2G;
   float _3G;
   float _4G;
   float _5G;
   float _3W;
   float _4W;
   float _5W;
   float _6W;
   float _7W;
   float _8;

};

class BALL_BOX{

public:
   pfDCS* root,
          *lowRightDCS,
          *upRightDCS,
          *upLeftDCS,
          *lowLeftDCS;
//          *checkDCS;
//          *menuDCS;

   float    cumRotLeft,
          cumRotRight,
          cumRotUp,
          cumRotLow,                          //Heading and roll.
          spread;
          
   //pfGeode*  box;

   //pfVec3  *boxPoints;

   pfGroup *downGroup, 
           *upGroup,    
           *rightGroup,  
           *leftGroup;
           

   pfGroup *HLGroup;

   active current;

   ballType typeBall;

   COEF coef;
         
   BALL_BOX();
};
   
class M198_BOX : public BALL_BOX{

public:
   

   M198_BOX();
      

};

class M60_BOX : public BALL_BOX{

public:
   M60_BOX();
      

};

class Mort81mm_BOX : public BALL_BOX{

public:
   Mort81mm_BOX();
      

};

#endif
