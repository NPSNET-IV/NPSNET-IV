
#ifndef __NPS_PO_INTERFACE_H__
#define __NPS_PO_INTERFACE_H__

#include <Performer/pf.h>
// #include <gl/device.h>

const int NPS_MOUSEX = 0;
const int NPS_MOUSEY = 1;

const int NPS_LEFTMOUSE = 2;
const int NPS_MIDDLEMOUSE = 3;
const int NPS_RIGHTMOUSE = 4;

int initPoMeas ( const char *, unsigned char, unsigned char, unsigned short );
int updatePoMeas ();
int closePoMeas ();

int po_input_mode ();

int display_po_key ();

long get_valuator ( int );

int get_button ( int );

#endif

