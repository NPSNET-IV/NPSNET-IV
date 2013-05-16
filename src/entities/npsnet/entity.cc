// File: <entity.cc>


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

#include <stdlib.h>
#include <pf.h>
#include <assert.h>
#include <string.h>
#include <bstring.h>

#include "entity.h"

#include "common_defs2.h"
#include "appearance.h"
#include "terrain.h"
#include "netutil.h"
#include "aircraft_veh.h"
#include "fixed_wing_veh.h"
#include "ground_veh.h"
#include "jack_di_veh.h"
#include "jack_sailor_veh.h"
#include "diguy_veh.h"
#ifndef NODUDE
#include "dude.h"
#include "jade.h"
#endif // NODUDE
#include "person.h"
#include "rotary_wing_veh.h"
#include "real_rotary_wing_veh.h"
#include "ship_veh.h"
#ifndef NONPSSHIP
#include "ship_walk_veh.h"
#endif
#include "stealth_veh.h"
#include "submersible_veh.h"
#include "unmanned_veh.h"
#include "interface_const.h"
#include "input_manager.h"


// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile STATIC_DATA     *G_static_data;
extern volatile int              G_drivenveh;
extern          VEH_LIST        *G_vehlist;
extern          VEHTYPE_LIST    *G_vehtype;
extern          EntityID         G_my_entity_id;
extern          float            G_filter_range;
#ifdef JACK
extern int G_runisms;    
extern int G_num_soldiers;
#endif

static unsigned short L_entity_counter = 1001;


int load_models(ifstream *)
   {
   return 1;
   }


void initialize_vehicle_arrays()
{
   int index;
   static pfVec3 zerovec = { 0.0f, 0.0f, 0.0f };
   int i;

   //allocate memory for the arrays
   G_vehlist = new VEH_LIST [MAX_VEH];
   G_vehtype = new VEHTYPE_LIST [MAX_VEHTYPES];
   
   //get some work pointers
   VEH_LIST *current_veh = &(G_vehlist[0]);
   VEHTYPE_LIST *current_type = &(G_vehtype[0]);
 
   // nobody is in the world yet
   for (index=0;index<MAX_VEH;index++) {
      current_veh->id = current_veh->type = NOSHOW;
      current_veh->cat = VEH_TYPE;
      current_veh->vehptr = NULL;
      current_veh++;
   }

   //load the types array, we don't have any vehicles yet
   for (index=0;index<MAX_VEHTYPES;index++) {
      current_type->id = NOSHOW;
      current_type->model = NULL;
      current_type->num_joints = 0;
      current_type->numclones = 0;
      current_type->num_weapons = 0;

      for ( i = 0; i < MAXJOINTS; i++ ) {
         strcpy ( current_type->joints[i].name, "" );
         current_type->joints[i].dis_id = 0;
         current_type->joints[i].dcs = NULL;
         current_type->joints[i].min_range.xyz[PF_X] = 0.0f;
         current_type->joints[i].min_range.xyz[PF_Y] = 0.0f;
         current_type->joints[i].min_range.xyz[PF_Z] = 0.0f;
         current_type->joints[i].min_range.hpr[PF_H] = 0.0f;
         current_type->joints[i].min_range.hpr[PF_P] = 0.0f;
         current_type->joints[i].min_range.hpr[PF_R] = 0.0f;
         current_type->joints[i].max_range.xyz[PF_X] = 0.0f;
         current_type->joints[i].max_range.xyz[PF_Y] = 0.0f;
         current_type->joints[i].max_range.xyz[PF_Z] = 0.0f;
         current_type->joints[i].max_range.hpr[PF_H] = 0.0f;
         current_type->joints[i].max_range.hpr[PF_P] = 0.0f;
         current_type->joints[i].max_range.hpr[PF_R] = 0.0f;
         current_type->joints[i].current_setting.xyz[PF_X] = 0.0f;
         current_type->joints[i].current_setting.xyz[PF_Y] = 0.0f;
         current_type->joints[i].current_setting.xyz[PF_Z] = 0.0f;
         current_type->joints[i].current_setting.hpr[PF_H] = 0.0f;
         current_type->joints[i].current_setting.hpr[PF_P] = 0.0f;
         current_type->joints[i].current_setting.hpr[PF_R] = 0.0f;
         bzero ( &(current_type->joints[i].articulate),
                 6*sizeof(unsigned char) );
      }

      for ( i = 0; i < MAX_FIRE_INFO; i++ ) {
         current_type->fireinfo[i].munition = D_MUNITION_INDEX+i;
         current_type->fireinfo[i].classtype = BULLET_TYPE;
         current_type->fireinfo[i].firedelay = 1.0f;
         pfCopyVec3 ( current_type->fireinfo[i].firepoint, zerovec );
      }
      current_type++;
   }
}


