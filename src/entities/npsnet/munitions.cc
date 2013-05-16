// File: <munitions.cc>

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

#include <string.h>
#include <bstring.h>
#include <iostream.h>

#include "munitions.h"

#include "disnetlib.h"
#include "common_defs2.h"
#include "appearance.h"
#include "effects.h"
#include "terrain.h"
#ifdef JACK
#include "jack_di_veh.h"
#endif
#include "collision_const.h"

// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile STATIC_DATA     *G_static_data;
extern volatile float            G_curtime;
extern          VEHTYPE_LIST    *G_vehtype;
extern          VEH_LIST        *G_vehlist;
extern volatile int              G_drivenveh;
extern          WEAPON_LIST     *G_weaplist;
extern          pfGroup         *G_moving;
extern          DIS_net_manager *G_net;


// Locals used by this library
static pfSwitch *L_current_switch;
static int L_veh_node = NOSHOW;
static int L_veh_hit = NOSHOW;
static SWITCHNODEDATA *L_dead_list=NULL;

extern unsigned short event_cnt;



void initialize_weapons_array(int isms)
//set up the weapons list
{
   pfGroup *tempmodel;
   pfSwitch *munswitch;
   int typedex;
   int modeldex;


      munswitch = pfNewSwitch();

   if ( !isms )
      typedex = G_vehlist[G_drivenveh].type;
   else
     typedex = 85;
   
   //read in the models and bind them to the switch node
   
      modeldex = G_vehtype[typedex].fireinfo[PRIMARY].munition;
      tempmodel = G_vehtype[modeldex].model;
      if ( tempmodel != NULL )
         pfAddChild(munswitch, tempmodel );
      else
         cerr << "Primary weapon model not loaded for weapons array.\n";
   
      modeldex = G_vehtype[typedex].fireinfo[SECONDARY].munition;
      tempmodel = G_vehtype[modeldex].model;
      if ( tempmodel != NULL )
         pfAddChild(munswitch, tempmodel );
      else
         cerr << "Secondary weapon model not loaded for weapons array.\n";
   
      modeldex = G_vehtype[typedex].fireinfo[TERTIARY].munition;
      tempmodel = G_vehtype[modeldex].model;
      if ( tempmodel != NULL )
         pfAddChild(munswitch, tempmodel );
      else
         cerr << "Tertiary weapon model not loaded for weapons array.\n";
   

      pfSwitchVal(munswitch,PFSWITCH_OFF);
 
      //create a new list
      G_weaplist = new WEAPON_LIST [MAX_MUNITIONS];
   
      //go through the attaching the stuff
      WEAPON_LIST *current_weap = &(G_weaplist[0]);
      for (int index=0;index<MAX_MUNITIONS;index++)
      {
         current_weap->id = NOSHOW;
         current_weap->cat = MINACTIVE;
         current_weap->mptr = NULL;
         current_weap->icon = (pfSwitch *)pfClone(munswitch,0);
         current_weap->dcs = pfNewDCS();
         pfAddChild(current_weap->dcs,current_weap->icon);
/*
         pfAddChild(G_missiles,current_weap->dcs);
*/
         pfDCSTrans(current_weap->dcs,-10000.0f,-10000.0f,-10000.0f);
         current_weap++;
      }
}

void update_munitions(float delta_time,float cum_time)
//move all of my munitions
{
   WEAPON_LIST *current_weap = &(G_weaplist[0]);

   for (int index=0;index<MAX_MUNITIONS;index++) {
      if (current_weap->cat != NOSHOW) {
#ifdef TRACE
        cerr << "moving weapon " << index << endl;
#endif
        if(DEACTIVE == current_weap->mptr->move(delta_time,cum_time)){
            //so much for this one
            deactive_weap(index);
        }
      }
      current_weap++;
   }

#ifdef TRACE
   cerr << "Leaving update_munitions\n";
#endif


}


