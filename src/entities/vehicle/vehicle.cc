// File: <vehicle.cc>
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
#include <string.h>
#include <bstring.h>
#include <iostream.h>
#ifdef IRISGL
#include <fmclient.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "vehicle.h"

#include "disnetlib.h"
#include "manager.h"
#include "common_defs2.h"
#include "macros.h"
#include "appearance.h"
#include "terrain.h"
#include "entity.h"
#include "identify.h"
#include "sound.h"
#include "effects.h"
#include "nps_smoke.h"
#include "munitions.h"
#include "netutil.h"
#include "bbox.h"
#include "draw.h"
#include "read_config.h"
#include "radar.h"
#include "fontdef.h"

#ifndef NOAHA
#include "aha.h"
#endif
#include "conversion_const.h"
#include "collision_const.h"
#include "interface_const.h"
#include "local_snd_code.h"
#include "nps_converter.h"
#include "articulat.h"

#include "automateClass.h"

#ifndef NO_DVW
#define NO_PFSMOKE
#define NO_NPSSMOKE
#endif

#ifndef NO_PFSMOKE
#define NO_NPSSMOKE
#endif

// Globals used by this library
extern volatile MANAGER_REC     *G_manager;
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern volatile STATIC_DATA     *G_static_data;
extern volatile HUD_options	*G_HUD_options;
extern          VEH_LIST        *G_vehlist;
extern volatile int              G_drivenveh;
extern          pfGroup         *G_moving;
extern          pfGroup         *G_fixed;
extern          VEHTYPE_LIST    *G_vehtype;
extern volatile float            G_curtime;
extern          DIS_net_manager *G_net;
#ifdef JACK
extern          int              G_runisms;
#endif


/*************************** For development *******************/
//#define DEBUG_SCALE
//#define TRACE
//#define DEBUG
//#define DEBUG_DIS

static pfVec3 ZeroVec = {0.0f,0.0f,0.0f};
static pfCoord ZeroCoord = {{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f}};

pfGroup *NPSFindNode ( pfGroup *cur_node, char path[50][30], int cur_level );

#ifndef NO_PFSMOKE

extern int G_pfsmoke_smokes_count;
extern int G_pfsmoke_flames_count;
extern int G_pfsmoke_trails_count;

static pfVec3 UpDir = {0.0f,0.0f,0.05f}, UpMore = {0.0f,0.0f,11.0f};
static pfVec3 White = {1.0f,1.0f,1.0f}, 
              Grey = {0.6f,0.7f,0.8f}, Brown = {0.82,0.75,0.49},
              TrailDir = {0.0f,0.0f,0.001f};
       pfVec3 WakeDir = {0.0f, 0.1f, 0.0f};

npsAutomate *VEHICLE::vehAuto = (npsAutomate *) NULL;

#define SMOKE 0
#define FLAME 1
#define TRAIL 2

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

#define PFUTRAIL_RADIUS 1.0f
#define PFUTRAIL_SPEED 1.0f
#define PFUTRAIL_TURBULENCE 0.0f
#define PFUTRAIL_DENSITY 2.0f
#define PFUTRAIL_DISSIPATION 0.85f
#define PFUTRAIL_EXPANSION 1.0f
#define PFUTRAIL_DIRECTION TrailDir
#define PFUTRAIL_BEGIN_COLOR Brown
#define PFUTRAIL_END_COLOR Brown
#define PFUTRAIL_DURATION -1.0f // Infinite
#endif


VEHICLE::VEHICLE (int id, int atype, ForceID the_force)
{
   int i;
#ifdef TRACE
  cerr << "Basic Vehicle constructor\n";
#endif

  cdata.vid = -1;
  cdata.reset = 0;
  cdata.rigid_body = 0;
  cdata.roll = 0.0f;
  cdata.pitch = 0.0f;
  cdata.thrust = 0.0f;
  cdata.rudder = 0.0f;
  cdata.rudder_present = 0;
  cdata.sidethrust = 0.0f;
  cdata.elevthrust = 0.0f;
  cdata.heading = 0.0f;
  cdata.current_time = 0.0f;
  cdata.last_time = 0.0f;
  cdata.delta_time = 0.0f;

  tether_model_off = -1;
  tethered = FALSE;
  targetting = TRUE;
  paused = FALSE;

  for ( i = 0; i < MAX_TRANSPORT; i++ ) {
     transport_data.location[i].xyz[X] =
        (G_dynamic_data->bounding_box.xmin +
         G_dynamic_data->bounding_box.xmax) / 2.0f;
     transport_data.location[i].xyz[Y] =
        (G_dynamic_data->bounding_box.ymin +
         G_dynamic_data->bounding_box.ymax) / 2.0f;
     transport_data.location[i].xyz[Z] = 0.0f;
     pfSetVec3 ( transport_data.location[i].hpr, 0.0f, 0.0f, 0.0f );
     strcpy ( transport_data.comment[i], " // Center of computed bbox" );

  }
  strcpy ( transport_data.filename, (char *)G_static_data->transport_file );
  transport_data.current_count = 0;
  transport_data.changed = FALSE;
  read_transport ( transport_data ); 

  vid = id;
  posture = ZeroCoord;
  look = ZeroCoord;
  altitude = 0.0f;
  speed = 0.0f;
  thrust = 0.0f;
  pfCopyVec3(eyepoint,ZeroVec);
  pfCopyVec3(firepoint,ZeroVec);
  pfCopyVec3(velocity, ZeroVec);
  pfCopyVec3(acceleration, ZeroVec);
  hull = NULL;
  lastPDU = -1;
  drpos = ZeroCoord;
  drparam = ZeroCoord;
  icontype = -1;
  status = 0;
  num_joints = 0;
  for ( i = 0; i < MAX_FIRE_INFO; i++ )
     firetimes[i] = 0.0f;
  head_switch = NULL;
  grenade_switch = NULL;
  num_heads = 0;
  num_grenades = 0;
  weapons[0] = MINACTIVE;
  weapons[1] = MINACTIVE;
  weapons[2] = MINACTIVE;
  force = D_FORCE;
  displaying_model = TRUE;
  birds_eye = FALSE;
  snap_switch = TRUE;
  quiet_switch = TRUE;
  video_switch = TRUE;
  net_type = disNet;

  bzero ( (char *)&controller, sizeof(sender_info) );
  pfMakeEmptyBox ( &bounding_box );
  bbox_geom = NULL;

#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
  G_dynamic_data->hud_callback = vehicle_hud_draw;
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

  input_control = NPS_NO_INPUT;
  input_number = 0;

#ifndef NO_PFSMOKE
   smoke_index=flame_index=smoke_status=status_lock=0;
   pfCopyVec3(old_posture.xyz,ZeroVec); pfCopyVec3(old_posture.hpr,ZeroVec);
   pfCopyVec3(older_posture.xyz,ZeroVec); pfCopyVec3(older_posture.hpr,ZeroVec);
#endif

  create_entity ( atype, the_force );

  input_manager *l_input_manager = G_manager->ofinput;
  if (l_input_manager->verify_device (NPS_LWS) && (G_drivenveh == vid)) {
     short info[3];
     info[0] = (short)G_vehlist[vid].DISid.address.site; 
     info[1] = (short)G_vehlist[vid].DISid.address.host;
     info[2] = (short)G_vehlist[vid].DISid.entity;
     l_input_manager->set_multival ((void *) info, NPS_LWS, NPS_LW_ENTITY);
  }

}

#ifndef NODUDE

VEHICLE::VEHICLE (int id)
{
   int i;
#ifdef TRACE
  cerr << "Basic Vehicle constructor\n";
#endif

  vid = id;
  posture = ZeroCoord;
  pfCopyVec3(eyepoint,ZeroVec);
  pfCopyVec3(firepoint,ZeroVec);
  pfCopyVec3(velocity, ZeroVec);
  pfCopyVec3(acceleration, ZeroVec);
  hull = NULL;
  lastPDU = -1;
  drpos = ZeroCoord;
  drparam = ZeroCoord;
  icontype = -1;
  status = 0;
  num_joints = 0;
  for ( i = 0; i < MAX_FIRE_INFO; i++ )
     firetimes[i] = 0.0f;
  for ( i = 0; i < MAXJOINTS; i++ )
     joint[i].numlods = 0;
#ifdef IPORT_INPUT
  for ( i = 0; i < MAXLODS; i++ )
     {
     head_switch[i] = NULL;
     grenade_switch[i] = NULL;
     }
  num_heads = 0;
  num_grenades = 0;
#endif
  weapons[0] = MINACTIVE;
  weapons[1] = MINACTIVE;
  weapons[2] = MINACTIVE;
  force = D_FORCE;
  displaying_model = TRUE;
  birds_eye = FALSE;
  snap_switch = TRUE;
  bzero ( (char *)&controller, sizeof(sender_info) );
  pfMakeEmptyBox ( &bounding_box );
  bbox_geom = NULL;

#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
  G_dynamic_data->hud_callback = vehicle_hud_draw;
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
}
#endif // NODUDE

VEHICLE::~VEHICLE()
{
int i;
char *nodename;
#ifndef NOAHA
  // AHA, by Jiang Zhu
  if ( vid != G_drivenveh )
     ahaRemoveSelectableObject( (pfNode *)hull );
#endif

#ifdef TRACE
  cerr << "Veh class destructor\n";
#endif
  if ( hull != NULL )
     {
     pfRemoveChild(G_moving,hull);
     nodename = (char *)pfGetNodeName(hull);
     if ( (pfDelete(hull)) != TRUE )
        cerr << "Object NOT deleted..." << nodename << endl;
     models = NULL;
     hull = NULL;
     }
  for (i = 0; i < num_joints; i++)
     {
     joint[i].dcs = NULL;
     strcpy ( joint[i].name, "" );
     }
  num_joints = 0;

  if ( transport_data.changed ) {
     if ( !(write_transport ( transport_data )) ) {
        cerr << "WARNING:\tUnable to save changes to the transport file:"
             << endl;
        cerr << "\t" << transport_data.filename << endl;
        cerr << "\tTrying to save file as /tmp/NPSNET_transports.dat ";
        strcpy ( transport_data.filename, "/tmp/NPSNET_transports.dat");
        if ( (write_transport (transport_data )) ) {
           cerr << "== SUCCESSFUL." << endl;
        }
        else {
           cerr << "== FAILED." << endl;
           cerr << "\tDUMPING TRANSPORT INFORMATION TO SCREEN:" << endl;
           cerr << "===== Start cut here. =====" << endl;
           strcpy ( transport_data.filename, "cerr");
           write_transport (transport_data );
           cerr << "===== Stop cut here. =====" << endl;
        }
     }
  }

#ifndef NO_PFSMOKE
  if(smoke_status&Appearance_SmokePlume)
        pfuSmokeMode(G_dynamic_data->smokes[smoke_index],PFUSMOKE_STOP);
  if(smoke_status&Appearance_Flaming)
        pfuSmokeMode(G_dynamic_data->flames[flame_index],PFUSMOKE_STOP);
  if(smoke_status&Appearance_TrailingEffects_Mask)
        pfuSmokeMode(G_dynamic_data->trails[trail_index],PFUSMOKE_STOP);
#endif

}


void VEHICLE::del_vehicle()
//delete the object
{
#ifdef TRACE
  cerr << "public destructor called\n";
#endif

  this->VEHICLE::~VEHICLE();
}


void VEHICLE::snap_on()
{
//cerr << "Snap is on for " << vid << endl;
snap_switch = TRUE;
}


void VEHICLE::snap_off()
{
//cerr << "Snap is off for " << vid << endl;
snap_switch = FALSE;
}

void VEHICLE::set_look_hpr (pfVec3 theHPR)
{
pfCopyVec3 (look.hpr, theHPR);
}

void VEHICLE::set_look_xyz (pfVec3 theXYZ)
{
pfCopyVec3 (look.xyz, theXYZ);
}


int VEHICLE::oktofire ( wdestype designation, float curtime )
   {
   int typedex;
   float oktime;

   typedex = G_vehlist[vid].type;

   if ( designation < G_vehtype[typedex].num_weapons )
      {
      oktime = firetimes[designation] + 
               G_vehtype[typedex].fireinfo[designation].firedelay;
      return ( oktime < curtime );
      }
   else
      return FALSE;
   }


int VEHICLE::setfiretime ( wdestype designation, float curtime )
   {
   int typedex;

   typedex = G_vehlist[vid].type;

   if ( designation < G_vehtype[typedex].num_weapons )
      {
      firetimes[designation] = curtime;
      return TRUE;
      }
   else
      return FALSE;
   }


void VEHICLE::init_posture(pfCoord &pos)
{
  pfCopyVec3 ( posture.xyz, pos.xyz );
  pfCopyVec3 ( posture.hpr, pos.hpr );
  pfCopyVec3 ( eyepoint, pos.xyz );
  pfCopyVec3 ( firepoint, pos.xyz );
  if ( hull != NULL )
     {
     pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
     pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);     }

#ifndef NO_PFSMOKE
  pfCopyVec3(old_posture.xyz,pos.xyz); pfCopyVec3(old_posture.hpr,pos.hpr);
  pfCopyVec3(older_posture.xyz,pos.xyz); pfCopyVec3(older_posture.hpr,pos.hpr);
#endif

}


int &VEHICLE::index(int)
  { static int i = 0; return i; }

void VEHICLE::turn_on_grenades ()
{
   pfSwitchVal(grenade_switch, PFSWITCH_ON);
}

void VEHICLE::turn_off_grenades ()
{
   pfSwitchVal(grenade_switch, PFSWITCH_OFF);
}

void VEHICLE::turn_on_heads ()
{
//cerr << "turn ON heads called\n";
   pfSwitchVal(head_switch, PFSWITCH_ON);
}

void VEHICLE::turn_off_heads ()
{
   pfSwitchVal(head_switch, PFSWITCH_OFF);
}


vtype VEHICLE::gettype() 
{ 
  return (VEH_TYPE); 
}


EntityType VEHICLE::get_DIS_entity_type()
{
  return type;
}


ctltype VEHICLE::getControl()
{
  return (CTL_TYPE);
}


ForceID VEHICLE::getforce()
{
  return force;
}


void VEHICLE::get_eyepoint ( pfVec3 &eye )
{
   pfCopyVec3 ( eye, eyepoint );
}


void VEHICLE::get_firepoint ( pfVec3 &fire )
{
   pfCopyVec3 ( fire, firepoint );
}

void VEHICLE::get_weapon_view (pfCoord &point)
{
   pfCopyVec3 (point.xyz, firepoint);
   pfCopyVec3 (point.hpr, posture.hpr);
   pfAddVec3 (point.xyz, point.xyz, look.xyz);
   pfAddVec3 (point.hpr, point.hpr, look.hpr);
}

void VEHICLE::get_weapon_look (pfCoord &point)
{
   pfCopyVec3 (point.xyz, look.xyz);
   pfCopyVec3 (point.hpr, look.hpr);
}


float *VEHICLE::getposition()
// Return a pointer the vehicle's position vector
{ 
  return posture.xyz;
}


float *VEHICLE::getorientation()
// Return a pointer the vehicle's orientation vector
{
  return posture.hpr;
}


pfDCS *VEHICLE::gethull()
//Return a pointer to the vehicle's hull
{
  return hull;
}


pfVec3 *VEHICLE::getvelocity()
// Return a pointer to the vehicle's velocity vector
{
  return &velocity;
}


pfGroup *NPSFindNode ( pfGroup *cur_node, char path[50][30], int cur_level )
   {
   int i;
   int next_level;
   const char *hold_name;
   pfGroup *result;
   int num_children;
   int found = FALSE;

   if ( cur_level < 0 )
      {
      return NULL;
      }
   else
      {
      hold_name = pfGetNodeName(cur_node);
      if ( (cur_level == 0) && (hold_name != NULL) &&
           (!strcmp(path[cur_level],hold_name)) )
         {
         found = TRUE;
         return cur_node;
         }
      else if (pfIsOfType(cur_node, pfGetGroupClassType()) &&
               ((NULL == hold_name) || 
               (!strcmp(path[cur_level],hold_name))) )
         { 
         i = 0;
         if ( NULL == hold_name )
            next_level = cur_level;
         else
            next_level = cur_level - 1;
         num_children = int (pfGetNumChildren ( cur_node ));
         while ( ( i < num_children ) && (!found) )
            {
            if ( NULL != 
               (result = NPSFindNode((pfGroup *)pfGetChild(cur_node,i),
                                     path,next_level)) )
               found = TRUE;
            else
               i++;
            }
         if ( found )
            return result;
         else
            return NULL;
         }
      else
         {
         return NULL;
         }
      }
   }
  
 
