// File: <kaflight.cc>

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
/** File:  kaflight.cc **/

/* Source file for Kinney Aero Flight Control System, with non-split        */
/* Throttle,  written in C++.                                               */
/* Version: 1.0                                                             */
/* Programmer:  Paul T. Barham, Computer Specialist                         */
/*              Naval Postgraduate School, Computer Science Department      */
/*              Code CS - Barham, Monterey, CA 93943                        */
/*              E-mail:  barham@cs.nps.navy.mil                             */

/* Date:        Changes                                         Programmer: */
/* 7/9/93       Initial release.                                Barham      */
/* 7/25/95      Updated Kpoll_continuous to read into it's own  Barham      */
/*              temp buffer.  When the data is known to be                  */
/*              good, then it is copied into the class buffer2.             */
/*              When the consumer (app) is ready for data, it locks         */
/*              the datalock and copies buf2 into buf1.                     */

#include <iostream.h>/* For C++ standard I/O stuff                          */
#include <fstream.h> /* For C++ file I/O stuff                              */
#include <unistd.h>  /* For standard Unix read, write stuff                 */
#include <fcntl.h>   /* For file constant definitions and flags             */
#include <termio.h>  /* For terminal I/O stuff                              */
#include <termios.h> /* For terminal I/O stuff                              */
#include <stdlib.h>  /* Standard library stuff                              */
#include <sys/types.h> /* For system type stuff                             */
#include <sys/prctl.h> /* For process control stuff                         */
#include <sys/signal.h>/* For process signal stuff                          */
#include <sys/sysmp.h> /* For system multi-process stuff                    */
#include <bstring.h>

#include "kaflight.h" /* For the HOTAS C++ class definitions, etc.           */

#ifdef FIXCPLUSPLUS
extern "C" 
{
int ioctl ( int, int, termio* );
}
#endif


// Local defines

/* Define constants used for communication with the HOTAS.  Communications  */
/* format shown below constant definitions:                                 */

#define PACKET_SIZE 9 
#define MAX_POLL_RETRIES 1000
#define BELL (char) 7

#define HAT_MIDDLE 0
#define HAT_LEFT 1
#define HAT_RIGHT 2
#define HAT_DOWN 3
#define HAT_UP 4


// Static member variable initializations

int kaflight::num_kaflight = 0;

// Local prototypes

int Kopen_port ( const char *port_name, int &portfd );
int Kget_data ( int portfd, char *data );
void Ksend_data ( int portfd, char *data, int num_bytes );
void Kshow_data ( char *data, int num_bytes );
int Kget_packet ( int portfd, char *buffer, int &num_bytes );

// Local functions

/* Function open_port attempts to open the port indicated by port_name.     */
/* If the port cannot be opened or cannot be initialized after openning     */
/* then the function returns 0, otherwise if successful then returns 1 and  */
/* returns the port's file descriptor used by read and write in portfd.     */

int Kopen_port ( const char *port_name, int &portfd )
   {
   int success = 0;
   int initok = 0;
   struct termio term;

   /* Try to open the port for reading and writing.  Configure the port     */
   /* so that read will not wait if no data is present.                     */

   success = ((portfd = open ( port_name, O_RDWR|O_NOCTTY|O_NDELAY )) != -1);

   /* Try to configure the port.                                            */

   if (success)
      {

      memset ( &term, 0, sizeof(term) );

      term.c_cflag = B9600;  /* 9600 Baud rate */
      term.c_iflag = IGNBRK|IGNPAR;  /* Ignore break and parity */
      term.c_oflag = 0;
      /* Configure for HUPCL = hang up on last close, CS8 = 8 bits,         */
      /*               CLOCAL = local line, CREAD = enable receiver.        */
      term.c_cflag |= HUPCL | CS8 | CLOCAL | CREAD;
      term.c_lflag = 0;
      term.c_cc[VMIN] = 0;
      term.c_cc[VTIME] = 0;

      /* Initialize port with settings.                                     */
      initok = ioctl ( portfd, TCSETAF, &term );

      if ( initok < 0 )
         {
         /* If we can't initialize the port, then close it and fail.        */
         close ( portfd );
         success = 0;
         }

      }
   return success;
   } /* end open_port */


/* Function "get_data" reads characters from the port identified by portfd  */
/* into the character array data and returns the number of characters read. */

int Kget_data ( int portfd, char *data )
   {
   char buffer[1];
   int count = 0;
   char *current_byte;

   buffer[0] = 0;
   current_byte = data; /* Define pointer into current position in array    */

   /* As long as their are characters waiting, keep reading.                */

   while ( read ( portfd, buffer, 1 ) )
      {
      *current_byte++ = *buffer;
      count++;
      }

   return count;

   } /* end get_data */


/* Function "send_data" writes num_bytes characters from the character      */
/* array data to the port identified by portfd.  No error checking is done. */

void Ksend_data ( int portfd, char *data, int num_bytes )
   {
   write ( portfd, data, num_bytes );
   } /* end send_data */


/* Function "show_data" writes num_bytes characters from the character      */
/* array data to standard output in hexadecimal form.                       */

void Kshow_data ( char *data, int num_bytes )
   {
   cout << dec << num_bytes << " bytes (hex): " << hex;
   for ( int i = 0; i < num_bytes; i++ )
      cout << int(data[i]) << ".";
   cout << dec << endl;
   } /* end show_data */


/* Function "get_packet" attempts to read a standard 14 byte packet from    */
/* the port identified by portfd into the character array buffer.  The      */
/* parameter num_bytes returns the actual number of bytes read.  The        */
/* function returns 1 if the data read is in the correct format and 0 if    */
/* the data should not be used.                                             */

int Kget_packet ( int portfd, char *buffer, int &num_bytes )
   {
   char query[] = { 0, 1, 2, 3, 4, 5, 6, 7, 7 };
   int numchannels = sizeof(query);
   char *channel = query;
   char *current;
   int num_this_time;
   int count = 0;
   int retries;
   int done;


   /* Signal the controller by sending request for channel 0                */
   Ksend_data ( portfd, channel, 1 );

   current = buffer; /* Set up a pointer into the character buffer          */
   done = 0;
   retries = 0;
   num_bytes = 0;

   /* Constantly read the port for data until a large enough packet is      */
   /* received or until we have tried reading with no success a given       */
   /* number of times.                                                      */

   while ( (!done) && (retries < MAX_POLL_RETRIES) )
      {

      /* Read all the characters that are in the ports input buffer         */
      num_this_time = Kget_data ( portfd, current );

      /* Request next channel's information until all channels have been    */
      /* requested.                                                         */
      if ( ++count < numchannels )
         Ksend_data ( portfd, ++channel, 1 );

      if ( num_this_time == 0 )
         retries++;   /* Nothing was there */
      else
         { /* Something there, add it to what we have already               */
         retries = 0;
         num_bytes += num_this_time;
         current += num_this_time;

         /* See if we have enough to stop and if the stop signal has been   */
         /* received in the correct position.                               */
         done = (num_bytes >= numchannels);
         }

      } /* end while */
   return done;

   } /* end get_packet */


/* Function "read_file" reads initialization file, if one exists, for the   */
/* initial values of the device valuator components.                        */
/* The function returns 1 if the file can be opened, 0 otherwise.           */
/* The function performs no error checking on the file format.              */

