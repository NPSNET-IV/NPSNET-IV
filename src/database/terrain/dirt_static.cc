// File: <dirt_static.cc>

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
#include <pfdb/pfflt.h>
#include <pfutil.h>
#include <pfdu.h>
#include <pfdu.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>

#include "terrain.h"
#include "common_defs2.h"
#include "fltcallbacks.h"
#include "fileio.h"
#include "munitions.h"
#include "bbox.h"

#include "database_const.h"
#include "collision_const.h"
#include "environment_const.h"
#include "loadBBoard.h"

// InitManager
#include "imstructs.h"
#include "imclass.h"

extern InitManager *initman;

// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          pfGroup      *G_fixed;
extern          VEHTYPE_LIST *G_vehtype;
extern          OBJTYPE_LIST *G_objtype;


//local variables used to keep trac of the terrain subtree
static pfGroup *L_q_mat_obj[NUMNODES][NUMNODES];
static pfGroup *L_q_mat_dirt[NUMNODES][NUMNODES];
static float L_binsize_x,L_inverse_binsize_x;
static float L_binsize_y,L_inverse_binsize_y;


int inplay(pfVec3 something)
// Returns true it the point is with in the current field of play's
// Bounding box, False otherwise
{
   static float bbox_xmin;
   static float bbox_xmax;
   static float bbox_ymin;
   static float bbox_ymax;

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

   if(((something[X] > bbox_xmin) &&
       (something[X] < bbox_xmax)) &&
      ((something[Y] > bbox_ymin) &&
       (something[Y] < bbox_ymax)))
     return TRUE;
   else
     return FALSE;
} 

// the following functions are used for the object hiearchy
pfGroup *findnode(pfVec3 loc)
/*finds which of the nodes this model belongs to*/
{
   int xindex,yindex;

#ifdef DEBUG
   cout << "xnode = "
        << ((int)(loc[X]) * L_inverse_binsize_x)
        << "ynode = "
        << ((int)(loc[Y]) * L_inverse_binsize_y)
        << endl;
#endif
   xindex = (int)floorf(loc[X] * L_inverse_binsize_x);
   yindex = (int)floorf(loc[Y] * L_inverse_binsize_y);
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
   return(L_q_mat_obj[xindex][yindex]);

}

pfGroup *deviy_up()
//divide terrain database to improve culling returns a pointer to a group
{
  int ix,jx;
  int maxarray = NUMNODES-1;
   //what size are bins?

#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   L_binsize_x = G_dynamic_data->bounding_box.xsize / float(NUMNODES);
   L_inverse_binsize_x = 1.0f/L_binsize_x;
   L_binsize_y = G_dynamic_data->bounding_box.ysize / float(NUMNODES);
   L_inverse_binsize_y = 1.0f/L_binsize_y;
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

   /*build the base layer*/
   for(ix=0;ix<NUMNODES;ix++){
     for(jx=0;jx<NUMNODES;jx++){
       L_q_mat_obj[ix][jx] = pfNewGroup();
     }
   }

   //now, build the quadtree to support rapid culling and intersection
   return(build_objtree(maxarray,0,maxarray,0,maxarray));
}

pfGroup *build_objtree(int nside, int sx, int ex, int sy, int ey)
//build the node for this level, or attach it to the L_q_mat_obj matrix
//nside is the number of base nodes that are on the side of this node's
// quad tree
{
  //number of nodes at this level
  int hside = nside/2;
  int pside = nside/2+1;
  pfGroup *tnode;
#ifdef DEBUG    
  cerr<<"Side cnt " <<nside<<"  "<<sx<<"  "<<ex<<"  "<<sy<<"  "<<ey<<"\n";
#endif
  if(nside == 0){
    //this is the base case
    return(L_q_mat_obj[sx][sy]);
  }

  //else we have to do some bigtime work
  tnode = pfNewGroup();
  
  pfAddChild(tnode,build_objtree(nside/2,sx      ,sx+hside,sy      ,sy+hside));
  pfAddChild(tnode,build_objtree(nside/2,sx      ,sx+hside,sy+pside,ey      ));
  pfAddChild(tnode,build_objtree(nside/2,sx+pside,ex      ,sy      ,sy+hside));
  pfAddChild(tnode,build_objtree(nside/2,sx+pside,ex      ,sy+pside,ey      ));

  //return tnode which is the root of the tree at this location
  return(tnode);
}

