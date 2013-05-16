#include <iostream.h>

#ifdef __cplusplus
extern "C" {
#endif

#define bool xbool
#include <curses.h>
#undef bool

#ifdef __cplusplus
}
#endif

#include <signal.h>
#include <stdlib.h>
#include <math.h>

#undef mips

#include "DI_comm.h"
#include "Mcast.h"

void test ();

WINDOW *the_win;
McastGroup mcg;
DIPacket toSig;

float speed = 0.0f;
float heading = 0.0f;
int state = 0;

const float HUMAN_TURN_DEGS = 1.0f;
const float DTOR = 0.017453289f;

void main ()
{
   void die (int);

   mcast_CreateSendGroup (ISMS_GROUP, ISMS_PACKET_PORT, &mcg);

   the_win = initscr ();

   crmode();
   noecho();
   clear();
   refresh();
   signal (SIGINT, die);

   move (0, 0);
//   standout ();
   addstr ("Heading:");
   move (1, 2);
   addstr ("Speed:");
   move (2, 2);
   addstr ("State:");
//   standend ();

   char a_char = 'c';
   while (a_char != 'Z') {
      a_char = getch ();
      if (a_char == char (27) ) {
         a_char = getch ();
         if (a_char == char (91) ) {
            a_char = getch ();
            switch (a_char) {
               case 'D': // Left arrow key
                  heading += HUMAN_TURN_DEGS;
                  break;
               case 'C': // Right arrow key
                  heading -= HUMAN_TURN_DEGS;
                  break;
               case 'A': // Up arrow key
                  break;
               case 'B': // Down arrow key
                  break;
            }
         }
      }
      if (a_char == 'a') {
         speed += 0.1f;
      }
      if (a_char == 'A') {
         speed += 3.0f;
      }
      if (a_char == 'd') {
         speed -= 0.1f;
      }
      if (a_char == 'f') {
         speed -= 3.0f;
      }
      if (a_char == 's') {
         speed = 0.0f;
      }
      if (a_char == 'j') {
         state = STANDING;
         move (2, 9);
         addstr ("STANDING");
      }
      if (a_char == 'k') {
         state = KNEELING;
         move (2, 9);
         addstr ("KNEELING");
      }
      if (a_char == 'l') {
         state = PRONE;
         move (2, 9);
         addstr ("PRONE   ");
      }
      while (heading >= 360.0f) {
         heading -= 360.0f;
      }
      while (heading < 0.0f) {
         heading += 360.0f;
      }
      move (0, 9);
      printw ("%f",heading);
      move (1, 9);
      printw ("%f",speed);
      refresh ();
      test ();
   }

   die (0);
}

void die (int)
{
   signal (SIGINT, SIG_IGN);

   mvcur (0, COLS - 1, LINES - 1, 0);
  
   endwin ();

   cerr << "Exiting Finger-Port v1.0..." << endl;

   exit (0);
}


void test ()
{
   toSig.position.hpr[0] = heading;
   toSig.velocity[0] = -speed * sinf (DTOR * heading);
   toSig.velocity[1] = speed * cosf (DTOR * heading);
   toSig.velocity[2] = 0.0f;
   toSig.status = state;
   mcast_Send (sizeof (DIPacket), (void *)&toSig, &mcg);
}
