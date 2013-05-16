/*
 File       :  shipglobals.h
 Authors    :  James F. Garrova
            :  Joseph A. Nobles
 Date       :  22 January 1995
 Advisors   :  Dr Michael J. Zyda
            :  Dr David A. Pratt
 Reference  :  npsnetIV Ver 6.0

 Description:  This header file is a depository of 
               include files and global variables for
                deployable shiphandling simulator.
                   
*/

#ifndef _SHIPGLOBALS_H
#define _SHIPGLOBALS_H

//Standard includes
#include<stdlib.h>
#include<unistd.h>
#include<iostream.h>
#include<string.h>
#include<fstream.h>
#include<math.h>
#include<bstring.h>
#include<ctype.h>
#include<sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

/* X Windows includes */
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

//GL includes
#include <gl/device.h>
#include <gl/gl.h>

//Performer includes
#include "pf.h"
#include "prmath.h"
#include "pfutil.h"
#include "pfflt.h"
#include "pfsgi.h"

// Ship includes
#include "disnetlib.h"
#include "ship_common.h"
#include "ship_defs.h"
#include "ship_macros.h"
#include "ship_init.h"
#include "ship_draw.h"
#include "ship_environ.h"
#include "ship_terrain.h"
#include "ship_network.h"
#include "ship_vehicle.h"
#include "ship_cpanel.h"
#include "ship_common2.h"
#include "main.h"
#include "shipview.h"
#include "ship_entity.h"

#ifndef _GLOBAL_
#define _GLOBAL_ extern
#endif


_GLOBAL_
DIS_net_manager * net;

_GLOBAL_
DYNAMIC_DATA * G_dynamicData;

_GLOBAL_
STATIC_DATA * G_staticData;

_GLOBAL_
int G_drivenShip = 0;

_GLOBAL_
SHIP_LIST *G_vehlist;

_GLOBAL_
SHIPTYPE_LIST *G_vehtype;

_GLOBAL_
OBJECTTYPE_LIST *G_objTypeList;

_GLOBAL_
OBJECT_LIST *G_objList;

_GLOBAL_
pfGroup *G_moving;

_GLOBAL_
pfGroup *G_stationary;

_GLOBAL_
float G_curtime;

_GLOBAL_
CONTROL control_data;

_GLOBAL_
PASS_DATA *ownShipData;

_GLOBAL_
float G_curTime;

_GLOBAL_
ofstream G_scriptFile;

_GLOBAL_
ifstream playBackFile;

#endif

