//**************************************************************
// File: ship_walk_veh
// Purpose:  Class definition for Walkthrough ships
//**************************************************************

#include <math.h>
#include <string.h>
#include <bstring.h>
#include <iostream.h>

#include "manager.h"
#include "vehicle.h"
#include "ship_veh.h"
#include "ship_walk_veh.h"
#include "entity.h"
#include "common_defs2.h"
#include "appearance.h"
#include "disnetlib.h"
#include "macros.h"
#include "sound.h"
#include "effects.h"
#include "nps_smoke.h"
#include "bbox.h"
#include "fltcallbacks.h"

#ifndef NOAHA
#include "aha.h"
#endif // NOAHA
#include "munitions.h"
#include "ship_constants.h"
#include "interface_const.h"
#include "collision_const.h"

#include "pvs.h"
#include "object.h"
#include "picking.h"
#include "matrix.h"

#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.29577951f
#define HALFPI M_PI/2.0f
#define METER_TO_INCH 39.370079 
#define INCH_TO_METER 0.0254f
#define MILE_TO_KILOMETER 1.6093471f
#define METER_TO_FOOT 3.2808f
#define MPS_TO_KNOTS 1.94f
#define MAX_OBJECTS 250
#define REAL_SMALL 0.0001f
#define SMALL 0.5
#define UPDATETIME 5.0f
#define IS_ZERO(f)   ((f > -REAL_SMALL) && (f < REAL_SMALL))

#define ventDecrementValue   0.005f
#define halonDecrementValue  0.02f
#define waterDecrementValue  0.005f
#define hoseDistance         6.0f
#define hoseAngle            15.0f

// NPS Ship Sound Constants
#define START_YELL   6
#define OPEN_DOOR    24
#define START_GQ     25
#define START_STEAM  26
#define FIRE_OUT     27
#define START_HALON  28
#define OPEN_NOZZLE  29
#define START_VENT   30

// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile STATIC_DATA     *G_static_data;
extern          VEH_LIST        *G_vehlist;
extern          VEHTYPE_LIST    *G_vehtype;
extern          pfGroup         *G_moving;
extern          pfGroup         *G_fixed;
extern volatile int              G_drivenveh;
extern volatile MANAGER_REC     *G_manager;

#ifndef NOHIRESNET
extern          DIS_net_manager *G_hr_net;
extern volatile float            G_curtime;
#endif // NOHIRESNET

const  pfVec3    NozzlePosit = {-101.0f,13.34f,4.75f}; //-101.25
static pfDCS    *nozzle_Loc;
static pfGroup  *nozzle_I_beam;
Pick   *myPick;
LocationRec dummy_snd_pos;

void detachNozzle();

//********************************************************************//
//****************** class SHIP_WALK_VEH *****************************//
//********************************************************************//

SHIP_WALK_VEH::SHIP_WALK_VEH(int id, int atype, ForceID the_force)
:SHIP_VEH(id,atype,the_force)
{
    create_entity(atype, the_force);

    dummy_snd_pos.x = 0.0;
    dummy_snd_pos.y = 0.0;
    dummy_snd_pos.z = 0.0;

#ifndef NOHIRESNET
   infodataStruct *data;
   bzero (hiResObjects, sizeof (fixedDatumRecord) * NUM_SHIP_HIRES_VALUES);
   //Init Doors
   hiResObjects[DR0].datum_id = DR0_TAG;
   hiResObjects[DR1].datum_id = DR1_TAG;
   hiResObjects[DR2].datum_id = DR2_TAG;
   hiResObjects[DR3].datum_id = DR3_TAG;
   hiResObjects[DR4].datum_id = DR4_TAG;
   hiResObjects[DR5].datum_id = DR5_TAG;
   hiResObjects[DR6].datum_id = DR6_TAG;
   hiResObjects[DR7].datum_id = DR7_TAG;
   //Init Valves
   hiResObjects[VL0].datum_id = VL0_TAG;
   data = (infodataStruct *)pfGetNodeTravData(ShipValves[STEAM_VALVE],PFTRAV_ISECT);
   hiResObjects[VL0].datum = data->perCent;
   hiResObjects[VL1].datum_id = VL1_TAG;
   data = (infodataStruct *)pfGetNodeTravData(ShipValves[OIL_VALVE],PFTRAV_ISECT);
   hiResObjects[VL1].datum = data->perCent;
   //Init Nozzle
   hiResObjects[NZ0].datum_id = NZ0_TAG;
   hiResObjects[NZ0].datum = -1.0f;
   hiResObjects[NZ1].datum_id = NZ1_TAG;
   hiResObjects[NZ2].datum_id = NZ2_TAG;
   hiResObjects[NZ3].datum_id = NZ3_TAG;
   //Init Casualties
   hiResObjects[STL].datum_id = STL_TAG;
   hiResObjects[FOL].datum_id = FOL_TAG;
   hiResObjects[FIR].datum_id = FIR_TAG;
   //Init Buttons
   hiResObjects[HAL].datum_id = HAL_TAG;
   hiResObjects[VEN].datum_id = VEN_TAG;
   okToSendHiRes = FALSE;
   okToUpdateHiRes = FALSE;
   numHiResObjects = NUM_SHIP_HIRES_VALUES;
   hiResTimeStamp = G_curtime;
#endif // NOHIRESNET

}

SHIP_WALK_VEH::~SHIP_WALK_VEH()
{
   infodataStruct  *nozzledata = (infodataStruct *)
                    pfGetNodeTravData(ShipSwitches[NOZZLE], PFTRAV_ISECT);
   if (nozzledata->index >= 0) {
      pfSwitchVal(ShipSwitches[NOZZLE], 0);
      detachNozzle();
      nozzledata->index = -1;
      nozzledata->perCent = 0.0f;
   }
   removeWaterCell();
   delete steam_cas;
   delete oil_cas;
   delete fire_cas;
}

void SHIP_WALK_VEH::changestatus(unsigned int stat)
{
  VEHICLE::changestatus(stat);
  if (status & Appearance_DamageDestroyed) {
#ifndef NOHIRESNET
     okToSendHiRes = FALSE;
     okToUpdateHiRes = FALSE;
     set_net_type (disNet);
#endif // NOHIRESNET
  }

} // end JACK_DI_VEH::changestatus()


