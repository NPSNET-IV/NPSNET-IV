#ifndef _SHIP_ENTITY_H_
#define _SHIP_ENTITY_H_

#include "shipglobals.h"
//#include "ship_defs.h"
//#include "ship_common.h"

extern void
initVehList();

extern void
createOwnship();

extern void
createNewEntity(int, int, ForceID);

extern void
updatePosture();

void
repositShipsForPlayback();
void
clearPBList(int);
#endif
