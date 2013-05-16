// File: <nps_smoke.cc>

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
#include <iostream.h>
#include <pf.h>
#include <pfdb/pfflt.h>


#ifndef NO_DVW
#define NO_PFSMOKE
#define NO_NPSSMOKE
#endif

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#endif

 
#include "nps_smoke.h"
#include "common_defs2.h"
#include "fltcallbacks.h"
#include "terrain.h"
#include "database_const.h"
#include "collision_const.h"
#ifndef NO_DVW
extern "C" void createSmokePlume(float, float, float);
#endif

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;


#define degtorad 0.017453292	//degrees to rads 
#define NUM_PUFFS 75		//number of puffs per col
#define NUM_FLAMES 15		//number of flames per col
#define NUM_COLS 1
#define MAXPLUMES 5            //this is the biggest array of puffs you
                                	//can use--NO MORE

#define ACTIVE 1                //to determine if active
#define STANDBY 0
#define AYEARSWORTHOFSECS 315360000.0   //num secs per year


#ifndef NO_NPSSMOKE

static float wind_vel = 2.;            //if not changed by init 
static float direction_of_wind = 0.0f;
static float mysin;                    //lookup table
static float mycos;

static int number_of_plumes[MAX_SMOKE_COLORS];           //set in init by user
static int lastusedfire[MAX_SMOKE_COLORS];
static int usingcolorsmoke[MAX_SMOKE_COLORS]; 


//local variables used to keep trac of the terrain subtree
static pfGroup *L_q_mat_smoke[NUMNODES][NUMNODES];
static float dansL_binsize_x,dansL_inverse_binsize_x;
static float dansL_binsize_y,dansL_inverse_binsize_y;

// following declarations are for flame generator  
//struct for the puffs of flames
struct flame {
   pfVec3 POSIT;          //x,y,z
   int frames_since_zero;
   float life;
   int alive;             //used or unused que
   float birthtime;       //when born
   float distance_from_origin;   //how far trvled
   float age;             //secs
   float windvel;       
   pfSwitch *flameswitch;  //used to turn on or off rendering of this
   pfDCS *flamepuff;       //used to move the puppy         
   };

typedef struct flame FLAME;

//this contains the information about the whole plume
struct flameplumes { 
   int reset;   //used to linger 
   int status;        //active or standby
   int start_time;
   float lasttime;    
   //int alive;
   float age;         //in secs
   int stoppingflame; //last to render
   FLAME flamecol1[NUM_FLAMES];  //all the puffs
   };

//define the type		     
typedef struct flameplumes FLAMEPLUMES;
  
//make an array containing a bunch of plumes
static FLAMEPLUMES flameplume[MAX_SMOKE_COLORS][MAXPLUMES];
  
//positions of flames
static float FLAMEPOSITS[MAX_SMOKE_COLORS][MAXPLUMES][4];
  
//is a smoke plume active? active = 1.
//the second cell contains the number of seconds
//the user wants the plume to exist
static int FLAMEACTIVE[MAX_SMOKE_COLORS][MAXPLUMES][1]; 	

//contains the pointer to the puff image file
static pfGroup *puffimage;
  
//contains all the pointers to the plumes
static pfSwitch *getaplume[MAX_SMOKE_COLORS][MAXPLUMES];

#endif

#ifndef NO_PFSMOKE

int G_pfsmoke_smokes_count = -1;
int G_pfsmoke_flames_count = -1;
int G_pfsmoke_msmoke_count = -1;
int G_pfsmoke_trails_count = -1;
int G_pfsmoke_lsmoke_count = -1;

static pfVec3 LandUp = {0.0f,0.0f,15.0f};
static pfVec3 White = {1.0f,1.0f,1.0f}, Black = {0.01f,0.01f,0.01f},
              Red = {1.0f,0.01f,0.01f}, Green = {0.01f,1.0f,0.01f},
              Yellow = {1.0f,0.01f,1.0f};

