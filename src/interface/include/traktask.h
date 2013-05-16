
/*File -*- c -*-
 *  $RCSfile: traktask.h,v $
 *  $Revision: 2.0 $
 *  $Author: bryant $, $State: Rel $, $Locker:  $
 *  Program:: I-Port
 *  Project:: ISMS
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

#ifndef __traktask_h
#define __traktask_h

/* Referenced System Header Files. */

/* Referenced Lab Header Files. */

#ifdef VW
#include <semLib.h>
#endif


/* Referenced Project Header Files. */

/* Referenced Local Header Files. */

/* Exported Definitions (for other that function-like macros). */

#define NUM_RECEIVERS 4
#define TRAK_HEAD 1
#define TRAK_RIFLE 2
#define TRAK_BODY 3
#define TRAK_PRIORITY 99


/* Exported Type Declarations. */

struct trak_whiteboard
{
  SEM_ID trak_mutex;
  float trak[NUM_RECEIVERS][7]; /* X, Y, Z, Quaternion for receivers */
};


/* Exported Function Prototypes. */

int trak_Initialize (void);
int trak_build (DIPacket *, DIBody *);
int trak_shutdown (int);
int trak_reconfigure (void);
void print_cal_data (DIPacket *);


/* Exported Definitions (for function-like macros). */
 
/* Exported Variable Declarations. */

extern int is_wise;
extern int body_trak;

#endif /* __traktask_h */
