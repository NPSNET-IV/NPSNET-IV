// File: <planes.cc>

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
// see net_open_select() call
#define DEBUG_A

#include <stdlib.h>
#include <math.h>
#include <iostream.h>
#include  <fstream.h>
#include <strings.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>

//Network include files
#include "disnetlib.h"

// Initialization Manager
#include "im.tab.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "imclass.h"

InitManager *initman;

#define X 0
#define Y 1
#define Z 2

#define MAX_AIR 50
typedef struct {
  float pos[3]; //where I am
  float orient[3]; // what is my orientation
  float velocity[3]; //what is my speed
} VEHSTATE;

typedef struct {
  EntityID disid;  //the DIS ID record
  EntityType distype;  //the DISType
  float cent[3];  //center of the circle
  float radius;   //radius of circle
  float speed;  //speed magnetuted
  float heading; //which way are we going
  VEHSTATE posture;
  float nexttime;
} AIRREC;

//prototypes
void readdatafile(char *);
void findsitehost(char *);
extern "C"{int gethostname ( char *, int );}
void turn_plane(int);
void moveaircraft(int,float);
void sendentitystate(int, DIS_net_manager *);

AIRREC air[MAX_AIR]; // the array of aircraft
#define INTERVAL 1.0f
#define TURNAMOUNT 1.0f //how much it turns every INTERVAL time

int num_air;  //number of aircraft being modeled
EntityID address;

main (int argc, char *argv[])
//generates a continous path for an airplane, or set of them traveling 
//along a flat circular path the parameters are the datafile
{

  float oldtime, curtime, deltatime,starttime;
  struct tms timebuffer; /*the time buffer, we will not use this*/
  int ix;

   // Multicast Defaults
   int multicast = 0;
   u_short port;
   char group[33];
   u_char ttl = DEFAULT_MC_TTL;
   port = 0;

   char net_interface[20];
   int exercise = 0;
   int round = 0;
   char rfile[512];

   DIS_net_manager *net;
   char entity_filename[1024];

   // START Initialization Manager routines
   union query_data local;
   initman = new InitManager();         // defaults assigned
   initman->process_Master_Configs(argc, argv);
   initman->process_environment_vars();
   initman->process_cl_args(argc, argv);
   initman->parse_input_files();
   initman->validate_data();
   initman->dump_inits();       // let's keep a record of what we've got
   cout.flush();
   // END Initialization Manager routines

   initman->query(ENTITY_FILE, &local, GET);
   strcpy ( entity_filename, local.string );
   if ( ( (strcmp(entity_filename, "NONE")) == 0 ) ||
        ( (strcmp(entity_filename, "")) == 0 ) ) {
      cerr << "Please supply configuration command ENTITY_FILE." << endl;
      cerr << "\tCurrent value is = " << entity_filename << endl;
      exit(0);
   }

   initman->query(IM_PORT, &local, GET);
   port = (u_short)local.ints[0];

   initman->query(MULTICAST, &local, GET);
   multicast = local.ints[0];
   initman->query(MULTICAST_TTL, &local, GET);
   ttl = (u_char) local.ints[0];
   initman->query(MULTICAST_GROUP, &local, GET);
   strncpy ( group, local.string, STRING32 );

   initman->query(DIS_EXERCISE, &local, GET);
   exercise = (u_char) local.ints[0];

   initman->query(ROUND_WORLD_FILE, &local, GET);
   strcpy ( rfile, local.string );
   round = (strcmp(rfile,"NONE") != 0);

   initman->query(ETHER_INTERF, &local, GET);
   strncpy ( net_interface, local.string, 10 );

   roundWorldStruct *rw = NULL;

   if (round) {
      rw = new roundWorldStruct;
      struct RW initManRW;
      initman->get_round_world_data(&initManRW);
      rw->northing = initManRW.northing;
      rw->easting = initManRW.easting;
      rw->o_zone_num = initManRW.zone_num;
      rw->o_zone_letter = initManRW.zone_letter;
      rw->map_datum = initManRW.map_datum;
      rw->width = initManRW.width;
      rw->height = initManRW.height;
   }

   if ( multicast )
      {
      if ( port == 0 )
         port = DEFAULT_MC_PORT;
      net = new DIS_net_manager ( group, ttl, port, exercise, round, rw,
                                  DEFAULT_BUF_SIZE, net_interface );
      }
   else
      {
      if ( port == 0 )
         port = DEFAULT_BC_PORT;
      net = new DIS_net_manager ( exercise, round, rw, DEFAULT_BUF_SIZE,
                                  net_interface, port );
      }

  readdatafile(entity_filename);

  if ( !net->net_open() )
    {
    cerr << "Network could not be opened." << endl;
    exit(0);
    }

  //get the curent time
  starttime = times(&timebuffer)/HZ;   
  oldtime = 0.0; 
  fprintf ( stderr, "%d airplanes being processed\n", num_air );
  while(TRUE){
    //get the time
    curtime = times(&timebuffer)/HZ - starttime;
    deltatime = curtime - oldtime;
    oldtime = curtime;


    //move the airplane
    for(ix=0;ix< num_air;ix++){
      //compute new position
      moveaircraft(ix,deltatime);
      if(air[ix].nexttime < curtime){
        turn_plane(ix);
        sendentitystate(ix, net);
        air[ix].nexttime = curtime + INTERVAL;
      }
    }
  }
}