#define PFULAND_RADIUS 8.0f
#define PFULAND_SPEED 1.10f
#define PFULAND_TURBULENCE 0.0f
#define PFULAND_DENSITY 0.70f
#define PFULAND_DISSIPATION 4.0f
#define PFULAND_EXPANSION 3.0f
#define PFULAND_DIRECTION LandUp
#endif


//defines needed locally
#define JUSTSTARTED 1		//used in flames and smoke
#define BEENRUNNING 0           //used in flames and smoke

#ifndef NO_NPSSMOKE
//-----------------function declarations-----------------------
void generate_flames(float x, float y, float z, FLAMEPLUMES flamecols[], 
	int index, float lengthoftime, int color);
	
void generate_flame_plume(float x, float y, float z, 
      FLAMEPLUMES *flamecolumn, int flameindex, float lengthoftime,
      int color);
      
void reset_fire(FLAMEPLUMES *fp);

void buildflamestructure(int color);

//pfGroup *findnode_smoke(pfVec3 loc);

pfGroup *build_smoketree(int nside, int sx, int ex, int sy, int ey);
  
float get_a_rand(float min, float max);

int rand(void);
//----------------------end function declarations----------------
#endif



//----------------------------------------------------------
//  this is generate_all_fire().   It is used by the user to
//  generate all plumes.  it generates the plumes associated
//  with the positions set in use_next_fire_plume(x, y,  z).
//  For best results place at the end  of the rendering loop.
//  this runs thru the array of positions for generating fire
//  and if the plume is active then it is generated
//-----------------------------------------------------------  
void generate_all_fire()
{    

#ifndef NO_DVW
   return;
#endif

#ifndef NO_PFSMOKE
   return;
#endif

#ifndef NO_NPSSMOKE
  int index;
  int color;

  //run thru the array of fire plumes finding the
  //active ones and cause smoke to billow
  
  for(color = 0; color < MAX_SMOKE_COLORS; color++ )
     {
     for(index = 0; index < number_of_plumes[color]; index++)
        {
        if(FLAMEACTIVE[color][index][0] == ACTIVE) 
           {
           generate_flames(FLAMEPOSITS[color][index][0], 
           FLAMEPOSITS[color][index][1], FLAMEPOSITS[color][index][2], 
           flameplume[color], index, FLAMEPOSITS[color][index][3], color);
           }
        }

     }
#endif

}//end generate_all_fire()


//-----------------------------------------------------
//  this is use_next_fire_plume(x, y,  z).  It is used
//  by the user to associate a plume with a position.
//  A plume will be associated to the position everytime
//  it is called--so only do it once for each plume
//  otherwise slowdown.  This returns the index into the
//  array containing the information  of the
//  plume that will be generated by the call to this 
//  function--length of time is in seconds input -1
//  for an infinite length of time
//---------------------------------------------------- 
int use_next_fire_plume(float x, float y, float z, 
	float lengthoftime, int color)
 