int
kaflight::read_file ()
   {
   int i;

   /* Attempt to open the default file.                                     */
   ifstream fcs_file ( KFILENAME );

   if (!fcs_file) return 0; /* Could not be opened */
   else
      { /* file opened, read values */
      fcs_file >> flip_roll >> rollinfo.min >> rollinfo.mid >> rollinfo.max;
      fcs_file >> flip_pitch >> pitchinfo.min >> pitchinfo.mid >> pitchinfo.max;
      fcs_file >> lthrottle.min >> lthrottle.max;
      fcs_file >> rthrottle.min >> rthrottle.max;
      for ( i = 0; i < 4; i++ )
         fcs_file >> t_buttons[i];
      for ( i = 0; i < 9; i++ )
         fcs_file >> t_switches[i];
      for ( i = 0; i < 16; i++ )
         fcs_file >> s_buttons[i];
      for ( i = 0; i < 5; i++ )
         fcs_file >> s_hat[i];
      fcs_file.close();
      return 1;
      }

   } /* end read_file */


/* Function "write_file" writes the initialization file.  The function      */
/* returns 1 if the file was created or 0 if it cannot create the file.     */

int
kaflight::write_file ()
   {
   int i;
   ofstream fcs_file ( KFILENAME, ios::out );
   if (!fcs_file)
      {
      cerr << BELL << "Could not create fcs data file." << endl;
      return 0;
      }
   else
      {
      fcs_file << flip_roll << " " << rollinfo.min << " " 
               << rollinfo.mid << " " << rollinfo.max << endl;
      fcs_file << flip_pitch << " " << pitchinfo.min << " "
               << pitchinfo.mid << " " << pitchinfo.max << endl;
      fcs_file << lthrottle.min << " " << lthrottle.max << endl;
      fcs_file << rthrottle.min << " " << rthrottle.max << endl;
      for ( i = 0; i < 4; i++ )
         fcs_file << t_buttons[i] << " ";
      fcs_file << endl;
      for ( i = 0; i < 9; i++ )
         fcs_file << t_switches[i] << " ";
      fcs_file << endl;
      for ( i = 0; i < 16; i++ )
         fcs_file << s_buttons[i] << " ";
      fcs_file << endl;
      for ( i = 0; i < 5; i++ )
         fcs_file << s_hat[i] << " ";
      fcs_file << endl;
      fcs_file.close();
      return 1;
      }
   } /* end write_file */


// Class definition


/* Function "fcs" is the constructor for the fcs class.  The function       */
/* attempts to open the communications port specified by port_name.  If the */
/* port is opened successfully, the function sets the valid flag to 1 and   */
/* initializes all appropriate member variables.                            */

kaflight::kaflight ( const char *port_name, int prompt, int multi )
   {
   int stop = 0;
   char key;
   int use_file = 0;
   int numcpus;
   int file_exists;

   /* Try to open the port and communicate with HOTAS */

   if ( (valid = Kopen_port ( port_name, portfd )) )
      {
      tcflush ( portfd, TCIOFLUSH );
      valid = poll(10);
      }
   else
      {
      cerr << BELL << "Cannot open/initialize port: " << port_name << endl;
      cerr << "Probably due to incorrect read/write permissions." << endl;
      }


   if (valid)
      { /* port was opened */

       /* Initialize member variables to their default states.              */
      data = buffer1;
      hasdata = 0;
      is_reading = 1;
      fcspid = -1;
      killflag = 0;
      rollinfo.deaden = 0.0;
      pitchinfo.deaden = 0.0;
      lthrottle.deaden = 0.0;
      rthrottle.deaden = 0.0;
      rollinfo.sensitive = 1.0;
      pitchinfo.sensitive = 1.0;
      lthrottle.sensitive = 1.0;
      rthrottle.sensitive = 1.0;
      flip_pitch = 0;
      flip_roll = 0;
      if ( file_exists = read_file() )
         { /* Initialization file exists, see if user wants to use it       */
         if ( prompt )
            {
            cout << "An KAflight initialization file exists." << endl;
            cout << "Do you want to use that file? (y or n): " << endl;
            cin.get(key);
            cin.get();
            if ( ( key == 'y' ) || ( key == 'Y') )
               use_file = 1;
            }
         else
            {
#ifdef DEBUG
            cout << "Using KAflight values from existing file:  " << KFILENAME
                 << endl;
#endif
            use_file = 1;
            }
         } /* end if read_file */

      /* If the user does not want to use the file or if one does not exist,*/
      /* then have the user initialize the device.                          */
      if ( !use_file )
          {
          stop = !initialize(ALL);
          /* If we are creating the file for the first time, set the file   */
          /* permission to read and write for everyone.                     */
          if ((!stop) && (!file_exists))
             system ( KMAKEREADFILE );
          }
      else
         {
         /* Initialize the ranges for valuators if file is used.            */
         deaden_roll ( rollinfo.deaden );
         deaden_pitch ( pitchinfo.deaden );
         deaden_lthrottle ( lthrottle.deaden );
         deaden_rthrottle ( rthrottle.deaden );
         }

      if (!stop)
         {

         /* See how many processors this machine has                        */
         numcpus = sysmp ( MP_NAPROCS );

         /* If this machine has more than one processor and the multi       */
         /* parameter is set, then create the locks and semaphors needed    */
         /* and sproc the FCS read process off to another processor as a    */
         /* lightweight thread.                                             */

         if ( numcpus > 1 && multi )
            {
            char arena_name[255];

            /* Create an arena file to get the locka and semaphore from     */
            sprintf ( arena_name, "%s.%d.%d", 
                      KARENA_FILE, getpid(), num_kaflight );
            usconfig(CONF_ARENATYPE, US_SHAREDONLY);
            arena = usinit ( arena_name );
            if ( arena == NULL )
                {
                cerr << BELL << "Could not use arena file:  " << KARENA_FILE 
                     << endl;
                valid = 0;
                }

            else
               {
               /* Set up the arean file with read and write permissions     */
               /* for everyone.                                             */
               usconfig ( CONF_CHMOD, arena, KARENA_PERMISSIONS);

               /* Create a lock to provide mutual exclusion to the current  */
               /* data buffer being used for producing and consuming.       */

               datalock = usnewlock ( arena );
               if ( datalock == NULL )
                  {
                  cerr << BELL << "Could not obtain data lock from arena." 
                       << endl;
                  usinitlock ( datalock );
                  valid = 0;
                  }

               /* Create a semaphore for suspending the producer code while */
               /* the producer re-initializes the device.                   */

               resetsema = usnewsema ( arena, 1 );
               if ( resetsema == NULL )
                  {
                  cerr << BELL << "Could not obtain reset semaphore from arena."
                       << endl;
                  valid = 0;
                  }

               /* Start the producer code as a lightweight thread who       */
               /* shares all data space with this consumer process.         */
 
               if ( valid )
                  {
                  fcspid = sproc ( Kpoll_continuous, PR_SALL, (void *)this );
                  if ( fcspid == -1 )
                     {
                     perror("Could not start KAflight read process -\n   ");
                     valid = 0;
                     }
                  else
                     {
                     signal(SIGCLD,SIG_IGN);
#ifdef PROCESSTRACE 
                     cout << "KAflight read process spawned:  " << fcspid 
                          << endl;
#endif
                     }
                  }
               }
            }
#ifdef DEBUG
         else
            cout << "KAflight read process part of main process." << endl;
#endif
         }

      } /* end if valid */
   else
      cerr << BELL << "KAflight not responding on port " << port_name << endl; 

   } /* end fcs */


void
kaflight::close_kaflight()
   {
   if (valid)
      {

      /* If the producer was started as a lightweight thread, then signal   */
      /* the producer process to die.  Free the lock and semaphore.         */

      if ( fcspid != -1 )
         {
         killflag = 1;
         if (!is_reading)
            usvsema ( resetsema );
         while (killflag);
         sleep(1);
         fcspid = -1;
         usfreelock ( datalock, arena );
         usfreesema ( resetsema, arena );
         }

      if ( arena != NULL ) {
         usdetach(arena);
         arena = NULL;
      }

      /* Flush all characters from the FCS port and then close it.          */
      tcflush ( portfd, TCIOFLUSH );
      close ( portfd );
      valid = 0;
      }
   } /* end close_fcs */


