/*
 *  object.cc
 *	This code was written as part of the NPS Ship Simulator Project
 *	for the thesis of Bryan Stewart.
 *	This contains the source code to manipulate the
 *	objects the user can control in the shipboard
 *	walkthrough. For a description of the functions 
 *      in this file, please see object.h
 *
 *    Authors: Bryan Stewart
 *    Thesis Advisors:  Prof. John Falby
 *			Dr. Mike Zyda
 *    Date:  07 December 1995
 *
 *    Modified:
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <bstring.h>
#include <math.h>
#include <fmclient.h>

#include <Performer/pf.h>
#include <pfdu.h>
#include "pfdb/pfflt.h"
#include "prmath.h"
#include "pfutil.h"
#include "common_defs2.h"
#include "object.h"
//#include "picking.h"
#include "ship_constants.h"
#include "pvs.h"
//#include "matrix.h"
#include "collision_const.h"


/************************************************************************
 *									*		
 * 		      Global Variables for object.c			*
 *									*
 ************************************************************************/

/*
int	actualManip = 0;
int	actualInfo = 0;
int	actualMulti = 0;
int	highestManip = -5;
int	actualGauges = 0;
int	actualDCS = 0;
int 	openObject;		//global variable to control door swing

int	nozzleUpdate;
int	fireUpdate;
int	halonUpdate;
int	steamUpdate;
int	FOUpdate;
int	ventUpdate;
*/

int	actualDoors = 0;
int	actualSwitches = 0;
int	actualValves = 0;
int	actualButtons = 0;



/* 	The maximum potential DOF nodes, objects which can
 *	be manipulated, information node, and intersection
 *	nodes.

enum	{ maxDOF = 16 };
enum	{ maxManipulate = 16};
enum	{ maxDiffMulti = 16};
enum	{ maxTotalMulti = 128};
enum	{ maxValves = 8 };
enum	{ maxGauges = 8};
enum	{ maxDoors = 16};
*/

//char		 sound[16];
pfNode	        *multiMatrix[MAX_INFO];
pfSwitch        *ShipSwitches[MAX_SWITCHES];
pfGroup         *ShipValves[MAX_VALVES];
pfGroup         *ShipButtons[MAX_BUTTONS];
pfGroup         *ShipDoors[MAX_DOORS];
infodataStruct  *ship_info_data;

extern volatile DYNAMIC_DATA *G_dynamic_data;
extern          VEH_LIST     *G_vehlist;
extern volatile int           G_drivenveh;

/************************************************************************
 *									*		
 * 		  Declaration of functions local to object.c		*
 *									*
 ************************************************************************/


/*
 * openFlightCallBack-
 *
 *	This function is the callback used by the flight
 *	loader.  It reads the database's special fields and 
 *	uses those values to control the loading of the
 *	database, controling such aspects as potentially
 *	visible sets, intersection masks, and objects which 
 *	can be manipulated.
 *
 */
extern	void
openFlightCallBack ( pfNode *node, int mgOp, int *cbs, COMMENTcb *cbcom, void * );


/*
 * registerNode-
 *
 *	This extern function is used by the flt loader to
 *	add the loading callback.
 *
 */
//extern void	
//( *registerNode ) ( pfNode *node, int mgOp, int *cbs, COMMENTcb *cbcom, void * );

/************************************************************************
 *									*		
 * 			openFlightCallBack				*
 *									*
 ************************************************************************/

