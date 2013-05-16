// File: <dirt_intersect.cc>

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
#include <pf.h>
#include <pfutil.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <ieeefp.h>

#include "terrain.h"
#include "common_defs2.h"
#include "fltcallbacks.h"
#include "fileio.h"
#include "munitions.h"
#include "bbox.h"

#include "database_const.h"
#include "collision_const.h"
#include "environment_const.h"

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          pfGroup      *G_fixed;
extern          VEHTYPE_LIST *G_vehtype;
extern          OBJTYPE_LIST *G_objtype;


void set_intersect_mask(pfGroup *treeroot, long mask, long mode)
//set the masks for the intesection testing
{
    long imode;

#ifdef DEBUG
     cerr << "Beginning collision setup mask " << mask << "\n";
#endif

    if (mode == COLLIDE_DYNAMIC)
        imode = PFTRAV_SELF|PFTRAV_DESCEND|PFTRAV_IS_UNCACHE;
    else
        imode = PFTRAV_SELF|PFTRAV_DESCEND|PFTRAV_IS_CACHE;

    pfNodeTravMask(treeroot, PFTRAV_ISECT, mask, imode, PF_SET);

#ifdef DEBUG
     cerr << "End collision setup mask " << mask << "\n";
#endif

}

// from SRC code
int
grnd_normal ( pfCoord &posture, pfVec3 point, pfVec3 normal )
  {
   long        isect;
   pfSegSet    segment;
   pfHit       **hits[100];
   pfVec3      pnt, norm;


  /* make a ray looking "down" at terrain */
   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   PFCOPY_VEC3(segment.segs[0].pos, posture.xyz);
   segment.segs[0].pos[Z] += 5000.0f;
   PFSET_VEC3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   pfNormalizeVec3(segment.segs[0].dir);
   segment.segs[0].length = 10000.0f;
   segment.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = ((PERMANENT_TUNNEL|PERMANENT_DIRT|PERMANENT_BRIDGE)<<PERMANENT_SHIFT);

   isect = pfNodeIsectSegs(G_fixed, &segment, hits);
   if(isect)
     {
     long flags;
     
     pfQueryHit(*hits[0],PFQHIT_POINT, &pnt);
     pfCopyVec3(point,pnt);
     pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags );
     pfQueryHit(*hits[0], PFQHIT_NORM, &norm);
     pfCopyVec3(normal, norm);
     pfNormalizeVec3 ( normal );
     if ( ! (flags & PFHIT_NORM) )
       pfSetVec3 ( normal, 0.0f, 0.0f, 2.0f );
     
     return TRUE;
   }
   else 
     {
     cerr << "!WARNING!- No intersection detected with ground at " << posture.xyz[X] << ", " << posture.xyz[Y] << ", " << posture.xyz[Z] << endl;
     return FALSE;
     }
  }



float gnd_level(pfVec3 &pos)
// This function is used to get aircraft altitude, it shoots the segment
// from the vehicle's actual location. This allows flying through the
// under-world tunnels
{
   long        isect;
   pfSegSet    segment;
   pfHit       **hits[100];
   pfNode      *node;
   pfVec3      pnt;
   long        the_mask;

   /* make a ray looking "down" at terrain */
   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   PFCOPY_VEC3(segment.segs[0].pos, pos);
   segment.segs[0].pos[Z] += 2.5f;
   PFSET_VEC3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   pfNormalizeVec3(segment.segs[0].dir);
   segment.segs[0].length = 20000.0f;
   segment.mode = PFTRAV_IS_PRIM;
   segment.activeMask = 0x01;
   segment.isectMask = PERMANENT_MASK;
   isect = pfNodeIsectSegs(G_fixed, &segment, hits);

   if (isect) 
      {
      pfQueryHit(*hits[0], PFQHIT_NODE, &node);
      the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
      if ( int((the_mask & PERMANENT_MASK) >> PERMANENT_SHIFT)
          == PERMANENT_TUNNEL)
         return (-350.0f);
      else
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
//         pfQueryHit(*hits[0], PFQHIT_XFORM, &mat);
//         pfFullXformPt3(pnt,pnt,mat);
//cerr << "Altitude: " << pnt[PF_Z] << "\n";     
         return(pnt[PF_Z]);
      }
   //NO intersection, we had a boo boo or we are really flying
   return (0.0f);
}


