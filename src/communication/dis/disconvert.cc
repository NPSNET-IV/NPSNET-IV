// File: <disconvert.cc>

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
/*
Note: Some of the code in these functions came from the Loral MODSAF libs and
other code came from IST via NRaD/ETA. I had to change a lot of it to suit our
needs so I have not kept entire functions. All the code was developed under
government contract. If you are really concerned about the legalities of the
code, call a lawyer and let me know what they say. For the most part, thanks
guys - Dave Pratt 
*/

#include "libcoordinates.h"
#include "libreader.h"
#include <stdext.h>
#include <stdio.h>
#include <math.h>
#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include "disconvert.h"

#include "imstructs.h"
#include "imclass.h"

extern InitManager *initman;

#ifndef NO_DVW
#include "env_pdu.h"
#endif

#define DEG_2_RAD 0.017453293
#define FILENAME_SIZE 255

COORD_TCC_PTR      L_utm_tcc;
  /* TCC coordinate transformation ptr */
int32 L_map_datum;
   /* the measure of how round the world is */

double L_cos_lat, L_sin_lat, L_cos_lon, L_sin_lon ;
double L_sin_sin, L_sin_cos, L_cos_sin, L_cos_cos ;
double L_latitude,L_longitude;

extern volatile int G_coord_error;

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/


/*                        Internal Functions                                 */


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

/* Local Prototypes */
COORD_TCC_PTR define_coord ( roundWorldStruct * );
void velocity_DIS_2_NPS ( VelocityVector * );
void velocity_NPS_2_DIS ( VelocityVector * );
void orientation_DIS_2_NPS ( EntityOrientation * );
void orientation_NPS_2_DIS ( EntityOrientation *orient );

void my_upcase ( char *data )
   {
   static char minch = 'a';
   static char maxch = 'z';
   static int offset = ('A' - 'a');
   if ( data != NULL )
      {
      char *lastchar = data + strlen ( data );
      for (char *curr1 = data; curr1 < lastchar; curr1++)
         {
         if ( ( *curr1 >= minch ) && ( *curr1 <= maxch ) )
            *curr1 += offset;
         }
      }
   }

void my_stripblanks ( char *data )
   {
   char *current = data;
   char *next = data;
   char *last = data + strlen(data);
   if ( data != NULL )
      {
      while ( next < last )
         {
         if ( *next != ' ' )
            *current++ = *next;
         next++;
         }
      *current = NULL;
      }
   }

int my_read_int ( ifstream &infile, int &temp_int )
   {
   infile >> temp_int;
   if (!infile)
      {
      infile.clear();
      return 0;
      }
   else
      return 1;
   }

int my_read_float ( ifstream &infile, float &temp_float )
   {
   infile >> temp_float;
   if (!infile)
      {
      infile.clear();
      return 0;
      }
   else
      return 1;
   }

int my_read_string ( ifstream &infile, char *temp_str )
   {
   if ( infile >> temp_str )
      {
      my_upcase ( temp_str );
      my_stripblanks ( temp_str );
      if (strlen(temp_str) == 0)
         return 0;
      else
         return 1;
      }
   else
      return 0;
   }

int my_read_string_to_EOL ( ifstream &infile, char *temp_str )
   {
   if ( infile.getline ( temp_str, FILENAME_SIZE) )
      {
      my_upcase ( temp_str );
      my_stripblanks ( temp_str );
      if (strlen(temp_str) == 0)
         return 0;
      else
         return 1;
      }
   else
      return 0;
   }

int my_read_filename ( ifstream &infile, char *filename )
   {
   if ( infile.getline ( filename, FILENAME_SIZE) )
      {
      my_stripblanks ( filename );
      if (strlen(filename) == 0)
         return 0;
      else
         return 1;
      }
   else
      return 0;
   }