void turn_plane(int ix)
//turn the plane by TURNAMOUNT and compute the velocity and orient parameters
{
  float rad_heading;

  air[ix].heading += TURNAMOUNT;
  rad_heading = air[ix].heading * 0.017453293;  //convert it radians
 
  air[ix].posture.velocity[X] = fcos(rad_heading) * air[ix].speed;
  air[ix].posture.velocity[Y] = fsin(rad_heading) * air[ix].speed;

  air[ix].posture.orient[Z] = rad_heading;
}  


void moveaircraft(int ix,float deltatime)
//since these are dumb aircraft, just use basic DR, and Z never changes
{
  air[ix].posture.pos[X] += air[ix].posture.velocity[X] * deltatime;
  air[ix].posture.pos[Y] += air[ix].posture.velocity[Y] * deltatime;
}
  

void readdatafile(char filename[])
//reads the datafile into the arrays
//file format is one plane per line with the following elements
//Entity_Number DIStype_spec circle_center (X, Y, Z)  Speed
{ 
 int ix = 0;
 unsigned int entity_kind,domain,country,category,subcategory,specific;

  ifstream datafile (filename);
  if (!datafile) {
    cerr << "Unable to open the datafile " << filename <<"\n";
    exit(0);
  }

  while (datafile >> air[ix].disid.entity){
    //read the file to load the array

    datafile >> entity_kind >> domain >> country >> 
                   category >> subcategory >> specific;
    //force the puppys to the right types
    air[ix].distype.entity_kind = (unsigned char) entity_kind;
    air[ix].distype.domain = (unsigned char) domain;
    air[ix].distype.country = (unsigned short) country;
    air[ix].distype.category = (unsigned char) category;
    air[ix].distype.subcategory = (unsigned char) subcategory;
    air[ix].distype.specific = (unsigned char) specific;

    datafile >> air[ix].cent[X] >> air[ix].cent[Y] >> air[ix].cent[Z] 
             >> air[ix].speed;

#ifdef DEBUG_A
    cerr<<"Entity   " << air[ix].disid.entity << " Index " << ix <<"\n";
    cerr<<"DIS Type "<< 0 + air[ix].distype.entity_kind <<" "
                     << 0 + air[ix].distype.domain <<" "
                     << 0 + air[ix].distype.country<<" "
                     << 0 + air[ix].distype.category<<" " 
                     << 0 + air[ix].distype.subcategory<<" "
                     << 0 + air[ix].distype.specific<< "\n"; 
#endif
    //assign it the rest of the address
    air[ix].disid.address.site = address.address.site;
    air[ix].disid.address.host = address.address.host;

    //set the times to send out the first set of PDUs
    //but let's stagger them so we don't have burst
    air[ix].nexttime = ix % (int)(INTERVAL); 

    //compute the intial position this is an approximation of a circle
    //for large TURNAMONTs it will result in offcenter figures
    air[ix].posture.pos[X] = air[ix].cent[X] +
        air[ix].speed* (INTERVAL - air[ix].nexttime);
    air[ix].posture.pos[Y] = air[ix].cent[Y] - 
             (360.0f / TURNAMOUNT) * air[ix].speed*INTERVAL*0.15915494f;
    air[ix].posture.pos[Z] = air[ix].cent[Z];
cerr << "Starting Position " << air[ix].posture.pos[X] <<"  "
                             << air[ix].posture.pos[Y] <<"  "
                             << air[ix].posture.pos[Z] <<"\n";     
    //the intail veloctiy is easy
    air[ix].posture.velocity[X] =  air[ix].speed;
    air[ix].posture.velocity[Y] = 0.0f;
    air[ix].posture.velocity[Z] = 0.0f;

    //as is the orientation
    air[ix].posture.orient[X] = 0.0f;
    air[ix].posture.orient[Y] = 0.0f;
    air[ix].posture.orient[Z] = 0.0f;
    air[ix].heading = 0.0f;

    //ok on to the next
    ix++;
    if(ix == MAX_AIR){
      cerr << "Too many aircraft requestd " << ix << " Only "<< MAX_AIR 
           << " allowed \n";
      exit(0);
    }
  }
  num_air = ix;
}
    

