/*
 *  casualties.cc
 *    Authors: Bryan C. Stewart
 *    Thesis Advisors:	Prof. John Falby
 *        		Dr. Dave Pratt
 *			Dr. Mike Zyda
 *    Date:  22 January 1996
 *
 *    Continuing Thesis work by Bryan Stewart
 *    Modified 29 January 96
 *
 */

// Standard C includes 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include <ctype.h>
#include <sys/sysmp.h>
#include <bstring.h> // for bzero func call

// IRIS Performer includes 
#include <Performer/pf.h>
#include "pfutil.h"
#include "common_defs2.h"
#include "effects.h"
#include "nps_smoke.h"
#include "timeofday.h"
#include "casualties.h"

#define PFUSMOKE_RADIUS 0.60 // Multiplied by half diagonal length of bbox
#define PFUSMOKE_SPEED 1.0f
#define PFUSMOKE_TURBULENCE 0.0f
#define PFUSMOKE_DENSITY 0.65f
#define PFUSMOKE_DISSIPATION 3.0f
#define PFUSMOKE_EXPANSION 3.0f
#define PFUSMOKE_DIRECTION UpMore
#define PFUSMOKE_BEGIN_COLOR Grey
#define PFUSMOKE_END_COLOR Grey
#define PFUSMOKE_DURATION -1 // 600.0f Ten minutes

#define PFUFLAME_RADIUS 0.45f
#define PFUFLAME_SPEED 1.0f
#define PFUFLAME_TURBULENCE 0.0f
#define PFUFLAME_DENSITY 0.11f
#define PFUFLAME_DISSIPATION 0.50f
#define PFUFLAME_EXPANSION 0.50f
#define PFUFLAME_DIRECTION UpDir
#define PFUFLAME_BEGIN_COLOR White
#define PFUFLAME_END_COLOR White
#define PFUFLAME_DURATION -1 // 180.0f Three minutes

#define FIRE_GROWTH_INC         0.005f
#define SMOKE_GROWTH_INC        0.005f
#define MAXFIRESIZE             2.0f
#define MAXSMOKESIZE            0.25f //20.0f

extern volatile DYNAMIC_DATA *G_dynamic_data;
#ifndef NO_PFSMOKE
extern int G_pfsmoke_smokes_count;
extern int G_pfsmoke_flames_count;

static pfVec3 UpDir = {0.0f,0.0f,0.05f}, UpMore = {0.0f,0.0f,11.0f};
static pfVec3 White = {1.0f,1.0f,1.0f}, Grey = {0.6f,0.7f,0.8f};
#endif

/************************************************************************
 *									*		
 * 		              particleSetUp	         		*
 *									*
 ************************************************************************/
void particleSetUp(pfLightPoint **parts, pfVec3 *vel, pfVec3 *dest, 
                   long num, pfVec4 color, pfVec3 origin, 
                   float size, float dir, float pitch, 
                   float radius, float length)
{
    int          i= 0;						
    //pfVec3       pos;
    float        SIN, COS, PSIN;
    long         numTargets = 2 *num;  
    int          rings = 20;
    float        elevation = 0.0;
    float        radDec;
    float        tarRadius = 0.0;
    double       random;
    float        xtarget, ytarget, ztarget;
    pfVec3       tarOrigin;
    pfSeg        tarLocation;
    float        angle = 361.0;
    int          index;
    float        heading;
    float        angleInc = 0.0;
    float        area;
    float        totalArea;
    //float        inc;
  
 // Allow use of larger pixels than allowed by Performer
 //   glcompat(1011,0); this causes a core dump in Perfomer 2.0

    pfLPointSize(*parts, size);

    PSIN = fsin(pitch);

    // Figure out particles target parameters
    pfSinCos(dir, &SIN, &COS);
    xtarget = -SIN*length;
    ytarget = COS*length;
    ztarget = PSIN*length;
    radDec = radius/((float)rings);
    tarOrigin[0]= origin[0] + xtarget;
    tarOrigin[1]= origin[1] + ytarget;
    tarOrigin[2]= origin[2] + ztarget;
    heading = dir -90.0;
    totalArea = 3.1459*powf(radius,2.0);
    
 
    pfLPointShape(*parts, PFLP_OMNIDIRECTIONAL, 0.0, 0.0, 0.0);

       
    // load the destination paths for the particles
    for (i = 0; i < numTargets; i++)
    {
       pfMakePolarSeg(&tarLocation, tarOrigin, heading, elevation, tarRadius);
     
      dest[i][0] = tarLocation.pos[0] +(tarLocation.dir[0]*tarRadius);
      dest[i][1] = tarLocation.pos[1] +(tarLocation.dir[1]*tarRadius);
      dest[i][2] = tarLocation.pos[2] +(tarLocation.dir[2]*tarRadius);

      
      angle += angleInc;
      if (angle > 360.0)
      {
        tarRadius += radDec;  // increment target radius
        heading = dir -90.0;
        area = 3.1459*powf(tarRadius, 2.0);
        angleInc = 360.0/(((float)numTargets*(area/totalArea)) - (float)i);
        angle = angleInc;
      }  
      // determine elevation and heading for polar segments
      if ((angle <= 90.0) && (angle > 0.0))
      {
        elevation = angle;
        heading = dir -90.0;
      }
      if ((angle <= 180.0) && (angle > 90.0))
      {      
        elevation = 180 -angle;
        heading = dir +90.0;
      }
      if ((angle <= 270.0) && (angle >180.0))
      {
        elevation = 180.0 -angle;
        heading = dir + 90.0;
      }
      if ((angle <= 360.0) && (angle > 270.0))
      {
        elevation = angle - 360.0;
        heading = dir -90.0;
      }
     
    } 

    // Initialize all particles positions and velocities
    for (i = 0; i< num; i++)
    {
      pfLPointColor(*parts, i, color);
      pfLPointPos(*parts, i, origin);
      random = drand48();
      index = (int)(random *numTargets);
      pfSubVec3(vel[i], dest[index], origin);
      random = drand48();
      if (random < 0.3)
      {
         vel[i][0] *= (random +0.2);
         vel[i][1] *= (random +0.2);
         vel[i][2] *= (random +0.2); 
      } 
      else
      {
        vel[i][0] *= random;
        vel[i][1] *= random;
        vel[i][2] *= random;
      }
    } 
} //particleSetup