COORD_TCC_PTR define_coord ( roundWorldStruct *rw )
{
   float64 northing;
   float64 easting;
   int32   o_zone_num;
   char    o_zone_letter;
   //char    zone_letter_str[10];
   int32   map_datum;
   int32   width;      /* East to West */
   int32   height;     /* South to North */
   /*
   float   temp_float;
   int     temp_int;
   char    temp_str[255];
   ifstream coord_file ( input_file );
   */

//   struct RW rw;

#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager starting define_coord()." << endl;
#endif

//   initman->get_round_world_data(&rw);
   northing = (float64) rw->northing;
   easting = (float64) rw->easting;
   o_zone_num = (int32) rw->o_zone_num;
   o_zone_letter = rw->o_zone_letter;
   map_datum = (int32) rw->map_datum;
   width = (int32) rw->width;
   height = (int32) rw->height;

#ifdef DEBUG
   cerr << "MAP=" << northing << ","
                  << easting << ","
                  << o_zone_num << ","
                  << o_zone_letter << ","
                  << map_datum << ","
                  << width << ","
                  << height << endl;
#endif // DEBUG

/*
   if (!coord_file)
      {
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager round coordinate conversion file, "
           << input_file << ", not found." << endl;
#endif
      return NULL;
      }
   else
      {
#ifdef DEBUG
      cerr << "DEBUG:	DIS_net_manager initializing round world"
           << " coordinates using file:  " << input_file << endl;
#endif
      while ( !my_read_float(coord_file,temp_float) &&
               my_read_string_to_EOL(coord_file, temp_str) )
         {
#ifdef DEBUG
            cerr << "DEBUG:	DIS_net_manager processed comment in "
                 << "round world coordinate file." << endl;
#endif
         }
      if ( !coord_file )
         {
         cerr << "ERROR:	DIS_net_manager format error in round "
              << "coordinate file, " << input_file << ", with Northing." << endl;
         return NULL;
         }
      else
         {
         northing = temp_float;
         if ( my_read_float(coord_file, temp_float) )
            easting = temp_float;
         else
            {
            cerr << "ERROR:	DIS_net_manager format error in round "
                 << "coordinate file, " << input_file << ", with Easting." << endl;
            return NULL;
            }
         if ( my_read_int(coord_file, temp_int) )
            o_zone_num = temp_int;
         else
            {
            cerr << "ERROR:	DIS_net_manager format error in round "
                 << "coordinate file, " << input_file << ", with Zone Num." << endl;
            return NULL;
            }
         if ( my_read_string(coord_file, temp_str) )
            {
            strncpy ( zone_letter_str, temp_str, 9 );
            o_zone_letter = zone_letter_str[0];
            }
         else
            {
            cerr << "ERROR:	DIS_net_manager format error in round "
                 << "coordinate file, " << input_file << ", with Zone Let." << endl;
            return NULL;
            }
         if ( my_read_int(coord_file, temp_int) )
            map_datum = temp_int;
         else
            {
            cerr << "ERROR:	DIS_net_manager format error in round "
                 << "coordinate file, " << input_file << ", with Map Datum." << endl;
            return NULL;
            }
         if ( my_read_int(coord_file, temp_int) )
            width = temp_int;
         else
            {
            cerr << "ERROR:	DIS_net_manager format error in round "
                 << "coordinate file, " << input_file << ", with Width." << endl;
            return NULL;
            }
         if ( my_read_int(coord_file, temp_int) )
            height = temp_int;
         else
            {
            cerr << "ERROR:	DIS_net_manager format error in round "
                 << "coordinate file, " << input_file << ", with Height." << endl;
            return NULL;
            }
         } 
         
      coord_file.close();
      }
*/

  // Define the coordinate system
   L_utm_tcc = coord_define_tcc ( COORD_UTM_NE,
                                  northing, easting,
                                  o_zone_num, o_zone_letter, map_datum,
                                  width, height );
#ifdef TRACE
   cerr << "TRACE:	DIS_net_manager finished define_coord()." << endl;
#endif

   return(L_utm_tcc);

}