void SHIP_WALK_VEH::create_entity(int atype, ForceID the_force)
//fill in the fields that the constuctor does not
{
   int *vidprt;
   //int i,j;
   //pfNode *cur_node, *parent_node;
   //int found, failed;
   //const char *hold_name;
   //char unique_str[10];
   //char cur_name[30];
   //char root_parent[30];
   //char rename_root[40];
   //char temp_name[500];
   //char search_path[500];
   char hull_name[30];
   //char path_stack[50][30];
   //int stack_top = 0;
   //static int unique = 0;
   static int count = 0;
   long entity_mask, force_mask;
   Control = OTHER;
   force = the_force;


//cerr << "adding ship_walk_veh vid "<<vid <<" create_entity\n";

#ifndef NOAHA
  // AHA, by Jiang Zhu
   if ( vid != G_drivenveh )
      ahaRemoveSelectableObject( (pfNode *)hull );
#endif

   pfRemoveChild(G_moving,hull);
   pfDelete(hull); models = NULL;

   //Setup the Cell Locator information for this ship
   Locator = initPVS();
   //This myPick only hits terrain does not hit the ship
   myPick = NewPick((pfNode *)G_dynamic_data->pipe[0].channel[0].scene, 
                    (pfChannel **)&(G_dynamic_data->pipe[0].channel[0].chan_ptr));

   //make the hull DCS
   hull = pfNewDCS();
   models = pfNewSwitch();//(pfSwitch *)pfClone(G_vehtype[atype].mswitch,0);
   pfAddChild(models,Locator);
   //pfAddChild(models,G_vehtype[G_vehtype[atype].dead].model);
   if(models != NULL) {
     G_vehtype[atype].numclones++;
     sprintf ( hull_name, "%d_%s_#%d",
               ++count,
               G_vehtype[atype].name,
               G_vehtype[atype].numclones );
     if ( 0 == ( pfNodeName(hull,hull_name) ) )
        cerr << "Performer says name for hull not unique:  "
             << hull_name << endl;
     pfSwitchVal(models,PFSWITCH_ON);
     //pfSwitchVal(models,ALIVE);
     pfAddChild(hull,models);

     //Get pointer to Vehicle Ramp Switch
     vehRamp = ShipSwitches[VEHRAMP];
     if (vehRamp != NULL)
         pfSwitchVal(vehRamp, 0);  //Set switch to first child (Ramp Down)
     else
         cerr<<"vehRamp Switch not Found"<<endl;

     //Initialize all casualties
     pfVec3 pos = {-9.92f, -105.51f, -3.85f};
     steam_cas = new CASUALTY_STEAM((pfNode *)Locator, pos);
     pfSetVec3(pos,-7.75f, -113.54f, -4.27f);
     oil_cas = new CASUALTY_OIL_LEAK((pfNode *)Locator, pos);
     pfSetVec3(pos,-7.75f, -113.0f, -4.27f); //-113.54, -4.27
     fire_cas = new CASUALTY_FIRE(pos);
     

     switch ( int(G_vehtype[atype].distype.entity_kind) )
        {
        case EntityKind_Other:
        case EntityKind_Platform:
        case EntityKind_Munition:
        case EntityKind_LifeForm:
        case EntityKind_Environmental:
        case EntityKind_Cultural:
           entity_mask =
              (long(G_vehtype[atype].distype.entity_kind) << ENTITY_KIND_SHIFT);
           break;
        default:
           entity_mask =
              (long(EntityKind_Other) << ENTITY_KIND_SHIFT);
           cerr << "Unknown DIS type, entity kind ->"
                << int(G_vehtype[atype].distype.entity_kind)
                << "<- setting INTERSECT mask.\n";
           break;
        }
  
  /* ADD DOMAIN STUFF HERE */
  
     switch ( int(force) )
        {
        case ForceID_Blue:
        case ForceID_Red:
        case ForceID_White:
        case ForceID_Other:
           force_mask = (long(force)<<ENTITY_FORCE_SHIFT);
           break;
        default:
           force_mask = (long(ForceID_Other)<<ENTITY_FORCE_SHIFT);
           cerr << "Unknown DIS force, entity force ->"
                << int(force)
                << "<- setting INTERSECT mask.\n";
           break;
        }
  
     entity_mask = entity_mask | force_mask; //| (PERMANENT_OTHER<<PERMANENT_SHIFT);
  
     pfNodeTravMask ( hull, PFTRAV_ISECT, entity_mask,
                      PFTRAV_SELF|PFTRAV_DESCEND|PFTRAV_IS_UNCACHE, PF_SET);

     bounding_box = G_vehtype[atype].bounding_box;
     pfAddVec3 ( bounding_box_center, bounding_box.min, bounding_box.max );
     pfScaleVec3 ( bounding_box_center, 0.5f, bounding_box_center );
     bbox_geom = make_bounding_box ( &bounding_box );
     if ( bbox_geom != NULL )
        {
        pfAddChild(hull,bbox_geom);
        pfNodeTravMask ( bbox_geom, PFTRAV_ISECT, 0x0,
                         PFTRAV_SELF|PFTRAV_DESCEND|PFTRAV_IS_UNCACHE, PF_SET);
        pfSwitchVal(bbox_geom,PFSWITCH_OFF);
        }
/*
     num_joints = G_vehtype[atype].num_joints;
     for ( i = 0; i < G_vehtype[atype].num_joints; i++ )
        {
        joint[i].numlods = G_vehtype[atype].joints[i].numlods;
        for ( j = 0; j < G_vehtype[atype].joints[i].numlods; j++ )
           {
           stack_top = 0;
           cur_node = (pfNode *)G_vehtype[atype].joints[i].dcs[j];
#ifdef ARTIC_DEBUG
           cerr << "Loop i = " << i << ", j = " << j << endl;
           cerr << "Starting backward search from:  "
                <<  pfGetNodeName(G_vehtype[atype].joints[i].dcs[j]) << endl;
           cerr << "Trying to reach:  " 
                << pfGetNodeName(G_vehtype[atype].mswitch) << endl;
           cerr << "Cloned goal is:  "
                << pfGetNodeName(hull) << endl;
#endif
           strcpy ( root_parent, pfGetNodeName(G_vehtype[atype].mswitch) );
           strcpy ( search_path, "" );
           found = FALSE;
           failed = FALSE;
           do
              {
              hold_name = pfGetNodeName(cur_node);
              if ( hold_name != NULL )
                 {
                 strcpy ( cur_name, hold_name );
                 strcpy ( path_stack[stack_top], cur_name );
                 stack_top++;
                 if ( strcmp ( cur_name, root_parent ) )
                    { 
                    strcpy ( temp_name, "/" );
                    strcat ( temp_name, cur_name );
                    strcat ( temp_name, search_path );
                    strcpy ( search_path, temp_name );
                    }
                 else
                    {
                    found = TRUE;
                    strcpy ( temp_name, "/G_scene/G_veh/" );
                    strcat ( temp_name, hull_name );
                    strcat ( temp_name, "/" );
                    strcpy ( rename_root, "__NPS_tEmP" );
                    unique++;
                    sprintf ( unique_str, "%d", unique );
                    strcat ( rename_root, unique_str );
                    strcat ( temp_name, rename_root );
                    strcat ( temp_name, search_path );
                    strcpy ( search_path, temp_name );
#ifdef ARTIC_DEBUG
                    cerr << "Full articulated path name:  " 
                         << search_path << endl;
#endif
                    }
                 }
              if (!found )
                 {
                 if ( pfGetNumParents(cur_node) == 0 )
                    {
                    cerr << "ERROR locating root parent for articulated part.\n";
                    cerr << "    - Model: " 
                         << pfGetNodeName(G_vehtype[atype].mswitch)
                         << endl;
                    cerr << "    - Articulated part:  "
                         << G_vehtype[atype].joints[i].name[j] << endl;
                    failed = TRUE;
                    }
                 else
                    {
                    parent_node = (pfNode *)pfGetParent(cur_node,0);
                    if ( parent_node == NULL )
                       {
                       cerr << "ERROR locating root for articulated part.\n";
                       cerr << "    - Model: " 
                            << pfGetNodeName(G_vehtype[atype].mswitch)
                            << endl;
                       cerr << "    - Articulated part:  "
                            << G_vehtype[atype].joints[i].name[j] << endl;
                       failed = TRUE;
                       }
                    else
                       cur_node = parent_node;
                    }
                 }
              }
           while ( !found && !failed );
           
           if ( found )
              {
//           if ( 0 == (pfNodeName ( models, rename_root )) )
//              cerr << "Performer says " << rename_root 
//                   << " is not unique!\n";
//           pfPrint ( models, PFTRAV_SELF|PFTRAV_DESCEND, PFPRINT_VB_NOTICE, NULL );
//           pfDebugPrint ( models, PFTRAV_SELF | PFTRAV_DESCEND, NULL );
              if ( NULL == 
                   (joint[i].dcs[j] = 
                      (pfDCS *)NPSFindNode(
                                  (pfGroup *)models,path_stack,stack_top-1)) )
//                (joint[i].dcs[j] = (pfDCS *)pfFindGroup(search_path)) )
                 {
                 cerr << "ERROR locating articulation part.\n";
                 cerr << "    - Pathname given is:  " << search_path << endl;
                 }
              else
                 {
                 strcpy ( joint[i].name[j], 
                          G_vehtype[atype].joints[i].name[j] );
#ifdef ARTIC_DEBUG
                 cerr << "Name of node found:  " 
                      << pfGetNodeName (joint[i].dcs[j]) << endl;
#endif
                 }
   //           pfNodeName ( models, root_parent );
              }//if(found)
           }// do
      }//for til num_joints
*/

   }
   else{
     cerr << "Unable to Clone a icon for vid "<< vid << " Type " 
          << atype << "\n";
     pfDelete(hull);
     hull = NULL;
     models = NULL;
   }

   //attach it to the scene graph
   pfSync();
   pfAddChild(G_moving,hull);

#ifndef NOAHA
   // AHA, by Jiang Zhu
   // 1 is the ahaClass of the object. For the moment, simply take 1.

   // Commented out 1/26/96: Crashes the system when used due to a problem
   // calculating a BSphere in aha code thru this classes structure
   if ( vid != G_drivenveh )
      ahaAddSelectableObject((pfNode *) hull, vid, 1);
#endif

   //store the index so it can be used as a call back
   pfNodeTravFuncs(hull,PFTRAV_ISECT,vid_store,NULL);
   vidprt = new int;
   *vidprt = vid;
   pfNodeTravData(hull,PFTRAV_ISECT,vidprt);

   //fill in the type parameters
   bcopy ( (char *)&G_vehtype[atype].distype, (char *)&type,
           sizeof(EntityType) );
   icontype = atype;
} //create_entity()