void VEHICLE::create_entity(int atype, ForceID the_force)
//fill in the fields that the constuctor does not
{
   int *vidprt;
   int i;
   pfNode *cur_node, *parent_node;
   int found, failed;
   const char *hold_name;
   char unique_str[10];
   char cur_name[30];
   char root_parent[30];
   char rename_root[40];
   char temp_name[500];
   char search_path[500];
   char hull_name[30];
   char path_stack[50][30];
   int stack_top = 0;
   static int unique = 0;
   static int count = 0;
   long entity_mask, force_mask;
   Control = OTHER;


#ifdef TRACE
   cerr << "adding vid "<<vid <<" create_entity\n";
#endif
   
   force = the_force;

   //make the hull DCS
   hull = pfNewDCS();
   models =(pfSwitch *)pfClone(G_vehtype[atype].mswitch,0);
   if(models != NULL) {
     G_vehtype[atype].numclones++;
     sprintf ( hull_name, "%d_%s_#%d",
               ++count,
               G_vehtype[atype].name,
               G_vehtype[atype].numclones );
     if ( 0 == ( pfNodeName(hull,hull_name) ) )
        cerr << "Performer says name for hull not unique:  "
             << hull_name << endl;
     pfSwitchVal(models,ALIVE);
     pfAddChild(hull,models);

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
  
     entity_mask = entity_mask | force_mask;
  
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

     num_joints = G_vehtype[atype].num_joints;
#ifdef ARTIC_DEBUG
     cerr << "ARTIC: Vehicle type index = " << atype << endl;
     cerr << "\tNumber of articulations = " << num_joints << endl;
#endif
     for ( i = 0; i < G_vehtype[atype].num_joints; i++ )
        {
        stack_top = 0;
        cur_node = (pfNode *)G_vehtype[atype].joints[i].dcs;
#ifdef ARTIC_DEBUG
        cerr << "Loop i = " << i << endl;
        cerr << "Starting backward search from:  "
             <<  pfGetNodeName(G_vehtype[atype].joints[i].dcs << endl;
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
                      << G_vehtype[atype].joints[i].name << endl;
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
                         << G_vehtype[atype].joints[i].name << endl;
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
//        if ( 0 == (pfNodeName ( models, rename_root )) )
//           cerr << "Performer says " << rename_root 
//                << " is not unique!\n";
//        pfPrint ( models, PFTRAV_SELF|PFTRAV_DESCEND, PFPRINT_VB_NOTICE, NULL );
//        pfDebugPrint ( models, PFTRAV_SELF | PFTRAV_DESCEND, NULL );
           if ( NULL == 
                (joint[i].dcs = 
                   (pfDCS *)NPSFindNode(
                               (pfGroup *)models,path_stack,stack_top-1)) )
//             (joint[i].dcs = (pfDCS *)(pfGroup*)pfLookupNodeXXX(search_path, pfGetGroupClassType() /* XXX PF2.0 pfLookup() includes subclasses, e.g. pfLOD */)) )
              {
              cerr << "ERROR locating articulation part.\n";
              cerr << "    - Pathname given is:  " << search_path << endl;
              }
           else
              {
              strcpy ( joint[i].name, 
                       G_vehtype[atype].joints[i].name );
              joint[i].dis_id = G_vehtype[atype].joints[i].dis_id;
              joint[i].min_range = G_vehtype[atype].joints[i].min_range;
              joint[i].max_range = G_vehtype[atype].joints[i].max_range;
              joint[i].current_setting.xyz[PF_X] = 0.0f;
              joint[i].current_setting.xyz[PF_Y] = 0.0f;
              joint[i].current_setting.xyz[PF_Z] = 0.0f;
              joint[i].current_setting.hpr[PF_H] = 0.0f;
              joint[i].current_setting.hpr[PF_P] = 0.0f;
              joint[i].current_setting.hpr[PF_R] = 0.0f;
              bcopy ( (char *)&(G_vehtype[atype].joints[i].articulate),
                      (char *)&(joint[i].articulate),
                      sizeof(unsigned char)*6);
#ifdef ARTIC_DEBUG
              cerr << "Name of node found:  " 
                   << pfGetNodeName (joint[i].dcs) << endl;
#endif
              }
   //        pfNodeName ( models, root_parent );

           }
      }

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
}

 
void VEHICLE::init_velocity(pfVec3 vel)
{
  pfCopyVec3(velocity, vel);
}


void VEHICLE::assign_dralgo(DeadReckAlgorithm dra)
{
  dralgo = dra;
}


void VEHICLE::assign_weapons(mtype *mt)
{
  weapons[0] = mt[0];
  weapons[1] = mt[1];
  weapons[2] = mt[2];
}


void VEHICLE::assign_entity_type(int e_type)
{
  type = G_vehtype[e_type].distype;
  G_vehlist[vid].type = icontype = e_type;
  G_vehlist[vid].cat =  G_vehtype[e_type].cat;
  
}


#ifndef NO_PFSMOKE
void VEHICLE::find_top(pfVec3 top) {
   pfVec3 temp_vec; pfMatrix top_matrix;
   float veh_rad; pfCoord temp_coord;

   if(G_static_data->reality_graphics) {
     pfMakeCoordMat(top_matrix,&older_posture);
         pfCopyVec3(temp_coord.hpr,older_posture.hpr);
   }
   else {
         pfMakeCoordMat(top_matrix,&posture);
         pfCopyVec3(temp_coord.hpr,posture.hpr);
   }
   pfFullXformPt3(temp_coord.xyz,bounding_box_center,top_matrix);

   veh_rad=(pfDistancePt3(bounding_box.max,bounding_box.min)/2)*0.20;
   pfSetVec3(top, 0, 0, 1);
   pfScaleVec3(temp_vec,((bounding_box.max[Z]-bounding_box.min[Z])/2)+veh_rad,top);
   pfMakeCoordMat(top_matrix,&temp_coord);
   pfFullXformPt3(top,temp_vec,top_matrix);
}
void VEHICLE::find_dust_origin(pfVec3 dust_origin) {

#if 0
   pfVec3 temp_vec;
   pfMatrix a_matrix;
   pfCoord temp_coord;

   if(G_static_data->reality_graphics) {
         pfMakeCoordMat(a_matrix,&older_posture);
         pfCopyVec3(temp_coord.hpr,older_posture.hpr);
   }
   else {
         pfMakeCoordMat(a_matrix,&posture);
         pfCopyVec3(temp_coord.hpr,posture.hpr);
   }
   pfFullXformPt3(temp_coord.xyz,bounding_box_center,a_matrix);

   pfSetVec3(dust_origin, 0, -1.65, -1);
   pfScaleVec3(temp_vec,(bounding_box.max[Z]-bounding_box.min[Z]),dust_origin);
   pfMakeCoordMat(a_matrix,&temp_coord);
   pfFullXformPt3(dust_origin,temp_vec,a_matrix);
#endif
   pfVec3 back = { 0.0f, -1.0f, 0.0f };
   pfVec3 down = { 0.0f, 0.0f, -1.0f };
   float trail_rad,veh_rad;
   pfMatrix orient;

   pfMakeEulerMat ( orient, posture.hpr[HEADING],
                            posture.hpr[PITCH],
                            posture.hpr[ROLL]);

   pfFullXformPt3 ( down, down, orient );
   pfNormalizeVec3 ( down );
   pfFullXformPt3 ( back, back, orient );
   pfNormalizeVec3 ( back );

#ifndef NONPSSHIP
   pfCopyVec3(WakeDir,back);
   //pfScaleVec3(WakeDir,pfLengthVec3(velocity)*5.0f,WakeDir);
#endif //NONPSSHIP

   veh_rad=(pfDistancePt3(bounding_box.max,bounding_box.min)/2.0f);

   switch(status&Appearance_TrailingEffects_Mask) {
      case Appearance_TrailingEffectsLarge:
         trail_rad=(veh_rad*PFUTRAIL_RADIUS*0.60f);
         break;
      case Appearance_TrailingEffectsMedium:
         trail_rad=(veh_rad*PFUTRAIL_RADIUS*0.40f);
         break;
      case Appearance_TrailingEffectsSmall:
         trail_rad=(veh_rad*PFUTRAIL_RADIUS*0.20f);
         break;
   }

   pfScaleVec3(back,-bounding_box.min[Y]+trail_rad,back);
   pfScaleVec3(down,-bounding_box.min[Z],down);
   pfAddVec3(dust_origin,posture.xyz,back);
   pfAddVec3(dust_origin,dust_origin,down);

}
void VEHICLE::movePuffs(pfuSmoke *smoke, pfVec3 origin) {
        long i, n, startPuff; pfVec3 temp_diff;

        n=smoke->numPuffs;
        startPuff=smoke->startPuff;
        for(i=n-1;i>=0;i--) {
          pfSubVec3(temp_diff,smoke->puffs[(i+startPuff)%MAX_PUFFS]->origin,
                                smoke->origin);
          pfAddVec3(smoke->puffs[(i+startPuff)%MAX_PUFFS]->origin,origin,temp_diff);
        }
        pfCopyVec3(smoke->origin,origin);
}
void VEHICLE::update_pfsmokes(void) {
   pfVec3 the_loc;
   double flame_time;

   flame_time=G_dynamic_data->flames[flame_index]->startTime;

   find_top(the_loc);
   if(smoke_status&Appearance_Flaming) {
     movePuffs(G_dynamic_data->flames[flame_index],the_loc);
   }
   if(smoke_status&Appearance_SmokePlume) {
     pfCopyVec3(G_dynamic_data->smokes[smoke_index]->origin,the_loc);
   }
   find_dust_origin(the_loc);
   if(smoke_status&Appearance_TrailingEffects_Mask) {
     if(pfLengthVec3(velocity)>0) {
#ifndef NONPSSHIP
        if (gettype() == SHIP) {
           static int smoke_count=0;
           the_loc[Z] = G_dynamic_data->trails[trail_index]->origin[Z];
           pfCopyVec3(G_dynamic_data->trails[trail_index]->origin,the_loc);
           if (smoke_count++ == 50) {
              pfuSmokeDuration(G_dynamic_data->trails[trail_index],25.0f);
              smoke_status&=(~Appearance_TrailingEffects_Mask);
              smoke_count = 0;
           }
        }
        else
#endif
           pfCopyVec3(G_dynamic_data->trails[trail_index]->origin,the_loc);
     }
     else {
        pfuSmokeDuration(G_dynamic_data->trails[trail_index],4.0f);
        smoke_status&=(~Appearance_TrailingEffects_Mask);
     }
   }
   else if((status&Appearance_TrailingEffects_Mask)&&(pfLengthVec3(velocity))>0)     changestatus(status);
   pfCopyVec3(older_posture.xyz,old_posture.xyz);
   pfCopyVec3(older_posture.hpr,old_posture.hpr);
   pfCopyVec3(old_posture.xyz,posture.xyz);
   pfCopyVec3(old_posture.hpr,posture.hpr);
}
void VEHICLE::setSmoke(int the_index, int type) {
   float veh_rad, trail_rad;
   pfVec3 the_top, dust_origin;

   veh_rad=(pfDistancePt3(bounding_box.max,bounding_box.min)/2);
   find_top(the_top);

   if(type==SMOKE) {
       G_dynamic_data->smokes[the_index]->startPuff=
           G_dynamic_data->smokes[the_index]->numPuffs=0;
       pfuSmokeType(G_dynamic_data->smokes[the_index],PFUSMOKE_SMOKE);
           pfuSmokeTex(G_dynamic_data->smokes[the_index],G_dynamic_data->smokeTex);
       pfuSmokeOrigin(G_dynamic_data->smokes[the_index],the_top,PFUSMOKE_RADIUS*veh_rad);
       pfuSmokeVelocity(G_dynamic_data->smokes[the_index],PFUSMOKE_TURBULENCE,PFUSMOKE_SPEED);
       pfuSmokeDir(G_dynamic_data->smokes[the_index],PFUSMOKE_DIRECTION);
       pfuSmokeDensity(G_dynamic_data->smokes[the_index],PFUSMOKE_DENSITY,PFUSMOKE_DISSIPATION,PFUSMOKE_EXPANSION);
       pfuSmokeColor(G_dynamic_data->smokes[the_index],PFUSMOKE_BEGIN_COLOR,PFUSMOKE_END_COLOR);
       pfuSmokeDuration(G_dynamic_data->smokes[the_index],PFUSMOKE_DURATION);
       pfuSmokeMode(G_dynamic_data->smokes[the_index],PFUSMOKE_START);
   }
   else if(type==FLAME) {
           G_dynamic_data->flames[the_index]->startPuff=
           G_dynamic_data->flames[the_index]->numPuffs=0;
       pfuSmokeType(G_dynamic_data->flames[the_index],PFUSMOKE_FIRE);
           pfuSmokeTex(G_dynamic_data->flames[the_index],G_dynamic_data->fireTex);
           pfuSmokeOrigin(G_dynamic_data->flames[the_index],the_top,PFUFLAME_RADIUS*veh_rad);
       pfuSmokeVelocity(G_dynamic_data->flames[the_index],PFUFLAME_TURBULENCE,PFUFLAME_SPEED);
           pfuSmokeDir(G_dynamic_data->flames[the_index],PFUFLAME_DIRECTION);
           pfuSmokeDensity(G_dynamic_data->flames[the_index],PFUFLAME_DENSITY,PFUFLAME_DISSIPATION,PFUFLAME_EXPANSION);
           pfuSmokeColor(G_dynamic_data->flames[the_index],PFUFLAME_BEGIN_COLOR,PFUFLAME_END_COLOR);
           pfuSmokeDuration(G_dynamic_data->flames[the_index],PFUFLAME_DURATION);
           pfuSmokeMode(G_dynamic_data->flames[the_index],PFUSMOKE_START);
   }
   else if(type==TRAIL) {
       G_dynamic_data->trails[the_index]->startPuff=
           G_dynamic_data->trails[the_index]->numPuffs=0;
       switch(status&Appearance_TrailingEffects_Mask) {
                        case Appearance_TrailingEffectsLarge:
                                trail_rad=(veh_rad*PFUTRAIL_RADIUS*0.60f); break;
                        case Appearance_TrailingEffectsMedium:
                                trail_rad=(veh_rad*PFUTRAIL_RADIUS*0.40f); break;
                        case Appearance_TrailingEffectsSmall:
                                trail_rad=(veh_rad*PFUTRAIL_RADIUS*0.20f); break;
           }

       find_dust_origin(dust_origin);

       pfuSmokeType(G_dynamic_data->trails[the_index],PFUSMOKE_DUST);
       pfuSmokeTex(G_dynamic_data->trails[the_index],G_dynamic_data->missileTex);
       pfuSmokeVelocity(G_dynamic_data->trails[the_index],PFUTRAIL_TURBULENCE,PFUTRAIL_SPEED);
       pfuSmokeDensity(G_dynamic_data->trails[the_index],PFUTRAIL_DENSITY,PFUTRAIL_DISSIPATION,PFUTRAIL_EXPANSION);
       pfuSmokeDuration(G_dynamic_data->trails[the_index],PFUTRAIL_DURATION);

#ifndef NONPSSHIP
       if (gettype() == SHIP) {
           dust_origin[Z] = -trail_rad + 0.5f;
           pfuSmokeOrigin(G_dynamic_data->trails[the_index],dust_origin,trail_rad);
           pfuSmokeDir(G_dynamic_data->trails[the_index],WakeDir);
           pfuSmokeColor(G_dynamic_data->trails[the_index],PFUFLAME_BEGIN_COLOR,PFUFLAME_END_COLOR);
       }
       else 
#endif //NONPSSHIP
       {
           pfuSmokeOrigin(G_dynamic_data->trails[the_index],dust_origin,trail_rad);
           pfuSmokeDir(G_dynamic_data->trails[the_index],PFUTRAIL_DIRECTION);
           pfuSmokeColor(G_dynamic_data->trails[the_index],PFUTRAIL_BEGIN_COLOR,PFUTRAIL_END_COLOR);
       }
       pfuSmokeMode(G_dynamic_data->trails[the_index],PFUSMOKE_START);
   }
}
#endif



void VEHICLE::changestatus(unsigned int stat)
{
#ifndef NO_PFSMOKE
  float veh_rad;

  veh_rad=(pfDistancePt3(bounding_box.max,bounding_box.min)/2);

  status=(status&status_lock)|(stat&(~status_lock));
#else
  status = stat;
#endif
 
  if (displaying_model || birds_eye)
     {
     if (status & Appearance_DamageDestroyed)  
	pfSwitchVal(models,DEAD_MODEL);
     else
        // Switch to the live model
        pfSwitchVal(models,ALIVE);

#ifndef NO_PFSMOKE
     if ((status&Appearance_Flaming)&&!(smoke_status&Appearance_Flaming)) {

        // Go to next pfuSmoke and turn it off
        //   (just in case it was on previously)
        G_pfsmoke_flames_count = (G_pfsmoke_flames_count + 1) % PFUMAXSMOKES;
        flame_index = G_pfsmoke_flames_count;
        pfuSmokeMode(G_dynamic_data->flames[flame_index],PFUSMOKE_STOP);
        G_dynamic_data->flames[flame_index]->prevTime = -1.0f;

        setSmoke(flame_index,FLAME);
        smoke_status|=Appearance_Flaming;
     }
     else if(!(status&Appearance_Flaming)&&(smoke_status&Appearance_Flaming)) {
        pfuSmokeMode(G_dynamic_data->flames[flame_index],PFUSMOKE_STOP);
        smoke_status^=Appearance_Flaming;
     }

     if ((status&Appearance_SmokePlume) &&
         !(smoke_status&Appearance_SmokePlume)) {
        // Go to next pfuSmoke and turn it off
        //   (just in case it was on previously)
        G_pfsmoke_smokes_count = (G_pfsmoke_smokes_count + 1) % PFUMAXSMOKES;
        smoke_index = G_pfsmoke_smokes_count;
        pfuSmokeMode(G_dynamic_data->smokes[smoke_index],PFUSMOKE_STOP);
        G_dynamic_data->smokes[smoke_index]->prevTime = -1.0f;
        setSmoke(smoke_index,SMOKE);
        smoke_status|=Appearance_SmokePlume;
     }
     else if (!(status&Appearance_SmokePlume) &&
              (smoke_status&Appearance_SmokePlume)) {
        pfuSmokeMode(G_dynamic_data->smokes[smoke_index],PFUSMOKE_STOP);
        smoke_status^=Appearance_SmokePlume;
     }

     if ((status&Appearance_TrailingEffects_Mask) !=
         (smoke_status&Appearance_TrailingEffects_Mask)) {
        if(pfLengthVec3(velocity)>0) {
           if(!(status&Appearance_TrailingEffects_Mask)) {
              pfuSmokeMode(G_dynamic_data->trails[trail_index],PFUSMOKE_STOP);
              smoke_status&=(~Appearance_TrailingEffects_Mask);
           }
           else {
              if(smoke_status&Appearance_TrailingEffects_Mask) {
                 switch(status&Appearance_TrailingEffects_Mask) {
                    case Appearance_TrailingEffectsLarge:
                       G_dynamic_data->trails[trail_index]->radius=
                          (veh_rad*PFUTRAIL_RADIUS*1.0f);
                       break;
                    case Appearance_TrailingEffectsMedium:
                       G_dynamic_data->trails[trail_index]->radius=
                          (veh_rad*PFUTRAIL_RADIUS*0.75f);
                       break;
                    case Appearance_TrailingEffectsSmall:
                       G_dynamic_data->trails[trail_index]->radius=
                          (veh_rad*PFUTRAIL_RADIUS*0.50f);
                       break;
                 }
                 smoke_status&=(~Appearance_TrailingEffects_Mask);
                 smoke_status|=(status&Appearance_TrailingEffects_Mask);
              }
              else {
                 // Go to next pfuSmoke and turn it off
                 //   (just in case it was on previously)
                 G_pfsmoke_trails_count = 
                    (G_pfsmoke_trails_count + 1) % PFUMAXSMOKES;
                 trail_index = G_pfsmoke_trails_count;
                 pfuSmokeMode(G_dynamic_data->flames[trail_index],
                              PFUSMOKE_STOP);
                 G_dynamic_data->flames[trail_index]->prevTime = -1.0f;

                 setSmoke(trail_index,TRAIL);
                 smoke_status&=(~Appearance_TrailingEffects_Mask);
                 smoke_status|=(status&Appearance_TrailingEffects_Mask);
              }
           }
        }
     }
#endif

     }
#ifndef NO_PFSMOKE
     else {
        if(smoke_status&Appearance_Flaming) {
           pfuSmokeMode(G_dynamic_data->flames[flame_index],PFUSMOKE_STOP);
           smoke_status^=Appearance_Flaming;
        }
        if(smoke_status&Appearance_SmokePlume) {
           pfuSmokeMode(G_dynamic_data->smokes[smoke_index],PFUSMOKE_STOP);
           smoke_status^=Appearance_SmokePlume;
        }
        if(smoke_status&Appearance_TrailingEffects_Mask) {
           pfuSmokeMode(G_dynamic_data->trails[trail_index],PFUSMOKE_STOP);
           smoke_status&=(~Appearance_TrailingEffects_Mask);
        }
     }
#endif

}


void VEHICLE::bbox ( int display )
{
   if ( bbox_geom != NULL ) {
      if (display) {
         if ( G_dynamic_data->use_bounding_boxes ) { 
            pfSwitchVal(bbox_geom,PFSWITCH_ON);
         }
      }
      else {
         pfSwitchVal(bbox_geom,PFSWITCH_OFF);
      }
   }
}


int VEHICLE::toggle_bbox ()
{
   if ( bbox_geom != NULL ) {
      if ( (pfGetSwitchVal(bbox_geom)) == PFSWITCH_ON ) {
         pfSwitchVal(bbox_geom,PFSWITCH_OFF);
         return FALSE;
      }
      else {
         if ( G_dynamic_data->use_bounding_boxes ) {
            pfSwitchVal(bbox_geom,PFSWITCH_ON);
            return TRUE;
         }
         else {
            return FALSE;
         }
      }
   }
   return FALSE;
}


void VEHICLE::changeWeaponStatus(unsigned int)
{} // Now only for DIs


unsigned int VEHICLE::getstatus()
   {
   return status;
   }


void VEHICLE::turnoff() 
   {
   pfSwitchVal(models,PFSWITCH_OFF);
   displaying_model = FALSE;
   }


void VEHICLE::turnon()
   {
   displaying_model = TRUE;
   
   this->toggle_model();
   }


void VEHICLE::toggle_model()
// Turn the model off it is on and off if it is on
{
   if (displaying_model || birds_eye) {
      if (status & Appearance_DamageDestroyed) {
         // Switch to the dead model
         pfSwitchVal(models,DEAD_MODEL);
      }
      else {
         // Switch to the live model
         pfSwitchVal(models,ALIVE);
      }
   }
}


int VEHICLE::birds_eye_view(int value)
{
   birds_eye = !value;
   if (birds_eye && !displaying_model) {
      if (status & Appearance_DamageDestroyed) {
        // Switch to the dead model
        pfSwitchVal(models,DEAD_MODEL);
      }
      else {
        // Switch to the live model
        pfSwitchVal(models,ALIVE);
      }
   }
   else if (!birds_eye && !displaying_model) {
      pfSwitchVal(models,PFSWITCH_OFF);
   }
   else {
   }
#ifndef NO_PFSMOKE
   changestatus(status);
#endif
   return birds_eye;
}


void VEHICLE::teleport(pfCoord &pos, float &speed)
{
   pfMatrix orient;
   /* Mod by Shirley for addition of DIs */
   pfVec3 direction;  
   if ((gettype() == DI_GUY) || (gettype() == JACK_DI)
     ||  (gettype() == JACK_TEAM) || (gettype() == PERSON))
	pfSetVec3(direction, 0.0f, -T_P_DI_DISTANCE, 0.0f );
   else
	pfSetVec3(direction, 0.0f, -T_P_DISTANCE, 0.0f );

#ifdef TRACE
   cerr << "Tethered to vehicle No. " << vid << endl;
#endif

   pfMakeEulerMat ( orient, posture.hpr[HEADING], 
			    posture.hpr[PITCH], 
      			    posture.hpr[ROLL]);
	  
   pfFullXformPt3 ( direction, direction, orient );
        
   pfAddVec3 (pos.xyz, posture.xyz, direction);

   speed = pfLengthVec3(velocity);

#ifdef DEBUG
   cerr << "Set speed to "<< speed << endl;
#endif

   if ( (!G_static_data->stealth) && (G_vehlist[G_drivenveh].vehptr) )
     G_vehlist[G_drivenveh].vehptr->init_velocity(velocity);   

   pfCopyVec3(pos.hpr, posture.hpr);
}


void VEHICLE::attach(pfCoord &pos, float &speed, float distance)
   {
   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 toward_center;

   pfMakeEulerMat ( orient, pos.hpr[HEADING],
                            pos.hpr[PITCH],
                            pos.hpr[ROLL]);

   pfFullXformPt3 ( direction, direction, orient );
   pfNormalizeVec3 ( direction );
   pfScaleVec3 ( direction, -distance, direction );

   pfFullXformPt3 ( toward_center, bounding_box_center, orient );
   pfAddVec3 ( direction, direction, toward_center );

   pfAddVec3 ( pos.xyz, posture.xyz, direction );
   speed = pfLengthVec3(velocity);

   }


movestat VEHICLE::move()
// move the vehicle for the next frame
{
   pfMatrix orient;
   pfVec3 direction = { 0.0f, 1.0f, 0.0f };
   pfVec3 up = { 0.0f, 0.0f, 1.0f };
   float speed_setting;
   pfVec3 oldpos;
   pfVec3 tempeye;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tVEHICLE::move" << endl;
#endif

#ifdef TRACE
   cerr << "Moving a Base Class Vehicle\n";
#endif

   update_time ();
   collect_inputs ();

   speed_setting = cdata.thrust * MAX_SPEED;
   
   
   pfCopyVec3(oldpos, posture.xyz);

   if (!(paused || tethered || cdata.rigid_body) )
      {
      // Move to the location we should be at after the last time interval
      posture.xyz[X] += velocity[X] * cdata.delta_time;
      posture.xyz[Y] += velocity[Y] * cdata.delta_time;
      posture.xyz[Z] += velocity[Z] * cdata.delta_time;
      tempeye[X] = eyepoint[X] + (velocity[X] * cdata.delta_time);
      tempeye[Y] = eyepoint[Y] + (velocity[Y] * cdata.delta_time);
      tempeye[Z] = eyepoint[Z] + (velocity[Z] * cdata.delta_time);

      altitude = 5.0f;
      // Now see if we are going to hit something in this time interval    
      
      if (!G_static_data->stealth)
         {
         if ( check_collide( oldpos,posture.xyz,
                             posture.hpr,
                             speed, altitude) )  
            return(HITSOMETHING);
         }

      // Calculate the requested speed
      if ((input_control == NPS_FCS) || (input_control == NPS_KAFLIGHT) || 
          (input_control == NPS_BASIC_PANEL) || (input_control == NPS_FLYBOX) ) 
         {
         if (speed < speed_setting)
            speed += FCS_SPEED_CHANGE;
         if (speed > speed_setting)
            speed -= FCS_SPEED_CHANGE;
         }
      else   // Using the keyboard or spaceball    
         {
         speed += cdata.thrust;
         if ((speed < KEY_SPEED_CHANGE) && (speed > 0.0f) )
            speed -= 1.0f;
         cdata.thrust = 0.0f;
         }
      // Don't want to exceed the max speed
      if (speed > MAX_SPEED)
         speed = MAX_SPEED;
      // Don't want to go backwards too fast
      if (speed < MAX_REVERSE)
         speed = MAX_REVERSE;        

      // Determine the new heading, pitch, and roll. Set them in the 
      // reverse order due to angle dependancies
      posture.hpr[ROLL] = 
              fmod ( cdata.roll + posture.hpr[ROLL], 360.0f);
      // Roll limits -180 to +180
      if (posture.hpr[ROLL] > 180.0f)
          posture.hpr[ROLL] -= 360.0f;
      if (posture.hpr[ROLL] < -180.0f)
          posture.hpr[ROLL] += 360.0f;
     
      posture.hpr[PITCH] =
           fmod ( cosf(posture.hpr[ROLL]*DEG_TO_RAD) * cdata.pitch
                   + posture.hpr[PITCH], 180.0f);
      // Pitch limits -90 to +90 
      if (posture.hpr[PITCH] > 90.0f)
         {
         posture.hpr[PITCH] = 180.0f - posture.hpr[PITCH];
         posture.hpr[ROLL] -= 180.0f;
         posture.hpr[HEADING] -= 180.0f;
         if ( posture.hpr[HEADING] < 0.0f)
             posture.hpr[HEADING] += 360.0f;  
         }
      if (posture.hpr[PITCH] < -90.0f)
         {
         posture.hpr[PITCH] = -180.0f - posture.hpr[PITCH];
         posture.hpr[ROLL] -= 180.0f;
         posture.hpr[HEADING] -= 180.0f;
         if ( posture.hpr[HEADING] < 0.0f)
             posture.hpr[HEADING] += 360.0f;  
         }
          
      // Calculate the heading 
      posture.hpr[HEADING] = fmod (( posture.hpr[HEADING] - 
             ((cdata.pitch+1.0f) * sinf(posture.hpr[ROLL] * 
             DEG_TO_RAD) * speed/MAX_SPEED)),360.0f);
      // Heading limits 0 to 360
      if (posture.hpr[HEADING] < 0.0f) 
         posture.hpr[HEADING] += 360.0f;   

      // Zero the control data inputs
      cdata.pitch = cdata.roll = 0.0f;

      // Make sure we are still in bounds, if not, stop it at the boundary.
      if (at_a_boundary(posture.xyz))
         {    
         posture.hpr[HEADING] = 
	      fmod(posture.hpr[HEADING] + 180.0f, 360.0f);
         pfNegateVec3(velocity,ZeroVec); 
         } 
      else  // Still in bounds
         {
         // Calculate the new velocity vector
         pfMakeEulerMat ( orient, posture.hpr[HEADING], 
        	                  posture.hpr[PITCH], 
                                  posture.hpr[ROLL]);
      
         pfFullXformPt3 ( direction, direction, orient );
         pfNormalizeVec3 ( direction );
         pfScaleVec3 ( velocity, speed, direction);
         }
  

      } // End !paused !tethered !rigid_body

   // If tethered, let the other vehicle calculate our position for us 
   if (tethered)
      {
      if ((cdata.vid > -1) && G_vehlist[cdata.vid].vehptr)
         {
         if ( tethered == 1)
            G_vehlist[cdata.vid].vehptr->teleport(posture,speed);
         else if ( tethered == 2)
            G_vehlist[cdata.vid].vehptr->attach(posture,speed,
                                                G_dynamic_data->attach_dis);
         }
      else
         {
#ifdef DATALOCK
         pfDPoolLock ( (void *) G_dynamic_data );
#endif
         tethered = FALSE;
#ifdef DATALOCK
         pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
         }
      }

   pfMakeEulerMat ( orient, posture.hpr[HEADING],
                            posture.hpr[PITCH],
                            posture.hpr[ROLL]);

   pfFullXformPt3 ( up, up, orient );
   pfNormalizeVec3 ( up );
   pfScaleVec3 ( up, EYE_OFFSET, up);
   pfAddVec3 ( eyepoint, posture.xyz, up );
   pfCopyVec3 ( firepoint, eyepoint );

   altitude = ::get_altitude(posture.xyz);   
   if ( (!tethered) && (!G_static_data->stealth) && (altitude < -0.1) )
      {
#ifdef DATALOCK
      pfDPoolLock ( (void *) G_dynamic_data );
#endif
//      G_dynamic_data->cause_of_death = UNDER_GROUND_DEATH;
//      G_dynamic_data->dead = DYING;
      posture.xyz[PF_Z] -= altitude;
#ifdef DATALOCK
      pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
      }

   check_targets ( posture.xyz, posture.hpr, 
                   look.hpr );

   //put the vehicle in the correct location in space
   pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
   pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);

#ifdef DEBUG_SCALE
   pfDCSScale(hull,2.0f);
#endif

   thrust = cdata.thrust;

   return(FINEWITHME);

}