void init_drivenveh( int vid, SimulationAddress *sitehost )
{
   static pfVec3 no_velocity = {0.0f,0.0f,0.0f};
   EntityID entity_id;
   int type;
   pfCoord start_posture;

   entity_id.address = *sitehost;
   entity_id.entity = L_entity_counter++;

   type = find_type_from_name((char *)G_static_data->driven_file);
   create_new_entity ( LOCAL_CTL, vid, entity_id,
                       type, G_dynamic_data->force );
   pfCopyVec3 ( start_posture.xyz,
                (float *)G_static_data->init_location.xyz );
   pfCopyVec3 ( start_posture.hpr,
                (float *)G_static_data->init_location.hpr );
   G_vehlist[vid].vehptr->init_posture(start_posture);
   G_vehlist[vid].vehptr->transport ( start_posture.xyz[PF_X],
                                      start_posture.xyz[PF_Y],
                                      start_posture.xyz[PF_Z] );
   G_vehlist[vid].id = vid;
   G_vehlist[vid].vehptr->init_velocity(no_velocity);
   G_vehlist[vid].vehptr-> set_input_control(G_dynamic_data->control);

   //Don't draw  driven vehicle model -- except DIs  
   if (G_vehlist[vid].vehptr->getCat() != PERSON)  {
#ifndef NONPSSHIP
      if (G_vehlist[vid].vehptr->getCat() == SHIP) {
         // turn targetting on automatically 
         G_dynamic_data->targetting = TRUE;
      }
      else
#endif
         G_vehlist[vid].vehptr->turnoff();
   }
   else {
           CHANNEL_DATA *chan;
           chan = (CHANNEL_DATA *)G_dynamic_data->pipe[0].channel;

           if (G_vehlist[vid].vehptr->getCat() == PERSON) {
              // move distances closer in for humans
              G_dynamic_data->birdsdis = 10.0f;
              if(chan->clip_plane.near !=  0.1f) {
                 chan->clip_plane.near = 0.1f;
                 pfChanNearFar(chan->chan_ptr, chan->clip_plane.near,
                     chan->clip_plane.far);
              }      
           }

           // turn targetting on automatically 
           G_dynamic_data->targetting = TRUE;
        }
//cerr<<"done init_drvveh"<<endl;


}


void deactivate_drivenveh ( int vid )
{
   if ( (vid != -1) && (G_vehlist[vid].vehptr != NULL) ) 
      {
#ifdef DIS_2v4
      unsigned int entity_disappear = 
         G_vehlist[vid].vehptr->getstatus();
      entity_disappear |= Appearance_Platform_Deactivated;
      G_vehlist[vid].vehptr->changestatus(entity_disappear);
      G_vehlist[vid].vehptr->sendentitystate();
#endif
      deactiveate__entity(vid);
      }
   vid = -1;
}


int find_type_from_name(char *modname)
//find out what NPSNET type the models is based upon the name
{ 
  VEHTYPE_LIST *current = &G_vehtype[0];
  for (int index=0;index<MAX_VEHTYPES;index++)
  {
     if (!strcmp(current->name,modname)){
#ifdef DEBUG  
   cerr << "Type found from name - " << current->id << endl;
#endif

        return current->id;
   }
     current++;
  }
  cerr << "\tFIND_TYPE_FROM_NAME:" << endl;
  if ( modname != NULL )
     cerr << "\t\t" << modname << " model not found, using default vehicle.\n";
  else
     cerr << "\t\tmodname parameter is NULL!" << endl;
  return 0;     
}


