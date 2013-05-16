/*
 *  object.h
 *	This code was written as part of the NPS Virtual
 *	Ship project and for the thesis of Perry McDowell
 *	and Tony King.
 *	This contains the declarations of functions 
 *	to manipulate the objects the user can control 
 *	in the shipboard walkthrough. 
 *
 *    Authors: Perry L.McDowell & Tony King
 *    Thesis Advisors:	Dr. Dave Pratt
 *			Dr. Mike Zyda
 *    Date:  01 December 1994
 *
 *    Modified 21 September 1995
 *    by Jim O'Byrne
 */

#ifndef __OBJECT_H__
#define __OBJECT_H__ 

#define MAX_SWITCHES 15
#define MAX_VALVES 15
#define MAX_BUTTONS 16
#define MAX_DOORS 16
#define STEAM_VALVE 0
#define OIL_VALVE 1
#define VENT_BUTTON 0
#define HALON_BUTTON 1

#define MAX_INFO 128

// The indicators of where certain switches are
#define NOZZLE  0
#define WALL    1
#define VEHRAMP 2

typedef struct
{
   ulong	type;		//The type of infodata
   char		name[32];
   char		function[128];
   char		soundbite[16];
   int		index;		//MultiNode index
   float	perCent;	//Valve percent open
   int		pointer;	//Gauge Value
   DOFcb	*dof;		//Degree of Freedom information
   
} infodataStruct;

extern pfNode	  *multiMatrix[MAX_INFO];
extern pfSwitch   *ShipSwitches[MAX_SWITCHES];
extern pfGroup    *ShipValves[MAX_VALVES];
extern pfGroup    *ShipButtons[MAX_BUTTONS];
extern pfGroup    *ShipDoors[MAX_DOORS];

/*
 * openFlightConfig-
 *
 *	This function tells the loader that 'openFlightCallBack'
 *	is the loading callback.  It must be called before
 *	any call of 'LoadFile' or 'LoadFlt'.
 *
 */
pfGroup  *openFlightConfig (char *);

void ship_draw_info( pfChannel *chan, void *data );
void infodisplay(char *name, char *function, char *sound);

#endif

/*
 * ObjectRelatedEvent-
 */
//void ObjectRelatedEvent(pfNode *node, int direction);
