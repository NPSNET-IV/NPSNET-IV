// File: <netutil.cc>

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
#include <string.h>
#include <bstring.h>
#include <pf.h>
#include <pfdb/pfflt.h>
#include <pfdu.h>
#include <pfutil.h>

#include "common_defs2.h"
#include "netutil.h"
#include "macros.h"
#include "munitions.h"
#include "fltcallbacks.h"
#include "effects.h"
#include "nps_smoke.h"
#include "terrain.h"
#include "sound.h"
#include "entity.h"

#include "appearance.h"
#include "fileio.h"
#include "jack_di_veh.h"
#ifndef NODUDE
#include "dude_funcs.h"
#include "jade.h"
#endif
#include "diguy_veh.h"

#include "imstructs.h"
#include "imclass.h"
#include "imdefaults.h"

#define NONE -1

#ifdef JACK
// #include "motion.h"
#include "jack.h"
#ifndef NONPSSHIP
#include "object.h"
#include "ship_walk_veh.h"
#endif


extern int G_runjack;
#endif

#include "collision_const.h"
#include "local_snd_code.h"

#ifndef NO_DVW
#define DVW_NPSNET
#define _CSSM
#define _DVW_VERBOSE_
#define _DVW_MULTI_TASKS_
#define _CC_COMPILER_
#define PERFORMER1_2
#include "cloud.h"
//extern "C" void processEnvironmentalPDU(char *);
extern "C" void processEnvironmentalPDU(EnvironmentalPDU *);
#endif
  
#define DEATH_DIST_SQR 1000.0f
#define MAX_PDU_FILTERS 15
#define MAX_JACK_TEXTURES 10
#ifndef NODUDE
#define MAX_DUDE_TEXTURES 2
#endif // NODUDE

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          VEH_LIST     *G_vehlist;
extern          VEHTYPE_LIST *G_vehtype;
extern volatile int           G_drivenveh;
extern          pfGroup      *G_fixed;
#ifndef NOHIRESNET
extern volatile float         G_curtime;
#endif // NOHIRESNET

extern volatile loading_phase FLT_load_phase;

extern void exitroutine();

extern InitManager *initman;

int NU_mod_num; /* shared with fltcallbacks.h */

enum filter_action { NO_ACTION, BLOCK, FORCE_APPEARANCE };

typedef struct {
   EntityID entity_id;
   filter_action action;
   u_int mask;
   u_int override;
   pfVec3 position;
   float range;
} filter_entry;

static filter_entry L_filter[MAX_PDU_FILTERS];
static int L_num_filters;

#ifdef JACK
static pfTexture *jack_textures[MAX_JACK_TEXTURES];
#endif

#ifndef NODUDE
static pfTexture *dude_textures[MAX_DUDE_TEXTURES];
#endif // NODUDE


//*************  For development purposes ************
//#define DEBUG_Z

int find_vehicle ( EntityID *, int *, unsigned int *);

extern "C"
   {
   int gethostname ( char *, int );
   }


//int read_filter_file ( char *filterfilename )
int read_filter_file ()
{
   struct FIL im_filters[MAX_PDU_FILTERS];
   int num_filters;

   initman->get_filters((struct FIL**) im_filters, &num_filters);
   L_num_filters = num_filters;

   for (int i = 0, j = 0; i < num_filters; i++) {
      if (im_filters[i].filter_type == TYPE_BLOCK) {
         L_filter[j].entity_id.address.site =
            (ushort)im_filters[i].Type.Block.site;
         L_filter[j].entity_id.address.host =
            (ushort)im_filters[i].Type.Block.host;
         L_filter[j].entity_id.entity =
            (ushort)im_filters[i].Type.Block.entity;
         L_filter[j].action = BLOCK;
         L_filter[j].mask = (u_int)0;
         L_filter[j].override = (u_int)0;
         j++;
      }
      else if (im_filters[i].filter_type == TYPE_FORCE) {
         L_filter[j].entity_id.address.site = (u_short)0;
         L_filter[j].entity_id.address.host = (u_short)0;
         L_filter[j].entity_id.entity = (u_short)0;
         L_filter[j].action = FORCE_APPEARANCE;
         L_filter[j].mask = (u_int)im_filters[i].Type.Force.and_mask;
         L_filter[j].override = (u_int)im_filters[i].Type.Force.or_mask;
         L_filter[j].range = (float)im_filters[i].Type.Force.range;
         L_filter[j].position[X] = (float)im_filters[i].Type.Force.x;
         L_filter[j].position[Y] = (float)im_filters[i].Type.Force.y;
         L_filter[j].position[Z] = (float)im_filters[i].Type.Force.z;
         j++;
      }
   }

/* This is the old code
   for (int i = 0, j = 0; i < num_filters; i++) {
      if (im_filters[i].filter_type == TYPE_BLOCK) {
         L_filter[j].entity_id.address.site = im_filters[i].Type.Block.site;
         L_filter[j].entity_id.address.host = im_filters[i].Type.Block.host;
         L_filter[j].entity_id.entity = im_filters[i].Type.Block.entity;
         j++;
      }
   }
*/
   return (j);
}


int filterPDU ( EntityStatePDU *header )
{
   int filter_count = 0;
   int found = FALSE;
   int do_appearance = FALSE;
  
   while ( (filter_count < L_num_filters) && (!found) ) {
      if ( ((L_filter[filter_count].entity_id.address.site == 0)||
            (L_filter[filter_count].entity_id.address.site ==
             header->entity_id.address.site)) &&
           ((L_filter[filter_count].entity_id.address.host == 0)||
            (L_filter[filter_count].entity_id.address.host ==
             header->entity_id.address.host)) &&
           ((L_filter[filter_count].entity_id.entity == 0)||
            (L_filter[filter_count].entity_id.entity ==
             header->entity_id.entity)) )
         found = TRUE; 
      else
         filter_count++;
   }
   if (found) {
      if ( L_filter[filter_count].action == FORCE_APPEARANCE ) {
         if (L_filter[filter_count].range < 0) {
            do_appearance = TRUE;
         }
         else {
            static pfVec3 entity_pos;
            static float distance;
            pfSetVec3 ( entity_pos, 
                        (float)header->entity_location.x,
                        (float)header->entity_location.y,
                        (float)header->entity_location.z);
            distance = PFDISTANCE_PT3(L_filter[filter_count].position,
                                      entity_pos );
//cerr << "Checking entity against range." << endl;
            if ( distance < L_filter[filter_count].range )
               do_appearance = TRUE;
         }
        
         if ( do_appearance ) { 
            header->entity_appearance = 
               header->entity_appearance & L_filter[filter_count].mask;
            header->entity_appearance = 
               header->entity_appearance | L_filter[filter_count].override;
         }
         found = FALSE;
      }
   }
   return (found);
}


VIDTABLETYPE vidtable[VIDTABLETYPESIZE]; //vehicle id hash table
int currvehcnt = 0; //current number of vehicles

VTYPENODE *L_typeroot; // the root of the type tree


int remove_vid_from_table(EntityID *netid)
//once a entity is deactivated it should be removed from the table
//TRUE if it is removed, FALSE of it doesn't find it
{

  unsigned int icompkey;   //the index into the hash table
  int vid;


  if ( find_vehicle ( netid, &vid, &icompkey ) )
     {
     vidtable[icompkey].netid.entity = 0;
     vidtable[icompkey].netid.address.host = 0;
     vidtable[icompkey].netid.address.site = 0;
     vidtable[icompkey].vid = AVAIL;
     currvehcnt--;
     if ( currvehcnt < 0 )
        cerr << "Remove vehicle -- Number of vehicle underflow.\n";
     return TRUE;
     }
  else
     {
     cerr << "Tried to remove non-existant vehicle from hash table: "
          << netid->address.site << ", "
          << netid->address.host << ", "
          << netid->entity << endl;
     return FALSE;
     }
}

int find_vehicle ( EntityID *netid, int *vid, unsigned int *icompkey )
   {
   int counter = 0;
 
   //compute the hash key

   *icompkey = ((netid->address.site << 24) |
                (netid->address.host << 16) |
                (netid->entity));
   *icompkey %= VIDTABLETYPESIZE;
   
   while ((vidtable[*icompkey].vid != EMPTY) && (counter < VIDTABLETYPESIZE))
     {
     counter++;
     //this spot was not empty, does it match what we passed in?
     if((vidtable[*icompkey].vid != AVAIL) &&
        (vidtable[*icompkey].netid.entity == netid->entity) &&
        (vidtable[*icompkey].netid.address.host == netid->address.host) &&
        (vidtable[*icompkey].netid.address.site == netid->address.site) &&
        (G_vehlist[vidtable[*icompkey].vid].type != NOSHOW))
       {
       //we found a match
       *vid = vidtable[*icompkey].vid;
       return TRUE;
       } 
     *icompkey = (*icompkey + 1) % VIDTABLETYPESIZE;
     
     }
   return FALSE;
   }