void velocity_DIS_2_NPS (VelocityVector *velocity)
/*Convert the velocity vector from the DIS coordinate the NPS coodinate frame*/
   {
   float vel[3];
 
   /* multiplying the velocity vector in DIS with the rotation matrix */
   vel[0] =   velocity->x * (-L_sin_lon)
            + velocity->y * L_cos_lon ;
 
   vel[1] =   velocity->x * (-L_sin_cos)
            - velocity->y * L_sin_sin
            + velocity->z * L_cos_lat ;
 
   vel[2] =   velocity->x * L_cos_cos
            + velocity->y * L_cos_sin
            + velocity->z * L_sin_lat ;
 
   velocity->x = vel[0];
   velocity->y = vel[1];
   velocity->z = vel[2];
 
   } /* end of velocity_DIS_2_NPS */


void velocity_NPS_2_DIS(VelocityVector *velocity)
/*convert from the velocity vector to DIS coordinates*/
   {
   float vel[3];
 
   vel[0] = velocity->x;
   vel[1] = velocity->y;
   vel[2] = velocity->z;
 
   /* apply the rotation matrix to the velocity vector */
   velocity->x =  vel[0] * (-L_sin_lon)
                - vel[1] * L_sin_cos
                + vel[2] * L_cos_cos ;
 
   velocity->y =  vel[0] * L_cos_lon
                - vel[1] * L_sin_sin
                + vel[2] * L_cos_sin ;
 
   velocity->z =  vel[1] * L_cos_lat
                + vel[2] * L_sin_lat ;

//cerr << "In velocity_NPS_2_DIS" << endl 
//     << "\tL_sin_lon: "<< L_sin_lon << endl
//     << "\tL_sin_cos: " << L_sin_cos << endl
//     << "\tL_cos_cos: " << L_cos_cos << endl
//     << "\tL_cos_lon: " << L_cos_lon << endl
//     << "\tL_sin_sin: " << L_sin_sin << endl
//     << "\tL_cos_sin: " << L_cos_sin << endl
//     << "\tL_cos_lat: " << L_cos_lat << endl
//     << "\tL_sin_lat: " << L_sin_lat << endl
//     << "-=-=-=-=-=-=-=-=-=-=-=-" << endl;

 
 
   } /* end of velocity_NPS_2_DIS */

 
void orientation_DIS_2_NPS(EntityOrientation *orient)
/*convert the orientation from DIS to NPS local coordinate frame*/
   {
   float cosR,sinR,cosP,sinP,cosY,sinY ;
   float roll, pitch, yaw ; /* SIMNET's roll, pitch and yaw */
   float B_sub_11, B_sub_12 ;
   float B_sub_23, B_sub_33 ;
   float cosPcosY, cosPsinY ;
   float cosRsinY, cosRcosY, sinRsinY, sinRcosY ;

   /* calculate the trigonometric functions given the Y, P, and R */
   cosR = fcos(orient->phi);
   sinR = fsin(orient->phi);
   cosP = fcos(orient->theta);
   sinP = fsin(orient->theta);
   cosY = fcos(orient->psi);
   sinY = fsin(orient->psi);
 
   /* calculating pitch for NPSNET */
 
   pitch = fasin( L_cos_cos*cosP*cosY + L_cos_sin*cosP*sinY - L_sin_lat*sinP );
 
   /* calculating yaw for NPSNET */
 
   cosPcosY = cosP * cosY ;
   cosPsinY = cosP * sinY ;
 
   B_sub_11 = -L_sin_lon*cosPcosY + L_cos_lon*cosPsinY ;
   B_sub_12 = -L_sin_cos*cosPcosY - L_sin_sin*cosPsinY - L_cos_lat*sinP ;
 
   yaw = fatan2(B_sub_11,B_sub_12) ;
 
   /* calculating roll for NPSNET */
 
   cosRsinY = cosR * sinY ;
   cosRcosY = cosR * cosY ;
   sinRsinY = sinR * sinY ;
   sinRcosY = sinR * cosY ;
 
   B_sub_23 = -(  L_cos_cos*(-cosRsinY + (sinP*sinRcosY) )
                + L_cos_sin*( cosRcosY + (sinP*sinRsinY) )
                + L_sin_lat*( sinR*cosP                  ) ) ;
 
   B_sub_33 = -(  L_cos_cos*( sinRsinY + (sinP*cosRcosY) )
                + L_cos_sin*(-sinRcosY + (sinP*cosRsinY) )
                + L_sin_lat*( cosR*cosP                  ) ) ;
 
   roll = fatan2(B_sub_23,B_sub_33) ;
 
   /*put them back in the structure*/
   orient->psi = yaw;
   orient->theta = pitch;
   orient->phi = roll;
 
   } /* end of orientation_DIS_2_NPS */


