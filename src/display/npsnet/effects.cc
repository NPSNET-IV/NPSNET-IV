// File: <effects.cc>

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

#include <fstream.h>
#include <iostream.h>
#include <pf.h>
#include <pfdu.h>
#include <pfdb/pfflt.h>

#include "effects.h"

#include "common_defs2.h"
#include "macros.h"
#include "animations.h"
#include "fltcallbacks.h"
#include "terrain.h"
#include "collision_const.h"

#include "imstructs.h"
#include "imclass.h"
#include "imextern.h"

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          pfGroup      *G_animations;
extern          pfGroup      *G_fixed;

extern void exitroutine();

#define MAX_FIREBALL 10 
#define MAX_CRATER 10
#define CRATER_LIFE 30.0f
#define FIREBALL_LIFE 0.075f

//local globals
static ANIMNODETYPE L_fireball[MAX_FIREBALL];
static ANIMNODETYPE L_crater[MAX_CRATER];
static LIGHTREC L_blinking_lights[MAX_BLINKING_LIGHTS];
static ANIMATION_LIST animate[MAX_ANIMATIONS];

extern InitManager *initman;

void init_animations ()
{
   int i;
   int j;

   for ( i = 0; i < MAX_ANIMATIONS; i++ )
      {
      animate[i].numloaded = 0;
      animate[i].next = 0;
      for ( j = 0; j < MAX_ANIM_COPIES; j++ )
         {
         animate[i].animation[j].dcs = NULL;
         animate[i].animation[j].sequence = NULL;
         animate[i].animation[j].active = FALSE;
         }
      }

}


void load_animation()
{
   struct Anim im_animations[MAX_ANIMATIONS];
   int	index;
   int	count;
   long	mode;
   long	reps;
   int	num_effects;
   struct Eff *eptr;
   int	temp_count;

   initman->get_animations((struct Anim**) im_animations);

   for (int i = 0; i < MAX_ANIMATIONS; i++) {
      if (im_animations[i].num_effects == 0) {
         continue;
      }
      index = i;
      count = im_animations[i].count;
      mode = (long) im_animations[i].mode;
      reps = (long) im_animations[i].reps;
      num_effects = im_animations[i].num_effects;
      eptr = (struct Eff *) im_animations[i].Effect;
      animate[index].animation[0].sequence =
                   load_sequence(mode, reps, temp_count, num_effects, eptr);
      if (animate[index].animation[0].sequence == NULL) {
         animate[index].numloaded = 0;
         cerr << "ERROR:\tCould not load animation, index = " << index << endl;
      } else {
         animate[index].numloaded = 1;
         animate[index].animation[0].dcs = pfNewDCS();
         pfAddChild (animate[index].animation[0].dcs,
                     animate[index].animation[0].sequence);
         animate[index].numloaded = 1;
         for (int j = 1; j < count; j++) {
            animate[index].animation[j].sequence =
               (pfSequence *)pfClone(animate[index].animation[0].sequence,0); 
            if ( animate[index].animation[j].sequence == NULL ) {
               cerr << "ERROR:\tCould not clone animation: index = "
                    << index << ", copy = " << j << endl;
            } else {
               pfSeqInterval(animate[index].animation[j].sequence,
                             mode,0,long(temp_count-1));
               pfSeqDuration(animate[index].animation[j].sequence,
                             1.0f,reps);
               pfSeqMode(animate[index].animation[j].sequence,
                         PFSEQ_STOP);
               animate[index].numloaded++;
               animate[index].animation[j].dcs = pfNewDCS();
               pfAddChild (animate[index].animation[j].dcs, 
                           animate[index].animation[j].sequence);
            }
         }
      }
   } // end for
}


