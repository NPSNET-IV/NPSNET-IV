// File: <soundlib.cc>

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
/* File:        soundlib.c
 * Description: library of sound functions for the NPSNET sound server
 * Revision:    4.1- August94
 *
 * References:  MIDI standard
 *
 * Author:      Susannah Bloch
 *              Leif Dahl
 *              John Roesli
 *		Paul Barham
 *              Russell Storms
 *
 * Hardware:    John Roesli
 *              Russell Storms
 *              
 *
 * Internet:    storms@cs.nps.navy.mil 
 *              roesli@twics.com
 *
 * Changes:
 *          date                change
 *         23 Jul 93          added my_sound_trigger
 *         24 Jul 93          added env_sound_on and env_sound_off, with 
 *                            ENV_CHANNEL
 *         27 Jul 93          added some subwoofer (channel 8) sound to 
 *                            all triggered sounds at volume = "volume"
 *         28 Jul 93          took out the three notes in my_sound_on and
 *                            my_sound_off, and made them just one..
 */

#include "soundlib.h"

extern "C" {int ioctl(int, int, ...);}

//extern "C" {int sginap(long);}

int flopTime = 2;
int soundCount = 0;

double playTime1, 
       playTime2;


/***********sleep - this is a BAD hack *****************/
void asleep(int seconds)
   
{
   sleep(seconds);
}

/*****************************initialize_port****************************/
int initialize_port(char *MidiPortName)
{

   int arg;                 /* temp variables */
   struct termio t ;        /* port structure */
   struct strioctl str;		/* who knows? */
   int MidiPort;                  /* file descriptor to use for the port */


   /* Open the port for read/write */
   MidiPort = open (MidiPortName, O_RDWR, 0622 );

   if(MidiPort < 0)
   {
      printf("initialize_port(): Could not open %s\n",MidiPortName);
      perror("initialize_port()");
      exit(1);
   }

   t.c_iflag = IGNBRK;
   t.c_oflag = 0;
   t.c_cflag = B9600 | CS8 | CREAD | CLOCAL | HUPCL;
   t.c_lflag = 0;
   t.c_cc[VINTR] = 0;
   t.c_cc[VQUIT] = 0;
   t.c_cc[VERASE] = 0;
   t.c_cc[VKILL] = 0;
   t.c_cc[VMIN] = 1;
   t.c_cc[VTIME] = 0;
   ioctl(MidiPort, TCSETAF, &t);

   str.ic_cmd = SIOC_RS422;
   str.ic_timout = 0;
   str.ic_len = 4;
   arg = RS422_ON;
   str.ic_dp = (char *)&arg;
   if (ioctl(MidiPort, I_STR, &str) < 0) {
       perror("Can't ioctl RS422");
       exit(1);
   }

   str.ic_cmd = SIOC_EXTCLK;
   str.ic_timout = 0;
   str.ic_len = 4;
   arg = EXTCLK_32X;
   str.ic_dp = (char *)&arg;
   if (ioctl(MidiPort, I_STR, &str) < 0) {
       perror("Can't ioctl EXTCLK");
       exit(1);
   }
  
   str.ic_cmd = SIOC_ITIMER;
   str.ic_timout = 0;
   str.ic_len = 4;
   arg = 0;
   str.ic_dp = (char *)&arg;
   if (ioctl(MidiPort, I_STR, &str) < 0) {
       perror("Can't ioctl ITIMER");
       exit(1);
   }

   fflush(stderr);

   /* return the file descriptor */
   return MidiPort;


}  /* End initialize_port() */	

/*********************************close_port****************************/
void close_port(int midifd)
   
{
   close(midifd);
}


/******************************send_midi_command**********************/
/* The following routine sends bytes out to the Midi line.  */
void send_midi_command(int MidiPort, unsigned char send_it)
{

   unsigned char all_bytes;   /* hold for the bytes so we do one write */
   all_bytes = send_it;
   write(MidiPort, &all_bytes, 1);   /* send those bytes */
  /* printf("not sending: %d\n",send_it);*/
  // printf("sending: %d\n",send_it);

}


void add_sound(int volume,int sound,int midiport,int channel)
   
{
   send_midi_command( midiport, (unsigned char) (NOTE_ON + channel));
   send_midi_command( midiport, (unsigned char) sound);
   send_midi_command( midiport, (unsigned char) volume);
}


/* this is  the same idea as trigger_sound; for continuous sounds */ 
void update_sound(int volume ,int sound ,int midiport ,int channel)
   