void orientation_NPS_2_DIS(EntityOrientation *orient)
/*convert the orientation from NPS to DIS coodinate frames*/
   {
   double A_sub_12, A_sub_11 ;
   double A_sub_23, A_sub_33 ;
   float cosR,sinR,cosP,sinP,cosY,sinY;
   float cosRsinY,cosRcosY,sinRsinY,sinRcosY;
 
   /* Calculate the trig functions for NPSNET roll, pitch, and yaw */
   cosR = cosf(orient->phi);
   sinR = sinf(orient->phi);
   cosP = cosf(orient->theta);
   sinP = sinf(orient->theta);
   cosY = cosf(orient->psi);
   sinY = sinf(orient->psi);
 
   /* Calculate intermediate values */
   cosRsinY = cosR * sinY;
   cosRcosY = cosR * cosY;
   sinRsinY = sinR * sinY;
   sinRcosY = sinR * cosY;
 
   /* calculating pitch for DIS */
 
   orient->theta = fasin( (L_sin_lat*-sinP) - (L_cos_lat*cosY * cosP) );
 
   /* calculating yaw for DIS */
 
   A_sub_12=(L_cos_lon*sinY * cosP)-(L_sin_sin*cosY * cosP) - (L_cos_sin*-sinP);
   A_sub_11=(-L_sin_lon*sinY * cosP)-(L_sin_cos*cosY * cosP) - (L_cos_cos*-sinP);
   orient->psi = fatan2(A_sub_12,A_sub_11) ;
 
   /* calculating roll for DIS */
 
   A_sub_23 = (L_cos_lat*sinRcosY * sinP - cosRsinY) - (L_sin_lat*cosP * sinR) ;
   A_sub_33 = (L_cos_lat*cosRcosY * sinP + sinRsinY) - (L_sin_lat*cosP * cosR) ;
 
   orient->phi = fatan2(A_sub_23,A_sub_33) ;
   } /* end of orientation_NPS_2_DIS */


void find_lat_long_from_DIS(EntityLocation pos)
/*load the globals with the lat long routines that they all use*/
   {
   double lat,lon,alt;

   /*call the ModSaf convert routine*/
   /*convert the Coordinates to Geocentric*/
//   if ( L_map_datum > -1 ) {
      if(coord_convert(COORD_GCC,pos.x,pos.y,pos.z,
                       COORD_LATLON,&lat,&lon,&alt,L_map_datum))
         {
        /*error condition*/
         cerr << "ERROR:\tDIS_net_manager error converting DIS lat/long:  "
              << endl;
         cerr << "\tCoordinates:  " << pos.x << ", " << pos.y 
              << ", " << pos.z << endl;
         cerr << "	Map Datum:  " << L_map_datum << endl;
         G_coord_error = 1;
         L_latitude = 0.0;
         L_longitude = 0.0;
         }
      else 
        {
        /*convert it to radians*/
        L_latitude = lat*DEG_2_RAD;
        L_longitude  = lon*DEG_2_RAD;
        }
   /*}
   else {
   // Round world isn't defined
      L_latitude = 32.5f*DEG_2_RAD;
      L_longitude = -130.0f*DEG_2_RAD;
   }*/
   
   /*do all the precalculation*/
 
   /* obtain the cosines and sines of the latitude and longitude angles */
   L_cos_lat = cos(L_latitude);
   L_sin_lat = sin(L_latitude);
   L_cos_lon = cos(L_longitude);
   L_sin_lon = sin(L_longitude);

//cerr << "\tlat: " << lat << endl
//     << "\tlon: " << lon << endl
//     << "\tL_latitude: " << L_latitude << endl
//     << "\tL_longitude: " << L_longitude << endl;
//cerr << "pos.x: " << pos.x << " pos.y: " << pos.y << " pos.z: " << pos.z << endl;
 
   /* precalculate the variables */
   L_sin_sin = L_sin_lat * L_sin_lon ;
   L_sin_cos = L_sin_lat * L_cos_lon ;
   L_cos_sin = L_cos_lat * L_sin_lon ;
   L_cos_cos = L_cos_lat * L_cos_lon ;

   }/*end of find_lat_long_from_DIS*/