//And now for the ones for the dirt hiearachy used for the craters

pfGroup *findnode_dirt(pfVec3 loc)
/*finds which of the nodes this model belongs to*/
{

   int xindex,yindex;

#ifdef DEBUG
   cout << "xnode = "
        << ((int)(loc[X]) * L_inverse_binsize_x)
        << "ynode = "
        << ((int)(loc[Y]) * L_inverse_binsize_y)
        << endl;
#endif

/* ********* NOTE:  Need to offset for origin because this won't work *** */
/* *********        for negative coordinates */

   xindex = (int)floorf(loc[X] * L_inverse_binsize_x);
   yindex = (int)floorf(loc[Y] * L_inverse_binsize_y);
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
   return(L_q_mat_dirt[xindex][yindex]);

}

void deviy_up_dirt()
//divide terrain database to improve culling
{
  int ix,jx;
  int maxarray = NUMNODES-1;
   //what size are bins?

#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   L_binsize_x = G_dynamic_data->bounding_box.xsize / NUMNODES;
   L_inverse_binsize_x = 1.0f/L_binsize_x;
   L_binsize_y = G_dynamic_data->bounding_box.ysize / NUMNODES;
   L_inverse_binsize_y = 1.0f/L_binsize_y;
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

   /*build the base layer*/
   for(ix=0;ix<NUMNODES;ix++){
     for(jx=0;jx<NUMNODES;jx++){
       L_q_mat_dirt[ix][jx] = pfNewGroup();
       pfNodeTravMask(L_q_mat_dirt[ix][jx],PFTRAV_ISECT,
                  ALL_MASK,
                  PFTRAV_SELF|PFTRAV_DESCEND|PFTRAV_IS_CACHE, PF_SET);

     }
   }

   //now, build the quadtree to support rapid culling and intersection and
   //add it to the dirt hiearchy
   pfAddChild(G_fixed,build_dirttree(maxarray,0,maxarray,0,maxarray));
}

pfGroup *build_dirttree(int nside, int sx, int ex, int sy, int ey)
//build the node for this level, or attach it to the L_q_mat_dirt matrix
//nside is the number of base nodes that are on the side of this node's
// quad tree
{
  //number of nodes at this level
  int hside = nside/2;
  int pside = nside/2+1;
  pfGroup *tnode;
#ifdef DEBUG   
  cerr<<"Side cnt " <<nside<<"  "<<sx<<"  "<<ex<<"  "<<sy<<"  "<<ey<<"\n";
#endif
  if(nside == 0){
    //this is the base case
    return(L_q_mat_dirt[sx][sy]);
  }

  //else we have to do some bigtime work
  tnode = pfNewGroup();
  pfNodeTravMask(tnode,PFTRAV_ISECT,
             ALL_MASK,
             PFTRAV_SELF|PFTRAV_DESCEND|PFTRAV_IS_CACHE, PF_SET);
 
  pfAddChild(tnode,build_dirttree(nside/2,sx      ,sx+hside,sy      ,sy+hside));
  pfAddChild(tnode,build_dirttree(nside/2,sx      ,sx+hside,sy+pside,ey      ));
  pfAddChild(tnode,build_dirttree(nside/2,sx+pside,ex      ,sy      ,sy+hside));
  pfAddChild(tnode,build_dirttree(nside/2,sx+pside,ex      ,sy+pside,ey      ));

  //return tnode which is the root of the tree at this location
  return(tnode);
}

