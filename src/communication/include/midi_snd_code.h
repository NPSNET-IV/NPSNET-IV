// File: <midi_snd_code.h>

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

#ifndef __NPS_MIDI_SND_CODES__
#define __NPS_MIDI_SND_CODES__

#define STOP_SEQUENCE     0xFC
#define START_SEQUENCE    0xFA
#define SONG_SELECT       0xF3
#define SONG_DIS          0x00
#define SONG_THEME        0x01
#define SONG_ON_MESSAGE   0x02
#define SONG_OFF_MESSAGE  0x03
#define SONG_DEAD_MESSAGE 0x04
#define PITCH_WHEEL_RANGE 0xE8   // was 0xE6

#define ALL_NOTES_OFF     0x7B

#define CHANGE_PRESET     0xCF
#define DEFAULT_LOAD_PRESET 0x08
// #define DEFAULT_LOAD_PRESET 0x0F
#define DEFAULT_PRESET    0x08
//#define DEFAULT_PRESET    0x03

/* Vehicle sounds */

#define GUN_MOVING_SOUND 0x41
#define TURRET_MOVING_SOUND 0x3c

/* Land Types */
#define V_DEF_LAND    0x30
#define V_TANK        0x30 /* Tank */
#define V_AFV         0x2D /* Armored Fighting Vehicle */
#define V_AUV         0x2D /* Armored Utility Vehicle */
#define V_SPA         0x2D /* Self-Propelled Artillery */
#define V_TA          0x2D /* Towed Artillery */
#define V_SWUV        0x2D /* Small Wheeled Utility Vehicle */
#define V_LWUV        0x2D /* Large Wheeled Utility Vehicle */
#define V_STUV        0x2D /* Small Tracked Utility Vehicle */
#define V_LTUV        0x2D /* Large Tracked Utility Vehicle */

/* Air Types */
#define V_DEF_AIR     0x2F
#define V_FIGHTER     0x2F 
#define V_ATTACK      0x2F
#define V_BOMBER      0x2F
#define V_CARGO       0x2F
#define V_ASW         0x2F /* Anti-sumbarine warfare */
// the following Electronic Warfare used to be 0x2F, 
// but was changed to support NPSNETIV.8
#define V_EW          0x2B /* Electronic warfare */
#define V_RECON       0x2F /* Reconnaissance */
#define V_SURV        0x2F /* Surveillance/c2 */
#define V_AH          0x2B /* Attack helicopter */
#define V_UH          0x2B /* Utility helicopter */
#define V_ASH         0x2B /* Anti-submarine helicopter */
#define V_CH          0x2B /* Cargo helicopter */
#define V_OH          0x2B /* Observation helicopter */
#define V_SO          0x2F /* Special operations */
#define V_TRAIN       0x2F /* Trainers */
#define V_UNMANNED    0x2F 

/* Surface Types */
#define V_DEF_SUR     0x35

/* Subsurface Types */
#define V_DEF_SUB     0x35

/* Space Types */
#define V_DEF_SPACE   0x35

/* Lifeform Types */
#define V_DEF_LF      0x35

#define F_LRG_GUN_0 0x48
#define F_LRG_GUN_1 0x4A
#define F_LRG_GUN_2 0x4C
#define F_LRG_GUN_3 0x4D
#define F_LRG_GUN_4 0x00
#define F_LRG_GUN_5 0x00
#define F_LRG_GUN_6 0x00
#define F_LRG_GUN_7 0x00
#define F_LRG_GUN_8 0x00
#define F_LRG_GUN_9 0x00

#define F_MED_GUN_0 0x35
#define F_MED_GUN_1 0x37
#define F_MED_GUN_2 0x00
#define F_MED_GUN_3 0x00
#define F_MED_GUN_4 0x00
#define F_MED_GUN_5 0x00
#define F_MED_GUN_6 0x00
#define F_MED_GUN_7 0x00
#define F_MED_GUN_8 0x00
#define F_MED_GUN_9 0x00

#define F_SML_GUN_0 0x30
#define F_SML_GUN_1 0x32
#define F_SML_GUN_2 0x34
#define F_SML_GUN_3 0x00
#define F_SML_GUN_4 0x00
#define F_SML_GUN_5 0x00
#define F_SML_GUN_6 0x00
#define F_SML_GUN_7 0x00
#define F_SML_GUN_8 0x00
#define F_SML_GUN_9 0x00

#define F_LRG_MISSLE_0 0x47
#define F_LRG_MISSLE_1 0x00
#define F_LRG_MISSLE_2 0x00
#define F_LRG_MISSLE_3 0x00
#define F_LRG_MISSLE_4 0x00

#define F_MED_MISSLE_0 0x45
#define F_MED_MISSLE_1 0x00
#define F_MED_MISSLE_2 0x00
#define F_MED_MISSLE_3 0x00
#define F_MED_MISSLE_5 0x00

#define F_SML_MISSLE_0 0x43
#define F_SML_MISSLE_1 0x00
#define F_SML_MISSLE_2 0x00
#define F_SML_MISSLE_3 0x00
#define F_SML_MISSLE_4 0x00

#define D_LRG_GUN_0 0x3E
#define D_LRG_GUN_1 0x39
#define D_LRG_GUN_2 0x3B
#define D_LRG_GUN_3 0x3C
#define D_LRG_GUN_4 0x40
#define D_LRG_GUN_5 0x41
#define D_LRG_GUN_6 0x00
#define D_LRG_GUN_7 0x00
#define D_LRG_GUN_8 0x00
#define D_LRG_GUN_9 0x00

#define D_MED_GUN_0 0x00
#define D_MED_GUN_1 0x00
#define D_MED_GUN_2 0x00
#define D_MED_GUN_3 0x00
#define D_MED_GUN_4 0x00
#define D_MED_GUN_5 0x00
#define D_MED_GUN_6 0x00
#define D_MED_GUN_7 0x00
#define D_MED_GUN_8 0x00
#define D_MED_GUN_9 0x00

#define D_SML_GUN_0 0x00
#define D_SML_GUN_1 0x00
#define D_SML_GUN_2 0x00
#define D_SML_GUN_3 0x00
#define D_SML_GUN_4 0x00
#define D_SML_GUN_5 0x00
#define D_SML_GUN_6 0x00
#define D_SML_GUN_7 0x00
#define D_SML_GUN_8 0x00
#define D_SML_GUN_9 0x00

#define D_LRG_MISSLE_0 0x00
#define D_LRG_MISSLE_1 0x00
#define D_LRG_MISSLE_2 0x00
#define D_LRG_MISSLE_3 0x00
#define D_LRG_MISSLE_4 0x00

#define D_MED_MISSLE_0 0x00
#define D_MED_MISSLE_1 0x00
#define D_MED_MISSLE_2 0x00
#define D_MED_MISSLE_3 0x00
#define D_MED_MISSLE_5 0x00

#define D_SML_MISSLE_0 0x00
#define D_SML_MISSLE_1 0x00
#define D_SML_MISSLE_2 0x00
#define D_SML_MISSLE_3 0x00
#define D_SML_MISSLE_4 0x00


#endif

