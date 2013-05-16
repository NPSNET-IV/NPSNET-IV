/*
*  File       :  ship_cpanel.h
*  Author     :  Jim Garrova
*                Joe Nobles
*  Description:  This file is used for the program shipdemopf
*                ("Performer shipdemo") which drives a ship model
*                 through a terrain database.  Parts of this code
*                 were derived from the SGI perfly utility 
*                 gui.h source code. This source creates and
*                 updates a control panel or "GUI"
*/

#ifndef _SHIP_CPANEL_H_
#define _SHIP_CPANEL_H_
#include "ship_defs.h"
#include "shipglobals.h"

/* Enumerate options for control panel */
enum CPANELID
{
  CPANEL_AA,
  CPANEL_LIGHTING,
  CPANEL_QUIT,
  CPANEL_REPOSITION,
  CPANEL_RESET_ALL,
  CPANEL_TEXTURE,
  CPANEL_TOD,
  CPANEL_WIREFRAME,
  CPANEL_RUDDERANGLE,
  CPANEL_LEFTENGINERPM,
  CPANEL_RIGHTENGINERPM,  
  CPANEL_BRIDGEVIEW,
  CPANEL_FLYVIEW,
  CPANEL_IMAGE,
  CPANEL_DISPOFF,
  CPANEL_LEFTWINGVIEW,
  CPANEL_RIGHTWINGVIEW,
  CPANEL_RUNLIGHTS,
  CPANEL_STATS,
  CPANEL_BRIDGEVIEWOFFSET,
  CPANEL_HORIZONANGLE,
  CPANEL_AZCIRCLE,
  CPANEL_PLAYBACK,
  CPANEL_FASTFORWARD,
  CPANEL_FOGSWITCH,
  CPANEL_FOGFARRANGE,
  CPANEL_COMBINESHAFTS
};
typedef enum CPANELID CPANALId;

/* Function prototypes */
extern void initCPanel(pfPipe *);
extern void updateCPanel();
extern void updateWidget(long, float);
extern void updateSim(pfChannel *);
static void updateStats(pfChannel *);
//void checkLights();
void lightTheShips();
#endif