void find_lat_long_from_NPS(EntityLocation pos)
/*load the globals with the lat long routines that they all use*/
   {
   double lat,lon,alt;
   
//   if ( L_map_datum > -1 ) {
      /*call the ModSaf convert routine*/
      if(coord_convert(COORD_TCC,L_utm_tcc,pos.x,pos.y,pos.z,
                      COORD_LATLON,&lat,&lon,&alt,L_map_datum))
         {
         /*error condition*/
         cerr << "ERROR:\tDIS_net_manager error converting NPS lat/long:  "
              << endl;
         cerr << "\tCoordinates:  " << pos.x << ", " << pos.y 
              << ", " << pos.z << endl;
         cerr << "	Map Datum:  " << L_map_datum << endl;
         G_coord_error = 1;
         L_latitude = 0.0;
         L_longitude = 0.0;
         }
      else
         {
         /*convert it to radians*/
         L_latitude = lat*DEG_2_RAD;
         L_longitude  = lon*DEG_2_RAD;
         }
   /*}
   else {
      L_latitude = 32.5f*DEG_2_RAD;
      L_longitude = -130.0f*DEG_2_RAD;
   }*/

   /*convert it to radians*/
   L_latitude = lat*DEG_2_RAD;
   L_longitude  = lon*DEG_2_RAD;
 
   /*do all the precalculation*/
 
   /* obtain the cosines and sines of the latitude and longitude angles */
   L_cos_lat = cos(L_latitude);
   L_sin_lat = sin(L_latitude);
   L_cos_lon = cos(L_longitude);
   L_sin_lon = sin(L_longitude);
 
   /* precalculate the variables */
   L_sin_sin = L_sin_lat * L_sin_lon ;
   L_sin_cos = L_sin_lat * L_cos_lon ;
   L_cos_sin = L_cos_lat * L_sin_lon ;
   L_cos_cos = L_cos_lat * L_cos_lon ;
 
   }/*end find_lat_long_from_NPS*/



/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/


/*                        External Functions                                 */


/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/


int init_round_world ( roundWorldStruct *input_file )
   {
//   if (L_utm_tcc == NULL) {
   reader_init(0);
   if (coord_init("./datafiles", 0) != 0)
      {
      cerr << "ERROR:	DIS_net_manager unable to initialize round world "
           << "coordinate transformations." << endl;
      return FALSE;
      }
   if ( (L_utm_tcc = define_coord(input_file)) == NULL )
      {
      cerr << "ERROR:	DIS_net_manager unable to define round world "
           << "TCC coordinate." << endl;
      return FALSE;
      }
   // }
   return TRUE;
   }