// move the vehicle for the next frame & update PVS data
movestat SHIP_WALK_VEH::move()
{
    movestat	tmp = SHIP_VEH::move();
    return tmp;
}

movestat SHIP_WALK_VEH::moveDR(float deltatime,float curtime)
{
    movestat	tmp = SHIP_VEH::moveDR(deltatime,curtime);
    
#ifndef NOHIRESNET
    //HeartBeat for Ship PDU's on Hi Res Net
    if(G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) {
       okToSendHiRes = TRUE;
    }
    if (G_hr_net != NULL) { //(vid == G_drivenveh) && 
      if (okToSendHiRes) {
         DataPDU hrDPDU;
         hrDPDU.orig_entity_id = G_vehlist[G_drivenveh].DISid;
#ifndef DIS_2v4
         hrDPDU.orig_force_id = (ForceID)G_dynamic_data->force;
#endif
         hrDPDU.recv_entity_id = G_vehlist[vid].DISid;
#ifndef DIS_2v4
         bzero ( (void *) &hrDPDU.recv_force_id, sizeof (ForceID) );
#endif
         hrDPDU.request_id = HIRES_DATA_PDU_SHIP_TAG; // Need to set for id purposes
         hrDPDU.num_datum_fixed = numHiResObjects;
         bcopy ( (void *) hiResObjects, (void *) (hrDPDU.fixed_datum),
                sizeof (fixedDatumRecord) * numHiResObjects);
         switch ( G_dynamic_data->hr_network_mode ) {
            case NET_OFF:
            case NET_RECEIVE_ONLY:
               break;
            case NET_SEND_ONLY:
            case NET_SEND_AND_RECEIVE:
            default:
               G_hr_net->write_pdu ( (char *)&hrDPDU, DataPDU_Type);
               break;
         }
         okToSendHiRes = FALSE;
      }
    }

    if (okToUpdateHiRes) {
       apply_hi_res ();
       okToUpdateHiRes = FALSE;
    }
#endif // NOHIRESNET

    LocatorData *locdata = (LocatorData *)pfGetNodeTravData(Locator, PFTRAV_CULL);
    infodataStruct *data, *halondata, *ventdata, *nozzledata;
    float           current_speed = pfLengthVec3(velocity);
    float           total_extinguish = 0.0f;
    static float    oilleak_start_time;
    pfVec3          deltaVec; //fireLoc,
    float          *nozzleXYZ, *nozzleHPR;
    float           fireHeading;
    int             water_extinguish = 0;
    

    // Set the vehRamp based on ship's velocity
    if ((current_speed != 0)) {
       pfSwitchVal(vehRamp,1);
    }
    else {
       pfSwitchVal(vehRamp,0);
    }

    // Update all casualties on ship
    //steam_leak
    if (steam_cas->isStarted()) {
       data = (infodataStruct *)
              pfGetNodeTravData(ShipValves[STEAM_VALVE], PFTRAV_ISECT);
       if (data->perCent > 0.0f)
          steam_cas->move(data->perCent);
       else {
          steam_cas->stop();
#ifndef NOHIRESNET
          hiResObjects[STL].datum = steam_cas->isStarted();
          okToSendHiRes = TRUE;
#endif // NOHIRESNET

       }
    }

    //fuel oil leak
    if (oil_cas->isStarted()) {
       data = (infodataStruct *)
              pfGetNodeTravData(ShipValves[OIL_VALVE], PFTRAV_ISECT);
       if (data->perCent > 0.0f)
          oil_cas->move(data->perCent);
       else {
          oil_cas->stop();
#ifndef NOHIRESNET
          hiResObjects[FOL].datum = oil_cas->isStarted();
          okToSendHiRes = TRUE;
#endif // NOHIRESNET

       }
    }
    else {
       if (!fire_cas->isStarted()) {
          //set the start time for the fuel oil leak
          oilleak_start_time = curtime;
       }
    }

    //Fuel oil Fire
    if (curtime - oilleak_start_time > 20.0f) {
       halondata = (infodataStruct *)
              pfGetNodeTravData(ShipButtons[HALON_BUTTON], PFTRAV_ISECT);
       ventdata = (infodataStruct *)
              pfGetNodeTravData(ShipButtons[VENT_BUTTON], PFTRAV_ISECT);
       nozzledata = (infodataStruct *)
              pfGetNodeTravData(ShipSwitches[NOZZLE], PFTRAV_ISECT);
       if (!fire_cas->isStarted()) {
          //Start the Fire
          fire_cas->start(posture);
          if ( G_static_data->sound ) {
              playsound(&dummy_snd_pos, &dummy_snd_pos, START_GQ,-1);
          }

          //Hack to prevent vent & halon from remaining on by accident
          halondata->perCent = 0.0f;
          ventdata->perCent = 0.0f;
       }
       else { // Update Fire's size and shape
          fire_cas->update_position(posture);
          if (nozzledata->perCent) { //Check to see if water is hitting fire
             nozzleXYZ= G_vehlist[nozzledata->index].vehptr->getposition();
             nozzleHPR= G_vehlist[nozzledata->index].vehptr->getorientation();
             deltaVec[X] = nozzleXYZ[X] - fire_cas->getposition()[X];
             deltaVec[Y] = nozzleXYZ[Y] - fire_cas->getposition()[Y];
             fireHeading = pfArcTan2(deltaVec[Y], deltaVec[X]) + 90.0f; 
             if (fireHeading < 0.0f) 
                fireHeading += 360.0f;
             else if (fireHeading > 360.0f)
                     fireHeading -= 360.0f;
             if (( (PF_ABS(deltaVec[X]) < hoseDistance) || 
                   (PF_ABS(deltaVec[Y]) < hoseDistance)) &&
                 (PF_ABS(nozzleHPR[HEADING] - fireHeading) < hoseAngle) )   
                 water_extinguish = 1;
          }
          total_extinguish = (halondata->perCent * halonDecrementValue) + 
                             (water_extinguish * waterDecrementValue);
          fire_cas->update_size(total_extinguish);
          if (!fire_cas->isStarted()) { //Is fire out, if so put it out
             oilleak_start_time = curtime;
             halondata->perCent = 0.0f;
#ifndef NOHIRESNET
             hiResObjects[HAL].datum = halondata->perCent;
             okToSendHiRes = TRUE;
#endif // NOHIRESNET
             if ( G_static_data->sound ) {
                 playsound(&dummy_snd_pos, &dummy_snd_pos, FIRE_OUT,-1);
             }

          } // fire isStarted
       } // else Update Fire's Size & Position
    }//  curtime - oilleak_start_time

    //Smoke / Ventilation
    if (fire_cas->isSmoke()) {
       // Are we inside the engine room
       int inside_ship = (locdata->lastcell == 8) ||
                         (locdata->lastcell == 9) ||
                         (locdata->lastcell == 10);
       ventdata = (infodataStruct *)
              pfGetNodeTravData(ShipButtons[VENT_BUTTON], PFTRAV_ISECT);
       total_extinguish = (ventdata->perCent * ventDecrementValue);
       fire_cas->update_smoke_size(total_extinguish, inside_ship);
       if (!fire_cas->isSmoke()) { //Is Smoke out, if so turen off ventilation
          ventdata->perCent = 0.0f;
#ifndef NOHIRESNET
          hiResObjects[VEN].datum = ventdata->perCent;
          okToSendHiRes = TRUE;
#endif // NOHIRESNET
       } // isSmoke out
    } // isSmoke on
    
    return tmp;
} // moveDR()