void deactive_weap(int mid)
//deactivate the weapon
{
  WEAPON_LIST *weapptr;
  weapptr = &(G_weaplist[mid]);
  pfGroup *parent = NULL;

  if(weapptr) {
     //deactivate a weapon
     delete weapptr->mptr;
     weapptr->id = NOSHOW;
     weapptr->cat = MINACTIVE;
     weapptr->mptr = NULL;
     if ( weapptr->dcs != NULL )
        {
        parent = pfGetParent(weapptr->dcs,0);
        if ( parent != NULL )
        pfDCSTrans(weapptr->dcs,-10000.0f,-10000.0f,-10000.0f);
        }
   }
}

void firebuttonhit(wdestype designation,int vid, pfCoord veh_posture,
                   pfCoord veh_look )
//Switch statement to handle the local firing of the weapons
{
  static int mid = -1;  //the next available slot
  static teamMember = -1;
  WEAPON_LIST *weapptr;
  pfCoord posture;
  pfVec3 velocity;
  int target_lock;
  int the_target;
  G_vehlist[vid].vehptr->getposparm(&posture,&velocity);
  int icontype;
  int weapon_index;
  mtype munition_type;
  int newcat=int(designation);
  JACK_TEAM_VEH *jack_team;

  icontype = G_vehlist[vid].type;

  // Do some special things for soldiers
// May need to add some thing to get DUDE to work here? -rb
  if((G_vehlist[vid].vehptr->gettype() == JACK_DI) ||
      (G_vehlist[vid].vehptr->gettype() == JACK_TEAM)) {

     if(G_vehlist[vid].vehptr->gettype() == JACK_TEAM) {
        jack_team = (JACK_TEAM_VEH *) G_vehlist[vid].vehptr;
        teamMember = (teamMember+1) % jack_team->getTeamCount(); 
     }

     // Change weapon status to deployed, if vel ==0 . Note S/B
     // appear deployed if vel !=0 but can't now so don't change status 
     if(((G_vehlist[vid].vehptr->getstatus() &
	Appearance_LifeForm_State_Mask) == Appearance_LifeForm_UprightStill) ||
	((G_vehlist[vid].vehptr->getstatus() &
        Appearance_LifeForm_State_Mask) == Appearance_LifeForm_Kneeling) ||
        ((G_vehlist[vid].vehptr->getstatus() &
        Appearance_LifeForm_State_Mask) == Appearance_LifeForm_Prone)) {

	if ((G_vehlist[vid].vehptr->getstatus() &
       	Appearance_LifeForm_Weapon1_Mask) != 
	Appearance_LifeForm_Weapon1_Deployed) {
           G_vehlist[vid].vehptr->changeWeaponStatus(Appearance_LifeForm_Weapon1_Deployed);
        //cerr << "Deploying weapon\n";
        }
     }

  }

  if(((G_vehlist[vid].vehptr->gettype() != JACK_TEAM) && 
       (G_vehlist[vid].vehptr->oktofire(designation,G_curtime))) ||
     ((G_vehlist[vid].vehptr->gettype() == JACK_TEAM)  && 
       ((teamMember > 0) || ((teamMember == 0) &&
       (G_vehlist[vid].vehptr->oktofire(designation,G_curtime)))))) {

     mid = (mid + 1) % MAX_MUNITIONS;
     weapptr = &(G_weaplist[mid]);
#ifdef DATALOCK
     pfDPoolLock ( (void *) G_dynamic_data );
#endif
     target_lock = G_dynamic_data->target_lock;
     if ( target_lock )
        the_target = G_dynamic_data->target;
     else
        the_target = -1;
#ifdef DATALOCK
     pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

     if(weapptr->mptr){
       //the thing was not done, so explode it and then deactivate it
       //or some body was still active so, deactiveate it
       //cerr << "Mid " << mid << " was still in use\n";
       //no effect for this entity
       weapptr->mptr->senddetonation(DetResult_None);
       deactive_weap(mid);
     } 

     //newcat = int(designation);
     if ( (newcat < 0) && (newcat >= MAX_FIRE_INFO) )
        newcat = 0;

     weapon_index = G_vehtype[icontype].fireinfo[newcat].munition;
     munition_type = G_vehtype[icontype].fireinfo[newcat].classtype;

     //cerr << "I am an icontype: " << icontype << " firing a "
     //     << weapon_index << " of type " << munition_type << endl;
 
     switch(munition_type){
       case BULLET_TYPE: //You shot some bullets
         weapptr->mptr = new BULLET(mid); 
         G_vehlist[vid].vehptr->setfiretime(designation,G_curtime);
         if ( G_static_data->scoring )
         {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->bullets_fired += 1;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
            }
         break;
       case BOMB_TYPE:  //Dropped a bomb
         weapptr->mptr = new BOMB(mid); 
         G_vehlist[vid].vehptr->setfiretime(designation,G_curtime);
         if ( G_static_data->scoring )
            {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->bombs_fired += 1;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         }
      break;
    case MISSILE_TYPE: //lauched a missile
      if (target_lock == NO_LOCK)
         weapptr->mptr = new MISSILE(mid); 
      else
         weapptr->mptr = new MISSILE ( mid, the_target );
      G_vehlist[vid].vehptr->setfiretime(designation,G_curtime);
      if ( G_static_data->scoring )
         {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->missiles_fired += 1;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         }
      break;
#ifndef NOSUB
    case TORPEDO_SL_TYPE:  //shot a torpedo  
      weapptr->mptr = new TORPEDO_SL(mid, G_dynamic_data->inital_torpedo_course);
      G_vehlist[vid].vehptr->setfiretime(designation,G_curtime);
      if ( G_static_data->scoring )
         {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->bombs_fired += 1;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         }
      break;
    case MISSILE_SL_TYPE:  //Dropped a torpedo  //NEW
      weapptr->mptr = new MISSILE_SL(mid, G_dynamic_data->inital_tlam_course);                           
      G_vehlist[vid].vehptr->setfiretime(designation,G_curtime);
      if ( G_static_data->scoring )
         {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->missiles_fired += 1;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         }
      break;
#endif // NOSUB
    case TORPEDO_AL_TYPE:  //Dropped a torpedo
      weapptr->mptr = new TORPEDO_AL(mid); 
      G_vehlist[vid].vehptr->setfiretime(designation,G_curtime);
      if ( G_static_data->scoring )
         {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->bombs_fired += 1;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         }
      break;
    default:   //Got me, assume a base class
      weapptr->mptr = new MUNITION(mid); 
      G_vehlist[vid].vehptr->setfiretime(designation,G_curtime);
      break;
  }
  
  weapptr->cat = munition_type;
  weapptr->id = mid;

  
  switch(munition_type){
    case BOMB_TYPE:  //Droped a bomb
      weapptr->mptr->sendfire(mid,G_vehlist[vid].vehptr,newcat,
                                    veh_posture, veh_look, 1.0f, the_target );
      break;
#ifndef NOSUB
    case TORPEDO_SL_TYPE:  //Launched a torpedo 
      weapptr->mptr->sendfire(mid,G_vehlist[vid].vehptr,newcat,
                                    veh_posture, veh_look, 20.0f, the_target );
      break;
    case MISSILE_SL_TYPE:  //Launched a sub missile 
      weapptr->mptr->sendfire(mid,G_vehlist[vid].vehptr,newcat,
                                    veh_posture, veh_look, 100.0f, the_target );
      break;
#endif // NOSUB
    case TORPEDO_AL_TYPE:  //Dropped a torpedo
      weapptr->mptr->sendfire(mid,G_vehlist[vid].vehptr,newcat,
                                    veh_posture, veh_look, 0.0f, the_target );
      break;
    case BULLET_TYPE: //You shot some bullets
      weapptr->mptr->sendfire(mid,G_vehlist[vid].vehptr,newcat,
                                    veh_posture, veh_look, 1000.0f, the_target );
      break;
    case MISSILE_TYPE: //lauched a missile
    default:   //Got me, assume a base class
      weapptr->mptr->sendfire(mid,G_vehlist[vid].vehptr,newcat,
                                    veh_posture, veh_look, 250.0f, the_target );
      break;
  }

  }
  
}