int vid_from_net( EntityID *netid,int *vid)
//get the vehcile id from the table
//Evaluates to TRUE and returns a vid if it was in the table
//             FALSE              vid          not
//             FALSE              NOSHOW if there was no space in the table
{
  unsigned int icompkey;   //the index into the hash table
  int ix;
  
  if ( find_vehicle ( netid, vid, &icompkey ) )
     return TRUE;

  if(currvehcnt >= MAX_VEH){
    //at the max number of vehicles
    *vid = NOSHOW;
    cerr << "ERROR:\tMaximum number of vehicles exceeded.\n";
    return(FALSE);
  }

  //if we are here, this must be a new entity

  //get a vehicle id, 0 is always the driven vehicle
  for(ix=0;ix<MAX_VEH;ix++){
    if(G_vehlist[ix].vehptr == NULL){
      //found a slot
      vidtable[icompkey].netid = *netid;
      vidtable[icompkey].vid = ix;
      *vid = ix;
      currvehcnt++;
      return(FALSE);
    }
   }
  
  //should never get here
  cerr << "ERROR:\tDid not find a slot in the G_vehlist array" << endl;
  *vid = NOSHOW;
  return(FALSE);
         
}

void setup_entity_net_table()
//intialize the table
{
  int ix;
  
  //clean it out
  for (ix = 0; ix<VIDTABLETYPESIZE;ix++){
    vidtable[ix].vid = EMPTY;
  }
}

EntityID *get_netid(int npsid)
//get the DIS network id for a specific vid
{
    if(G_vehlist[npsid].vehptr){
      //valid vid
      return(&(G_vehlist[npsid].DISid));
    }   
    else{
      return(NULL);
    }
}