void SHIP_WALK_VEH::class_specific_keyboard()
{
   int counter = 0;
   input_manager *l_input_manager = G_manager->ofinput;

   // Start SteamLeak
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_S);
   if ( counter > 0 ) {
      steam_cas->start();
      if ( G_static_data->sound ) {
          playsound(&dummy_snd_pos, &dummy_snd_pos, START_STEAM,-1);
      }
#ifndef NOHIRESNET
      hiResObjects[STL].datum = 1.0f;
      okToSendHiRes = TRUE;
#endif // NOHIRESNET
   }

   // Start FuelOilLeak
   l_input_manager->query_button(counter,NPS_KEYBOARD,CTRL_F);
   if ( counter > 0 ) {
      oil_cas->start();
      if ( G_static_data->sound ) {
          playsound(&dummy_snd_pos, &dummy_snd_pos, START_YELL,-1);
      }
#ifndef NOHIRESNET
      hiResObjects[FOL].datum = 1.0f;
      okToSendHiRes = TRUE;
#endif // NOHIRESNET
   }

}// class_specific_keyboard()

#ifndef NONPSSHIP
void SHIP_WALK_VEH::mount(VEHICLE *vehptr, int vid) 
#else
void SHIP_WALK_VEH::mount(VEHICLE *, int)
#endif
{ 
#ifndef NONPSSHIP
   mounted_veh[vid] = vehptr;
#endif
}


