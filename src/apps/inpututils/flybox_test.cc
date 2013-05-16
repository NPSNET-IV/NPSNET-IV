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
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <bstring.h>
#include <sys/schedctl.h>

#include "lv3.h"
#include "flybox.h"


/*
 * main()
 *
 * This is where everything happens
 *
 */

void main(int argc,char *argv[])
{
   flyboxClass *fb;

   if(argc < 2){
     printf("Usage %s <port name> [sample rate]\n",argv[0]);
     exit(0);
   }

   //open up the box;
   if(argc == 2)
     fb = new flyboxClass(argv[1]);
   else if(argc > 2)
     fb = new flyboxClass(argv[1],atoi(argv[2]));

   if(!fb->isvalid()){
     printf("Flybox set up failed port %s\n",argv[1]);
     exit(0);
  }

  //request the first set of data
  fb->fb_request();

  while(1) {

    if(fb->fb_read() ){
      printf("%6.2f %6.2f %6.2f %6.2f %6.2f ",
         fb->joyroll(),fb->joypitch(),fb->joytwist(),
          fb->leftlever(),fb->rightlever());
      fb->topleft()?printf("1"):printf("0");
      fb->topright()?printf("1"):printf("0");
      fb->midleft()?printf("1"):printf("0");
      fb->midright()?printf("1"):printf("0");
      fb->botleft()?printf("1"):printf("0");
      fb->botright()?printf("1"):printf("0");
      printf(" ");
      fb->momleft()?printf("1"):printf("0");
      fb->momright()?printf("1"):printf("0");
      printf(" ");
      fb->trigger()?printf("1"):printf("0");
      printf(" ");
      fb->momleftEdge()?printf("1"):printf("0");
      fb->momrightEdge()?printf("1"):printf("0");
      printf(" ");
      fb->triggerEdge()?printf("1\n"):printf("0\n");

      
      //set the deadzone
      fb->setdeadzone(fb->leftlever());

      if(fb->trigger()){
        printf("Trigger pulled, exit\n"); 
        //call it quits
        delete fb;
        exit(0);
      }
    }
    
    //request the data for the next go round
    fb->fb_request();
  }
}