void create_new_entity ( entity_control controller, int vid, EntityID entity_id,
                         int type, ForceID force )
// Based on the class category of the new entity, create the correctly
//    classed entity.
{
   VEH_LIST *varr = &G_vehlist[vid];
   VEHTYPE_LIST *vtypea = &G_vehtype[type];

   //Update the G_vehlist values for this entity
   varr->id   = vid;
   varr->type = type;
   varr->control = controller;
   varr->cat  = vtypea->cat;
   varr->DISid = entity_id;
//cerr<<"creating new entity: "<<vid<<" "<<type<<endl;
   if (varr->vehptr != NULL) {
      cerr << "create_new_entity:\tDestroying old entity at array position "
           << vid << endl;
      deactiveate__entity(vid);
   }

   switch(vtypea->cat) {
      case FLYING_CARPET:
           varr->vehptr = new STEALTH_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new stealth vehicle." << endl;;
#endif
           break;
      case GND_VEH: // Ground Vehciles
           varr->vehptr = new GROUND_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new ground vehicle." << endl;;
#endif
           break;
      case AIR_VEH:  //Air Vehicles and most munitions
           varr->vehptr = new AIRCRAFT_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new air vehicle." << endl;;
#endif
           break;
      case FIXED_WING:// Fixed wing Aircraft
           varr->vehptr = new FIXED_WING_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new fixed wing vehicle." << endl;;
#endif
           break;
      case PB_ROTARY_WING: // Physically based helio wop wops
           varr->vehptr = new REAL_ROTARY_WING_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new PB rotary wing vehicle." << endl;;
#endif
           break;
      case ROTARY_WING: //Helio wop wops
           varr->vehptr = new ROTARY_WING_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new rotary wing vehicle." << endl;;
#endif
           break;
      case SHIP: //Squid carriers
#ifndef NONPSSHIP
           //cerr<<"vtypea->name:"<<vtypea->name<<endl;
           if(strstr(vtypea->name,"antares") != NULL) 
           {
               varr->vehptr = new SHIP_WALK_VEH(vid,type,force);
#ifdef DEBUG_CLASS
               cerr<<"\tJust created new ship_walk vehicle."<<endl;
#endif
           }
           else 
#endif
           {
               varr->vehptr = new SHIP_VEH(vid,type,force);
#ifdef DEBUG_CLASS
               cerr << "\tJust created new ship vehicle." << endl;;
#endif
           }
           break;
      case SUBMERSIBLE: //Smart Squid vehicle
           varr->vehptr = new SUBMERSIBLE_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new submersible vehicle." << endl;;
#endif
           break;
      case PERSON:
           varr->vehptr = new PERSON_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new person vehicle." << endl;;
#endif
           break;
      case UNMANNED:
           varr->vehptr = new UNMANNED_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new UAV vehicle." << endl;;
#endif
           break;
      case JACK_DI:
           if(G_num_soldiers < 50) 
           {
#ifndef NONPSSHIP
              if(strstr(vtypea->name,"sailor") != NULL) 
              {
                  varr->vehptr = new JACK_SAILOR_VEH(vid,type,force);
#ifdef DEBUG_CLASS
                  cerr<<"\tJust created new Jack Sailor vehicle."<<endl;
#endif
              }
              else 
#endif
              {
                  varr->vehptr = new JACK_DI_VEH(vid,type,force);
#ifdef DEBUG_CLASS
                  cerr << "\tJust created new Jack DI vehicle." << endl;;
#endif
              }
           }
           break;
     case JACK_TEAM:
          varr->vehptr = new JACK_TEAM_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new Jack Team vehicle." << endl;;
#endif
          break;
     case DI_GUY:
           varr->vehptr = new DIGUY_VEH(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new diguy  vehicle." << endl;;
#endif
          break;
#ifndef NODUDE
     case DUDE:
           varr->vehptr = new DUDECLASS(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new dude vehicle." << endl;;
#endif
          break;
     case JADE:
           varr->vehptr = new JADECLASS(vid,type,force);
           cerr << "\tJust created new jade vehicle." << endl;;
#ifdef DEBUG_CLASS
           cerr << "\tJust created new jade vehicle." << endl;;
#endif
          break;
#endif // NODUDE      case VEH_TYPE:  //default vehicle
      default:
           varr->vehptr = new VEHICLE(vid,type,force);
#ifdef DEBUG_CLASS
           cerr << "\tJust created new default vehicle." << endl;;
#endif
           break;
   }


}


//move all of the entites currently in the vehicle array
void
  update_entities(PASS_DATA *pdata,CONTROL &cdata)
  {
  movestat vehmovestat;
  VEH_LIST *current = &G_vehlist[0];
  HUD_VEH_PTR hud_vehicles = NULL; 
  pfVec3 location;
  unsigned int veh_status;
  
  // Set the first space in the array to NOSHOW incase there are no veh
  pdata->hud_data[0].type = NOSHOW;

  // Loop through the vehicle array, and dead recon all vehicles.
  // If our dead-recon position differs from the actual, a PDU is sent
  for ( int ix = 0; ix < MAX_VEH; ix++ )
    {
    if ( current->vehptr != NULL )
      //is it an active entity
      {
#ifdef DEBUG
      cerr << "Veh index "<< ix << " cat = " << current->cat << endl; 
#endif

      if ( current->control == LOCAL_CTL ) {
         if(current->vehptr->getControl() != IPORT) {
           // Move the driven vehicle for the current time interval
           // otherwise Places model in final resting place and tracks dead time
     
           if ((G_dynamic_data->dead > ALIVE ) && (!G_static_data->stealth)) {
             vehmovestat = current->vehptr->movedead();
           }
           else {
             vehmovestat = current->vehptr->move();
           }
     
           current->vehptr->fill_in_pass_data(pdata);
           G_dynamic_data->tethered =
              current->vehptr->get_tether_mode();
           G_dynamic_data->paused =
              current->vehptr->get_paused_mode();
           G_dynamic_data->targetting =
              current->vehptr->get_targetting_mode();
     
     
        }
     
         // Get the new location of the driven vehicle for distance calculations
         pfCopyVec3(location,(current->vehptr->getposition()));

      }

#ifdef DIS_2v4
      veh_status = ((current->vehptr->getstatus()) & Appearance_Platform_Status_Mask);
#endif


      // We only want to work on non-I-port vehicles.
      
      if( current->vehptr->getControl() != IPORT)
        {
#ifdef DIS_2v4
         if ( (veh_status & Appearance_Platform_Frozen) == 0 )
#endif
         vehmovestat = current->vehptr->moveDR(cdata.delta_time, cdata.current_time);
         }
      else
         vehmovestat = FINEWITHME;

#ifdef DIS_2v4
      if ( (vehmovestat != DEACTIVE) && ((veh_status & Appearance_Platform_Deactivated) != 0) )
         {
         vehmovestat = DEACTIVE;
         }
#endif

#ifdef DEBUG
      if ( vehmovestat == DEACTIVE )
        cerr << "Vehicle " << ix << " move stat " << vehmovestat << "\n";
#endif

      if ( (vehmovestat == DEACTIVE) && (ix!=G_drivenveh) )
        {
        //we have to deactivate the vehicle
        deactiveate__entity(ix);

#ifdef DEBUG
        cerr << "Vehicle " << ix << " has been deactivated\n";
#endif
        
        } /* end if */
      else
      { 

         // Build a linked list of the vehicles so they will be in order
         // of distance from our position.
         if ((ix != G_drivenveh) && (current->vehptr != NULL)) 
         {      

#ifdef TRACE
             cerr << "\ngoing to 'build_hud_data'\n";
#endif

             build_hud_data(hud_vehicles,current,&location);
          }
       }
 
      } /* end if */
    
    current ++;       

    } /* end for */
#ifdef TRACE
  cerr << "going to 'update_pdata'\n";
#endif
      
#ifndef NONPSSHIP
  // This is put hear for updating pdata after mounted vehicle moves
  current = &G_vehlist[0];
  current->vehptr->fill_in_pass_data(pdata);
#endif

  // Copy the closest 'HUD_MAX_VEH' into the pdata->hud_data
  update_pdata(hud_vehicles, pdata->hud_data);  

  } /* end routine */




#ifndef NONPSSHIP
//Update Locators of all ship_walk_veh entites currently in the vehicle array
void update_locators()
{
   VEH_LIST *varr;
   //VEHTYPE_LIST *vtypea;

   // Loop through the vehicle array, and update locators of ship_walk_vehs
   for ( int ix = 0; ix < MAX_VEH; ix++ ) {
      varr = &G_vehlist[ix];
      if(varr->vehptr != NULL) {
         //vtypea = &G_vehtype[varr->type];
         //if(vtypea->cat == SHIP && strstr(vtypea->name,"antares") != NULL ) {
         if (varr->vehptr->gettype() == SHIP) {
            if (((SHIP_VEH *)varr->vehptr)->is_WALK_CLASS()) {
              ((SHIP_WALK_VEH *)varr->vehptr)->update_locator();
            }
         }
      }
   }  
}  /* end update_locators */
#endif



#ifndef NONPSSHIP
//Update the picking mechanisms of all ship_walk_veh entites in vehicle array
void update_ship_picking()
{
   VEH_LIST *varr;
   //VEHTYPE_LIST *vtypea;

   // Loop through the vehicle array, and update picking of ship_walk_vehs
   for ( int ix = 0; ix < MAX_VEH; ix++ ) {
      varr = &G_vehlist[ix];
      if(varr->vehptr != NULL) {
         //vtypea = &G_vehtype[varr->type];
         //if(vtypea->cat == SHIP && strstr(vtypea->name,"antares") != NULL ) {
         if (varr->vehptr->gettype() == SHIP) {
            if (((SHIP_VEH *)varr->vehptr)->is_WALK_CLASS()) {
              ((SHIP_WALK_VEH *)varr->vehptr)->update_ship_picking();
            }
         }
      }
   }  

} // update_ship_picking
#endif



void build_hud_data(HUD_VEH_PTR &hud_vehicles, VEH_LIST *cur_veh, pfVec3 *loc_driven)
{
   HUD_VEH_PTR current = hud_vehicles;
   HUD_VEH_PTR previous = NULL;
   float *cur_veh_loc;
   float dist;


#ifdef TRACE
   cerr << "Starting build_hud_data.\n";
#endif
   
   cur_veh_loc = cur_veh->vehptr->getposition();
   dist  = pfDistancePt3(*loc_driven,cur_veh_loc);

   if(cur_veh->vehptr->getCat() == PERSON) {

      PERSON_VEH *person;
      person = (PERSON_VEH *)cur_veh->vehptr;

      if(dist < G_filter_range/4.0f ) {
        person->immed_transition_off();// smooth position changes
      }
      else if(dist >= G_filter_range/4.0f ) {
        person->immed_transition_on(); // snap position changes
      }

      if(dist < G_filter_range/2.0f ) {
        person->quiet_off();// joint updates
      }
      else if(dist >= G_filter_range/2.0f ) {
        person->quiet_on(); // NO joint updates
      }

   }

   if ( dist < G_filter_range )
     {
     cur_veh->vehptr->snap_on();
     }
   else
     {
     cur_veh->vehptr->snap_off();
     }

   if (!current)  // The first vehicle to be added
   {  
      hud_vehicles = (HUD_VEH_PTR)malloc(sizeof(HUD_VEH));
      hud_vehicles->vehptr = cur_veh;
      hud_vehicles->distance = dist;
      hud_vehicles->next = NULL;
   }
   else 
   {
     while (current && (current->distance < dist))
     {
        previous = current;       
        current = current->next;             
     }

     HUD_VEH_PTR temp_node = (HUD_VEH_PTR)malloc(sizeof(HUD_VEH));
     if (!temp_node) 
     {
        cerr << "Could not allocate memory, hud will not be updated. \n";
        return;
     }
     temp_node->next = current;

     if (current == hud_vehicles)
        hud_vehicles = temp_node;
     else      
        previous->next = temp_node;
     temp_node->vehptr = cur_veh;
     temp_node->distance = dist;
   }
#ifdef TRACE
   cerr << "Ending build_hud_data.\n";
#endif

}


void update_pdata(HUD_VEH_PTR vehlist,HUD_VEH_REC *hud_data)
// Copy the closest vehicles into the PASS_DATA array for display
{
   HUD_VEH_PTR current;
   HUD_VEH_REC *v_rec = hud_data;
   float *pos;
   int index = 0;

   if(vehlist == NULL){
     //the list is empty
     return;
   };

   while (index<HUD_MAX_VEH && vehlist) 
   // Fill the list for displaying vehicles on the HUD
   {
      current = vehlist;
      vehlist = vehlist->next;
      pos = current->vehptr->vehptr->getposition();  
      v_rec->location[X] = *(pos+X);
      v_rec->location[Y] = *(pos+Y);
      v_rec->elevation = *(pos+Z);
      v_rec->force = current->vehptr->vehptr->getforce();

#ifdef DEBUG
   cerr << "Vehicle X: " << *(pos+X) << " Vehicle Y: " << *(pos+Y) << endl;
#endif

      v_rec->vid = current->vehptr->id;
      v_rec->type = current->vehptr->type; 
      v_rec->status = current->vehptr->vehptr->getstatus();

      delete current;
      v_rec++;
      index++;
   }
   //set the end of the pass data list to NOSHOW
   if (index <HUD_MAX_VEH) 
       v_rec->type = NOSHOW;

   while (vehlist)
   // Delete rest of list beyond the ones to be displayed
   {
      current = vehlist;
      vehlist = vehlist->next;
      delete current;
   }

#ifdef TRACE
   cerr << "Updated the hud vehicle array\n";
#endif
} 


void deactiveate__entity(int vid)
//get rid of the puppy
{
  if(G_vehlist[vid].vehptr)
    {
    delete G_vehlist[vid].vehptr;
    G_vehlist[vid].vehptr = NULL;
    G_vehlist[vid].id = NOSHOW;
    G_vehlist[vid].cat = VEH_TYPE;
    if ( vid != G_drivenveh )
       remove_vid_from_table(&(G_vehlist[vid].DISid));
    }
}


int at_a_boundary(pfVec3 &pos)
//used to keep entities inside the boundary
{
   int status = FALSE;
   float bbox_xmin;
   float bbox_xmax;
   float bbox_ymin;
   float bbox_ymax;
#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   bbox_xmin = G_dynamic_data->bounding_box.xmin;
   bbox_xmax = G_dynamic_data->bounding_box.xmax;
   bbox_ymin = G_dynamic_data->bounding_box.ymin;
   bbox_ymax = G_dynamic_data->bounding_box.ymax;
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
   if (pos[X] < bbox_xmin)
      {
      pos[X] = bbox_xmin + 1.0f;
      status = TRUE;
      }
   else if (pos[X] > bbox_xmax)
           {
           pos[X] = bbox_xmax - 1.0f;
           status = TRUE;
           }

   if (pos[Y] < bbox_ymin)
      {
      pos[Y] = bbox_ymin + 1.0f;
      status = TRUE;
      }
   else if (pos[Y] > bbox_ymax)
           {
           pos[Y] = bbox_ymax - 1.0f;
           status = TRUE;
           }

//   if (pos[Z] > G_dynamic_data->max_alt)
//      pos[Z] = G_dynamic_data->max_alt;

   return status;
}

