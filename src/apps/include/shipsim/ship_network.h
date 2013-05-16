
/*
 File       :  ship_network.h
 Authors    :  James F. Garrova
            :  Joseph A. Nobles
 Date       :  22 January 1995
 Advisors   :  Dr Michael J. Zyda
            :  Dr David A. Pratt
 Reference  :  npsnetIV Ver 6.0 netutil.h
               disnetlib.h
   
 Description:  This file contains the function prototypes for
               the shiphandling simulator's network utilities.
*/

#ifndef _SHIP_NETWORK_H_
#define _SHIP_NETWORK_H_
#include "shipglobals.h"

#define EMPTY -1
// a removed entity leaves on of these in the next block is used
#define AVAIL -2

extern "C"
{
  int gethostname(char*, int);
}


//Structure for each item in a hash table
typedef struct{  // the hash table type
    int vid;
    EntityID netid;
} VIDTABLETYPE;


#define VIDTABLETYPESIZE 6741 //size of vehicle id hash table
#define MAXTYPES 10
#define MAXCLASS  10
#define MAXHULL 50

VIDTABLETYPE shipIDHash[VIDTABLETYPESIZE];

int shipTypeArray[MAXTYPES][MAXCLASS][MAXHULL];



void findsitehost(char *);

extern void openNet();
extern int checkTheNet(DIS_net_manager *);
extern void initModelArray(char filename[]);
extern void insertShipType(EntityType *, int);
extern int delta_send(pfCoord *, pfCoord *);

void initTheHash();

void processEntity(EntityStatePDU *);
extern void initShipTypeArray();
void insertShipType(EntityType *, int);
int getShipType(EntityType *);
int locateShipID(EntityID *, int *, unsigned int *);
int getShipID(EntityID *, int *);

#endif