void findsitehost(char ether_interf[])
//find the site/host numbers and which interface this is
{
  char name[80],temp[80];
  int dummy;
  int success = FALSE;

  gethostname(name,80);

  ifstream hostfile ("./datafiles/hosts.dat");
  if (!hostfile)
    {
    cerr << "Unable to open ./datafiles/hosts.dat to start the network." 
         << endl;
    }
  else
    {
    //while ( !success && (hostfile >> temp) )
    while ( !success && (hostfile >> "HOST" >> temp) )
      {
      if(!strcmp(temp,name))
        {
        hostfile >> address.address.site >> address.address.host 
                 >> ether_interf;
#ifdef DEBUG_A
        cerr << "Using Site " << address.address.site
             << " Host " << address.address.host << endl;
 
#endif
        success = TRUE;
        }
      if ( !success )
         hostfile >> dummy >> dummy >> temp;
      }
    }

  if ( !success )
    {
    cerr << name << " was not found in the host table, ./datafiles/hosts.dat"
         << endl;
    cerr << "\tUsing Site 99 Host 99 and default ethernet interface." << endl;
    address.address.site = 99;
    address.address.host = 99;
    strcpy ( ether_interf, "" );
    }

}

void sendentitystate(int ix, DIS_net_manager *net )
//send an entity state PDU
{
  static EntityStatePDU epdu;
  char tempstr[50];
  

  //fill in the parameters of an entity state PDU, this assumes there
  //are no articulated parameters
  //ID and Type
  epdu.entity_id = air[ix].disid;
  epdu.entity_type = air[ix].distype;

  //How fast is it going
  epdu.entity_velocity.x = air[ix].posture.velocity[X];
  epdu.entity_velocity.y = air[ix].posture.velocity[Y];
  epdu.entity_velocity.z = air[ix].posture.velocity[Z];
  //Where we are
  epdu.entity_location.x = air[ix].posture.pos[X];
  epdu.entity_location.y = air[ix].posture.pos[Y];
  epdu.entity_location.z = air[ix].posture.pos[Z];
  //How we are facing
  epdu.entity_orientation.psi   = -(air[ix].posture.orient[Z] - 1.570793f);
  epdu.entity_orientation.theta =  air[ix].posture.orient[Y];
  epdu.entity_orientation.phi   =  air[ix].posture.orient[X];
  //Send out dr accelerations and velocities
  epdu.dead_reckon_params.linear_accel[X] = 0.0f;
  epdu.dead_reckon_params.linear_accel[Y] = 0.0f;
  epdu.dead_reckon_params.linear_accel[Z] = 0.0f;

  // We don't calculate these at the moment but for the future
  //epdu.dead_reckon_params.angular_velocity[HEADING] = 0.0f;
  //epdu.dead_reckon_params.angular_velocity[PITCH] = 0.0f;
  //epdu.dead_reckon_params.angular_velocity[ROLL] = 0.0f;

  //what we look like
  epdu.entity_appearance = 0x00020000;
  //project our movement
  epdu.dead_reckon_params.algorithm = DRAlgo_DRM_FPW;

  epdu.entity_marking.character_set = CharSet_ASCII;
  sprintf ( tempstr, "Planes#%d", air[ix].disid.entity );
  strncpy ( (char *)epdu.entity_marking.markings, tempstr,(MARKINGS_LEN-1) );

  if ( net->write_pdu((char *)&epdu,EntityStatePDU_Type) == FALSE)
  	printf("net_write() failed\n");

#ifdef DEBUG
  cerr << "sent a Vehicle Entity State PDU\n";
#endif

}


