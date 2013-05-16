#ifndef _SHIP_TERRAIN_H
#define _SHIP_TERRAIN_H
#include "shipglobals.h"
#include "ship_defs.h"




void
loadTerrain();

void
loadTerrainModels();

void
setTheMasks(pfGroup * theSwitch, int type, int specific);

#endif