int switch_store(pfTraverser *trav, void *data)
//store the switch value so that we can blow up a building
{
  pfTraverser *temp;
  temp = trav;
  L_current_switch = ((NODEDATA *)data)->node;
//cerr <<"Traversing "<<L_current_switch<<"  "<<((NODEDATA *)data)->node <<endl;
  return(PFTRAV_CONT);
}

int buildinghit(pfHit *result)

//we hit a building, so kill it
{ 

   pfHit *temp;

  temp = result;
  if ( L_current_switch != NULL)
     {
//   cerr <<"Hit building "<< (int)L_current_switch << " " << DEAD <<endl;
     if(L_current_switch)
       {
       pfSwitchVal(L_current_switch,DEAD_MODEL);
//     cerr << "Kill this node "<< L_current_switch << endl;
       addtodeadlist(L_current_switch,G_curtime+DEAD_TIME,0L);
       }
     }
  return(PFTRAV_TERM);
}

int target_lock (pfHit *result)
   {
   long target_mask;
   pfHit *temp;
   temp = result;

#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   G_dynamic_data->target = L_veh_node;

   if ( G_vehlist[L_veh_node].vehptr != NULL )
      {
      target_mask = pfGetNodeTravMask(G_vehlist[L_veh_node].vehptr->gethull(),
                                      PFTRAV_ISECT);

      G_vehlist[L_veh_node].vehptr->bbox(TRUE);
  
      if ( int((target_mask&ENTITY_FORCE_MASK)>>ENTITY_FORCE_SHIFT) 
                == int(ForceID_Blue))
         G_dynamic_data->target_lock = BLUE_LOCK;
      else if ( int((target_mask&ENTITY_FORCE_MASK)>>ENTITY_FORCE_SHIFT) 
                == int(ForceID_Red))
         G_dynamic_data->target_lock = RED_LOCK;
      else if ( int((target_mask&ENTITY_FORCE_MASK)>>ENTITY_FORCE_SHIFT) 
                == int(ForceID_White))
         G_dynamic_data->target_lock = WHITE_LOCK;
      else if ( int((target_mask&ENTITY_FORCE_MASK)>>ENTITY_FORCE_SHIFT) 
                == int(ForceID_Other))
         G_dynamic_data->target_lock = OTHER_LOCK;
      else
         cerr << "Locked onto UNKNOWN force. ERROR!\n";
      }
   else
      G_dynamic_data->target = -1;
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
   return(PFTRAV_TERM);
   }


