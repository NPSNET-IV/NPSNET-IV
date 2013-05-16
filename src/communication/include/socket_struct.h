// File: <socket_struct.h>

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


//************************************************************************
//
//      /bossie/work2/mcmahan/thesis/panel/socket_struct.h
//
//************************************************************************

// This file describes the data to be passed from the interface
// to NPSNET and back.
//
// #define lines define the bit mask to apply to the BYTE types listed
// BYTE consists of 8 bits   x   x   x   x   x   x   x   x
// and the bit mask hex #s  40  20  10  08  04  02  01  00
//
// total size of structure as compiled is 80 bytes, with bytes
// 78 and 79 used as padding only

// The following illustrate how to set bit-masked flags.
// to turn tether mode on
// attachMode |= GUI_TETHER

// to turn tether mode off
// attachMode &= ~(GUI_TETHER);

#include "pdu.h"

#ifndef SOCKET_STRUCT_H
#define SOCKET_STRUCT_H

// mask for all BYTE variables
#define GUI_OFF	    0x00
#define GUI_ERROR   0x01

// attachMode
#define GUI_TETHER	0x02
#define GUI_ATTACH	0x04
#define GUI_TARGET	0x08
#define GUI_TELEPORT	0x10

// weaponsMode
#define GUI_PRIMARY	0x02
#define GUI_SECONDARY	0x04
#define GUI_TERTIARY	0x08
#define GUI_TARGETING	0x10
#define GUI_VIDEOMISSILE 0x20

// hudMode
#define GUI_HUD		0x02

// environmentMode
#define GUI_FOG		0x02
#define GUI_WIREFRAME	0x04
#define GUI_TEXTURE	0x08
#define GUI_CAMERA	0x10


typedef unsigned char BYTE;

typedef struct {
    // identify the type of data structure being passed
    // To be used for future expansion, should the need
    // arise for additional data structures

    double status;
    unsigned short type;
    unsigned short length;

    // throttle data required to read the throttle position
    // (-1.0 to 1.0) and to set the throttle input with the
    // scale widget

    float throttleSetting;

    // joystick data required to read the joystick position
    // and set input as required (each is between -1.0 and 1.0)

    float joystickX;
    float joystickY;

    // vehicle settings read from NPSNET only (not sent from interface)

    float positionX;
    float positionY;
    float positionZ;

    float altitude;
    float heading;
    float pitch;
    float roll;
    float velocity;

    float gunAzimuth;
    float gunElevation;

    EntityID vehicleID;

    // flag settings to execute NPSNET actions

    EntityID targetVehicleID;

    BYTE attachMode;  // including tether, attach, target,teleport

    BYTE weaponsMode; // including primary, secondary,tertiary,
                      // targetingEnable

    BYTE hudMode;     // including hudEnable

    BYTE environmentMode;  // including fogEnable,wireframeEnable,
			   // textureEnable, cameraEnable

    // variable settings to control NPSNET functions

    BYTE fogSetting;
    BYTE hudSetting;

} GUI_MSG_DATA;

#endif
