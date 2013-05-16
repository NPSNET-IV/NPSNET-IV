/*
 *  picking.c
 *	This code was written as part of the NPS Virtual
 *	Ship project and for the thesis of Perry McDowell
 *	and Tony King.
 *	This contains the source code to allow the
 *	user tto pick objects and interact with objects
 *	during the shipboard walkthrough. For a 
 *	description of the functions in this file, 
 *	please see picking.h
 *
 *    Authors: Perry L.McDowell & Tony King
 *    Thesis Advisors:	Dr. Dave Pratt
 *			Dr. Mike Zyda
 *    Date:  01 December 1994
 *
 */



#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <strings.h>
#include <Performer/pf.h>
#include "pfutil.h"
#include "manager.h"
#include "common_defs2.h"
#include "interface_const.h"
#include "picking.h"


extern volatile MANAGER_REC       *G_manager;

Pick *NewPick(pfNode *N, pfChannel **C)
{
    Pick *P = (Pick *)pfMalloc(sizeof(Pick), pfGetSharedArena());

    P->chan = C;
    P->root = N; //pfNewDCS();
    P->traverse= PFTRAV_IS_PRIM | PFPK_M_NEAREST | PFTRAV_IS_PATH;
    return P;
}


long PathStringMake(char *pname, pfPath *P)
{
    int i, n;
    char *name;
    const char *tmp;
   
    n = pfGetListArrayLen(P);
    pname[0] = '\0';
    
    for(i=0;i<n;i++)
    {
	if (pfGet(P, i) == NULL)
	    break;
	tmp = pfGetNodeName(pfGet(P, i));
	if (tmp)
	{
	    name = strdup(tmp);
	    pname[strlen(pname)+1] = '\0';
	    pname[strlen(pname)] = '"';
	    strcat(pname, name);
	    free(name);
	    pname[strlen(pname)+1] = '\0';
	    pname[strlen(pname)] = '"';
	}
	else
	{
	    name = strdup(pfGetTypeName((pfObject *)pfGet(P, i)));
	    strcat(pname, name);
	    free(name);
	}
	if (pfGetNodeTravData((pfNode *)pfGet(P, i),PFTRAV_ISECT)) {
	   pname[strlen(pname)+1] = '\0';
	   pname[strlen(pname)] = '@';
        }
	pname[strlen(pname)+1] = '\0';
	pname[strlen(pname)] = '/';
    }
    return n;
}


pfNode *FindDataFromPath(pfPath *P)
{
    int i, n;
    const char *tmp;
    pfNode *node_ptr = NULL;
   
    n = pfGetListArrayLen(P);
    
    for(i=n-1;i>=0;i--)
    {
	if (pfGet(P, i) == NULL)
	    continue;
	tmp = pfGetNodeName(pfGet(P, i));
        if (tmp) {
           // Check to see if we have gone to far in the tree
	   if (strstr(tmp,"antares") != NULL)
              break;
        }
        // Check to see if node contains infodata for ships
	if (pfGetNodeTravData((pfNode *)pfGet(P, i),PFTRAV_ISECT)) {
           node_ptr = (pfNode *)pfGet(P, i);
           break;
        }
    }
    return node_ptr;
}


pfNode *DoPick(Pick *P)
{
    long            pick_count = 0;
    static pfHit 	**ipicked[10];
    pfPath         *path;
    char            pathname[300];
    pfNode         *node_ptr;


    pfNodePickSetup(P->root);
   
    if ((*P->chan != NULL) && (P->root != NULL)) {
       pick_count = pfChanPick(*P->chan, P->traverse, 0.5f, 0.5f, 0.0f, ipicked);
    }

    if (pick_count > 0) {
   	pfQueryHit(*ipicked[0], PFQHIT_NODE, &P->picked);
    	pfQueryHit(*ipicked[0], PFQHIT_PATH, &path);
	PathStringMake(pathname, path);
//cerr<<"Picking ("<<pick_count<<") Path: "<<pathname<<endl;
        // Search for correct data to use later
        node_ptr = FindDataFromPath(path);
    }
    else {
        node_ptr = NULL;
        P->picked = NULL;
    }

    return node_ptr;
}


int
get_pick_button ( int num, int &input_number)
{
   input_manager *l_input_manager = G_manager->ofinput;
   int counter = 0;

   switch ( num ) {
         case NPS_MOUSE_LEFT_BUTTON:
            l_input_manager->query_button(counter,NPS_KEYBOARD,
                                          NPS_MOUSE_LEFT_BUTTON);
            if ( counter > 0 ) {
               counter = 1;
            }
            break;
         case NPS_MOUSE_RIGHT_BUTTON:
            l_input_manager->query_button(counter,NPS_KEYBOARD,
                                          NPS_MOUSE_RIGHT_BUTTON);
            if ( counter > 0 ) {
               counter = 1;
            }
            break;
         case NPS_STCK_BOT_BUTTON:
            l_input_manager->query_button(counter,NPS_FCS,
                                          NPS_STCK_BOT_BUTTON,input_number);
            if ( counter > 0 ) {
               counter = 1;
            }
            break;
         case NPS_THRTL_BUTTON_7:
            l_input_manager->query_button(counter,NPS_FCS,
                                          NPS_THRTL_BUTTON_7,input_number);
            if ( counter > 0 ) {
               counter = 1;
            }
            break;
         case NPS_THRTL_BUTTON_3:
            l_input_manager->query_button(counter,NPS_FCS,
                                          NPS_THRTL_BUTTON_3,input_number);
            if ( counter > 0 ) {
               counter = 1;
            }
            break;
         default:
            break;
   } // switch

   return (int)counter;
}

