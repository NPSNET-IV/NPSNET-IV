// File: <view.cc>

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

#include <math.h>
#include <string.h>
#include <bstring.h>
#include <iostream.h>

#include "view.h"

#include "macros.h"

#include "common_defs.h"
#include "common_defs2.h"
#include "appearance.h"

//#include "traker.h" // mcmillan
#include "hmdClass.h"

#include "collision_const.h"
#include "conversion_const.h"
#include "ship_veh.h"

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern          STATIC_DATA  *G_static_data;
extern          VEH_LIST     *G_vehlist;
extern volatile int           G_drivenveh;
extern          pfGroup      *G_fixed;

static pfVec3 ZeroVec = { 0.0f, 0.0f, 0.0f };

//======================================================================

void getHMDView(pfCoord &view, PASS_DATA *pdata)
{
  VEH_LIST *varr = &G_vehlist[G_drivenveh]; // Added for DIs
  VEHICLE *veh = varr->vehptr;

  // get the posture to look transformation matrix
  pfMatrix view_mat;
  G_static_data->hmd_unit->getOrientation(view_mat);
  pfCopyVec3(view_mat[3],pdata->look.xyz);
  pfGetOrthoMatCoord(view_mat, &(pdata->look));
  pdata->look.hpr[PF_R] = 0.0f;
  veh->set_look_hpr (pdata->look.hpr);

/*
  char tem[80];
  sprintf(tem, "look(h,p,r) = (%6.1f,%6.1f,%6.1f)",
          pdata->look.hpr[0], pdata->look.hpr[1], pdata->look.hpr[2]);
  cerr << tem << endl;
 */

  // get the performer to look transformation matrix
  pfMatrix posture_mat;
  pfMakeCoordMat(posture_mat, &pdata->posture);
  pfMultMat(view_mat, view_mat, posture_mat);
  pfGetOrthoMatCoord (view_mat, &view);
  view.hpr[PF_R] = 0.0f;
/*
  sprintf(tem, "posture = (%6.1f,%6.1f,%6.1f)",
          pdata->posture.hpr[0], pdata->posture.hpr[1], pdata->posture.hpr[2]);
  cerr << tem << endl;

  sprintf(tem, "view(h,p,r) = (%6.1f,%6.1f,%6.1f)",
          view.hpr[0], view.hpr[1], view.hpr[2]);
  cerr << tem << endl;
 */

  if ( G_dynamic_data->birdseye && veh ) {
     float speed;
     veh->attach((pfCoord )G_dynamic_data->birdsview,
                           speed,
                    (float)G_dynamic_data->birdsdis );
     view = G_dynamic_data->birdsview;
  }
 
  if ( !G_dynamic_data->camera ) {
     G_dynamic_data->camera = TRUE;
  } 
}

//======================================================================

void update_view (pfCoord &view, PASS_DATA *pdata, int &change, int camera)
{      
   pfMatrix orient;
   pfVec3 veh_x_vec = { 1.0f, 0.0f, 0.0f };
   pfVec3 veh_z_vec = { 0.0f, 0.0f, 1.0f };
   pfVec3 veh_y_vec = { 0.0f, 1.0f, 0.0f };
   pfVec3 view_offsets = { 0.0f, 0.0f, 0.0f };
  
   VEH_LIST *varr = &G_vehlist[G_drivenveh]; // Added for DIs
   VEHICLE *veh = varr->vehptr;
   pfMakeEulerMat ( orient, pdata->posture.hpr[HEADING], 
                            pdata->posture.hpr[PITCH], 
                            pdata->posture.hpr[ROLL]);
   
   pfFullXformPt3 ( veh_y_vec, veh_y_vec, orient );
   pfFullXformPt3 ( veh_x_vec, veh_x_vec, orient );
   pfFullXformPt3 ( veh_z_vec, veh_z_vec, orient );
   pfNormalizeVec3 ( veh_x_vec );
   pfNormalizeVec3 ( veh_y_vec );
   pfNormalizeVec3 ( veh_z_vec );

   if (veh->getCat() != PERSON) { 
#ifndef NONPSSHIP
      if (veh->getCat() == SHIP){
         pfVec3 offset;
         ((SHIP_VEH *)veh)->getbridgeview(offset);
         pfScaleVec3 ( view_offsets, offset[1], veh_y_vec );
         view_offsets[Z] = offset[2];
      }
      else 
#endif
      if (veh->getCat() == DI_GUY) {
         pfScaleVec3 ( view_offsets, DI_EYE_OFFSET, veh_z_vec );
      }
      else {
         pfScaleVec3 ( view_offsets, EYE_OFFSET, veh_z_vec );
      }
   }

   pfScaleVec3 ( veh_x_vec, pdata->look.xyz[X], veh_x_vec );
   pfScaleVec3 ( veh_y_vec, pdata->look.xyz[Y], veh_y_vec );
   pfScaleVec3 ( veh_z_vec, pdata->look.xyz[Z], veh_z_vec );

   if ( change ) {

      pfAddVec3 ( pdata->posture.hpr, pdata->posture.hpr, pdata->look.hpr );
      pfAddVec3 ( pdata->posture.xyz, pdata->posture.xyz, veh_x_vec );
      pfAddVec3 ( pdata->posture.xyz, pdata->posture.xyz, veh_y_vec );
      pfAddVec3 ( pdata->posture.xyz, pdata->posture.xyz, veh_z_vec );
      bzero ( &(pdata->look), sizeof(pfCoord) );
      pfCopyVec3 ( view.xyz, pdata->posture.xyz );
      pfAddVec3 ( view.xyz, view.xyz, view_offsets );
      pfCopyVec3 ( view.hpr, pdata->posture.hpr );
      change = FALSE;
   }
   else {

      pfCopyVec3 ( view.xyz, pdata->posture.xyz );

      if (camera || (veh->getCat() == PERSON)) {
         pfAddVec3 ( view.xyz, view.xyz, veh_x_vec );
         pfAddVec3 ( view.xyz, view.xyz, veh_y_vec );
         pfAddVec3 ( view.xyz, view.xyz, veh_z_vec );
         pfAddVec3 ( view.hpr, pdata->posture.hpr, pdata->look.hpr);
      }
      else {
         pfCopyVec3 ( view.hpr, pdata->posture.hpr );
      }
      pfAddVec3 ( view.xyz, view.xyz, view_offsets );

   }
/*
#ifndef NONPSSHIP
   // Attempt to make head stay steady when sailor mounted on a ship
   if (veh->getCat() == PERSON) {
      float speed;
      pfCoord tmp_view;
      pfCopyVec3(tmp_view.xyz, pdata->
      veh->attach();
      
   }
#endif
*/
   if ( G_dynamic_data->birdseye && veh ) {
      float speed;
      veh->attach((pfCoord )G_dynamic_data->birdsview,
                            speed,
                     (float)G_dynamic_data->birdsdis );
      view = G_dynamic_data->birdsview;
   }
}


