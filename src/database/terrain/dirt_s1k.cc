// File: <dirt_s1k.cc>

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

#ifndef NOS1kLOADER
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <Performer/pf.h>

#define _PERF_2_0_
extern "C" {
#include "pfs1k.h"
}

#include "common_defs.h"

extern volatile DYNAMIC_DATA    *G_dynamic_data;

#define OVERRIDEFILE "/tmp/command.override.file.s1k"

pfGroup * LoadDirtS1k(char *fileName)
//load the static S1000 databases, this is just a wrapper
{
  return((pfGroup *)pfdLoadFile_s1k(fileName));
}


pfGroup * LoadDirtS1kOverride(char *fileName)
//load the static S1000 databases, since there is a override from the 
//command line we have to play some games
{
  FILE*        cfile,*ofile;
  pfGroup*      dirt;
  char         token[255],dummy[255];
  
  //first read the file and write it out with the new offsets
  // read the control file
  if(NULL == (cfile = fopen(fileName,"r"))){
    printf("Unable to open the control file %s\n",fileName);
    exit(0);
  }
  // open up the temp file
  if(NULL == (ofile = fopen(OVERRIDEFILE,"w"))){
    printf("Unable to open the terrain control file %s\n",OVERRIDEFILE);
    exit(0);
  }

  //read and filter the file
  while (1 == fscanf(cfile,"%s ",token)){
    if(!strcmp(token,"SEARCH_WINDOW")){
      fgets(dummy,255,cfile);
      printf("Token SEARCH_WINDOW has been overridden\n");
    }
    else if(!strcmp(token,"ORIGIN")){
      fgets(dummy,255,cfile);
      printf("Token ORIGIN has been overridden\n");
    }
    else { //normal stuff, just print it out
       fgets(dummy,255,cfile);
       fprintf(ofile,"%s %s",token,dummy);
    }  
  } 
  //now write out the size of the database
  fprintf(ofile,"SEARCH_WINDOW %10.1f %10.1f %10.1f %10.1f\n",
      G_dynamic_data->TDBoffsets[0],
      G_dynamic_data->TDBoffsets[1],
      G_dynamic_data->TDBoffsets[0]+G_dynamic_data->bounding_box.xsize,
      G_dynamic_data->TDBoffsets[1]+G_dynamic_data->bounding_box.ysize);

  //base it a zero to reduce the error with the large databases
  fprintf(ofile,"ORIGIN  %10.1f %10.1f\n",
      G_dynamic_data->TDBoffsets[0],G_dynamic_data->TDBoffsets[1]);
  
  
  fclose(ofile);

  dirt = (pfGroup *)pfdLoadFile_s1k(OVERRIDEFILE);
  //get rid fo the file
  //unlink(OVERRIDEFILE);
  printf("Remove the coment from the unlink\n");

  return(dirt);
}

pfGroup * LoadDirtS1kPaged(char *fileName)
//setup the paged terrain databases
{
  printf("Terrain Paging is not yet implemented file %s\n",fileName);
  pfExit();
  exit(0);
  return(NULL);
}

#endif