int VEHICLE::check_targets ( pfVec3 &endpnt,
                             pfVec3 &orientation, pfVec3 &aux_orient )
{
   (endpnt);
   (orientation);
   (aux_orient);
   pfMatrix Orient;
   pfVec3 rl_vec = { 1.0f, 0.0f, 0.0f };
   pfVec3 tb_vec = { 0.0f, 0.0f, 1.0f };
   pfVec3 fb_vec = { 0.0f, 1.0f, 0.0f };
   pfVec3 tempvec;
   pfSegSet the_aim;
   pfSeg *aim_segment[5];
//   pfCylinder the_barrel;
   pfHit    **aim_hits[32];
   pfVec3 tempright;
   pfVec3 tempup;
   int i;
   pfCoord weapon_view;

#ifdef DATALOCK
   pfDPoolLock ( (void *) G_dynamic_data );
#endif
   G_dynamic_data->target_lock = FALSE;
   if (G_dynamic_data->target != -1)
      {
      if ( G_vehlist[G_dynamic_data->target].vehptr != NULL )
         G_vehlist[G_dynamic_data->target].vehptr->bbox(FALSE);
      G_dynamic_data->target = -1;
      }
#ifdef DATALOCK
   pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

   if ( !targetting )
      return FALSE;

   get_weapon_view(weapon_view);

   // Check dynamic vehicles to see if we can lock weapons onto one
   pfMakeEulerMat ( Orient, weapon_view.hpr[PF_H],
                            weapon_view.hpr[PF_P], weapon_view.hpr[PF_R] );
   pfFullXformPt3 ( rl_vec, rl_vec, Orient );
   pfFullXformPt3 ( tb_vec, tb_vec, Orient );
   pfFullXformPt3 ( fb_vec, fb_vec, Orient );
/*
   pfCopyVec3 (the_aim.segs[0].pos, endpnt);
   pfCopyVec3 ( the_aim.segs[0].dir, fb_vec );
   pfScaleVec3 ( tempvec, EYE_OFFSET, tb_vec );
   pfAddVec3 ( the_aim.segs[0].pos, the_aim.segs[0].pos, tempvec );

   get_firepoint(the_aim.segs[0].pos);
   pfAddVec3 ( the_aim.segs[0].pos, the_aim.segs[0].pos, endpnt );
*/
   pfCopyVec3 ( the_aim.segs[0].pos, weapon_view.xyz );
   pfCopyVec3 ( the_aim.segs[0].dir, fb_vec );

   if(the_aim.segs[0].dir[Y] == 0.0) the_aim.segs[0].dir[Y] = 0.01f;
   if(the_aim.segs[0].dir[X] == 0.0) the_aim.segs[0].dir[X] = 0.01f;
   pfNormalizeVec3 (the_aim.segs[0].dir);
   the_aim.segs[0].length = AIM_DISTANCE;

   pfNormalizeVec3 ( rl_vec );
   pfNormalizeVec3 ( tb_vec );

   pfScaleVec3 ( rl_vec, AIM_OFFSETS, rl_vec );
   pfScaleVec3 ( tb_vec, AIM_OFFSETS, tb_vec );
   pfAddVec3 ( the_aim.segs[1].pos, the_aim.segs[0].pos, rl_vec );
   pfSubVec3 ( the_aim.segs[2].pos, the_aim.segs[0].pos, rl_vec );
   pfAddVec3 ( the_aim.segs[3].pos, the_aim.segs[0].pos, tb_vec );
   pfSubVec3 ( the_aim.segs[4].pos, the_aim.segs[0].pos, tb_vec );

   pfScaleVec3 ( tempvec, AIM_DISTANCE, the_aim.segs[0].dir );
   pfAddVec3 ( tempvec, tempvec, the_aim.segs[0].pos );

   pfScaleVec3 ( tempright, AIM_OFFSETS*20.0f, rl_vec );
   pfScaleVec3 ( tempup, AIM_OFFSETS*20.0f, tb_vec );

   pfAddVec3 ( the_aim.segs[1].dir, tempvec, tempright );
   pfSubVec3 ( the_aim.segs[1].dir, the_aim.segs[1].dir, the_aim.segs[1].pos );
   pfSubVec3 ( the_aim.segs[2].dir, tempvec, tempright );
   pfSubVec3 ( the_aim.segs[2].dir, the_aim.segs[2].dir, the_aim.segs[2].pos );

   pfAddVec3 ( the_aim.segs[3].dir, tempvec, tempup);
   pfSubVec3 ( the_aim.segs[3].dir, the_aim.segs[3].dir, the_aim.segs[3].pos );

   pfSubVec3 ( the_aim.segs[4].dir, tempvec, tempup );
   pfSubVec3 ( the_aim.segs[4].dir, the_aim.segs[4].dir, the_aim.segs[4].pos );


   for (i = 1; i < 5; i++ )
      {
      if(the_aim.segs[i].dir[Y] == 0.0) the_aim.segs[i].dir[Y] = 0.01f;
      if(the_aim.segs[i].dir[X] == 0.0) the_aim.segs[i].dir[X] = 0.01f;
      the_aim.segs[i].length = pfLengthVec3(the_aim.segs[i].dir);
      pfNormalizeVec3(the_aim.segs[i].dir);
      }
   for (i = 0; i < 5; i++ )
      aim_segment[i] = &(the_aim.segs[i]);


//   pfCylAroundSegs ( &the_barrel, (const pfSeg **)aim_segment, 5 );
   the_aim.userData = NULL;
   the_aim.mode = PFTRAV_IS_GEODE;
   the_aim.activeMask = 0x01; // 0x1F;
   the_aim.isectMask = ENTITY_MASK;
   the_aim.bound = NULL; // &the_barrel;
   the_aim.discFunc = target_lock;

   if ( (pfNodeIsectSegs(G_moving, &the_aim, aim_hits)) > 0 ) {
      return TRUE;
   }
   else {
      return FALSE;
   }

}




int VEHICLE::check_collide(pfVec3 &startpnt, pfVec3 &endpnt,
                           pfVec3 &orientation,
                           float &speed, float &altitude)
{
   long     isect;
   pfSegSet seg;
   pfHit    **hits[32];
   long     flags;
//   pfNode   *node;
   pfGeoSet *gset;
   pfVec3   normal;
   pfVec3   pnt;
   pfMatrix xform;
   int hit_index;
   pfSeg *sp[2] ;
   pfCylinder the_barrel;
   long the_mask;
   pfVec3 incoming_vec;
   pfVec3 outgoing_vec;
   pfVec3 normal_vec;
   pfVec3 eye_vec, foot_vec;
   pfVec3 start_eye, end_eye;
   pfVec3 start_pos, end_pos;
   float temp_float;
   pfMatrix Orient;
   pfVec3 tb_vec = { 0.0f, 0.0f, 1.0f };
   pfVec3 fb_vec = { 0.0f, 1.0f, 0.0f };
   vtype what_am_i;
   int the_item_hit;

// cerr << "start startpnt = " << startpnt[X] << ", " << startpnt[Y]
//      << ", " << startpnt[Z] << endl;
// cerr << "start endpnt = " << endpnt[X] << ", " << endpnt[Y]
//      << ", " << endpnt[Z] << endl;
// cerr << "start velocity = " << velocity[X] << ", " << velocity[Y]
//      << ", " << velocity[Z] << endl;

   what_am_i = gettype();

   if (int(pfEqualVec3(startpnt,endpnt))) 
      {

#ifdef TRACE
      cerr << " NOT MOVING -> NO collisions\n";
#endif

      return FINEWITHME;
      }

#ifdef TRACE   
   cerr << "startpnt " << startpnt[X] << " " <<  startpnt[Y] 
        << " " <<  startpnt[Z] << endl;

   cerr << "endpnt "<< endpnt[X] << " " <<  endpnt[Y] 
        << " " <<  endpnt[Z] << endl;
#endif

   pfSetVec3 ( tb_vec, 0.0f, 0.0f, 1.0f );
   pfMakeEulerMat ( Orient, orientation[HEADING],
                            orientation[PITCH],
                            orientation[ROLL] );
   pfFullXformPt3 ( tb_vec, tb_vec, Orient );
   pfScaleVec3 ( eye_vec, EYE_OFFSET, tb_vec );
   pfScaleVec3 ( foot_vec, INTERSECT_OFFSET, tb_vec );
   pfAddVec3 ( start_eye, startpnt, eye_vec );
   pfAddVec3 ( end_eye, endpnt, eye_vec );
   pfAddVec3 ( start_pos, startpnt, foot_vec );
   pfAddVec3 ( end_pos, endpnt, foot_vec );

   
   //create a segment along the path we just followed
   pfSetVec3(seg.segs[0].dir,(end_pos[X] - start_pos[X]),
                        (end_pos[Y] - start_pos[Y]),
                        (end_pos[Z] - start_pos[Z]));
   pfSetVec3(seg.segs[1].dir,(end_eye[X] - start_eye[X]),
                        (end_eye[Y] - start_eye[Y]),
                        (end_eye[Z] - start_eye[Z]));

   seg.segs[0].length = pfLengthVec3(seg.segs[0].dir);
   seg.segs[1].length = pfLengthVec3(seg.segs[1].dir);

   PFCOPY_VEC3(seg.segs[0].pos,start_pos);
   PFCOPY_VEC3(seg.segs[1].pos,start_eye);

   //fix a performer bug
   if(seg.segs[0].dir[Y] == 0.0) seg.segs[0].dir[Y] = 0.01f;
   if(seg.segs[0].dir[X] == 0.0) seg.segs[0].dir[X] = 0.01f;
   if(seg.segs[1].dir[Y] == 0.0) seg.segs[1].dir[Y] = 0.01f;
   if(seg.segs[1].dir[X] == 0.0) seg.segs[1].dir[X] = 0.01f;

   pfNormalizeVec3(seg.segs[0].dir);
   pfNormalizeVec3(seg.segs[1].dir);
 
   sp[0] = &(seg.segs[0]);
   sp[1] = &(seg.segs[1]);

  pfCylAroundSegs ( &the_barrel, (const pfSeg **)sp, 2 );
#ifdef TRACE
   cerr << "Chck Pt 2 Number of Kids " << pfGetNumChildren(G_moving) <<"\n";
#endif


   //*** intersect test against the fixed objects ********************//

   if (altitude < 1000.0f)
      {
      seg.mode = PFTRAV_IS_PRIM;
      seg.userData = NULL;
      seg.activeMask = 0x03;
      seg.isectMask = DESTRUCT_MASK;
      seg.bound = NULL;
      seg.discFunc = buildinghit;
      isect = pfNodeIsectSegs(G_fixed, &seg, hits);
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      if ( flags )
         {
         pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
         }
      else
         {
         pfQueryHit(*hits[1], PFQHIT_POINT, &pnt);
         pfQueryHit(*hits[1], PFQHIT_XFORM, &xform);
         pfQueryHit(*hits[1], PFQHIT_FLAGS, &flags);
         }
      if (isect && (flags & PFHIT_POINT))
         {
         /*set the point at the intersectin point in the world COORDINATES*/
         pfFullXformPt3(posture.xyz, pnt, xform);

#ifdef DEBUG
         cout << "\n\nBuilding hit at " << posture.xyz[X] << " "
                                        << posture.xyz[Y] << " "
                                        << posture.xyz[Z] << endl;
#endif
         what_am_i = gettype();
         if ( ( what_am_i == GND_VEH ) || ( what_am_i == PERSON)
#ifdef JACK
            || ( what_am_i == JACK_DI) || ( what_am_i == JACK_TEAM)
#endif
                                    )
            {
            pfSubVec3 ( fb_vec, start_pos, end_pos);
            pfScaleVec3 ( fb_vec, speed, fb_vec );
            pfCopyVec3 ( posture.xyz, startpnt );
            pfAddVec3 ( posture.xyz, posture.xyz, fb_vec );
            speed = -1.0f;
            }
         else
            {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->dead = DYING;
            G_dynamic_data->cause_of_death = S_OBJECT_DEATH;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
            senddetonation(posture.xyz);
            }
         return(HITSOMETHING);
         }
      }

   //******* intersect test against the ground *********************//
   seg.mode = PFTRAV_IS_PRIM|PFTRAV_IS_NORM;
   seg.userData = NULL;
   seg.activeMask = 0x03;
   seg.isectMask = PERMANENT_MASK;
   seg.bound = NULL;
   seg.discFunc = NULL;
   if (pfNodeIsectSegs(G_fixed, &seg, hits))
      {
      pfQueryHit(*hits[0], PFQHIT_FLAGS, &flags);
      if ( flags )
          {
//          pfQueryHit(*hits[0], PFQHIT_NODE, &node); 
          pfQueryHit(*hits[0], PFQHIT_GSET, &gset);
          pfQueryHit(*hits[0], PFQHIT_NORM, &normal);
          pfQueryHit(*hits[0], PFQHIT_XFORM, &xform);
          pfQueryHit(*hits[0], PFQHIT_POINT, &pnt);
          hit_index = 0;
          }
      else
          {
          pfQueryHit(*hits[1], PFQHIT_FLAGS, &flags);
//          pfQueryHit(*hits[1], PFQHIT_NODE, &node);
          pfQueryHit(*hits[1], PFQHIT_GSET, &gset);
          pfQueryHit(*hits[1], PFQHIT_NORM, &normal);
          pfQueryHit(*hits[1], PFQHIT_XFORM, &xform);
          pfQueryHit(*hits[1], PFQHIT_POINT, &pnt);
          hit_index = 1;
          }

//      the_mask = pfGetNodeTravMask(node, PFTRAV_ISECT);
      the_mask = pfGetGSetIsectMask(gset);
      the_item_hit = int((the_mask & PERMANENT_MASK)>>PERMANENT_SHIFT);

      // find the orientation of the ground
//cerr << "HIT SOMETHING IN G_DIRT\n";
      if ( (((PERMANENT_DIRT & the_item_hit) == PERMANENT_DIRT) ||
            ((PERMANENT_BRIDGE & the_item_hit)  == PERMANENT_BRIDGE)) &&
           (flags & PFHIT_NORM) )
         {
         pfVec3 head, head90, hpr;
         float sh, ch;
         float dotp;
//cerr << "HIT GROUND!\n";
         pfSinCos(posture.hpr[HEADING], &sh, &ch);
         head[0] = -sh;
         head[1] =  ch;
         head[2] = 0.0f;
         dotp = PFDOT_VEC3(head, normal);
         hpr[PITCH] = pfArcCos(dotp) - 90.0f;
         head90[0] =  ch;
         head90[1] =  sh;
         head90[2] = 0.0f;
         dotp = PFDOT_VEC3(head90, normal);
         hpr[ROLL]  = 90.0f - pfArcCos(dotp);

         //  Check to see if we are at a safe landing angle
         if ( ( ABS(posture.hpr[PITCH] - hpr[PITCH]) > SAFE_ANG ) ||
              ( ABS(posture.hpr[ROLL] - hpr[ROLL]) > SAFE_ANG ) )
              {
#ifdef DEBUG
         cout << "\n\nGround strike at " << result[hit_index].point[X] << " "
                                         << result[hit_index].point[Y] << " "
                                         << result[hit_index].point[Z] << endl;
#endif
               if ( what_am_i != GND_VEH )
                  {
#ifdef DATALOCK
                  pfDPoolLock ( (void *) G_dynamic_data ); 
#endif
                  G_dynamic_data->dead = DYING;
                  if ( the_item_hit == PERMANENT_DIRT )
                     G_dynamic_data->cause_of_death = GROUND_DEATH;
                  else
                     G_dynamic_data->cause_of_death = BRIDGE_DEATH;
#ifdef DATALOCK
                  pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
                  return(HITSOMETHING);
                  }
               else if ( the_item_hit == PERMANENT_BRIDGE )
                  {
                  pfSubVec3 ( fb_vec, start_pos, end_pos);
                  pfScaleVec3 ( fb_vec, speed, fb_vec );
                  pfCopyVec3 ( posture.xyz, startpnt );
                  pfAddVec3 ( posture.xyz, posture.xyz, fb_vec );
                  speed = -1.0f;
                  }


                
               }   
         }      // end intersection test

      else if ( ( the_item_hit == PERMANENT_FOILAGE) || 
                ( the_item_hit == PERMANENT_WATER ) )
         { // Hit the water or foilage
//cerr << "HIT WATER OR FOILAGE\n";
        if ( what_am_i != GND_VEH )
           {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->dead = DYING;
            if (the_item_hit == PERMANENT_FOILAGE)
               G_dynamic_data->cause_of_death = FOILAGE_DEATH;
            else
               G_dynamic_data->cause_of_death = WATER_DEATH;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
            return(HITSOMETHING);
           }
        else if (the_item_hit == PERMANENT_WATER )
           {
#ifdef DATALOCK
            pfDPoolLock ( (void *) G_dynamic_data );
#endif
            G_dynamic_data->dead = DYING;
            G_dynamic_data->cause_of_death = WATER_DEATH;
#ifdef DATALOCK
            pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
            return(HITSOMETHING);
           }
         } 

      else if ( (the_item_hit == PERMANENT_TUNNEL) &&  
                (flags & PFHIT_NORM) 
               && (flags & PFHIT_POINT) )
         {
         float ordered_heading;
         float pitch_angle, roll_angle, bearing_angle;
         pfMatrix orient;
         pfVec3 direction = { 0.0f, 1.0f, 0.0f };
// cerr << "HIT TUNNEL\n";
// tunnel stuff
//         pfQueryHit(*hits[0], PFQHIT_XFORM, xform);
//         pfFullXformPt3 ( pnt, pnt, xform );
//              << ", " << endpnt[Z] << endl;
         pfSubVec3 ( incoming_vec, startpnt, endpnt);
         pfNormalizeVec3 ( incoming_vec );
//         cerr << "incoming_vec " << incoming_vec[X] << ", "
//              << incoming_vec[Y] << ", " << incoming_vec[Z] << endl;
//         xform[0][0] = -1.0f;
//         xform[1][1] = -1.0f;
//         xform[2][2] = -1.0f;
//         xform[3][3] = -1.0f;
//         pfFullXformPt3(normal_vec, normal, xform);
           normal_vec[X] = normal[X];
           normal_vec[Y] = normal[Y];
           normal_vec[Z] = normal[Z];
//         int i_1, j_1;
//         cerr << "xform" << endl;
//         for(i_1 = 0 ; i_1 <= 3 ; i_1++)
//            {
//            for(j_1 = 0 ; j_1 <= 3 ; j_1++)
//               {
//               cerr << xform[i_1][j_1] << " ";
//               }
//               cerr << endl;
//            }
         
         pfNormalizeVec3 ( normal_vec );
//         cerr << "normal_vec " << normal_vec[X] << ", "
//              << normal_vec[Y] << ", " << normal_vec[Z] << endl;
//         cerr << "normal " << normal[X] << ", " << normal[Y] << ","
//              << normal[Z] << endl;
         temp_float = 1.25f * pfDotVec3 ( normal_vec, incoming_vec );
         pfScaleVec3 ( outgoing_vec, temp_float, normal_vec );
         pfSubVec3 ( outgoing_vec, outgoing_vec, incoming_vec );
         pfNormalizeVec3 ( outgoing_vec );
//         cerr << "outgoing_vec " << outgoing_vec[X] << ", "
//              << outgoing_vec[Y] << ", " << outgoing_vec[Z] << endl;
         
         bearing_angle = pfArcTan2( -outgoing_vec[Y], -outgoing_vec[X] );
         pitch_angle = 1.5f * pfArcSin ( outgoing_vec[Z] );
         if (pitch_angle >= 90.0f)
            pitch_angle -= 90.0f;
         if (pitch_angle <= -90.0f)
            pitch_angle += 90.0f;
         roll_angle = 0.0f; 
         ordered_heading = (bearing_angle + 90.0f);
//         cerr << "bearing_angle = " << bearing_angle << endl;
//         cerr << "pitch_angle = " << pitch_angle << endl;
//         cerr << "roll_angle = " << roll_angle << endl;
         if (ordered_heading >= 360.0)
            ordered_heading -= 360.0f;
         if (ordered_heading < 0.0)
            ordered_heading += 360.0f;
//       cerr << "ordered_heading = " << ordered_heading << endl;
         
         pfSetVec3 ( orientation, ordered_heading, pitch_angle, roll_angle );

         pfMakeEulerMat ( orient, orientation[HEADING],
                                  orientation[PITCH],
                                  orientation[ROLL]);
         pfFullXformPt3 ( direction, direction, orient );
         pfNormalizeVec3 ( direction );
//         result.normal[Z] *= -1.0f;
         if ( hit_index == 1 )
            pfScaleVec3 ( normal, EYE_OFFSET + 5.0f, normal );
         else
            pfScaleVec3 ( normal, INTERSECT_OFFSET + 5.0f, normal );                                         
         pfAddVec3 (endpnt, pnt, normal);
//         pfAddVec3(endpnt, pnt, direction);
         altitude = endpnt[Z] - gnd_level(endpnt);;
         temp_float= 0.75f * pfLengthVec3(velocity);
         speed *= 0.75f;
         pfScaleVec3 ( velocity, temp_float, direction);
//         cerr << "velocity = " << velocity[X] << ", " << velocity[Y]
//              << ", " << velocity[Z] << endl;
         }

      }      
    
      //if we got here it did not hit anything 
      return FINEWITHME;

   }