{
#ifndef NO_DVW
  lengthoftime=color=0;
  createSmokePlume(x, y, z);
  return TRUE;
#endif

#ifndef NO_PFSMOKE
  pfVec3 loc;

  int the_index;

  pfSetVec3(loc,x,y,z);

  // Go to next pfuSmoke and turn it off (just in case it was on previously)
  G_pfsmoke_lsmoke_count = (G_pfsmoke_lsmoke_count + 1) % PFUMAXSMOKES;
  the_index = G_pfsmoke_lsmoke_count;
  pfuSmokeMode(G_dynamic_data->lsmoke[the_index],PFUSMOKE_STOP);
  G_dynamic_data->lsmoke[the_index]->prevTime = -1.0f;

  // Set the pfuSmoke up for another run at a new position
  pfuSmokeType(G_dynamic_data->lsmoke[the_index],PFUSMOKE_SMOKE);
  pfuSmokeTex(G_dynamic_data->lsmoke[the_index],G_dynamic_data->missileTex);
  pfuSmokeOrigin(G_dynamic_data->lsmoke[the_index],loc,PFULAND_RADIUS);
  pfuSmokeVelocity(G_dynamic_data->lsmoke[the_index],PFULAND_TURBULENCE,PFULAND_SPEED);
  pfuSmokeDir(G_dynamic_data->lsmoke[the_index],PFULAND_DIRECTION);
  pfuSmokeDensity(G_dynamic_data->lsmoke[the_index],PFULAND_DENSITY,PFULAND_DISSIPATION,PFULAND_EXPANSION);
  switch(color) {
        case BLACK_SMOKE:
          pfuSmokeColor(G_dynamic_data->lsmoke[the_index],Black,Black); break;
        case RED_SMOKE:
          pfuSmokeColor(G_dynamic_data->lsmoke[the_index],Red,Red); break;
        case GREEN_SMOKE:
          pfuSmokeColor(G_dynamic_data->lsmoke[the_index],Green,Green); break;
        case YELLOW_SMOKE:
          pfuSmokeColor(G_dynamic_data->lsmoke[the_index],Yellow,Yellow); break;        case WHITE_SMOKE:
          pfuSmokeColor(G_dynamic_data->lsmoke[the_index],White,White); break;
        default:
          pfuSmokeColor(G_dynamic_data->lsmoke[the_index],Black,White); break;
  }
  pfuSmokeDuration(G_dynamic_data->lsmoke[the_index],lengthoftime*2.5f);
  pfuSmokeMode(G_dynamic_data->lsmoke[the_index],PFUSMOKE_START);
  return the_index;
#endif

#ifndef NO_NPSSMOKE

  int onedavewants;
  pfVec3 loc;

  loc[0] = x;
  loc[1] = y;
  loc[2] = z;

  //need to consider if the plume is already
  //active.  if it is then stopit, reset it and
  //startit at the desired location.

  if(FLAMEACTIVE[color][lastusedfire[color]][0] == ACTIVE){
            //remove from the dirt group node
            pfRemoveChild(pfGetParent(getaplume[color][lastusedfire[color]],0),
             getaplume[color][lastusedfire[color]]);

            //reset individual column
            reset_fire(&flameplume[color][lastusedfire[color]]);
            FLAMEACTIVE[color][lastusedfire[color]][0] = STANDBY;
   }

  FLAMEPOSITS[color][lastusedfire[color]][0] = x;
  FLAMEPOSITS[color][lastusedfire[color]][1] = y;
  FLAMEPOSITS[color][lastusedfire[color]][2] = z;       //height

  if(lengthoftime >= 0.){
        FLAMEPOSITS[color][lastusedfire[color]][3] = lengthoftime;
  }
  else{
        //bellow for a long time
        FLAMEPOSITS[color][lastusedfire[color]][3] = AYEARSWORTHOFSECS;
  }

  FLAMEACTIVE[color][lastusedfire[color]][0] = ACTIVE;

  //added to try to make it spatial vs logical
  pfAddChild(findnode_dirt(loc), getaplume[color][lastusedfire[color]]);

  set_intersect_mask((pfGroup *)getaplume[color][lastusedfire[color]],
                     PRUNE_MASK,COLLIDE_DYNAMIC);

  //turn on the switch of the plume we are going to use
  pfSwitchVal(getaplume[color][lastusedfire[color]], PFSWITCH_ON);

  //dr dave wants to have a handle to the plume being generated
  onedavewants = lastusedfire[color];

#ifdef SMOKE_DEBUG
  cerr << "return num of plumes =  "<< onedavewants << "\n";
#endif

  //mod is used to stay in bounds of array holding plume info
  lastusedfire[color] = ((lastusedfire[color] + 1)%(number_of_plumes[color]));

#ifdef SMOKE_DEBUG
  cerr << "last used fire =  "<< lastusedfire[color] << "\n";
#endif

  return(onedavewants);

#endif

}


//-----------------------------------------------------
//this is used to shut down an actively billowing
//plume 1 is return if successful otherwise 0 os returned
//-----------------------------------------------------

