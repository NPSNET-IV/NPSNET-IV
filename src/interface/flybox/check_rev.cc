/*
 * Copyright 1994   BG Systems, Inc.
 * check_rev.c
 *
 * routine that checks the EPROM revision
 *
 * Author         Date       Comments
 * John Green     21-Oct-94  Author
 * John Green     01-Feb-95  Rev 3.0 release version.
 * John Green     09-Feb-95  Fixed input checking.
 * Dave Pratt     04-Jul-95  Modified for C++ compiling (Minor Changes)
 * Paul Barham    24-Jul-95  Changed C I/O to C++ I/O
 */

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lv3.h"

static char Cpy[] = "Copyright (c), BG Systems";
static char Cpyw[] = "Low level Flybox code Copyright (c), BG Systems";

int parse_year(char *);
void no_answer(void);

int check_rev(bglv *bgp)
{
   int st;
   int chars_read = 0;
   char str[64];

/*
 *  Send a "T" and see if the Box responds
 */
   st = write(bgp->sp_fd, "T", 2);
   sginap(100); 
   chars_read = read(bgp->sp_fd, str, 44);

/*
 *  If chars_read <= 0, looks like we have a Rev 1.x EPROM
 */
   if (chars_read <= 0)
   {
      no_answer();
      return(-1);
   }
   else
   {
/*
 *  Check the string length
 */
      if ( chars_read != 44 )
      {
         cerr << "Unexpected characters:  " <<  chars_read
              << " " <<  str << endl;
         return(-1);
      }
      else
      {
/*
 *  Check that it is the Copyright string
 */
         if ( strncmp(str, Cpy, strlen(Cpy)) != 0 )
         {
            cerr << "Unexpected characters:  " << chars_read 
                 << " " << str << endl;
            return(-1);
         }
         else
         {
/*
 *  If we go this far, we should have the right string 
 */
            bgp->Rev.year  = parse_year(str);
            bgp->Rev.major = str[38]-48;
            bgp->Rev.minor = str[40]-48;
            bgp->Rev.bug   = str[41]-48;
            bgp->Rev.alpha = str[42];
            cerr << Cpyw << " " << bgp->Rev.year << "Revision "
                 << bgp->Rev.major << "." << bgp->Rev.minor 
                 << bgp->Rev.bug << bgp->Rev.alpha << endl;
         }
      } 
   }
 
   return (bgp->Rev.major);
}

int parse_year(char *s)
{
   int i = 0;
   char yr[12];

   while ( *s != '1' )
      *s++;
   yr[i] = *s;
   while ( *s != ' ' && *s != ',' )
      yr[i++] = *s++;
   yr[i] = '\0';
   return(atoi(yr));
}

int check_setup(bglv *bgp)
{
   int i;
   int st = 0;

/*
 *  This routine checks the EPROM revision against the
 *  requested setup, and attempts to identify inconsistencies !
 */

   if ( bgp->Rev.major == 2 )
   {
      if ( bgp->analog_out != 0x0 )
      {
         cerr << "  Analog outputs not supported by LV816" << endl;
         st = -1;
      }
      if ( bgp->dig_out != 0x0 )
      {
         cerr << "  Digital outputs not supported by LV816" << endl;
         st = -2;
      }
      if ( bgp->dig_in & 0x40 )
      {
         cerr << "  Digital inputs 19-24 not supported by LV816" << endl;
         st = -3;
      }
   }
   else if ( bgp->Rev.major == 3 )
   {
      switch(bgp->Rev.alpha)
      {
       case 'e':
         cerr << "LV824-E" << endl;
         if ( bgp->analog_out != 0x0 )
         {
            cerr << "  Analog outputs not supported" << endl;
            st = -1;
         }
         if ( bgp->dig_out != 0x0 )
         {
            cerr << "  Digital outputs not supported" << endl;
            st = -2;
         }
         break;
       case 'f':
         cerr << "LV824-F" << endl;
         if ( bgp->analog_out != 0x0 )
         {
            cerr << "  Analog outputs not supported" << endl;
            st = -2;
         }
         break;
       case 'g':
         cerr << "LV824-G" << endl;
         break;
       default:
         st = -3;
         cerr << "Not an LV824 board" << endl;
         break;
      }
      if ( st < 0 )
         return(st);
/*
 *  Check also for conflict in the digital channels
 */

      if ( bgp->dig_in && bgp->dig_out )
      {
         for ( i = 0; i < 3; i++ )
         {
            if ( ( (bgp->dig_in >> i) &0x1 ) 
                 && ( (bgp->dig_out >> i) &0x1 ) )
            {

cerr << "Invalid set-up requested." << endl;
cerr << "  Digital input group " << i+1 << " AND output group "
     << i+1 << " selected." << endl;
cerr << endl << endl << "  Digital channels can be set in groups of 8 as"
     << endl;
cerr << "  either inputs or outputs." << endl;
cerr << "  Of course you can (for example) set the bottom 8" << endl;
cerr << "  to inputs DIC1 and the top 16 to outputs DOC2 | DOC3" << endl;

               st = -5;
               return(st);
            }
         }
      }
   }
   return(st);
}

void no_answer()
{
   cerr << endl << "Writing a 'T' to the Box produced no answer.  " << endl;
   cerr << endl;
   cerr << "The expected string was not returned from the BG box." << endl;
   cerr << "Here are some possible problems:" << endl;
   cerr << "   1. Check power to Box" << endl;
   cerr << "   2. Check the serial cable" << endl;
   cerr << "   3. Check the environment variable FBPORT" << endl;
   cerr << "      - does it match the connected serial port ?" << endl;
   cerr << "   4. Is the serial port configured as a terminal ?" << endl; 
   cerr << "      - if so use \"System Manager\" to disconnect the port" << endl;
   cerr << "   5. You have an old FlyBox (serial no. less than 60) " << endl;
   cerr << "         which has a revision 1.0 EPROM.  Call BG Systems.\n" << endl;

   cerr << endl << endl;
}