/************************************************************************
 *									*		
 * 			moveParticles
 *									*
 ************************************************************************/

void moveParticles(pfLightPoint **particles, pfVec3 *targets, pfVec3 *vel, 
                   long num, pfVec3 origin,
                   float length, float percent, int physBased)
{
   long       i = 0;
   double     random;
   pfVec3     pos;
   float      numTargets = 2*num;
   int        newTarget;
   
   if (physBased) { 
     for ( i = 0; i< num; i++) {
       pfGetLPointPos(*particles, i, pos);
       if ((pos[2]-origin[Z]) < 0.0) {
         pfCopyVec3(pos, origin);
         random = drand48();
         newTarget = (int)(random*numTargets);
         pfSubVec3(vel[i], targets[newTarget], origin);
         random = drand48();
         vel[i][0] *= (random*percent);
         vel[i][1] *= (random*percent);
         vel[i][2] *= (random*percent);
       }
       vel[i][2] -= 0.05;
       pfAddVec3(pos, vel[i], pos);
       pfLPointPos(*particles, i, pos);
     }
   }
   else {
     for ( i = 0; i< num; i++) {
       pfGetLPointPos(*particles, i, pos);
       if (pfDistancePt3(pos, origin) > length) {
         pfCopyVec3(pos, origin);
         random = drand48();
         newTarget = (int)(random*numTargets);
         pfSubVec3(vel[i], targets[newTarget], origin);
         random = drand48();
         if (random < 0.3) {
           vel[i][0] *= (random +0.2);
           vel[i][1] *= (random +0.2);
           vel[i][2] *= (random +0.2); 
         } 
         else {       
           vel[i][0] *= random;
           vel[i][1] *= random;
           vel[i][2] *= random;
         }
       }
       pfAddVec3(pos, vel[i], pos);
       pfLPointPos(*particles, i, pos);
     }
   }
} // moveParticles

/*
    // Initialize  GUI Shared data from DCVET
    Shared->lamps		= 1;
    Shared->flashlight          = 1.0;
    Shared->flash	        = 0;
    Shared->height              = 1.8f;
    Shared->spread              = 10.0f;
    Shared->exponent            = 90.0f;
    Shared->waterHit            = 0;
    Shared->close               = 1;
    Shared->stow                = 1;
    Shared->nozzle              = 0;
    Shared->okToQuit            = 0;
    Shared->actualDCS           = 0;
    Shared->steamPerCent        = 1.0f;
    Shared->FOPerCent           = 1.0f;
    Shared->desmoke             = 0;
    Shared->fans                = 0;
    Shared->steam               = 0;
    Shared->steamValve          = 0;
    Shared->halon               = 0;
    Shared->whoseFire           = -1;
    Shared->whoseSteam          = -1;
    Shared->numWall             = 0;
    Shared->growth              = MIN_GROWTH;
    Shared->fogFlag             = PFFOG_OFF;	// fog on/off
    Shared->fireFlag            = 0;
    Shared->jackNozzle          = FALSE;
*/