float gnd_level2(pfVec3 pos)
// This function is used to place static models on the terrain. It is the
// same as gnd_level except that the segment is started at the model's Z
// location plus 4000 meters
{
   long        isect;
   pfSegSet    segment;
   pfHit       **hits[32];
   pfVec3      pnt;
   long        flags;

   /* make a ray looking "down" at terrain */

//cerr << "In gnd_level2 - segment pos = " << segment.pos[X] << " " 
//     << segment.pos[Y] << " " << segment.pos[Z] << endl;
//cerr << "              - segment orientation = " << segment.dir[X] << " "
//     << segment.dir[Y] << " " << segment.dir[Z] << endl;
//cerr << "              - segment length = " << segment.length << endl;

   /* find intersection with terrain */
   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   pfCopyVec3(segment.segs[0].pos, pos);
   segment.segs[0].pos[Z] = 4000.0f;
   pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   pfNormalizeVec3(segment.segs[0].dir);
   segment.segs[0].length = 14000.0f;
   segment.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = ((PERMANENT_DIRT|PERMANENT_WATER|PERMANENT_BRIDGE)
                        <<PERMANENT_SHIFT);
   isect = pfNodeIsectSegs(G_fixed, &segment, hits); 
   pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
   if (isect && (flags & PFHIT_POINT))
      {
      pfQueryHit(*hits[0],PFQHIT_POINT, &pnt);
      return(pnt[PF_Z]);
      }
   else
      {
/*
      cerr << " *** !WARNING!- No intersection detected with ground "
           << "placing static model.\n";
      cerr << "Coordinates "<< pos[X] <<" " << pos[Y] << " " << pos[Z] <<"\n";
      cerr << "Coordinates "<< segment.segs[0].pos[X] <<" " << segment.segs[0].pos[Y] << " " << segment.segs[0].pos[Z] <<"\n";
*/
   //No intersection, we had a boo boo or we are really flying
      }
   return (0.0f);
}


float get_altitude (pfVec3 pos)
{
   long        isect;
   pfSegSet    segment;
   pfHit       **hits[32];
   pfVec3      pnt; 
   float myaltitude;

   /* make a ray looking "down" at terrain */

   /* find intersection with terrain */
   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   segment.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = PERMANENT_MASK;
   pfCopyVec3(segment.segs[0].pos, pos);
   segment.segs[0].pos[Z] += 0.1f;
   pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   segment.segs[0].length = fabs(segment.segs[0].pos[Z]) + MAX_GROUND_HEIGHT + 100.0f;
   
   isect = pfNodeIsectSegs(G_fixed, &segment, hits);

   if (isect)                                                                                 
      {
      pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
      myaltitude = pos[Z] - pnt[Z];
      }
   else
      {
      segment.userData = NULL;
      segment.bound = NULL;
      segment.discFunc = NULL;
      segment.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
      segment.activeMask = 0x01;
      segment.isectMask = PERMANENT_MASK;
      pfCopyVec3(segment.segs[0].pos, pos);
      segment.segs[0].pos[Z] -= 0.1f;
      pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, 1.0f);
      segment.segs[0].length = fabs(segment.segs[0].pos[Z]) + MAX_GROUND_HEIGHT + 10000.0f;
   
      isect = pfNodeIsectSegs(G_fixed, &segment, hits);

      if ( isect )
         {
         pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
         myaltitude = pos[Z] - pnt[Z];
         }
      else
         myaltitude = 0.0f;
      }
   return myaltitude;
}