void addtodeadlist(pfSwitch *curnode,float alivetime,long status)
//build a link list of dead things that should be switched off in the future
{
   SWITCHNODEDATA *cur, *last;
   SWITCHNODEDATA *newnode = new SWITCHNODEDATA;
   
   //fill in the fields
   newnode->node = curnode;
   newnode->timeoff = alivetime;
   newnode->status = status;
   
   //put it in the list
   if(L_dead_list == NULL){
     //first thing on the list
     newnode->next = NULL;
     L_dead_list = newnode;   
   }
   else{ 
     if(L_dead_list->timeoff >= alivetime){
       //insert at the head of the list 
       newnode->next = L_dead_list;
       L_dead_list = newnode;
     }
     else{ 
       //insert it somewhere in the list
       last = L_dead_list;
       cur = last->next;
       while((cur != NULL) && (cur->timeoff < alivetime)){
         last = cur;
         cur = cur->next;
       }
     
       newnode->next = cur;
       last->next =  newnode; 
     }
   } 
#ifdef DEBUG
   cerr << endl;
   cur = L_dead_list;
   while (cur){
     cerr << "Pointer " << cur <<" Next "<<cur->next
          <<" Switch " << cur->node <<" Wake up " << cur->timeoff<<endl;
     cur = cur->next;
   }
#endif
}

