// File: <lake.h>

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
 *    URL: http://www.cs.nps.navy.mil/research/npsnet/
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *
 * Thank you to our sponsors:  ARL, ARPA, DMSO, STRICOM, TEC, TRAC and
 *                             NPS Direct Funding
 */


#ifndef _LAKE_H_
#define _LAKE_H_

#include <math.h>
#include <string.h>
#include <iostream.h>

#include "pf.h"
#include "pfdb/pfflt.h"
#include "water.h"

static pfGroup 		*lakeRoot;
typedef struct
{
    pfTexture 	**texlist;
    long	texcount;
    long 	bindtex;
    int		*activeBuffer;
    Wave	wave[4];
} LakeData;

typedef struct
{
    long	numChannels;
    long	dftTexOn;
    LakeData    *lakeData;
} SharedWaterData;

extern SharedWaterData         *SharedWater;

void InitLake(void);
void SelectLake(long s);
long LoadLakeDB(pfScene *scene);
void LakeSim(pfChannel *chan, float xposit, float yposit, float direction);
void ActivateLake(int onoff);
#endif