int grnd_orient(pfCoord &posture, pfBox *bounding_box)
// This function set the Z location value and the pitch and roll values
// once the X-Y position and heading are set.
{
   pfSegSet segment;
   pfHit    **hits[32];
   pfVec3   normal;
   pfVec3   pnt;
   long     flags;

   /* make a ray looking "down" at terrain */

   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   PFCOPY_VEC3(segment.segs[0].pos, posture.xyz);
   segment.segs[0].pos[Z] += 5.0f;
   PFSET_VEC3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   pfNormalizeVec3(segment.segs[0].dir);
   segment.segs[0].length = 400.0f;
   segment.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = ((PERMANENT_WATER|PERMANENT_TUNNEL|PERMANENT_DIRT|PERMANENT_BRIDGE)<<PERMANENT_SHIFT);
  
   /* find intersection with terrain */
  
   if ( pfNodeIsectSegs(G_fixed, &segment, hits) )
      {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
      pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
      pfNormalizeVec3( normal );
      posture.xyz[Z] = pnt[PF_Z];
      if ( flags & PFHIT_NORM )
         {
         if ( bounding_box != NULL )
            {
            posture.xyz[Z] -= bounding_box->min[PF_Z];
            }
         pfVec3 head, head90;
         float sh, ch;
         float dotp;
         pfSinCos(posture.hpr[HEADING], &sh, &ch);
         head[0] = -sh;
         head[1] =  ch;
         head[2] = 0.0f;
         dotp = PFDOT_VEC3(head, normal);
         posture.hpr[PITCH] = pfArcCos(dotp) - 90.0f;
         head90[0] =  ch;
         head90[1] =  sh;
         head90[2] = 0.0f;
         dotp = PFDOT_VEC3(head90, normal);
         posture.hpr[ROLL]  = 90.0f - pfArcCos(dotp);
#ifdef DEBUG
         cerr << "GRND_ORIENT POSTURE X = " << posture.xyz[X] << "  Y = " 
              << posture.xyz[Y] << "  Z = " << posture.xyz[Z] << endl; 
         cerr << "                    H = " << posture.hpr[X] << "  P = "
              << posture.hpr[Y] << "  R = " << posture.hpr[Z] << endl;
#endif
         return TRUE;   
         }
      else
         return FALSE;
      }
   else
      return FALSE;     
}

int grnd_orient2(pfCoord &posture)
// This function set the Z location value and the pitch and roll values
// once the X-Y position and heading are set.
{
   pfSegSet segment;
   pfHit    **hits[32];
   pfVec3   normal;
   pfVec3   pnt;
   long     flags;

   /* make a ray looking "down" at terrain */
   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   PFCOPY_VEC3(segment.segs[0].pos, posture.xyz);
   segment.segs[0].pos[Z] += 10000.0f;
   PFSET_VEC3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   pfNormalizeVec3(segment.segs[0].dir);
   segment.segs[0].length = 20000.0f;
   segment.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = ((PERMANENT_WATER|PERMANENT_TUNNEL|PERMANENT_DIRT|PERMANENT_BRIDGE)<<PERMANENT_SHIFT);
   
   if ( pfNodeIsectSegs(G_fixed, &segment, hits) )
      {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
      pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
      pfNormalizeVec3 ( normal );
      posture.xyz[Z] = pnt[PF_Z];
      if (flags & PFHIT_NORM)
         {
         pfVec3 head, head90;
         float sh, ch;
         float dotp;

         pfSinCos(posture.hpr[HEADING], &sh, &ch);
         head[0] = -sh;
         head[1] =  ch;
         head[2] = 0.0f;
         dotp = PFDOT_VEC3(head, normal);
         posture.hpr[PITCH] = pfArcCos(dotp) - 90.0f;
         head90[0] =  ch;
         head90[1] =  sh;
         head90[2] = 0.0f;
         dotp = PFDOT_VEC3(head90, normal);
         posture.hpr[ROLL]  = 90.0f - pfArcCos(dotp);
#ifdef DEBUG
         cerr << "GRND_ORIENT POSTURE X = " << posture.xyz[X] << "  Y = "
              << posture.xyz[Y] << "  Z = " << posture.xyz[Z] << endl;
         cerr << "                    H = " << posture.hpr[X] << "  P = "
              << posture.hpr[Y] << "  R = " << posture.hpr[Z] << endl;
#endif
         return TRUE;
         }
      else
         return FALSE;
      }
   else
      return FALSE;
}