void
openFlightCallBack ( pfNode *node, int mgOp, int *cbs, COMMENTcb *cbcom, void * )
{
    infodataStruct	*infodata;
    switch ( mgOp )
     {
	case CB_SWITCH:
 	{
	   //SWITCHcb	*SwitchData = (SWITCHcb *) cbs;
	   pfSwitch	*SwitchNode = (pfSwitch *) node;
#ifdef VERBOSE
	    fprintf( stderr,
		"OPENFLIGHT: found CB_SWITCH, switch[] = %s.\n",
		 SwitchData->id );
#endif
            infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                  pfGetSharedArena());
            infodata->type = MANIP_MASK; //Assign type value
            strcpy(infodata->name,"");
            strcpy(infodata->function,"");
            strcpy(infodata->soundbite,"");
            infodata->index = -1;       //indicate nozzle data
            infodata->perCent = 0.0f;	//nozzle percent open
            infodata->dof = NULL;	//Degree of Freedom information
            pfNodeTravData(SwitchNode,PFTRAV_ISECT,infodata);
            ShipSwitches[actualSwitches++] = SwitchNode;
            //This was to remove compiler warning
            //pfNodeTravData(SwitchNode,PFTRAV_ISECT,SwitchData);
        break;
        }
	case CB_DOF:
 	{
	    DOFcb	*dofData = (DOFcb *) cbs;
	    pfDCS	*dcsNode = (pfDCS *) node;
	    
	    // ----------------------------------------------------------------  
	    // OpenFlight loader already initialized DCS matrix.		 
	    // Remember DCS node and DOF data for later updates.		 
	    // ----------------------------------------------------------------  

#ifdef VERBOSE
            cerr<<"OPENFLIGHT: found CB_DOF "<<dofData->id<<endl;
#endif
            infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                  pfGetSharedArena());
            infodata->type = DCS_MASK; //Assign type value
            strcpy(infodata->name,"");
            strcpy(infodata->function,"");
            strcpy(infodata->soundbite,"");
            infodata->perCent = 0.0f;	//Valve percent open
            infodata->dof = dofData;	//Degree of Freedom information
            pfNodeTravData(dcsNode,PFTRAV_ISECT,infodata);
	break;
 	}

	case CB_GROUP:
 	{
	   GROUPcb	*GroupData = (GROUPcb *) cbs;
	   pfGroup	*GroupNode = (pfGroup *) node;

	   switch (GroupData->special1)
 	   {
	      case PVS_CB:
#ifdef VERBOSE
	 	  fprintf( stderr,
	 		"OPENFLIGHT: found PVS_CB, PVSMatrix[ %d ] = %s.\n",
			GroupData->special2, GroupData->id );
#endif
                  PVSCells[ GroupData->special2 ] = GroupNode;
	      break;


	      case HARD_INTERSECT_CB:
#ifdef VERBOSE
                 cerr<<"OPENFLIGHT: found Group INTER_CB "<<GroupData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = HARD_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
	      break;

	      case MANIPULATE_CB:
#ifdef VERBOSE
                 cerr<<"OPENFLIGHT: found Group MANIP_CB "<<GroupData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = MANIP_MASK;   //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 infodata->index = GroupData->special2;
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
	      break;
		
	      case INFORMATION_CB:
#ifdef VERBOSE
                 cerr<<"OPENFLIGHT: found Group INFO_CB "<<GroupData->id<<endl;
#endif
                 if( GroupData->special2 )
                     PVSCells[ GroupData->special2 ] = GroupNode;
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = INFO_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
	      break;


	      case LDR_CB:
#ifdef VERBOSE
                 cerr<<"OPENFLIGHT: found Group LDR_CB "<<GroupData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = LADDER_MASK;  //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
	      break;


	      case DECK_CB:
#ifdef VERBOSE
                 cerr<<"OPENFLIGHT: found Group DECK_CB "<<GroupData->id<<endl;
#endif
                 if( GroupData->special2 )
                     PVSCells[ GroupData->special2 ] = GroupNode;
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = DECK_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
	      break;

	      case BLKD_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Group BLKD_CB "<<GroupData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = BULK_MASK;  //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
	      break;

	      case DEFN_MULT_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Group DEFN_MULT_CB "<<GroupData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = MULT_MASK;  //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 multiMatrix[GroupData->special2] = (pfNode*) GroupNode;
                 infodata->index = GroupData->special2;
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
	      break;

	      case MULT_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Group MULT_CB "<<GroupData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = MULT_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 infodata->index = GroupData->special2;
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
	      break;

	      case GAUGE_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Group GAUGE_CB "<<GroupData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = INFO_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
	      break;

	      case VALVE_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Group VALVE_CB "<<GroupData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = VALVE_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 infodata->index = actualValves;//index
                 infodata->perCent = 1.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
                 ShipValves[actualValves++] = GroupNode;
	      break;

	      case DOOR_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Group DOOR_CB "<<GroupData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = DOOR_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 infodata->index = actualDoors;	//index
                 infodata->perCent = 0.0f;	//Door percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(GroupNode,PFTRAV_ISECT,infodata);
                 ShipDoors[actualDoors++] = GroupNode;
	      break;
	   default: break;
	   }
	}

	case CB_OBJECT:
 	{
	   OBJcb	*ObjectData = (OBJcb *) cbs;
	   pfGroup	*ObjectNode = (pfGroup *) node;

	   switch (ObjectData->special1)
 	   {
	      case HARD_INTERSECT_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Obj INTER_CB "<<ObjectData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = HARD_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(ObjectNode,PFTRAV_ISECT,infodata);
              break;

	      case MANIPULATE_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Obj MANIP_CB "<<ObjectData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = MANIP_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 infodata->index = ObjectData->special2;
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(ObjectNode,PFTRAV_ISECT,infodata);
                 ShipButtons[actualButtons++] = ObjectNode;
	      break;

	      case INFORMATION_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Obj INFO_CB "<<ObjectData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = INFO_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(ObjectNode,PFTRAV_ISECT,infodata);
	      break;

	      case LDR_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Obj LDR_CB "<<ObjectData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = LADDER_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(ObjectNode,PFTRAV_ISECT,infodata);
	      break;

	      case DECK_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Obj DECK_CB "<<ObjectData->id<<endl;
#endif
                 if( ObjectData->special2 )
                     PVSCells[ ObjectData->special2 ] = ObjectNode;
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = DECK_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(ObjectNode,PFTRAV_ISECT,infodata);
	      break;

	      case BLKD_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Obj BLKD_CB "<<ObjectData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = BULK_MASK;  //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(ObjectNode,PFTRAV_ISECT,infodata);
	      break;


	      case DEFN_MULT_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Obj DEFN_MULT_CB "<<ObjectData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = MULT_MASK;  //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 sscanf(cbcom->text,"%[^\n] %[^\n] %[^\n]", 
                        infodata->name,infodata->function,infodata->soundbite);
                 multiMatrix[ObjectData->special2] = (pfNode*) ObjectNode;
                 infodata->index = ObjectData->special2;
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(ObjectNode,PFTRAV_ISECT,infodata);
	      break;


	      case MULT_CB:
#ifdef VERBOSE	 
                 cerr<<"OPENFLIGHT: found Obj MULT_CB "<<ObjectData->id<<endl;
#endif
                 infodata = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                       pfGetSharedArena());
                 infodata->type = MULT_MASK;    //Assign type value
                 strcpy(infodata->name,"");
                 strcpy(infodata->function,"");
                 strcpy(infodata->soundbite,"");
                 infodata->index = ObjectData->special2;
                 infodata->perCent = 0.0f;	//Valve percent open
                 infodata->dof = NULL;		//Degree of Freedom information
                 pfNodeTravData(ObjectNode,PFTRAV_ISECT,infodata);
	      break;

	      default: break;

    	    }

	}
        default:
        break;
   
  }
}


