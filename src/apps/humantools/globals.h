/****************************************************************
 *  Program:  globals.h                                         *
 *  Author :  Jim Ehlert                                        *
 *  Date   :  17 MAR 95                                         *
 *  Description : This program contains the global information  *
 *  for main.c to read the three dude files.                    *
 ****************************************************************/
#ifndef __GLOBALS_H__
#define __GLOBALS_H__
#ifndef __COMMON__
#define __COMMON__ extern
#endif

#include <fstream.h>
#include <gl/glws.h>
#include "Performer/pf.h";

typedef struct // structure residing in shared memory that the
{                     // application, cull, and draw processes can access
   float radius;       
   float offsetZ;
   float sinH;
   float cosH;
   float sinP;
   float cosP;
} eyeStruct;         

typedef struct
{
    eyeStruct   eye; 
    long        exitFlag;
    long        standing;
    long        kneeling;
    long        prone;
    long        walking;
    long        standing1;
    long        kneeling1;
    long        prone1;
    long        walking1;
    long        inWindow;
    float       mouseX;
    float	mouseY;
    long	mouseButtons;
    long	winOriginX;
    long	winOriginY;
    long	winSizeX;
    long	winSizeY;
    pfCoord	view;
    float	sceneSize;
    int		drawStats;
    Window	xWin;
    Window	glWin;
    Window	overWin;
    long	redrawOverlay;
    pfScene     *scene;
    int         distance;
} SharedData;

__COMMON__
SharedData         *Shared;

__COMMON__
Display *XDpy;

__COMMON__
ulong WinConfig;

__COMMON__
pfLight *Sun;  // light source created and updated in draw-process 


#endif