//*********************************************************************//
//****************** class CASUALTY_STEAM *****************************//
//*********************************************************************//

CASUALTY_STEAM::CASUALTY_STEAM(pfNode *node, pfVec3 origin_xyz)
{
    particles = pfNewLPoint(STEAMPOINTS);
    pfSetVec4(color,0.56, 0.56, 0.56, 1.0);
    pfCopyVec3(origin, origin_xyz);

    // set up steam particle system
    particleSetUp(&particles, velocity, dest, 
                  STEAMPOINTS, color, origin, STEAMSIZE, STEAMDIR, 
                  STEAMPITCH, STEAMRADIUS, STEAMLENGTH);
    parent = node;
}

CASUALTY_STEAM::~CASUALTY_STEAM()
{
    if (parent != NULL) {
      pfRemoveChild(parent, particles);
    }
    pfDelete(particles);
}

void CASUALTY_STEAM::move(float percent)
{
    moveParticles(&particles, dest, velocity, 
                  STEAMPOINTS, origin, (percent*STEAMLENGTH), 0.0, 0);
}

void CASUALTY_STEAM::start()
{
    if (pfGetNumParents((pfNode *)particles)==NULL) {
       pfAddChild(parent, particles);  
    } 
}

void CASUALTY_STEAM::stop()
{
    if (pfGetNumParents((pfNode *)particles)!=NULL) {
       pfRemoveChild(parent, particles);
    }
}

int CASUALTY_STEAM::isStarted()
{
    return pfGetNumParents((pfNode *)particles)!=NULL;
}

//*********************************************************************//
//****************** class CASUALTY_OIL_LEAK **************************//
//*********************************************************************//

CASUALTY_OIL_LEAK::CASUALTY_OIL_LEAK(pfNode *node, pfVec3 origin_xyz)
{
    particles = pfNewLPoint(OILPOINTS);
    pfSetVec4(color,0.607, 0.247, 0.0, 1.0);
    pfCopyVec3(origin, origin_xyz);

    // set up steam particle system
    particleSetUp(&particles, velocity, dest, 
                  OILPOINTS, color, origin, OILSIZE, OILDIR, 
                  OILPITCH, OILRADIUS, OILLENGTH);
    parent = node;
}

CASUALTY_OIL_LEAK::~CASUALTY_OIL_LEAK()
{
    if (parent != NULL) {
      pfRemoveChild(parent, particles);
    }
    pfDelete(particles);
}

void CASUALTY_OIL_LEAK::move(float percent)
{
    moveParticles(&particles, dest, velocity, 
                  OILPOINTS, origin, OILLENGTH, percent, 1);
}


void CASUALTY_OIL_LEAK::start()
{
    if (pfGetNumParents((pfNode *)particles)==NULL) {
       pfAddChild(parent, particles);  
    } 
}

void CASUALTY_OIL_LEAK::stop()
{
    if (pfGetNumParents((pfNode *)particles)!=NULL) {
       pfRemoveChild(parent, particles);
    }
}

int CASUALTY_OIL_LEAK::isStarted()
{
    return pfGetNumParents((pfNode *)particles)!=NULL;
}


//*********************************************************************//
//****************** class CASUALTY_FIRE ******************************//
//*********************************************************************//

CASUALTY_FIRE::CASUALTY_FIRE(pfVec3 origin_xyz)
{
   flame_index = -1;
   puffs_index = -1;
   smoke_index = -1;
   size = 0.0f;
   smoke_size = 0.0f;
   pfCopyVec3(relposvec, origin_xyz);
   pfSetVec3(loc, 0.0f, 0.0f, 0.0f);
}


CASUALTY_FIRE::~CASUALTY_FIRE()
{
}