int water_orient(pfCoord &posture)
// This function set the Z location value and the pitch and roll values
// once the X-Y position and heading are set.
{
   pfSegSet segment;
   pfHit    **hits[32];
   pfVec3   normal;
   pfVec3   pnt;
   long     flags;
   /* make a ray looking "down" at terrain */
   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   PFCOPY_VEC3(segment.segs[0].pos, posture.xyz);
   segment.segs[0].pos[Z] += 500.0f;
   PFSET_VEC3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   pfNormalizeVec3(segment.segs[0].dir);
   segment.segs[0].length = 50000.0f;
   segment.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = (PERMANENT_WATER<<PERMANENT_SHIFT);

   if ( pfNodeIsectSegs(G_fixed, &segment, hits) )
      {
      pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
      posture.xyz[Z] = pnt[PF_Z];
      if (flags & PFHIT_NORM)
         {
         pfVec3 head, head90;
         float sh, ch;
         float dotp;

         pfSinCos(posture.hpr[HEADING], &sh, &ch);
         head[0] = -sh;
         head[1] =  ch;
         head[2] = 0.0f;
         dotp = PFDOT_VEC3(head, normal);
         posture.hpr[PITCH] = pfArcCos(dotp) - 90.0f;
         head90[0] =  ch;
         head90[1] =  sh;
         head90[2] = 0.0f;
         dotp = PFDOT_VEC3(head90, normal);
         posture.hpr[ROLL]  = 90.0f - pfArcCos(dotp);
#ifdef DEBUG
         cerr << "WATER_ORIENT POSTURE X = " << posture.xyz[X] << "  Y = " 
              << posture.xyz[Y] << "  Z = " << posture.xyz[Z] << endl; 
         cerr << "                    H = " << posture.hpr[X] << "  P = "
              << posture.hpr[Y] << "  R = " << posture.hpr[Z] << endl;
#endif
         
      }
      return TRUE;
      }
   else
      return FALSE;     
}


int tunnel_orient(pfCoord &posture)
// This function set the location values and the pitch and roll values
// once the X-Y position and heading are set.
{
   pfSegSet segment;
   pfHit    **hits[32];
   pfVec3   normal;
   pfVec3   pnt;
   long     flags;

   /* make a ray looking in the direction of travel*/
   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   PFCOPY_VEC3(segment.segs[0].pos, posture.xyz);
   PFSET_VEC3(segment.segs[0].dir, 0.0f, 1.0f, 0.0f);
   segment.segs[0].length = 30.0f;
   segment.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = (PERMANENT_TUNNEL<<PERMANENT_SHIFT);

   if ( pfNodeIsectSegs(G_fixed, &segment, hits) )
      {
      pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
      posture.xyz[Z] = pnt[PF_Z];
      if (flags & PFHIT_NORM)
         {
         pfVec3 head, head90;
         float sh, ch;
         float dotp;

         pfSinCos(posture.hpr[HEADING], &sh, &ch);
         head[0] = -sh;
         head[1] =  ch;
         head[2] = 0.0f;
         dotp = PFDOT_VEC3(head, normal);
         posture.hpr[PITCH] = pfArcCos(dotp) - 90.0f;
         head90[0] =  ch;
         head90[1] =  sh;
         head90[2] = 0.0f;
         dotp = PFDOT_VEC3(head90, normal);
         posture.hpr[ROLL]  = 90.0f - pfArcCos(dotp);
#ifdef DEBUG
         cerr << "TUNNEL_ORIENT POSTURE X = " << posture.xyz[X] << "  Y = " 
              << posture.xyz[Y] << "  Z = " << posture.xyz[Z] << endl; 
         cerr << "                    H = " << posture.hpr[X] << "  P = "
              << posture.hpr[Y] << "  R = " << posture.hpr[Z] << endl;
#endif
         
         }
      return TRUE;
      }
   else
      return FALSE;     
}

int isNanVec3 (pfVec3 the_vec)
{
   int isNanInVec = TRUE;

   if ( !(isnanf (the_vec[X])
       || isnanf (the_vec[Y])
       || isnanf (the_vec[Z]) ) ) {
       isNanInVec = FALSE;
   } 

   return isNanInVec;
}