void lazarus()
//raises buildings from the dead
{
  
  SWITCHNODEDATA *cur,*temp;
  
  if(L_dead_list){
    //there is something on the list
    cur = L_dead_list;
    while ((cur) && (cur->timeoff < G_curtime)){
      //walk the list bringing to life everything whose time as come
      pfSwitchVal(cur->node,ALIVE);
      if(cur->status & Appearance_SmokePlume_Engine){
        //it was smoking
//      turn_smoke_off(data);
      }
      temp = cur;
      cur = cur->next;
      delete temp;
    }
    L_dead_list = cur;
  }
  //set the current switch building node to NULL in preperation for next loop
  L_current_switch = NULL;
  //set the vehicle node to the driven veh
  if ( G_static_data->stealth )
    L_veh_node = 0;
  else
     L_veh_node = G_drivenveh;
  
}

void super_lazarus()
  {
  SWITCHNODEDATA *cur,*temp;
  if(L_dead_list)
     {
     cur = L_dead_list;
     while ( cur )
        {
        pfSwitchVal(cur->node,ALIVE);
        temp = cur;
        cur = cur->next;
        delete temp;
        }
     L_dead_list = cur;
     } 
  }


#ifndef NONPSSHIP
int mounthit(pfHit *)
/*the function called when a vehicle is hit by ship mounting code*/
{
   int mounted_node = L_veh_node;
//cerr<<"mounted_node:"<<mounted_node<<endl;
   if ( G_vehlist[mounted_node].vehptr == NULL )
      {
         cerr << "UNKNOWN mounted_node. ERROR!\n";
      }
   else
      {
#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
         G_dynamic_data->target = mounted_node;
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
      }
   return(PFTRAV_TERM);
}
#endif


int vehiclehit(pfHit *result)
/*the function called when a vehicle is hit by a missile*/
{
   pfHit *temp;
   temp = result;
   L_veh_hit = L_veh_node;
   if(L_veh_node != G_drivenveh)
      return(PFTRAV_TERM);
   else
      return(PFTRAV_PRUNE);
}

int hitveh()
//returns the current vehicle as pointed to the traversal pointer
{
  return(L_veh_hit);
}

int vid_store(pfTraverser *trav, void *data)
//store the switch value so that we can blow up a building
{
   pfTraverser *temp;
   temp = trav;
   if(*(int *)data == G_drivenveh){
//cerr << "Traversing "<<L_veh_node<<"  "<<*(int *)data << " Driven\n";
     return(PFTRAV_PRUNE);
   }
   else{
     //not the driven vehicle
     L_veh_node = *(int *)data;
//cerr << "Traversing "<<L_veh_node<<"  "<<*(int *)data << endl;
     return(PFTRAV_CONT);
   }
}

/*********************************************************************/
#if 0 // from SRC code
// This function is a friend to munitions class 