/* THE PRE-INIT MANAGER VERSION
int load_animation ( int index, int count, char *filename, 
                     long mode, long reps )
{
   int i;
   int temp_count;

   if ( (index < 0) || (index > MAX_ANIMATIONS) )
      {
      cerr << "ERROR:  Requested to load animation, out of range:  "
           << index << endl;
      return FALSE;
      }
   else if ( (count < 0) || (count > MAX_ANIM_COPIES) )
      {
      cerr << "ERROR:  Requested to copy animation, out of range:  "
           << count << endl;
      return FALSE;
      }
   else
      {
      animate[index].animation[0].sequence = load_sequence ( filename, 
                                                             mode, reps,
                                                             temp_count );
      if ( animate[index].animation[0].sequence == NULL )
         {
         animate[index].numloaded = 0;
         cerr << "ERROR:\tCould not load animation, index = " << index << endl;
         }
      else
         {
         animate[index].numloaded = 1;
         animate[index].animation[0].dcs = pfNewDCS();
         pfAddChild (animate[index].animation[0].dcs,
                     animate[index].animation[0].sequence);
         animate[index].numloaded = 1;
         for ( i = 1; i < count; i++ )
            {
            animate[index].animation[i].sequence =
               (pfSequence *)pfClone(animate[index].animation[0].sequence,0); 
            if ( animate[index].animation[i].sequence == NULL )
               cerr << "ERROR:\tCould not clone animation: index = "
                    << index << ", copy = " << i << endl;
            else
               {
               pfSeqInterval(animate[index].animation[i].sequence,
                             mode,0,long(temp_count-1));
               pfSeqDuration(animate[index].animation[i].sequence,
                             1.0f,reps);
               pfSeqMode(animate[index].animation[i].sequence,
                         PFSEQ_STOP);
               animate[index].numloaded++;
               animate[index].animation[i].dcs = pfNewDCS();
               pfAddChild (animate[index].animation[i].dcs, 
                           animate[index].animation[i].sequence);
               }
            }
         }
      if ( animate[index].numloaded > 0 )
         return TRUE;
      else 
         return FALSE;
      }
}
*/


int play_animation ( int index, pfVec3 position, pfVec3 orientation )
{
   ANIMATION *anim;

   if ( (index < 0) || (index > MAX_ANIMATIONS) )
      {
      cerr << "ERROR:  Requested to play animation, out of range:  "
           << index << endl;
      return FALSE;
      }
   else if ( animate[index].numloaded == 0 )
      return FALSE;
   else
      {
      anim = &(animate[index].animation[animate[index].next]);
      animate[index].next = (animate[index].next + 1) % 
                               animate[index].numloaded; 

      if ( anim->active )
         {
         pfSeqMode(anim->sequence, PFSEQ_STOP);
         pfRemoveChild(pfGetParent(anim->dcs,0),
                                    anim->dcs);

         }

      set_intersect_mask((pfGroup *)anim->dcs,
                         PRUNE_MASK,COLLIDE_DYNAMIC);

      pfAddChild ( G_animations, anim->dcs );

/*
      pfAddChild ( findnode(position), anim->dcs );
*/

      anim->active = TRUE;
      pfSeqMode ( anim->sequence, PFSEQ_STOP);
      pfDCSTrans ( anim->dcs, position[X], position[Y], position[Z] );
      pfDCSRot ( anim->dcs, orientation[HEADING], orientation[PITCH],
                            orientation[ROLL] );
      pfSeqMode ( anim->sequence, PFSEQ_START );
      return TRUE;
      }
   }

 
void make_fireball_array()
//make the array containing the fireballs 
{
  pfSequence *flame;
  pfGroup *fireball;
  pfDCS *scaledcs;
  int ix,jx;

  //create a sequence to represent the fire ball
  if((fireball  = (pfGroup *)pfdLoadFile("fireball2.flt")) == NULL){
    cerr<<"Unable to open the fireball file fireball2.flt\n";
    exitroutine();
  }

  for(jx=0;jx<MAX_FIREBALL;jx++){
        //it will get bigger then smaller once
    flame = pfNewSeq();
    for (ix= 0;ix<10;ix++){
      scaledcs = pfNewDCS();
      pfAddChild(scaledcs,fireball);
      pfDCSScale(scaledcs,2.0f*(ix+1.0f));
      pfAddChild(flame,scaledcs);
    }
    /*set up the parameters*/
    //pfSeqTime(flame,-1,0.03333333);
    pfSeqTime(flame,-1,FIREBALL_LIFE);
    pfSeqDuration(flame,1.0f,2);
    pfSeqInterval(flame,PFSEQ_SWING,0,9);

    //now attach them to the array of fireballs
    L_fireball[jx].dcs = pfNewDCS();
    L_fireball[jx].seq = flame;
    pfAddChild(L_fireball[jx].dcs,L_fireball[jx].seq);
    L_fireball[jx].active = FALSE;
  }
}