movestat VEHICLE::moveDR(float deltatime,float curtime)
//move routine for DR vehicles simple move
{
  float accelfac;  // 1/2*T used for accelerations
  static char text[100];
  static float oldspeed = 0.0f;
  float speed;

#ifdef TRACE
   cerr << "Dead Recon a Base Class Vehicle\n";
#endif

#ifndef NO_PFSMOKE
   update_pfsmokes();
#endif

  if ( ((curtime - lastPDU) > G_static_data->DIS_deactivate_delta ) && 
       (vid != G_drivenveh))
  {
    //Deactivate the puppy
//    cerr <<"It has been to long since the last update VEHICLE\n";
    //not us, so we should deactiveate this vehcile
    return(DEACTIVE);
  }

  switch(dralgo)
  {
    case DRAlgo_Static: //it is fixed, so it does not move
      break;
    case DRAlgo_DRM_FPW: //Fixed orientation,Constant Speed
      drpos.xyz[X] += drparam.xyz[X]*deltatime;
      drpos.xyz[Y] += drparam.xyz[Y]*deltatime;
      drpos.xyz[Z] += drparam.xyz[Z]*deltatime;
      break;
    case DRAlgo_DRM_FVW: //Constant rotation,varable velocity
      accelfac = 0.5f*deltatime*deltatime;
      drpos.xyz[X] += drparam.xyz[X]*deltatime+accelfac*drparam.hpr[X];
      drpos.xyz[Y] += drparam.xyz[Y]*deltatime+accelfac*drparam.hpr[Y];
      drpos.xyz[Z] += drparam.xyz[Z]*deltatime+accelfac*drparam.hpr[Z];
      drparam.xyz[X] += accelfac * drparam.hpr[X];
      drparam.xyz[Y] += accelfac * drparam.hpr[Y];
      drparam.xyz[Z] += accelfac * drparam.hpr[Z];
      break;
    //I don't have a clue how to do the others, so we will default 
    case DRAlgo_Other:
    case DRAlgo_DRM_RPW:
    case DRAlgo_DRM_RVW:
    case DRAlgo_DRM_FPB:
    case DRAlgo_DRM_RPB:
    case DRAlgo_DRM_RVB:
    case DRAlgo_DRM_FVB:
    default:
      //move the default vehicle type in a stright line
      drpos.xyz[X] += velocity[X]*deltatime;
      drpos.xyz[Y] += velocity[Y]*deltatime;
      drpos.xyz[Z] += velocity[Z]*deltatime;
      break;
  }

  if (vid != G_drivenveh){
     posture = drpos;
     //put the vehicle in the correct location in space
     pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y],posture.xyz[Z]);
     pfDCSRot(hull,posture.hpr[HEADING],posture.hpr[PITCH], posture.hpr[ROLL]);

#ifdef DEBUG_SCALE
  pfDCSScale(hull,2.0f);
#endif

  }
  else if ( !G_static_data->stealth )
  {
    speed = pfLengthVec3(velocity);
    //this is us, have we exceeded the DR error?
    if ((G_static_data->DIS_heartbeat_delta < (curtime - lastPDU)) || 
        (delta_send(&posture,&drpos))) 
    {

#ifdef DEBUG
       cerr <<"X       " << posture.xyz[HEADING] << " " <<drpos.xyz[HEADING] << "\n";
       cerr <<"Y       " << posture.xyz[PITCH] << " " <<drpos.xyz[PITCH] << "\n";
       cerr <<"Z       " << posture.xyz[ROLL] << " " <<drpos.xyz[ROLL] << "\n";
       cerr <<"HEADING " << posture.hpr[HEADING] << " " <<drpos.hpr[HEADING] << "\n";
       cerr <<"PITCH   " << posture.hpr[PITCH] << " " <<drpos.hpr[PITCH] << "\n";
       cerr <<"ROLL    " << posture.hpr[ROLL] << " " <<drpos.hpr[ROLL] << "\n";
       cerr << "send a delta pdu\n";
       cerr << "acceleration" << acceleration[X] << " " << acceleration[Y]
                              << " " << acceleration[Z] << endl;

#endif
       sendentitystate();
       oldspeed = speed;
       if ( G_static_data->pdu_position && (G_static_data->pdupos_fd >= 0) )
          {
          sprintf ( text, "%8.2f %8.2f %8.2f %7.1f %7.1f %7.1f %14.2f\n",
                    posture.xyz[X], posture.xyz[Y], posture.xyz[Z],
                    posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL],
                    curtime );
          if (write (G_static_data->pdupos_fd, text, strlen(text)) == -1)
             {
             cerr << "Write to PDU position file failed.  Closing file.\n";
             close ( G_static_data->pdupos_fd );
             G_static_data->pdupos_fd = -1;
             }
          }
    }
    else if ( speed != oldspeed )
       {
       if ( (!IS_ZERO(oldspeed)) && (IS_ZERO(speed)) )
          {
          sendentitystate();
          oldspeed = speed;
          }
       else if ( (IS_ZERO(oldspeed)) && (!IS_ZERO(speed)) )
          {
          sendentitystate();
          oldspeed = speed;
          }
       else if ( fabs(oldspeed-speed) > 50.0f )
          {
          sendentitystate();
          oldspeed = speed;
          }
       }
  }
      return(FINEWITHME);
}


movestat VEHICLE::movedead()
{
  float gnd_elev = ::get_altitude(posture.xyz);
  static float dead_time;
  static float grnd_slam_time;
  static int hit;
  static int my_status;
  static int cause;

#ifdef CLASS_TRACE
   cerr << "CLASS TRACE:\tVEHICLE::movedead" << endl;
#endif

   update_time ();
   collect_inputs();

#ifdef DATALOCK
  pfDPoolLock ( (void *) G_dynamic_data );
#endif
  my_status = G_dynamic_data->dead;
  cause = G_dynamic_data->cause_of_death;
#ifdef DATALOCK
  pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

  if ( my_status == DYING )
     {
     if ( G_static_data->sound )
        {
        LocationRec dummy_snd_pos;
        dummy_snd_pos.x = 0.0;
        dummy_snd_pos.y = 0.0;
        dummy_snd_pos.z = 0.0;
        if ( cause == MUNITION_DEATH )
           playsound(&dummy_snd_pos, &dummy_snd_pos, HIT_INDEX,-1);
        else if ( cause == FALLING_DEATH )
           playsound(&dummy_snd_pos, &dummy_snd_pos, FALLING_INDEX,-1);
        else
           playsound(&dummy_snd_pos, &dummy_snd_pos, DYING_INDEX,-1);
        }

     if (gnd_elev >= 2.5f)
        {
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = AIRDEAD;
        my_status = AIRDEAD;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        grnd_slam_time = G_curtime;
        hit = FALSE;
        }
     else
        {
        grnd_slam_time = G_curtime;
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = GRNDDEAD;
        my_status = GRNDDEAD;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        status = Appearance_DamageDestroyed | Appearance_SmokePlume;
        changestatus(status);
        pfCopyVec3(velocity,ZeroVec);
        sendentitystate();
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        cause = G_dynamic_data->cause_of_death; 
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        if ( cause != MUNITION_DEATH )
           senddetonation(posture.xyz);
        if ( cause == GROUND_DEATH )
           makecrater_pos(posture.xyz);
        use_next_fire_plume(posture.xyz[X], posture.xyz[Y],
                            posture.xyz[Z], 20.0f );
        makefireball(posture.xyz);
        }
     }

  if ( my_status == AIRDEAD )
     {
     if (!hit)
        {
        posture.hpr[HEADING] = (float)(rand() % 360);
        posture.hpr[PITCH] = (float)-45.0f;//(rand() % 360);
        posture.hpr[ROLL] = (float)(rand() % 360);
        posture.xyz[X] += velocity[X]*cdata.delta_time;
        posture.xyz[Y] += velocity[Y]*cdata.delta_time;
        posture.xyz[Z] += velocity[Z]*cdata.delta_time;
        velocity[Z] -= 5.0f;
// As we die here, we should be checking for collisions with other
// objects
        }
     else
        {
        posture.hpr[PITCH] = 45.0f;
        posture.hpr[ROLL] = 0.0f;
        }
     if ( (gnd_elev <= 1.0f) && !hit)
        {
        speed = 0.0f;
        posture.xyz[Z] -= gnd_elev;
        status = Appearance_DamageDestroyed | Appearance_SmokePlume;
        changestatus(status);
        pfCopyVec3(velocity,ZeroVec);
        sendentitystate();
        senddetonation(posture.xyz);
        makefireball(posture.xyz);
// Can only leave a crater if we know we hit the ground (i.e. could
//   have hit water, bridge, etc.
//        makecrater_pos(posture.xyz);
        use_next_fire_plume(posture.xyz[X], posture.xyz[Y],
                            posture.xyz[Z], 20.0f );
        hit = TRUE;
        }
     if (hit && grnd_slam_time + GROUND_SLAM_DELAY < G_curtime)
        {
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = DEAD;
        my_status = DEAD;
        if ( G_static_data->sound )
           {
           LocationRec dummy_snd_pos;
           dummy_snd_pos.x = 0.0;
           dummy_snd_pos.y = 0.0;
           dummy_snd_pos.z = 0.0;
           playsound(&dummy_snd_pos,&dummy_snd_pos,DEAD_INDEX,-1);
           }
        G_dynamic_data->cause_of_death = 0;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        dead_time = G_curtime;
        }
     }
  else if ( my_status == GRNDDEAD )
     {
     posture.hpr[PITCH] = 45.0f;
     if ( grnd_slam_time + GROUND_SLAM_DELAY < G_curtime )
        {
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = DEAD;
        my_status = DEAD;
        dead_time = G_curtime;
        G_dynamic_data->cause_of_death = 0;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        }
     }
#ifndef NODEADCYCLE
  else
     {
     if (dead_time + DEAD_TIME_DELAY < G_curtime)
        {
        bzero ( (void *)&(look), sizeof(pfCoord) );
        pfCopyVec3 ( posture.hpr, 
                     (float *)G_static_data->init_location.hpr );
        G_vehlist[vid].vehptr->
              transport(G_static_data->init_location.xyz[PF_X],
                        G_static_data->init_location.xyz[PF_Y],
                        G_static_data->init_location.xyz[PF_Z] );
	
#ifdef DATALOCK
        pfDPoolLock ( (void *) G_dynamic_data );
#endif
        G_dynamic_data->dead = ALIVE;
        my_status = ALIVE;
#ifdef DATALOCK
        pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
        status = ALIVE;
        changestatus(status);
     if ( G_static_data->sound )
        {
        LocationRec dummy_snd_pos;
        dummy_snd_pos.x = 0.0;
        dummy_snd_pos.y = 0.0;
        dummy_snd_pos.z = 0.0;
        playsound(&dummy_snd_pos, &dummy_snd_pos, ALIVE_INDEX,-1);
        }



        }
     }
#endif // NODEADCYCLE


   altitude = ::get_altitude(posture.xyz);

  //put the vehicle in the correct location in space
  pfDCSTrans(hull,posture.xyz[X], posture.xyz[Y], posture.xyz[Z]);
  pfDCSRot(hull,posture.hpr[HEADING], posture.hpr[PITCH], posture.hpr[ROLL]);
     

  return FINEWITHME;
}



void VEHICLE::getposparm(pfCoord *pos, pfVec3 *vel)
// stuff for firing of weapons
{
  *pos = posture;
  pfCopyVec3(*vel,velocity);
}

EntityID VEHICLE::getdisname()
//get the DIS name of the vehcile
{
  return(disname);
}

void VEHICLE::basicentitystate(EntityStatePDU *epdu)
//fill basic  entity state pdu
{

  //fill in the parameters of an entity state PDU, this assumes there
  //are no articulated parameters
  //ID and Type

  epdu->entity_id = disname = G_vehlist[vid].DISid ;
//  epdu.entity_id.entity = disname.entity = vid + 1001;

  epdu->entity_id = disname = G_vehlist[vid].DISid ;

#ifdef DEBUG_DIS
  cerr << "DISid - " << disname.address.site << endl;
  cerr << "DISid - " << disname.address.host << endl;
  cerr << "DISid - " << disname.entity << endl;
#endif

  epdu->alt_entity_type = epdu->entity_type = type;

  //How fast is it going
  epdu->entity_velocity.x = velocity[X];
  epdu->entity_velocity.y = velocity[Y];
  epdu->entity_velocity.z = velocity[Z];

  //Where we are
  epdu->entity_location.x = posture.xyz[X];
  epdu->entity_location.y = posture.xyz[Y];
  epdu->entity_location.z = posture.xyz[Z];

  //How we are facing
  epdu->entity_orientation.psi   = -posture.hpr[HEADING]*DEG_TO_RAD;
  epdu->entity_orientation.theta =  posture.hpr[PITCH]*DEG_TO_RAD;
  epdu->entity_orientation.phi   =  posture.hpr[ROLL]*DEG_TO_RAD;

  //Send out dr accelerations and velocities
  epdu->dead_reckon_params.linear_accel[X] = acceleration[X];
  epdu->dead_reckon_params.linear_accel[Y] = acceleration[Y];
  epdu->dead_reckon_params.linear_accel[Z] = acceleration[Z];

  // We don't calculate these at the moment but for the future 
  //epdu.dead_reckon_params.angular_velocity[HEADING] = 0.0f;
  //epdu.dead_reckon_params.angular_velocity[PITCH] = 0.0f;
  //epdu.dead_reckon_params.angular_velocity[ROLL] = 0.0f;
  epdu->force_id = (ForceID)G_dynamic_data->force;

#ifdef DEBUG_DIS
  cerr << "DIS Heading (psi) = " << epdu->entity_orientation.psi 
       << "\n    Pitch (theta) = " << epdu->entity_orientation.theta
       << "\n     Roll (phi)   = " << epdu->entity_orientation.phi <<endl;
#endif

  //what we look like
  epdu->entity_appearance = status;
  //project our movement
  epdu->dead_reckon_params.algorithm = DRAlgo_DRM_FPW;
}

void VEHICLE::sendentitystate()
{
  EntityStatePDU epdu;
  char tempstr[MARKINGS_LEN];

  basicentitystate(&epdu);

  epdu.num_articulat_params = (unsigned char)0;

  epdu.entity_marking.character_set = CharSet_ASCII;

  if ( *(G_static_data->marking) )
     {
     strcpy ( (char *)epdu.entity_marking.markings, 
              (char *)G_static_data->marking );
     }
  else
     {
     strncpy ( tempstr, 
              (char *)G_static_data->hostname, MARKINGS_LEN-4 );
     strcpy ( (char *)epdu.entity_marking.markings, "NPS" );
     strcat ( (char *)epdu.entity_marking.markings, tempstr );
     }

  sendEPDU(&epdu);

#ifdef DEBUG
  cerr << "sent a Vehicle Entity State PDU\n";
#endif

  updateDRparams();
}


void VEHICLE::sendEPDU(EntityStatePDU *epdu)
{
    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          G_net->write_pdu((char *)epdu,EntityStatePDU_Type);
          break;
       }
}


void VEHICLE::updateDRparams()
{
  //update the DR parameters
  drpos = posture;
  pfCopyVec3(drparam.xyz,velocity); //speed
  pfCopyVec3(drparam.hpr,acceleration); //Acceleration

  lastPDU = G_curtime;
}