void SHIP_WALK_VEH::dismount(int vid) 
{ 
   infodataStruct  *nozzledata = (infodataStruct *)
                    pfGetNodeTravData(ShipSwitches[NOZZLE], PFTRAV_ISECT);
   if (nozzledata->index == vid) {
      pfSwitchVal(ShipSwitches[NOZZLE], 0);
      detachNozzle();
      nozzledata->index = -1;
      nozzledata->perCent = 0.0f;
#ifndef NOHIRESNET
      hiResObjects[NZ0].datum = -1.0f;
      hiResObjects[NZ1].datum = nozzledata->perCent;
      okToSendHiRes = TRUE;
#endif // NOHIRESNET
   }
#ifndef NONPSSHIP
   mounted_veh[vid] = NULL;
#endif
}


void SHIP_WALK_VEH::update_locator()
{
    updatePVS(Locator,posture);

}//update_locator


/************************************************************************
 *									*		
 * 			moveValve					*
 *									*
 ************************************************************************/
void moveValve(pfNode *node, infodataStruct *data, 
               int direction, float updatedPerCent)
{
    float delta = 0.0f;
    float valveIncrement = 0.01f;
    int numTurns = 2;
    const float degPerCircle = 360.0f;

    //Used by dcvet's network code not used here
    //whose = whoseSteamLeak();

    switch (direction)
    {
       case 0: //Close Valve
	  data->perCent = data->perCent - valveIncrement;

	  // If valve is already shut, we don't want to move the DCS's
	  if (data->perCent< 0.0f) {
	      data->perCent = 0.0f;
	      return;
	  }

	  // Compute delta so stem and handwheel move in unision
	  delta = -(degPerCircle * (float) numTurns * valveIncrement);

          //if (whose <= localID)
          //   data->perCent = newPerCent; 

       break;

       case 1: //Open valve
	  data->perCent = data->perCent + valveIncrement;

	  // If valve is already open, we don't want to move the DCS's
	  if (data->perCent > 1.0f) {
	      data->perCent = 1.0f;
	      return;
	  }

	  // Compute delta so stem and handwheel move in unision
	  delta = (degPerCircle * (float) numTurns * valveIncrement);

          //if (whose <= localID)
          //   data->perCent = newPerCent; 

       break;

       case 2:
	  //if (Shared->whoseSteam != localID)
	  //{
	  if (updatedPerCent > data->perCent)
	     delta = (degPerCircle * (float) numTurns * valveIncrement);// 0.0f;
	  else delta = -(degPerCircle * (float) numTurns * valveIncrement);// 0.0f;
		
	  if(updatedPerCent > 1.0f) data->perCent = 1.0f;
	  else 
	     if(updatedPerCent < 0.0f) data->perCent = 0.0f;
	     else data->perCent = updatedPerCent;
	  //}
       break;

    }// Switch Valve Direction

    pfNode          *stem, *handwheel;
    infodataStruct  *stemdata;
    infodataStruct  *handwheeldata;
    stem = pfGetChild(node,0);
    stemdata = (infodataStruct *)pfGetNodeTravData(stem,PFTRAV_ISECT);
    handwheel = pfGetChild(node,1);
    handwheeldata = (infodataStruct *)pfGetNodeTravData(handwheel,PFTRAV_ISECT);

    // Handle the stem first
    updateObject(stem, stemdata->dof, data->perCent, delta, 0); 

    // Handle the handwheel next.
    updateObject(handwheel, handwheeldata->dof, 0.0f, delta, 1); 
}


/************************************************************************
 *									*		
 * 			swingDoor					*
 *									*
 ************************************************************************/

void swingDoor(pfNode *node, infodataStruct *data, int direction)
{
    float	delta = 0.0f;

    switch (direction)
    {
       case 0: //Closing Door
	  delta = -3.0f;
          if ( G_static_data->sound ) {
             playsound(&dummy_snd_pos, &dummy_snd_pos,OPEN_DOOR,-1);
          }
       break;

       case 1: //Opening Door
	  delta = 3.0f;
       break;

       case 2: //Updating Door from network
	  delta = 0.0f;
       break;
   }

   // Move the door
    updateObject(node, data->dof, 0.0f, delta, 0);
}

/************************************************************************
 *									*		
 * 			attachNozzle					*
 *									*
 ************************************************************************/

//Attach nozzle to driven vehicle
void attachNozzle(int vid)
{
//    const char      *name;
    pfGroup         *parent;

    parent = pfGetParent((pfNode *)pfGetParent(ShipSwitches[NOZZLE],0),0);
//cerr<<"\t\t\tAttaching Nozzle: ";
    nozzle_Loc = pfNewDCS();
    nozzle_I_beam = pfGetParent(parent, 0);
//name = pfGetNodeName((pfNode *)nozzle_I_beam);
//if (name) cerr<<name<<endl;
//else cerr<<"nozzle_I_beam"<<endl;
    pfRemoveChild(nozzle_I_beam, parent);
    pfAddChild(nozzle_Loc, parent);
//pfDCSTrans(nozzle_Loc,-101.0f,13.34f,4.75f); 
    pfDCSTrans(nozzle_Loc,NozzlePosit[X],
                          NozzlePosit[Y],
                          NozzlePosit[Z]);
    pfDCSRot(nozzle_Loc, 90.0f, 0.0f, 0.0f);
    pfAddChild((pfNode *)G_vehlist[vid].vehptr->gethull(), nozzle_Loc);
} // attachNozzle