/************************************************************************
 *									*		
 * 			openFlightConfig				*
 *									*
 ************************************************************************/

pfGroup *
openFlightConfig (char *mfile)
{

    pfGroup	*root;

    // Initialized all ship arrays
    //for (ix=0; ix < MAX_INFO; ix++) multiMatrix[ix] = NULL;
    //for (ix=0; ix < MAX_SWITCHES; ix++) ShipSwitches[ix] = NULL;
    //for (ix=0; ix < MAX_VALVES; ix++) ShipValves[ix] = NULL;
    //for (ix=0; ix < MAX_BUTTONS; ix++) ShipButtons[ix] = NULL;
    //for (ix=0; ix < MAX_DOORS; ix++) ShipDoors[ix] = NULL;

#ifdef VERBOSE
    cerr<<"OPENFLIGHT: entering openFlightConfig()."<<endl;
#endif
    
    // ------------------------------------------------------------------------  
    // Initialize OpenFlight loader callback function pointer.			 
    // ------------------------------------------------------------------------  

    // registerNode = openFlightCallBack;

    fltRegisterNodeT pFunc =  openFlightCallBack;
    void *hFunc            = &pFunc;
    pfdConverterAttr_flt (PFFLT_REGISTER_NODE, hFunc);

    pfdConverterMode_flt(PFFLT_CLEAN, FALSE);
    root = (pfGroup *)pfdLoadFile(mfile); 

    ship_info_data = (infodataStruct *)pfMalloc(sizeof(infodataStruct),
                                                pfGetSharedArena());
    
#ifdef VERBOSE
    cerr<<"OPENFLIGHT: exiting openFlightConfig()."<<endl;
#endif

    return root;
}


/************************************************************************
 *									*		
 * 			infodisplay					*
 *									*
 ************************************************************************/