int JUNK = 0;
#define p cerr << JUNK++ << endl;

void readnplacemodels()
   // read the models in and place them on the terrain -- InitMan version
{
   int		i, j, ix;
   char		mfile[PF_MAXSTRING];
   char         tempstr[PF_MAXSTRING];
   pfCoord	indata;
   pfMatStack	*matstack;	// the matrix stack
   pfMatrix	mat;		// the matrix for the Static Coordinate Systems
   pfSCS	*objstat;	// the static coordinate node
   pfSwitch	*objswitch;	// the switch node for the current instance
				//   of the obj
   pfGroup     *objmodel;
   NODEDATA    *nodedata;	// the node data to blow up buildings
#ifdef STATIC_BBOX
   pfBox        bounding_box;
   pfSwitch    *bbox_geom;
#endif
   pfGroup      *dirt_node;

   struct Model	*im_models = (struct Model *) NULL;
   int           num_models;

   // Get Location File data
   if (initman->get_models((struct Model**) (&im_models), &num_models) == 0)
      return;


   // to make a static coordinate we have to jump through some hoops
   // basicly, what we have to do is create a matrix stack then manipulate it
   // allocate the stack
   matstack = pfNewMStack(1,NULL);

   // Create and clean out the array
   G_objtype = new OBJTYPE_LIST[MAX_STATIC_OBJ_TYPES];
   for (ix = 0; ix < MAX_STATIC_OBJ_TYPES; ix++) {
      G_objtype[ix].id = NOSHOW;
      G_objtype[ix].model = NULL;
   }

   for (i = 0; i < num_models; i++) {		// M lines
      int alive = im_models[i].alive_mod;
      G_objtype[alive].id = alive;
      G_objtype[alive].dead = im_models[i].dead_mod;
      strcpy(G_objtype[alive].name, im_models[i].flightfile);
      G_objtype[alive].cat = (vtype) im_models[i].otype;
      strcpy ( tempstr, G_objtype[alive].name );
      strcpy ( mfile, G_objtype[alive].name );
      if ( !pfFindFile ( tempstr, mfile, R_OK ) ) {
         sprintf(mfile, "%s.flt", G_objtype[alive].name);
      }

      if (strstr(mfile,".bb") == NULL) {
         if ((G_objtype[alive].model = (pfGroup*) pfdLoadFile(mfile)) == NULL) {
            cerr << "ERROR: Location File static model. "
                 << "Unable to load flt file " << mfile << endl;
         }
      }
      else {
         G_objtype[alive].model = (pfGroup*) LoadBBoard(mfile);
      }

      // Make a switch node for this object
      if (G_objtype[alive].id != G_objtype[alive].dead)
         G_objtype[alive].mswitch = pfNewSwitch();
      else
         G_objtype[alive].mswitch = NULL;

      // If there is an error in the file make it a beach ball
      if (G_objtype[alive].model == NULL) {
         cerr << "Alive model index " << alive
              << " not loaded.  Will try to use default.\n";
         G_objtype[alive].model = G_vehtype[0].model;
      }
      if (G_objtype[G_objtype[alive].dead].model == NULL) {
         cerr << "Dead model index " << G_objtype[alive].dead
              << " not loaded.\n";
         cerr << "  Referenced by alive model index " << alive << endl;
         cerr << "  Will try to use default.\n";
         G_objtype[G_objtype[alive].dead].model = G_vehtype[0].model;
      }

      if ( G_objtype[alive].mswitch != NULL ) { 
         pfAddChild(G_objtype[alive].mswitch, G_objtype[alive].model);
         pfAddChild(G_objtype[alive].mswitch,
                    G_objtype[G_objtype[alive].dead].model);
         pfSwitchVal(G_objtype[alive].mswitch,PFSWITCH_OFF);
      }

      for (j = 0; j < im_models[i].num_points; j++) {	// P lines
         indata.xyz[X] = im_models[i].points[j].x;
         indata.xyz[Y] = im_models[i].points[j].y;
         indata.xyz[Z] = im_models[i].points[j].z;
         indata.hpr[HEADING] = im_models[i].points[j].heading;
         indata.hpr[PITCH] = im_models[i].points[j].pitch;
         indata.hpr[ROLL] = im_models[i].points[j].roll;

         // find out where the ground is
         indata.xyz[Z] += gnd_level2(indata.xyz);

         // build an SCS
         pfResetMStack(matstack);
         pfPreTransMStack(matstack,indata.xyz[X],indata.xyz[Y],indata.xyz[Z]);
         if (indata.hpr[HEADING] != 0.0f) 
            pfPreRotMStack(matstack,indata.hpr[HEADING],0.0f,0.0f,1.0f);
         if (indata.hpr[PITCH] != 0.0f) 
            pfPreRotMStack(matstack,indata.hpr[PITCH],1.0f,0.0f,0.0f);
         if (indata.hpr[ROLL] != 0.0f) 
            pfPreRotMStack(matstack,indata.hpr[ROLL],0.0f,1.0f,0.0f);

#ifdef DEBUG_C
         pfScaleMStack(matstack,10.0f,10.0f,10.0f);
#endif

         pfGetMStack(matstack, mat);
         // Create a static coordinate system
         objstat = pfNewSCS(mat);

#ifdef STATIC_BBOX
         pfMakeEmptyBox(&bounding_box);
#endif

         if (G_objtype[alive].mswitch != NULL) {
            objswitch = (pfSwitch *) pfClone(G_objtype[alive].mswitch, 0);
#ifdef STATIC_BBOX
            pfuTravCalcBBox (objswitch, &bounding_box);
#endif
            pfAddChild(objstat, objswitch);
            pfSwitchVal(objswitch, PF_ON);
         } else {
            objmodel = (pfGroup *) pfClone(G_objtype[alive].model, 0);
#ifdef STATIC_BBOX
            pfuTravCalcBBox(objmodel, &bounding_box);
#endif
            pfAddChild(objstat, objmodel);
         }

#ifdef STATIC_BBOX
         bbox_geom = make_bounding_box(&bounding_box);
         if (bbox_geom != NULL)
            pfAddChild(objstat, bbox_geom);
         else
            cerr << "ERROR making bounding box geometry." << endl;
#endif
         pfFlatten(objstat, 0);
         // add it to the quad tree and turn on the geometry
         pfSync();

         dirt_node = findnode_dirt(indata.xyz);
         if (G_objtype[alive].mswitch != NULL) {
            pfAddChild(dirt_node, objswitch);
            pfSwitchVal(objswitch, ALIVE);
         } else
            pfAddChild(dirt_node,objmodel);

#ifdef STATIC_BBOX
         if (bbox_geom != NULL)
            pfAddChild(dirt_node, bbox_geom);
#endif

         // set up the traversal call back so we know what we hit
         nodedata = new NODEDATA;
         if (G_objtype[alive].mswitch != NULL) {
            nodedata->node = objswitch;
            pfNodeTravFuncs(objswitch,PFTRAV_ISECT,switch_store,NULL);
            pfNodeTravData(objswitch,PFTRAV_ISECT,nodedata);
            set_intersect_mask((pfGroup *)objswitch, 
                                DESTRUCT_MASK, COLLIDE_DYNAMIC);
         } else {
            nodedata->node = (pfSwitch *)NULL;
            pfNodeTravFuncs(objmodel,PFTRAV_ISECT,switch_store,NULL);
            pfNodeTravData(objmodel,PFTRAV_ISECT,nodedata);
            set_intersect_mask((pfGroup *)objmodel,
                                DESTRUCT_MASK, COLLIDE_DYNAMIC);
         }

      } // for each P
   } // for each M

   // Set to hit Buildings and other static objects
   //set_intersect_mask(G_obj, BUILDING_MASK, COLLIDE_DYNAMIC);

} // end readnplacemodels() -- InitMan version