/* Function "~fcs" is the destructor for the fcs class.  The function       */
/* closes the port previously opened by the constructor (if any).           */

kaflight::~kaflight ()
   {
   close_kaflight ();
   } /* end ~fcs */


/* Function "poll_continuous" is the producer code that reads the FCS and   */
/* stores the data when there are more than one processors on the machine   */
/* and the multi parameter was set for the constructor.                     */
/* This process runs until the killflag is set by the consumer.             */

void
Kpoll_continuous ( void *tempthis )
   {
   int tries = 2;
   int done = 0;
   int count = 0;
   int num_read;
   char temp_buffer[KBUFF_SIZE];
   kaflight *that;
   int good_data;

   that = (kaflight *) tempthis;

   signal ( SIGTERM, SIG_DFL );

   while ( !that->killflag ) {

   done = 0;
   count = 0;

   while (( count < tries ) && ( !done ))
      {

      /* Acquire the semaphore before reading data from the FCS.  This      */
      /* guarantees that the consumer cannot communicate with the FCS at    */
      /* same time via a call to initialize.                                */

      if ( uspsema ( that->resetsema ) == -1 )
         cerr << BELL << "Error reading reset semaphore." << endl; 
      bzero ( (char *)temp_buffer, KBUFF_SIZE);
      num_read = 0;
      good_data = Kget_packet ( that->portfd, temp_buffer, num_read );
      usvsema ( that->resetsema );

      if ( good_data ) 
         {

         /* If a valid packet is received and is expected size then we are  */
         /* done and can toggle to the valid data with the data pointer.    */
         if (num_read == PACKET_SIZE)
            {
            ussetlock(that->datalock);
            bcopy ( (char *)temp_buffer, (char *)that->buffer2,
                    KBUFF_SIZE );
            usunsetlock(that->datalock);
            that->hasdata = 1;
            done = 1;
            }

         /* Otherwise, the read failed.  This would mean that > 14 bytes    */
         /* were received and the last character happened to be FF.         */
         else
            count++;
         }

      /* Read failed.  Advance number of tries.                             */
      else
         {
#ifdef DEBUG
         cout << "Bad packet received." << endl;
         Kshow_data ( temp_buffer, num_read );
#endif
         count++;
         }
  
      } /* end while */

   } /* end while infinite loop */

#ifdef DEBUG
   cout << "KAflight read process terminating." << endl;
#endif
   that->killflag = 0;
/*
   while (!that->killflag);
*/
   exit(0);

 } /* end poll_continous */


/* Function "poll" prompts the device to send its current status and then   */
/* waits for a response.  The function tries to get a valid packet for up   */
/* to the number of times indicated by parameter tries.  If after all tries */
/* a valid packet is not received, the function returns 0.  Otherwise, if a */
/* valid packet is received, the function returns 1.                        */

int
kaflight::poll ( int tries )
   {
   int num_read;
   char *curr_buffer = buffer2;
   int count = 0;
   int done = 0;

   /* Force tries to be positive */
   if ( tries < 0 ) 
      tries = 1;

   /* Try to get a valid packet until one is recieved or we have tried      */
   /* without success tries times.                                          */

   while (( count < tries ) && ( !done )) 
      {

      if ( Kget_packet ( portfd, curr_buffer, num_read ) )
         {

         /* If a valid packet is received and is expected size then we are  */
         /* done and can toggle to the valid data with the data pointer.    */
         if (num_read == PACKET_SIZE)
            {
            bcopy ( (char *)buffer2, (char *)buffer1, KBUFF_SIZE );
            done = 1;
            }

         /* Otherwise, the read failed.  This would mean that > 14 bytes    */
         /* were received and the last character happened to be FF.         */
         else
            count++;
         }

      /* Read failed.  Advance number of tries.                             */
      else
         count++;

      } /* end while */

   return done;

   } /* end poll */


/* Function "new_data" returns true if there is new data since the last     */
/* call to "clear_data".  If the FCS read process is running in parallel    */
/* then we check the hasdata flag.  If the read process is part of this     */
/* process, then we poll the FCS device for data and see if it returns any. */

int
kaflight::new_data ()
   {
   if ( (is_reading) && (fcspid == -1) )
      {
      hasdata = poll(2);
      if (!hasdata)
         cerr << BELL << "FCS not responding..." << endl;
      }
   else if (!is_reading)
      hasdata = 0;
   return hasdata;
   }


/* Function "get_data" guarantees that the consumer and the producer are    */
/* using different buffers.                                                 */

void
kaflight::get_data ()
   {
   if ( fcspid != -1 )
      ussetlock ( datalock );
      bcopy ( (char *)buffer2, (char *)buffer1, KBUFF_SIZE );
      hasdata = 0;
      usunsetlock ( datalock );
   }


/* Function "clear_data" clears the hasdata flag indicating old data is in  */
/* the current buffer.                                                      */

void
kaflight::clear_data ()
   {
   /* No op */
   }

