/*
*  File       :  ship_environ.h
*  Author     :  Jim Garrova
*                Joe Nobles
*  Date       :  30 January 1991  
*  Revised    :  6 February 1995              
*                 
*  Advisors   :  Dr Michael J. Zyda                
*             :  Dr David R. Pratt
*  Reference  :  SGI perfly utility env.c      
*/

#ifndef _SHIP_ENVIRON_H_
#define _SHIP_ENVIRON_H_
#include "ship_defs.h"
#include "shipglobals.h"


extern void initEnviron();
extern void updateEnviron();
extern void updateTimeOfDay(float);
void lightTheTown(long);
#endif