void CASUALTY_FIRE::update_position(pfCoord &posture)
{
   pfMatrix   orient;
   pfVec3     posvec;
   float      poslen=pfLengthVec3(relposvec);
   pfCopyVec3(posvec, relposvec);

   pfMakeEulerMat ( orient, posture.hpr[HEADING], 
                            posture.hpr[PITCH], 
                            posture.hpr[ROLL]);
   pfNormalizeVec3(posvec);
   pfXformVec3(posvec, posvec, orient);
   pfScaleVec3(posvec, poslen, posvec);
   pfAddVec3(loc,posture.xyz,posvec);
#ifndef NO_PFSMOKE
   if (flame_index >= 0) {
      pfuSmokeOrigin(G_dynamic_data->flames[flame_index],loc,
                     PFUFLAME_RADIUS*2.0f*size);
   }
   if (puffs_index >= 0) {
      pfuSmokeOrigin(G_dynamic_data->smokes[puffs_index],loc,
                     PFUSMOKE_RADIUS*2.0f*size);
   }
#endif
/* Commented out 2/2/96 by bcs - looks bad using this
   if (smoke_index >= 0) {
cerr<<"smoke_size:"<<smoke_size<<endl;
      pfSetVec3(SmokeDir, 0.0f,0.0f, smoke_size);
      pfuSmokeDir(G_dynamic_data->smokes[smoke_index],SmokeDir);
      pfuSmokeOrigin(G_dynamic_data->smokes[smoke_index],loc,14.0f);
   }
*/
}


void CASUALTY_FIRE::start(pfCoord &posture)
{
#ifndef NO_PFSMOKE
   int the_index;

   // Go to next pfuSmoke and turn it off
   //   (just in case it was on previously)
   G_pfsmoke_flames_count = (G_pfsmoke_flames_count + 1) % PFUMAXSMOKES;
   the_index = flame_index = G_pfsmoke_flames_count;
   pfuSmokeMode(G_dynamic_data->flames[flame_index],PFUSMOKE_STOP);
   G_dynamic_data->flames[flame_index]->prevTime = -1.0f;

   //setup the fire stuff
   G_dynamic_data->flames[the_index]->startPuff=
   G_dynamic_data->flames[the_index]->numPuffs=0;
   pfuSmokeType(G_dynamic_data->flames[the_index],PFUSMOKE_FIRE);
   pfuSmokeTex(G_dynamic_data->flames[the_index],G_dynamic_data->fireTex);
   pfuSmokeVelocity(G_dynamic_data->flames[the_index],PFUFLAME_TURBULENCE,PFUFLAME_SPEED);
   pfuSmokeDir(G_dynamic_data->flames[the_index],PFUFLAME_DIRECTION);
   pfuSmokeDensity(G_dynamic_data->flames[the_index],PFUFLAME_DENSITY,PFUFLAME_DISSIPATION,PFUFLAME_EXPANSION);
   pfuSmokeColor(G_dynamic_data->flames[the_index],PFUFLAME_BEGIN_COLOR,PFUFLAME_END_COLOR);
   pfuSmokeDuration(G_dynamic_data->flames[the_index],PFUFLAME_DURATION);
   pfuSmokeMode(G_dynamic_data->flames[the_index],PFUSMOKE_START);

   // Go to next pfuSmoke and turn it off
   //   (just in case it was on previously)
   G_pfsmoke_smokes_count = (G_pfsmoke_smokes_count + 1) % PFUMAXSMOKES;
   the_index = puffs_index = G_pfsmoke_smokes_count;
   pfuSmokeMode(G_dynamic_data->smokes[puffs_index],PFUSMOKE_STOP);
   G_dynamic_data->smokes[puffs_index]->prevTime = -1.0f;

   //setup puffs stuff
   G_dynamic_data->smokes[the_index]->startPuff=
   G_dynamic_data->smokes[the_index]->numPuffs=0;
   pfuSmokeType(G_dynamic_data->smokes[the_index],PFUSMOKE_SMOKE);
   pfuSmokeTex(G_dynamic_data->smokes[the_index],G_dynamic_data->smokeTex);
   pfuSmokeVelocity(G_dynamic_data->smokes[the_index],PFUSMOKE_TURBULENCE,PFUSMOKE_SPEED);
   pfuSmokeDir(G_dynamic_data->smokes[the_index],PFUSMOKE_DIRECTION);
   pfuSmokeDensity(G_dynamic_data->smokes[the_index],PFUSMOKE_DENSITY,PFUSMOKE_DISSIPATION,PFUSMOKE_EXPANSION);
   pfuSmokeColor(G_dynamic_data->smokes[the_index],PFUSMOKE_BEGIN_COLOR,PFUSMOKE_END_COLOR);
   pfuSmokeDuration(G_dynamic_data->smokes[the_index],PFUSMOKE_DURATION);
   pfuSmokeMode(G_dynamic_data->smokes[the_index],PFUSMOKE_START);

   if (!isSmoke()) {
/*Commented out 2/2/96 by bcs - looks bad using this
      // Go to next pfuSmoke and turn it off
      //   (just in case it was on previously)
      G_pfsmoke_smokes_count = (G_pfsmoke_smokes_count + 1) % PFUMAXSMOKES;
      the_index = smoke_index = G_pfsmoke_smokes_count;
      pfuSmokeMode(G_dynamic_data->smokes[smoke_index],PFUSMOKE_STOP);
      G_dynamic_data->smokes[smoke_index]->prevTime = -1.0f;

      //setup smoke stuff
      G_dynamic_data->smokes[the_index]->startPuff=
      G_dynamic_data->smokes[the_index]->numPuffs=0;
      pfuSmokeType(G_dynamic_data->smokes[the_index],PFUSMOKE_SMOKE);
      pfuSmokeTex(G_dynamic_data->smokes[the_index],G_dynamic_data->smokeTex);
      pfuSmokeVelocity(G_dynamic_data->smokes[the_index],
                       PFUSMOKE_TURBULENCE,PFUSMOKE_SPEED*0.5f);
      pfuSmokeDensity(G_dynamic_data->smokes[the_index],
                      PFUSMOKE_DENSITY*0.5f,3.0f,3.0f);
      pfuSmokeColor(G_dynamic_data->smokes[the_index],Grey,Grey);
      pfuSmokeDuration(G_dynamic_data->smokes[the_index],PFUSMOKE_DURATION);
      pfuSmokeMode(G_dynamic_data->smokes[the_index],PFUSMOKE_START);
      smoke_size = 0.1f;
*/
      smoke_size = 0.1f;
   } // !isSmoke()

   size = 1.0f;
#endif
   update_position(posture);
}


