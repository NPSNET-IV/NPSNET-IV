// File: <netutil.h>

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
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *    npsnet-info@cs.nps.navy.mil
 *       Contact principle investigators.
 *       Overall research project information and funding.
 *       Requests for demonstations.
 *
 * Thank you to our sponsors:  ARL, STRICOM, TRAC, ARPA and DMSO.
 */


#ifndef __NPS_NETUTIL__
#define __NPS_NETUTIL__

#include <pf.h>

#include "constants.h"
#include "disnetlib.h"

// Flags uset to help the chaining
//Used entry
#define EMPTY -1
// a removed entity leaves on of these in the next block is used
#define AVAIL -2

//This is a default for this level
#define DEFAULT (unsigned char)0

struct vtypenode {  //the tree node stucture
     int vtype;
     char name[50];
     EntityType DIStype;
     vtypenode *next;
     vtypenode *child;
};
typedef struct vtypenode VTYPENODE;


//the type utilites
//int read_filter_file ( char *filterfilename );
int read_filter_file ();
int filterPDU ( PDUHeader *header );
int vehtype_from_net(EntityType *);
EntityType *get_nettype(int );
//void setup_type_table(char *);
void setup_type_table();

//These should never be called by the user
VTYPENODE *build_node_kind(EntityType,VTYPENODE *,int);
VTYPENODE *build_node_domain(EntityType,VTYPENODE *,int);
VTYPENODE *build_node_country(EntityType,VTYPENODE *,int);
VTYPENODE *build_node_category(EntityType,VTYPENODE *,int);
VTYPENODE *build_node_subcategory(EntityType,VTYPENODE *,int);
VTYPENODE *build_node_specific(EntityType,VTYPENODE *,int);
int find_entity_kind(EntityType *,VTYPENODE *);
int find_domain(EntityType *,VTYPENODE *);
int find_country(EntityType *,VTYPENODE *);
int find_category(EntityType *,VTYPENODE *);
int find_subcategory(EntityType *,VTYPENODE *);
int find_specific(EntityType *,VTYPENODE *);

VTYPENODE *new_VTYPENODE(int);

//the network id table
int vid_from_net(EntityID *,int *);
EntityID *get_netid(int );
void setup_entity_net_table();
int remove_vid_from_table(EntityID *);

typedef struct{  // the hash table type
    int vid;
    EntityID netid;
} VIDTABLETYPE;

const int VIDTABLETYPESIZE = 6741; //size of vehicle id hash table

//general purpose functions
void get_im_hosts();
void findsitehost(char *, char *,
                  SimulationAddress * );

int delta_send(pfCoord *, pfCoord *);

//Receive pdu handlers
void empty_net_buffers ( DIS_net_manager * );
int parse_net_pdus(DIS_net_manager *);  //read the network and parse the pdus 
int gotentity(EntityStatePDU *, sender_info &); //process an EntityStatePDU
#ifndef NOHIRESNET
int gotHiResDataPDU(DataPDU *, sender_info &); // process a DataPDU for HRhumans
#endif // NOHIRESNET
void gotfire(FirePDU *);          //process an FirePDU
void gotdetonate(DetonationPDU *); //process an DetonationPDU

//build a segment
void  buildseg(pfSeg *, DetonationPDU *);


#endif 