int shutoffplumenumber(int index, int color )
{

#ifndef NO_DVW
        index=color=0;
        return 1;
#endif

#ifndef NO_PFSMOKE
        G_dynamic_data->lsmoke[index]->duration=5.0f; color=0;
        return 1;
#endif

#ifndef NO_NPSSMOKE
        if(FLAMEACTIVE[color][index][0] == ACTIVE){

                //make the lengthoftime 0 so it will
                //shut itself down normally

                FLAMEPOSITS[color][index][3] = 0.;
                return(1);
        }

        else{

                return(0);
        }
#endif
}



//------------------------------------------------------  
//Title: initialize_fire()
//Author: Daniel Corbin
//Description: sets switches and gives initial attributes
//------------------------------------------------------
 
void initialize_fire(int inputted_number_of_plumes, float winddir,
	float windspd, int color )
{

#ifndef NO_DVW
  inputted_number_of_plumes=color=winddir=windspd=0;
#endif

#ifndef NO_PFSMOKE
  int counter;
  
  inputted_number_of_plumes=color=winddir=windspd=0;
  pfuInitSmokes();

  for(counter=0;counter<PFUMAXSMOKES;counter++) {
    G_dynamic_data->smokes[counter]=pfuNewSmoke();
    G_dynamic_data->flames[counter]=pfuNewSmoke();
    G_dynamic_data->msmoke[counter]=pfuNewSmoke();
    G_dynamic_data->trails[counter]=pfuNewSmoke();
    G_dynamic_data->lsmoke[counter]=pfuNewSmoke();
  }

  G_pfsmoke_smokes_count = -1;
  G_pfsmoke_flames_count = -1;
  G_pfsmoke_msmoke_count = -1;
  G_pfsmoke_trails_count = -1;
  G_pfsmoke_lsmoke_count = -1;

  G_dynamic_data->smokeTex=pfNewTex(pfGetSharedArena());
  G_dynamic_data->fireTex=pfNewTex(pfGetSharedArena());
  G_dynamic_data->missileTex=pfNewTex(pfGetSharedArena());
  pfLoadTexFile(G_dynamic_data->smokeTex,"smoke.tex");
  pfLoadTexFile(G_dynamic_data->fireTex,"fire.tex");
  pfLoadTexFile(G_dynamic_data->missileTex,"missile.tex");
  pfTexRepeat(G_dynamic_data->smokeTex,PFTEX_WRAP,PFTEX_CLAMP);
  pfTexRepeat(G_dynamic_data->fireTex,PFTEX_WRAP,PFTEX_CLAMP);
  pfTexRepeat(G_dynamic_data->missileTex,PFTEX_WRAP,PFTEX_CLAMP);
#endif

#ifndef NO_NPSSMOKE
  int pindex;
  int sindex;
  static int first_time = 1;

  direction_of_wind = winddir;
  wind_vel = windspd;

  if ( first_time )
     {
     for ( int i = 0; i < MAX_SMOKE_COLORS; i++ )
        {
        number_of_plumes[i] = 0;
        lastusedfire[i] = 0;
        usingcolorsmoke[i] = 0;
        }
     first_time = 0;
     }

 
  if (inputted_number_of_plumes > MAXPLUMES )
     {
     cerr << "More than " << MAXPLUMES << " smoke plumes requested.\n"; 
     inputted_number_of_plumes = MAXPLUMES;
     }
  else if (inputted_number_of_plumes < 0 )
     {
     cerr << "Less than 0 smoke plumes requested.\n";
     inputted_number_of_plumes = 0;
     }

  usingcolorsmoke[color] = 1;

  //set all plume switches to NULL and turn them off
  for(pindex = 0; pindex < inputted_number_of_plumes; pindex++)
  {
     getaplume[color][pindex] = NULL;
     getaplume[color][pindex] = pfNewSwitch();
     pfSwitchVal(getaplume[color][pindex], PFSWITCH_OFF);
  }

  //fill up plume array with the attributres of the desired no of
  //plumes--it comes from the user as input to initalization()
  for(pindex = 0; pindex < inputted_number_of_plumes; pindex++)
  {
  
    flameplume[color][pindex].reset = 0;
    flameplume[color][pindex].status = STANDBY;
    flameplume[color][pindex].stoppingflame = 0;
    //flameplume[color][pindex].maxage = maxage;
    flameplume[color][pindex].age = 0.;
    flameplume[color][pindex].start_time = 1;
    
    //initialize all the puff attributes
    for(sindex = 0; sindex < NUM_FLAMES; sindex++){
     
      flameplume[color][pindex].flamecol1[sindex].POSIT[0] = 0.0f;
      flameplume[color][pindex].flamecol1[sindex].POSIT[2] = 0.0f;
      flameplume[color][pindex].flamecol1[sindex].POSIT[1] = 0.0f;
      flameplume[color][pindex].flamecol1[sindex].
          frames_since_zero = 0;
      flameplume[color][pindex].flamecol1[sindex].birthtime = .0f;
      flameplume[color][pindex].flamecol1[sindex].
          distance_from_origin = 0.0f;
      flameplume[color][pindex].flamecol1[sindex].alive = 1;
      flameplume[color][pindex].flamecol1[sindex].age = 0.f;
      
      //set switch pointer to NULL and turn it off
      flameplume[color][pindex].flamecol1[sindex].flameswitch = NULL;
      flameplume[color][pindex].flamecol1[sindex].flameswitch = pfNewSwitch();
      pfSwitchVal(flameplume[color][pindex].flamecol1[sindex].flameswitch,
      	PFSWITCH_OFF);
      
      //set DCS to NULL and define it as a DCS;
      flameplume[color][pindex].flamecol1[sindex].flamepuff = NULL;
      flameplume[color][pindex].flamecol1[sindex].flamepuff = pfNewDCS();
      
    }

  }
  
   //set the flame group to null 
/*
   G_theflame = NULL;
   G_theflame = pfNewGroup();
*/
   puffimage = NULL;
  
   char filename[255]; 
   switch ( color )
      {
      case RED_SMOKE:
         sprintf ( filename, 
                   "red_smoke_puff.flt" );
         break;
      case GREEN_SMOKE:
         sprintf ( filename, 
                   "green_smoke_puff.flt" );
         break;
      case YELLOW_SMOKE:
         sprintf ( filename, 
                   "yellow_smoke_puff.flt" );
         break;
      case WHITE_SMOKE:
         sprintf ( filename, 
                   "white_smoke_puff.flt" );
         break;
      default:
         sprintf ( filename, 
                   "smoke_puff.flt" );
         break;
      }

   if ( (puffimage = (pfGroup *)LoadFlt(filename)) == NULL )
      cerr << "Smoke puff file not found:  " << filename <<  endl;
 
  lastusedfire[color] = 0;
  number_of_plumes[color] = inputted_number_of_plumes;

#ifdef SMOKE_DEBUG  
  cerr << "num of plumes =  "<< number_of_plumes << "\n";
#endif
  
  //build the tree of plumes so it will not have to be
  //   done at run time
  buildflamestructure(color);
  
  //get the sin and cos values of the wind direction
  pfSinCos(direction_of_wind, &mysin, &mycos);
  srand(1962);

#endif  
}