//void setup_type_table(char filename[])
void setup_type_table()
   // Set up the type table, read in the models
{
   int          modtype;
   char		modname[80],moddir[80]; 
#ifdef JACK 
   char		jackmodel[80]; 
   JointInfo	jack_jointInfo[MAX_JOINTS];
   int		jack_jointCount;
   FigureInfo	jack_figureInfo[MAX_FIGURES];
   int		jack_figureCount;
#endif
   char		mfile[200];
   char		nodename[200];
   static int	alive;
   int		moddead, ix, num_textures, i;
   EntityType	intype;
   VTYPENODE	*insert_node;
   unsigned int	entity_kind,domain,country,category,subcategory,specific,cat;
   static int	munition_num = 0;
   int		munition_index;
   int		munition_class;
   float	fireX, fireY, fireZ;
   float	delaytime;
   long		entity_mask;

#ifndef NODUDE
   pfSwitch *whichSwitch[2];  // temp pfSwitch ptrs
   int whichIndex = 0;
#endif // NODUDE

   struct Dyn	im_models[MAX_VEHTYPES];
   int		num_models;

   initman->get_dynamic_objs((struct Dyn **) im_models, &num_models);

   NU_mod_num = 0;

   // Clean out the array
   for (ix = 0; ix < MAX_VEHTYPES; ix++) {
      G_vehtype[ix].id = NOSHOW;
   }

   for (i = 0; i < num_models; i++) {		// D/H lines
      munition_num = 0;
      modtype = im_models[i].mtype;
      strncpy(modname, im_models[i].filename, 80);
      strncpy(moddir, im_models[i].path, 80);
      alive = im_models[i].alive_mod;
      moddead = im_models[i].dead_mod;
      cat = im_models[i].vtype;
      entity_kind = im_models[i].etype.entity_kind;
      domain = im_models[i].etype.domain;
      country = im_models[i].etype.country;
      category = im_models[i].etype.category;
      subcategory = im_models[i].etype.subcategory;
      specific = im_models[i].etype.specific;
      G_vehtype[alive].modtype = modtype;

      if (modtype == TYPE_TIPS) {
#ifdef JACK
         strncpy(jackmodel, im_models[i].jack_model, 64);
         sprintf(mfile, "./%s/%s.tips", moddir, jackmodel);
#else
         cerr << "ERROR: Jack is not implemented." << endl;
#endif
      }
      else if (modtype == TYPE_FLT) {
         sprintf(mfile, "./%s/%s.flt", moddir, modname);
      }
      else if (modtype == TYPE_JCD) {
#ifndef NODUDE
         sprintf(mfile,"./%s/%s.jcd",moddir,modname);
#else
         cerr << "ERROR: Dude is not implemented." << endl;
#endif // NODUDE
      }
      else if (modtype == TYPE_JADE) {
#ifdef NODUDE
         cerr << "ERROR: Dude is not implemented." << endl;
#endif // NODUDE
      }

      NU_mod_num = alive;

      if (G_static_data->load_dynamic_models ||
         (!G_static_data->load_dynamic_models && (alive == 0))) {
         // force the puppys to the right types
  FLT_load_phase = DYNAMIC_MODELS;

         intype.entity_kind = (unsigned char) entity_kind;
         intype.domain = (unsigned char) domain;
         intype.country = (unsigned short) country;
         intype.category = (unsigned char) category;
         intype.subcategory = (unsigned char) subcategory;
         intype.specific = (unsigned char) specific;

         G_vehtype[alive].id = alive;
         G_vehtype[alive].dead = moddead;
         bcopy ((char *) &intype, (char *) &G_vehtype[alive].distype,
                 sizeof(EntityType));
         strcpy(G_vehtype[alive].name, modname);
         G_vehtype[alive].cat = (vtype) cat;

         if(modtype == TYPE_FLT) {

#ifndef NONPSSHIP

            if(strstr(modname,"antares") != NULL) {

               //**************************************************************
               //                       Load Antares Model
               //**************************************************************
               G_vehtype[alive].model = openFlightConfig(mfile);

               fltRegisterNodeT pFunc =  myregisterfunc;
               void *hFunc            = &pFunc;
               pfdConverterAttr_flt (PFFLT_REGISTER_NODE, hFunc);

            }
            else
#endif

            G_vehtype[alive].model = (pfGroup *) pfdLoadFile(mfile);
         } 
         else if (modtype == TYPE_TIPS && G_runjack) {
            G_vehtype[alive].model =
                    (pfGroup *) loadTIPSFile(mfile, &num_textures, TRUE,
                                     &jack_jointCount,
                                     &jack_jointInfo[0],
                                     &jack_figureCount,
                                     &jack_figureInfo[0],
				     jack_textures[0]);

            if (G_vehtype[alive].model == NULL) {
               cerr << "ERROR loading TIPS file: " << mfile << endl;
            } else {
               // cerr << "\tTIPS file loaded: " << mfile << endl;
               JackSoldier::jack->njoints = 
                 (G_vehtype[alive].num_joints = jack_jointCount);
               memcpy ( JackSoldier::jack->jointsinfo, jack_jointInfo, 
	               sizeof ( JackSoldier::jack->jointsinfo ) );
               JackSoldier::jack->nfigs = 
                 (G_vehtype[alive].num_figs = jack_figureCount);
               memcpy ( JackSoldier::jack->figsinfo, jack_figureInfo, 
	               sizeof ( JackSoldier::jack->figsinfo ) );
           
	       for (int j = 0; j < G_vehtype[alive].num_joints; j++) { 
                  int JNT = getJNT(jack_jointInfo[j].name);          

                  static char current_lod[30] = "High";
                  strcpy(G_vehtype[alive].joints[JNT].name, current_lod);
                  strcat( G_vehtype[alive].joints[JNT].name,"_");
                  strcat( G_vehtype[alive].joints[JNT].name,
                    jack_jointInfo[j].name);

	         pfDCS *tempDCS;
	         tempDCS = (pfDCS*)pfLookupNode(jack_jointInfo[j].name, pfGetDCSClassType());
                                     pfNodeName (tempDCS, 
		 G_vehtype[alive].joints[JNT].name);
                 strcpy(jack_jointInfo[j].name, 
	                G_vehtype[alive].joints[JNT].name);
	          strcpy(JackSoldier::jack->jointsinfo[j].name,
                        G_vehtype[alive].joints[JNT].name);
                 G_vehtype[alive].joints[JNT].dcs = tempDCS;
            }

             for (int ix = 0;ix< num_textures;ix++) {
                G_dynamic_data->texture_list_ptr[G_dynamic_data->number_of_textures++]
                   = jack_textures[ix];
             }

#if 0
             pfDCS *temp_dcs = (pfDCS*)pfLookupNode("M60", pfGetDCSClassType());
             if ( temp_dcs ) {
                pfGroup *bipod;
                int count;
                pfDCS *bipod_dcs = pfNewDCS();
                bipod = (pfGroup *)
                   loadTIPSFile("./otherlibs/Jack/m60bipod.tips",
                                &count, FALSE, NULL, NULL, NULL, NULL, NULL);
                if ( bipod ) {
                   cerr << "\t\tLoaded bipod for M60." << endl;
                   pfDCSScale(bipod_dcs,0.5f);
                   pfDCSTrans(bipod_dcs,0.22f,0.1f,-0.15f);
                   pfDCSRot(bipod_dcs,100.0f,0.0f,-25.0f);
                   pfAddChild(bipod_dcs,bipod);
                   pfAddChild(temp_dcs,bipod_dcs);
                }
             }
#endif

          }
       }
#ifndef NODUDE
       else if (modtype == TYPE_JCD) {
          // create structure of dude objects
          if(NULL == (G_vehtype[alive].model = 
             (pfGroup *)LoadDude(mfile, &num_textures,
              dude_textures[0]))){
             cerr << "Unable to load Dude "<< mfile << "\n";
          }

          // define a bounding box
          pfMakeEmptyBox(&(G_vehtype[alive].bounding_box));
          pfuTravCalcBBox ( G_vehtype[alive].model,
                            &(G_vehtype[alive].bounding_box) );

          // add Dude textures to npsnet list
          for (int ix = 0;ix< num_textures;ix++)
             G_dynamic_data->texture_list_ptr[G_dynamic_data->number_of_textures++]
                = dude_textures[ix];
        }
        else if (modtype == TYPE_JADE) {
           munition_num = 0;
           pfGroup      *jade;
           pfSwitch     *which_human;

           // create new nodes
           jade        = pfNewGroup();
           which_human = pfNewSwitch();

           whichSwitch[whichIndex] = which_human;
           whichIndex++;

            // create structure of jade
            cout <<" Building JADE model"<<endl;
            pfAddChild(jade, which_human);
            // Add jack and dude models at end after mswitch created

            pfNodeName(jade, "mainjade_GP");
            pfNodeName(which_human, "whichSwitch");

            // assign JADE model
            G_vehtype[alive].model = jade;
         }
#endif // NODUDE
         else if (modtype == TYPE_DIGUY) {
            G_vehtype[alive].model = pfNewGroup();
            DIGUY_VEH::addType (subcategory, im_models[i].jack_model);
         }

         pfMakeEmptyBox(&(G_vehtype[alive].bounding_box));

         if (G_vehtype[alive].model == NULL) 
                        cerr << "Unable to load the dynamic model file " << mfile << "\n";
         else {
            if (modtype == TYPE_DIGUY) {
               G_vehtype[alive].bounding_box.min[PF_X] = -0.35f;
               G_vehtype[alive].bounding_box.min[PF_Y] = -0.2f;
               G_vehtype[alive].bounding_box.min[PF_Z] = 0.0f;
               G_vehtype[alive].bounding_box.max[PF_X] = 0.35f;
               G_vehtype[alive].bounding_box.max[PF_Y] = 0.3f;
               G_vehtype[alive].bounding_box.max[PF_Z] = 1.85f;
            }
            else {
               pfuTravCalcBBox (G_vehtype[alive].model,
                              &(G_vehtype[alive].bounding_box) );
            }
            pfNodeName ( G_vehtype[alive].model, modname );
            switch (int(G_vehtype[alive].distype.entity_kind)) {
               case EntityKind_Other:
               case EntityKind_Platform:
               case EntityKind_Munition:
               case EntityKind_LifeForm:
               case EntityKind_Environmental:
               case EntityKind_Cultural:
                  entity_mask = long(G_vehtype[alive].distype.entity_kind);
                  entity_mask = (entity_mask << ENTITY_KIND_SHIFT);
                  break;
               default:
                  entity_mask = long(EntityKind_Other);
                  entity_mask = (entity_mask << ENTITY_KIND_SHIFT);
                  cerr << "Unknown DIS type: "
                       << int(G_vehtype[alive].distype.entity_kind)
                       << ": setting INTERSECT mask.\n";
                  cerr << "Model " << modname << ", Type ID: " << alive << endl;
                  break;
            }
            set_intersect_mask(G_vehtype[alive].model,
                                   entity_mask, COLLIDE_DYNAMIC);
      } // if G_static_data->load_dynamic_models...

      for (int k = 0; k < im_models[i].num_weapons; k++) {	// W lines
         munition_index = im_models[i].weapons[k].index;
         munition_class = im_models[i].weapons[k].wclass;
         fireX = im_models[i].weapons[k].x;
         fireY = im_models[i].weapons[k].y;
         fireZ = im_models[i].weapons[k].z;
         delaytime = im_models[i].weapons[k].delay;

         if (G_static_data->load_dynamic_models ||
              (!G_static_data->load_dynamic_models && (alive == 0)) ) {
            if ( (munition_index >= START_WEAPON) &&
                 (munition_index < (START_WEAPON+MAX_MUNITIONS)) ) {
               if ( munition_num < MAX_FIRE_INFO ) {
                  G_vehtype[alive].fireinfo[munition_num].munition = 
                     munition_index;
                  G_vehtype[alive].fireinfo[munition_num].classtype =
                     mtype(munition_class);
                  G_vehtype[alive].fireinfo[munition_num].firedelay=
                     delaytime;
                  pfSetVec3( G_vehtype[alive].fireinfo[munition_num].firepoint,
                              fireX, fireY, fireZ );
                  munition_num++;
                  G_vehtype[alive].num_weapons++;
               }
            }
         }
      } // for each W line
     }
   } // for each D/H line

  //ok, now the fun starts
  // we will build a tree that we can search to find the NPSnet type
  // here is the basic premise, if the item is not found at the level, it
  // will use the next higher level

  L_typeroot = new_VTYPENODE(0);

  //for all of the models that we have read in and stored in the array
  for(ix=0;ix<MAX_VEHTYPES;ix++){
    if(G_vehtype[ix].id == NOSHOW){
      // this one is not used
      continue;
    }
    //find out which node we are inserting

#ifdef DEBUG_B
    cerr << "Working on "<< G_vehtype[ix].name << " id " << ix << "\n";
#endif  
    
    insert_node = build_node_kind(G_vehtype[ix].distype,L_typeroot,ix);

  }
  
  //kind of wasteful to go thought the loop the secound time, but it keeps
  // the code a little cleaner
  //create the switch nodes
#ifndef NODUDE
  whichIndex = 0; // restart index
#endif // NODUDE

  for(ix=0;ix<MAX_VEHTYPES;ix++){
    if(G_vehtype[ix].id == NOSHOW){
      // this one is not used
      continue;
    }
    if(G_vehtype[ix].model == NULL)
       G_vehtype[ix].model = G_vehtype[0].model;
    if(G_vehtype[G_vehtype[ix].dead].model== NULL)
       G_vehtype[G_vehtype[ix].dead].model = G_vehtype[0].model;

#ifdef DEBUG_B
    cerr << "Making Switch node "<< G_vehtype[ix].name << " id " << ix << "\n";
#endif

#ifdef DEBUG_B
    cerr << "Chkpt 1 " << (int)(G_vehtype[ix].mswitch = pfNewSwitch()) <<"\n";
    cerr << "Chkpt 2 " << pfAddChild(G_vehtype[ix].mswitch,
             G_vehtype[ix].model)<<"\n";
    cerr << "Chkpt 3 " << pfAddChild(G_vehtype[ix].mswitch,
             G_vehtype[G_vehtype[ix].dead].model)<<"\n";
    cerr << "Chkpt 4 " << pfSwitchVal(G_vehtype[ix].mswitch,PFSWITCH_OFF)<<"\n";
#else
    G_vehtype[ix].mswitch = pfNewSwitch();
    pfAddChild(G_vehtype[ix].mswitch,G_vehtype[ix].model); 
    // Add dead model only to non human models
   
    switch ( G_vehtype[ix].modtype ) {
       case TYPE_FLT:
          pfAddChild(G_vehtype[ix].mswitch,G_vehtype[G_vehtype[ix].dead].model);          break;
       default:
          break;
    }
 
#ifndef NODUDE
    if(G_vehtype[ix].cat == JADE) {
cerr << "adding JACK/DUDE models to JADE\n";
       if(ix == JADE_GOOD) {
          pfAddChild(whichSwitch[whichIndex], G_vehtype[JACKFRIENDLY].mswitch);
          pfAddChild(whichSwitch[whichIndex], G_vehtype[DUDE_FRIEND].model);
       }
       else { // JADE_EVIL
          pfAddChild(whichSwitch[whichIndex], G_vehtype[JACKHOSTILE].mswitch);
          pfAddChild(whichSwitch[whichIndex], G_vehtype[DUDE_ENEMY].model);
       }
       whichIndex++;
    }
#endif // NODUDE
    pfSwitchVal(G_vehtype[ix].mswitch,PFSWITCH_OFF);
    strcpy ( nodename, G_vehtype[ix].name );
    strcat ( nodename, "_switch" );
    pfNodeName(G_vehtype[ix].mswitch, nodename );
#endif
  }

  FLT_load_phase = DEFAULT_LOAD_PHASE;
}  //that's all