/************************************************************************
 *									*		
 * 			detachNozzle					*
 *									*
 ************************************************************************/

//Detach nozzle from driven vehicle
void detachNozzle()
{
    pfGroup         *parent;

    parent = pfGetParent((pfNode *)pfGetParent(ShipSwitches[NOZZLE],0),0);
//cerr<<"\t\t\tDetaching Nozzle"<<endl;
    pfRemoveChild((pfNode *)G_vehlist[G_drivenveh].vehptr->gethull(),nozzle_Loc);
    pfRemoveChild(nozzle_Loc, parent);
    pfDelete(nozzle_Loc);
    pfAddChild(nozzle_I_beam, parent);
    nozzle_I_beam = NULL;
    nozzle_Loc = NULL;
} // detatchNozzle


/************************************************************************
 *									*		
 * 			ObjectRelatedEvent				*
 *									*
 ************************************************************************/

void SHIP_WALK_VEH::ObjectRelatedEvent(pfNode  *node, int direction)
{
    pfGroup         *parent;
    infodataStruct  *parentdata;
    infodataStruct  *data = (infodataStruct *)pfGetNodeTravData(node,PFTRAV_ISECT);

    //Make sure node contains data, if not just exit
    if (!data) return;

    switch (data->type)
    {
      case DCS_MASK:  // DCS Mask  
          parent = pfGetParent(node,0);
          parentdata = (infodataStruct *)pfGetNodeTravData(parent,PFTRAV_ISECT);
	  
	  switch (parentdata->type)
	  {
	     case DOOR_MASK:
		 swingDoor(node, data, direction);
#ifndef NOHIRESNET
                 hiResObjects[parentdata->index].datum = data->dof->curtwist;
                 okToSendHiRes = TRUE;
#endif // NOHIRESNET

                 //handSignal(0, OPEN_DOOR);
                 //if(Shared->soundReady) {
                 //    if (Shared->soundLocal) playSound(HATCHOPEN,PRI);
                 //    Shared->soundReady = FALSE;
                 //}
	     break;

	     case VALVE_MASK:
		moveValve((pfNode *)parent, parentdata, direction, 0.0f);
#ifndef NOHIRESNET
                hiResObjects[8+parentdata->index].datum = parentdata->perCent;
                okToSendHiRes = TRUE;
#endif // NOHIRESNET

                //if(which == 0) // steam valve
                //   handSignal(0, TURN_VALVE_HIGH);
                //else // otherwise it's the fuel oil valve
                //   handSignal(0, TURN_VALVE);
	     break;
               
	     default:
	     break;
	  }
      break;
      case MANIP_MASK:   // manipulate mask  
          int manip_obj;  //Which object to manipulate
          if (node == (pfNode *)ShipSwitches[NOZZLE]) {
             manip_obj = 2;
//cerr<<"Picked Nozzle Handle"<<endl;
          }
          else manip_obj = data->index;
          switch (manip_obj)
          {
              case 0:  //ventilation fans  
                 //handSignal(0, OPEN_DOOR);    
                 if (data->perCent < 0.1f) {
		    sprintf(data->name, "ENGINE ROOM FANS ARE RUNNING");
		    sprintf(data->function, "VENTILATING ENGINE ROOM");
                    data->perCent = 1.0f;
#ifndef NOHIRESNET
                    hiResObjects[VEN].datum = data->perCent;
                    okToSendHiRes = TRUE;
#endif // NOHIRESNET
                    if ( G_static_data->sound ) {
                      playsound(&dummy_snd_pos, &dummy_snd_pos, START_VENT,-1);
                    }

		 }
		 else {
		    sprintf(data->name, "Ventilation cannot be currently activated ");
		    sprintf(data->function, "Don't touch what you don't understand");
		 }
                 infodisplay(data->name,data->function,data->soundbite);
              break;

              case 1:  // Halon activator 
                 //handSignal(0, OPEN_DOOR);
                 if (data->perCent < 0.1f) {
		    sprintf(data->name, "HALON HAS BEEN ACTIVATED");
		    sprintf(data->function, "EVACUATE SPACE IMMEDIATELY");
                    data->perCent = 1.0f;
#ifndef NOHIRESNET
                    hiResObjects[HAL].datum = data->perCent;
                    okToSendHiRes = TRUE;
#endif // NOHIRESNET
                    if ( G_static_data->sound ) {
                      playsound(&dummy_snd_pos, &dummy_snd_pos, START_HALON,-1);
                    }
 
		 }
		 else {
		    sprintf(data->name, "HALON cannot be currently activated ");
		    sprintf(data->function, "Don't touch what you don't understand");
		 }
                 infodisplay(data->name,data->function,data->soundbite);
              break;

              case 2:  // Manipulate nozzle
                 parentdata = (infodataStruct *)
                              pfGetNodeTravData(ShipSwitches[NOZZLE],PFTRAV_ISECT);
                 if (direction == 0) { //Left button pushed
                    if (parentdata->index < 0) { 
                       parentdata->index = 0;
                       attachNozzle(G_drivenveh);
#ifndef NOHIRESNET
                       hiResObjects[NZ0].datum = 1.0f; //parentdata->perCent;
                       hiResObjects[NZ1].datum = parentdata->perCent;
                       okToSendHiRes = TRUE;
#endif // NOHIRESNET
                    }
                    else if (parentdata->index == 0) { 
                            // Toggle the nozzle open/closed
                            if (parentdata->perCent < 0.5f) { //Open the nozzle
//cerr<<"\t\t\tOpening Nozzle"<<endl;
                               parentdata->perCent = 1.0f;
                               pfSwitchVal(ShipSwitches[NOZZLE], 1);
                               if ( G_static_data->sound ) { //Is nozzle opening
                                  playsound(&dummy_snd_pos, 
                                            &dummy_snd_pos,OPEN_NOZZLE,-1); 
                               }
                            }
                            else { //Close the nozzle
//cerr<<"\t\t\tClosinging Nozzle"<<endl;
                               parentdata->perCent = 0.0f;
                               pfSwitchVal(ShipSwitches[NOZZLE], 0);
                            }
#ifndef NOHIRESNET
                            hiResObjects[NZ1].datum = parentdata->perCent;
                            okToSendHiRes = TRUE;
#endif // NOHIRESNET

                    }
                 }
                 else if (direction == 1){ //Right button pushed
                    if (parentdata->index >= 0) { 
                       parentdata->index = -1;
                       parentdata->perCent = 0.0f;
                       pfSwitchVal(ShipSwitches[NOZZLE], 0);
                       detachNozzle();
#ifndef NOHIRESNET
                       hiResObjects[NZ0].datum = -1.0f;
                       hiResObjects[NZ1].datum = parentdata->perCent;
                       okToSendHiRes = TRUE;
#endif // NOHIRESNET
                    }
                 }
              break;

              default:
              break;
          }
          break;

       case INFO_MASK:  // information stuff  
       case BULK_MASK:  // information stuff  
       case HARD_MASK:  // information stuff  
       case DECK_MASK:  // information stuff  
       case LADDER_MASK:  // information stuff  
       case GROUND_MASK:  // information stuff  
       case VALVE_MASK:  // information stuff  
       case DOOR_MASK:  // information stuff  
          infodisplay(data->name,data->function,data->soundbite);
          /*if(Shared->soundReady)
          {
             if (Shared->soundLocal) playSoundFile(Shared->soundBite);
             Shared->soundReady = FALSE;
             //cout << "INFO sound file is " << Shared->soundBite << endl;
          }*/

       break;

       case MULT_MASK:  // information stuff 
          parent = (pfGroup *)multiMatrix[data->index]; 
          parentdata = (infodataStruct *)pfGetNodeTravData(parent,PFTRAV_ISECT);
          infodisplay(parentdata->name,parentdata->function,parentdata->soundbite);
	  /*if(Shared->soundReady)
          {
             if (Shared->soundLocal) playSoundFile(Shared->soundBite);
             Shared->soundReady = FALSE;
             //cout << "MULTI sound file is " << Shared->soundBite << endl;
          }*/

       break;

       default:
	  cerr<<"nothing in database matches this mask"<<endl;
       break;
    }
}