void VEHICLE::entitystateupdate(EntityStatePDU *epdu, sender_info &sender)
//update a vehicle from the network
//also assuming there is no articulated parameters
{
   //what and who I am
   disname = epdu->entity_id;
   type = epdu->entity_type;
   force = (ForceID)epdu->force_id;
   dralgo = epdu->dead_reckon_params.algorithm;
   switch(dralgo){
     case DRAlgo_Static: //it is fixed, so it does not move
      velocity[X] = velocity[Y] = velocity[Z] = 0.0f;
      break;
    case DRAlgo_DRM_FPB:
    case DRAlgo_DRM_RPB:
    case DRAlgo_DRM_RVB:
    case DRAlgo_DRM_FVB:
     //Body coordinates, lets do some magic, later
     velocity[X] = epdu->entity_velocity.x;
     velocity[Y] = epdu->entity_velocity.y;
     velocity[Z] = epdu->entity_velocity.z;
     break;

    case DRAlgo_DRM_FPW: //Fixed orientation,Constant Speed
    case DRAlgo_DRM_FVW: //Constant rotation,varable velocity
    case DRAlgo_DRM_RPW:
    case DRAlgo_DRM_RVW:
    case DRAlgo_Other:
    default:
      //World Corrdinates, we are safe with these
      velocity[X] = epdu->entity_velocity.x;
      velocity[Y] = epdu->entity_velocity.y;
      velocity[Z] = epdu->entity_velocity.z;
      break;
  }
  //Where we are
  posture.xyz[X] = (float)epdu->entity_location.x;
  posture.xyz[Y] = (float)epdu->entity_location.y;
  posture.xyz[Z] = (float)epdu->entity_location.z;

  //How we are facing
  posture.hpr[HEADING] =  ( epdu->entity_orientation.psi > 0)? 	
         - (epdu->entity_orientation.psi + 2*M_PI)*RAD_TO_DEG :
	           - (epdu->entity_orientation.psi)*RAD_TO_DEG;		

  posture.hpr[PITCH]   =  epdu->entity_orientation.theta*RAD_TO_DEG;
  posture.hpr[ROLL]    =  epdu->entity_orientation.phi*RAD_TO_DEG;
  //Acceleration vector values
  acceleration[X] = epdu->dead_reckon_params.linear_accel[X];
  acceleration[Y] = epdu->dead_reckon_params.linear_accel[Y];
  acceleration[Z] = epdu->dead_reckon_params.linear_accel[Z];

  pfCopyVec3(drparam.xyz,velocity);
  pfCopyVec3(drparam.hpr,acceleration);
  drpos = posture;

  update_articulations ( epdu->articulat_params, epdu->num_articulat_params );

  //what we look like
  status = epdu->entity_appearance;
  changestatus(status);

  bcopy ( (char *)&epdu->entity_marking, 
          (char *)&marking, sizeof(EntityMarking) );

  if ( (strcmp(controller.internet_address,sender.internet_address)) != 0 )
     {
     struct hostent *entity;
     unsigned long netaddress = inet_addr(sender.internet_address);
     strcpy ( controller.internet_address, sender.internet_address );
     entity = gethostbyaddr ( (void *)&netaddress,
                              sizeof(struct in_addr), AF_INET );
     if ( entity != NULL )
        strcpy ( controller.host_name, entity->h_name );
     else
        strcpy ( controller.host_name, "Unknown" );

     }

  //Update the time of last message
  lastPDU = G_curtime;
}


void VEHICLE::fillinDetonationPDU(DetonationPDU *Dpdu,pfVec3 impact)
//fill in the standard part of the detonation
{
  /*fill in the Fire and Munition ID*/

  bzero ( (char *)Dpdu, sizeof(DetonationPDU) );
  Dpdu->firing_entity_id.address.site = disname.address.site;
  Dpdu->firing_entity_id.address.host = disname.address.host;
  Dpdu->firing_entity_id.entity = disname.entity ;
  Dpdu->munition_id.address.site = disname.address.site;
  Dpdu->munition_id.address.host = disname.address.host;
  Dpdu->munition_id.entity =  disname.entity;

  /*the event ID*/
  Dpdu->event_id.address.site = disname.address.site;
  Dpdu->event_id.address.host = disname.address.host;
  Dpdu->event_id.event = 0;

  /*what did we shoot*/
  Dpdu->burst_descriptor.munition = type;
#ifdef DEBUG
  cerr << "DIS Type "<< 0 +type.entity_kind <<" "<< 0 +type.domain
        <<" "<<type.country<<" "<<0 +type.category<<" ";
  cerr <<0 +type.subcategory<<" "<<0 +type.specific<<" fillinDetonationPDU\n";
#endif


  Dpdu->burst_descriptor.warhead = WarheadMunition_Other;
  Dpdu->burst_descriptor.fuze = FuzeMunition_Contact;
  Dpdu->burst_descriptor.quantity = 1; /*One at a time*/
  Dpdu->burst_descriptor.rate = 0;

  /*movement parameters*/
  Dpdu->velocity.x = velocity[X];
  Dpdu->velocity.y = velocity[Y];
  Dpdu->velocity.z = velocity[Z];

  /*where we hit*/
  Dpdu->location_in_world.x = impact[X];
  Dpdu->location_in_world.y = impact[Y];
  Dpdu->location_in_world.z = impact[Z];


  /*no Aticulated Parameters,yet*/
  Dpdu->num_articulat_params = (unsigned char) 0x0;
}

void VEHICLE::senddetonation(pfVec3 impact)
{
  DetonationPDU Dpdu; //the PDU

  //allocate the memory for the message

  //fillin the standard part
  fillinDetonationPDU(&Dpdu,impact);

  //since we missed everything
  Dpdu.location_in_entity.x = 0;
  Dpdu.location_in_entity.y = 0;
  Dpdu.location_in_entity.z = 0;

  Dpdu.target_entity_id.address.site = 0;
  Dpdu.target_entity_id.address.host = 0;
  Dpdu.target_entity_id.entity = 0;

  //we hit the dirt
  Dpdu.detonation_result = DetResult_GroundImpact;

    switch ( G_dynamic_data->network_mode )
       {
       case NET_OFF:
       case NET_RECEIVE_ONLY:
          break;
       case NET_SEND_ONLY:
       case NET_SEND_AND_RECEIVE:
       default:
          /*send it*/
          if (G_net->write_pdu((char *) &Dpdu, (PDUType )DetonationPDU_Type) 
             == FALSE){
              cerr << "net->write_pdu() failed\n";
          break;
       }

  }

#ifdef DEBUG
   cout << "Sent a DetonationPDU\n";
   cout << "site " << Dpdu->munition_id.address.site
        << " host " << Dpdu->munition_id.address.host
        << " entity " << Dpdu->munition_id.entity << endl;
#endif

}


void acquire_veh_info ( INFO_PDATA *info_pdata )
   {
   static int first_time = TRUE;
   static int last_veh_index;
   int veh;
   VEHICLE *veh_ptr;

   if ( first_time )
      {
      first_time = FALSE;
      last_veh_index = -1;
      }

   if ( (last_veh_index == -1) && (info_pdata->veh_index > -1) )
      {
      /* Turn on the display of the information channel */
      pfChanTravMode(G_dynamic_data->pipe[0].channel[2].chan_ptr,
                     PFTRAV_DRAW, PFDRAW_ON);
      }
   else if ( (last_veh_index > -1) && (info_pdata->veh_index == -1) )
     {
     /* Turn off the display of the information channel */
      pfChanTravMode(G_dynamic_data->pipe[0].channel[2].chan_ptr,
                     PFTRAV_DRAW, PFDRAW_OFF);
     }

   last_veh_index = info_pdata->veh_index;
   veh = info_pdata->veh_index;

   /* If a current vehicle is selected, then update its information */
   /*    being sent to the information channel                      */

   if ( (veh > -1) && (G_vehlist[veh].vehptr != NULL) )
      {
      info_pdata->veh_type_index = G_vehlist[veh].type;
      bcopy ( (char *)&G_vehtype[info_pdata->veh_type_index].distype, 
              (char *)&info_pdata->matched_entity_type,
              sizeof(EntityID) );
      strcpy ( info_pdata->matched_model_name,
               G_vehtype[info_pdata->veh_type_index].name );

      veh_ptr = G_vehlist[veh].vehptr; 
      info_pdata->veh_posture = veh_ptr->posture;
      info_pdata->veh_class_type = veh_ptr->gettype();
      info_pdata->veh_speed = pfLengthVec3 ( veh_ptr->velocity );
      info_pdata->veh_force = veh_ptr->force;
      bcopy ( (char *)&veh_ptr->type,
              (char *)&info_pdata->veh_entity_type,
              sizeof(EntityID) );
      bcopy ( (char *)&veh_ptr->disname,
              (char *)&info_pdata->veh_entity_id, sizeof(EntityID) );
      bcopy ( (char *)&veh_ptr->marking,
              (char *)&info_pdata->veh_marking, sizeof(EntityMarking) );
      info_pdata->veh_appearance = veh_ptr->status;
      bcopy ( (char *)&veh_ptr->controller, 
              (char *)&info_pdata->veh_controller,
              sizeof(sender_info) );
      }

   }


int
select_nearest_vid_to_mouse ( radar_screen_info *radar_info,
                              pfVec3 world_center,
                              HUD_VEH_REC *hud_data,
                              float range )
{
   input_manager *l_input_manager;
   pfVec2 mouse_pos;
   float diff_X_meters, diff_Y_meters;
   pfVec3 world;
   pfMatrix temp_matrix, transform_matrix;
   float mindist;
   int closest, vehcount;
   HUD_VEH_REC *next_veh;

   if ( (G_manager != NULL) && (G_manager->ofinput != NULL) ) {
      l_input_manager = G_manager->ofinput;

      if ( l_input_manager->query_multival((void *)mouse_pos,
              NPS_KEYBOARD,NPS_MV_MOUSE_LOCATION) ) {
         diff_X_meters = (mouse_pos[PF_X] - radar_info->radar_centerX) *
                         radar_info->meters_per_pixelX;
         diff_Y_meters = (mouse_pos[PF_Y] - radar_info->radar_centerY) *
                         radar_info->meters_per_pixelY;
         world[PF_X] = world_center[PF_X] + diff_X_meters;
         world[PF_Y] = world_center[PF_Y] + diff_Y_meters;
         world[PF_Z] = world_center[PF_Z];
         pfMakeTransMat ( transform_matrix, -world_center[PF_X],
                                            -world_center[PF_Y],
                                            0.0f );
         pfMakeRotMat (temp_matrix, -radar_info->rot_amount, 0.0f, 0.0f, 1.0f);
         pfPostMultMat ( transform_matrix, temp_matrix );
         pfMakeTransMat ( temp_matrix, world_center[PF_X],
                                       world_center[PF_Y],
                                       0.0f );
         pfPostMultMat ( transform_matrix, temp_matrix );
         pfFullXformPt3 ( world, world, transform_matrix );
         
         mindist = 500000.0f;
         closest = -1;
         next_veh = hud_data;
         vehcount = 0;
         while ( (vehcount < HUD_MAX_VEH) && (next_veh->type != NOSHOW) ) {
            static float dist;
            dist = pfDistancePt2 ( world, next_veh->location );
            if ( dist <= mindist ) {
               mindist = dist;
               closest = vehcount; 
            }
            vehcount++;
            next_veh++;
         }
         if ( (closest > -1) && (mindist < (0.0625f*range) ) ) {
            return hud_data[closest].vid;
         }
      }
   }

   return -1;

}


void vehicle_hud_draw ( pfChannel *chan, void *data )
{
  PASS_DATA *pdata = (PASS_DATA *)data;

  // Used to be parameters
  // ************************
  float aspect = G_HUD_options->aspect;
  float range = G_HUD_options->range;
  int hud_level = G_HUD_options->hud_level;
  int veh_info = G_HUD_options->veh_info;
  int misc_info = G_HUD_options->misc_info;
  float eyesep = G_HUD_options->eyesep;
  int num_samples = G_HUD_options->num_samples;
  int full_screen = G_HUD_options->full_screen;
  int color = G_HUD_options->color;
  int metric = G_HUD_options->metric;
  unsigned int talpha = G_HUD_options->talpha;
  int rotmode = G_HUD_options->rotmode;
  int colormode = G_HUD_options->colormode;
  int iconmode = G_HUD_options->iconmode;
  // ************************

  int selected_vid = G_dynamic_data->last_selected_vid;
  char text[80];
  float real_heading = (360.0f - (pdata->posture.hpr[HEADING]));
  float reflength;
  float curpos[3];
  float ll[2];
  float rr[2];
  long max_font_height;
  long max_font_width;
  float inc = 1.0f;
  float line = 0.0f;
  int found,i;
  static float speed;
#ifdef IRISGL
  unsigned int alpha = talpha<<24;
  static fmfonthandle deadfont;
#else //OPENGL
  static pfuXFont *display_font;
  int gl_matrix_mode;
#endif
  float temp_size;
  static int first_time = 1;
  static float font_height;
  float screen_x;
  float screen_y;
  static float warn_time = -2.0f;
  float cur_time = pfGetTime();
  static pfMatrix identity;
  static int targetting;
  static int target_lock;
  static int dead_or_alive;
  static float bbox_xmin;
  static float bbox_xmax;
  static float bbox_ymin;
  static float bbox_ymax;
  static float view_right;
  static float view_left;
  static float view_top;
  static float view_bottom;
  static float originX;
  static float originY;
  static float mouseX;
  static float mouseY;
  static int stereo;
  static int visual_mode;
  static int static_hits;
  static int dynamic_hits;
  static int bullets_fired;
  static int missiles_fired;
  static int bombs_fired;
  static NPS_Input_Device control;
  static int control_num;
  static int max_samples;
  static float fovx;
  static int early_warn;
  static int paused;
  static int tethered;
  static int cause_of_death;

  static int compsize;
  static float completter;
  static float fieldofview;
  static float tempdir;
  static float width,height;
  static float halfX, halfY;
  static float base,top,mytop;
  static float xpos,ypos,step,startX,stopX;
  static float alt;
  static int camera;
  static int camlock;
  static long aamode;
#if 0
  static float start_txt;
#endif
  static nps_converter *coord_converter = NULL;
  static float64 northing = 0.0f;
  static float64 easting = 0.0f;

  static radar_screen_info *radar_info;

  long original_trans_mode;

#ifdef DATALOCK
  pfDPoolLock ( (void *) G_dynamic_data );
#endif
  view_right = G_dynamic_data->view.right;
  view_left = G_dynamic_data->view.left;
  view_top = G_dynamic_data->view.top;
  view_bottom = G_dynamic_data->view.bottom;
  originX = G_dynamic_data->originX;
  originY = G_dynamic_data->originY;
  mouseX = G_dynamic_data->mouse[X];
  mouseY = G_dynamic_data->mouse[Y];
  dead_or_alive = G_dynamic_data->dead;
  bbox_xmin = G_dynamic_data->bounding_box.xmin;
  bbox_xmax = G_dynamic_data->bounding_box.xmax;
  bbox_ymin = G_dynamic_data->bounding_box.ymin;
  bbox_ymax = G_dynamic_data->bounding_box.ymax;
  targetting = G_dynamic_data->targetting;
  target_lock = G_dynamic_data->target_lock;
  stereo = G_dynamic_data->stereo;
  visual_mode = G_dynamic_data->visual_mode;
  static_hits = G_dynamic_data->static_hits;
  dynamic_hits = G_dynamic_data->dynamic_hits;
  bullets_fired = G_dynamic_data->bullets_fired;
  missiles_fired = G_dynamic_data->missiles_fired;
  bombs_fired = G_dynamic_data->bombs_fired;
  control = pdata->input_control;
  control_num = pdata->input_number;
  max_samples = G_static_data->max_samples;
  fovx = G_dynamic_data->fovx;
  early_warn = G_dynamic_data->early_warn;
  paused = G_dynamic_data->paused;
  tethered = G_dynamic_data->tethered;
  cause_of_death = G_dynamic_data->cause_of_death;
  fieldofview = G_dynamic_data->fovx;
  camera = G_dynamic_data->camera;
  camlock = G_dynamic_data->camlock;
#ifdef DATALOCK
  pfDPoolUnlock ( (void *) G_dynamic_data );
#endif

  font_height = (45.0f * ((view_top - view_bottom)/1024.0f));

  if (first_time) {
     first_time = 0;
     pfMakeIdentMat(identity);
#ifdef IRISGL
     deadfont = fmscalefont(fmfindfont("Times-Roman"), font_height);
#endif
#if 0
     num_landmarks = read_landmarks( landmark );
#endif
     coord_converter = new nps_converter((roundWorldStruct *)
                                          &(G_static_data->rw));
     if ( coord_converter == NULL ) {
        cerr << "Vehicle HUD, unable to create coordinate converter."
             << endl;
     }
  }

  while ( real_heading < 0.0f ) {
     real_heading += 360.0f;
  }
  while ( real_heading > 360.0f ) {
     real_heading -= 360.0f;
  }

  aamode = pfGetAntialias();

  if ( !G_dynamic_data->birdseye && (dead_or_alive == ALIVE) )
     {

     curpos[X] = pdata->posture.xyz[X];
     curpos[Y] = pdata->posture.xyz[Y];
     curpos[Z] = pdata->posture.xyz[Z];

     radar_info = 
        draw_radar ( chan, pdata->posture.xyz,
                     G_HUD_options->range,real_heading,
                     pdata->look.hpr[HEADING], pdata->hud_data,
                     G_dynamic_data->last_selected_vid,
                     G_dynamic_data->bounding_box.xmin,
                     G_dynamic_data->bounding_box.xmax,
                     G_dynamic_data->bounding_box.ymin,
                     G_dynamic_data->bounding_box.ymax,
                     (hud_level >= ENABLE_RINGS),
                     (hud_level >= ENABLE_LANDMARKS),
                     (hud_level >= ENABLE_BOUNDARY),
                     (hud_level >= ENABLE_RADAR),
                     (hud_level >= ENABLE_RADAR),
                     G_HUD_options->iconmode,
                     G_HUD_options->colormode,
                     G_HUD_options->rotmode,
                     G_HUD_options->talpha,
                     0.0f, 1.0f, 0.0f, 1.0f );

      static int nearest_vid; 
      if ( (hud_level >= ENABLE_RADAR) &&
           ( (G_dynamic_data->pick_mode == TWO_D_RADAR_PICK) ||
             (G_dynamic_data->pick_mode == TWO_D_RADAR_INFO) ) ) { 
         nearest_vid = 
            select_nearest_vid_to_mouse ( radar_info,
                                          pdata->posture.xyz,
                                          pdata->hud_data,
                                          G_HUD_options->range );
      }
      else {
         nearest_vid = -1;
      }
      G_dynamic_data->nearest_radar_vid_to_mouse = nearest_vid;


      ll[X] = curpos[X] - 0.5f*range*aspect;
      ll[Y] = curpos[Y] - 0.5f*range;
      rr[X] = ll[X] + range*aspect;
      rr[Y] = ll[Y] + range;

#ifdef IRISGL
     ortho2 (ll[X], rr[X], ll[Y], rr[Y]);
#else //OPENGL
     // This is here because OpenGL handles the MODELVIEW & PROJECTION matrixes
     // differently then does IRISGL
     // OpenGL multiplys the matrices using gluOrtho, but GL does not.
     // Therefore must load identity on PROJECTION matrix before gluOrtho
     glGetIntegerv (GL_MATRIX_MODE, &gl_matrix_mode);
     glMatrixMode (GL_PROJECTION);
     glLoadIdentity();
     gluOrtho2D( ll[X], rr[X], ll[Y], rr[Y] );
     pfVec2 v; //vertex for drawing lines
     npsFont(DEFFONT);
#endif

     // Draw the targetting diamond
     if ( (hud_level >= ENABLE_CROSS) && !camera )
        {
        reflength = range*0.05f;
        if (targetting)
           {
           switch (target_lock)
              {
              case BLUE_LOCK :    
#ifdef IRISGL
                 cpack(0x00ff0000|alpha);
                 linewidth(2);
#else //OPENGL
                 glColor4f(0.0f, 0.0f, 1.0f, 0.0f);
                 glLineWidth(2);
#endif
                 break;
              case RED_LOCK  :
#ifdef IRISGL
                 cpack(0x000000ff|alpha);
                 linewidth(2);
#else //OPENGL
                 glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
                 glLineWidth(2);
#endif
                 break;
              case WHITE_LOCK:
#ifdef IRISGL
                 cpack(0x00ffffff|alpha);
                 linewidth(2);
#else //OPENGL
                 glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
                 glLineWidth(2);
#endif
                 break;
              case OTHER_LOCK:
#ifdef IRISGL
                 cpack(0x0000ff00|alpha);
                 linewidth(2);
#else //OPENGL
                 glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
                 glLineWidth(2);
#endif
                 break;
              default:
#ifdef IRISGL
                 cpack(0x000ff0f5|alpha);
                 linewidth(1);
#else //OPENGL
                 glColor4f(0.95f, 0.94f, 0.05f, 0.0f);
                 glLineWidth(1);
#endif
                 break;
              }
           }
        else
           {
#ifdef IRISGL
           cpack(0x000ff0f5|alpha);
           linewidth(1);
#else //OPENGL
           glColor4f(0.95f, 0.94f, 0.05f, 0.0f);
           glLineWidth(1);
#endif
           }
#ifdef IRISGL
        move2(curpos[X] + reflength,curpos[Y]);
        draw2(curpos[X], curpos[Y]+reflength);
        draw2(curpos[X] - reflength, curpos[Y]);
        draw2(curpos[X], curpos[Y]-reflength);
        draw2(curpos[X] + reflength,curpos[Y]);
        draw2(curpos[X] - reflength, curpos[Y]);
        move2(curpos[X], curpos[Y]-reflength);
        draw2(curpos[X], curpos[Y]+reflength);
        linewidth(1);
  
        pushmatrix();
        loadmatrix(identity);
        translate(0.0f, (-pdata->posture.hpr[PITCH]*range/180.0f), 0.0f);
        translate(curpos[X],curpos[Y],0.0f);
        rot(pdata->posture.hpr[ROLL],'z');
        translate(-curpos[X],-curpos[Y],0.0f);
        cpack(0x000000ff|alpha);
        move2(curpos[X] - 3.0f*reflength,curpos[Y]);
        draw2(curpos[X] - 2.0f*reflength,curpos[Y]);
        draw2(curpos[X] - 1.5*reflength,curpos[Y] - 0.5*reflength);
        draw2(curpos[X] - reflength,curpos[Y]);
        draw2(curpos[X] + reflength,curpos[Y]);
        draw2(curpos[X] + 1.5*reflength,curpos[Y] - 0.5*reflength);
        draw2(curpos[X] + 2.0f*reflength,curpos[Y]);
        draw2(curpos[X] + 3.0f*reflength,curpos[Y]);
  
        move2(curpos[X],curpos[Y]- reflength);
        draw2(curpos[X],curpos[Y]+ reflength);
        popmatrix();
#else //OPENGL
        glBegin(GL_LINE_STRIP);
           PFSET_VEC2(v, curpos[X] + reflength, curpos[Y]); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X], curpos[Y]+reflength); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X] - reflength, curpos[Y]); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X], curpos[Y]-reflength); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X] + reflength, curpos[Y]); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X] - reflength, curpos[Y]); glVertex2fv(v);
        glEnd();
        glBegin(GL_LINES);
           PFSET_VEC2(v, curpos[X], curpos[Y]-reflength); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X], curpos[Y]+reflength); glVertex2fv(v);
        glEnd();
        glLineWidth(1);

        pfPushMatrix();

        glTranslatef(0.0f, (-pdata->posture.hpr[PITCH]*range/180.0f), 0.0f);
        glTranslatef(curpos[X],curpos[Y],0.0f);
        glRotatef(pdata->posture.hpr[ROLL],0.0f,0.0f,1.0f);
        glTranslatef(-curpos[X],-curpos[Y],0.0f);
        glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_STRIP);
           PFSET_VEC2(v, curpos[X] - 3.0f*reflength, curpos[Y]); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X] - 2.0f*reflength, curpos[Y]); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X] - 1.5*reflength, curpos[Y] - 0.5*reflength); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X] - reflength, curpos[Y]); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X] + reflength, curpos[Y]); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X] + 1.5*reflength, curpos[Y] - 0.5*reflength); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X] + 2.0f*reflength, curpos[Y]); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X] + 3.0f*reflength, curpos[Y]); glVertex2fv(v);
        glEnd();
        glBegin(GL_LINES);
           PFSET_VEC2(v, curpos[X], curpos[Y]- reflength); glVertex2fv(v);
           PFSET_VEC2(v, curpos[X], curpos[Y]+ reflength); glVertex2fv(v);
        glEnd();

        pfPopMatrix();