{
   send_midi_command(midiport, (unsigned char) (NOTE_ON + channel));
   send_midi_command(midiport, (unsigned char) sound);
   send_midi_command(midiport, (unsigned char) 0); 
   send_midi_command(midiport, (unsigned char) (NOTE_OFF + channel));
   send_midi_command(midiport, (unsigned char) sound);
   send_midi_command(midiport, (unsigned char) volume);
}

void end_sound(int sound,int midiport,int channel)
   
{
   send_midi_command( midiport, (unsigned char) (NOTE_OFF + channel));
   send_midi_command( midiport, (unsigned char) sound);
   send_midi_command( midiport, (unsigned char) 0);
}


void trigger_sound(int volume, int sound,int midiport,int channel)
   
{
   send_midi_command(midiport, (unsigned char) (NOTE_ON + channel));
   send_midi_command(midiport, (unsigned char) sound);
   send_midi_command(midiport, (unsigned char) volume);

   send_midi_command(midiport, (unsigned char) (NOTE_OFF + channel));
   send_midi_command(midiport, (unsigned char) sound);
   send_midi_command(midiport, (unsigned char) 0);
 }

void my_sound_on(int midiport,int sound,int volume)
   
{
   send_midi_command(midiport, (unsigned char) (NOTE_ON + MY_CHANNEL));
   send_midi_command(midiport, (unsigned char) sound);
   send_midi_command(midiport, (unsigned char) volume);
}

void my_attribute_on(int midiport,int sound,int volume)

{
   send_midi_command(midiport, (unsigned char) (NOTE_ON + ATTR_CHANNEL));
   send_midi_command(midiport, (unsigned char) sound);
   send_midi_command(midiport, (unsigned char) volume);
}

void my_reset(int midiport)
   
{
   /* sequencer stop */
   send_midi_command(midiport, (unsigned char) 0xfc);
   /* null command */
   send_midi_command(midiport, (unsigned char) 0xf0);
   send_midi_command(midiport, (unsigned char) 0x18);
   send_midi_command(midiport, (unsigned char) 0x02);
   send_midi_command(midiport, (unsigned char) 0x16);
   send_midi_command(midiport, (unsigned char) 99); 
   send_midi_command(midiport, (unsigned char) 0xf7);
   sginap(40);
}

void my_sound_trigger(int midiport,int sound,int volume)
   
{
   trigger_sound(volume, sound, midiport, MY_CHANNEL);
}

void my_sound_off(int midiport,int sound)
   
{
   end_sound(sound, midiport, MY_CHANNEL);
   /*end_sound(sound+1, midiport, MY_CHANNEL);
   end_sound(sound+2, midiport, MY_CHANNEL);
   end_sound(sound+3, midiport, MY_CHANNEL);*/
}

void my_attribute_off(int midiport,int sound)
  
{
   end_sound(sound, midiport, ATTR_CHANNEL);
   /*end_sound(sound+1, midiport, ATTR_CHANNEL);
   end_sound(sound+2, midiport, ATTR_CHANNEL);
   end_sound(sound+3, midiport, ATTR_CHANNEL);*/
}


void env_sound_on(int midiport, int sound, int volume)
   
{
   update_sound(volume, sound, midiport, ENV_CHANNEL);
}

void env_sound_off(int midiport, int sound)
   
{
   end_sound(sound, midiport, ENV_CHANNEL);
}


// compute magnitude of a vector
float compute_vector_magnitude(float vec[])
{
  float sum = 0.0;
  for (int l = 0; l < 3; l++)
  {
	 sum += vec[l]*vec[l];
  }
  return sqrt(sum);
}



