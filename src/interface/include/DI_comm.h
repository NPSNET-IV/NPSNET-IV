/*File -*- c -*-
 *  $RCSfile: DI_comm.h,v $
 *  $Revision: 2.1 $
 *  $Author: moore $, $State: Exp $, $Locker:  $
 *  Program:: 
 *  Project:: 
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  (C) Copyright Sarcos Group/University of Utah Research Foundation, 1992
 *
 *  The contents of this file may be reproduced electronically, or in
 *  printed form, IN ITS ENTIRETY with no changes, providing that this
 *  copyright notice is intact and applicable in the copy. No other
 *  copying is permitted without the expressed written consent of the
 *  copyright holder.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Description::
 *    
 *
 *  Compile Flags::
 *    flag ; description.
 *
 *****************************************************************************/

#ifndef __DI_comm_h
#define __DI_comm_h

/* Referenced System Header Files. */

/* Referenced Lab Header Files. */

/* Referenced Project Header Files. */

#include "pdu.h"
#ifdef mips
#include <Performer/pf.h>
#endif


/* Referenced Local Header Files. */

/* Exported Definitions (for other that function-like macros). */

#define DIJOINTCOUNT   40

#define ISMS_GROUP     250
#define ISMS_PACKET_PORT      5150
#define ISMS_RESPONSE_PORT   5151


#define ISMS_DATA_MESS         103


#if 0
#define DI_ISMS_DATA_MESS          201

#define DI_JACK_CONNECT_MESS  401
#define DI_JACK_CALIBRATE_MESS 402
#define DI_JACK_DATA_MESS     403


#define JACK_DI_DATA_MESS        501
#define JACK_PORT_REQUEST_MESS   504
#endif


/* Message status flags. */

#define IPORT_TELEPORT_FL        0x00000001
#define PRONE              0x00030000
#define SYSFAIL_FL         0x00000100
#define ENT_RESURRECT_FL   0x00000400


#define FORCE_STOP_FL      0x00000002
#define ENT_DEAD_FL        0x00000020
#define NON_ISMS_FL        0x00000040
#define JACKNOENTITY_FL    0x00000080
#define OVERSEER           0x00000200
#define STANDING           0x00010000
#define KNEELING           0x00020000
#define US_DESERT_CAMMI    0x80000000
#define US_WOODLAND_CAMMI  0x40000000
#define IRAQI_GREY         0x20000000


/* Exported Type Declarations. */

#ifndef mips
typedef float pfVec3[3];

typedef struct
  {
  pfVec3 xyz;
  pfVec3 hpr;
  } pfCoord;
#endif

  
/*Type
 *{struct}
 *  Name:: DIRifle
 *  Description::
 *    This type defines a rifle. It contains both the position and the
 *    trigger information.
 *
 *  Fields::
 *    pfCoord position ; Position and orientation in space.
 *    int active ; If zero then the rifle is inactive.
 *    int fired ; Count of times trigger pulled.
 *    int secondary ; Count of times the secondary weapon was fired.
 */

typedef struct
  {
  pfCoord
    position;
  int
    active;
  int
    fired;
  int
    secondary;
  
  } DIRifle;

typedef enum
  {
  DIUpright = 0,
  DIDead = 3,
  DIProne = 2
  
  } DIPosture;

/*Type
 *{struct}
 *  Name:: ISMS_DATA_MESS_TYPE
 *  Description::
 *    This packet is sent by the ISMS process to the DI periodically
 *    in order to update the display. The information is first sent
 *    to Jack who fills in `body' with the new angles for the legs
 *    (or more based on posture). This packet also causes a new
 *    packet to be sent from the DI to the ISMS with new terrain
 *    information.
 *
 *  Fields::
 *    int type ; The type of the packet, ISMS_DATA_MESS.
 *    int entity ; The entity number.
 *    float jointlist[] ; Complete description of the body.
 *    DIRifle rifle ; Location and orientation of the rifle in world
 *                  coordinates.
 *    float velocity[3] ; Velocity in m/s in world coordinates.
 *    int jack_posture ; Posture information for Jack.
 *    unsigned int status ; Flag information defined above.
 */

typedef struct
  {
  int
    type;
  int
    entity;
  EntityType
    entityType;

  pfCoord
    gaze;

  pfCoord
    position;
  
  float
    jointlist[DIJOINTCOUNT];
			
  DIRifle
    rifle;

  pfVec3
    velocity;

  DIPosture
    posture;
  
  unsigned int
    status;

  } DIPacket;


/*Type
 *{struct}
 *  Name:: DIResponse
 *  Description::
 *    Contains information sent to the ISMS in response to the
 *    ISMS_DATA_MESS_TYPE message. This information is used to modify
 *    the behavior of the I-Port.
 *
 *  Fields::
 *    int length ; The length of the packet.
 *    int type ; The type of the message, DI_ISMS_DATA_MESS.
 *    int entity ; The entity number.
 *    float desired_position[3] ; X,Y,Z data only valid if the teleport
 *                                bit is set.
 *    float norm[3] ; The polygon norm at our location.
 *    float mobility ; Mobility coefficient, [0.0 - 1.0].
 *    float front_pnt[3] ; The point that we will collide with or that is
 *                         in front of us.
 *    float front_norm[3] ; The polygon norm at the front_pnt.
 *    unsigned int status ; Flags as defined above.
 */

typedef struct
  {
  int
    type;
  
  pfVec3
    desired_position;
  float
    altitude;
  pfVec3
    norm;

  float
    mobility;

  pfVec3
    front_pnt;
  pfVec3
    front_norm;

  unsigned int
    status;

  } DIResponse;


/* Exported Function Prototypes. */

/* Exported Definitions (for function-like macros). */
 
/* Exported Variable Declarations. */

#endif /* __DI_comm_h */



