
#include "nps_converter.h"
#include <pf.h>
#include <string.h>
#include <math.h>
#include <iostream.h>
#include "fileio.h"

#include "imstructs.h"
#include "imclass.h"

extern InitManager *initman;

void *
nps_converter::operator new ( size_t size )
{
   register nps_converter *temp = NULL;
   void *arena;

   arena = pfGetSharedArena();
   if ( arena != NULL ) {
      temp = (nps_converter *)pfCalloc(size,1,arena);
   }

   return temp;
}



void
nps_converter::operator delete ( void *p, size_t )
{
   pfFree ( p );
}


nps_converter::nps_converter ( const roundWorldStruct *rw )
{
   /*
   char    zone_letter_str[10];
   float   temp_float;
   int     temp_int;
   char    temp_str[255];
   */

   northing = (float64) rw->northing;
   easting = (float64) rw->easting;
   o_zone_num = (int32) rw->o_zone_num;
   o_zone_letter = rw->o_zone_letter;
   map_datum = (int32) rw->map_datum;
   width = (int32) rw->width;
   height = (int32) rw->height;
   valid_converter = rw->rw_valid;

/*
   ifstream coord_file ( utm_origin_file );
   
   valid_converter = TRUE;

   if (!coord_file)
      {
      valid_converter = FALSE;
      }
   else
      {
      while ( !read_float(coord_file,temp_float) &&
               read_string_to_EOL(coord_file, temp_str) );
      if ( !coord_file )
         {
         cerr << "ERROR:\tnps_converter format error in UTM "
              << "coordinate file, " << utm_origin_file 
              << ", with Northing." << endl;
         valid_converter = FALSE;
         }
      else
         {
         northing = temp_float;
         if ( read_float(coord_file, temp_float) )
            easting = temp_float;
         else
            {
            cerr << "ERROR:\tnps_converter format error in UTM "
                 << "coordinate file, " << utm_origin_file 
                 << ", with Easting." << endl;
            valid_converter = FALSE;
            }
         if ( valid_converter && read_int(coord_file, temp_int) )
            o_zone_num = temp_int;
         else
            {
            cerr << "ERROR:\tnps_converter format error in UTM "
                 << "coordinate file, " << utm_origin_file 
                 << ", with Zone Num." << endl;
            valid_converter = FALSE;
            }
         if ( valid_converter && read_string(coord_file, temp_str) )
            {
            strncpy ( zone_letter_str, temp_str, 9 );
            o_zone_letter = zone_letter_str[0];
            }
         else
            {
            cerr << "ERROR:\tnps_converter format error in UTM "
                 << "coordinate file, " << utm_origin_file 
                 << ", with Zone Let." << endl;
            valid_converter = FALSE;
            }
         if ( valid_converter && read_int(coord_file, temp_int) )
            map_datum = temp_int;
         else
            {
            cerr << "ERROR:\tnps_converter format error in UTM "
                 << "coordinate file, " << utm_origin_file 
                 << ", with Map Datum." << endl;
            valid_converter = FALSE;
            }
         if ( valid_converter && read_int(coord_file, temp_int) )
            width = temp_int;
         else
            {
            cerr << "ERROR:\tnps_converter format error in UTM "
                 << "coordinate file, " << utm_origin_file 
                 << ", with Width." << endl;
            valid_converter = FALSE;
            }
         if ( valid_converter && read_int(coord_file, temp_int) )
            height = temp_int;
         else
            {
            cerr << "ERROR:\tnps_converter format error in UTM "
                 << "coordinate file, " << utm_origin_file 
                 << ", with Height." << endl;
            valid_converter = FALSE;
            }
         }

      coord_file.close();
      }
*/
}


nps_converter::~nps_converter()
{
  valid_converter = FALSE;
}


int
nps_converter::nps_to_utm ( const float64 nps_X,
                            const float64 nps_Y,
                            float64 &utm_easting,
                            float64 &utm_northing )
{
   int success = valid_converter;

   if ( success ) {
      utm_easting = fmod ( (nps_X + easting), 1000000.00 );
      utm_northing = fmod ( (nps_Y + northing), 1000000.00 );;
   }
   else {
      utm_easting = nps_X;
      utm_northing = nps_Y;
   }

   return success;
}


int
nps_converter::utm_to_nps ( const float64 utm_easting,
                            const float64 utm_northing,
                            float64 &nps_X,
                            float64 &nps_Y )
{
   int success = valid_converter;

   if ( success ) {
      nps_X = utm_easting - easting;
      nps_Y = utm_northing - northing;
   }
   else {
      nps_X = utm_easting;
      nps_Y = utm_northing;
   }

   return success;
}


