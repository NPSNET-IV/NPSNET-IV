/*
 *
 *----------------------------------------------------------------------
 *
 *  Filename:
 *     lv3.h
 *
 *----------------------------------------------------------------------
 *        Copyright (C) 1994-5   BG Systems, Inc.
 *                  All rights reserved
 *----------------------------------------------------------------------
 *
 *  File description:
 *     Definitions for LV824 software
 *
 *  Modifications:
 *     Name        Date        Comment
 *     John Green  04-Jul-93   Created
 *     John Green  15-Oct-94   Major modification for Rev 3.0
 *     John Green  01-Feb-95   Cleanup for release
 *     John Green  16-Apr-95   Added DriveBox
 *     Dave Pratt  04-Jul-95   Added function prototypes
 *
 *----------------------------------------------------------------------
 */
#ifndef __LV_H
#define __LV_H

static char lv_SccsId[] = "@(#)lv3.h	1.4  17 Apr 1995";

#include <time.h>

#define FLYBOX    1
#define BEEBOX    2
#define CEREALBOX 3
#define CAB       4
#define DRIVEBOX  5

#define FB_NOBLOCK 1
#define FB_BLOCK 2

#define AIC1   0x01
#define AIC2   0x02
#define AIC3   0x04
#define AIC4   0x08
#define AIC5   0x10
#define AIC6   0x20
#define AIC7   0x40
#define AIC8   0x80

#define AOC1   0x01
#define AOC2   0x02
#define AOC3   0x04

#define DIC1    0x10
#define DIC2    0x20
#define DIC3    0x40

#define DOC1    0x10
#define DOC2    0x20
#define DOC3    0x40

#define BAUD576 0x70
#define BAUD384 0x60
#define BAUD192 0x50
#define BAUD96  0x40
#define BAUD48  0x30
#define BAUD24  0x20
#define BAUD12  0x10

#define OFFSET  0x21

/*
 *  Define some commands
 */

#define BURST       'B'    /* Burst mode                  */
#define BURST_SET   'b'    /* Burst mode rate set         */
#define CONT        'c'    /* Continuous buffered         */
#define DEFAULT     'd'    /* Reset to Default            */
#define PACKET      'p'    /* One input and one output    */
#define ONCE        'o'    /* One input                   */
#define ONCE_CS     'O'    /* One input with check sum    */
#define RESET_FB    'r'    /* Reset 3 chars with offset   */
#define RESET_FB_O  'R'    /* Reset (rev 2.2 no offset)   */
#define STOP        'S'    /* Stop burst mode             */
#define SETUP       's'    /* Setup rev 3.0 eprom         */
#define TEST1       'T'    /* Test (and copyright)        */
#define TEST2       't'    /* Test (and copy, and rev #)  */

typedef struct rs_struct
{
  int  wrt;     /* write error */
  int  rd;      /* read error  */
  int  len;     /* string length error  */
  int  nl;      /* last char error  */
  int  cycles;  /* numer of cycles */
  int  thou;    /* thousands of cycles */
} RS_ERR;

typedef struct REVISION
{
   int    major;          /*  Software major revision             */
   int    minor;          /*  Software minor revision             */
   int    bug;            /*  Software bug revision               */
   char   alpha;          /*  EPROM alpha revision                */
   int    year;
}revision;

/*
 *  For v3.0 software, define a new structure
 */
typedef struct BGLV_STRUCT
{
   int    n_analog_in;    /*  Number of analog inputs (8 max)     */
   int    analog_in;      /*  Analog input selector               */
   int    n_dig_in;       /*  Number of digital inputs (24 max)   */
   int    dig_in;         /*  Digital input selector              */
   int    n_analog_out;   /*  Number of analog outputs (3 max)    */
   int    analog_out;     /*  Analog out channel selector         */
   int    n_dig_out;      /*  Number of digital outputs (24 max)  */
   int    dig_out;        /*  Digital output selector             */
   float  ain[8];         /*  Analog input data                   */
   int    aout[3];        /*  Analog output data                  */
   int    din[3];         /*  Digital input data                  */
   int    dout[3];        /*  Digital output data                 */
   long   count;
   int    str_len;        /*  Length of string to expect          */
   int    baud;           /*  Baud rate selected                  */
   char   mode[2];        /*  Mode to send - rev 2.2              */
   time_t tag;    
   int    port;
   int    box_type;       /*  Device type                         */
   int    sp_fd;          /*  Serial port file descriptor         */
   revision   Rev;        /*  Software major revision             */
}bglv;

extern int open_lv(bglv *, char *, int );
extern int init_lv(bglv *);
extern void close_lv(bglv *);

extern int w_lv(int, char *);
extern int r_lv(bglv *);

extern int check_rev(bglv *);
extern int check_setup(bglv *);
#endif /* __LV_H */