void convert_fire_to_round ( FirePDU *pdu )
/*Fire outgoing*/
/* convert location_in_world and velocity */
   {
   EntityLocation pos;

   /*call the setup routine*/
   find_lat_long_from_NPS(pdu->location_in_world);
  
   /* convert location_in_world and velocity */
   if(coord_convert(COORD_TCC,L_utm_tcc,
                    pdu->location_in_world.x,
                    pdu->location_in_world.y,
                    pdu->location_in_world.z,
                    COORD_GCC,&pos.x,&pos.y,&pos.z))
      {
      cerr << "ERROR:	DIS_net_manager converting fire PDU to round:" << endl;
      cerr << "	Invalid coordinates:  "
           << pdu->location_in_world.x << ", "
           << pdu->location_in_world.y << ", "
           << pdu->location_in_world.z << endl;
      G_coord_error = 1;
      pos.x = pos.y = pos.z = 0.0;
      }

   pdu->location_in_world.x = pos.x;
   pdu->location_in_world.y = pos.y;
   pdu->location_in_world.z = pos.z;

   /*convert the velocity*/
   velocity_NPS_2_DIS(&(pdu->velocity));
  
   }


void convert_entity_state_to_round ( EntityStatePDU *pdu )
/* convert entity_location, entity_velocity and entity_orientation */
   {
   EntityLocation pos;

   /*call the setup routine*/
   find_lat_long_from_NPS(pdu->entity_location);

   /* convert location_in_world and velocity */

   if(coord_convert(COORD_TCC,L_utm_tcc,
                    pdu->entity_location.x,
                    pdu->entity_location.y,
                    pdu->entity_location.z,
                    COORD_GCC,&pos.x,&pos.y,&pos.z))
      {
      cerr << "ERROR:	DIS_net_manager converting Entity State PDU to round:" 
           << endl;
      cerr << "	Invalid coordinates:  "
           << pdu->entity_location.x << ", "
           << pdu->entity_location.y << ", "
           << pdu->entity_location.z << endl;
      G_coord_error = 1;
      pos.x = pos.y = pos.z = 0.0;
      }

   pdu->entity_location.x = pos.x;
   pdu->entity_location.y = pos.y;
   pdu->entity_location.z = pos.z;

   /*convert the velocity*/
   velocity_NPS_2_DIS(&(pdu->entity_velocity));

   /*convert the orientation*/
   orientation_NPS_2_DIS(&(pdu->entity_orientation));
  
   }

#ifndef NO_DVW
void convert_environment_to_round ( EnvironmentalPDU * )
/* convert environment_location, environment_velocity and environment_orientation */
{
// need to fill in later

}
#endif

void convert_location_to_round ( double *x, double *y, double *z )
   {
   EntityLocation pos;

   /*call the setup routine*/
   pos.x = *x;
   pos.y = *y;
   pos.z = *z;
   find_lat_long_from_NPS(pos);

   /* convert location_in_world */
   if(coord_convert(COORD_TCC,L_utm_tcc,
                    pos.x,
                    pos.y,
                    pos.z,
                    COORD_GCC,x,y,z))
      {
      cerr << "ERROR:   Converting location to round:"
           << endl;
      cerr << " Invalid coordinates:  "
           << pos.x << ", "
           << pos.y << ", "
           << pos.z << endl;
      G_coord_error = 1;
      *x = *y = *z = 0.0;
      }

   }

void convert_detonation_to_round ( DetonationPDU *pdu )
/* convert location_in_world and velocity */
   {
   EntityLocation pos;

   /*call the setup routine*/
   find_lat_long_from_NPS(pdu->location_in_world);
 
   /* convert location_in_world and velocity */
   if(coord_convert(COORD_TCC,L_utm_tcc,
                    pdu->location_in_world.x,
                    pdu->location_in_world.y,
                    pdu->location_in_world.z,
                    COORD_GCC,&pos.x,&pos.y,&pos.z))
      {
      cerr << "ERROR:	DIS_net_manager converting Detonation PDU to round:" 
           << endl;
      cerr << "	Invalid coordinates:  "
           << pdu->location_in_world.x << ", "
           << pdu->location_in_world.y << ", "
           << pdu->location_in_world.z << endl;
      G_coord_error = 1;
      pos.x = pos.y = pos.z = 0.0;
      }


   pdu->location_in_world.x = pos.x;
   pdu->location_in_world.y = pos.y;
   pdu->location_in_world.z = pos.z;

   /*convert the velocity*/
   velocity_NPS_2_DIS(&(pdu->velocity));

   }



