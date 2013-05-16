/***************************************************************************
Title: mypvs.C
Author:  Bryan Stewart
Date:    11/29/95

Purpoose: A hybrid pvs algorithm from UNC pfPortals and the current pvs.h
*****************************************************************************/
#include <Performer/pf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h> 
#include <unistd.h>

#include <fcntl.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

#include "common_defs2.h"
#include "macros.h"
#include "pvs.h"
#include "picking.h"
#include "collision_const.h"

/*
 * Globals visible outside this file
 */
pfGroup		*PVSCells[MAXPVSCELLS];		// The PVS array of cells
pfSwitch	*water_switch=NULL;		// Switch to external water

// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile int              G_drivenveh;
extern          VEH_LIST        *G_vehlist;

/************************************************************************
 *									*		
 * 			initPVS						*
 *									*
 ************************************************************************/

pfGroup  *initPVS()
{
   ifstream     infile("./models/SurfaceVeh/pvsData.dat");
   int		n = 0;
   int		ix = 0;		// index  
   int		jx, num;
   PVSData	*pvsData;
   pfGroup	*Locator;

   Locator = pfNewGroup();
   LocatorData     *LocDat = (LocatorData *)pfMalloc(sizeof(LocatorData),
                                                     pfGetSharedArena());
   LocDat->lastcell = -1;
   pfNodeTravData(Locator,PFTRAV_CULL,LocDat);

   for (ix=0; ix < MAXPVSCELLS; ix++){
      LocDat->cells[ix] = pfNewSwitch();
      pfSwitchVal(LocDat->cells[ix],PFSWITCH_OFF);
      pfNodeTravMask ( LocDat->cells[ix], PFTRAV_ISECT, PERMANENT_MASK,
                       PFTRAV_SELF|PFTRAV_DESCEND|PFTRAV_IS_UNCACHE, PF_SET);
   }

   while ( !infile.eof() )
   {
      infile >> ix >> num;  //cerr<<"cell "<<ix<<":";
      if (PVSCells[ix]==NULL) continue;   //Cludge fix to handle end of file problem
      pvsData = (PVSData *)pfMalloc(sizeof(PVSData), pfGetSharedArena());
      LocDat->bboxes[ix] = (pfBox *)pfMalloc(sizeof(pfBox), pfGetSharedArena());
      if (num==1) {
          pfSetVec3(LocDat->bboxes[ix]->min, 0.0f, 0.0f, -10000.0f);
          pfSetVec3(LocDat->bboxes[ix]->max, 0.0f, 0.0f, -10000.0f);
      }
      else {
          pfuTravCalcBBox(PVSCells[ix], LocDat->bboxes[ix]);
          if(LocDat->bboxes[ix]->max[2] - LocDat->bboxes[ix]->min[2] < 2.5f) {
             //cerr<<"Updating bbox height...";
             LocDat->bboxes[ix]->max[2] = LocDat->bboxes[ix]->min[2] + 5.0f;
          }
      }
      pfAddChild(LocDat->cells[ix],PVSCells[ix]);
      pfAddChild(Locator,LocDat->cells[ix]);
      for (jx=0; jx < MAXPVSCELLS;jx++)   //clear PVSData list
          pvsData->list[jx] = 0;
      for (jx=0; jx < num; jx++)           //Setup PVSData list for cell ix
      {
          infile >> n; //cerr<<n<<" ";
	  pvsData->list[n] = 1;
      }
      //cerr<<endl;
      LocDat->cellinfo[ix] = pvsData;
   }
   //Make the last cell's bbox be for the whole ship, so you know when you 
   //Have to check for if you are within the ship's domain
   LocDat->bboxes[26] = (pfBox *)pfMalloc(sizeof(pfBox), pfGetSharedArena());
   pfuTravCalcBBox(Locator, LocDat->bboxes[26]);
   //cerr<<"Done..."<<endl;
   infile.close();
   return Locator;
}

/*
 * Function:	UpdatePVS
 * Description:	This function is associated with the Locator, a pfGroup node
 *		near the root of the scene. All of the cells in the scene are
 *		children of the Locator; this callback determines which cells
 *		are visible to the viewer and enables those cell. We do this
 *		by finding and traversing the cell containing the viewer,
 *		recursively traversing their attached cells. If the user is
 *		not within any cell, all children of the Locator are enabled
 *		so that all cells get traversed.
 */