void compute_hpr ( pfVec3 &from_pos, pfVec3 &to_pos, pfVec3 &hpr_result )
   {
   float bearing_angle;
   float pitch_angle;
   float ordered_heading;
   float target_dist;

   target_dist = pfDistancePt3 ( to_pos, from_pos );
   bearing_angle = atan2f( (from_pos[Y] - to_pos[Y]),
                           (from_pos[X] - to_pos[X]) );
   pitch_angle = RAD_TO_DEG * asinf( (to_pos[Z] - from_pos[Z])/target_dist );
   ordered_heading = RAD_TO_DEG * (HALFPI + bearing_angle);
   while ( ordered_heading < 0.0f )
      ordered_heading += 360.0f;
   while ( ordered_heading >= 360.0f )
      ordered_heading -= 360.0f;
   pfSetVec3 ( hpr_result, ordered_heading, pitch_angle, 0.0f );
   }

void compute_hpr_to_veh ( pfVec3 &from_pos, int veh, pfVec3 &hpr_result )
   {
   float bearing_angle;
   float pitch_angle;
   float ordered_heading;
   float target_dist;
   float *to_pos;

   pfCopyVec3 (hpr_result, ZeroVec);

   if ( G_vehlist[veh].vehptr == NULL )
      {
      if ( G_dynamic_data->camlock )
         G_dynamic_data->camlock = FALSE;
      }
   else
      {
      to_pos = G_vehlist[veh].vehptr->getposition();
      target_dist = pfDistancePt3 ( to_pos, from_pos );
      if (!IS_ZERO (target_dist)) {
         bearing_angle = atan2f( (from_pos[Y] - to_pos[Y]),
                                 (from_pos[X] - to_pos[X]) );
         pitch_angle = RAD_TO_DEG * asinf( (to_pos[Z] - from_pos[Z])/target_dist );
         ordered_heading = RAD_TO_DEG * (HALFPI + bearing_angle);
         while ( ordered_heading < 0.0f )
            ordered_heading += 360.0f;
         while ( ordered_heading >= 360.0f )
            ordered_heading -= 360.0f;
         pfSetVec3 ( hpr_result, ordered_heading, pitch_angle, 0.0f );
         }
      }
   }


int compute_fixed_point ( pfVec3 &viewpos, pfVec3 &viewvec, pfVec3 &the_point )
   {
   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfSegSet   seg;
   pfHit      **hits[32];
   pfVec3     pnt;
   long       flags;
   pfMatrix    xform;

   pfMakeEulerMat ( orient, viewvec[HEADING],
                            viewvec[PITCH],
                            viewvec[ROLL] );
   pfFullXformPt3 ( direction, direction, orient );
   pfCopyVec3 ( seg.segs[0].dir, direction );
   pfNormalizeVec3 ( seg.segs[0].dir );
   seg.segs[0].length = 20000.0f;
   pfCopyVec3 ( seg.segs[0].pos, viewpos );
   if(seg.segs[0].dir[Y] == 0.0f) seg.segs[0].dir[Y] = 0.01f;
   if(seg.segs[0].dir[X] == 0.0f) seg.segs[0].dir[X] = 0.01f;
   seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   seg.userData = NULL;
   seg.activeMask = 0x01;
   seg.isectMask = PERMANENT_MASK|DESTRUCT_MASK;
   seg.bound = NULL;
   seg.discFunc = NULL;
   
   if ( pfNodeIsectSegs(G_fixed, &seg, hits) )
      {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      if ( flags & PFHIT_POINT )
         {
         pfQueryHit (*hits[0], PFQHIT_POINT, &pnt);
         if ( flags & PFHIT_XFORM )
            pfFullXformPt3(the_point,pnt,xform);
         else
            pfCopyVec3 (the_point, pnt );
         return TRUE;
         }
      else
         return FALSE;
      }
   else
      return FALSE;

   }