#endif
  
        }
 
/****************************************/
     // Draw the cross hairs
/****************************************/
  width = view_right - view_left;
  height = view_top - view_bottom;
#ifdef IRISGL
  ortho2 ( 0.0f, width, 0.0f, height );
#else //OPENGL
  glLoadIdentity();
  gluOrtho2D( 0.0f, width, 0.0f, height );
#endif

  halfX = width / 2.0f;
  halfY = height / 2.0f;

#ifdef BIRD
  if ( G_static_data->bird_exists )
     {
     /* DRAW A 2D MOUSE INDICATOR */
     linewidth(2);
     if ( G_dynamic_data->last_selected_vid == -1 )
        cpack(0x000000ff|alpha);
     else
        cpack(0x0000ffff|alpha);
     float half_cursor_size = (height/1024.0f)*10.0f;
     move2i ( mouseX-originX, mouseY+half_cursor_size-originY );
     draw2i ( mouseX-originX, mouseY-half_cursor_size-originY );
     move2i ( mouseX-half_cursor_size-originX, mouseY-originY );
     draw2i ( mouseX+half_cursor_size-originX, mouseY-originY );
     linewidth(1);
     }
#endif

  if ( (hud_level >= ENABLE_CROSS) && camera )
     {
     if (targetting)
        {
        switch (target_lock)
           {
           case BLUE_LOCK :
#ifdef IRISGL
              cpack(0x00ff0000|alpha);
              linewidth(2);
#else //OPENGL
              glColor4f(0.0f, 0.0f, 1.0f, 0.0f);
              glLineWidth(2);
#endif
              break;
           case RED_LOCK  :
#ifdef IRISGL
              cpack(0x000000ff|alpha);
              linewidth(2);
#else //OPENGL
              glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
              glLineWidth(2);
#endif
              break;
           case WHITE_LOCK:
#ifdef IRISGL
              cpack(0x00ffffff|alpha);
              linewidth(2);
#else //OPENGL
              glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
              glLineWidth(2);
#endif
              break;
           case OTHER_LOCK:
#ifdef IRISGL
              cpack(0x0000ff00|alpha);
              linewidth(2);
#else //OPENGL
              glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
              glLineWidth(2);
#endif
              break;
           default:
#ifdef IRISGL
              cpack(0x000ff0f5|alpha);
              linewidth(1);
#else //OPENGL
              glColor4f(0.95f, 0.94f, 0.5f, 0.0f);
              glLineWidth(1);
#endif
              break;
           }
        }
     else
        {
#ifdef IRISGL
        cpack(0x000ff0f5|alpha);
        linewidth(1);
#else //OPENGL
        glColor4f(0.95f, 0.95f, 0.5f, 0.0f);
        glLineWidth(1);
#endif
        }
#ifdef IRISGL
     move2i((long)halfX, (long)halfY + int(0.1953125*height));
     draw2i((long)halfX, (long)halfY + int(0.048828125*height));
     move2i((long)halfX, (long)halfY - int(0.1953125*height));
     draw2i((long)halfX, (long)halfY - int(0.048828125*height));

     move2i((long)halfX + int(0.1953125*height), (long)halfY);
     draw2i((long)halfX + int(0.048828125*height), (long)halfY);
     move2i((long)halfX - int(0.1953125*height), (long)halfY);
     draw2i((long)halfX - int(0.048828125*height), (long)halfY);
#else //OPENGL
     glBegin(GL_LINES);
        PFSET_VEC2(v, halfX, halfY + (0.1953125*height)); glVertex2fv(v);
        PFSET_VEC2(v, halfX, halfY + (0.048828125*height)); glVertex2fv(v); 
        PFSET_VEC2(v, halfX, halfY - (0.1953125*height)); glVertex2fv(v); 
        PFSET_VEC2(v, halfX, halfY - (0.048828125*height)); glVertex2fv(v); 
        PFSET_VEC2(v, halfX + (0.1953125*height), halfY); glVertex2fv(v); 
        PFSET_VEC2(v, halfX + (0.048828125*height), halfY); glVertex2fv(v); 
        PFSET_VEC2(v, halfX - (0.1953125*height), halfY); glVertex2fv(v); 
        PFSET_VEC2(v, halfX - (0.048828125*height), halfY); glVertex2fv(v); 
     glEnd();
#endif
     }

/****************************************/
     // Draw the compass 
/****************************************/
   if (hud_level >= ENABLE_CROSS)
     {
     compsize = int(0.073242188*height);
     completter = compsize + (0.013 * height);

#ifdef IRISGL
     pushmatrix();
     translate ( completter+12.0f, completter+12.0f, 0.0f );

     linewidth(2);

     RGBcolor(255,0,0);
     move2i(0,0);
     draw2i(0, compsize+20);
     cmov2i(-10,compsize+20);
     tempdir = real_heading;
     sprintf(text, "%3.0f",tempdir);
     charstr(text);

     RGBcolor(255,255,255);
     circi(0,0,compsize);
     /*figure out where to put the letters*/
     tempdir = real_heading * DEG_TO_RAD - HALFPI;
     cmov2((fcos(tempdir)*completter),(fsin(tempdir)*completter));
     charstr("S");
     tempdir = tempdir + HALFPI;
     cmov2((fcos(tempdir)*completter),(fsin(tempdir)*completter));
     charstr("E");
     tempdir = tempdir + HALFPI;
     cmov2((fcos(tempdir)*completter),(fsin(tempdir)*completter));
     charstr("N");
     tempdir = tempdir + HALFPI;
     cmov2((fcos(tempdir)*completter),(fsin(tempdir)*completter));
     charstr("W");

     /*now the lookangle*/
     RGBcolor(255,0,255);
     tempdir = (pdata->look.hpr[HEADING]*DEG_TO_RAD) + HALFPI;
     move2i(0, 0);
     draw2((fcos(tempdir)*compsize),(fsin(tempdir)*compsize));

     /*don't forget the field of view*/
     RGBcolor(0,255,255);
     tempdir += fieldofview* 0.5 *DEG_TO_RAD;
     move2i(0, 0);
     draw2((fcos(tempdir)*compsize),(fsin(tempdir)*compsize));
     tempdir -= fieldofview * DEG_TO_RAD;
     move2i(0, 0);
     draw2((fcos(tempdir)*compsize),(fsin(tempdir)*compsize));
     /*end of compass*/

     linewidth(1);

     popmatrix();
#else //OPENGL
     pfPushMatrix();
     glTranslatef ( completter+12.0f, completter+12.0f, 0.0f );
     glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
     glLineWidth(2);
     glBegin(GL_LINES);
        PFSET_VEC2(v, 0.0f, 0.0f); glVertex2fv(v);
        PFSET_VEC2(v, 0.0f, (float)(compsize+20)); glVertex2fv(v);
     glEnd();

     tempdir = real_heading;
     sprintf(text, "%3.0f",tempdir);
     npsDrawStringPos(text,-10.0f,(float)(compsize+20),0.0f);

     glColor4f(1.0f, 1.0f, 1.0f, 0.0f);

     // Draw The circle
     {
     GLUquadricObj *qobj = gluNewQuadric();
     gluDisk(qobj,(float)compsize-1.0f,(float)compsize,100,1);
     gluDeleteQuadric(qobj);
     }

     /*figure out where to put the letters*/
     tempdir = real_heading * DEG_TO_RAD - HALFPI;
     npsDrawStringPos("S",(fcos(tempdir)*completter),(fsin(tempdir)*completter),0.0f);
     tempdir = tempdir + HALFPI;
     npsDrawStringPos("E",(fcos(tempdir)*completter),(fsin(tempdir)*completter),0.0f);
     tempdir = tempdir + HALFPI;
     npsDrawStringPos("N",(fcos(tempdir)*completter),(fsin(tempdir)*completter),0.0f);
     tempdir = tempdir + HALFPI;
     npsDrawStringPos("W",(fcos(tempdir)*completter),(fsin(tempdir)*completter),0.0f);

     //Look Heading
     glColor4f(1.0f, 0.0f, 1.0f, 0.0f);
     tempdir = (pdata->look.hpr[HEADING]*DEG_TO_RAD) + HALFPI;
     glBegin(GL_LINES);
        PFSET_VEC2(v, 0.0f, 0.0f); glVertex2fv(v);
        PFSET_VEC2(v, (fcos(tempdir)*compsize),(fsin(tempdir)*compsize)); 
        glVertex2fv(v);
     glEnd();

     //Field of View
     glColor4f(0.0f, 1.0f, 1.0f, 0.0f);
     tempdir += fieldofview* 0.5 *DEG_TO_RAD;
     glBegin(GL_LINES);
        PFSET_VEC2(v, 0.0f, 0.0f); glVertex2fv(v);
        PFSET_VEC2(v, (fcos(tempdir)*compsize),(fsin(tempdir)*compsize)); 
        glVertex2fv(v);
     glEnd();
     tempdir -= fieldofview * DEG_TO_RAD;
     glBegin(GL_LINES);
        PFSET_VEC2(v, 0.0f, 0.0f); glVertex2fv(v);
        PFSET_VEC2(v, (fcos(tempdir)*compsize),(fsin(tempdir)*compsize)); 
        glVertex2fv(v);
     glEnd();

     glLineWidth(1);
     pfPopMatrix();
#endif

/****************************************/
     // Draw the wing's level indicator 
/****************************************/

     original_trans_mode = pfGetTransparency();
     pfOverride(PFSTATE_TRANSPARENCY, PF_OFF );
     pfTransparency(PFTR_ON);
     pfOverride(PFSTATE_TRANSPARENCY, PF_ON );

#ifdef IRISGL
     pushmatrix();
     translate ( width - completter - 12.0f, completter + 12.0f, 0.0f );
     rot(pdata->posture.hpr[ROLL],'z');
     cpack(0x6fffb400);
     arcf(0,0,compsize,0,1800);
     cpack(0x6f004b90);
     arcf(0,0,compsize,1800,3600); 
     rot(-pdata->posture.hpr[ROLL],'z');
     RGBcolor(0,0,0);
     circi(0,0,compsize);
     linewidth(2);
     RGBcolor(255,0,0);
     move2i(-compsize,0);
     draw2(-compsize/2.0f,0);
     draw2(-compsize/4.0f,-0.2f*compsize);
     draw2(0,0);
     draw2(compsize/4.0f,-0.2f*compsize);
     draw2(compsize/2.0f,0);
     draw2(compsize,0);
     linewidth(1);
     popmatrix();
#else //OPENGL
     GLUquadricObj *qobj = gluNewQuadric();
     pfPushMatrix();
     glTranslatef ( width - completter - 12.0f, completter + 12.0f, 0.0f );
     glRotatef(pdata->posture.hpr[ROLL],0.0f,0.0f,1.0f);

     // Draw the top half of horizon circle
     glColor4f(0.0f, 0.7f, 1.0f, 0.43f);
     gluPartialDisk(qobj,0.0f,(float)compsize,100,1,270.0f,180.0f);

     // Draw the bottom half of horizon circle
     glColor4f(0.56f, 0.29f, 0.0f, 0.43f);
     gluPartialDisk(qobj,0.0f,(float)compsize,100,1,90.0f,180.0f);

     // Need to reset matrix back to initial conditions for next draw
     pfPopMatrix();
     pfPushMatrix();
     glTranslatef ( width - completter - 12.0f, completter + 12.0f, 0.0f );
     //glRotatef(-pdata->posture.hpr[ROLL],0.0f,0.0f,1.0f);
     // Draw the circle outlining indicator 
     glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
     gluDisk(qobj,(float)compsize-1.0f,(float)compsize,100,1);

     glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
     glLineWidth(2);
     glBegin(GL_LINE_STRIP);
        PFSET_VEC2(v, (float)-compsize,0.0f); glVertex2fv(v);
        PFSET_VEC2(v, -compsize/2.0f,0.0f); glVertex2fv(v);
        PFSET_VEC2(v, -compsize/4.0f,-0.2f*compsize); glVertex2fv(v);
        PFSET_VEC2(v, 0.0f,0.0f); glVertex2fv(v);
        PFSET_VEC2(v, compsize/4.0f,-0.2f*compsize); glVertex2fv(v);
        PFSET_VEC2(v, compsize/2.0f,0.0f); glVertex2fv(v);
        PFSET_VEC2(v, (float)compsize,0.0f); glVertex2fv(v);
     glEnd();

     glLineWidth(1);
     pfPopMatrix();
     gluDeleteQuadric(qobj);
#endif

     pfOverride(PFSTATE_TRANSPARENCY, PF_OFF );
     pfTransparency(original_trans_mode);
     pfOverride(PFSTATE_TRANSPARENCY, PF_ON );

/****************************************/
     // Draw the thrust indicator
/****************************************/
#ifdef IRISGL
     linewidth(2);
     RGBcolor(255,255,0);
#else //OPENGL
     glColor4f(1.0f, 1.0f, 0.0f, 0.0f);
     glLineWidth(2);
#endif
     startX = halfX - halfX/2.0f;
     stopX = halfX + halfX/2.0f;
     step = halfX / 10.0f;
     ypos = completter + 12.0f;
#ifdef IRISGL
     move2 ( startX, ypos );
     draw2 ( stopX, ypos );
     for(xpos = startX; xpos <= stopX; xpos += step) {
        move2(xpos,ypos);
        draw2(xpos,ypos-(0.1f*compsize));
     }
     RGBcolor(255,0,0);
     xpos = startX + (((pdata->thrust + 1.0f)/2.0f)*halfX);
     move2 ( xpos, ypos-(0.1f*compsize) );
     draw2 ( xpos, ypos+(0.1f*compsize) );
     cmov2 ( xpos, ypos+(0.2f*compsize) );
     sprintf(text,"%d%%", int(pdata->thrust*100.0f));
     charstr(text);
     cmov2 ( startX, ypos - ((getheight()*height)/512.0f) );
     charstr("-100%");
     cmov2 ( stopX, ypos - ((getheight()*height)/512.0f) );
     charstr("100%");
     cmov2 ( halfX, ypos - ((getheight()*height)/512.0f) );
     charstr("0");
     linewidth(1);
#else //OPENGL
     glBegin(GL_LINES);
        PFSET_VEC2(v, startX, ypos); glVertex2fv(v);
        PFSET_VEC2(v, stopX, ypos); glVertex2fv(v);
     glEnd();
     for(xpos = startX; xpos <= stopX; xpos += step) {
        glBegin(GL_LINES);
           PFSET_VEC2(v, xpos,ypos); glVertex2fv(v);
           PFSET_VEC2(v, xpos,ypos-(0.1f*compsize)); glVertex2fv(v);
        glEnd();
     }
     glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
     xpos = startX + (((pdata->thrust + 1.0f)/2.0f)*halfX);
     glBegin(GL_LINES);
        PFSET_VEC2(v, xpos, ypos-(0.1f*compsize)); glVertex2fv(v);
        PFSET_VEC2(v, xpos, ypos+(0.1f*compsize)); glVertex2fv(v);
     glEnd();
     sprintf(text,"%d%%", int(pdata->thrust*100.0f));
     npsDrawStringPos(text,xpos,ypos+(0.2f*compsize),0.0f);

     pfuXFont *fontdead;
     fontdead=npsGetCurXFont();
     int fontHeight = npsGetXFontHeight(fontdead);
     npsDrawStringPos("-100%",startX,ypos-((fontHeight*height)/512.0f),0.0f);
     npsDrawStringPos("100%",stopX,ypos-((fontHeight*height)/512.0f),0.0f);
     npsDrawStringPos("0",halfX,ypos-((fontHeight*height)/512.0f),0.0f);

     glLineWidth(1);
#endif
     }

/****************************************/
     // Draw the speed indicator
/****************************************/
  base = (0.333 * height);
  top = (0.666 * height);

  if (hud_level >= ENABLE_CROSS)
     {

     if ( metric )
        {
        speed = (pdata->speed * MPS_TO_KPH);
        }
     else
        {
        speed = pdata->speed * MPS_TO_MPH;
        }

#define STOPPED 0.0
#define SLOW 150.0
#define FAST 1000.0
#define VERYFAST 2000.0
#define TOOFAST 10000.0


#ifdef IRISGL
     RGBcolor(255,255,0);
#else //OPENGL
     glColor4f(1.0f, 1.0f, 0.0f, 0.0f);
#endif
     step = (top - base) / 10.0;
     for(ypos = base;ypos<= top; ypos += step){
#ifdef IRISGL
       move2((0.058594*width),ypos);
       draw2((0.078125*width),ypos);
#else //OPENGL
     glBegin(GL_LINES);
        PFSET_VEC2(v, (0.058594*width),ypos); glVertex2fv(v);
        PFSET_VEC2(v, (0.078125*width),ypos); glVertex2fv(v);
     glEnd();
#endif
     }
     ypos += step;
#ifdef IRISGL
     cmov2 ( (0.068359375*width)-20.0f,ypos);
     charstr("Speed");
#else //OPENGL
     npsDrawStringPos("Speed",(0.068359375*width)-20.0f,ypos,0.0f);
#endif

     /* Your speed gauge*/
      if(speed < SLOW){
        mytop = SLOW;
      }
      else{ if(speed <FAST){
        mytop = FAST;
      }
      else{ if(speed <VERYFAST){
        mytop = VERYFAST;
      }
      else{
        mytop = TOOFAST;
      }}}

#ifdef SPEEDBUFFER

// MAX_SPEED_SAMPLE defines how many speed samples are kept.
// for example 10 means that it keeps ten previous speed samples
// I did not have any way to tune this so you will have to do it your
// self.  -rb

#define MAX_SPEED_SAMPLE 10

     static int thePlace = -1;
     static float speedBuffer[MAX_SPEED_SAMPLE];
     static int lastUnitsType = FALSE;

     if (lastUnitsType != metric) {
        thePlace = -1;
        lastUnitsType = metric;
     }
 
     float averageSpeed = 0.0f;

     if (thePlace == -1) {
        for (int ix = 0; ix < MAX_SPEED_SAMPLE; ix++) {
           speedBuffer[ix] = speed;
        }
        thePlace = 0;
     }
     else {
        speedBuffer[thePlace] = speed;
        thePlace = (thePlace + 1) % MAX_SPEED_SAMPLE;
     } 

     
     for (int ix = 0; ix < MAX_SPEED_SAMPLE; ix++) {
        averageSpeed += speedBuffer[ix];
     }

     averageSpeed /= MAX_SPEED_SAMPLE;
#endif // SPEEDBUFFER

  
     if ( width > 700.0f )
        {
#ifdef IRISGL
        cmov2((0.058594*width)-48.0f,base);
        sprintf(text,"%4.0f",STOPPED);
        charstr(text);
        cmov2((0.058594*width)-60.0f,top);
        sprintf(text,"%4.0f",mytop);
        charstr(text);
#else //OPENGL
        sprintf(text,"%4.0f",STOPPED);
        npsDrawStringPos(text,(0.058594*width)-48.0f,base,0.0f);
        sprintf(text,"%4.0f",mytop);
        npsDrawStringPos(text,(0.058594*width)-60.0f,top,0.0f);
#endif
        }
  
     /*figure out where out where to put the writing*/
#ifdef IRISGL
     RGBcolor(255,0,0);
#else //OPENGL
     glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
#endif
     if(speed < 0.0)
       ypos = base;
     else
       ypos = base + (speed/mytop)*(top-base);
     if(ypos > top) ypos = top;
#ifdef IRISGL
     move2((0.058594*width),ypos);
     draw2((0.078125*width),ypos);
     cmov2((0.058594*width),ypos+(0.00293*height));
#else //OPENGL
     glBegin(GL_LINES);
        PFSET_VEC2(v, (0.058594*width),ypos); glVertex2fv(v);
        PFSET_VEC2(v, (0.078125*width),ypos); glVertex2fv(v);
     glEnd();
#endif

#ifdef SPEEDBUFFER
// change here to get even more decimal places. right now %.2f
// mean two decimal places to the right of the decimal place.
// if you want say 5 then you would change it to %.5f
// -rb
     sprintf(text,"%.2f",averageSpeed);
#else
     sprintf(text,"%.0f",speed);
#endif // SPEEDBUFFER
     if ( metric )
        strcat ( text, "kph" );
     else
        strcat ( text, "mph" );
#ifdef IRISGL
     charstr(text);
#else //OPENGL
     npsDrawStringPos(text,(0.058594*width),ypos+(0.00293*height),0.0f);
#endif

     }