void updatePVS(pfGroup *node, pfCoord posture)
{
    LocatorData		*data = (LocatorData *)pfGetNodeTravData(node, PFTRAV_CULL);
    PVSData		*curdata;
    pfVec3		 localviewpoint,viewpoint,vieworient;
    pfMatrix		 orient;
    float		 veclength;
    float               *vid_pos;
    int 		 i;
    
//cerr<<"enterUpdatePVS!"<<endl;
    if (node == NULL)
    {
	cerr<<"ERROR: enterUpdatePVS called with node == NULL"<<endl;
	return;
    }

    // Get user's location based on type of driven vehicle
    if (G_vehlist[G_drivenveh].vehptr->gettype () == SHIP) {
       pfGetChanView(G_dynamic_data->pipe[0].channel[0].chan_ptr, 
                     viewpoint, vieworient);
    }
    else {
       vid_pos = G_vehlist[G_drivenveh].vehptr->getposition();
       pfSetVec3(viewpoint,vid_pos[X],vid_pos[Y],vid_pos[Z]+1.0f);
    }

    pfSubVec3(localviewpoint, viewpoint, posture.xyz);
    veclength = pfLengthVec3(localviewpoint);
    pfNormalizeVec3(localviewpoint);
    pfMakeEulerMat ( orient, -posture.hpr[HEADING],
                             posture.hpr[PITCH], 
                             posture.hpr[ROLL]);
    pfXformVec3(localviewpoint,localviewpoint,orient);
    pfScaleVec3(localviewpoint,veclength,localviewpoint);

/*
      cerr<<"Locator: viewpoint = ["
        <<localviewpoint[0]<<" "<<localviewpoint[1]<<" "<<localviewpoint[2]
        <<"]";
*/

    if (data->lastcell != -1 && data->lastcell < 26) {
        if (pfBoxContainsPt(data->bboxes[data->lastcell], localviewpoint)) {
	   /* If still in the same cell as last frame, traverse it again */
	   //cerr<<"Locator: still in "<<data->lastcell<<endl;
           return;
        }
    }
    else if(data->lastcell == 26) {
            if (!pfBoxContainsPt(data->bboxes[26], localviewpoint)) return;
         }

    /* Otherwise, test against each of the cell's bboxes */
    //cerr<<"checking cells...";
    for (i=0;i < MAXPVSCELLS-1; i++)
    {
        //cerr<<i<<"/";
	if (pfBoxContainsPt(data->bboxes[i], localviewpoint))
	{
            //if (i==data->lastcell) return;   Fix for exterior selection
	    //cerr<<"Locator: now in cell "<<i<<endl;
            curdata = data->cellinfo[i];
            for(int ix=0;ix<MAXPVSCELLS;ix++){
               if(curdata->list[ix]) {
                  //cerr<<"Adding Child "<<ix<<endl;
                  pfSwitchVal(data->cells[ix],PFSWITCH_ON);
                  //pfAddChild(myPick->root,data->cells[ix]);
               }
               else {
                       //cerr<<"Removing Child "<<ix<<endl;
                       //pfRemoveChild((pfGroup *)node, data->cells[ix]);
                       pfSwitchVal(data->cells[ix],PFSWITCH_OFF);
                       //pfRemoveChild(myPick->root,data->cells[ix]);
                    }
            }

            // Set water_switch on/off depending on whether it is visible
            if(water_switch != NULL) {
               if(i<11 || (i>11 && i<16)) { //water not visible
                  pfSwitchVal(water_switch, PFSWITCH_OFF);
               }
               else { //water is visible
                  pfSwitchVal(water_switch, PFSWITCH_ON);
               }
            }
            else {
               cerr<<"Water_Switch not set..."<<endl;
            }

            data->lastcell = i;
            return;
	}
    }
    // User is not in any cell, so show exterior only
    if (data->lastcell == 26) return;
//cerr<<"Locator: not in any cell currently"<<endl;
    for(int ix=0;ix<MAXPVSCELLS;ix++){
        if(ix == 11 || ix==12 || (ix>=18 && ix<=25) ) {
           pfSwitchVal(data->cells[ix],PFSWITCH_ON);
        }
        else {
                pfSwitchVal(data->cells[ix],PFSWITCH_OFF);
             }
    }
    pfSwitchVal(water_switch, PFSWITCH_ON);
    data->lastcell = 26;
    
    return;
}


// Routine to get pointers to terrain_cells in external database
void addWaterCell(pfSwitch *sw)
{
   water_switch = sw;
}

void removeWaterCell()
{
   pfSwitchVal(water_switch, PFSWITCH_ON);
}