void dump_fire()
  {
  int jx;
  for ( jx = 0 ; jx < MAX_FIREBALL ; jx++ )
     {
     if ( L_fireball[jx].active )
        {
        pfRemoveChild(G_animations, L_fireball[jx].dcs);
        }
     L_fireball[jx].active = FALSE;
     }
  }


void init_blinking_lights ()
   {
   for ( int i = 0; i < MAX_BLINKING_LIGHTS; i++ )
      {
      L_blinking_lights[i].active = FALSE;
      pfSetVec3 ( L_blinking_lights[i].color, 0.0f, 0.0f, 0.0f );
      L_blinking_lights[i].animation = NULL;
      L_blinking_lights[i].light[0] = NULL;
      L_blinking_lights[i].light[1] = NULL;
      }
   }

int read_blinking_lights ()
{
/*
   int count = 0;
   char tempstr[120];
   float lightsize;
   double blinkrate;
*/
   pfVec3 lightpos,lightcolor;
   struct BL im_lights[MAX_BLINKING_LIGHTS];
   int num_lights;

   initman->get_blinking_lights((struct BL**) im_lights, &num_lights);

   for (int i = 0; i < num_lights; i++) {
      lightpos[X] = im_lights[i].Position.x;
      lightpos[Y] = im_lights[i].Position.y;
      lightpos[Z] = im_lights[i].Position.z;
      lightcolor[X] = im_lights[i].Color.r;
      lightcolor[Y] = im_lights[i].Color.g;
      lightcolor[Z] = im_lights[i].Color.b;
      makeblinkinglight(lightpos, lightcolor, im_lights[i].size,
                        (double) im_lights[i].blink_rate);
      /*
      cout << "Pos=" << im_lights[i].Position.x << ","
                     << im_lights[i].Position.y << ","
                     << im_lights[i].Position.z << "\t";
      cout << "Col=" << im_lights[i].Color.r << ","
                     << im_lights[i].Color.g << ","
                     << im_lights[i].Color.b << "\t";
      cout << "Size=" << im_lights[i].size << "\t";
      cout << "Rate=" << im_lights[i].blink_rate << "\n";
      */
   }
   return (num_lights);

/*
   ifstream bl_file ( (char *)G_static_data->blinking_lights_file );

   if ( !bl_file )
      {
      cerr << "** Blinking lights file not found: "
           << (char *)G_static_data->blinking_lights_file << endl;
      return count;
      }
   else
      {
      count = 0;
      while ( !bl_file.eof() && (count < MAX_BLINKING_LIGHTS) )
         {
         bl_file >> "B" >> lightpos[X] >> lightpos[Y] >> lightpos[Z]
                 >> lightcolor[X] >> lightcolor[Y] >> lightcolor[Z]
                 >> lightsize >> blinkrate;
         if (!bl_file)
            {
            if (!bl_file.eof())
               {
               cerr << "** Error reading from blinking lights file: "
                    << (char *)G_static_data->blinking_lights_file << endl;
               bl_file.clear();
               bl_file.getline (tempstr, 120);
               cerr << "   Incorrect format for line: " << tempstr << endl;
               }
            }
         else
            {
            bl_file.getline ( tempstr, 120 );
            makeblinkinglight (lightpos,lightcolor,lightsize,blinkrate);
            count++;
            }
         }
      return count;
      }
*/
} 