VTYPENODE *new_VTYPENODE(int index_of_type)
//allocate and fill in the fields of the type node
{
  VTYPENODE *thenode;

  thenode = new VTYPENODE;
  //blank out the pointers, and clear any trash
  bzero(thenode,sizeof(VTYPENODE));

  //fill in the type parameter fields
  strcpy(thenode->name,G_vehtype[index_of_type].name);
  thenode->vtype = G_vehtype[index_of_type].id;
  thenode->DIStype = G_vehtype[index_of_type].distype;

  return(thenode);
}

VTYPENODE *build_node_kind(EntityType DIStype,VTYPENODE *parent,int index_of_type)
//when you have no clue how to do something, put it in a function
//this builds the tree of types,based upon the entity_kind field
//if it is not the default, send it down to fill in the domain level
{
  VTYPENODE *tempnode, *indexnode,*trailnode;

  if(parent->child == NULL){
    //first time called, create this level

#ifdef DEBUG_B
    cerr << "Add the first node kind\n";
#endif

    //create a new node
    tempnode = new_VTYPENODE(index_of_type);
    parent->child = tempnode;
  }
  else{
    if (parent->child->DIStype.entity_kind <= DIStype.entity_kind){

#ifdef DEBUG_B
    cerr << "Add the node to the head " << 0+parent->child->DIStype.entity_kind
         << " input "<<0+DIStype.entity_kind ;
#endif

      if(parent->child->DIStype.entity_kind == DIStype.entity_kind){
        //this is a same value so just go down
        tempnode = parent->child;

#ifdef DEBUG_B
    cerr << " DUP\n";
#endif

      }
      else{
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);

        //insert this one in the head
        tempnode->next = parent->child;
        parent->child = tempnode;

#ifdef DEBUG_B
    cerr << " NEW\n";
#endif

      }
    }
    else{
      indexnode = trailnode = parent->child;

#ifdef DEBUG_B
    cerr << "Starting with "<< 0+indexnode->DIStype.entity_kind
         << " and input " << 0+DIStype.entity_kind << "\n";
#endif

      //step though the list until we run out of nodes or find a match
      while((indexnode != NULL) &&
             (indexnode->DIStype.entity_kind > DIStype.entity_kind)){
#ifdef DEBUG_B
    cerr << "Looking at "<< 0+indexnode->DIStype.entity_kind 
         << " and input " << 0+DIStype.entity_kind << "\n";
#endif

         trailnode = indexnode;
         indexnode = indexnode->next;
      }
      if ((indexnode == NULL) ||
          (indexnode->DIStype.entity_kind != DIStype.entity_kind)){
        //it is not equal so create a node and insert it in the list
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert into the list
        tempnode->next = indexnode;
        trailnode->next = tempnode;
      }
      else{

#ifdef DEBUG_B
    cerr << "Dup " << 0+indexnode->DIStype.entity_kind
         << " and input " << 0+DIStype.entity_kind << "\n";
#endif

        //use this as a branch node, it is a dup
        tempnode = indexnode;
      }
      
    }
  }

#ifdef DEBUG_B
  cerr << "Running the list ";
  indexnode = parent->child;
  while (indexnode){
    cerr << 0+indexnode->DIStype.entity_kind << " ";
    indexnode = indexnode->next;
  }
  cerr << " End of list\n";
#endif

  if(DIStype.entity_kind == DEFAULT){
       //the default nodes do not have children
       return(tempnode);
  }
  else{
       //go down a level
       return(build_node_domain(DIStype,tempnode,index_of_type));
  }
}

VTYPENODE *build_node_domain(EntityType DIStype,VTYPENODE *parent,int index_of_type)
//when you have no clue how to do something, put it in a function
//this builds the tree of types,based upon the domain field
//if it is not the default, send it down to fill in the domain level
{
  VTYPENODE *tempnode, *indexnode,*trailnode;

  if(parent->child == NULL){
    //first time called, create this level
    //create a new node
    tempnode = new_VTYPENODE(index_of_type);
    parent->child = tempnode;
  }
  else{
    if (parent->child->DIStype.domain <= DIStype.domain){
      if(parent->child->DIStype.domain == DIStype.domain){
        //this is a same value so just go down
        tempnode = parent->child;
      }
      else{
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert this one in the head
        tempnode->next = parent->child;
        parent->child = tempnode;
      }
    }
    else{
      indexnode = trailnode = parent->child;

      //step though the list until we run out of nodes or find a match
      while((indexnode != NULL) &&
             (indexnode->DIStype.domain > DIStype.domain)){
         trailnode = indexnode;
         indexnode = indexnode->next;
      }
      if ((indexnode == NULL) ||
          (indexnode->DIStype.domain != DIStype.domain)){
        //it is not equal so create a node and insert it in the list
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert into the list
        tempnode->next = indexnode;
        tempnode->next = indexnode;
        trailnode->next = tempnode;
      }
      else{
        //use this as a branch node, it is a dup
        tempnode = indexnode;
      }

    }
  }
  if(DIStype.domain == DEFAULT){
       //the default nodes do not have children
       return(tempnode);
  }
  else{
       //go down a level
       return(build_node_country(DIStype,tempnode,index_of_type));
  }
}

VTYPENODE *build_node_country(EntityType DIStype,VTYPENODE *parent,int index_of_type)
//when you have no clue how to do something, put it in a function
//this builds the tree of types,based upon the country field
//if it is not the default, send it down to fill in the country level
{
  VTYPENODE *tempnode, *indexnode,*trailnode;

  if(parent->child == NULL){
    //first time called, create this level
    //create a new node
    tempnode = new_VTYPENODE(index_of_type);
    parent->child = tempnode;
  }
  else{
    if (parent->child->DIStype.country <= DIStype.country){
      if(parent->child->DIStype.country == DIStype.country){
        //this is a same value so just go down
        tempnode = parent->child;
      }
      else{
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert this one in the head
        tempnode->next = parent->child;
        parent->child = tempnode;
      }
    }
    else{
      indexnode = trailnode = parent->child;

      //step though the list until we run out of nodes or find a match
      while((indexnode != NULL) &&
             (indexnode->DIStype.country > DIStype.country)){
         trailnode = indexnode;
         indexnode = indexnode->next;
      }
      if ((indexnode == NULL) ||
          (indexnode->DIStype.country != DIStype.country)){
        //it is not equal so create a node and insert it in the list
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert into the list
        tempnode->next = indexnode;
        tempnode->next = indexnode;
        trailnode->next = tempnode;
      }
      else{
        //use this as a branch node, it is a dup
        tempnode = indexnode;
      }

    }
  }
  //go down a level, who cares what country we are using
  return(build_node_category(DIStype,tempnode,index_of_type));
}

VTYPENODE *build_node_category(EntityType DIStype,VTYPENODE *parent,int index_of_type)
//when you have no clue how to do something, put it in a function
//this builds the tree of types,based upon the category field
//if it is not the default, send it down to fill in the category level
{
  VTYPENODE *tempnode, *indexnode,*trailnode;

  if(parent->child == NULL){
    //first time called, create this level
    //create a new node
    tempnode = new_VTYPENODE(index_of_type);
    parent->child = tempnode;
  }
  else{
    if (parent->child->DIStype.category <= DIStype.category){
      if(parent->child->DIStype.category == DIStype.category){
        //this is a same value so just go down
        tempnode = parent->child;
      }
      else{
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert this one in the head
        tempnode->next = parent->child;
        parent->child = tempnode;
      }
    }
    else{
      indexnode = trailnode = parent->child;

      //step though the list until we run out of nodes or find a match
      while((indexnode != NULL) &&
             (indexnode->DIStype.category > DIStype.category)){
         trailnode = indexnode;
         indexnode = indexnode->next;
      }
      if ((indexnode == NULL) ||
          (indexnode->DIStype.category != DIStype.category)){
        //it is not equal so create a node and insert it in the list
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert into the list
        tempnode->next = indexnode;
        trailnode->next = tempnode;
      }
      else{
        //use this as a branch node, it is a dup
        tempnode = indexnode;
      }

    }
  }
  if(DIStype.category == DEFAULT){
       //the default nodes do not have children
       return(tempnode);
  }
  else{
       //go down a level
       return(build_node_subcategory(DIStype,tempnode,index_of_type));
  }
}