/****************************************/
  // Draw the altitute indicator
/****************************************/

#define A_MSL 0.0
#define A_VERYLOW 1000.0
#define A_LOW 5000.0
#define A_MEDIUM 10000.0
#define A_HIGH  50000.0
  if (hud_level >= ENABLE_CROSS)
     {
#ifdef IRISGL
     RGBcolor(255,255,0);
#else //OPENGL
     glColor4f(1.0f, 1.0f, 0.0f, 0.0f);
#endif
     step = (top - base) / 10.0f;
     for(ypos = base;ypos<= top; ypos += step){
#ifdef IRISGL
       move2((0.91796875*width),ypos);
       draw2((0.9375*width),ypos);
#else //OPENGL
     glBegin(GL_LINES);
        PFSET_VEC2(v, (0.91796875*width),ypos); glVertex2fv(v);
        PFSET_VEC2(v, (0.9375*width),ypos); glVertex2fv(v);
     glEnd();
#endif
     }
     ypos += step;
#ifdef IRISGL
     cmov2((0.92773438*width)-36.0f,ypos);
     charstr("Altitude");
#else //OPENGL
     npsDrawStringPos("Altitude",(0.92773438*width)-36.0f,ypos,0.0f);
#endif
 
     if ( metric )
        {
        alt = pdata->altitude;
        }
     else
        {
        alt = (pdata->altitude * METER_TO_FOOT);
        }
  
      if(alt < A_VERYLOW){
        mytop = A_VERYLOW;
      }
      else{ if(alt < A_LOW){
        mytop = A_LOW;
      }
      else{ if(alt < A_MEDIUM){
        mytop = A_MEDIUM;
      }
      else{
        mytop = A_HIGH;
      }}}
  
     if ( width > 700.0f )
        {
#ifdef IRISGL
        cmov2((0.9375*width)+12.0f,base);
        sprintf(text,"%.0f",STOPPED);
        charstr(text);
        cmov2((0.9375*width)+12.0f,top);
        sprintf(text,"%.0f",mytop);
        charstr(text);
#else //OPENGL
        sprintf(text,"%.0f",STOPPED);
        npsDrawStringPos(text,(0.9375*width)+12.0f,base,0.0f);
        sprintf(text,"%.0f",mytop);
        npsDrawStringPos(text,(0.9375*width)+12.0f,top,0.0f);
#endif
        }
  
     /*figure out where out where to put the writing*/
#ifdef IRISGL
     RGBcolor(255,0,0);
#else //OPENGL
     glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
#endif
     if(alt < 0.0)
       ypos = base;
     else
       ypos = base + (alt/mytop)*(top-base);
     if(ypos > top) ypos = top;
#ifdef IRISGL
     move2((0.91796875*width),ypos);
     draw2((0.9375*width),ypos);
     cmov2((0.9375*width)-60.0f,ypos+(0.00293*height));
#else //OPENGL
     glBegin(GL_LINES);
        PFSET_VEC2(v, (0.91796875*width),ypos); glVertex2fv(v);
        PFSET_VEC2(v, (0.9375*width),ypos); glVertex2fv(v);
     glEnd();
#endif
     sprintf(text,"%5.0f",alt);
     if ( metric )
        strcat ( text, "m" );
     else
        strcat ( text, "ft" );
#ifdef IRISGL
     charstr(text);
#else //OPENGL
     npsDrawStringPos(text,(0.9375*width)-60.0f,ypos+(0.00293*height),0.0f);
#endif
     }

#if 0
/****************************************/
//NEW
  /*now let's do thrust for the physically based helo only*/
#define MAX_HELO_THRUST 100.0f
#define MIN_HELO_THRUST 0.0f
  if (hud_level >= ENABLE_CROSS)
     {
     RGBcolor(255,255,0);

     // Set the distance between tickmarks
     step = (top - base) / 10.0f;

     // Build the tickmarks on the thrust scale
     for(ypos = base;ypos<= (top+1); ypos += step){
       move2((0.81796875*width),ypos);
       draw2((0.8375*width),ypos);
     }

     // Put the %Thrust label above the scale
     ypos += step;
     cmov2((0.82773438*width)-36.0f,ypos);
     charstr("% Thrust");
 
     // Turn raw thrust data into a percentage
     thrust = 100.0f * pdata->thrust;

     // This is the max value on the scale
     mytop = MAX_HELO_THRUST;

     if ( width > 700.0f )
        {
        cmov2((0.8375*width)+12.0f,base);
        sprintf(text,"%.0f",STOPPED);
        charstr(text);
        cmov2((0.8375*width)+12.0f,top);
        sprintf(text,"%.0f",mytop);
        charstr(text);
        }
  
     /*figure out where out where to put the writing*/
     RGBcolor(255,0,0);
     if(thrust < 0.0)
       ypos = base;
     else
       ypos = base + (thrust/mytop)*(top-base);
     if(ypos > top) ypos = top;
     move2((0.81796875*width),ypos);
     draw2((0.8375*width),ypos);
     cmov2((0.8375*width)-60.0f,ypos+(0.00293*height));
     sprintf(text,"%5.1f",thrust);
     strcat ( text, "%" );
     charstr(text);
     }
/****************************************/
#endif

/****************************************/
   // Draw View Type in Upper left hand corner
/****************************************/
#ifdef IRISGL  
     max_font_height = getheight();
#else //OPENGL
     display_font = npsGetCurXFont();
     max_font_height = npsGetXFontHeight(display_font); 
#endif

     max_font_width = long(D_FONT_WIDTH);

     ll[X] = 0.0f;
     ll[Y] = 0.0f;
     rr[X] = (float(width)/float(max_font_width));
     if (full_screen && stereo)
        rr[Y] = (492.0/float(max_font_height));
     else
        rr[Y] = (float(height)/float(max_font_height));

#ifdef IRISGL  
     ortho2 ( ll[X], rr[X], ll[Y], rr[Y] );

     if ( color == COLOR1 )
        RGBcolor ( 127, 255, 212 );
     else if ( color == COLOR2 )
        RGBcolor ( 255, 0, 0 );
     else if ( color == COLOR3 )
        RGBcolor ( 0, 255, 0 );
     else if ( color == COLOR4 )
        RGBcolor ( 255, 0, 255 );
     else
        RGBcolor ( 255, 255, 255 );
#else //OPENGL
     glLoadIdentity();
     gluOrtho2D( ll[X], rr[X], ll[Y], rr[Y] );
     
     if ( color == COLOR1 )
        glColor4f(0.49f, 1.0f, 0.82f, 0.0f);
     else if ( color == COLOR2 )
        glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
     else if ( color == COLOR3 )
        glColor4f(0.0f, 1.0f, 0.0f, 0.0f);
     else if ( color == COLOR4 )
        glColor4f(1.0f, 0.0f, 1.0f, 0.0f);
     else
        glColor4f(1.0f, 1.0f, 1.0f, 0.0f);
#endif

#if 0
     switch ( pdata->eyeview )
        {
        case FORWARD_VIEW:
           strcpy ( text, "Forward View" );
           break;
        case CAMERA_VIEW:
           if ( G_dynamic_data->camlock )
              strcpy ( text, "TRACKING View" );
           else
              strcpy ( text, "Camera View" );
           break;
        case MUNITION_VIEW:
           strcpy ( text, "Munition View" );
           break;
        case WEAPON_VIEW:
           strcpy ( text, "Weapon View" );
           break;
        case VEHICLE_VIEW:
           strcpy ( text, "Alternate Vehicle View" );
           break;
        case BIRDSEYE_VIEW:
           strcpy ( text, "Bird's Eye View" );
           break;
        default:
           strcpy ( text, "Undocumented View" );
           break;
        }
     start_txt = ((ll[X]+rr[X]-strlen(text))/2.0f);
     cmov2 ( start_txt, rr[Y] - 4.0f );
     charstr(text);
#endif
  
/****************************************/
   // Draw Game Statistics 
/****************************************/
     if ( veh_info )
        {
        line = rr[Y];
        line -= inc;
#ifdef IRISGL
        cmov2 ( ll[X] + 2.0f, line );
#endif
        sprintf ( text, VERSION );
        if ( G_static_data->stealth )
           strcat ( text, " STEALTH" );
        if ( G_dynamic_data->net_mode )
           strcat ( text, " MULTICAST" );
#ifdef DEMO
        strcat ( text, " DEMO" );
#endif
#ifdef IRISGL
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,ll[X] + 2.0f,line,0.0f);
#endif


        if ( visual_mode )
           {
           line -= inc;
#ifdef IRISGL
           cmov2 ( ll[X] + 2.0f, line );
           charstr ( "Video Missle Engaged");
#else //OPENGL
           npsDrawStringPos("Video Missle Engaged",ll[X] + 2.0f,line,0.0f);
#endif
           }

        if ( G_static_data->scoring )
           {
           line -= inc;
#ifdef IRISGL
           cmov2 ( ll[X] + 2.0f, line );
           sprintf ( text, "Still hits:  %d", static_hits );
           charstr ( text );
           line -= inc;
           cmov2 ( ll[X] + 2.0f, line );
           sprintf ( text, "Moving hits: %d", dynamic_hits );
           charstr ( text );
           line -= inc;
           cmov2 ( ll[X] + 2.0f, line );
           sprintf ( text, "Bullets fired:  %d", bullets_fired);
           charstr ( text );
           line -= inc;
           cmov2 ( ll[X] + 2.0f, line );
           sprintf ( text, "Missiles fired: %d", missiles_fired);
           charstr ( text );
           line -= inc;
           cmov2 ( ll[X] + 2.0f, line );
           sprintf ( text, "Bombs dropped:  %d", bombs_fired);
           charstr ( text );
#else //OPENGL
           sprintf ( text, "Still hits:  %d", static_hits );
           npsDrawStringPos(text,ll[X] + 2.0f,line,0.0f);
           line -= inc;
           sprintf ( text, "Moving hits: %d", dynamic_hits );
           npsDrawStringPos(text,ll[X] + 2.0f,line,0.0f);
           line -= inc;
           sprintf ( text, "Bullets fired:  %d", bullets_fired);
           npsDrawStringPos(text,ll[X] + 2.0f,line,0.0f);
           line -= inc;
           sprintf ( text, "Missiles fired: %d", missiles_fired);
           npsDrawStringPos(text,ll[X] + 2.0f,line,0.0f);
           line -= inc;
           sprintf ( text, "Bombs dropped:  %d", bombs_fired);
           npsDrawStringPos(text,ll[X] + 2.0f,line,0.0f);
#endif
           }

        }
  
/****************************************/
   // Draw Driven Vehicle Icon 
/****************************************/
      if ( misc_info )
        {
#ifdef IRISGL  
        font(1);
#else //OPENGL
        npsFont(ICONFONT);
#endif
        found = FALSE;
        i = 0;
        while ( !found && (i < G_static_data->num_radar_divs) )
           {
           if ( (pdata->type >= G_static_data->radar[i].min) &&
                (pdata->type <= G_static_data->radar[i].max) )
              {
              found = TRUE;
              sprintf(text,"%c",G_static_data->radar[i].icon_char);
              }
           else
              i++;
           }
        if (!found )
           {
           if ( G_static_data->stealth )
              strcpy (text, "STEALTH");
           else
              sprintf(text,"%c",D_ICON_CHAR);
           }

//        if(isgroundveh(pdata->type))
//           sprintf(text,"%c",'E');
//        else if(isseaveh(pdata->type))
//           sprintf(text,"%c",'B');
//        else if(isairveh(pdata->type))
//           sprintf(text,"%c",'A');
//        else if(!isalive(pdata->type))
//           sprintf(text,"%c",'/');
//        else
//           sprintf(text,"%c",'?');
        line = rr[Y];
        line -= inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 7.0f, line );
#else //OPENGL
        // Add more space to top of screen for icon fonts
        //line -= inc;
#endif
        if ( found )
           {
#ifdef IRISGL
           charstr(text);
           font(0);
#else //OPENGL
           npsDrawStringPos(text,rr[X] - 7.0f,line,0.0f);
           npsFont(DEFFONT);
#endif
           }
        else if ( G_static_data->stealth )
           {
#ifdef IRISGL
           font(DEFFONT);
           charstr(text);
#else //OPENGL
           npsFont(DEFFONT);
           npsDrawStringPos(text,rr[X] - 7.0f,line,0.0f);
#endif
           }
        else {
#ifdef IRISGL
           font(0);
#else //OPENGL
           npsFont(DEFFONT);
#endif
        }

/****************************************/
    // Draw Position Information on right side of screen
/****************************************/
        line -= 2.0f * inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 9.0f, line );
        charstr("POSITION");
#else //OPENGL
        npsDrawStringPos("POSITION",rr[X] - 9.0f,line,0.0f);
#endif
        sprintf ( text, "%9.0f X", G_dynamic_data->TDBoffsets[X] + 
                                      pdata->posture.xyz[X] );
        line -= inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 12.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 12.0f,line,0.0f);
#endif
        sprintf ( text, "%9.0f Y", G_dynamic_data->TDBoffsets[Y] +
                                      pdata->posture.xyz[Y] );
        line -= inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 12.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 12.0f,line,0.0f);
#endif
        sprintf ( text, "%9.0f Z", pdata->posture.xyz[Z] );
        line -= inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 12.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 12.0f,line,0.0f);
#endif

        coord_converter->nps_to_utm ( 
           G_dynamic_data->TDBoffsets[X] + pdata->posture.xyz[X],
           G_dynamic_data->TDBoffsets[Y] + pdata->posture.xyz[Y],
           easting, northing );

        sprintf ( text, "%9.0f UTM-N", northing );
        line -= inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 16.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 16.0f,line,0.0f);
#endif

        sprintf ( text, "%9.0f UTM-E", easting );
        line -= inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 16.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 16.0f,line,0.0f);
#endif

 
/*
        line -= 1.5f * inc;
        cmov2 ( rr[X] - 12.0f, line );
        charstr("ORIENTATION");
        sprintf ( text, "%9.0f H", real_heading );
        line -= inc;
        cmov2 ( rr[X] - 12.0f, line );
        charstr(text);
        sprintf ( text, "%9.0f P", pdata->posture.hpr[PITCH] );
        line -= inc;
        cmov2 ( rr[X] - 12.0f, line );
        charstr(text);
        sprintf ( text, "%9.0f R", pdata->posture.hpr[ROLL] );
        line -= inc;
        cmov2 ( rr[X] - 12.0f, line );
        charstr(text);
  
        if (!IS_ZERO(pdata->look.hpr[HEADING]) ||
            !IS_ZERO(pdata->look.hpr[PITCH]) ||
            !IS_ZERO(pdata->look.hpr[ROLL]) ||
            !IS_ZERO(pdata->look.xyz[X]) ||
            !IS_ZERO(pdata->look.xyz[Y]) ||
            !IS_ZERO(pdata->look.xyz[Z]) )
           {
           line -= 1.5f * inc;
           cmov2 ( rr[X] - 8.0f, line );
           charstr("OFFSETS");
           sprintf ( text, "%9.0f X", pdata->look.xyz[X] );
           line -= inc;
           cmov2 ( rr[X] - 12.0f, line );
           charstr(text);
           sprintf ( text, "%9.0f Y", pdata->look.xyz[Y] );
           line -= inc;
           cmov2 ( rr[X] - 12.0f, line );
           charstr(text);
           sprintf ( text, "%9.0f Z", pdata->look.xyz[Z] );
           line -= inc;
           cmov2 ( rr[X] - 12.0f, line );
           charstr(text);
           sprintf ( text, "%9.0f H", pdata->look.hpr[HEADING] );
           line -= inc;
           cmov2 ( rr[X] - 12.0f, line );
           charstr(text);
           sprintf ( text, "%9.0f P", pdata->look.hpr[PITCH] );
           line -= inc;
           cmov2 ( rr[X] - 12.0f, line );
           charstr(text);
           }

*/
        }

/****************************************/
    // Draw Misc Information on right side of screen
/****************************************/
      if ( misc_info )
        {

        line = ll[Y] + 1.0f;
        switch ( G_dynamic_data->pick_mode )
           {
           case TWO_D_RADAR_INFO:
              sprintf(text,"RADAR INFO");
              break;
           case TWO_D_RADAR_PICK:
              sprintf(text,"RADAR SLCT");
              break;
           case PO_PICK:
              sprintf(text,"P O   PICK");
              break;
#ifndef NOAHA
           case TWO_D_AHA_INFO:
              sprintf(text,"2dAHA INFO");
              break;
           case TWO_D_AHA_PICK:
              sprintf(text,"2dAHA SLCT");
              break;
           case THREE_D_AHA_INFO:
              sprintf(text,"3dAHA INFO");
              break;
           case THREE_D_AHA_PICK:
              sprintf(text,"3dAHA SLCT");
              break;
#endif
#ifndef NONPSSHIP
           case SHIP_PICK:
              sprintf(text,"SHIP  PICK");
              break;
#endif
           default:
              sprintf(text, "ERROR SLCT");
              break;
           }
#ifdef IRISGL
        cmov2(rr[X] - 14.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 14.0f,line,0.0f);
#endif

        line += inc;
        sprintf(text, "Spec:  %d", G_dynamic_data->specific);
#ifdef IRISGL
        cmov2(rr[X] - 14.0f, line);
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 14.0f,line,0.0f);
#endif

        line += inc;
        if ( G_dynamic_data->exercise > 0 )
           sprintf(text, "Exer:  %d", G_dynamic_data->exercise);
        else
           sprintf(text, "Exer:  1*");
#ifdef IRISGL
        cmov2(rr[X] - 14.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 14.0f,line,0.0f);
#endif

        if (control == NPS_KAFLIGHT)
           sprintf(text, " IN:  KAF%d",control_num);
        else if (control == NPS_FCS)
           sprintf(text, " IN:  FCS%d",control_num);
        else if (control == NPS_SPACEBALL)
           sprintf(text," IN:  SPB%d",control_num);
        else if (control == NPS_KEYBOARD)
           sprintf(text," IN:  KBD%d",control_num);
#ifndef NOBASICPANEL
        else if (control == NPS_BASIC_PANEL)
           sprintf(text," IN:  RMT%d",control_num);
#endif
        else if (control == NPS_SHIP_PANEL)
           sprintf(text," IN:  SHP%d",control_num);
        else if (control == NPS_SUB_PANEL)
           sprintf(text," IN:  SUP%d",control_num);
        else if (control == NPS_FLYBOX)
           sprintf(text," IN:  FBX%d",control_num);
        else if (control == NPS_LEATHER_FLYBOX)
           sprintf(text," IN:  LFB%d",control_num);
#ifndef NO_IPORT
        else if (control == NPS_TREADPORT)
           sprintf(text," IN:  TRD%d",control_num);
        else if (control == NPS_UNIPORT)
           sprintf(text," IN:  UNI%d",control_num);
        else if (control == NPS_TESTPORT)
           sprintf(text," IN:  TST%d",control_num);
#endif // NO_IPORT
#ifndef NOUPPERBODY
        else if (control == NPS_UPPERBODY)
           sprintf(text," IN:  UBS%d",control_num);
#endif // NOUPPERBODY
        else if (control == NPS_FAKESPACE)
           sprintf(text," IN:  FKS%d",control_num);
        else if (control == NPS_ODT)
           sprintf(text," IN:  ODT%d",control_num);
        else if (control == NPS_LWS)
           sprintf(text," IN:  LWS%d",control_num);
        else
           sprintf(text," *ERROR*");
        line += inc;
#ifdef IRISGL
        cmov2(rr[X] - 13.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 13.0f,line,0.0f);
#endif

        line += inc;
        if ( G_static_data->covert )
           {
           sprintf(text, " rf:%5.0f m", pdata->rng_fltr );
           }
        else
           {
           sprintf(text, " RF:%5.0f m", pdata->rng_fltr );
           }
#ifdef IRISGL
        cmov2 ( rr[X] - 13.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 13.0f,line,0.0f);
#endif

        line += inc;
        if ( G_static_data->cpu > 0 )
           {
           sprintf(text, " CPU: LOCK");
           }
        else if ( G_static_data->cpu < 0 )
           {
           sprintf(text, " CPU:  ERR" );
           }
        else
           {
           sprintf(text, " CPU: UNLK" );
           }
#ifdef IRISGL
        cmov2 ( rr[X] - 14.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 14.0f,line,0.0f);
#endif

        if ( aamode ==  PFAA_ON )
           {
           if ( max_samples > 0 )
              {
              if ( num_samples == 0 )
                 sprintf ( text, " AA:  MS0" );
              else if ( num_samples == 4 )
                 sprintf ( text, " AA:  MS4" );
              else if ( num_samples == 8 )
                 sprintf ( text, " AA:  MS8" );
              else if ( num_samples == 16 )
                 sprintf ( text, " AA: MS16" );
              }
           else
              sprintf ( text, " AA:   On" );
           }
        else
           sprintf ( text, " AA:  Off" );
        line += inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 13.0f, line );
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 13.0f,line,0.0f);
#endif


        line += inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 13.0f, line );
        sprintf ( text, "TRN:%5d", int(talpha) );
        charstr(text);
#else //OPENGL
        sprintf ( text, "TRN:%5d", int(talpha) );
        npsDrawStringPos(text,rr[X] - 13.0f,line,0.0f);