void infodisplay(char *name, char *function, char *sound)
{
   cerr<<"***************************************"<<endl;
   cerr<<"** Name: "<<name<<endl;
   cerr<<"** Function: "<<function<<endl;
   cerr<<"** Sound: "<<sound<<endl;
   cerr<<"***************************************"<<endl;

   strcpy(ship_info_data->name,name);
   strcpy(ship_info_data->function,function);
}




void ship_draw_info( pfChannel *chan, void *data )
   {
   static int first_time = TRUE;
   static float view_right;
   static float view_left; 
   static float view_top; 
   static float view_bottom;
   static float right; 
   static float top; 
   static float left; 
   static float bottom;
   //static infodataStruct *info_data;
   static char text[255];
#ifdef IRISGL
   float line, column;
   static fmfonthandle our_font;
#endif
   static float font_height;
   //static float font_width;
   long orignal_trans_mode = pfGetTransparency();
   static pfMatrix identity;

   //Used to remove compiler warning;
   data;

   if ( first_time )
      {
      float width, height,temp;
      /* This twisted convoluted next statement is just to use */
      /* the chan parameter to keep the compiler happy */
      if ( chan == NULL )
         first_time = FALSE;
      else
         first_time = FALSE;
      width = (G_dynamic_data->view.right - G_dynamic_data->view.left);
      height = (G_dynamic_data->view.top - G_dynamic_data->view.bottom);
      view_top = G_dynamic_data->view.top;
      temp = (0.667f * height);
      view_bottom = G_dynamic_data->view.bottom + temp;
      view_left = G_dynamic_data->view.left;
      view_right = G_dynamic_data->view.right; //(temp*(height/width));

      
      font_height = ((view_top - view_bottom) * 14.0f) / 341.0f;
#ifdef IRISGL
      our_font = fmscalefont(fmfindfont("Times-Roman"), font_height);
#else //OPENGL
#endif
      //font_width = float(fmgetstrwidth(our_font,"1, 2, 333, 4, 55, 6"))/19.0f;
      right = (view_right - view_left); //  /(font_width);
      top = (view_top - view_bottom)/(1.3f * font_height);
      left = 0.0f;
      bottom = 0.0f;
      pfMakeIdentMat ( identity );
      }

   // Save Performer state and load identity matrix. Same as hud_mode in draw.cc
   pfPushState();
   pfBasicState();
#ifdef IRISGL
   pushmatrix();
   loadmatrix(identity);
   zfunction(ZF_ALWAYS);

   ortho2 ( left, right, bottom, top );
#else //OPENGL
   // We have to store the original matrix because OpenGL multiplys the
   // matrices using gluOrtho, but GL does not.
   pfPushMatrix();
   gluOrtho2D( left, right, bottom, top );
//   pfVec2 v; //vertex for drawing lines
#endif

   pfOverride(PFSTATE_TRANSPARENCY, PF_OFF );
   pfTransparency(PFTR_ON);
   pfOverride(PFSTATE_TRANSPARENCY, PF_ON );

#ifdef IRISGL
   cpack (0x5F800000);
//   rectf ( left, bottom, right, top );

   RGBcolor (238, 201, 0);
   fmsetfont(our_font);

   //font(0);
   column = 0.1f;
   line = 6.0f;
   cmov2 (column, line ); 
   sprintf (text, " Name: %s", ship_info_data->name);
   fmprstr(text);
   
   //font(0);
   line = 3.0f;
   cmov2 (column, line );
   sprintf (text, " Function: %s", ship_info_data->function);
   fmprstr(text);   
#else //OPENGL
   glColor4f(0.0f, 0.0f, 0.5f, 0.37f);
   // Draw A rectangle on screen to border text
   glRectf( left, bottom, right, top );
   glColor4f(0.92f, 0.79f, 0.0f, 1.0f);
   sprintf (text, " Name: %s", ship_info_data->name);
   pfuDrawStringPos(text,0.1f,6.0f,0.0f);
   sprintf (text, " Function: %s", ship_info_data->function);
   pfuDrawStringPos(text,0.1f,3.0f,0.0f);
#endif

   pfOverride(PFSTATE_TRANSPARENCY, PF_OFF );
   pfTransparency(orignal_trans_mode);
   pfOverride(PFSTATE_TRANSPARENCY, PF_ON );
   
   // Reset Performer state. Same as performer_mode() in draw.cc
#ifdef IRISGL
   popmatrix();
   zfunction(ZF_LEQUAL);
#else //OPENGL
   pfPopMatrix();
#endif
   pfPopState();
   }





