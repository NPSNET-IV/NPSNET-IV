/*
*  File       :  main.h
*  Author     :  Jim Garrova
*                Joe Nobles
*  Description:  This file is used for the program shipdemopf
*                "Performer shipdemo" which drives a ship model
*                 through a terrain database.  Parts of this code
*                 were derived from the SGI perfly utility 
*                 main.h source code.
*/

#ifndef _MAIN_H_
#define _MAIN_H_

#include "ship_defs.h"
#include "shipglobals.h"

extern long FreeInitCPUs;
extern void DrawFunc(pfChannel *, void *);
extern void CullFunc(pfChannel *, void *);

#endif