// 
//functions for the user to make smoke
// 

//--------------------------------------------------
//--------------------------------------------------  
//              FLAME UTILITIES                  
//The following are the UTILITY functions used to
//generate the smoke 				
//--------------------------------------------------
//-------------------------------------------------- 

//this builds the inital structure for the flame
//so it wont have to be done during execution
//THIS BUILDS THE SMOKE PLUMES THAT MUST BE ADDED
//TO THE A LEAF IN THE G_theflames SUBTREE THAT IS
//ATTACHED TO THE SCENE GROUP IN MAIN.CC 

#ifndef NO_NPSSMOKE

void buildflamestructure(int color)
{
int i,k;

   for(i = 0; i < number_of_plumes[color]; i++){
      //add the plume pointers to the flamegroup
      for(k = 0; k < NUM_FLAMES; k++){
         pfAddChild(getaplume[color][i], 
                    flameplume[color][i].flamecol1[k].flameswitch);		
         pfAddChild(flameplume[color][i].flamecol1[k].flameswitch,
                    flameplume[color][i].flamecol1[k].flamepuff);	
         pfAddChild(flameplume[color][i].flamecol1[k].flamepuff, 
                    puffimage);
      }
   }

}//end buildflamestructure

//---------------------------------------------
//generate flames is called to run thru array
//holding all plume info
//---------------------------------------------
void generate_flames(float x, float y, float z, 
	FLAMEPLUMES flamecols[], int index, float lengthoftime, int color)
{
   flamecols[index].status = ACTIVE;
   generate_flame_plume(x, y, z, &flamecols[index], index, lengthoftime,
                        color);

}// *end generated_flames

