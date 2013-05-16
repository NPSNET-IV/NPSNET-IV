/*
 * flybox_test.c
 *
 * Main routine for v3.0 test for FlyBox
 *
 * Copyright 1995 BG Systems 
 *
 * Author       Date        Comments
 * John Green   01-Feb-95   Written
 * Dave Pratt     04-Jul-95  Modified for C++ compiling (Minor Changes)
 * Dave Pratt     04-Jul-95  Changed to support a flybox class
 * Paul Barham  13-Jul-95   Added abstract base class input device routines.
 *                          Changed C I/O to C++ I/O.
 * Paul Barham    24-Jul-95  Changed C I/O to C++ I/O
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <iostream.h>
#include <bstring.h>

#include "flybox.h"

//a good idea, but it took some hacking
flyboxClass *L_the_box;
void alarmfunc(int);

const FB_verify_times = 5;

int flyboxClass::fb_request()
{
  if( w_lv(bgdata.sp_fd, "o") < 0){
    return(0);
  }
  valid = 0;
  return(1);
}

int flyboxClass::fb_read(float analog[8] ,int digital[2])
{
  //wait for the alarm to read the data
  while(valid == 0) cerr << ".";
  cerr << endl;
  //sigpause(SIGALRM);

  if(r_lv(&bgdata) < 0){
    return(0);
  }
  
  //copy the data over
  bcopy(bgdata.ain,analog,sizeof(float)*8);
  bcopy(bgdata.din,digital,sizeof(int)*2);
  return(1);                
}

int flyboxClass::fb_read()
{
  //wait for the alarm to read the data
  //while(valid == 0);
  sigpause(SIGALRM);

  if(r_lv(&bgdata) < 0){
    return(0);
  }
  return(1);                
}

void flyboxClass::setdeadzone(float range)
{ 
  if (range < 0.0f) 
    deadzone = (0.0f-range);
  else 
    deadzone = range;
}


float addjustVal(float value,float zone)
//it is really hard to zero out the controls, so we can create a user defined
//dead zone that set the range to 0
//I am making the assumption that the values of the floats wiil be bounded 
//by FLY_MAX and FLY_MIN
{
  if (((value < 0)?(-value):value) < zone){ 
    //in the dead zone
    return(0.0f);
  }
  //it is out side the deadzone but we don't want to rather ramp it up
  if(value < 0.0f){
    return( (value + zone) /(FLY_MAX-zone));
  }
   return( (value - zone) /(FLY_MAX-zone));
}

float flyboxClass::joyroll()
{
  if(deadzone > 0.0){
    return (addjustVal(bgdata.ain[FB_JOYROLL],deadzone));
  }
  return (bgdata.ain[FB_JOYROLL]); 
}

float flyboxClass::joypitch()
{
  if(deadzone > 0.0){
    return (addjustVal(bgdata.ain[FB_JOYPITCH],deadzone));
  }
  return (bgdata.ain[FB_JOYPITCH]);
}

float flyboxClass::joytwist()
{
  if(deadzone > 0.0){
    return (addjustVal(bgdata.ain[FB_JOYTWIST],deadzone));
  }
  return (bgdata.ain[FB_JOYTWIST]);
}

float flyboxClass::leftlever()
{
  if(deadzone > 0.0){
    return (addjustVal(bgdata.ain[FB_LEFTLEVER],deadzone));
  }
  return (bgdata.ain[FB_LEFTLEVER]);
}
 
float flyboxClass::rightlever()
{
  if(deadzone > 0.0){
    return (addjustVal(bgdata.ain[FB_RIGHTLEVER],deadzone));
  }
  return (bgdata.ain[FB_RIGHTLEVER]);
}

int flyboxClass::momleftEdge()
{
  static int stat = 0;

  if((bgdata.din[0] & FB_MOMLEFT) && (stat == 0)){
     stat = 1; 
     return(1);
  }
  if((!(bgdata.din[0] & FB_MOMLEFT)) && (stat == 1))
    stat = 0;
  return(0);
}

int flyboxClass::momrightEdge()
{
  static int stat = 0;

  if((bgdata.din[0] & FB_MOMRIGHT) && (stat == 0)){
     stat = 1;
     return(1);
  }
  if((!(bgdata.din[0] & FB_MOMRIGHT)) && (stat == 1))
    stat = 0;
  return(0);
}

int flyboxClass::triggerEdge()
{
  static int stat = 0;

  if((bgdata.din[1] & FB_TRIGGER) && (stat == 0)){
     stat = 1;
     return(1);
  }
  if((!(bgdata.din[1] & FB_TRIGGER)) && (stat == 1))
    stat = 0;
  return(0);
}


flyboxClass::flyboxClass(const char *port,const int sampleRate)
{
   //assume everything is going to work
   valid = 1;   
   deadzone = 0.1f;
   
   //zero everything out
   bzero(&bgdata,sizeof(bgdata));
   
   //Defaults to 5 analog, and 16 discretes
   bgdata.analog_in = 0;
   bgdata.analog_in = AIC1 | AIC2 | AIC3 | AIC4 | AIC5;

   bgdata.dig_in = 0;
   bgdata.dig_in = DIC1 | DIC2;

   //Set the baud rate
   bgdata.baud    = BAUD192;
 
   //Open the port & drivers
   if ((open_lv(&bgdata, (char *)port, FB_NOBLOCK)) < 0)
   {
      cerr << "Unable to open port " << port << " for the Flybox."
           << endl;
      valid = 0;
      return;
   }
 
   //Send the init string
   if ( init_lv(&bgdata) < 0 )
   {
      check_setup(&bgdata);
      cerr << "Invalid Flybox setup requested. " << endl;
      valid = 0;
      return;
   }

   //set up the timer to handle the reading of the Flybox
   itv.it_interval.tv_sec = 0;
   itv.it_interval.tv_usec = (int)(100000.0f/sampleRate);
   itv.it_value = itv.it_interval;
   setitimer(ITIMER_REAL, &itv, (struct itimerval *)0 );

   //I was unable to get this to work right, so here is the brute force
   L_the_box = this;
   sigset(SIGALRM,  alarmfunc);
   
   //wait a little bit for the system calls to catch up
   sginap(2);
   //ask for the first set of data
   fb_request();
}

flyboxClass:: ~flyboxClass()
{
   // Cancel signal processing
   sigpause(SIGALRM);
   while ( (sigignore(SIGALRM)) != 0 );

   //Turn timer off
   itv.it_interval.tv_sec = 0;
   itv.it_interval.tv_usec = 0;
   itv.it_value = itv.it_interval;
   setitimer(ITIMER_REAL, &itv, (struct itimerval *)0 );

   //does not evaluate to anything
   close_lv(&bgdata);
}

void alarmfunc(int)
{
  //used to take care of the alarms
  L_the_box->setvalid(1);
}


void
flyboxClass::get_all_inputs ( pfChannel * )
{
/*
   if ( this->fb_request() ) {
      sginap(2);
      this->fb_read();
   }
*/
   //resequence the calls to avoid a 1/50 sec delay
   if(!fb_read() ){
      cerr << "Bad Flybox read, might cause funny behavior\n";
   }
   fb_request();
}


