// File: <dude.h> 

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

#ifndef __DUDE_H__
#define __DUDE_H__

#include "person.h"
#include "human_const.h"

enum {DUDE_FRIEND = 97, DUDE_ENEMY};

class JADECLASS;

// class for creation for dudes
class DUDECLASS: public PERSON_VEH {

   friend class JADECLASS;

protected:
   // holds each new instantiation of dude
   pfNode       *body;

   // DCS nodes corresponding to body segments
   pfDCS        *main_DCS,        *head_DCS, 
                *armright_DCS,    *armleft_DCS,
                *armrightlow_DCS, *armleftlow_DCS,
                *legright_DCS,    *legleft_DCS,
                *legrightlow_DCS, *legleftlow_DCS,
                *far_body_DCS;

   // switch nodes to body segments
   pfSwitch     *sw_SW, *sw1_SW, *sw2_SW, *sw3_SW, *sw4_SW;

   // for culling functions
   CHANNEL_DATA *chan;

   // bounding sphere for each dude instantiation and culling point
   pfSphere	*Sphere; 
   pfVec3	pt;

   // draw variables
   Boolean      cull;
   Boolean      dead;
   double       distance;
   int          Jack_offset;
   int          medLODrange;
   int          medfarLODrange;
   int          farLODrange;
 
   // walking variables
   float        ArmBy; 
   float        DeltaArmBy;
   float        LegBy; 
   float        calfright; 
   float        calfleft; 
   Boolean      first_step;
   float        speed;
   float        oldspeed;

   // viewing variables
   float        direction;
   float        elevation;

   // driven dude functions
   void dude_walk();
   void dude_stand();
   void dude_prone();
   void dude_kneel();
   void dude_dead();
   void dude_appear();
   void rot_elev_head(); 
   movestat move();

   // remote dude functions
   void dude_DR(float, float);
   void dude_deadDR();
   void rot_elev_headDR(float, float); 
   movestat moveDR(float, float); 

   // common to both dude functions
   void attach_pt();
   void draw();                  
   void drawpt();                
   void dude_switch();
   virtual void create_entity(int, ForceID); 
   void changestatus(unsigned int); 
   void calc_dist();                  

public:
   // constructors and destructor
   DUDECLASS(int,int,ForceID);
   DUDECLASS(pfDCS *, pfSwitch *, int); 
   ~DUDECLASS();
};

//the basic structure used to hold the geometry, this has to be a 
//structure, it is the only way we can get it out of shared memory

typedef struct {
   pfDCS    *hullDCS,       *headDCS,
            *legleftDCS,    *legrightDCS,
            *legleftlowDCS, *legrightlowDCS,
            *armleftDCS,    *armrightDCS,
            *armleftlowDCS, *armrightlowDCS,
            *far_bodyDCS;

   pfGroup  *rootGP,       *torsoGP,    *headGP,
            *armleftlowGP, *armrightlowGP,
            *armleftupGP,  *armrightupGP,
            *armleftGP,    *armrightGP, 
            *legleftlowGP, *legrightlowGP,
            *legleftupGP,  *legrightupGP,
            *legleftGP,    *legrightGP;

   pfSwitch *sw, *sw1, *sw2, *sw3, *sw4;
 
   pfGroup  *far_body,    *body,
            *skull,       *face,
            *armleftmed,  *armleftup,  *armleftlow,
            *armrightmed, *armrightup, *armrightlow,
            *legleftmed,  *legleftup,  *legleftlow,
            *legrightmed, *legrightup, *legrightlow;

   // dude_med creation variables
   float  colors_med        [4][3];
   float  norms_med         [5][3];
   float  texs_med          [4][2];
   float  verts_med         [12][3];
   ushort index_med         [20];
   ushort index_medB        [20];
   ushort normvert_med      [5];
   ushort texsvert_med      [20];

   float  verts_head_med    [24];
   ushort index_head_med    [16];
   ushort norms_head_med    [4];
   ushort texs_head_med     [16];

   float  verts_face_med    [12];
   ushort index_face_med    [4];
   ushort norms_face_med    [1];
   ushort texs_face_med     [4];

   float  verts_arm_med_up  [24];
   float  verts_arm_med_low [24];

   float  verts_leg_med_up  [24];
   float  verts_leg_med_low [24];

   // dude_medfar creation variables
   float  verts_arm         [24];
   float  verts_leg         [24];

   // dude_far creation variables
   float  verts_far         [12][3];
   ushort index_far         [20];
   ushort normvert_far      [5];

} DUDE_GEOM;         

#endif
