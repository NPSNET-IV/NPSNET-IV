// File: <common_defs2.h>

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

#ifndef __NPS_COMMON_DEFS2__
#define __NPS_COMMON_DEFS2__

#include <pf.h>
#include "constants.h"
#include "pdu.h"
#include "common_defs.h"
#include "vehicle.h"
#include "weapon.h"

//these are the data stuctures that are used globally but all the class have
//to have all for the classes defined first

typedef struct
   {
   int munition;
   mtype classtype;
   pfVec3 firepoint;
   float firedelay;
   } FIRE_INFO;

// individual vehicle record within the global array
typedef struct
   {
   int            id;           // vehicle Id number
   int            type;         // index into the type array (G_vehtype)
   entity_control control;      // who controls the vehicle
   vtype          cat;          // the Catagory of the entity
   EntityID       DISid;        // the DIS id of the vehicle
   VEHICLE        *vehptr;      // pointer the vehicle's data
   } VEH_LIST;

// matches geometry to each vehicle type and an id
typedef struct
   {
   int        id;         // vehicle type number
   int        dead;       // what it is when it is dead
   char       name[100];  // The name of the model
   int        numclones;  // Number of cloned instances of model
   int        num_joints; // Number of articulation points
   int	      num_figs;   // Nbr of figs in model (rifle makes 2)
   JNT_INFO   joints[MAXJOINTS];// Name of articulation points
   vtype      cat;        // the Catagory of the entity
   int        num_weapons;// The number of weapons < MAX_FIRE_INFO
   FIRE_INFO  fireinfo[MAX_FIRE_INFO]; // Munitions capabilities
   EntityType distype;    // the DIS type
   pfGroup    *model;     // the model of the vehicle
   pfSwitch   *mswitch;    // the switch node for alive/dead
   pfBox      bounding_box; // Bounding box around model geometry
   int        modtype;      // Flight, Tips, etc.
   } VEHTYPE_LIST;

//the list of munitions, these are the ones that I control, all the others
// are kept in the vehicle array.
typedef struct 
   {
   int      id;        // missileId number
   ushort   eventid;   // used by DIS to keep track of fire/impact records*/
   mtype    cat;       // the Catagory of the munition
   pfDCS    *dcs;      // the DCS of the weapon
   pfSwitch *icon;     // the switch node to tell which type of Icon it is
   MUNITION *mptr;
   } WEAPON_LIST;

//The list of static object types
typedef struct
   {
   int        id;         // object type number
   int        dead;       // what it is when it is dead
   char       name[100];  // The name of the model
   vtype      cat;        // the Catagory of the object
   pfGroup    *model;     // the model of the object
   pfSwitch   *mswitch;    // the switch node for alive/dead
   } OBJTYPE_LIST;

//Link list node used to build the hud display list

typedef struct _HUD_VEH
   {
   float distance;
   VEH_LIST *vehptr;
   struct _HUD_VEH *next;
   } HUD_VEH;

//Pointer to a link list node used for the hud display list
typedef HUD_VEH *HUD_VEH_PTR;

   
#endif