void CASUALTY_FIRE::stop()
{
#ifndef NO_PFSMOKE
   //Stop the flames
   pfuSmokeMode(G_dynamic_data->flames[flame_index],PFUSMOKE_STOP);
   G_dynamic_data->flames[flame_index]->prevTime = -1.0f;
   flame_index = -1;

   //Stop the puffs
   pfuSmokeMode(G_dynamic_data->smokes[puffs_index],PFUSMOKE_STOP);
   G_dynamic_data->smokes[puffs_index]->prevTime = -1.0f;
   puffs_index = -1;
#endif
}

int CASUALTY_FIRE::isStarted()
{
   return flame_index >= 0;
}

int CASUALTY_FIRE::isSmoke()
{
   //return smoke_index >= 0;
   return smoke_size > 0.001f;
}

void CASUALTY_FIRE::update_size(float extinguish)
{
   if (size < 0.001f) {
      size = 0.0f;
      stop();
   }
   else { //shrink/expand fire size as appropriate
      if (extinguish < 0.001f) { //Then fire increase
         if (size > MAXFIRESIZE) 
            size = MAXFIRESIZE;
         else
            size += FIRE_GROWTH_INC;
      }
      else
         size -= extinguish;
   }
}

void CASUALTY_FIRE::update_smoke_size(float extinguish, int inside_ship)
{
   static float last_smoke_val = 0.0f;
   float smoke_diff;

   if (smoke_size < 0.001f) {
      smoke_size = 0.0f;
      //Stop the smoke
/*Commented out 2/2/96 by bcs - looks bad using this
      pfuSmokeMode(G_dynamic_data->smokes[smoke_index],PFUSMOKE_STOP);
      G_dynamic_data->smokes[smoke_index]->prevTime = -1.0f;
      smoke_index = -1;
*/
   }
   else { //shrink/expand smoke size as appropriate
      if (extinguish < 0.001f && flame_index >=0) { //Then fire increase
         if ((G_dynamic_data->tod - smoke_size) < MAXSMOKESIZE) 
            smoke_size = G_dynamic_data->tod - MAXSMOKESIZE;
         else
            smoke_size += SMOKE_GROWTH_INC;
      }
      else
         smoke_size -= extinguish;
/*Commented out 2/2/96 by bcs - looks bad using this
      pfuSmokeDensity(G_dynamic_data->smokes[smoke_index],0.3f*smoke_size,3.0f,3.0f);
*/
   }

   if (last_smoke_val != smoke_size) {
      last_smoke_val = smoke_size;
      if (smoke_size > 0) {
         smoke_diff = G_dynamic_data->tod - smoke_size;
      }
      else {
         smoke_diff = G_dynamic_data->tod;
      }
      if (inside_ship) { // Only change lighting is inside of ship
        updatetod(smoke_diff,
                  (CHANNEL_DATA *)&(G_dynamic_data->pipe[0].channel[0]));
      }
      else {
        updatetod(G_dynamic_data->tod,
                  (CHANNEL_DATA *)&(G_dynamic_data->pipe[0].channel[0]));
      }
   }
}