#endif


void dump_smoke()
  {

#ifndef NO_PFSMOKE
  int ix;
  for(ix=0;ix<PFUMAXSMOKES;ix++) G_dynamic_data->lsmoke[ix]->mode=PFUSMOKE_STOP;  return;
#endif

#ifndef NO_NPSSMOKE
  int ix,jx;

  for ( ix = 0; ix < MAX_SMOKE_COLORS; ix++ )
     {
     if ( usingcolorsmoke[ix] )
        {
        for ( jx = 0 ; jx < NUM_FLAMES ; jx++ )
           {
           shutoffplumenumber ( ix, jx );
           }
        }
     }
#endif
  }


#ifndef NO_NPSSMOKE

//---------------------------------------------  
// * this is reset_fire.  it is used to reset the entire
// * plume so it will be ready the next time it is called
//---------------------------------------------  
void reset_fire(FLAMEPLUMES *fp)

{
  int sindex;


  fp->reset = 0;
  fp->start_time = JUSTSTARTED;
  fp->status = STANDBY;
  fp->stoppingflame = 0;
  //fp->alive = 1;
  fp->age = 0.f;
 
  for(sindex = 0; sindex < NUM_FLAMES; sindex++){
    fp->flamecol1[sindex].POSIT[0] = 0.0f;
    fp->flamecol1[sindex].POSIT[2] = 0.0f;
    fp->flamecol1[sindex].POSIT[1] = 0.0f;
    fp->flamecol1[sindex].frames_since_zero = 0;
    fp->flamecol1[sindex].birthtime = 0.;
    
    //turn the switch off so it will not be drawn until it
    //is wanted again next time used   
    pfSwitchVal(fp->flamecol1[sindex].flameswitch,
		    	PFSWITCH_OFF);

#ifdef SMOKE_DEBUG			
   cerr << " done with reset " <<endl;			
#endif
			
  }
  
}// end reset fire


//---------------------------------------------------  
// *
// * new_generate fire
// *
// *modifications :
// *  who   when      what
// *  dc    17jun93   make fire generate for a user defined  
// *  dc    17jul93   timechanged the algo so a plume can be 
//		      placed anywhere
//                    
//--------------------------------------------------- 
void generate_flame_plume(float x, float y, float z, 
	FLAMEPLUMES *flamecolumn, int flameindex, 
	float lengthoftime, int color)