int get_isect_terrain (float &iPnt, pfVec3 &c_norm, pfVec3 xyz)
{

   static pfVec3 lastValidNrm = {0.0f, 0.0f, 1.0f};

   ulong         t_isect;
   ulong         t_flags;
   pfSegSet      segment;
   pfHit       **hits[32];
   pfVec3        pnt;
   int           success = FALSE;


   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   segment.mode = PFTRAV_IS_PRIM | PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = PERMANENT_MASK;
   pfCopyVec3(segment.segs[0].pos, xyz);
   segment.segs[0].pos[Z] += 2.0f;
   pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   segment.segs[0].length =
            fabs(xyz[Z]) + MAX_GROUND_HEIGHT + 100.0f;
 
   t_isect = pfNodeIsectSegs(G_fixed, &segment, hits);

   if (t_isect) {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &t_flags);
      if (t_flags & PFHIT_NORM) {
         pfQueryHit(*hits[0], PFQHIT_NORM, c_norm);
         pfCopyVec3(lastValidNrm, c_norm);
      }
      else {
         pfCopyVec3(c_norm, lastValidNrm);
         cerr << "Using last valid normal." << endl;
      }
      pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
      iPnt = pnt[Z];
      success = TRUE;
   }
   else {
      pfCopyVec3(segment.segs[0].pos, xyz);
      segment.segs[0].pos[Z] -= 0.1f;
      pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, 1.0f);
      segment.segs[0].length = fabs(segment.segs[0].pos[Z])
                              + MAX_GROUND_HEIGHT + 10000.0f;
 
      t_isect = pfNodeIsectSegs(G_fixed, &segment, hits);

      if ( t_isect ) {
         pfQueryHit(*hits[0], PFQHIT_FLAGS, &t_flags);
         if (t_flags & PFHIT_NORM) {
            pfQueryHit(*hits[0], PFQHIT_NORM, c_norm);
            pfCopyVec3(lastValidNrm, c_norm);
         }
         else {
            pfCopyVec3(c_norm, lastValidNrm);
            cerr << "Using last valid normal." << endl;
         }
         pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
         iPnt = pnt[Z];
         success = TRUE;
      }
      else {
         pfCopyVec3(c_norm, lastValidNrm);
         iPnt = 0.0f;
      }
   }
   return success;
}

int get_isect_fixed (float &iPnt, pfVec3 &c_norm, pfVec3 xyz)
{

   static pfVec3 lastValidNrm = {0.0f, 0.0f, 1.0f};

   ulong         t_isect;
   ulong         t_flags;
   pfSegSet      segment;
   pfHit       **hits[32];
   pfVec3        pnt;
   int           success = FALSE;


   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   segment.mode = PFTRAV_IS_PRIM | PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = DESTRUCT_MASK;
   pfCopyVec3(segment.segs[0].pos, xyz);
   segment.segs[0].pos[Z] += 2.0f;
   pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   segment.segs[0].length =
            fabs(xyz[Z]) + MAX_GROUND_HEIGHT + 100.0f;

   t_isect = pfNodeIsectSegs(G_fixed, &segment, hits);

   if (t_isect) {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &t_flags);
      if (t_flags & PFHIT_NORM) {
         pfQueryHit(*hits[0], PFQHIT_NORM, c_norm);
         pfCopyVec3(lastValidNrm, c_norm);
      }
      else {
         pfCopyVec3(c_norm, lastValidNrm);
         cerr << "Using last valid normal." << endl;
      }
      pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
      iPnt = pnt[Z];
      success = TRUE;
   }
   else {
      pfCopyVec3(segment.segs[0].pos, xyz);
      segment.segs[0].pos[Z] -= 0.1f;
      pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, 1.0f);
      segment.segs[0].length =
            fabs(segment.segs[0].pos[Z]) + MAX_GROUND_HEIGHT + 10000.0f;

      t_isect = pfNodeIsectSegs(G_fixed, &segment, hits);

      if ( t_isect ) {
         pfQueryHit(*hits[0], PFQHIT_FLAGS, &t_flags);
         if (t_flags & PFHIT_NORM) {
            pfQueryHit(*hits[0], PFQHIT_NORM, c_norm);
            pfCopyVec3(lastValidNrm, c_norm);
         }
         else {
            pfCopyVec3(c_norm, lastValidNrm);
            cerr << "Using last valid normal." << endl;
         }
         pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
         iPnt = pnt[Z];
         success = TRUE;
      }
      else {
         pfCopyVec3(c_norm, lastValidNrm);
         iPnt = 0.0f;
      }
   }
   return success;
}