void makeblinkinglight(pfVec3 loc, pfVec3 thecolor, float the_size, double rate)
{
   pfVec3 black = { 0.0f, 0.0f, 0.0f };
   int i = 0;
   //  switched to pfSphere because pfNodeBBox no longer suported
   pfSphere bsphere;

   while ( (i < MAX_BLINKING_LIGHTS) && (L_blinking_lights[i].active) )
      i++;

   if ( i < MAX_BLINKING_LIGHTS )
      {
      L_blinking_lights[i].active = TRUE;
      L_blinking_lights[i].animation = pfNewSeq();

      L_blinking_lights[i].light[0] = pfNewLPoint(1);
      pfCopyVec3 ( L_blinking_lights[i].color, thecolor );
      pfLPointColor(L_blinking_lights[i].light[0],0, thecolor);
      pfLPointSize(L_blinking_lights[i].light[0],the_size);
      pfLPointShape(L_blinking_lights[i].light[0],
                    PFLP_OMNIDIRECTIONAL,0.0f,0.0f,0.0f);
      pfLPointPos(L_blinking_lights[i].light[0],0,loc);
      pfAddChild(L_blinking_lights[i].animation,L_blinking_lights[i].light[0]);

      L_blinking_lights[i].light[1] = pfNewLPoint(1);
      pfLPointColor(L_blinking_lights[i].light[1],0,black);
      pfLPointShape(L_blinking_lights[i].light[1],
                    PFLP_OMNIDIRECTIONAL,0.0f,0.0f,0.0f);
      pfLPointPos(L_blinking_lights[i].light[1],0,loc);
      pfAddChild(L_blinking_lights[i].animation,L_blinking_lights[i].light[1]);

      set_intersect_mask((pfGroup *)L_blinking_lights[i].animation,
                         PRUNE_MASK,COLLIDE_DYNAMIC);

//      pfAddChild(findnode(loc),L_blinking_lights[i].animation);

      pfAddChild(G_animations, L_blinking_lights[i].animation);

      pfSeqTime(L_blinking_lights[i].animation,-1,rate);
      pfSeqInterval(L_blinking_lights[i].animation,PFSEQ_CYCLE,0,1);
      pfSeqDuration(L_blinking_lights[i].animation,1.0f,-1);
      pfSeqMode(L_blinking_lights[i].animation,PFSEQ_START);
      pfSetVec3 (bsphere.center, loc[X], loc[Y], loc[Z]);
      bsphere.radius = 2.0f;
      pfNodeBSphere ( L_blinking_lights[i].animation, &bsphere,
                      PFBOUND_STATIC);
      }
   else
      {
      cerr << "** More than " << MAX_BLINKING_LIGHTS << " blinking lights"
           << " requested.\n";
      cerr << "   Request for light at " << loc[X] << ", " << loc[Y] << ", "
           << loc[Z] << " ignored.\n";
      }

   }


void check_blinking_lights ()
   {
   long themode;

   for ( int i = 0; i < MAX_BLINKING_LIGHTS; i++ )
      {
      if ( L_blinking_lights[i].active )
         {
         themode = pfGetSeqMode(L_blinking_lights[i].animation);
         if ( themode == PFSEQ_STOP )
            cerr << "Blinking light " << i << " has stopped!\n";
         else if ( themode == PFSEQ_PAUSE )
            cerr << "Blinking light " << i << " is paused!\n";
         }
      }
   }

void check_animations ()
   {

   long themode;
   int i,j;

   for ( i = 0; i < MAX_CRATER; i++ )
      {
      if (L_crater[i].active)
         {
         themode = pfGetSeqMode ( L_crater[i].seq );
         if ( (themode == PFSEQ_STOP) || (themode == PFSEQ_PAUSE) )
            {
            L_crater[i].active = FALSE;
            pfRemoveChild(G_animations,L_crater[i].dcs);
            }
         }
      }
   for ( i = 0; i < MAX_FIREBALL; i++ )
      {
      if (L_fireball[i].active)
         {
         themode = pfGetSeqMode ( L_fireball[i].seq );
         if ( (themode == PFSEQ_STOP) || (themode == PFSEQ_PAUSE) )
            {
            L_fireball[i].active = FALSE;
            pfRemoveChild(G_animations,L_fireball[i].dcs);
            }
         }
      }
   for ( i = 0; i < MAX_ANIMATIONS; i++ )
      {
      for ( j = 0; j < animate[i].numloaded; j++ )
         {
         if (animate[i].animation[j].active)
            {
            themode = pfGetSeqMode ( animate[i].animation[j].sequence );
            if ( (themode == PFSEQ_STOP) || (themode == PFSEQ_PAUSE) )
               {
               animate[i].animation[j].active = FALSE;
               pfRemoveChild(G_animations,animate[i].animation[j].dcs);
               }
            }
         }
      }


   }