/*
void readnplacemodels(char model_location_file[])
//read the models in and place them on the terrain
{
   char comment[200];
   int alive, dead, cat;
   char modname[100],moddir[100];
   char tag;
   pfCoord indata;
   pfMatStack *matstack; //the matrix stack
   pfMatrix mat; //the matrix for the Static Coordinate systems
   pfSCS *objstat;  // the static coordinate node
   pfSwitch *objswitch; //the switch node for the current instance of the obj
   pfGroup *objmodel;
   NODEDATA *nodedata; //the node data to blow up buildings
#ifdef STATIC_BBOX
   pfBox bounding_box;
   pfSwitch *bbox_geom;
#endif
   pfGroup *dirt_node;


// This needs to be changed so that Bill Boards are loaded correctly
  //read the file
  while (infile >> tag)
     {
     switch (tag)
      {
         infile >> modname >> moddir >> alive >> dead  >> cat;
         if ( !infile )
            {
            if ( infile.eof() )
               {
               cerr << "Premature EOF reached reading static location file.\n";
               cerr << "EOF reached during B command.\n";
               }
            else
               {
               infile.clear();
               infile.getline(comment,200);
               cerr << "ERROR reading static location file, error in B line:\n";
               cerr << "B " << comment << endl;
               }
            }
         else
            {

#ifdef DEBUG_C
            cerr << "Model Name " << modname << " Type id " << alive << " Dead "                 << dead << " cat "<<cat << "\n";
#endif
 
            if((alive > MAX_STATIC_OBJ_TYPES) || (dead > MAX_STATIC_OBJ_TYPES))
               {
               cerr << "ERROR reading static location file.\n";
               cerr << "Bad Model index  "<< modname << "   Alive " << alive
                    << "   Dead " << dead << endl;
               cerr << "Maximum allowable index is " << MAX_STATIC_OBJ_TYPES
                    << endl;
               }
            else
               {

               G_objtype[alive].id = alive;
               G_objtype[alive].dead = dead;
               strcpy(G_objtype[alive].name,modname);
               G_objtype[alive].cat = (vtype)cat;

               sprintf(mfile,"%s/%s.bb",moddir,modname);

#ifdef DEBUG_C
               cerr << "Reading " << mfile << "\n";
#endif
               if (NULL == (G_objtype[alive].model = 
                            (pfGroup *)LoadBBoard(mfile))) {
                  cerr << "ERROR:  Static model location file.\n";
                  cerr << "Unable to load the bb file " << mfile << endl;
                  }

#ifdef DEBUG_C
               cerr << "Making Switch node "<< G_objtype[alive].name
                    << " id " << alive << "\n";
#endif

               //make a switch node for this object
               if ( G_objtype[alive].id != G_objtype[alive].dead )
                  G_objtype[alive].mswitch = pfNewSwitch();
               else
                  G_objtype[alive].mswitch = NULL;

               //if there is an error in the file make it a beach ball
               if(G_objtype[alive].model == NULL)
                  {
                  cerr << "Alive model index " << alive
                       << " not loaded.  Will try to use default.\n";
                  G_objtype[alive].model = G_vehtype[0].model;
                  }
               if(G_objtype[G_objtype[alive].dead].model == NULL)
                  {
                  cerr << "Dead model index " << dead << " not loaded.\n";
                  cerr << "  Referenced by alive model index " << alive
                       << endl;
                  cerr << "  Will try to use default.\n";
                  G_objtype[G_objtype[alive].dead].model = G_vehtype[0].model;
                  }
 
               if ( G_objtype[alive].mswitch != NULL )
                  {
                  pfAddChild(G_objtype[alive].mswitch,G_objtype[alive].model);
                  pfAddChild(G_objtype[alive].mswitch,
                             G_objtype[G_objtype[alive].dead].model);
                  pfSwitchVal(G_objtype[alive].mswitch,PFSWITCH_OFF);
                  }
               }
            }
      case 'P': //an instance of the model
         //find out where the ground is
            if(tag == 'P') //relative Z value
              indata.xyz[Z] += gnd_level2(indata.xyz);
            //build an SCS
            pfResetMStack(matstack);
            pfPreTransMStack(matstack,indata.xyz[X],indata.xyz[Y],indata.xyz[Z]);
            if(indata.hpr[HEADING] != 0.0f) 
               pfPreRotMStack(matstack,indata.hpr[HEADING],0.0f,0.0f,1.0f);
            if(indata.hpr[PITCH] != 0.0f) 
               pfPreRotMStack(matstack,indata.hpr[PITCH],1.0f,0.0f,0.0f);
            if(indata.hpr[ROLL] != 0.0f) 
               pfPreRotMStack(matstack,indata.hpr[ROLL],0.0f,1.0f,0.0f);


            pfGetMStack(matstack,mat);
            //Create a static coordinate systems
            objstat = pfNewSCS(mat);

#ifdef STATIC_BBOX
            pfMakeEmptyBox(&bounding_box);
#endif
 
            if ( G_objtype[alive].mswitch != NULL ) 
               {
               objswitch = (pfSwitch *)pfClone(G_objtype[alive].mswitch,0);
#ifdef STATIC_BBOX
               pfuTravCalcBBox ( objswitch, &bounding_box );
#endif
               pfAddChild(objstat,objswitch);
               pfSwitchVal(objswitch,PF_ON);
               }
            else
               {
               objmodel = (pfGroup *)pfClone(G_objtype[alive].model,0);
#ifdef STATIC_BBOX
               pfuTravCalcBBox ( objmodel, &bounding_box );
#endif
               pfAddChild(objstat, objmodel );
               }
//cerr<<"starting sync..."<<endl;

#ifdef STATIC_BBOX
            bbox_geom = make_bounding_box(&bounding_box);
            if ( bbox_geom != NULL )
               pfAddChild(objstat,bbox_geom);
            else
               cerr << "ERROR making bounding box geometry." << endl;
#endif
            pfFlatten(objstat,0);
              //add it to the quad tree and turn on the geometry
            pfSync();
//cerr<<"done sync..."<<endl;
            dirt_node = findnode_dirt(indata.xyz);
            if ( G_objtype[alive].mswitch != NULL )
               {
               pfAddChild(dirt_node,objswitch);
               pfSwitchVal(objswitch,ALIVE);
               }
            else
               pfAddChild(dirt_node,objmodel);

#ifdef STATIC_BBOX
            if ( bbox_geom != NULL )
               pfAddChild(dirt_node,bbox_geom);
#endif

            //set up the traversal call back so we know what we hit
            nodedata = new NODEDATA;
            if ( G_objtype[alive].mswitch != NULL )
               {
               nodedata->node = objswitch;
               pfNodeTravFuncs(objswitch,PFTRAV_ISECT,switch_store,NULL);
               pfNodeTravData(objswitch,PFTRAV_ISECT,nodedata);
               set_intersect_mask((pfGroup *)objswitch, 
                                   DESTRUCT_MASK, COLLIDE_DYNAMIC);
               }
            else
               {
               nodedata->node = (pfSwitch *)NULL;
               pfNodeTravFuncs(objmodel,PFTRAV_ISECT,switch_store,NULL);
               pfNodeTravData(objmodel,PFTRAV_ISECT,nodedata);
               set_intersect_mask((pfGroup *)objmodel,
                                   DESTRUCT_MASK, COLLIDE_DYNAMIC);
               }
         break; 
      }
    }
}
*/