void convert_fire_from_round ( FirePDU *pdu )
/*Fire incoming*/
/* convert location_in_world and velocity */
   {
   EntityLocation pos;

   /*call the setup routine*/
   find_lat_long_from_DIS(pdu->location_in_world);

   /* convert location_in_world and velocity */
   if(coord_convert(COORD_GCC,
                    pdu->location_in_world.x,
                    pdu->location_in_world.y,
                    pdu->location_in_world.z,
                    COORD_TCC,L_utm_tcc,&pos.x,&pos.y,&pos.z))
      {
      cerr << "ERROR:	DIS_net_manager converting Fire PDU from round:"
           << endl;
      cerr << "	Invalid coordinates:  "
           << pdu->location_in_world.x << ", "
           << pdu->location_in_world.y << ", "
           << pdu->location_in_world.z << endl;
      G_coord_error = 1;
      pos.x = pos.y = pos.z = 0.0;
      }

   pdu->location_in_world.x = pos.x;
   pdu->location_in_world.y = pos.y;
   pdu->location_in_world.z = pos.z;

   /*convert the velocity*/
   velocity_DIS_2_NPS(&(pdu->velocity));

   }


void convert_entity_state_from_round ( EntityStatePDU *pdu )
/*Entity update coming in*/
/* convert entity_location, entity_velocity and entity_orientation */
   {
   EntityLocation pos;

   /*call the setup routine*/
   find_lat_long_from_DIS(pdu->entity_location);

   /* convert location_in_world and velocity */
   if(coord_convert(COORD_GCC,
                    pdu->entity_location.x,
                    pdu->entity_location.y,
                    pdu->entity_location.z,
                    COORD_TCC,L_utm_tcc,&pos.x,&pos.y,&pos.z))
      {
      cerr << "ERROR:	DIS_net_manager converting Entity State PDU from round:"
           << endl;
      cerr << "	Invalid coordinates:  "
           << pdu->entity_location.x << ", "
           << pdu->entity_location.y << ", "
           << pdu->entity_location.z << endl;
      G_coord_error = 1;
      pos.x = pos.y = pos.z = 0.0;
      }

   pdu->entity_location.x = pos.x;
   pdu->entity_location.y = pos.y;
   pdu->entity_location.z = pos.z;

   /*convert the velocity*/
   velocity_DIS_2_NPS(&(pdu->entity_velocity));

   /*convert the orientation*/
   orientation_DIS_2_NPS(&(pdu->entity_orientation));

   }