int lase_isect (pfCoord pos, pfVec3 &return_pnt)
{
   int isect;
   pfSegSet      segment;
   pfHit         **hits[32];
   ulong         t_flags;
   int success = FALSE;
   pfMatrix Orient;
   pfVec3 fb_vec = { 0.0f, 1.0f, 0.0f };

   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   segment.mode = PFTRAV_IS_PRIM;
   segment.activeMask = 0x01;
   segment.isectMask = ALL_MASK;
   pfCopyVec3(segment.segs[0].pos, pos.xyz);
   segment.segs[0].length = 5000.0f;

   pfMakeEulerMat ( Orient, pos.hpr[PF_H], pos.hpr[PF_P], pos.hpr[PF_R] );
   pfFullXformPt3 ( fb_vec, fb_vec, Orient );
   pfNormalizeVec3 ( fb_vec );
   pfCopyVec3(segment.segs[0].dir, fb_vec );


   isect = pfNodeIsectSegs(G_dynamic_data->pipe[0].channel[0].scene,
                           &segment, hits);

   if (isect) {
      pfMatrix xmat;
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &t_flags);
      if (t_flags & PFHIT_POINT) {
         pfQueryHit(*hits[0], PFQHIT_POINT, &return_pnt);
         success = TRUE;
         if (t_flags & PFHIT_XFORM) {
            pfQueryHit(*hits[0], PFQHIT_XFORM, xmat);
            pfFullXformPt3(return_pnt,return_pnt,xmat);
         }
      }
   }

   return success;
}




void person_altitude (float &myalt, pfCoord &posture, pfVec3 &c_norm)
{
   static pfVec3 lastValidTNrm = {0.0f, 0.0f, 1.0f};
   static pfVec3 lastValidFNrm = {0.0f, 0.0f, 1.0f};
   long          t_isect, f_isect;
   pfSegSet      segment;
   pfHit         **hits[32];
   pfVec3        pnt;
   pfVec3        t_nrm, f_nrm;
   float         t_alt, f_alt;
   long          t_flags, f_flags;

   posture.hpr[PITCH] = posture.hpr[ROLL] = 0.0f;

   /* make a ray looking "down" at terrain */

   /* find intersection with terrain */
   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   segment.mode = PFTRAV_IS_PRIM | PFTRAV_IS_NORM;
   segment.activeMask = 0x01;
   segment.isectMask = PERMANENT_MASK;
   pfCopyVec3(segment.segs[0].pos, posture.xyz);
   segment.segs[0].pos[Z] += 5.0f;
   pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   segment.segs[0].length = 
            fabs(segment.segs[0].pos[Z]) + MAX_GROUND_HEIGHT + 100.0f;
 
   t_isect = pfNodeIsectSegs(G_fixed, &segment, hits);

   if (t_isect) {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &t_flags);
      if (t_flags & PFHIT_NORM) {
         pfQueryHit(*hits[0], PFQHIT_NORM, t_nrm);
         pfCopyVec3(lastValidTNrm, t_nrm);
      }
      else {
         pfCopyVec3(t_nrm, lastValidTNrm);
         cerr << "Using last valid normal." << endl;
      }
      pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
      t_alt = pnt[Z];
   }
   else {
      pfCopyVec3(segment.segs[0].pos, posture.xyz);
      segment.segs[0].pos[Z] -= 0.1f;
      pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, 1.0f);
      segment.segs[0].length = 
            fabs(segment.segs[0].pos[Z]) + MAX_GROUND_HEIGHT + 10000.0f;
 
      t_isect = pfNodeIsectSegs(G_fixed, &segment, hits);

      if ( t_isect ) {
         pfQueryHit(*hits[0], PFQHIT_FLAGS, &t_flags);
         if (t_flags & PFHIT_NORM) {
            pfQueryHit(*hits[0], PFQHIT_NORM, t_nrm);
            pfCopyVec3(lastValidTNrm, t_nrm);
         }
         else {
            pfCopyVec3(t_nrm, lastValidTNrm);
            cerr << "Using last valid normal." << endl;
         }
         pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
         t_alt = pnt[Z];
      }
   }
   segment.activeMask = 0x01;
   segment.isectMask = DESTRUCT_MASK;
   pfCopyVec3(segment.segs[0].pos, posture.xyz);
   segment.segs[0].pos[Z] += 2.0;
   pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   segment.segs[0].length = fabs(segment.segs[0].pos[Z]) + MAX_GROUND_HEIGHT + 100.0f;

   f_isect = pfNodeIsectSegs(G_fixed, &segment, hits);

   if (f_isect) {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &f_flags);
      if (f_flags & PFHIT_NORM) {
         pfQueryHit(*hits[0], PFQHIT_NORM, f_nrm);
         pfCopyVec3(lastValidFNrm, f_nrm);
      }
      else {
         pfCopyVec3(f_nrm, lastValidFNrm);
         cerr << "Using last valid normal." << endl;
      }
      pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
      f_alt = pnt[Z];
   }
   else {
      pfCopyVec3(segment.segs[0].pos, posture.xyz);
      segment.segs[0].pos[Z] -= 2.0f;
      pfSetVec3(segment.segs[0].dir, 0.0f, 0.0f, 1.0f);
      segment.segs[0].length =
                fabs(segment.segs[0].pos[Z]) + MAX_GROUND_HEIGHT + 10000.0f;

      f_isect = pfNodeIsectSegs(G_fixed, &segment, hits);

      if ( f_isect ) {
         pfQueryHit(*hits[0], PFQHIT_FLAGS, &f_flags);
         if (f_flags & PFHIT_NORM) {
            pfQueryHit(*hits[0], PFQHIT_NORM, f_nrm);
            pfCopyVec3(lastValidFNrm, f_nrm);
         }
         else {
            pfCopyVec3(f_nrm, lastValidFNrm);
            cerr << "Using last valid normal." << endl;
         }
         pfQueryHit(*hits[0], PFQHIT_POINT, pnt);
         f_alt = pnt[Z];
      }
   }

   if ( f_isect && t_isect ) {
      myalt = posture.xyz[Z] - f_alt;
      posture.xyz[Z] = f_alt;
      pfCopyVec3 (c_norm, f_nrm);
   }
   else if ( f_isect ) {
      myalt = posture.xyz[Z] - f_alt;
      posture.xyz[Z] = f_alt;
      pfCopyVec3 (c_norm, f_nrm);
   }
   else if ( t_isect ) {
      myalt = posture.xyz[Z] - t_alt;
      posture.xyz[Z] = t_alt;
      pfCopyVec3 (c_norm, t_nrm);
   }
   else {  // should never reach this
      cerr << "Error in person_altitude.  No intersection detected." 
           << endl;
      posture.xyz[Z] = 3.5f;
      myalt = 0.0f;
      c_norm[X] = c_norm[Y] = 0.0f;
      c_norm[Z] = 1.0f;
   }
}