{   

  int index;
  float wvelmeterspersec = 2.;
  float max_downwind_dist = 60.0;
  float elapsed_time;
  float rightnow;
  pfVec3 source; 
  FLAME *tempflame;
  float dist;
  
  source[0] = 0.0f;
  source[1] = 0.0f;
  source[2] = 0.0f;

  //then new flame and start the timer
  if(flamecolumn->start_time > 0) {
    flamecolumn->lasttime =  (float)pfGetTime();
    flamecolumn->start_time = 0;
 
  }

  //reset the plume for next use
  if(flamecolumn->age >= lengthoftime) {
  
   	
   
        //then turn it off
 
    	flamecolumn->status = STANDBY;
      
    	//reset individual column	
    	reset_fire(&*flamecolumn);
    	FLAMEACTIVE[color][flameindex][0] = STANDBY;
    	pfSwitchVal(getaplume[color][flameindex], PFSWITCH_OFF);
        pfRemoveChild ( pfGetParent(getaplume[color][flameindex],0),
                        getaplume[color][flameindex] );
	}
 
	
  else if(flamecolumn->status == ACTIVE)
  {
    //start rendering
    //FLAMEACTIVE[color][flameindex][0] = STANDBY;
    rightnow = (float)pfGetTime();
    //rightnow = G_curtime;
    
    elapsed_time = rightnow - 
        flamecolumn->lasttime; 

    flamecolumn->age += elapsed_time;
   
    flamecolumn->lasttime = rightnow;

    if(flamecolumn->flamecol1[flamecolumn->stoppingflame].
        distance_from_origin >= 1.5  && 
	flamecolumn->stoppingflame != NUM_FLAMES-1)
    {//then wait before generting the next puff
 	flamecolumn->stoppingflame++; 
    }

   
    //go thru all the puffs in the array  
    for(index = flamecolumn->stoppingflame; index >= 0; index--)
    {
    
      //reduce the memory refs
      tempflame = &flamecolumn->flamecol1[index];
 
      //must reset the height and frames_since_zero of each puff

      //how far have we went
      if (tempflame->distance_from_origin 
          >= max_downwind_dist)
      {

        //reset for use during this session 
                    
        
        	    tempflame->frames_since_zero = 0;
        
        	    tempflame->alive = 1;
		    
		    tempflame->age = 0.f;
        
        	    tempflame->distance_from_origin = 0.0;
        
                    tempflame->POSIT[1] = 0.0;
        
                    tempflame->POSIT[0] = 0.0;
		    
		    tempflame->POSIT[2] = 0.0;
		 
		    pfSwitchVal(tempflame->flameswitch,
		    	PFSWITCH_OFF);
		
                    break;
        
       }
   
  
   
       else{
      
         //find dist of translation of puff to
	 //figure out if we can reset       
        tempflame->distance_from_origin = 
		pfDistancePt3(source, 
			tempflame->POSIT);
			
			
        //this is used only the very first time through the loop
        if(tempflame->
           frames_since_zero == 0){
	    
	    
	    //turn the switch for the puff on
            pfSwitchVal(tempflame->flameswitch,
		    	PFSWITCH_ON);
			
        tempflame->
               birthtime = (float)pfGetTime();

	      
        }
	
	//increase the age
	tempflame->age += elapsed_time;

        tempflame->POSIT[0] +=   (  (
        (elapsed_time) * 
            (wvelmeterspersec + get_a_rand(0., tempflame->age)) * mycos ));
	    
 

        tempflame->POSIT[1] +=  (  ( (elapsed_time)*  
	    (wvelmeterspersec + get_a_rand(0., tempflame->age))*
            mysin   ));        
	


	dist = pfSqrt((tempflame->POSIT[1] * tempflame->POSIT[1] + 
		tempflame->POSIT[0] * tempflame->POSIT[0]));   
	


        if(dist > 1.)  
          	tempflame->POSIT[2] = flog(dist) * dist; 
        else
	        tempflame->POSIT[2] = dist;
	  
	 

       // following translates each puff as a 
       // 	      function of the frame the puffwill appear in 
       // 		and  wind speed 
        
        pfDCSTrans(flamecolumn->flamecol1[index].flamepuff,
        tempflame->POSIT[0] + x,
        tempflame->POSIT[1] + y, 
	tempflame->POSIT[2] + z);
	

	    	
       pfDCSScale(tempflame->flamepuff,
        	(2.0+ powf(0.2* tempflame->
            distance_from_origin,1.1)));	

 
        //need to update the birthtime for the next iteration
        tempflame->birthtime = (float)pfGetTime();


	
  
      }
      
      

      //this puff is drawn so increments 
      	    //number since born
      tempflame->frames_since_zero++; 
         
    }
    
    
  }
  

  
}//end generate flames
 
#endif 
  