void makefireball(pfVec3 loc)
// Creates a fireball at the specified location
{
  static int next = -1;
  long status;

  //what is the next availble slot
  next = (next +1) % MAX_FIREBALL;
//cerr << "Fireball "<<next<<" Pos "<<loc[X]<<"  "<<loc[Y]<<"  "<<loc[Z]<<"\n";
  // Put the vehicle in the correct location oriented to the ground,

  //where should we attach this puppy?
  if(L_fireball[next].active){
    //the fireball has been used atleast once
    pfSeqMode(L_fireball[next].seq, PFSEQ_STOP);
    status = pfRemoveChild(pfGetParent(L_fireball[next].dcs,0),
                               L_fireball[next].dcs);
  }
  else
    L_fireball[next].active = TRUE;

  pfDCSTrans(L_fireball[next].dcs, loc[X], loc[Y],loc[Z]);

  set_intersect_mask((pfGroup *)L_fireball[next].dcs,
                      PRUNE_MASK,COLLIDE_DYNAMIC);

  pfAddChild (G_animations, L_fireball[next].dcs );

//  pfAddChild(findnode(loc),L_fireball[next].dcs);

  //start it up
  pfSeqMode(L_fireball[next].seq,PFSEQ_START);

}

void make_crater_array()
//really, it is just like to one above, with the exeption that it deals with 
//craters and we have to set the traversal masks
{
  pfSequence *hole;
  pfGroup *crater;
  pfDCS *scaledcs;
  int ix,jx;

  //create a sequence to represent the fire ball
  if((crater  = (pfGroup *)pfdLoadFile("crater2.flt")) == NULL){
    cerr<<"Unable to open the crater file crater2.flt\n";
    exitroutine();
  }
/*
  set_intersect_mask(crater,long(PERMANENT_DIRT<<PERMANENT_SHIFT), 
                     COLLIDE_DYNAMIC);
*/
  set_intersect_mask(crater,PRUNE_MASK,COLLIDE_DYNAMIC);

  for(jx=0;jx<MAX_CRATER;jx++){
    //it will get smaller and smaller 
    hole = pfNewSeq();
    for (ix= 0;ix<5;ix++){
      scaledcs = pfNewDCS();
      pfAddChild(scaledcs,crater);
      pfDCSScale(scaledcs,1.0f - (0.2f*ix));

      pfAddChild(hole,scaledcs);
    }
    /*set up the parameters*/
    pfSeqInterval(hole,PFSEQ_CYCLE,0,4);
    pfSeqDuration(hole,1.0f,1);
    pfSeqTime(hole,-1,CRATER_LIFE);

    //now attach them to the array of craters
    L_crater[jx].dcs = pfNewDCS();
    L_crater[jx].seq = hole;
    pfAddChild(L_crater[jx].dcs,L_crater[jx].seq);
/*
    set_intersect_mask((pfGroup *)L_crater[jx].dcs,
                     long(PERMANENT_DIRT<<PERMANENT_SHIFT), COLLIDE_DYNAMIC);
*/
  set_intersect_mask((pfGroup *)L_crater[jx].dcs,PRUNE_MASK,COLLIDE_DYNAMIC);
    L_crater[jx].active = FALSE;
  }
}

void dump_craters()
  {
  int jx;

  for(jx=0;jx<MAX_CRATER;jx++)
     {
     if ( L_crater[jx].active ) 
        {
        pfSeqMode(L_crater[jx].seq,PFSEQ_STOP);
        pfSync();
        pfRemoveChild(G_animations, L_crater[jx].dcs);
        }
     L_crater[jx].active = FALSE;
     }
   }
   