/************************trigger_3d_sound***************************/
/* 
   trigger_3d_sound takes the location of the sound, computes
   volumes, writes the midi command bytes to an array of integers
   (send_bytes), and then sends three note on and three note off
   commands down the line using send_midi_command 
   this depends upon the following setup in the sampler:
  
      poly mode is on
      +x - sounds are on preset 0 midi channel 1
      +y - sounds are on preset 1 midi channel 2
      +z - sounds are on preset 2 midi channel 3
      -x - sounds are on preset 3 midi channel 4
      -y - sounds are on preset 4 midi channel 5
      -z - sounds are on preset 5 midi channel 6
+x is right front, -x is left back, +y is left front, -y is right back, 


*/

   
int trigger_3d_sound(int midiPort, EntityLocation listener_loc,
                     EntityLocation sound_loc,EntityOrientation my_view,
                     int sound,  int total_volume)
{


/*   *** taken out for now
soundCount++;


if (flopTime == 1)
   { 
   playTime2 = get_time();
   flopTime = 2;   
   }
else
   {
   playTime1 = get_time();
   flopTime = 1;
   }

if (fabs(playTime1 - playTime2) > 0.05)
   soundCount = 0;
*/


   //float compute_vector_magnitude(float vec[]);

   double distance,volume, temp1, temp2;
   double x,y,z;
   int lfrVolume, lflVolume, lbrVolume, lblVolume,
       ufrVolume, uflVolume, ubrVolume, ublVolume;
   int poolvolume = 40; // the maximum differece of volume
                        // between speakers

   float size;
   float source[3];
   float listener[3];

   // speaker position with respect to the listener
   float speaker_position[8][3] = {{0.0, 0.0, 0.0},
                                   {0.0, 0.0, 0.0},
			           {0.0, 0.0, 0.0},
   			           {0.0, 0.0, 0.0},
                                   {0.0, 0.0, 0.0},
                                   {0.0, 0.0, 0.0},
			           {0.0, 0.0, 0.0},
   			           {0.0, 0.0, 0.0}}; 
   // from the listener to each speaker
   float speaker_vector[8][3] = {{0.0, 0.0, 0.0},
                                 {0.0, 0.0, 0.0},
			         {0.0, 0.0, 0.0},
   			         {0.0, 0.0, 0.0},
                                 {0.0, 0.0, 0.0},
                                 {0.0, 0.0, 0.0},
			         {0.0, 0.0, 0.0},
   			         {0.0, 0.0, 0.0}};

   // the individual speaker volumes
   int speaker_volume[8] = {0,0,0,0,0,0,0,0};

   // from the listener to the sound source 
   float lis_to_src_vector[4] = {0.0, 0.0, 0.0, 0.0};

   // between the lis_to_speaker and lis_to_src vector
   float angle[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};            
 
// set coordinates so listener_loc is at the origin and sound is at (x,y,z)    
   x = sound_loc.x - listener_loc.x;
   y = sound_loc.y - listener_loc.y;
   z = sound_loc.z - listener_loc.z;

// correct for sound source orientation to vehicle 
//    rotate by psi 
   temp1 = x;
   temp2 = y;
   x = (temp1 * cos(my_view.psi)) + (temp2 * sin(my_view.psi));
   y = (temp2 * cos(my_view.psi)) - (temp1 * sin(my_view.psi));  
//   printf("rot by psi = %lf deg: %lf,%lf,%lf\n",(my_view.psi*180/M_PI),x,y,z);

//    rotate by theta 
   temp1 = x;
   temp2 = z;
   x = (temp1 * cos(my_view.theta)) - (temp2 * sin(my_view.theta));
   z = (temp2 * cos(my_view.theta)) + (temp1 * sin(my_view.theta));  
//   printf("rot by theta = %lf deg: %lf,%lf,%lf\n",(my_view.theta*180/M_PI),x,y,z);

//    rotate by phi 
   temp1 = y;
   temp2 = z;
   y = (temp1 * cos(my_view.phi)) + (temp2 * sin(my_view.phi));
   z = (temp2 * cos(my_view.phi)) - (temp1 * sin(my_view.phi));  
//   printf("rot by phi = %lf deg: %lf,%lf,%lf\n",(my_view.phi*180/M_PI),x,y,z);


// now the sound source is oriented correctly with the vehicle
   source[0] = x;
   source[1] = y - 1;
   source[2] = z; 

// the listener is the negative relation to the source
   listener[0] = -x;
   listener[1] = -y;
   listener[2] = -z;
   
   /* the distance to the source */
   distance = sqrt((x*x) + (y*y) + (z*z));
  /* distance = sqrt((x*x) + (y*y));*/
//   printf("dist:  %lf\n",distance); 


  // New loudness algorithm
   if (distance  == 0) distance = EPSILON;
   if ( distance < RANGE)
   {
// 25 works very well as opposed to 50 for half distance
// the smaller the number the more the cut off of sound
     volume = ceil(( 1 - (((log(distance / 25)) /  (log(12700 / 25)))))  
              * total_volume);
   }

//cout << "Volume before = " << volume << endl;

if (volume > 127) volume = 127;

//cout << "Volume = " << volume << endl;

// an old volume algorithm  
  /* if (distance < RANGE) 
      {
	  volume = total_volume - ceil (9 * (log(distance/50))/(log(2)));
      }
	if (volume < 0)
	{
	volume = 0;
	return(0);
	}
	if (distance  == 0) distance = EPSILON;
*/
  
// volume algorithm based on the inverse square law I = W/4*PI*radius_squared
   /*
        volume = 20*(log(CONST1/(CONST2*distance)) / log(10));
   }*/

//compute the speaker positions
// pos   sign of x, y, z to offset from the listener
//  3    +++
//  7    ++-
//  2    +-+
//  6    +--
//  0    -++
//  4    -+-
//  1    --+
//  5    ---
//

   size = length/2.0;

  speaker_position[0][0] = listener[0] - size;
  speaker_position[0][1] = listener[1] + size;
  speaker_position[0][2] = listener[2] - size; 

  speaker_position[1][0] = listener[0] - size;
  speaker_position[1][1] = listener[1] - size;
  speaker_position[1][2] = listener[2] - size;

  speaker_position[2][0] = listener[0] + size;
  speaker_position[2][1] = listener[1] - size;
  speaker_position[2][2] = listener[2] - size;

  speaker_position[3][0] = listener[0] + size;
  speaker_position[3][1] = listener[1] + size;
  speaker_position[3][2] = listener[2] - size;

  speaker_position[4][0] = listener[0] - size;
  speaker_position[4][1] = listener[1] + size;
  speaker_position[4][2] = listener[2] + size; 

  speaker_position[5][0] = listener[0] - size;
  speaker_position[5][1] = listener[1] - size;
  speaker_position[5][2] = listener[2] + size;

  speaker_position[6][0] = listener[0] + size;
  speaker_position[6][1] = listener[1] - size;
  speaker_position[6][2] = listener[2] + size;

  speaker_position[7][0] = listener[0] + size;
  speaker_position[7][1] = listener[1] + size;
  speaker_position[7][2] = listener[2] + size;

// compute the vectors from the listener to each speaker
  for (int i = 0; i < 8; i++)
	 for (int j = 0; j < 3; j++)
		speaker_vector[i][j] = speaker_position[i][j] - listener[j];

//compute the vector from the listener to the source
  for (int k = 0; k < 3; k++)
	 lis_to_src_vector[k] = source[k] - listener[k];

// compute the angles between the lis_to_src_vector and 
// each speaker_vector, using dot product
// A*B = abcos(theta) => cos(theta) = A*B/ab
  float distant_b = compute_vector_magnitude(lis_to_src_vector);
  for (int m = 0; m < 8; m++)
  {
	 float a_dot_b   = 0.0;

	 for (int n = 0; n < 3; n++)
		a_dot_b += speaker_vector[m][n] * lis_to_src_vector[n];

	 float distant_a = compute_vector_magnitude(speaker_vector[m]);
         // do angles from 0 to 180
	 angle[m] = r_to_d*acos(a_dot_b/(distant_a*distant_b));
//cout<<"angle["<<m<<"] = "<<angle[m]<<endl; 
  }

// compute the volume that each speaker should play
   float sum = 0.0;
   int index_zero = -1;
   int index = 0;
   for (int p = 0; p < 8; p++)
   {
      if (angle[p] <= 70.5)
      {
         sum += angle[p];    // sum up all angles which are less than 70.5 degrees

//cout<<" sum angle["<<p<<"] = "<<angle[p]<<endl;
//cout << "sum = " << sum << endl;
// used to be index += 1;
	 index++;            // count up how many of them
	 if (angle[p] < 3.0) index_zero = p;  // for speaker in the direction of
					      // the source
      }
   }

   if (index_zero < 0)  // no speaker is in the direction of the source
   {
      for (int q = 0; q < 8; q++)
         {
	 if (angle[q] <= 70.5)  // divide the pool of volume among the speakers 
         {
        //devide by 2 for demos and devide by 4 for more realistic
              speaker_volume[q] = (volume + poolvolume/2) + 
                                poolvolume * (1 - ((index - 1) * angle[q]/sum));

            // either formula is the same, the above is the one we put in the paper
            // speaker_volume[q] = volume + (poolvolume/sum)*(sum - (index - 1)*angle[q]);
            if (speaker_volume[q] > 127) speaker_volume[q] = 127;            
         }
         else
	    speaker_volume[q] = 0.0;
	 }
   }
   else   // if there is a speaker in the direction of the source
   {
      for (int r = 0; r < 8 ; r++)
      {
      if (index_zero == r)
         speaker_volume[r] = total_volume; // set that speaker to play all volume
      else
         speaker_volume[r] = 0;    // set all others to zero
      }
   }

// this is just an idea for a quickfix
//for (int s = 0; s < 8; s++)
//{
//   if (angle[s] <= 45.0) speaker_volume[s] = volume;
//}

//   cout << "Total Volume = " << total_volume << endl;

   lflVolume = speaker_volume[0];
  
   lfrVolume = speaker_volume[3];
  
   lblVolume = speaker_volume[1];
  
   lbrVolume = speaker_volume[2];

   uflVolume = speaker_volume[4];
  
   ufrVolume = speaker_volume[7];
  
   ublVolume = speaker_volume[5];
  
   ubrVolume = speaker_volume[6];
  
/*   cout <<"lower front left volume = " << lflVolume << endl;   
   cout <<"lower front right volume = " << lfrVolume << endl;
   cout <<"lower back left volume = " << lblVolume << endl;
   cout <<"lower back right volume = " << lbrVolume << endl; 
   cout <<"upper front left volume = " << uflVolume << endl;   
   cout <<"upper front right volume = " << ufrVolume << endl;
   cout <<"upper back left volume = " << ublVolume << endl;
   cout <<"upper back right volume = " << ubrVolume << endl << endl;
*/

//cerr << "soundCount = " << soundCount << endl;

// because the vehicle is using 2 sounds,
// we need to limit any other sounds to a total of 14
//if ( (soundCount < 14) && (fabs(playTime1 - playTime2) > 0.05) )
//{
   trigger_sound(lflVolume, sound, midiPort, 1);
   // this is the collapsing of 3D onto 2D
   trigger_sound(uflVolume, sound, midiPort, 5);

   trigger_sound(lfrVolume, sound, midiPort, 0);
   trigger_sound(ufrVolume, sound, midiPort, 4);

   trigger_sound(lblVolume, sound, midiPort, 3);
   trigger_sound(ublVolume, sound, midiPort, 7);
   
   trigger_sound(lbrVolume, sound, midiPort, 2);
   trigger_sound(ubrVolume, sound, midiPort, 6);   
//}

// cerr << "Diff = " << fabs(playTime1 - playTime2) << endl;


/* bye bye! */
return(1);
}