void
  dismount_firebuttonhit ( DIDismount *dismount, int vid, wdestype designation )
  {
  pfVec3
    firefrom = { -0.003f, 0.529f, 0.024 };  // ??? fixed coordinates
  pfMatrix
    orient;
  DIRifle
    rifle;
 
  dismount->get_rifle ( rifle );


  /* Determine the world coordinate of the end of the rifle. Play
   * a small flash at that location and the heading of the rifle.
   */
  
  pfMakeEulerMat ( orient,
                   rifle.position.hpr[HEADING],
                   rifle.position.hpr[PITCH],
                   rifle.position.hpr[ROLL]);
     
  pfXformPt3 ( firefrom, firefrom, orient );
  pfAddVec3 ( firefrom, rifle.position.xyz, firefrom );
   
 
  static int mid = -1;  //the next available slot
  WEAPON_LIST *weapptr;
  pfVec3 velocity;
  int the_target;
  mtype munition_type;
  int newcat = (int) designation;
  int icontype;

  icontype = G_vehlist[vid].type;

  dismount->get_velocity ( velocity );
  

  /* Determine if it is OK to fire. */

  if ( G_vehlist[vid].vehptr->oktofire ( designation, G_curtime ) )
    {
    /* Determine the entry into the munition table, free the munition
     * already there if not already dead.
     */
  
    mid = (mid + 1) % MAX_MUNITIONS;
    weapptr = &(G_weaplist[mid]);
 
    the_target = -1;
 
    if ( weapptr->mptr )
      {
      //the thing was not done, so explode it and then deactivate it
      //or some body was still active so, deactiveate it
 
      weapptr->mptr->senddetonation ( DetResult_None );
      deactive_weap ( mid );
 
      } /* end if */


    if ( (newcat < 0) && (newcat >= MAX_FIRE_INFO) )
      newcat = 0;

    munition_type = G_vehtype[icontype].fireinfo[newcat].classtype;
    

    G_vehlist[vid].vehptr->setfiretime ( designation, G_curtime );
    
    float speed;
 
    switch ( munition_type )
      {
      case BULLET_TYPE: //You shot some bullets
      speed = 350.0f;
      weapptr->mptr = new BULLET(mid); 
      break;
 
      case MISSILE_TYPE: //lauched a missile
      speed = 250.0f;
      weapptr->mptr = new MISSILE(mid); 
      break;
 
      default:   //Got me, assume a base class
      speed = 250.0f;
      weapptr->mptr = new MUNITION(mid); 
      break;

      }
    
    weapptr->cat = munition_type;
    weapptr->id = mid;
 
 
    FirePDU Fpdu;
//    static unsigned short event_cnt = 0;
    int weapon_index;
    EntityID id;


    id.address.site = G_static_data->DIS_address.site;
    id.address.host = G_static_data->DIS_address.host;
    id.entity = vid + 1001;

    
    bzero ( (char *) &Fpdu, sizeof ( FirePDU ) );
    
    weapptr->mptr->dcs = weapptr->dcs; 
    pfSwitchVal ( weapptr->icon, (long)designation );

    
    /*fill in the Fire, Target (Unkown), and Munition ID*/

    Fpdu.firing_entity_id.address.site = id.address.site;
    Fpdu.firing_entity_id.address.host = id.address.host;
    Fpdu.firing_entity_id.entity = id.entity;

    weapptr->mptr->firedfrom = id.entity;

    if ( munition_type == BULLET_TYPE )
      {
      /* non-tracked munition */
      Fpdu.munition_id.address.site = 0;
      Fpdu.munition_id.address.host = 0;
      Fpdu.munition_id.entity = 0;
      weapptr->mptr->disname.address.site = 0;
      weapptr->mptr->disname.address.host = 0;
      weapptr->mptr->disname.entity = 0;
      }
    else
      {
      Fpdu.munition_id.address.site = id.address.site;
      Fpdu.munition_id.address.host = id.address.host;
      Fpdu.munition_id.entity = MUNITION_OFFSET + mid; 
      weapptr->mptr->disname.address.site = id.address.site;
      weapptr->mptr->disname.address.host = id.address.host;
      weapptr->mptr->disname.entity = MUNITION_OFFSET + mid;
      }


    /* We don't know who we are firing at. */

    Fpdu.target_entity_id.address.host = 0; 
    Fpdu.target_entity_id.address.site = 0; 
    Fpdu.target_entity_id.entity = 0; 
 
  
    Fpdu.event_id.address.site = weapptr->mptr->disname.address.site;
    Fpdu.event_id.address.host = weapptr->mptr->disname.address.host;

//    Fpdu.event_id.event = ++event_cnt;

    G_weaplist[mid].eventid = Fpdu.event_id.event = ++event_cnt;
 
    weapon_index = G_vehtype[icontype].fireinfo[newcat].munition;

   
cerr << "IPORT DI is of type " << icontype << " and is firing his " 
     << endl;
cerr << "\t" << newcat << " weapon which is of type " << weapon_index << endl;


    pfCoord starting;
 
    starting = rifle.position;
//    pfCopyVec3 ( starting.xyz, firefrom );
 
    weapptr->mptr->posture = starting;
 
    dismount->get_force ( weapptr->mptr->force );
 
    Fpdu.location_in_world.x = starting.xyz[X];
    Fpdu.location_in_world.y = starting.xyz[Y];
    Fpdu.location_in_world.z = starting.xyz[Z];
   
    //setup the DCS
    pfDCSTrans(weapptr->mptr->dcs,
               starting.xyz[X],starting.xyz[Y],starting.xyz[Z]);


    int mask = PRUNE_MASK;
    set_intersect_mask((pfGroup *)weapptr->dcs,
                       mask, COLLIDE_DYNAMIC);
    pfSync();
    pfAddChild ( G_moving, (pfGroup *)weapptr->dcs );
    
    /*what did we shoot*/

    Fpdu.burst_descriptor.munition = 
      weapptr->mptr->type = G_vehtype[weapon_index].distype;
 
    switch ( munition_type )
      {
      case BULLET_TYPE:
      Fpdu.burst_descriptor.warhead = WarheadMunition_Other;
      break;

      case BOMB_TYPE:
      case MISSILE_TYPE:
      Fpdu.burst_descriptor.warhead = WarheadMunition_HighExpShapedCharg;
      break;
 
      } /* end switch */
 
    Fpdu.burst_descriptor.fuze = FuzeMunition_Contact;
    Fpdu.burst_descriptor.quantity = 1; /*One at a time*/
    Fpdu.burst_descriptor.rate = 0;
 
    weapptr->mptr->set_speed(speed);
 
    Fpdu.velocity.x = weapptr->mptr->velocity[X];
    Fpdu.velocity.y = weapptr->mptr->velocity[Y];
    Fpdu.velocity.z = weapptr->mptr->velocity[Z];
 
    if ( munition_type == MISSILE_TYPE )
      Fpdu.range = MAX_MISSLE_RANGE;
    else if ( munition_type == BULLET_TYPE )
      Fpdu.range = MAX_BULLET_RANGE;
    else
      Fpdu.range = WEAP_RANGE;

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          if (G_net->write_pdu((char *) &Fpdu, (PDUType )FirePDU_Type) == FALSE)
             cerr << "net_write() failed\n";
          break;
       }
 
 
    G_dynamic_data->identify->effect ( &weapptr->mptr->type,
    				       &weapptr->mptr->effect );


    switch ( weapptr->mptr->effect.fire_effect)  // flash from launch
      {
      case SMALL_FLASH:
      play_animation ( SMALL_FLASH, firefrom, rifle.position.hpr );
      break;

      case MEDIUM_FLASH:
      play_animation ( MEDIUM_FLASH, firefrom, rifle.position.hpr );
      break;

      case LARGE_FLASH:
      play_animation ( LARGE_FLASH, firefrom, rifle.position.hpr );
      break;

      case SMALL_GUN:
      play_animation ( SMALL_GUN, firefrom, rifle.position.hpr );
      break;

      case MEDIUM_GUN:
      play_animation ( MEDIUM_GUN, firefrom, rifle.position.hpr );
      break;

      case LARGE_GUN:
      play_animation ( LARGE_GUN, firefrom, rifle.position.hpr );
      break;

      case SMALL_SMOKE:
      play_animation ( SMALL_SMOKE, firefrom, rifle.position.hpr );
      break;

      case MEDIUM_SMOKE:
      play_animation ( MEDIUM_SMOKE, firefrom, rifle.position.hpr );
      break;

      case LARGE_SMOKE:
      play_animation ( LARGE_SMOKE, firefrom, rifle.position.hpr );
      break;

      case EXPLOSION:
      play_animation ( EXPLOSION, firefrom, rifle.position.hpr );
      break;

      case AIR_BURST:
      play_animation ( AIR_BURST, firefrom, rifle.position.hpr );
      break;

      case GROUND_HIT:
      play_animation ( GROUND_HIT, firefrom, rifle.position.hpr );
      break;

      case IMPACT:
      play_animation ( IMPACT, firefrom, rifle.position.hpr );
      break;

      default:
      break;

      } /* end switch */
    
    } /* end if */
  
  } /* end routine */

 
#endif