VTYPENODE *build_node_subcategory(EntityType DIStype,VTYPENODE *parent,int index_of_type)
//when you have no clue how to do something, put it in a function
//this builds the tree of types,based upon the subcategory field
//if it is not the default, send it down to fill in the subcategory level
{
  VTYPENODE *tempnode, *indexnode,*trailnode;

  if(parent->child == NULL){
    //first time called, create this level
    //create a new node
    tempnode = new_VTYPENODE(index_of_type);
    parent->child = tempnode;
  }
  else{
    if (parent->child->DIStype.subcategory <= DIStype.subcategory){
      if(parent->child->DIStype.subcategory == DIStype.subcategory){
        //this is a same value so just go down
        tempnode = parent->child;
      }
      else{
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert this one in the head
        tempnode->next = parent->child;
        parent->child = tempnode;
      }
    }
    else{
      indexnode = trailnode = parent->child;

      //step though the list until we run out of nodes or find a match
      while((indexnode != NULL) &&
             (indexnode->DIStype.subcategory > DIStype.subcategory)){
         trailnode = indexnode;
         indexnode = indexnode->next;
      }
      if ((indexnode == NULL) ||
          (indexnode->DIStype.subcategory != DIStype.subcategory)){
        //it is not equal so create a node and insert it in the list
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert into the list
        tempnode->next = indexnode;
        tempnode->next = indexnode;
        trailnode->next = tempnode;
      }
      else{
        //use this as a branch node, it is a dup
        tempnode = indexnode;
      }

    }
  }
  if(DIStype.subcategory == DEFAULT){
       //the default nodes do not have children
       return(tempnode);
  }
  else{
       //go down a level
       return(build_node_specific(DIStype,tempnode,index_of_type));
  }
}

VTYPENODE *build_node_specific(EntityType DIStype,VTYPENODE *parent,int index_of_type)
//when you have no clue how to do something, put it in a function
//this builds the tree of types,based upon the specific field
//if it is not the default, send it down to fill in the specific level
{
  VTYPENODE *tempnode, *indexnode,*trailnode;

  if(parent->child == NULL){
    //first time called, create this level
    //create a new node
    tempnode = new_VTYPENODE(index_of_type);
    parent->child = tempnode;
  }
  else{
    if (parent->child->DIStype.specific <= DIStype.specific){
      if(parent->child->DIStype.specific == DIStype.specific){
        //this is a same value so just go down
        tempnode = parent->child;
      }
      else{
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert this one in the head
        tempnode->next = parent->child;
        parent->child = tempnode;
      }
    }
    else{
      indexnode = trailnode = parent->child;

      //step though the list until we run out of nodes or find a match
      while((indexnode != NULL) &&
             (indexnode->DIStype.specific > DIStype.specific)){
         trailnode = indexnode;
         indexnode = indexnode->next;
      }
      if ((indexnode == NULL) ||
          (indexnode->DIStype.specific != DIStype.specific)){
        //it is not equal so create a node and insert it in the list
        //create a new node
        tempnode = new_VTYPENODE(index_of_type);
        //insert into the list
        tempnode->next = indexnode;
        tempnode->next = indexnode;
        trailnode->next = tempnode;
      }
      else{
        //use this as a branch node, it is a dup
        tempnode = indexnode;
      }

    }
  }
  //What ever we find is good enough
  return(tempnode);
}

int vehtype_from_net(EntityType *DIStype)
//look up to find what type it is, we return an integer that 
//corresponds to the NPSNET id
//certain assumptions are made
// 1. 0 at each level is the default, except for county
//    when a default is reached, it returns with out further 
//    traversal
// 2. if there is not a default specified, the lowest number is used
//    as the default and the system returns.
// 3. Matches cause it to go down a level
{
  //this is kind of hooky, it just calls the first level

#ifdef DEBUG_Z
   int temp_type;
   
   temp_type = find_entity_kind(DIStype,L_typeroot);
   cerr << "DIS Type "<< 0 +DIStype->entity_kind <<" "<< 0 +DIStype->domain
        <<" "<<DIStype->country<<" "<<0 +DIStype->category<<" ";
   cerr <<0 +DIStype->subcategory<<" "<<0 +DIStype->specific 
        << " is type " << G_vehtype[temp_type].id <<" "
        <<G_vehtype[temp_type].name<<"\n";

   return(temp_type);
#else
   return(find_entity_kind(DIStype,L_typeroot));
#endif

}

int find_entity_kind(EntityType *DIStype,VTYPENODE *parent)
//traverse the list based upon entity_kind
{
  VTYPENODE *tempnode = parent->child;
  
  //find which one it was
  while((DIStype->entity_kind != tempnode->DIStype.entity_kind) &&
        (tempnode->next != NULL)){
    tempnode = tempnode->next;
  }
  if((DIStype->entity_kind == tempnode->DIStype.entity_kind) && 
     (DIStype->entity_kind != DEFAULT)){
    //we found a match, go to the next level
    return(find_domain(DIStype,tempnode));
  }
  else{
    //use one of the defaults
    return(tempnode->vtype);
  }
}

int find_domain(EntityType *DIStype,VTYPENODE *parent)
//traverse the list based upon domain
{
  VTYPENODE *tempnode = parent->child;

  
  //find which one it was
  while((DIStype->domain != tempnode->DIStype.domain) &&
        (tempnode->next != NULL)){
    tempnode = tempnode->next;
  }
  if((DIStype->domain == tempnode->DIStype.domain) && 
     (DIStype->domain != DEFAULT)){
    //we found a match, go to the next level
    return(find_country(DIStype,tempnode));
  }
  else{
    //use one of the defaults
    return(tempnode->vtype);
  }
}


int find_country(EntityType *DIStype,VTYPENODE *parent)
//traverse the list based upon country
{
  VTYPENODE *tempnode = parent->child;
  
  //find which one it was
  while((DIStype->country != tempnode->DIStype.country) &&
        (tempnode->next != NULL)){
    tempnode = tempnode->next;
  }
#ifdef temp_code
// for the time being we are using default countries
  if((DIStype->country == tempnode->DIStype.country) && 
     (DIStype->country != DEFAULT)){
    //we found a match, go to the next level
    return(find_category(DIStype,tempnode));
  }
  else{
    //use one of the defaults
    return(tempnode->vtype);
  }
#else
  if(DIStype->country == tempnode->DIStype.country){
    //we found a match, go to the next level
    return(find_category(DIStype,tempnode));
  }
  else{
    //use one of the defaults
    return(tempnode->vtype);
  }
#endif
}


int find_category(EntityType *DIStype,VTYPENODE *parent)
//traverse the list based upon category
{
  VTYPENODE *tempnode = parent->child;
  
  //find which one it was
  while((DIStype->category != tempnode->DIStype.category) &&
        (tempnode->next != NULL)){
    tempnode = tempnode->next;
  }
  if((DIStype->category == tempnode->DIStype.category) && 
     (DIStype->category != DEFAULT)){
    //we found a match, go to the next level
    return(find_subcategory(DIStype,tempnode));
  }
  else{
    //use one of the defaults
    return(tempnode->vtype);
  }
}

int find_subcategory(EntityType *DIStype,VTYPENODE *parent)
//traverse the list based upon subcategory
{
  VTYPENODE *tempnode = parent->child;
  
  //find which one it was
  while((DIStype->subcategory != tempnode->DIStype.subcategory) &&
        (tempnode->next != NULL)){
    tempnode = tempnode->next;
  }
  if((DIStype->subcategory == tempnode->DIStype.subcategory) && 
     (DIStype->subcategory != DEFAULT)){
    //we found a match, go to the next level
    return(find_specific(DIStype,tempnode));
  }
  else{
    //use one of the defaults
    return(tempnode->vtype);
  }
}

int find_specific(EntityType *DIStype,VTYPENODE *parent)
//traverse the list based upon specific
{
  VTYPENODE *tempnode = parent->child;
  
  //find which one it was
  while((DIStype->specific != tempnode->DIStype.specific) &&
        (tempnode->next != NULL)){
    tempnode = tempnode->next;
  }
  //return what ever we have found
  return(tempnode->vtype);
}


struct Host im_hosts[MAX_HOSTS];
int num_hosts;

void get_im_hosts()
   // Get the hosts data from the InitMan
{
   //cout << "get_im_hosts()\n";

   initman->get_hosts((struct Host**) im_hosts, &num_hosts);

   //cout << "\tnum_hosts=" << num_hosts << endl;

/*
   for (int i = 0; i < num_hosts; i++) {
      cout << "Host=" << im_hosts[i].hostname << ","
                      << im_hosts[i].siteno << ","
                      << im_hosts[i].hostno << ","
                      << im_hosts[i].interf << endl;
   }
*/
}