/*********************************** test **********************************/
void test(int midiport,int sound)
   
{
   sleep(2);
   printf("Lower Left Front\n");
   trigger_sound(127,sound,midiport, 1);

   sleep(2);
   printf("Lower Right Front\n");
   trigger_sound(127,sound,midiport, 0);
  
   sleep(2);
   printf("Lower Left Back\n");
   trigger_sound(127,sound,midiport, 3);

   sleep(2);
   printf("Lower Right Back\n");
   trigger_sound(127,sound,midiport, 2);

   sleep(2);
   printf("Upper Left Front\n");
   trigger_sound(127,sound,midiport, 5);

   sleep(2);
   printf("Upper Right Front\n");
   trigger_sound(127,sound,midiport, 4);
  
   sleep(2);
   printf("Upper Left Back\n");
   trigger_sound(127,sound,midiport, 7);

   sleep(2);
   printf("Upper Right Back\n");
   trigger_sound(127,sound,midiport, 6);

}

void loadbank( int midiport )
{
   cerr<<"Loading Bank 08"<<endl;
   send_midi_command( midiport, (unsigned char) 0xc0);
   send_midi_command( midiport, (unsigned char) 0x0f);
   send_midi_command( midiport, (unsigned char) 0xc0);
   send_midi_command( midiport, (unsigned char) 0x08);
   //send_midi_command( midiport, (unsigned char) 0x03);
   sleep(15); 
   cerr<<"Bank 08 Loaded"<<endl;
//   cerr<<"Select Supervisor mode on the EMAX II"<<endl;
//   cerr<<"Type g and hit enter when ready"<<endl;
//   cin>>v1;
//   send_midi_command( midiport, (unsigned char) 0xf3);
//   send_midi_command( midiport, (unsigned char) 0x01);
//   sleep(1);
//   send_midi_command( midiport, (unsigned char) 0xfa);
//   sleep(65);
//   send_midi_command( midiport, (unsigned char) 0xfc);
//   sleep(1);
//   send_midi_command( midiport, (unsigned char) 0xf3);
//   send_midi_command( midiport, (unsigned char) 0x00);
   
   
}