#ifndef NO_DVW
void convert_environment_from_round ( EnvironmentalPDU *pdu )
/*Entity update coming in*/
/* convert entity_location, entity_velocity and entity_orientation */
{
    ConeVariant        *cone;
    VdustVariant       *vdust;
    PlumeVariant       *plume;
    EntityLocation     pos;

    // if ( L_utm_tcc ) {

       switch(pdu->env_record[0].record_kind) {
   
          case WeatherPDU_Type:
             break;
   
          case PuffPDU_Type:
          case SmokePDU_Type:
   
             plume = &(pdu->env_record[1].variant.gaussian_plume);
             if(coord_convert(COORD_GCC,
                       plume->location.x,
                       plume->location.y,
                       plume->location.z,
                       COORD_TCC,L_utm_tcc,&pos.x,&pos.y,&pos.z)) {
                cerr << "ERROR:   DIS_net_manager converting Environment PDU from round:"
                     << endl;
                cerr << " Invalid coordinates:  "
                     << plume->location.x << ", "
                     << plume->location.y << ", "
                     << plume->location.z << endl;
                G_coord_error = 1;
                pos.x = pos.y = pos.z = 0.0;
             }
   
             plume->location.x = pos.x;
             plume->location.y = pos.y;
             plume->location.z = pos.z;
   // cerr << "NPS smoke loc: " << pos.x << " " << pos.y << " " << pos.z << endl;
             break;
   
          case FlarePDU_Type:
   
             cone = &(pdu->env_record[1].variant.cone);
             if(coord_convert(COORD_GCC,
                       cone->location.x,
                       cone->location.y,
                       cone->location.z,
                       COORD_TCC,L_utm_tcc,&pos.x,&pos.y,&pos.z)) {
                cerr << "ERROR:   DIS_net_manager converting Environment PDU from round:"
                     << endl;
                cerr << " Invalid coordinates:  "
                     << cone->location.x << ", "
                     << cone->location.y << ", "
                     << cone->location.z << endl;
                G_coord_error = 1;
                pos.x = pos.y = pos.z = 0.0;
             }
   
             cone->location.x = pos.x;
             cone->location.y = pos.y;
             cone->location.z = pos.z;
   // cerr << "NPS Flare loc: " << pos.x << " " << pos.y << " " << pos.z << endl;
             break;
   
          case VdustPDU_Type:
   
             for(int k=0; k<pdu->number_of_records; k++) {
                vdust = &(pdu->env_record[k].variant.vehicle_dust);
                if(coord_convert(COORD_GCC,
                       cone->location.x,
                       cone->location.y,
                       cone->location.z,
                       COORD_TCC,L_utm_tcc,&pos.x,&pos.y,&pos.z)) {
                   cerr << "ERROR:   DIS_net_manager converting Environment PDU from round:"
                        << endl;
                   cerr << " Invalid coordinates:  "
                        << cone->location.x << ", "
                        << cone->location.y << ", "
                        << cone->location.z << endl;
                   G_coord_error = 1;
                   pos.x = pos.y = pos.z = 0.0;
                }
   
                vdust->location.x = pos.x;
                vdust->location.y = pos.y;
                vdust->location.z = pos.z;
              }
   // cerr << "NPS dust loc: " << pos.x << " " << pos.y << " " << pos.z << endl;
              break;
   
       }
    //}
}
#endif


void convert_location_from_round ( double *x, double *y, double *z )
   {
   EntityLocation pos;

   /*call the setup routine*/
   pos.x = *x;
   pos.y = *y;
   pos.z = *z;
   find_lat_long_from_DIS(pos);

   /* convert location_in_world and velocity */
   if(coord_convert(COORD_GCC,
                    pos.x,
                    pos.y,
                    pos.z,
                    COORD_TCC,L_utm_tcc,x,y,z))
      {
      cerr << "ERROR:   Converting position from round:"
           << endl;
      cerr << " Invalid coordinates:  "
           << pos.x << ", "
           << pos.y << ", "
           << pos.z << endl;
      G_coord_error = 1;
      *x = *y = *z = 0.0;
      }

   }


void convert_detonation_from_round ( DetonationPDU *pdu )
/*Impact incoming, better duck*/
/* convert location_in_world and velocity */
   {
   EntityLocation pos;

   /*call the setup routine*/
   find_lat_long_from_DIS(pdu->location_in_world);

   /* convert location_in_world and velocity */
   if(coord_convert(COORD_GCC,
                    pdu->location_in_world.x,
                    pdu->location_in_world.y,
                    pdu->location_in_world.z,
                    COORD_TCC,L_utm_tcc,&pos.x,&pos.y,&pos.z))
      {
      cerr << "ERROR:   DIS_net_manager converting Entity State PDU from round:"
           << endl;
      cerr << " Invalid coordinates:  "
           << pdu->location_in_world.x << ", "
           << pdu->location_in_world.y << ", "
           << pdu->location_in_world.z << endl;
      G_coord_error = 1;
      pos.x = pos.y = pos.z = 0.0;
      }

   pdu->location_in_world.x = pos.x;
   pdu->location_in_world.y = pos.y;
   pdu->location_in_world.z = pos.z;

   /*convert the velocity*/
   velocity_DIS_2_NPS(&(pdu->velocity));

   }

/*EOF*/