#endif
  
        if ( stereo )
           {
           if (metric )
              sprintf (text, "EYE:%5.2f cm", eyesep * 100.0f );
           else
              sprintf (text, "EYE:%5.2f in", eyesep * METER_TO_INCH );
           line += inc;
#ifdef IRISGL
           cmov2 ( rr[X] - 13.0f, line );
           charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 13.0f,line,0.0f);
#endif
           }
  
        sprintf (text, "FoV:%5.0f deg", fovx );
        line += inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 13.0f, line );
        charstr(text);
        line += inc;
        cmov2 ( rr[X] - 14.0f, line );
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 13.0f,line,0.0f);
        line += inc;
#endif
        if ( camera ) {
#ifdef IRISGL
           charstr ( "View:  CAM" );
#else //OPENGL
           npsDrawStringPos("View:  CAM",rr[X] - 14.0f,line,0.0f);
#endif
        }
        else {
#ifdef IRISGL
           charstr ( "View:  FWD" );
#else //OPENGL
           npsDrawStringPos("View:  FWD",rr[X] - 14.0f,line,0.0f);
#endif
        }

        line += inc;
#ifdef IRISGL
        cmov2 ( rr[X] - 16.0f, line );
#endif
        if ( camlock ) {
#ifdef IRISGL
           charstr ( "Camera: LOCK" );
#else //OPENGL
           npsDrawStringPos("Camera: LOCK",rr[X] - 16.0f,line,0.0f);
#endif
        }
        else {
#ifdef IRISGL
           charstr ( "Camera: UNLK" );
#else //OPENGL
           npsDrawStringPos("Camera: UNLK",rr[X] - 16.0f,line,0.0f);
#endif
        }

        line += inc;
        if ( targetting )
           {
           if ( target_lock != NO_LOCK )
              sprintf (text, "TARGETING LCK" );
           else
              sprintf (text, "TARGETING ON" );
           }
        else
           sprintf (text, "TARGETING OFF" );
#ifdef IRISGL
        cmov2 ( rr[X] - 14.0f, line);
        charstr(text);
#else //OPENGL
        npsDrawStringPos(text,rr[X] - 14.0f,line,0.0f);
#endif


/****************************************/
    // Draw Early Warning Message 
/****************************************/

        if ( early_warn )
           {
#ifdef DATALOCK
           pfDPoolLock ( (void *) G_dynamic_data );
#endif
           G_dynamic_data->early_warn = 0;
#ifdef DATALOCK
           pfDPoolUnlock ( (void *) G_dynamic_data );
#endif
           warn_time = cur_time;
           }

        if ((warn_time + 2.0f) > cur_time)
           {
           screen_x = float (view_right - view_left);
           screen_y = float (view_top - view_bottom);
#ifdef IRISGL
           ortho2(0.0f, screen_x, 0.0f, screen_y);
           fmsetfont(deadfont);
           sprintf ( text, "INCOMING" );
           temp_size = float(fmgetstrwidth(deadfont,text));
           cmov2 ( (screen_x - temp_size)/2.0f + 2.0f,
                   (screen_y - font_height - 7.0f) );
           RGBcolor(238,201,0);
           fmprstr(text);
           cmov2 ( (screen_x - temp_size)/2.0f,
                   (screen_y - font_height - 5.0f) );
           RGBcolor(255,0,0);
           fmprstr(text);
#else //OPENGL
           glLoadIdentity();
           gluOrtho2D( 0.0f, screen_x, 0.0f, screen_y );
           npsFont(TEXTFONT);
           sprintf ( text, "INCOMING" );
           temp_size = font_height;
           glColor4f(0.93f, 0.79f, 0.0f, 0.0f);
           npsDrawStringPos(text, (screen_x - temp_size)/2.0f + 2.0f,
                                  (screen_y - font_height - 7.0f), 0.0f);
           glColor4f(1.0f, 0.0f, 0.0f, 0.0f);
           npsDrawStringPos(text, (screen_x - temp_size)/2.0f,
                                  (screen_y - font_height - 5.0f), 0.0f);
           npsFont(DEFFONT);
#endif
           }
        } //Misc Info

/****************************************/
    // Draw Paused Message 
/****************************************/
     if ( paused )
        {
/*
        sprintf(text,"PAUSED");
        cmov2( rr[X] - 7.0f, ll[Y] + 1.0f );
        charstr(text);
        cmov2 ( ll[X] + 2.0f, ll[Y] + 1.0f );
        charstr(text);
*/
        screen_x = float (view_right - view_left);
        screen_y = float (view_top - view_bottom);
#ifdef IRISGL
        ortho2(0.0f, screen_x, 0.0f, screen_y);
        fmsetfont(deadfont);
#else //OPENGL
        glLoadIdentity();
        gluOrtho2D( 0.0f, screen_x, 0.0f, screen_y );
        npsFont(TEXTFONT);
#endif
        sprintf ( text, "PAUSED" );
#ifdef IRISGL
        temp_size = float(fmgetstrwidth(deadfont,text));
        cmov2 ( (screen_x - temp_size)/2.0f + 2.0f,
                (font_height + 7.0f) );
        RGBcolor(0,0,128);
        fmprstr(text);
        cmov2 ( (screen_x - temp_size)/2.0f,
                (font_height + 5.0f) );
        RGBcolor(255,215,0);
        fmprstr(text);
#else //OPENGL
        //temp_size = font_height; 
        display_font = npsGetCurXFont();
        temp_size = npsGetXFontWidth(display_font,text); 
        glColor4f(0.0f, 0.0f, 0.5f, 0.0f);
        npsDrawStringPos(text, (screen_x - temp_size)/2.0f + 2.0f,
                               (font_height + 7.0f), 0.0f);
        glColor4f(1.0f, 0.84f, 0.0f, 0.0f);
        npsDrawStringPos(text, (screen_x - temp_size)/2.0f,
                               (font_height + 5.0f), 0.0f);
        npsFont(DEFFONT);
#endif

        }

     screen_x = float (view_right - view_left);
     screen_y = float (view_top - view_bottom);
#ifdef IRISGL
     ortho2(0.0f, screen_x, 0.0f, screen_y);
     fmsetfont(deadfont);
#endif

/****************************************/
    // Draw Tethered Message 
/****************************************/
     if ( tethered )
        {
/*
        sprintf(text,"TETHER");
        cmov2( rr[X] - 7.0f, ll[Y] + 1.0f );
        charstr(text);
        cmov2 ( ll[X] + 2.0f, ll[Y] + 1.0f );
        charstr(text);
*/
        screen_x = float (view_right - view_left);
        screen_y = float (view_top - view_bottom);
#ifdef IRISGL
        ortho2(0.0f, screen_x, 0.0f, screen_y);
        fmsetfont(deadfont);
#else //OPENGL
        glLoadIdentity();
        gluOrtho2D( 0.0f, screen_x, 0.0f, screen_y );
        npsFont(TEXTFONT);
#endif
        if ( tethered == 1 )
           sprintf ( text, "TETHERED" );
        else if ( tethered == 2 )
           sprintf ( text, "ATTACHED" );
        else if ( tethered == 3 )
           sprintf ( text, "" ); // MIMIC
        else if ( tethered == 4 )
           sprintf ( text, "" ); // CAMERA LOCK
        else if ( tethered == 5 ) 
           sprintf ( text, "" ); // STEALTH ZOOM
        else 
           sprintf ( text, "UNKOWN TETHER");
#ifdef IRISGL
        temp_size = float(fmgetstrwidth(deadfont,text));
        cmov2 ( (screen_x - temp_size)/2.0f + 2.0f,
                (font_height + 7.0f) );
        RGBcolor(0,0,128);
        fmprstr(text);
        cmov2 ( (screen_x - temp_size)/2.0f,
                (font_height + 5.0f) );
        RGBcolor(255,215,0);
        fmprstr(text);
#else //OPENGL
        //temp_size = font_height; 
        display_font = npsGetCurXFont();
        temp_size = npsGetXFontWidth(display_font,text); 
        glColor4f(0.0f, 0.0f, 0.5f, 0.0f);
        npsDrawStringPos(text, (screen_x - temp_size)/2.0f + 2.0f,
                               (font_height + 7.0f), 0.0f);
        glColor4f(1.0f, 0.84f, 0.0f, 0.0f);
        npsDrawStringPos(text, (screen_x - temp_size)/2.0f,
                               (font_height + 5.0f), 0.0f);
        npsFont(DEFFONT);
#endif
        }

#ifdef OPENGL
        // This is here because OpenGL handles the MODELVIEW & PROJECTION 
        // matrixes differently then does IRISGL
        glMatrixMode(gl_matrix_mode);
#endif

     } // !birseye and ALIVE
  else
     {
     screen_x = float (view_right - view_left);
     screen_y = float (view_top - view_bottom);
#ifdef IRISGL
     ortho2(0.0f, screen_x, 0.0f, screen_y);
#else //OPENGL
     // This is here because OpenGL handles the MODELVIEW & PROJECTION matrixes
     // differently then does IRISGL
     // We have to store the original matrix because OpenGL multiplys the
     // matrices using gluOrtho, but GL does not.
     glGetIntegerv (GL_MATRIX_MODE, &gl_matrix_mode);
     glMatrixMode (GL_PROJECTION);
     glLoadIdentity();
     gluOrtho2D( 0.0f, screen_x, 0.0f, screen_y );
#endif
     if ( dead_or_alive == ALIVE ) {
     }
     else if ( dead_or_alive != DEAD )
        {
#ifdef IRISGL
        fmsetfont(deadfont);
#else //OPENGL
        npsFont(TEXTFONT);
#endif
        switch (cause_of_death)
           {
           case GROUND_DEATH : sprintf ( text, "YOU HIT THE GROUND!");
                               break;
           case BRIDGE_DEATH : sprintf ( text, "YOU HIT A BRIDGE!");
                               break;
           case FOILAGE_DEATH : sprintf ( text, "YOU HIT THE TREES!" );
                                break;
           case WATER_DEATH : sprintf ( text, "YOU HIT THE WATER!" );
                                break;
           case S_OBJECT_DEATH : sprintf ( text, "YOU HIT A STATIONARY OBJECT!"
);
                                break;
           case D_OBJECT_DEATH : sprintf ( text, "YOU HIT ANOTHER PLAYER!" );
                                break;
           case MUNITION_DEATH : sprintf ( text, "YOU WERE HIT & BLOWN AWAY!" );                                break;
           case FALLING_DEATH : sprintf ( text, "YOU ARE FALLING!" );
                               break;
           case UNDER_GROUND_DEATH : sprintf ( text, "YOU WENT UNDERGROUND!" );
                                     break;
           default:
              sprintf ( text, "YOU HIT SOMETHING HARD!" );
           }


        if ( !G_runisms )
          {
#ifdef IRISGL
          temp_size = float(fmgetstrwidth(deadfont,text));
          RGBcolor(0,0,0);
          cmov2 ( (screen_x - temp_size)/2.0f,
                  (screen_y - font_height/2.0f) / 2.0f + 1.0f );
          fmprstr(text);
          RGBcolor(238,201,0);
          cmov2 ( (screen_x - temp_size) / 2.0f,
                  (screen_y - font_height/2.0f) / 2.0f );
          fmprstr(text);
#else //OPENGL
          //temp_size = font_height; 
          display_font = npsGetCurXFont();
          temp_size = npsGetXFontWidth(display_font,text); 
          glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
          npsDrawStringPos(text, (screen_x - temp_size)/2.0f,
                                 (screen_y - font_height/2.0f) / 2.0f + 1.0f,
                                 0.0f);
          glColor4f(0.93f, 0.79f, 0.0f, 0.0f);
          npsDrawStringPos(text, (screen_x - temp_size)/2.0f,
                                 (screen_y - font_height/2.0f) / 2.0f, 0.0f);
#endif
          }

#ifdef OPENGL
        npsFont(DEFFONT);
#endif

        }
     else
        {
#ifdef IRISGL
        fmsetfont(deadfont);
        sprintf(text,"PLEASE TRY AGAIN.");
        temp_size = float(fmgetstrwidth(deadfont,text));
        RGBcolor(0,0,0);
        cmov2 ( (screen_x - temp_size)/2.0f,
                (screen_y - font_height/2.0f) / 2.0f + 1.0f );
        fmprstr(text);
        RGBcolor(238,201,0);
        cmov2 ( (screen_x - temp_size) / 2.0f,
                (screen_y - font_height/2.0f) / 2.0f );
        fmprstr(text);
#else //OPENGL
        npsFont(TEXTFONT);
        sprintf(text,"PLEASE TRY AGAIN.");
        //temp_size = font_height; 
        display_font = npsGetCurXFont();
        temp_size = npsGetXFontWidth(display_font,text); 
        glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
        npsDrawStringPos(text, (screen_x - temp_size)/2.0f,
                               (screen_y - font_height/2.0f) / 2.0f + 1.0f,
                               0.0f);
        glColor4f(0.93f, 0.79f, 0.0f, 0.0f);
        npsDrawStringPos(text, (screen_x - temp_size)/2.0f,
                               (screen_y - font_height/2.0f) / 2.0f, 0.0f);
        npsFont(DEFFONT);
#endif
        }
#ifdef OPENGL
     pfPopMatrix();
#endif
     //pfPopMatrix();

#ifdef OPENGL
     // This is here because OpenGL handles the MODELVIEW & PROJECTION 
     // matrixes differently then does IRISGL
     glMatrixMode(gl_matrix_mode);
#endif
     }

}

void
VEHICLE::update_time ()
{
   cdata.last_time = cdata.current_time;
   cdata.current_time = G_curtime;
   cdata.delta_time = cdata.current_time - cdata.last_time;
}


void
VEHICLE::fill_in_pass_data ( PASS_DATA *pdata )
{
   pdata->posture = posture;
   if (input_control == NPS_ODT) {
      pdata->posture.hpr[PF_H] = 0.0f;
      pdata->posture.hpr[PF_P] = 0.0f;
      pdata->posture.hpr[PF_R] = 0.0f;
   }
   pdata->look = look;
   pdata->type = icontype;
   pdata->speed = speed;
   pdata->altitude = altitude;
   pdata->input_control = input_control;
   pdata->input_number = input_number;
   pdata->thrust = thrust;
}


int
VEHICLE::transport ( float new_x, float new_y, float new_alt )
{
   float curr_alt;

   if ( (G_dynamic_data->bounding_box.xmin <= new_x) &&
        (new_x <= G_dynamic_data->bounding_box.xmax) &&
        (G_dynamic_data->bounding_box.ymin <= new_y) &&
        (new_y <= G_dynamic_data->bounding_box.ymax) ) {
      posture.xyz[X] = new_x;
      posture.xyz[Y] = new_y;
      // leave temp_pos.xyz[Z] from old posture as good guess
      curr_alt = this->get_altitude();
      posture.xyz[Z] += (new_alt - curr_alt);
      return TRUE;
   }
   return FALSE;
}

float
VEHICLE::get_altitude()
{
   // Call the get altitude from terrain.cc
   return ::get_altitude(posture.xyz);
}

int
VEHICLE::find_joint ( unsigned short search_dis_id,
                         JNT_INFO **joint_ptr )
{
   int count = 0;
   int found = FALSE;

   while ( (count < num_joints) && (!found) ) {
      found = ( joint[count].dis_id == search_dis_id );
      if (!found) {
         count++;
      }
   }

   if ( found ) {
      *joint_ptr = &(joint[count]);
   }
   else {
      *joint_ptr = NULL;
   }
   return found;
}


void
VEHICLE::update_joint ( JNT_INFO *our_artic,
                           ArticulatParams *artic_from_pdu )
{
   unsigned short ap_type;
   float temp_float;

   ap_type = (artic_from_pdu->parameter_type & AP_Type_Mask);
   bcopy ( &(artic_from_pdu->parameter_value), &temp_float, 4);

   switch ( (int)ap_type ) {
      case AP_X:         // DIS X is Performer Y
         if (our_artic->articulate[1]) {
            if ( temp_float < our_artic->min_range.xyz[PF_Y] ) {
               our_artic->current_setting.xyz[PF_Y] =
                  our_artic->min_range.xyz[PF_Y];
            }
            else if ( temp_float > our_artic->max_range.xyz[PF_Y] ) {
               our_artic->current_setting.xyz[PF_Y] =
                  our_artic->max_range.xyz[PF_Y];
            }
            else {
               our_artic->current_setting.xyz[PF_Y] =
                  temp_float;
            }
         pfDCSCoord (our_artic->dcs, &(our_artic->current_setting));
         }
         break;
      case AP_Y:         // DIS Y is Performer X
         if (our_artic->articulate[0]) {
            if ( temp_float < our_artic->min_range.xyz[PF_X] ) {
               our_artic->current_setting.xyz[PF_X] =
                  our_artic->min_range.xyz[PF_X];
            }
            else if ( temp_float > our_artic->max_range.xyz[PF_X] ) {
               our_artic->current_setting.xyz[PF_X] =
                  our_artic->max_range.xyz[PF_X];
            }
            else {
               our_artic->current_setting.xyz[PF_X] =
                  temp_float;
            }
         pfDCSCoord (our_artic->dcs, &(our_artic->current_setting));
         }
         break;
      case AP_Z:         // DIS Z is Performer -Z
         temp_float = -temp_float;
         if (our_artic->articulate[2]) {
            if ( temp_float < our_artic->min_range.xyz[PF_Z] ) {
               our_artic->current_setting.xyz[PF_Z] =
                  our_artic->min_range.xyz[PF_Z];
            }
            else if ( temp_float > our_artic->max_range.xyz[PF_Z] ) {
               our_artic->current_setting.xyz[PF_Z] =
                  our_artic->max_range.xyz[PF_Z];
            }
            else {
               our_artic->current_setting.xyz[PF_Z] =
                  temp_float;
            }
         pfDCSCoord (our_artic->dcs, &(our_artic->current_setting));
         }
         break;
      case AP_Azimuth:         // DIS Azimuth is Performer -Heading
         temp_float = -temp_float * RAD_TO_DEG;
         while ( temp_float >= 360.0f ) {
            temp_float -= 360.0f;
         }
         while ( temp_float < 0 ) {
            temp_float += 360.0f;
         }
         if (our_artic->articulate[3]) {
            if ( temp_float < our_artic->min_range.hpr[PF_H] ) {
               our_artic->current_setting.hpr[PF_H] =
                  our_artic->min_range.hpr[PF_H];
            }
            else if ( temp_float > our_artic->max_range.hpr[PF_H] ) {
               our_artic->current_setting.hpr[PF_H] =
                  our_artic->max_range.hpr[PF_H];
            }
            else {
               our_artic->current_setting.hpr[PF_H] = temp_float;
            }
         pfDCSCoord (our_artic->dcs, &(our_artic->current_setting));
         }
         break;
      case AP_Elevation:         // DIS Elevation is Performer -Pitch
         temp_float = temp_float * RAD_TO_DEG;
         while ( temp_float >= 180.0f ) {
            temp_float -= 360.0f;
         }
         while ( temp_float < -180.0f ) {
            temp_float += 360.0f;
         }
         if (our_artic->articulate[4]) {
            if ( temp_float < our_artic->min_range.hpr[PF_P] ) {
               our_artic->current_setting.hpr[PF_P] =
                  our_artic->min_range.hpr[PF_P];
            }
            else if ( temp_float > our_artic->max_range.hpr[PF_P] ) {
               our_artic->current_setting.hpr[PF_P] =
                  our_artic->max_range.hpr[PF_P];
            }
            else {
               our_artic->current_setting.hpr[PF_P] = temp_float;
            }
         pfDCSCoord (our_artic->dcs, &(our_artic->current_setting));
         }
         break;
      case AP_Rotation:         // DIS Rotation is Performer -Roll
         temp_float = temp_float * RAD_TO_DEG;
         while ( temp_float >= 360.0f ) {
            temp_float -= 360.0f;
         }
         while ( temp_float < 0 ) {
            temp_float += 360.0f;
         }
         if (our_artic->articulate[5]) {
            if ( temp_float < our_artic->min_range.hpr[PF_R] ) {
               our_artic->current_setting.hpr[PF_R] =
                  our_artic->min_range.hpr[PF_R];
            }
            else if ( temp_float > our_artic->max_range.hpr[PF_R] ) {
               our_artic->current_setting.hpr[PF_R] =
                  our_artic->max_range.hpr[PF_R];
            }
            else {
               our_artic->current_setting.hpr[PF_R] = temp_float;
            }
         pfDCSCoord (our_artic->dcs, &(our_artic->current_setting));
         }
         break;
      case AP_Position:
      case AP_PositionRate:
      case AP_Extension:
      case AP_ExtensionRate:
      case AP_XRate:
      case AP_YRate:
      case AP_ZRate:
      case AP_AzimuthRate:
      case AP_ElevationRate:
      case AP_RotationRate:
      default:
         /* Unsupported functions in NPSNET */
         break;
   }


}


void
VEHICLE::update_articulations ( ArticulatParams *APNptr, int artic_count )
{
   int count = 0;
   ArticulatParams *next_artic = APNptr;
   JNT_INFO *next_joint;

   while ( count < artic_count ) {
#ifdef DIS_2v4
      if ( next_artic->parameter_type_designator == 0 ) {
         // Articulated Part as opposed to Attached Part = 1
#endif
      if ( find_joint((next_artic->parameter_type & AP_Part_Mask),
           &next_joint) ) {
         update_joint ( next_joint, next_artic );
      }
      else {
         pfNotify ( PFNFY_NOTICE, PFNFY_PRINT,
                    "Ground veh - update of illegal joint from net pdu." );
      }
#ifdef DIS_2v4
      }
#endif
   count++;
   next_artic++;
   }
}

void
VEHICLE::vehAuto_init (const char *autoFile)
{
    VEHICLE::vehAuto = new npsAutomate (autoFile);
}
