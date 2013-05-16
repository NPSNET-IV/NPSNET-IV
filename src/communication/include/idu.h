// File: <idu.h>

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


/* Define the DIS version to be used */

#ifndef __NPS_IDU__
#define __NPS_IDU__

#include "pdu.h"

/* Define the largest IDU we will accept */

#define MAX_IDU_BYTES 256


/* IDU Header **************************************************************/

/* Versions */
#define IDUVersionMarch95	(u_char)1

/* Types */
typedef unsigned char	IDUType;

#define OtherIDU_Type			(IDUType)0
#define Test_Type			(IDUType)1
#define ViewpointControl_Type		(IDUType)2
#define SS_To_Jack_Type			(IDUType)101
#define Jack_To_SS_Type			(IDUType)102
#ifndef NOSUB
#define NPSNET_To_SUB_Type              (IDUType)103
#define SUB_Helm_To_NPSNET_Type         (IDUType)104
#define SUB_Ood_To_NPSNET_Type          (IDUType)105
#define SUB_Weaps_To_NPSNET_Type        (IDUType)106
#endif // NOSUB

#define NPSNET_To_SHIP_Type             (IDUType)107
#define SHIP_Ood_To_NPSNET_Type         (IDUType)108

typedef struct {
   unsigned char	version;
   IDUType		type;
   unsigned short	length;
} IDUHeader;

/* IDU Definitions *********************************************************/
typedef struct {
   char data[MAX_IDU_BYTES];
} LargestIDU;

typedef struct {
   LargestIDU idu;
   sender_info sender;
} InfoIDU;

/* Declare all possible IDU types here corresponding to the defines above */
typedef struct {
   IDUHeader header;
   char   string[128];
   int    intnum;
   float  floatnum;
} TestIDU;

typedef struct {
   IDUHeader	header;
   float	speed;
   float	body_heading;
   float	view_heading;
   float	view_pitch;
   u_long	posture;
   u_long	weapon;
   u_long	fire;
} SSToJackIDU;

typedef struct {
   IDUHeader    header;
   float	position_X;
   float	position_Y;
   float	position_Z;
   u_long	status;
} JackToSSIDU;

#ifndef NOSUB
typedef struct {
   IDUHeader    header;
   //the following field are used to communicate data from NPSNET to SUBCONTROL
  float sub_depth;    //actual ,0000
  float sub_course;   //actual ,000
  float sub_speed;    //actual ,00

  float sub_rudder_angle;    //actual ,00
  float sub_sternplns_angle; //actual ,00
  float sub_fwtrplns_angle;  //actual ,00

  float sub_ordered_depth;  // 0000
  float sub_ordered_course; // 000
  float sub_ordered_speed;  //-3 - 4 (back full - flank)
  float sub_helm_ordered_speed;  //-3 - 4 (back full - flank)

  float sub_embtblow_onoff;   //1 0
  float sub_mbtvents_openshut; //1 0

  float sub_scope_raiselower;  //1 -1
  float sub_scope_by;    //bearing that scope is looking at when mark is hit.
  float sub_scope_rh;    //range to object that scope is looking at when mark 
                         //is hit.

  float sub_selecttube_2or4; //1 0
  float sub_selecttube_1or3; //1 0
  float sub_selecttlam;      //1 - 6
  u_long space_holder;
  
} NPSNETToSubIDU;

typedef struct {
   IDUHeader    header;
   float helm_speed;  //-3 - 4 (back full - flank)
   float space_holder;
   float space_holder2;
   u_long space_holder3;
} HelmToNPSNETIDU;

typedef struct {
   IDUHeader    header;
   float ood_depth;   //ordered
  float ood_course;  //ordered
  float ood_speed;   //ordered

  float ood_embtblow_onoff;    //1 0
  float ood_mbtvents_openshut; //1 0

  float ood_scope_raiselower;  //1 -1
  float ood_scope_updown;      //1 -1
  float ood_scope_leftright;   //1 -1
  float ood_scope_mark;        //1

  u_long space_holder;
} SubOodToNPSNETIDU; 

//NEW
typedef struct {
   IDUHeader    header;
   float weaps_select_tube_2or4;  //1 0
  float weaps_select_tube_1or3;  //1 0

  float weaps_course_port;
  float weaps_shoot_port;
  float weaps_course_stbd;
  float weaps_shoot_stbd;
  float weaps_select_tlam;
  float weaps_shoot_tlam;

  u_long space_holder;
  u_long space_holder2;
  u_long space_holder3;
} WeapsToNPSNETIDU;
#endif // NOSUB

typedef struct {
   IDUHeader    header;
   //the following field are used to communicate data from NPSNET to SHIPCONTROL
  float ship_course;   //actual ,000
  float ship_speed;    //actual ,00
  float ship_rudder_angle;    //actual ,00

  float ship_ordered_rudder_angle; // 000
  float ship_ordered_rpm;          // 000 
  float ship_ordered_port_bell;  //-3 - 5 (back full - flank)
  float ship_ordered_stbd_bell;  //-3 - 5 (back full - flank)

  u_long space_holder;
} NPSNETToShipIDU;

typedef struct {
   IDUHeader    header;
   float ood_rudder_angle;  // ordered
   float ood_rpm;           // 000 
   float ood_port_bell;     //-3 - 5 (back full - flank)
   float ood_stbd_bell;     //-3 - 5 (back full - flank)
   u_long space_holder;
} OodToNPSNETIDU;

union IDU {
   TestIDU		Tidu;
   SSToJackIDU		SS2Jidu;
   JackToSSIDU		J2SSidu;
   LargestIDU           Lidu;
   InfoIDU		Iidu;
#ifndef NOSUB
   NPSNETToSubIDU       NPSNET2Subidu;
   HelmToNPSNETIDU      H2NPSNETidu;
   SubOodToNPSNETIDU    SUBO2NPSNETidu;
   WeapsToNPSNETIDU     W2NPSNETidu;
#endif // NOSUB
   NPSNETToShipIDU       NPSNET2Shpidu;
   OodToNPSNETIDU       O2NPSNETidu;
};

#endif
/* EOF */