int
flyboxClass::button_pressed ( const NPS_BUTTON nps_button, int &num_presses )
{
   int success = FALSE;

   switch ( nps_button ) {
      case NPS_STCK_TRIGGER:
         num_presses = ( (this->triggerEdge() > 0) || 
                         (this->trigger() > 0) );
         success = TRUE;
         break;
      case NPS_MOMLEFT_BUTTON:
         num_presses = ( (this->momleftEdge() > 0) ||
                         (this->momleft() > 0) );
         success = TRUE;
         break;
      case NPS_MOMRIGHT_BUTTON:
         num_presses = ( (this->momrightEdge() > 0) ||
                         (this->momright() > 0) );
         success = TRUE;
         break;
      default:
         num_presses = 0;
         success = FALSE;
         break;
   }

   return success;
}


int
flyboxClass::valuator_set ( const NPS_VALUATOR nps_valuator, float &val )
{
   int success = FALSE;

   switch ( nps_valuator ) {
      case NPS_STCK_HORIZONTAL:
         val = this->joyroll();
         success = TRUE;
         break;
      case NPS_STCK_VERTICAL:
         val = this->joypitch();
         success = TRUE;
         break;
      case NPS_STCK_TWIST:
         val = this->joytwist();
         success = TRUE;
         break;
      case NPS_LEVER_LEFT:
         val = this->leftlever();
         success = TRUE;
         break;
      case NPS_LEVER_RIGHT:
         val = this->rightlever();
         success = TRUE;
         break;
      default:
         val = 0.0f;
         success = FALSE;
         break;
   }

   return success;
}


int
flyboxClass::switch_set ( const NPS_SWITCH nps_switch, NPS_SWITCH_VAL &val )
{
   int success = FALSE;

   switch ( nps_switch ) {
      case NPS_SWITCH_TOPLEFT:
         if ( this->topleft() ) {
            val = NPS_SWITCH_PRESSED;
         }
         else {
            val = NPS_SWITCH_RELEASED;
         }
         success = TRUE;
         break;
      case NPS_SWITCH_TOPRIGHT:
         if ( this->topright() ) {
            val = NPS_SWITCH_PRESSED;
         }
         else {
            val = NPS_SWITCH_RELEASED;
         }
         success = TRUE;
         break;
      case NPS_SWITCH_MIDLEFT:
         if ( this->midleft() ) {
            val = NPS_SWITCH_PRESSED;
         }
         else {
            val = NPS_SWITCH_RELEASED;
         }
         success = TRUE;
         break;
      case NPS_SWITCH_MIDRIGHT:
         if ( this->midright() ) {
            val = NPS_SWITCH_PRESSED;
         }
         else {
            val = NPS_SWITCH_RELEASED;
         }
         success = TRUE;
         break;
      case NPS_SWITCH_BOTLEFT:
         if ( this->botleft() ) {
            val = NPS_SWITCH_PRESSED;
         }
         else {
            val = NPS_SWITCH_RELEASED;
         }
         success = TRUE;
         break;
      case NPS_SWITCH_BOTRIGHT:
         if ( this->botright() ) {
            val = NPS_SWITCH_PRESSED;
         }
         else {
            val = NPS_SWITCH_RELEASED;
         }
         success = TRUE;
         break;
      default:
         val = NPS_SWITCH_RELEASED;
         success = FALSE;
         break;
   }

   return success;
}


int
flyboxClass::exists ()
{
   int done = FALSE;
   int count = 0;

   while ( (!done) && (count < FB_verify_times) ) {
      this->fb_request();
      sginap(2);
      done = this->fb_read();
      if ( !done ) {
         count++;
      }
   }

   return (count < FB_verify_times);
}

int
flyboxClass::calibrate ( const NPS_VALUATOR which_valuator, 
                         const NPS_CALIBRATION which_calibration,
                         const float value )
{
   int success = TRUE;

   switch ( which_valuator ) {
      case NPS_STCK:
         switch ( which_calibration ) {
            case NPS_DEAD_ZONE:
               success = ( (-1.0f <= value) && (value <= 1.0f) );
               if ( success ) {
                  this->setdeadzone(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      default:
         success = FALSE;
         break;
   }

   return success;
}