#ifndef SIG96HACK
void makecrater_pos(pfVec3 loc)
//Creates a crater at the specified location after finding out the orientation
{
  pfCoord posture;

  pfCopyVec3(posture.xyz,loc);
  pfSetVec3(posture.hpr,0.0f,0.0f,0.0f);
  if (grnd_orient(posture))
     makecrater(posture.xyz,posture.hpr);
}

void makecrater(pfVec3 xyz, pfVec3 hpr)
// Creates a crater at the specified location when the orientation is known
{
  static int next = -1;
  long status;
  //a real hack for now to keep the craters from showing on the water
  if((xyz[Z] < 1.0f) && (ABS(hpr[ROLL]) < 1.0f) && (ABS(hpr[PITCH]) < 1.0f)){
    return;
  }

  //what is the next availble slot
  next = (next +1) % MAX_CRATER; 
   

  // Put the crater in the correct location oriented to the ground,

#ifdef DEBUG
  cerr << "Crater " << xyz[X] << "  "<<xyz[Y]<<"  " <<xyz[Z] <<"\n";
  cerr << "Crater " << hpr[X] << "  "<<hpr[Y]<<"  " <<hpr[Z] <<"\n";
#endif


  //where should we attach this puppy?
  if(L_crater[next].active){
    //the crater has been used atleast once
    pfSeqMode(L_crater[next].seq,PFSEQ_STOP);
    status = pfRemoveChild(pfGetParent(L_crater[next].dcs,0),
                               L_crater[next].dcs);
  }
  else
    L_crater[next].active = TRUE;

/*
  set_intersect_mask((pfGroup *)L_crater[next].dcs, 
                     long(PERMANENT_DIRT<<PERMANENT_SHIFT), COLLIDE_DYNAMIC);
*/

  pfDCSTrans(L_crater[next].dcs, xyz[X], xyz[Y],xyz[Z]);
  pfDCSRot(L_crater[next].dcs, hpr[HEADING], hpr[PITCH], hpr[ROLL]);

  set_intersect_mask((pfGroup *)L_crater[next].dcs,
                     PRUNE_MASK,COLLIDE_DYNAMIC);

//  pfAddChild(findnode(xyz),L_crater[next].dcs);

  pfAddChild(G_animations, L_crater[next].dcs );

  //start it up
  pfSeqMode(L_crater[next].seq,PFSEQ_START);

}
#else
void makecrater_pos(pfVec3)
{}
void makecrater(pfVec3, pfVec3)
{}
#endif

int ground_conform(pfCoord *posture)
//sets the HPR of to that of the ground
{
   pfSegSet seg;
   pfHit    **hits[32];
   pfVec3   normal;
   long     flags;

   PFCOPY_VEC3(seg.segs[0].pos, posture->xyz);
   seg.segs[0].pos[Z] += 500.0f;
   PFSET_VEC3(seg.segs[0].dir, 0.0f, 0.0f, -1.0f);
   seg.segs[0].length = 1000.0f;
   seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   seg.userData = NULL;
   seg.activeMask = 0x01;
   seg.isectMask = ((PERMANENT_DIRT|PERMANENT_BRIDGE)<<PERMANENT_SHIFT);
   seg.bound = NULL;
   seg.discFunc = NULL;
   if ( pfNodeIsectSegs(G_fixed, &seg, hits) )
      {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      if (flags & PFHIT_NORM)
         {
         pfVec3 head, head90;
         float sh, ch;
         float dotp;

         pfSinCos(posture->hpr[HEADING], &sh, &ch);
         head[0] = -sh;
         head[1] =  ch;
         head[2] = 0.0f;
         pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
         dotp = PFDOT_VEC3(head, normal);
         posture->hpr[PITCH] = pfArcCos(dotp) - 90.0f;
         head90[0] =  ch;
         head90[1] =  sh;
         head90[2] = 0.0f;
         dotp = PFDOT_VEC3(head90, normal);
         posture->hpr[ROLL]  = 90.0f - pfArcCos(dotp);
         }
      
      return TRUE;
      }
   else
      return FALSE;
}