void
kaflight::init_resting ( int &stop, int prompt )
   {
   char keys[5];

   if ( !stop )
      {
      if (prompt)
         {
         cout << "Release the stick to the center and press enter:";
         cout << endl;
         cin.getline ( keys, 4 );
         }
      poll();
      if ( poll() )
         {
         pitchinfo.mid = fcsdata(data[5]);
         cout << "Info read - Pitch:  " << pitchinfo.mid << endl;
         rollinfo.mid = fcsdata(data[3]);
         cout << "Info read - Roll:  " << rollinfo.mid << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing resting stick position */
   }

void
kaflight::init_stick ( int &stop )
   {
   char keys[5];
   fcsdata temp;

   if ( !stop )
      {
      cout << "Move the stick to the extreme left and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         rollinfo.min = fcsdata(data[3]);
         cout << "Info read - Roll Minimum:  " << rollinfo.min << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         } /* end processing forward, left corner */
      }

   if ( !stop )
      {
      cout << "Move the stick to the extreme forward position and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         pitchinfo.max = fcsdata(data[5]);
         cout << "Info read - Pitch Maximum:  " << pitchinfo.max << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing forward, right corner */

   if ( !stop )
      {
      cout << "Move the stick to the extreme right and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         rollinfo.max = fcsdata(data[3]);
         cout << "Info read - Roll Maximum:  " << rollinfo.max << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing rear, left corner */

   if ( !stop )
      {
      cout << "Move the stick to the extreme rear position and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         pitchinfo.min = fcsdata(data[5]);
         cout << "Info read - Pitch Minimum:  " << pitchinfo.min << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing rear, right corner */

   if ( pitchinfo.max < pitchinfo.min )
      {
      flip_pitch = 1;
      temp = pitchinfo.max;
      pitchinfo.max = pitchinfo.min;
      pitchinfo.min = temp;
      }
   if ( rollinfo.max < rollinfo.min )
      {
      flip_roll = 1;
      temp = rollinfo.max;
      rollinfo.max = rollinfo.min;
      rollinfo.min = temp;
      }

   }

void
kaflight::init_throttle ( int &stop )
   {
   char keys[5];

   if ( !stop )
      {
      cout << "Move throttle(s) all the way forward and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         lthrottle.max = fcsdata(data[4]);
         cout << "Info read - Left Throttle:  " << lthrottle.max << endl;
         rthrottle.max = fcsdata(data[4]);
         cout << "Info read - Right Throttle:  " << rthrottle.max << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing forward throttle */

   if ( !stop )
      {
      cout << "Move throttle(s) all the way backward and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         lthrottle.min = fcsdata(data[4]);
         cout << "Info read - Left Throttle:  " << lthrottle.min << endl;
         rthrottle.min = fcsdata(data[4]);
         cout << "Info read - Right Throttle:  " << rthrottle.min << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end processing backward throttle */
   }


void
kaflight::init_button_singles ( int &stop )
   {
   char keys[5];

   if ( !stop )
      {
      cout << "Release all buttons and trigger on stick and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[0] = fcsdata(data[1]);
         cout << "Info read - Stick, no buttons pressed:  " 
              << s_buttons[0] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick trigger by itself and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[1] = fcsdata(data[1]);
         cout << "Info read - Trigger alone:  " << s_buttons[1] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press top stick button by itself and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[2] = fcsdata(data[1]);
         cout << "Info read - Top button alone:  " << s_buttons[2] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press middle stick button by itself and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[4] = fcsdata(data[1]);
         cout << "Info read - Middle button alone:  " << s_buttons[4] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press bottom stick button by itself and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[8] = fcsdata(data[1]);
         cout << "Info read - Bottom button alone:  " << s_buttons[8] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Release stick hat to center and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_hat[HAT_MIDDLE] = fcsdata(data[2]);
         cout << "Info read - Hat centered:  " << s_hat[HAT_MIDDLE] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Place stick hat to right and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_hat[HAT_RIGHT] = fcsdata(data[2]);
         cout << "Info read - Hat to right:  " << s_hat[HAT_RIGHT] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Place stick hat to top and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_hat[HAT_UP] = fcsdata(data[2]);
         cout << "Info read - Hat to top:  " << s_hat[HAT_UP] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Place stick hat to left and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_hat[HAT_LEFT] = fcsdata(data[2]);
         cout << "Info read - Hat to left:  " << s_hat[HAT_LEFT] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Place stick hat to bottom and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_hat[HAT_DOWN] = fcsdata(data[2]);
         cout << "Info read - Hat to bottom:  " << s_hat[HAT_DOWN] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Release both buttons on throttle and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_buttons[0] = fcsdata(data[7]);
         cout << "Info read - Throttle, no buttons pressed:  "
              << t_buttons[0] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press left button on throttle only and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_buttons[1] = fcsdata(data[7]);
         cout << "Info read - Throttle, left button pressed:  "
              << t_buttons[1] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press right button on throttle only and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_buttons[2] = fcsdata(data[7]);
         cout << "Info read - Throttle, right button pressed:  "
              << t_buttons[2] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Release both switches on throttle and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_switches[0] = fcsdata(data[8]);
         cout << "Info read - Throttle, switches at rest:  "
              << t_switches[0] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press top switch on throttle forward alone and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_switches[1] = fcsdata(data[8]);
         cout << "Info read - Throttle, top switch forward position:  "
              << t_switches[1] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press top switch on throttle backward alone and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_switches[2] = fcsdata(data[8]);
         cout << "Info read - Throttle, top switch backward position:  "
              << t_switches[2] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press bottom switch on throttle forward alone and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_switches[3] = fcsdata(data[8]);
         cout << "Info read - Throttle, bottom switch forward position:  "
              << t_switches[3] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press bottom switch on throttle backward alone and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_switches[4] = fcsdata(data[8]);
         cout << "Info read - Throttle, bottom switch backward position:  "
              << t_switches[4] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   }


void
kaflight::init_button_combo ( int &stop )
   {
   char keys[5];
 
   if ( !stop )
      {
      cout << "Press stick trigger & top button and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[3] = fcsdata(data[1]);
         cout << "Info read - Stick trigger with top button:  " 
              << s_buttons[3] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick trigger & middle button and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[5] = fcsdata(data[1]);
         cout << "Info read - Stick trigger with middle button:  "
              << s_buttons[5] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick trigger & bottom button and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[9] = fcsdata(data[1]);
         cout << "Info read - Stick trigger with bottom button:  "
              << s_buttons[9] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick top & middle buttons and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[6] = fcsdata(data[1]);
         cout << "Info read - Stick top & middle buttons:  "
              << s_buttons[6] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick top & bottom buttons and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[10] = fcsdata(data[1]);
         cout << "Info read - Stick top & bottom buttons:  "
              << s_buttons[10] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick middle & bottom buttons and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[12] = fcsdata(data[1]);
         cout << "Info read - Stick middle & bottom buttons:  "
              << s_buttons[12] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick trigger, top & middle buttons and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[7] = fcsdata(data[1]);
         cout << "Info read - Stick trigger, top & middle buttons:  "
              << s_buttons[7] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick trigger, top & bottom buttons and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[11] = fcsdata(data[1]);
         cout << "Info read - Stick trigger, top & bottom buttons:  "
              << s_buttons[11] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick trigger, middle & bottom buttons and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[13] = fcsdata(data[1]);
         cout << "Info read - Stick trigger, middle & bottom buttons:  "
              << s_buttons[13] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick top, middle & bottom buttons and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[14] = fcsdata(data[1]);
         cout << "Info read - Stick top, middle & bottom buttons:  "
              << s_buttons[14] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press stick trigger & all buttons and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         s_buttons[15] = fcsdata(data[1]);
         cout << "Info read - Stick trigger and all buttons:  "
              << s_buttons[15] << endl;
         }
      else
         {
         cerr << BELL << "Stick not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press both throttle buttons and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_buttons[3] = fcsdata(data[7]);
         cout << "Info read - Throttle both buttons:  "
              << t_buttons[3] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press both switches on throttle forward and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_switches[7] = fcsdata(data[8]);
         cout << "Info read - Throttle both switches forward position:  "
              << t_switches[7] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press both switches on throttle backward and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_switches[8] = fcsdata(data[8]);
         cout << "Info read - Throttle both switches backward position:  "
              << t_switches[8] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press top throttle switch forward, and bottom throttle "
           << "switch backward and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_switches[6] = fcsdata(data[8]);
         cout << "Info read - Throttle top switch forward, bottom switch "
              << "backward position:  " << t_switches[6] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   if ( !stop )
      {
      cout << "Press top throttle switch backward, and bottom throttle "
           << "switch forward and press enter:";
      cout << endl;
      cin.getline ( keys, 4 );
      poll();
      if ( poll() )
         {
         t_switches[5] = fcsdata(data[8]);
         cout << "Info read - Throttle top switch backward, bottom switch "
              << "forward position:  " << t_switches[5] << endl;
         }
      else
         {
         cerr << BELL << "Throttle not responding.  Stopping initialization."
              << endl;
         stop = 1;
         }
      } /* end */

   }


/* Function "initialize" instructs the user to position the stick and       */
/* throttle so their default boundary values can be read.  These values are */
/* then placed in the newly created initialization file.  All relevant      */
/* device data is initialized to their default settings.                    */

int
kaflight::initialize ( int item, int prompt )
   {
   int stop = 0;
   int sema_set;

   stop = !is_reading;

   if ( stop )
      cerr << BELL << "Initialization attempted on suspended KAflight." << endl;

   /* If user does not want to use the initialization file or if one does   */
   /* does not exists, then prompt user to position ball for reading of     */
   /* default extreme values.                                               */

   /* Grab the semaphore before proceeding to guarantee the producer is not */
   /* trying to read from the FCS at the same time when the producer is     */
   /* running in parallel.                                                  */

   if ( (!stop) && (fcspid != -1) )
      {
      sema_set = (uspsema ( resetsema ) != -1); 
      if (!sema_set)
         cerr << BELL << "Could not read reset KAflight semaphore." << endl;
      }
   else
      sema_set = 0;

   if (( item == RESTING ) || ( item == STICK ) || ( item == STICKTHROTTLE ) ||
       ( item == ALL ) )
      init_resting ( stop, prompt );

   if (( item == STICK ) || ( item == STICKTHROTTLE ) || ( item == ALL ))
      init_stick ( stop );

   if (( item == THROTTLE ) || ( item == STICKTHROTTLE ) || ( item == ALL ))
      init_throttle ( stop );

   if (( item == BUTTONSSINGLE) || ( item == ALL ) )
      init_button_singles ( stop );
   
   if ( item == BUTTONSSINGLE )
      {
      t_buttons[3] = -10;
      s_buttons[3] = -10;
      s_buttons[5] = -10;
      s_buttons[6] = -10;
      s_buttons[7] = -10;
      s_buttons[9] = -10;
      s_buttons[10] = -10;
      s_buttons[11] = -10;
      s_buttons[12] = -10;
      s_buttons[13] = -10;
      s_buttons[14] = -10;
      s_buttons[15] = -10;
      t_switches[5] = -10;
      t_switches[6] = -10;
      t_switches[7] = -10;
      t_switches[8] = -10;
      }

   if (( item == BUTTONSCOMBO ) || ( item == ALL ) )
      init_button_combo ( stop );

 
   /* If new data was read in about the default values then store it to the */
   /* initialization file.                                                  */
 
   if (!stop)
      write_file();

   /* Compute new dead zones for the new default valuator ranges.           */

   if (!stop)
      {
      deaden_roll ( rollinfo.deaden );  
      deaden_pitch ( pitchinfo.deaden );
      deaden_lthrottle ( lthrottle.deaden );
      deaden_rthrottle ( rthrottle.deaden );
      } /* end computing dead zone */

   if ( (fcspid != -1) && (sema_set) )
      usvsema ( resetsema );

   return (!stop);

   } /* end initialize */


/* Function "suspend" suspends the parallel, lightweight thread FCS read    */
/* read process and sets the "is_reading" flag to false for the FCS read    */
/* process.                                                                 */

void
kaflight::suspend ()
   {
   if ( is_reading )
      {
      if ( fcspid != -1 )
         uspsema ( resetsema );
      is_reading = 0;
      }
   }


/* Function "resume" awakens the lightweight thread FCS read process if it  */
/* has been put to sleep by a call to "suspend".  Also sets the "is_reading"*/
/* flag to true.                                                            */

void
kaflight::resume ()
   {
   if (!is_reading )
      {
      if ( fcspid != -1 )
         usvsema ( resetsema );
      is_reading = 1;
      }
   }


/* Function "deaden_roll" defines an area around the stick's resting        */
/* position that is considered to be equivalent to no motion.  This area    */
/* is calculated as a percentage of the positive and negative raw ranges.   */
      
void
kaflight::deaden_roll ( double percent )
   {
   fcsdata amount;

   /* Force percent to be a true percentage between 0.0 and 1.0.            */
   if ( percent < 0.0 )
      percent = 0.0;
   else if ( percent > 1.0 )
      percent = 1.0;

   /* Compute the range of values to the left of the resting position.      */
   /* Then compute the raw amount of dead zone from the percentage.         */
   /* Then compute the raw value the roll must exceed to be considered      */
   /*    as moved (by exceed, we mean less than in this case).              */
   /* Then adjust the usable range since is was decreased by the dead zone. */

   rollinfo.negrange = rollinfo.mid - rollinfo.min; 
   amount = fcsdata (percent * double(rollinfo.negrange));
   rollinfo.deadmin = rollinfo.mid - amount;
   rollinfo.negrange -= amount;

   /* In a similar fashion, compute the values for the area to the right of */
   /* of the resting position.                                              */

   rollinfo.posrange = rollinfo.max - rollinfo.mid;
   amount = fcsdata (percent * double(rollinfo.posrange)); 
   rollinfo.deadmax = rollinfo.mid + amount;
   rollinfo.posrange -= amount;

   /* Update the deaden variable in the device information.                 */
   rollinfo.deaden = percent;

   } /* end deaden_roll */


/* Function "deaden_pitch" defines an area around the stick's resting       */
/* position that is considered to be equivalent to no motion.  This area    */
/* is calculated as a percentage of the positive and negative raw ranges.   */

void
kaflight::deaden_pitch ( double percent )
   {
   fcsdata amount;

   /* Force percent to be a true percentage between 0.0 and 1.0.            */

   if ( percent < 0.0 )
      percent = 0.0;
   else if ( percent > 1.0 )
      percent = 1.0;

   /* Compute the range of values to the front of the resting position.     */
   /* Then compute the raw amount of dead zone from the percentage.         */
   /* Then compute the raw value the pitch must exceed to be considered     */
   /*    as moved (by exceed, we mean less than in this case).              */
   /* Then adjust the usable range since is was decreased by the dead zone. */

   pitchinfo.negrange = pitchinfo.mid - pitchinfo.min;
   amount = fcsdata (percent * double(pitchinfo.negrange));
   pitchinfo.deadmin = pitchinfo.mid - amount;
   pitchinfo.negrange -= amount; 

   /* In a similar fashion, compute the values for the area to the rear of  */
   /* of the resting position.                                              */

   pitchinfo.posrange = pitchinfo.max - pitchinfo.mid;
   amount = fcsdata (percent * double(pitchinfo.posrange)); 
   pitchinfo.deadmax = pitchinfo.mid + amount;
   pitchinfo.posrange -= amount; 

   /* Update the deaden variable in the device information.                 */
   pitchinfo.deaden = percent;

   } /* end deaden_pitch */


/* Function "deaden_lthrottle" defines an area forward of the throttle's    */
/* zero position (extreme rear) that is considered to be equivalent to no   */
/* motion.  This area is calculated as a percentage of the raw range.       */

void
kaflight::deaden_lthrottle ( double percent )
   {
   fcsdata amount;

   /* Force percent to be a true percentage between 0.0 and 1.0.            */

   if ( percent < 0.0 )
      percent = 0.0;
   else if ( percent > 1.0 )
      percent = 1.0;

   /* Compute the range of values to the front of the zero position.        */
   /* Then compute the raw amount of dead zone from the percentage.         */
   /* Then compute the raw value the throttle must exceed to be considered  */
   /*    as moved (by exceed, we mean greater than in this case).           */
   /* Then adjust the usable range since is was decreased by the dead zone. */

   lthrottle.posrange = lthrottle.max - lthrottle.min;
   amount = fcsdata (percent * double(lthrottle.posrange));
   lthrottle.deadmin = lthrottle.min + amount;
   lthrottle.posrange -= amount;

   /* Update the deaden variable in the device information.                */
   lthrottle.deaden = percent;

   } /* end deaden_lthrottle */


/* Function "deaden_rthrottle" defines an area forward of the throttle's    */
/* zero position (extreme rear) that is considered to be equivalent to no   */
/* motion.  This area is calculated as a percentage of the raw range.       */

void
kaflight::deaden_rthrottle ( double percent )
   {
   fcsdata amount;

   /* Force percent to be a true percentage between 0.0 and 1.0.            */

   if ( percent < 0.0 )
      percent = 0.0;
   else if ( percent > 1.0 )
      percent = 1.0;

   /* Compute the range of values to the front of the zero position.        */
   /* Then compute the raw amount of dead zone from the percentage.         */
   /* Then compute the raw value the throttle must exceed to be considered  */
   /*    as moved (by exceed, we mean greater than in this case).           */
   /* Then adjust the usable range since is was decreased by the dead zone. */

   rthrottle.posrange = rthrottle.max - rthrottle.min;
   amount = fcsdata (percent * double(rthrottle.posrange));
   rthrottle.deadmin = rthrottle.min + amount;
   rthrottle.posrange -= amount;

   /* Update the deaden variable in the device information.                */
   rthrottle.deaden = percent;

   } /* end deaden_rthrottle */


/* Function "deaden_throttle" is designed for use by non-split throttle    */
/* systems since left and right then have no meaning.                      */

void
kaflight::deaden_throttle ( double percent )
   {
   this->deaden_lthrottle ( percent );
   } /* end deaden_throttle */


/* Function "roll_sensitive" sets the roll scale value for the device      */
/* which is used to compute the value for the roll.                        */

void
kaflight::roll_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   rollinfo.sensitive = percent;
   } /* end roll_sensitive */


/* Function "pitch_sensitive" sets the pitch scale value for the device    */
/* which is used to compute the value for the pitch.                       */

void
kaflight::pitch_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   pitchinfo.sensitive = percent;
   } /* end pitch_sensitive */


/* Function "lthrottle_sensitive" sets the lthrottle scale value for the   */
/* device which is used to compute the value for the left throttle.        */

void
kaflight::lthrottle_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   lthrottle.sensitive = percent;
   } /* end lthrottle_sensitive */


/* Function "rthrottle_sensitive" sets the rthrottle scale value for the   */
/* device which is used to compute the value for the right throttle.       */

void
kaflight::rthrottle_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   rthrottle.sensitive = percent;
   } /* end rthrottle_sensitive */


/* Function "throttle_sensitive" sets the throttle scale value for the     */
/*device which is used to compute the value for the left throttle.         */
/* Designed for single throttle systems where left and right make no sense.*/

void
kaflight::throttle_sensitive ( double percent )
   {
   if ( percent < 0.0 )
      percent = 0.0;
   lthrottle.sensitive = percent;
   } /* end throttle_sensitive */


/* Function "roll" extracts the raw roll infomation from the correct       */
/* position of the data buffer, makes sure the value is not beyond the     */
/* initial minimums and maximums for the component, determines if the raw  */
/* value is outside the dead zone, and then scales the value according to  */
/* the component's sensitivity settings.  If the value is inside the dead  */
/* zone then the value is zeroed out.                                      */

float
kaflight::roll () const
   {
   fcsdata raw_roll;
   double temp_roll;

   /* Extract the raw information from the correct position of the buffer. */
   raw_roll = fcsdata(data[3]);

   /* Guarantee the raw value does not exceed the minimum and maximum for  */
   /* the component.                                                       */
   if ( raw_roll > rollinfo.max )
      raw_roll = rollinfo.max;
   else if ( raw_roll < rollinfo.min )
      raw_roll = rollinfo.min; 
   
   /* If the value is outside the dead zone then compute its normalized    */
   /* value reflecting the percentage of movement over the entire range.   */
   if ( raw_roll < rollinfo.deadmin )
      temp_roll = ( (raw_roll - rollinfo.deadmin) / double(rollinfo.negrange) );
   else if ( raw_roll > rollinfo.deadmax )
      temp_roll = ( (raw_roll - rollinfo.deadmax) / double(rollinfo.posrange) );

   /* If the value is inside the dead zone then zero it out.               */
   else
      temp_roll = 0;

   /* Return the computed value scaled by the components sensitivity.      */
   if (flip_roll)
      return ( float( -temp_roll * rollinfo.sensitive ) );
   else
      return ( float( temp_roll * rollinfo.sensitive ) );


   } /* end roll */


/* Function "pitch" extracts the raw pitch infomation from the correct     */
/* position of the data buffer, makes sure the value is not beyond the     */
/* initial minimums and maximums for the component, determines if the raw  */
/* value is outside the dead zone, and then scales the value according to  */
/* the component's sensitivity settings.  If the value is inside the dead  */
/* zone then the value is zeroed out.                                      */

float
kaflight::pitch () const
   {
   fcsdata raw_pitch;
   double temp_pitch;
   
   /* Extract the raw information from the correct position of the buffer. */
   raw_pitch = fcsdata(data[5]);

   /* Guarantee the raw value does not exceed the minimum and maximum for  */
   /* the component.                                                       */
   if ( raw_pitch > pitchinfo.max )
      raw_pitch = pitchinfo.max;
   else if ( raw_pitch < pitchinfo.min )
      raw_pitch = pitchinfo.min;

   /* If the value is outside the dead zone then compute its normalized    */
   /* value reflecting the percentage of movement over the entire range.   */
   if ( raw_pitch < pitchinfo.deadmin )
      temp_pitch = ( (raw_pitch - pitchinfo.deadmin) /
                     double(pitchinfo.negrange)        );
   else if ( raw_pitch > pitchinfo.deadmax )
      temp_pitch = ( (raw_pitch - pitchinfo.deadmax) /
                     double(pitchinfo.posrange)        );

   /* If the value is inside the dead zone then zero it out.               */
   else
      temp_pitch = 0;

   /* Return the computed value scaled by the components sensitivity.      */
   if (flip_pitch)
      return ( float( temp_pitch * pitchinfo.sensitive ) );
   else
      return ( float( -temp_pitch * pitchinfo.sensitive ) );


   } /* end pitch */


/* Function "stck_thumb_wheel" extracts the raw information from the       */
/* correct position of the data buffer and returns an integer indicating   */
/* its current position as reflected by the directional constants.         */

fcsdata
kaflight::stck_thumb_wheel () const
   {
   fcsdata temp_wheel;
   
   /* Extract the raw information from the correct position of the buffer. */
   temp_wheel = fcsdata(data[2]);

   /* Determine the wheel's direction and return the correct directional   */
   /* constant.                                                            */

   if ( (temp_wheel >= s_hat[HAT_UP]-1) && 
        (temp_wheel <= s_hat[HAT_UP]+1) )
      return UP;
   else if ( (temp_wheel >= s_hat[HAT_DOWN]-1) && 
             (temp_wheel <= s_hat[HAT_DOWN]+1) )
      return DOWN;
   else if ( (temp_wheel >= s_hat[HAT_LEFT]-1) && 
             (temp_wheel <= s_hat[HAT_LEFT]+1) )
      return LEFT;
   else if ( (temp_wheel >= s_hat[HAT_RIGHT]-1) && 
             (temp_wheel <= s_hat[HAT_RIGHT]+1) )
      return RIGHT;
   else
      return MIDDLE;

   } /* end stck_thumb_wheel */


/* Function "stck_top_button" returns 1 if the top button on the stick     */
/* is pressed or 0 if it is not pressed.                                   */

fcsdata
kaflight::stck_top_button () const
   {
   return ( ((data[1] >= s_buttons[2]-1) && (data[1] <= s_buttons[2]+1)) ||
            ((data[1] >= s_buttons[3]-1) && (data[1] <= s_buttons[3]+1)) ||
            ((data[1] >= s_buttons[6]-1) && (data[1] <= s_buttons[6]+1)) ||
            ((data[1] >= s_buttons[7]-1) && (data[1] <= s_buttons[7]+1)) ||
            ((data[1] >= s_buttons[10]-1) && (data[1] <= s_buttons[10]+1)) ||
            ((data[1] >= s_buttons[11]-1) && (data[1] <= s_buttons[11]+1)) ||
            ((data[1] >= s_buttons[14]-1) && (data[1] <= s_buttons[14]+1)) ||
            ((data[1] >= s_buttons[15]-1) && (data[1] <= s_buttons[15]+1)) );
   } /* end stck_top_button */


/* Function "stck_mid_button" returns 1 if the middle button on the stick  */
/* is pressed or 0 if it is not pressed.                                   */

fcsdata
kaflight::stck_mid_button () const
   {
   return ( ((data[1] >= s_buttons[4]-1) && (data[1] <= s_buttons[4]+1)) ||
            ((data[1] >= s_buttons[5]-1) && (data[1] <= s_buttons[5]+1)) ||
            ((data[1] >= s_buttons[6]-1) && (data[1] <= s_buttons[6]+1)) ||
            ((data[1] >= s_buttons[7]-1) && (data[1] <= s_buttons[7]+1)) ||
            ((data[1] >= s_buttons[12]-1) && (data[1] <= s_buttons[12]+1)) ||
            ((data[1] >= s_buttons[13]-1) && (data[1] <= s_buttons[13]+1)) ||
            ((data[1] >= s_buttons[14]-1) && (data[1] <= s_buttons[14]+1)) ||
            ((data[1] >= s_buttons[15]-1) && (data[1] <= s_buttons[15]+1)) );
   } /* end stck_mid_button */


/* Function "stck_bot_button" returns 1 if the bottom button on the stick  */
/* is pressed or 0 if it is not pressed.                                   */

fcsdata
kaflight::stck_bot_button () const
   {
   return ( ((data[1] >= s_buttons[8]-1) && (data[1] <= s_buttons[8]+1)) ||
            ((data[1] >= s_buttons[9]-1) && (data[1] <= s_buttons[9]+1)) ||
            ((data[1] >= s_buttons[10]-1) && (data[1] <= s_buttons[10]+1)) ||
            ((data[1] >= s_buttons[11]-1) && (data[1] <= s_buttons[11]+1)) ||
            ((data[1] >= s_buttons[12]-1) && (data[1] <= s_buttons[12]+1)) ||
            ((data[1] >= s_buttons[13]-1) && (data[1] <= s_buttons[13]+1)) ||
            ((data[1] >= s_buttons[14]-1) && (data[1] <= s_buttons[14]+1)) ||
            ((data[1] >= s_buttons[15]-1) && (data[1] <= s_buttons[15]+1)) );
   } /* end stck_bot_button */


/* Function "stck_trigger" returns 1 if the trigger on the stick is        */
/* pressed or 0 if it is not pressed.                                      */

fcsdata
kaflight::stck_trigger () const
   {
   return ( ((data[1] >= s_buttons[1]-1) && (data[1] <= s_buttons[1]+1)) ||
            ((data[1] >= s_buttons[3]-1) && (data[1] <= s_buttons[3]+1)) ||
            ((data[1] >= s_buttons[5]-1) && (data[1] <= s_buttons[5]+1)) ||
            ((data[1] >= s_buttons[7]-1) && (data[1] <= s_buttons[7]+1)) ||
            ((data[1] >= s_buttons[9]-1) && (data[1] <= s_buttons[9]+1)) ||
            ((data[1] >= s_buttons[11]-1) && (data[1] <= s_buttons[11]+1)) ||
            ((data[1] >= s_buttons[13]-1) && (data[1] <= s_buttons[13]+1)) ||
            ((data[1] >= s_buttons[15]-1) && (data[1] <= s_buttons[15]+1)) );
   } /* end stck_trigger */


/* Function "left_throttle" extracts the raw infomation from the correct   */
/* position of the data buffer, makes sure the value is not beyond the     */
/* initial minimums and maximums for the component, determines if the raw  */
/* value is outside the dead zone, and then scales the value according to  */
/* the component's sensitivity settings.  If the value is inside the dead  */
/* zone then the value is zeroed out.                                      */

float
kaflight::left_throttle () const
   {
   fcsdata raw_throttle;
   double temp_throttle;
   
   /* Extract the raw information from the correct position of the buffer. */
   raw_throttle = fcsdata(data[4]);

   /* Guarantee the raw value does not exceed the minimum and maximum for  */
   /* the component.                                                       */
   if ( raw_throttle > lthrottle.max )
      raw_throttle = lthrottle.max;
   else if ( raw_throttle < lthrottle.min )
      raw_throttle = lthrottle.min;

   /* If the value is outside the dead zone then compute its normalized    */
   /* value reflecting the percentage of movement over the entire range.   */
   if ( raw_throttle > lthrottle.deadmin )
      temp_throttle = ( (raw_throttle - lthrottle.deadmin) /
                        double(lthrottle.posrange)          );

   /* If the value is inside the dead zone then zero it out.               */
   else
      temp_throttle = 0.0;

   /* Return the computed value scaled by the components sensitivity.      */
   return float (lthrottle.sensitive * temp_throttle); 

   } /* end left_throttle */


/* Function "right_throttle" extracts the raw infomation from the correct  */
/* position of the data buffer, makes sure the value is not beyond the     */
/* initial minimums and maximums for the component, determines if the raw  */
/* value is outside the dead zone, and then scales the value according to  */
/* the component's sensitivity settings.  If the value is inside the dead  */
/* zone then the value is zeroed out.                                      */

float
kaflight::right_throttle () const
   {
   fcsdata raw_throttle;
   double temp_throttle;

   /* Extract the raw information from the correct position of the buffer. */
   raw_throttle = fcsdata(data[4]);

   /* Guarantee the raw value does not exceed the minimum and maximum for  */
   /* the component.                                                       */
   if ( raw_throttle > rthrottle.max )
      raw_throttle = rthrottle.max;
   else if ( raw_throttle < rthrottle.min )
      raw_throttle = rthrottle.min;

   /* If the value is outside the dead zone then compute its normalized    */
   /* value reflecting the percentage of movement over the entire range.   */
   if ( raw_throttle > rthrottle.deadmin )
      temp_throttle = ( (raw_throttle - rthrottle.deadmin) /
                        double(rthrottle.posrange)          );

   /* If the value is inside the dead zone then zero it out.               */
   else
      temp_throttle = 0.0;

   /* Return the computed value scaled by the components sensitivity.      */
   return float (rthrottle.sensitive * temp_throttle);

   } /* end right_throttle */


/* Function "throttle" is designed for non-split throttles since left and  */
/* right do not apply in that case.                                        */

float
kaflight::throttle () const
   {
   return this->left_throttle();
   } /* end throttle */


/* Function "thrtl_switch" extracts the raw information from the           */
/* correct position of the data buffer and returns an integer indicating   */
/* its current position as reflected by the directional constants.         */

fcsdata
kaflight::thrtl_switch1 () const
   {
   fcsdata temp_switch;
   
   /* Extract the raw information from the correct position of the buffer. */
   temp_switch = fcsdata(data[8]);

   /* Determine the switch's direction and return the correct directional  */
   /* constant.                                                            */

   if (((temp_switch >= t_switches[1]-1)&&(temp_switch <= t_switches[1]+1)) ||
       ((temp_switch >= t_switches[6]-1)&&(temp_switch <= t_switches[6]+1)) ||
       ((temp_switch >= t_switches[7]-1)&&(temp_switch <= t_switches[7]+1)))
      return FORWARD;
   else if
      (((temp_switch >= t_switches[2]-1)&&(temp_switch <= t_switches[2]+1)) ||
       ((temp_switch >= t_switches[5]-1)&&(temp_switch <= t_switches[5]+1)) ||
       ((temp_switch >= t_switches[8]-1)&&(temp_switch <= t_switches[8]+1)))
      return BACKWARD;
   else
      return MIDDLE;

   } /* end thrtl_switch */

/* Function "thrtl_switch" extracts the raw information from the           */
/* correct position of the data buffer and returns an integer indicating   */
/* its current position as reflected by the directional constants.         */

fcsdata
kaflight::thrtl_switch2 () const
   {
   fcsdata temp_switch;

   /* Extract the raw information from the correct position of the buffer. */
   temp_switch = fcsdata(data[8]);

   /* Determine the switch's direction and return the correct directional  */
   /* constant.                                                            */

   if (((temp_switch >= t_switches[3]-1)&&(temp_switch <= t_switches[3]+1)) ||
       ((temp_switch >= t_switches[5]-1)&&(temp_switch <= t_switches[5]+1)) ||
       ((temp_switch >= t_switches[7]-1)&&(temp_switch <= t_switches[7]+1)))
      return FORWARD;
   else if
      (((temp_switch >= t_switches[4]-1)&&(temp_switch <= t_switches[4]+1)) ||
       ((temp_switch >= t_switches[6]-1)&&(temp_switch <= t_switches[6]+1)) ||
       ((temp_switch >= t_switches[8]-1)&&(temp_switch <= t_switches[8]+1)))
      return BACKWARD;
   else
      return MIDDLE;

   } /* end thrtl_switch */

/* Function "thrtl_dial" extracts the raw information from the correct      */
/* position of the data buffer and returns this raw data.                   */

fcsdata
kaflight::thrtl_dial () const
   {
   return data[6];
   } /* end thrtl_dial */


/* Function "thrtl_ball_az" extracts the raw information from the correct  */
/* position of the data buffer and returns this raw data.                   */

fcsdata
kaflight::thrtl_ball_az () const
   {
   return 0; 
   } /* end thrtl_ball_az */


/* Function "thrtl_ball_el" extracts the raw information from the correct  */
/* position of the data buffer and returns this raw data.                   */

fcsdata
kaflight::thrtl_ball_el () const
   {
   return 0; 
   } /* end thrtl_ball_el */


/* Function "thrtl_but1" returns 1 if the button 1 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
kaflight::thrtl_but1 () const
   {
   return (( (data[7] >= t_buttons[1]-1) && (data[7] <= t_buttons[1]+1) ) || 
           ( (data[7] >= t_buttons[3]-1) && (data[7] <= t_buttons[3]+1) ));
   } /* end thrtl_but1 */


/* Function "thrtl_but2" returns 1 if the button 2 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
kaflight::thrtl_but2 () const
   {
   return (( (data[7] >= t_buttons[2]-1) && (data[7] <= t_buttons[2]+1) ) || 
           ( (data[7] >= t_buttons[3]-1) && (data[7] <= t_buttons[3]+1) ));
   } /* end thrtl_but2 */


/* Function "thrtl_but3" returns 1 if the button 3 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
kaflight::thrtl_but3 () const
   {
   return 0; 
   } /* end thrtl_but3 */


/* Function "thrtl_but4" returns 1 if the button 4 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
kaflight::thrtl_but4 () const
   {
   return 0; 
   } /* end thrtl_but4 */


/* Function "thrtl_but5" returns 1 if the button 5 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
kaflight::thrtl_but5 () const
   {
   return 0; 
   } /* end thrtl_but5 */


/* Function "thrtl_but6" returns 1 if the button 6 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
kaflight::thrtl_but6 () const
   {
   return 0; 
   } /* end thrtl_but6 */


/* Function "thrtl_but7" returns 1 if the button 7 on the throttle is     */
/* pressed or 0 if it is not pressed.                                     */

fcsdata
kaflight::thrtl_but7 () const
   {
   return 0; 
   } /* end thrtl_but7 */


void
kaflight::get_all_inputs ( pfChannel * )
{
   if ( valid && this->new_data() ) {
      this->get_data();
   }
}


int
kaflight::button_pressed ( const NPS_BUTTON button_num, int &num_presses )
{
   int success = FALSE;
   num_presses = 0;

   if ( valid ) {
      success = TRUE;
      switch ( button_num ) {
         case NPS_STCK_TRIGGER:
            num_presses = this->stck_trigger();
            break;
         case NPS_STCK_TOP_BUTTON:
            num_presses = this->stck_top_button();
            break;
         case NPS_STCK_MID_BUTTON:
            num_presses = this->stck_mid_button();
            break;
         case NPS_STCK_BOT_BUTTON:
            num_presses = this->stck_bot_button();
            break;
         case NPS_THRTL_BUTTON_1:
            num_presses = this->thrtl_but1();
            break;
         case NPS_THRTL_BUTTON_2:
            num_presses = this->thrtl_but2();
            break;
         case NPS_THRTL_BUTTON_3:
            num_presses = this->thrtl_but3();
            break;
         case NPS_THRTL_BUTTON_4:
            num_presses = this->thrtl_but4();
            break;
         case NPS_THRTL_BUTTON_5:
            num_presses = this->thrtl_but5();
            break;
         case NPS_THRTL_BUTTON_6:
            num_presses = this->thrtl_but6();
            break;
         case NPS_THRTL_BUTTON_7:
            num_presses = this->thrtl_but7();
            break;
         default:
            num_presses = 0;
            success = FALSE;
            break;
      }
   }

   return success;
}

int
kaflight::valuator_set ( const NPS_VALUATOR valuator_num, float &value )
{
   int success = FALSE;

   if ( valid ) {
      success = TRUE;
      switch ( valuator_num ) {
         case NPS_STCK_HORIZONTAL:
            value = this->roll();
            break;
         case NPS_STCK_VERTICAL:
            value = this->pitch();
            break;
         case NPS_THRTL:
            value = this->throttle();
            break;
         case NPS_THRTL_LEFT:
            value = this->left_throttle();
            break;
         case NPS_THRTL_RIGHT:
            value = this->right_throttle();
            break;
         default:
            value = 0.0f;
            success = FALSE;
            break;
      }
   }
   return success;
}

int
kaflight::switch_set ( const NPS_SWITCH switch_num, NPS_SWITCH_VAL &value )
{
   int success = FALSE;

   if ( valid ) {
      success = TRUE;
      switch ( switch_num ) {
         case NPS_SWITCH_0:
            value = (NPS_SWITCH_VAL)stck_thumb_wheel();
            break;
         case NPS_SWITCH_1:
            value = (NPS_SWITCH_VAL)thrtl_switch1();
            break;
         case NPS_SWITCH_2:
            value = (NPS_SWITCH_VAL)thrtl_switch2();
            break;
         default:
            value = NPS_SWITCH_CENTER;
            success = FALSE;
            break;
      }
   }

   return success;
}

int
kaflight::calibrate ( const NPS_VALUATOR which_valuator,
                 const NPS_CALIBRATION which_calibration,
                 const float value )
{
   int success = TRUE;

   switch ( which_valuator ) {
      case NPS_STCK:
         switch ( which_calibration ) {
            case NPS_RESTING_STATE:
               success = this->initialize(RESTING,0);
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_STCK_HORIZONTAL:
         switch ( which_calibration ) {
            case NPS_RESTING_STATE:
               success = this->initialize(RESTING,0);
               break;
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_roll(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->roll_sensitive(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_STCK_VERTICAL:
         switch ( which_calibration ) {
            case NPS_RESTING_STATE:
               success = this->initialize(RESTING,0);
               break;
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_pitch(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->pitch_sensitive(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_THRTL_LEFT:
         switch ( which_calibration ) {
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_lthrottle(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->lthrottle_sensitive(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_THRTL_RIGHT:
         switch ( which_calibration ) {
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_rthrottle(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->rthrottle_sensitive(value);
               }
               break;
            default:
               success = FALSE;
               break;
         }
         break;
      case NPS_THRTL:
         switch ( which_calibration ) {
            case NPS_DEAD_ZONE:
               success = ( (0.0f <= value) && (value <= 1.0) );
               if ( success ) {
                  this->deaden_throttle(value);
               }
               break;
            case NPS_SCALE_VALUE:
               success = (value >= 0.0f);
               if ( success ) {
                  this->throttle_sensitive(value);
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