int person_orient(pfCoord &posture)
// This function set the Z location value and the pitch and roll values
// once the X-Y position and heading are set.
{
   float    tmp_pos_ter, tmp_pos_fix;
   long     isect_ter, isect_fix;
   pfSegSet segment;
   pfHit    **hits[32];
   pfVec3   pnt;

   /* make a ray looking "down" at terrain */
   segment.userData = NULL;
   segment.bound = NULL;
   segment.discFunc = NULL;
   PFCOPY_VEC3(segment.segs[0].pos, posture.xyz);
   segment.segs[0].pos[Z] += 5.0f;
   PFSET_VEC3(segment.segs[0].dir, 0.0f, 0.0f, -1.0f);
   pfNormalizeVec3(segment.segs[0].dir);
   segment.segs[0].length = 400.0f;
   segment.mode = PFTRAV_IS_PRIM;
   segment.activeMask = 0x01;
   segment.isectMask = ((PERMANENT_TUNNEL|PERMANENT_DIRT|PERMANENT_BRIDGE)<<PERMANENT_SHIFT);
   posture.hpr[PITCH] = posture.hpr[ROLL] = 0.0f;

   /* find intersection with terrain */

   isect_ter = pfNodeIsectSegs(G_fixed, &segment, hits);

   if ( isect_ter )
      {
      pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);

      tmp_pos_ter = pnt[PF_Z];

      }
   segment.segs[0].pos[Z] -= 3.0f;
   segment.activeMask = 0x03;
   segment.isectMask = DESTRUCT_MASK;
   isect_fix = pfNodeIsectSegs(G_fixed, &segment, hits);

   if ( isect_fix )
      {
      pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);

      tmp_pos_fix = pnt[PF_Z];

      if ((tmp_pos_fix >= tmp_pos_ter) &&
         (((tmp_pos_fix - posture.xyz[Z]) <= 1.0f) ||
         ((tmp_pos_fix - posture.xyz[Z])  >= -1.0f)))
         {
    posture.xyz[Z] = tmp_pos_fix;
         return TRUE;
         }
      }
    if ( isect_ter )
      {
      posture.xyz[Z] = tmp_pos_ter;
      return TRUE;
      }

   return FALSE;
}