void findsitehost(char ether_interf[], char *hostname, 
                  SimulationAddress *sitehost )
   // Find the site/host numbers and which interface this is
{
   char name[80];
   /*
   int dummy;
   EntityID address;
   int found = FALSE;
   */

   //cout << "findsitehost()\n";

   gethostname(name,80);

   // Scan the array
   for (int i = 0; i < num_hosts; i++) {
      if (strcmp(im_hosts[i].hostname, name) == 0) {
         // open the network for this host
         strcpy(hostname, name);
         sitehost->site = (unsigned short) im_hosts[i].siteno;
         sitehost->host = (unsigned short) im_hosts[i].hostno;
         strcpy(ether_interf, im_hosts[i].interf);
         return;
      }
   }

   // Not found
   cerr << "Host " << name << " was not found in the network";
   cerr << " information file.\n";
   strcpy(hostname, name);
   sitehost->site = 99;
   sitehost->host = 1;
   strcpy(ether_interf, "");
   cerr << "Defaulting to Site 99, Host 1, default interface." << endl;

/*
  ifstream hostfile ((char *)G_static_data->network_file);
  if (!hostfile)
     {
     cerr << "Unable to open network information file:  "
          << (char *)G_static_data->network_file << endl;
     }
  else
     {
     while ( !found && (hostfile >> temp) )
       {
       if(!strcmp(temp,name))
         {
         //open the network for this host
         hostfile >> address.address.site >> address.address.host 
                  >> ether_interf;
         strcpy ( hostname, name );
         sitehost->site = address.address.site;
         sitehost->host = address.address.host;
         found = TRUE;
         }
       //trash this puppy
       hostfile >> dummy >> dummy >> temp;
       }
     }

   if ( !found )
      {
      cerr << "Host " << name << " was not found in the network" << endl;
      cerr << "\tinformation file, " << (char *)G_static_data->network_file 
           << endl;
      strcpy ( hostname, name );
      sitehost->site = 99;
      sitehost->host = 1;
      strcpy ( ether_interf, "" );
      cerr << "\tDefaulting to Site 99, Host 1, default interface." << endl;
      }
*/
}

void gotfire(FirePDU *pdu)
//we got an Fire PDU, what do we do with it
{
//   int vid;
   float *ourposition;
   EFFECT_REC effect;
   pfVec3 loc;
   pfVec3 orient = { 0.0f, 0.0f, 0.0f };
   pfVec3 temp;

//   vid_from_net( &(pdu->firing_entity_id),&vid);
   G_dynamic_data->identify->effect ( &pdu->burst_descriptor.munition,
   				      &effect );
   loc[X] = float(pdu->location_in_world.x);
   loc[Y] = float(pdu->location_in_world.y);
   loc[Z] = float(pdu->location_in_world.z);
   temp[X] = float(pdu->velocity.x);
   temp[Y] = float(pdu->velocity.y);
   temp[Z] = float(pdu->velocity.z);
   pfNormalizeVec3 ( temp );
   orient[PITCH] = pfArcSin ( temp[Z] );
   orient[HEADING] = pfArcTan2 ( -temp[X], temp[Y] );

   ourposition = G_vehlist[G_drivenveh].vehptr->getposition();

   if ( G_static_data->sound )
      {
      LocationRec our_pos;
      LocationRec the_snd_pos;
      our_pos.x = ourposition[X];
      our_pos.y = ourposition[Y];
      our_pos.z = ourposition[Z];
      the_snd_pos.x = loc[X];
      the_snd_pos.y = loc[Y];
      the_snd_pos.z = loc[Z];
      playsound(&our_pos,&the_snd_pos,effect.fire_local_snd);
      }

   switch (effect.fire_effect)
      {
      case SMALL_FLASH:
         play_animation ( SMALL_FLASH, loc, orient );
         break;
      case MEDIUM_FLASH:
         play_animation ( MEDIUM_FLASH, loc, orient );
         break;
      case LARGE_FLASH:
         play_animation ( LARGE_FLASH, loc, orient );
         break;
      case SMALL_GUN:
         play_animation ( SMALL_GUN, loc, orient );
         break;
      case MEDIUM_GUN:
         play_animation ( MEDIUM_GUN, loc, orient );
         break;
      case LARGE_GUN:
         play_animation ( LARGE_GUN, loc, orient );
         break;
      case SMALL_SMOKE:
         play_animation ( SMALL_SMOKE, loc, orient );
         break;
      case MEDIUM_SMOKE:
         play_animation ( MEDIUM_SMOKE, loc, orient );
         break;
      case LARGE_SMOKE:
         play_animation ( LARGE_SMOKE, loc, orient );
         break;
      case EXPLOSION:
         play_animation ( EXPLOSION, loc, orient );
         break;
      case AIR_BURST:
         play_animation ( AIR_BURST, loc, orient );
         break;
      case GROUND_HIT:
         play_animation ( GROUND_HIT, loc, orient );
         break;
      case IMPACT:
         play_animation ( IMPACT, loc, orient );
         break;
      case NONE:
         break;
      default:
         play_animation ( SMALL_FLASH, loc, orient );
         break;
      }

   if ( !G_static_data->stealth  )
      {
      if((pdu->target_entity_id.address.site ==
          G_vehlist[G_drivenveh].DISid.address.site)&&
         (pdu->target_entity_id.address.host ==
          G_vehlist[G_drivenveh].DISid.address.host)&&
         (pdu->target_entity_id.entity == G_vehlist[G_drivenveh].DISid.entity))
         {
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         G_dynamic_data->early_warn = TRUE;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         if ( G_static_data->sound )
            {
            LocationRec our_pos;
            our_pos.x = 0.0f;
            our_pos.y = 0.0f;
            our_pos.z = 0.0f;
            playsound(&our_pos,&our_pos,INCOMING_INDEX,-1);
            }

         }
      }
//   printf("Got %d:FirePDU        %d\n", FirePDU_Type,vid);
}

void do_detonate_effect ( int effect, pfVec3 pos, pfVec3 orient, 
                          int crater, int smoke )
{
   switch ( effect )
      {
      case SMALL_FLASH:
         play_animation ( SMALL_FLASH, pos, orient );
         break;
      case MEDIUM_FLASH:
         play_animation ( MEDIUM_FLASH, pos, orient );
         break;
      case LARGE_FLASH:
         play_animation ( LARGE_FLASH, pos, orient );
         break;
      case SMALL_GUN:
         play_animation ( SMALL_GUN, pos, orient );
         break;
      case MEDIUM_GUN:
         play_animation ( MEDIUM_GUN, pos, orient );
         break;
      case LARGE_GUN:
         play_animation ( LARGE_GUN, pos, orient );
         break;
      case SMALL_SMOKE:
         play_animation ( SMALL_SMOKE, pos, orient );
         break;
      case MEDIUM_SMOKE:
         play_animation ( MEDIUM_SMOKE, pos, orient );
         break;
      case LARGE_SMOKE:
         play_animation ( LARGE_SMOKE, pos, orient );
         break;
      case EXPLOSION:
         play_animation ( EXPLOSION, pos, orient );
         if ( G_dynamic_data->dynamic_terrain ) {
            if ( crater )
               makecrater_pos ( pos );
            if ( smoke )
               use_next_fire_plume(pos[X],pos[Y],pos[Z], 20.0f);
         }
         break;
      case AIR_BURST:
         play_animation ( AIR_BURST, pos, orient );
         break;
      case GROUND_HIT:
         play_animation ( GROUND_HIT, pos, orient );
         break;
      case IMPACT:
         play_animation ( IMPACT, pos, orient );
         break;
      case NONE:
         break;
      case FIREBALL:
         makefireball ( pos );
         break;
      case UNKNOWN:
      default:
         play_animation ( MEDIUM_SMOKE, pos, orient );
         break;
      }


}