void SHIP_WALK_VEH::update_ship_picking()
{
#define MAX_DCS_NODES 8
    int     direction, ix, jx=0;
    static int last_frm_btn = FALSE; 
    static int btn_pushed = FALSE;
    static int update_infochan = TRUE;
    static pfNode  *tmp=NULL, *newnode; //, *oldnode;
    LocatorData *data = (LocatorData *)pfGetNodeTravData(Locator, PFTRAV_CULL);
    pfNode *model_nodes[MAX_DCS_NODES];
    infodataStruct *nozzledata = (infodataStruct *)
                 pfGetNodeTravData(ShipSwitches[NOZZLE],PFTRAV_ISECT);

    last_frm_btn = btn_pushed;

    // Check for Picking Stuff
    if (get_pick_button(NPS_MOUSE_LEFT_BUTTON, input_number) ||
        get_pick_button(NPS_THRTL_BUTTON_7, input_number)) {
       direction = 0;
       btn_pushed = TRUE;
    }
    else if (get_pick_button(NPS_MOUSE_RIGHT_BUTTON, input_number) ||
             get_pick_button(NPS_STCK_BOT_BUTTON, input_number)) {
            direction = 1;
            btn_pushed = TRUE;
         }
    else if (btn_pushed) {
                 /* Commented out 1/15/95: Can not change DrawFunc after pfFrame
                 // Reset information channel to normal radar picking
                 pfChanTravMode(G_dynamic_data->pipe[0].channel[2].chan_ptr,
                                PFTRAV_DRAW, PFDRAW_OFF);
                 pfChanDrawFunc(G_dynamic_data->pipe[0].channel[2].chan_ptr,
                                draw_information);
                 */
            btn_pushed = FALSE;
            update_infochan = TRUE;
            return;
         }
    else if (get_pick_button(NPS_THRTL_BUTTON_3, input_number) &&
             nozzledata->index == 0) {
            ObjectRelatedEvent((pfNode *)ShipSwitches[NOZZLE], 0);
            return;
         }
    else return;

    //only do a pick or open info window if button was just pushed
    if(!last_frm_btn && btn_pushed) {
 
       // Setup the information channel for ship pick information
       if (update_infochan) {
          /*
          pfChanTravMode(G_dynamic_data->pipe[0].channel[2].chan_ptr,
                         PFTRAV_DRAW, PFDRAW_ON);
          pfChanDrawFunc(G_dynamic_data->pipe[0].channel[2].chan_ptr,
                         ship_draw_info);
          */
          update_infochan = FALSE;
       }

       // Setup scene database so ship objects can be picked
       pfRemoveChild(hull, models);
       for (ix=0; ix<MAXPVSCELLS-1; ix++){
          if (pfGetSwitchVal(data->cells[ix])==PFSWITCH_ON && ix!=19) {
             newnode = pfGetChild(data->cells[ix],0);
             pfAddChild(hull, newnode);
          }
       }

       //This is a hack to allow picking trough switch nodes for the nozzle
       //When the picking through switch node bug is fixed this can be removed
       infodataStruct *nozzledata = (infodataStruct *)
              pfGetNodeTravData(ShipSwitches[NOZZLE], PFTRAV_ISECT);
       pfDCS *drv_veh=G_vehlist[G_drivenveh].vehptr->gethull();
       pfMatrix hull_mat;
       pfGetDCSMat(drv_veh, hull_mat);
       pfDCS *temp_hull = pfNewDCS();
       pfDCSMat(temp_hull, hull_mat);
       pfAddChild(G_moving, temp_hull);
       if (nozzledata->index == 0) {
          long num = pfGetNumChildren((pfGroup *)drv_veh);
          //find non-DCS nodes of driven vehicle
          for (ix=0; ix<num; ix++){
              pfNode *node=pfGetChild((pfGroup *)drv_veh,ix);
              if (pfGetDCSClassType() == pfGetType((pfObject *)node)) {
                 model_nodes[jx++]=node;
                 pfRemoveChild((pfNode *)drv_veh,node);
                 pfAddChild((pfNode *)temp_hull,node);
              }   
          }
       }

       tmp = DoPick(myPick);

       // Restore driven_vehicle scene database
       if (nozzledata->index == 0) {
          for (ix=0; ix<jx; ix++) {
             pfRemoveChild((pfNode *)temp_hull, model_nodes[ix]);
             pfAddChild((pfNode *)drv_veh,model_nodes[ix]);
          }
          pfRemoveChild(G_moving, temp_hull);
          pfDelete(temp_hull);
       }

       // Restore scene database to pre-pick state
       for (ix=0; ix<MAXPVSCELLS-1; ix++){
          if (pfGetSwitchVal(data->cells[ix])==PFSWITCH_ON && ix!=19) {
             newnode = pfGetChild(data->cells[ix],0);
             pfRemoveChild(hull, newnode);
          }
       }
       pfAddChild(hull, models);
    } //if button was just pushed

    if (tmp) {
        ObjectRelatedEvent(tmp, direction);
    }//if object found by pick

}// update_ship_picking


