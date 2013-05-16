#ifndef _SHIP_INIT_H
#define _SHIP_INIT_H

#include "shipglobals.h"
#include "ship_defs.h"
//#include "ship_common.h"

extern void preConfig();
extern void initSharMem(char[]);
extern void initScene();
extern void initPipe();
extern void initChannel();
extern void initXformer();
extern void IsectFunc(void *);
extern void PreFrame();
extern void PostFrame();
extern void PreCull(pfChannel *, void *);
extern void PostCull(pfChannel *, void *);
extern void PreDraw(pfChannel *, void *);
extern void PostDraw(pfChannel *, void *);

void OpenPipeline(pfPipe *);
void InitGfx(pfPipe *p);

#endif