// the following functions are used for the smoke hiearchy
pfGroup *findnode_smoke(pfVec3 loc)
/*finds which of the nodes this model belongs to*/
{

#ifndef NO_DVW
   loc[0]=loc[1]=loc[2]=0;
   return NULL;
#endif

#ifndef NO_PFSMOKE
   loc[0]=loc[1]=loc[2]=0;
   return NULL;
#endif

#ifndef NO_NPSSMOKE

   int xindex,yindex;
#ifdef SMOKE_DEBUG
   cout << "xnode = "
        << ((int)(loc[X]) * L_inverse_binsize_x)
        << "ynode = "
        << ((int)(loc[Y]) * L_inverse_binsize_y)
        << endl;
#endif
   xindex = (int)floorf(loc[X] * dansL_inverse_binsize_x);
   yindex = (int)floorf(loc[Y] * dansL_inverse_binsize_y);
   if ( xindex > (NUMNODES-1) )
      {
      xindex = (NUMNODES-1); 
      }
   if ( yindex > (NUMNODES-1) )
      {
      yindex = (NUMNODES-1);
      }
   if ( xindex < 0 )
      {
      xindex = 0;
      }
   if ( yindex < 0 )
      {
      yindex = 0;
      }
   return(L_q_mat_smoke[xindex][yindex]);
#endif
}


pfGroup *deviy_up_for_smoke()
//divide terrain database to improve culling returns a pointer to a group
{
#ifndef NO_DVW
  return NULL;
#endif

#ifndef NO_PFSMOKE
  return NULL;
#endif

#ifndef NO_NPSSMOKE
  int ix,jx;
  int maxarray = NUMNODES-1;
  float bbox_xsize;
  float bbox_ysize;
   //what size are bins?

#ifdef DATALOCK
  pfDPoolLock ( (void *) G_dynamic_data );
#endif
  bbox_xsize = G_dynamic_data->bounding_box.xsize;
  bbox_ysize = G_dynamic_data->bounding_box.ysize;
#ifdef DATALOCK
  pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

   dansL_binsize_x = bbox_xsize / float(NUMNODES);
   dansL_inverse_binsize_x = 1.0f/dansL_binsize_x;
   dansL_binsize_y = bbox_ysize / float(NUMNODES);
   dansL_inverse_binsize_y = 1.0f/dansL_binsize_y;

   /*build the base layer*/
   for(ix=0;ix<NUMNODES;ix++){
     for(jx=0;jx<NUMNODES;jx++){
       L_q_mat_smoke[ix][jx] = pfNewGroup();
     }
   }

   //now, build the quadtree to support rapid culling and intersection
   return(build_smoketree(maxarray,0,maxarray,0,maxarray));
#endif
}


#ifndef NO_NPSSMOKE
pfGroup *build_smoketree(int nside, int sx, int ex, int sy, int ey)
//build the node for this level, or attach it to the L_q_mat_smoke matrix
//nside is the number of base nodes that are on the side of this node's
// quad tree
{
  //number of nodes at this level
  int hside = nside/2;
  int pside = nside/2+1;
  pfGroup *tnode;
  
#ifdef SMOKE_DEBUG    
  cerr<<"Side cnt " <<nside<<"  "<<sx<<"  "<<ex<<"  "<<sy<<"  "<<ey<<"\n";
#endif

  if(nside == 0){
    //this is the base case
    return(L_q_mat_smoke[sx][sy]);
  }

  //else we have to do some bigtime work
  tnode = pfNewGroup();
  pfAddChild(tnode,build_smoketree(nside/2,sx      ,sx+hside,sy      ,sy+hside));
  pfAddChild(tnode,build_smoketree(nside/2,sx      ,sx+hside,sy+pside,ey      ));
  pfAddChild(tnode,build_smoketree(nside/2,sx+pside,ex      ,sy      ,sy+hside));
  pfAddChild(tnode,build_smoketree(nside/2,sx+pside,ex      ,sy+pside,ey      ));

  //return tnode which is the root of the tree at this location
  return(tnode);
}


//
//    GENERIC UTILITIES          
//


//Title: get_a_rand()
//Author: J. Falby
//Description:  returns a float between min and max

float get_a_rand(float min, float max)

{

  float tmp;   

     tmp = ((max - min) * (.0000305185)) * (float)rand() + min;
	
     return(tmp);

} 

#endif