void SHIP_WALK_VEH::apply_hi_res ()
{
   infodataStruct *data;

   // Update Doors
   for (int ix=0; ix<MAX_DOORS; ix++) {
      if (ShipDoors[ix]) {
         data = (infodataStruct *)
             pfGetNodeTravData(pfGetChild(ShipDoors[ix],0), PFTRAV_ISECT);
         if (data) {
            if (data->dof->curtwist != hiResObjects[ix].datum) {
               data->dof->curtwist = hiResObjects[ix].datum;
               swingDoor((pfNode *)pfGetChild(ShipDoors[ix],0), data, 2);
            }// data->dof->curtwist
         }// data
      }// ShipDoors
   }// for loop

   // Update Steam Valve
   data = (infodataStruct *)
          pfGetNodeTravData(ShipValves[STEAM_VALVE], PFTRAV_ISECT);
   if (data) {
      if (data->perCent != hiResObjects[VL0].datum) {
         moveValve((pfNode *)ShipValves[STEAM_VALVE], data, 2, 
                    hiResObjects[VL0].datum );
      }
   }

   // Update Fuel Oil Valve
   data = (infodataStruct *)
          pfGetNodeTravData(ShipValves[OIL_VALVE], PFTRAV_ISECT);
   if (data) {
      if (data->perCent != hiResObjects[VL1].datum) {
         moveValve((pfNode *)ShipValves[STEAM_VALVE], data, 2, 
                   hiResObjects[VL1].datum );
      }
   }

   // Update Nozzle
   data = (infodataStruct *)
          pfGetNodeTravData(ShipSwitches[NOZZLE], PFTRAV_ISECT);
   if (data) {
      if (data->index != hiResObjects[NZ0].datum) {
         if (data->index < 0) {
            data->index = (int)hiResObjects[NZ0].datum;
            attachNozzle(data->index);
         }
         else if (hiResObjects[NZ0].datum < 0) {
                 data->index = (int)hiResObjects[NZ0].datum;
                 detachNozzle();
              }
      }
      if (data->perCent != hiResObjects[NZ1].datum) {
         data->perCent = hiResObjects[NZ1].datum;
         pfSwitchVal(ShipSwitches[NOZZLE], (int)data->perCent);
         if ( G_static_data->sound && data->perCent ) { //Is nozzle opening
            playsound(&dummy_snd_pos, &dummy_snd_pos,OPEN_NOZZLE,-1); 
         }
      }
   }

   // Update Steam Leak
   if (!steam_cas->isStarted() && (int)hiResObjects[STL].datum) {
      //Start Steam Casualty
      steam_cas->start();
      if ( G_static_data->sound ) {
          playsound(&dummy_snd_pos, &dummy_snd_pos, START_STEAM,-1);
      }
   }

   // Update Fuel Oil Leak
   if (!oil_cas->isStarted() && (int)hiResObjects[FOL].datum) {
      //Start Steam Casualty
      oil_cas->start();
      if ( G_static_data->sound ) {
          playsound(&dummy_snd_pos, &dummy_snd_pos, START_YELL,-1);
      }
   }

/*
   // Update Fire
*/

   // Update Halon System
   data = (infodataStruct *)
          pfGetNodeTravData(ShipButtons[HALON_BUTTON], PFTRAV_ISECT);
   if (data) {
      if (data->perCent != hiResObjects[HAL].datum) {
         data->perCent = hiResObjects[HAL].datum;
         if ( G_static_data->sound && data->perCent ) {
             playsound(&dummy_snd_pos, &dummy_snd_pos, START_HALON,-1);
         }
      }
   }

   // Update Ventilation System
   data = (infodataStruct *)
          pfGetNodeTravData(ShipButtons[VENT_BUTTON], PFTRAV_ISECT);
   if (data) {
      if (data->perCent != hiResObjects[VEN].datum) {
         data->perCent = hiResObjects[VEN].datum;
         if ( G_static_data->sound && data->perCent ) {
             playsound(&dummy_snd_pos, &dummy_snd_pos, START_VENT,-1);
         }
      }
   }

}// apply_hi_res()



void SHIP_WALK_VEH::set_hi_res (fixedDatumRecord *objArray, int noz_vid)
{
   for (int ix = 0; ix < numHiResObjects; ix++) {
      if (ix == NZ0 && objArray[ix].datum >= 0 && hiResObjects[ix].datum < 0) {
         // set vid of nozzle holder
         hiResObjects[ix].datum = noz_vid;
      }
      else hiResObjects[ix].datum = objArray[ix].datum;
   } // For Loop
} //set_hi_res()

void SHIP_WALK_VEH::startSteamLeak()
{
   //steam_cas->start();
}// startSteamLeak()


void SHIP_WALK_VEH::startOilLeak()
{
   //oil_cas->start();
}// startOilLeak()