void gotdetonate(DetonationPDU *pdu)
//we got a Detonate PDU, what do we do with it
{
//   int vid;
   pfVec3 pos;
   pfSegSet seg;
   pfVec3   pnt;
   float blast_dist;
   float *ourposition;
   EFFECT_REC effect;
   pfVec3 orient = { 0.0f, 0.0f, 0.0f };
   int do_smoke;

/*
  if((pdu->munition_id.address.site == 0) &&
      (pdu->munition_id.address.host == 0) &&
      (pdu->munition_id.entity == 0)){
    //it did not hit a vehcile
*/
 
   G_dynamic_data->identify->effect ( &pdu->burst_descriptor.munition,
   				      &effect );
 
   pfSetVec3(pos,(float)pdu->location_in_world.x,
                (float)pdu->location_in_world.y,
                (float)pdu->location_in_world.z);

  ourposition = G_vehlist[G_drivenveh].vehptr->getposition();

  if ( !G_static_data->stealth )
     {
     blast_dist = pfSqrDistancePt3(pos,ourposition);
     }

  if ( G_static_data->sound )
     {
     LocationRec our_pos;
     LocationRec the_snd_pos;
     our_pos.x = ourposition[X];
     our_pos.y = ourposition[Y];
     our_pos.z = ourposition[Z];
     the_snd_pos.x = pos[X];
     the_snd_pos.y = pos[Y];
     the_snd_pos.z = pos[Z];
     playsound(&our_pos,&the_snd_pos,effect.det_local_snd);
     }

  switch(pdu->detonation_result){
    case DetResult_None:  //we don't do anything with this case -- 6
      break;
    case DetResult_GroundImpact: //hit the ground -- 3
      do_smoke = FALSE;
      do_detonate_effect ( effect.det_effect, pos, orient, TRUE, do_smoke);
      break;
    case DetResult_GroundProxDet: //close to the ground -- 4
      do_detonate_effect ( effect.det_effect, pos, orient, FALSE, FALSE );
      break;
    case DetResult_Detonation:  //it just blew up  -- 5
      //it might have hit a building, so we have to check
      buildseg(seg.segs,pdu);
      if(pdu->location_in_world.z < 500.0f){
        pfHit **hits[32];
        seg.mode = PFTRAV_IS_PRIM;
        seg.userData = NULL;
        seg.activeMask = 0x01;
        seg.isectMask = DESTRUCT_MASK;
        seg.bound = NULL;
        seg.discFunc = buildinghit;
        if(pfNodeIsectSegs(G_fixed, &seg, hits))
        {
            
                //the actual work is handled by the call back and the discriminator
                //function
        
                //if intersect bldg--smoke
                pfQueryHit ( *hits[0], PFQHIT_POINT, &pnt);
                pfCopyVec3 ( pos, pnt );
                do_detonate_effect ( effect.det_effect, pos, orient, FALSE, TRUE );
        }               
        else
           do_detonate_effect ( effect.det_effect, pos, orient, FALSE, FALSE);


      }
      else
         do_detonate_effect ( effect.det_effect, pos, orient, FALSE, FALSE );
      break;
    case DetResult_EntityImpact: // hit a vehicle -- 1
    case DetResult_EntityProxDet: //Proximity detionation -- 2

       if ( (!G_static_data->stealth) && (blast_dist < DEATH_DIST_SQR) && 
            (!G_static_data->covert) )
          {

          if((pdu->firing_entity_id.address.site ==
                   G_vehlist[G_drivenveh].DISid.address.site) &&
             (pdu->firing_entity_id.address.host ==
                   G_vehlist[G_drivenveh].DISid.address.host) &&
             (pdu->firing_entity_id.entity ==
                   G_vehlist[G_drivenveh].DISid.entity))
             {
             //no self inflicted wounds, it is cleaner this way
             }
          else if((pdu->target_entity_id.address.site ==
                   G_vehlist[G_drivenveh].DISid.address.site) &&
            (pdu->target_entity_id.address.host ==
                   G_vehlist[G_drivenveh].DISid.address.host) &&
            (pdu->target_entity_id.entity ==
                   G_vehlist[G_drivenveh].DISid.entity))
            {

#ifdef DATALOCK
          pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->tethered = FALSE;
            G_dynamic_data->dead = DYING;
            G_dynamic_data->cause_of_death = MUNITION_DEATH;
            G_vehlist[G_drivenveh].vehptr->changestatus(Appearance_DamageDestroyed);
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
            }

         }

    case DetResult_Other: //Something else -- 0
    default:  //got something else
       do_detonate_effect ( effect.det_effect, pos, orient, FALSE, FALSE );
      break;
  }


  if ( effect.smoke_effect != NO_SMOKE )
     use_next_fire_plume(pos[X],pos[Y],pos[Z], 20.0f, effect.smoke_effect);


  if ((pdu->munition_id.address.site == 0) && 
      (pdu->munition_id.address.host == 0) &&
      (pdu->munition_id.entity == 0)){
     //this is a non-tracked detionaltion
     return;
  }
/*
  if ( vid_from_net( &(pdu->munition_id),&vid) )
     {
     if ( pdu->burst_descriptor.munition.entity_kind == EntityKind_Munition )
        deactiveate__entity(vid);
     }

   vid_from_net( &(pdu->munition_id),&vid);
   if ( pdu->burst_descriptor.munition.entity_kind == EntityKind_Munition )
        deactiveate__entity(vid);
*/
}

void  buildseg(pfSeg *seg, DetonationPDU *pdu)
//we got a Detonate PDU, what do we do with it
{
  pfSetVec3(seg->pos,(float)pdu->location_in_world.x,
                     (float)pdu->location_in_world.y,
                     (float)pdu->location_in_world.z);
  pfSetVec3(seg->dir,pdu->velocity.x,pdu->velocity.y,pdu->velocity.z);
  pfNormalizeVec3(seg->dir);
  pfScaleVec3(seg->dir, 4.0f, seg->dir);
  pfSubVec3(seg->pos, seg->pos, seg->dir);
  pfNormalizeVec3(seg->dir);
  //fix a performer bug
  if(seg->dir[Y] == 0.0f) seg->dir[Y] = 0.01f;
  if(seg->dir[X] == 0.0f) seg->dir[X] = 0.01f;
  seg->length = 6.0f;  // a fudge factor to account for hitting the skin

#ifdef DEBUG
//cerr << "Pos " << pos[X] <<" " << pos[Y] <<" "<< pos[Z] <<endl;
cerr << "Seg pos "<<seg->pos[X]<<" " <<seg->pos[Y]<<" "<<seg->pos[Z] <<endl;
cerr << "Seg dir "<<seg->dir[X]<<" " <<seg->dir[Y]<<" "<<seg->dir[Z] <<endl;
cerr << "Seg length  "<<seg->length <<endl;
#endif

} 


int gotentity (EntityStatePDU *pdu, sender_info &sender)
// Process an Entity State PDU from the network
{
  int vid;    // Vehicle index in G_vehlist array
  int exists; // Whether we already know about this entity or not
  int error = FALSE; // An error occurred

  // Hash Site/Host/Entity to see if entity exists
  exists = vid_from_net( &(pdu->entity_id),&vid);

  if (!exists) {
     // If entity does not already exist, then create new entity
     if (vid != NOSHOW) {
        create_new_entity ( NETWORK_CTL, vid, pdu->entity_id,
                            vehtype_from_net(&(pdu->entity_type)),
                            pdu->force_id);
     }
     else {
        error = TRUE;
        cerr << "ERROR:\tgotentity -- vid_from_net returned error." << endl;
        cerr << "\tCould not create entity: "
             << pdu->entity_id.address.site << "/"
             << pdu->entity_id.address.host << "/"
             << pdu->entity_id.entity << endl;
     }
  }

  // Update the entity's class information from the EPDU
  if ( !error ) {
     if ( G_vehlist[vid].vehptr != NULL ) {
        G_vehlist[vid].vehptr->entitystateupdate(pdu,sender);
     }
     else {
        cerr << "ERROR:\tgotentity -- Unable to update Entity State for "
             << "vehicle # " << vid << endl;
     }
  }

  return exists;
}

#ifndef NOHIRESNET

int gotHiResDataPDU(DataPDU *pdu, sender_info &)
{
   int vid;    // Vehicle index in G_vehlist array
   int exists; // Whether we already know about this entity or not
   int error = FALSE; // An error occurred
   JACK_DI_VEH *jackVeh = (JACK_DI_VEH *)NULL;
#ifndef NONPSSHIP
   SHIP_WALK_VEH *shipVeh = (SHIP_WALK_VEH *)NULL;
#endif // NONPSSHIP
   unsigned int icompkey;   //the index into the hash table

   // Hash Site/Host/Entity to see if entity exists

//   exists = vid_from_net( &(pdu->orig_entity_id),&vid);
// Changed by Barham at AUSA95, 10/14/95
   exists = find_vehicle ( &(pdu->orig_entity_id), &vid, &icompkey );

#ifndef NONPSSHIP
   EntityID *drveh = get_netid(G_drivenveh);
#endif // NONPSSHIP

   if (exists) {
      if(G_vehlist[vid].vehptr == NULL){
        cerr << "WARNING:\tGot HiRez packet for a DIS entity with Hash entry"
             << endl
             << "\tbut NO vehicle pointer: "
             << pdu->orig_entity_id.address.site << "/"
             << pdu->orig_entity_id.address.host << "/"
             << pdu->orig_entity_id.entity << endl;
        return(0);
      }

      if ( (pdu->request_id == HIRES_DATA_PDU_TAG) ) {
         if (G_vehlist[vid].vehptr->gettype () == JACK_DI) {
            jackVeh = (JACK_DI_VEH *)G_vehlist[vid].vehptr;
            if (jackVeh) {
               jackVeh->set_net_type (hiResNet);
               jackVeh->ok_to_update ();
#ifndef NOUPPERBODY
               jackVeh->hi_res_rifle_on ();
#endif // NOUPPERBODY
               jackVeh->set_hi_res_time_stamp (G_curtime);
               jackVeh->set_hi_res (pdu->fixed_datum, pdu->num_datum_fixed);
            }// jackVeh
         }//type == JACK_DI
      } // pdu == HIRES_TAG
      else if ( (pdu->request_id == HIRES_DATA_PDU_MIMIC_TAG) ) {
         if (G_vehlist[vid].vehptr->gettype () == DI_GUY) {
            DIGUY_VEH *diVeh = (DIGUY_VEH *) G_vehlist[vid].vehptr;
            if (diVeh) {
               diVeh->set_hr_mimic_info (pdu->fixed_datum,
                                         pdu->num_datum_fixed);
            }
         }
      }
#ifndef NONPSSHIP
      else if ( (pdu->request_id == HIRES_DATA_PDU_MOUNT_TAG) ) {
              if (G_vehlist[vid].vehptr->gettype () == JACK_DI) {
                 jackVeh = (JACK_DI_VEH *)G_vehlist[vid].vehptr;
                 if (jackVeh) {
                    jackVeh->set_net_type (hiResNet);
                    jackVeh->ok_to_update ();
                    jackVeh->set_hi_res_time_stamp (G_curtime);
                    jackVeh->set_hi_res (pdu->fixed_datum, pdu->num_datum_fixed);
                    // Setting recv_entity_id to ship we are mounted to
                    exists = find_vehicle( &(pdu->recv_entity_id), &vid, &icompkey );
                    if (exists) {
                       jackVeh->mount(vid);
//cerr<<"\t\tMounting vid:"<<vid<<endl;
                    }
                    else if((drveh->entity==pdu->recv_entity_id.entity) &&
                            (drveh->address.host==pdu->recv_entity_id.address.host) &&
                            (drveh->address.site==pdu->recv_entity_id.address.site)) {
//cerr<<"\t\tMounting_Driven_Veh"<<endl;
                            jackVeh->mount(G_drivenveh);
                         }
                    else {
                       jackVeh->dismount();
                    }
                 }// jackVeh
              }// type == JACK_DI
           } // pdu == MOUNT_TAG
      else if ( (pdu->request_id == HIRES_DATA_PDU_SHIP_TAG) ) {
              if (G_vehlist[vid].vehptr->gettype () == SHIP) {
                 shipVeh = (SHIP_WALK_VEH *)
                           ((SHIP_VEH *)G_vehlist[vid].vehptr)->is_WALK_CLASS();
                 if (shipVeh) {
//cerr<<"Updating Ship Sent DATA_PDU"<<endl;
                    shipVeh->set_net_type (hiResNet);
                    shipVeh->ok_to_update ();
                    shipVeh->set_hi_res_time_stamp (G_curtime);
                    shipVeh->set_hi_res (pdu->fixed_datum, -1);
                 } //shipVeh
              } //type == SHIP
              else if (G_vehlist[vid].vehptr->gettype () == JACK_DI) {
                      int svid; //vehicle id of ship
                      exists = find_vehicle( &(pdu->recv_entity_id), &svid, &icompkey );
                      if(!exists &&
                         ((drveh->entity==pdu->recv_entity_id.entity) &&
                          (drveh->address.host==pdu->recv_entity_id.address.host) &&
                          (drveh->address.site==pdu->recv_entity_id.address.site)) ){
                         svid = G_drivenveh;
                      }
                      else if (!exists) svid = -1;
                      if(svid >= 0) {
                         shipVeh = (SHIP_WALK_VEH *)G_vehlist[svid].vehptr;
//cerr<<"Updating Jack Sent DATA_PDU"<<endl;
                         shipVeh->set_net_type (hiResNet);
                         shipVeh->ok_to_update ();
                         shipVeh->set_hi_res_time_stamp (G_curtime);
                         shipVeh->set_hi_res (pdu->fixed_datum, vid);
                      } //svid > 0
                   }// type == JACK_DI
           }// pdu == SHIP TAG
#endif // NONPSSHIP
           else {
              cerr << "Unknow Tag: " << (hex) <<  pdu->request_id << (dec)
                   << endl;
           }
   } // exists
   else {
      error = TRUE;
      cerr << "WARNING:\tGot HiRez packet for a non-existant DIS entity"
           << endl
           << "\tNO hash entry for the following entity: "
           << pdu->orig_entity_id.address.site << "/"
           << pdu->orig_entity_id.address.host << "/"
           << pdu->orig_entity_id.entity << endl;
   }

 
   return exists;

}

#endif // NOHIRESNET

void empty_net_buffers ( DIS_net_manager *net )
{
   char *pdu;
   PDUType type;
   sender_info sender;
   int switched_buffers;

   while ( net->read_pdu(&pdu, &type, sender, switched_buffers) );

}

int parse_net_pdus( DIS_net_manager *net)
//read the network and parse the pdu's based upon type
//returns the number of PDUS read
{
   int numgot = 0;
   PDUType type;
   char *pdu;
   int switched_buffers;
   sender_info sender;
   int num_entities_created = 0;
   while ( (num_entities_created < 4) &&
           (numgot < 9) &&
           (net->read_pdu(&pdu, &type, sender, switched_buffers)) ) {
         //we got one
      PDUHeader            *header;

      header = (PDUHeader *) pdu;

      //if so what kind
      switch(type)
         {
         case (FirePDU_Type):
            gotfire((FirePDU *) pdu);
            break;
         case (EntityStatePDU_Type):
            if ( !filterPDU((EntityStatePDU *)header) ) {
               if ( !gotentity((EntityStatePDU *) pdu, sender) ) {
                  num_entities_created++;
               }
            }
            break;
         case (DetonationPDU_Type):
            gotdetonate((DetonationPDU *) pdu);
            break;
#ifndef NOHIRESNET
         case (DataPDU_Type):
            if ( !gotHiResDataPDU((DataPDU *) pdu, sender) ) {
            }
            break;
#endif
#ifndef NO_DVW
         case (CloudStatePDU_Type):
            gotCloudState((CloudStatePDU *) pdu);
            break;
         case (EnvironmentalPDU_Type):
            processEnvironmentalPDU((EnvironmentalPDU *)pdu);
            break;
#endif
         default:
            break;
          }
     numgot++;
  }   
  //return the number of pdus we have read
  return(numgot);
}

int delta_send(pfCoord *truepos, pfCoord *drpos)
//True if the angle or position is out of wack
{
  float rotdelta;
 
  if (MY_ABS(truepos->xyz[X],drpos->xyz[X]) > 
         G_static_data->DIS_position_delta) return(TRUE);
  if (MY_ABS(truepos->xyz[Y],drpos->xyz[Y]) > 
         G_static_data->DIS_position_delta) return(TRUE);
  if (MY_ABS(truepos->xyz[Z],drpos->xyz[Z]) > 
         G_static_data->DIS_position_delta) return(TRUE);
  rotdelta = MY_ABS(truepos->hpr[HEADING],drpos->hpr[HEADING]);
  if ( (G_static_data->DIS_orientation_delta<rotdelta) && 
       (rotdelta < 360.0f-G_static_data->DIS_orientation_delta) )
     return(TRUE);
  rotdelta = MY_ABS(truepos->hpr[PITCH],drpos->hpr[PITCH]);
  if ( (G_static_data->DIS_orientation_delta<rotdelta) && 
       (rotdelta < 360.0f-G_static_data->DIS_orientation_delta) )
     return(TRUE);
  rotdelta = MY_ABS(truepos->hpr[ROLL],drpos->hpr[ROLL]);
  if ( (G_static_data->DIS_orientation_delta<rotdelta) && 
       (rotdelta < 360.0f-G_static_data->DIS_orientation_delta))
     return(TRUE);
  //must be ok
  return(FALSE);
}

int find_victim()
// Return nearest victim's location. If no victims found, it returns the location
// of the driven vehicle.
{
  
  float  *begin, *location;
  pfVec3 loc, beg, diff;
  int veh, victim=G_drivenveh;
  float distance=0, min_distance= 99999999.9f;

  begin = G_vehlist[G_drivenveh].vehptr->getposition();
  pfSetVec3(beg, begin[X], begin[Y], begin[Z]);
 
  for (veh=1; veh< MAX_VEH; veh++) {
     if(((G_vehtype[G_vehlist[veh].type].cat == DI_GUY) || 
        (G_vehtype[G_vehlist[veh].type].cat == JACK_DI) ||
        (G_vehtype[G_vehlist[veh].type].cat == JACK_TEAM) ||
        (G_vehtype[G_vehlist[veh].type].cat == PERSON)) &&
        ((G_vehlist[veh].vehptr->getstatus() & Appearance_DamageDestroyed) ==
        Appearance_DamageDestroyed))     {
           location = G_vehlist[veh].vehptr->getposition();
           pfSetVec3(loc,location[X],location[Y],location[Z]);
           pfSubVec3(diff, loc, beg);
           distance = pfLengthVec3(diff); 
           if(distance < min_distance) {
                 min_distance = distance;
                 victim = veh;
           }
     }
  }

  if(victim == G_drivenveh)
	return -1;

  return  victim;

}

