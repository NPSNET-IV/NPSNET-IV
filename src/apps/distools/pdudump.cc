// File: <dump.cc>

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

#include <iostream.h>
#include <fstream.h>
#include <stream.h>
#include <strstream.h>
#include <iomanip.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <bstring.h>
#include <signal.h>

#include "disnetlib.h"

// Initialization Manager
#include "im.tab.h"
#include "imstructs.h"
#include "imdefaults.h"
#include "imclass.h"

InitManager *initman;

#define DEG_TO_RAD 0.017453292f
#define RAD_TO_DEG 57.29577951f

#define MAX_LETTERS 70

int G_exit_flag = FALSE;

void
handler(int sig, ...)
   {
   if ( !G_exit_flag )
      {
      signal ( sig, SIG_IGN );
      switch ( sig )
         {
         case SIGTERM:
         case SIGHUP:
         case SIGINT:
         case SIGQUIT:
            break;
         default:
            cerr << "DEATH NOTICE:\tpdudisplay exitting due to signal:  "
                 << sig << endl;
            break;
         }
      G_exit_flag = TRUE;
      signal ( sig, (void (*)(int))handler );
      }
   }

void output_PDUHeader ( PDUHeader *header, char *leader )
   {

   cout << leader << "PDU HEADER:" << endl;
   cout << leader << "   Protocol Version:         " 
        << (int)header->protocol_version << endl;
   cout << leader << "   Exercise Identification:  " 
        << (int)header->exercise_ident << endl;
   cout << leader << "   Type:                     "
        << (int)header->type << endl;
   cout << leader << "   Protocol Family:          "
        << (int)header->protocol_family << endl;
   cout << leader << "   Time Stamp:               "
        << (int)header->time_stamp << endl;
   cout << leader << "   Length:                   "
        << (int)header->length << endl;

   }

 

void output_EntityID ( EntityID *id, char *leader )
   {

   cout << leader << "Entity ID:"
        << "  Site " << (int)id->address.site
        << ", Host " << (int)id->address.host
        << ", Entity " << (int)id->entity << endl;

   }


void output_EntityType ( EntityType *et, char *leader )
   {

   cout << leader << "Entity Type:"
        << "  Kind " << (int)et->entity_kind
        << ", Dom " << (int)et->domain
        << ", Count " << (int)et->country
        << ", Cat " << (int)et->category
        << ", Scat " << (int)et->subcategory
        << ", Spec " << (int)et->specific << endl;   

   }


void output_EventID ( EventID *eid, char *leader )
   {

   cout << leader << "Event ID:"
        << "  Site " << (int)eid->address.site
        << ", Host " << (int)eid->address.host
        << ", Event " << (int)eid->event << endl;

   }


void output_ForceID ( ForceID *force, char *leader )
   {

   cout << leader << "Force:  ";
   switch ( *force )
      {
      case (unsigned char)ForceID_Friendly:
         cout << "Friendly (Blue) = ";
         break;
      case (unsigned char)ForceID_Opposing:
         cout << "Opposing (Red) = ";
         break;
      case (unsigned char)ForceID_Neutral:
         cout << "Neutral (White) = ";
         break;
      default:
         cout << "Other = ";
         break;
      }
   cout << (unsigned int)*force << endl;
   
   }


void output_VelocityVector ( VelocityVector *velocity, char *leader )
   {

   cout << leader << "Velocity = "
        << velocity->x << ", "
        << velocity->y << ", "
        << velocity->z << ".\n";

   }


void output_EntityLocation ( EntityLocation *location, char *leader )
   {

   cout << leader << "Location = "
        << location->x << ", "
        << location->y << ", "
        << location->z << ".\n";

   }


void output_EntityOrientation ( EntityOrientation *orient, char *leader )
   {

   cout << leader << "Orientation = "
        << orient->psi   << "(" << orient->psi*RAD_TO_DEG   << "deg),"
        << orient->theta << "(" << orient->theta*RAD_TO_DEG << "deg),"
        << orient->phi   << "(" << orient->phi*RAD_TO_DEG   << "deg).\n";

   }


void output_EntityAppearance ( unsigned int *appear, char *leader )
   {

/*
   cout << leader << "Appearance (in Hex):  " << hex 
        << *appear << dec << endl;
*/
  cout << leader << "Appearance (in Hex):  " << form("%08X",*appear)
         << endl;

   }


void output_Capabilities ( unsigned int *capab, char *leader )
   {

   cout << leader << "Capabilities (in Hex):  " << form("%08X",*capab)
        << endl;

   }


void output_EntityMarking ( EntityMarking *marking, char *leader )
   {
   unsigned char *current;
   int count = 0;

   cout << leader << "Marking:  '";
   if ( marking->character_set == (CharacterSet)CharSet_ASCII )
      {
      current = marking->markings;
      while ( (*current != (unsigned char)0) &&
              (count < MARKINGS_LEN) )
         {
         cout << (char)*current;
         current++;
         count++;
         }
      cout << "'";
      if ( count == 0 )
         cout << " <None>";
      }
   else
      cout << "<Not ASCII>";
   cout << endl;

   }


void output_ArticulatParams ( ArticulatParams artic, char *leader )
   {

   float tempfloat;

   bcopy ( &(artic.parameter_value),
           &tempfloat, 4);

   cout << leader;
   cout << "Designator: " << (int)artic.parameter_type_designator
        << ", Change: " << (int)artic.change
        << ", ID: " << (int)artic.ID
        << ", Type: " << (int)artic.parameter_type
        << ", Value: " << tempfloat
                       << "(" << RAD_TO_DEG*tempfloat << "deg).\n";

   }


void output_ArticulatParamsList ( ArticulatParams *list, 
                                  unsigned char num,
                                  char *leader )
   {
   char lead[120];
   char temp[140];
   char number[5];
   int  count = 0;

   strcpy ( lead, leader );
   strcat ( lead, "   " );
   cout << leader << (int)num << " Articulated Parameters.\n";
   while ( count < int(num) )
      {
      count++;
      sprintf ( number, "#%d: ", count ); 
      strcpy ( temp, lead );
      strcat ( temp, number );
      output_ArticulatParams ( list[count-1], temp );
      } 
   
   }


void output_DeadReckonParams ( DeadReckonParams *drp, char *leader )
   {

   cout << leader << "Dead Reckon Parameters:\n";
   cout << leader << "   Algorithm:  " << int(drp->algorithm) << endl;
   cout << leader << "   Linear Acceleration: "
        << drp->linear_accel[0] << ", "
        << drp->linear_accel[1] << ", "
        << drp->linear_accel[2] << ".\n";
   cout << leader << "   Angular Velocity: " 
        << drp->angular_velocity[0] << ", "
        << drp->angular_velocity[1] << ", "
        << drp->angular_velocity[2] << ".\n";

   }

void output_Env_model (ENV_MODEL_TYPE model,char *leader )
{
   switch (model) {
      case ENV_WEATHER_MODEL_SIMPLE:
         cout << leader << "ENV_WEATHER_MODEL_SIMPLE\n";
         break;
      case ENV_EPHEMERIS_MODEL_SIMPLE:
         cout << leader << "ENV_EPHEMERIS_MODEL_SIMPLE\n";
         break;
      case ENV_SMOKE_MODEL_OTHER:
         cout << leader << "ENV_SMOKE_MODEL_OTHER\n";
         break;
      case ENV_SMOKE_MODEL_SIMPLE:
         cout << leader << "ENV_SMOKE_MODEL_SIMPLE\n";
         break;
      case ENV_SMOKE_MODEL_ATLAS:
         cout << leader << "ENV_SMOKE_MODEL_ATLAS\n";
         break;
      case ENV_SMOKE_MODEL_COMBIC:
         cout << leader << "ENV_SMOKE_MODEL_COMBIC\n";
         break;
      case ENV_OBSCURANT_MODEL_OTHER:
         cout << leader << "ENV_OBSCURANT_MODEL_OTHER\n";
         break;
      case ENV_OBSCURANT_MODEL_SIMPLE:
         cout << leader << "ENV_OBSCURANT_MODEL_SIMPLE\n";
         break;
      default:
         cout << leader << "*** ERROR ****\n";
   }
}


void output_Env_status (ENV_STATUS_TYPE status,char *leader )
{
   switch (status) {
      case ENV_STATUS_OFF:
        cout << leader << "Off\n";
         break;
      case ENV_STATUS_ON:
        cout << leader << "ON\n";
         break;
      default:
         cout << leader << "*** ERROR ****\n";
   }
}

void output_ENV_STATE_SIMPLE_WEATHER_KIND (
     ENV_STATE_SIMPLE_WEATHER_VARIANT *rec, char *leader)
{
   cout << leader << "Simulation Time "<< rec->simulation_time << "\n";
   cout << leader << "Precipitation Type: ";
   switch (rec->precipitation_type) {
       case ENV_WEATHER_PRECIPITATION_NONE:
          cout << "None\n";
          break;
       case ENV_WEATHER_PRECIPITATION_OTHER:
          cout << "Other (Dogs and cats?)\n";
          break;
       case ENV_WEATHER_PRECIPITATION_RAIN:
          cout << "Rain\n";
          break;
       case ENV_WEATHER_PRECIPITATION_SLEET:
          cout << "Sleet\n";
          break;
       case ENV_WEATHER_PRECIPITATION_HAIL:
          cout << "Hail\n";
          break;
       case ENV_WEATHER_PRECIPITATION_SNOW:
          cout << "Snow\n";
          break;
       default:
          cout << "*** Error ***\n";
          break;
   }
   cout << leader << "Precipitation Rate: " << rec->precipitation_rate << "\n";
   cout << leader << "Extinction Type: ";
   switch (rec->extinction_type){
       case ENV_WEATHER_EXTINCTION_NONE:
          cout << "None\n";
          break;
       case ENV_WEATHER_EXTINCTION_OTHER:
          cout << "Other\n";
          break;
       case ENV_WEATHER_EXTINCTION_RURAL:
          cout << "Rural\n";
          break;
       case ENV_WEATHER_EXTINCTION_MARITIME:
          cout << "Maritime\n";
          break;
       case ENV_WEATHER_EXTINCTION_URBAN:
          cout << "Urban";
          break;
       case ENV_WEATHER_EXTINCTION_TROPOSPHERIC:
          cout << "Tropospheric\n";
          break;
       case ENV_WEATHER_EXTINCTION_ADVECTIVEFOG:
          cout << "Advective Fog\n";
          break;
       case ENV_WEATHER_EXTINCTION_RADIATIVEFOG:
          cout << "Radiative Fog\n";
          break;
       case ENV_WEATHER_EXTINCTION_DESERT:
          cout << "Desert\n";
          break;
       case ENV_weatherExtinctionDesertSummer:
          cout << "Desert Summer\n";
          break;
       case ENV_weatherExtinctionDesertWinter:
          cout << "Desert Winter\n";
          break;
       case ENV_weatherExtinctionTemperateSummerDay:
          cout << "Summer Day\n";
          break;
       case ENV_weatherExtinctionTemperateSummerNight:
          cout << "Summer night\n";
          break;
       case ENV_weatherExtinctionTemperateWinter:
          cout << "Winter\n";
          break;
       default:
          cout << "*** Error *** "<< rec->extinction_type << "\n";
          break;
   }
  cout << leader << "Extinction Amount: "<<  rec->extinction_amount << " km\n";
  cout << leader << "Temperature: "<<  rec->temperature << " degrees C\n";
  cout << leader << "Dewpoint: "<<  rec->dewpoint << " degrees C\n";
  cout << leader << "Relative Humidity: "<<  rec->relative_humidity << "\n";
  cout << leader << "Barometric Pressure: "<<  rec->barometric_pressure<< "\n";
  cout << leader << "Ambient Light: " <<  rec->ambient_light<< "\n";
  cout << leader << "Wind: " << rec->wind[0] << ", " 
                 << rec->wind[1] << ", " << rec->wind[2]<< " m/s \n";
  cout << leader << "Cloud Cover: % "<< rec->cloud_cover
                 << " Top: " << rec->cloud_top 
                 << " Base: " << rec->cloud_base << " meters ASL\n";
  cout << "\n";

}


void output_ENV_GEOMETRY_UNIFORM_KIND (ENV_GEOMETRY_UNIFORM_VARIANT *,
  char *leader)
{
  //This record contains nothing but padding?
  cout << leader << "This record contains nothing put padding\n";
  cout << "\n";

}

void output_Env_record (ENV_RECORD *rec,char *leader )
{
   switch (rec->record_kind) {
      case ENV_PADDING_KIND:
         cout << leader << "ENV_PADDING_KIND\n";
         break;
      case ENV_STATE_SIMPLE_ILLUMINATION_KIND:
         cout << leader << "ENV_STATE_SIMPLE_ILLUMINATION_KIND\n";
         break;
      case ENV_STATE_SIMPLE_SOUND_KIND:
         cout << leader << "ENV_STATE_SIMPLE_SOUND_KIND\n";
         break;
      case ENV_STATE_SIMPLE_OBSCURANT_KIND:
         cout << leader << "ENV_STATE_SIMPLE_OBSCURANT_KIND\n";
         break;
      case ENV_STATE_SIMPLE_WEATHER_KIND:
         cout << leader << "ENV_STATE_SIMPLE_WEATHER_KIND\n";
         output_ENV_STATE_SIMPLE_WEATHER_KIND (&(rec->variant.simple_weather),
                                              leader);
         break;
      case ENV_STATE_COMBIC_PLUME_KIND:
         cout << leader << "ENV_STATE_COMBIC_PLUME_KIND\n";
         break;
      case ENV_STATE_COMBIC_PUFF_KIND:
         cout << leader << "ENV_STATE_COMBIC_PUFF_KIND\n";
         break;
      case ENV_STATE_EPHEMERIS_KIND:
         cout << leader << "ENV_STATE_EPHEMERIS_KIND\n";
         break;
      case ENV_STATE_FLARE_KIND:
         cout << leader << "ENV_STATE_FLARE_KIND\n";
         break;
      case ENV_STATE_SIMULATION_DATE_KIND:
         cout << leader << "ENV_STATE_SIMULATION_DATE_KIND\n";
         break;
      case ENV_VEHICLE_DUST_KIND:
         cout << leader << "ENV_VEHICLE_DUST_KIND\n";
         break;
      case ENV_GEOMETRY_UNIFORM_KIND:
         cout << leader << "ENV_GEOMETRY_UNIFORM_KIND\n";
         output_ENV_GEOMETRY_UNIFORM_KIND (&(rec->variant.uniform),
                                              leader);
         break;
      case ENV_GEOMETRY_POINT_KIND:
         cout << leader << "ENV_GEOMETRY_POINT_KIND\n";
         break;
      case ENV_GEOMETRY_SPHEROID_KIND:
         cout << leader << "ENV_GEOMETRY_SPHEROID_KIND\n";
         break;
      case ENV_GEOMETRY_POLYGON_KIND:
         cout << leader << "ENV_GEOMETRY_POLYGON_KIND\n";
         break;
      case ENV_GEOMETRY_EXTENTS_KIND:
         cout << leader << "ENV_GEOMETRY_EXTENTS_KIND\n";
         break;
      case ENV_GEOMETRY_SPHERICAL_COORDS_KIND:
         cout << leader << "ENV_GEOMETRY_SPHERICAL_COORDS_KIND\n";
         break;
      case ENV_GEOMETRY_GAUSSIAN_PLUME_KIND:
         cout << leader << "ENV_GEOMETRY_GAUSSIAN_PLUME_KIND\n";
         break;
      case ENV_GEOMETRY_GAUSSIAN_PUFF_KIND:
         cout << leader << "ENV_GEOMETRY_GAUSSIAN_PUFF_KIND\n";
         break;
      case ENV_GEOMETRY_CONE_KIND:
         cout << leader << "ENV_GEOMETRY_CONE_KIND\n";
         break;
      default:
         cout << leader << "*** ERROR ****\n";
   }
}

    



void output_EntityStatePDU ( EntityStatePDU *es, sender_info sender,
                             char *leader )
   {

   char lead[120];
   char temp[140];

   strcpy ( lead, leader );
   strcat ( lead, "   " );
   
   cout << leader << "ENTITY STATE PDU *** from " 
        << sender.host_name << " (" << sender.internet_address << ")" << endl;
   output_PDUHeader ( &(es->entity_state_header), lead );
   output_EntityID ( &(es->entity_id), lead );
   output_ForceID ( &(es->force_id), lead );
   output_EntityType ( &(es->entity_type), lead );
   strcpy ( temp, lead );
   strcat ( temp, "Alternate " );
   output_EntityType ( &(es->alt_entity_type), temp );
   output_VelocityVector ( &(es->entity_velocity), lead );
   output_EntityLocation ( &(es->entity_location), lead );
   output_EntityOrientation ( &(es->entity_orientation), lead );
   output_EntityAppearance ( &(es->entity_appearance), lead );
   output_EntityMarking ( &(es->entity_marking), lead );
   output_Capabilities ( &(es->capabilities), lead );
   output_DeadReckonParams ( &(es->dead_reckon_params), lead );
   output_ArticulatParamsList ( es->articulat_params,
                                es->num_articulat_params,
                                lead );
   cout << endl;

   }



void output_FirePDU ( FirePDU *fire, sender_info sender, char *leader )
   {

   char lead[120];
   char temp[140];

   strcpy ( lead, leader );
   strcat ( lead, "   " );

   cout << leader << "FIRE PDU ### from "
        << sender.host_name << " (" << sender.internet_address << ")" << endl;
   output_PDUHeader ( &(fire->fire_header), lead );
   strcpy ( temp, lead );
   strcat ( temp, "Firing " );
   output_EntityID ( &(fire->firing_entity_id), temp );
   strcpy ( temp, lead );
   strcat ( temp, "Target " );
   output_EntityID ( &(fire->target_entity_id), temp );
   strcpy ( temp, lead );
   strcat ( temp, "Muntion " );
   output_EntityID ( &(fire->munition_id), temp );
   output_EntityType ( &(fire->burst_descriptor.munition), temp );
   output_EventID ( &(fire->event_id), temp);
   cout << endl;

   }


void output_DetonationPDU ( DetonationPDU *det, sender_info sender, 
                            char *leader )
   {

   char lead[120];
   char temp[140];

   strcpy ( lead, leader );
   strcat ( lead, "   " );

   cout << leader << "DETONATION PDU +++ from " 
        << sender.host_name << " (" << sender.internet_address << ")" << endl;
   output_PDUHeader ( &(det->detonation_header), lead );
   strcpy ( temp, lead );
   strcat ( temp, "Firing " );
   output_EntityID ( &(det->firing_entity_id), temp );
   strcpy ( temp, lead );
   strcat ( temp, "Target " );
   output_EntityID ( &(det->target_entity_id), temp );
   strcpy ( temp, lead );
   strcat ( temp, "Muntion " );
   output_EntityID ( &(det->munition_id), temp );
   output_EntityType ( &(det->burst_descriptor.munition), temp );
   output_EventID ( &(det->event_id), temp);
   cout << endl;

   }

void output_EnvironmentalPDU ( EnvironmentalPDU *en, sender_info sender,
                            char *leader )
   {

   char lead[120];
   char temp[140];
   int ix;

   strcpy ( lead, leader );
   strcat ( lead, "   " );

   cout << leader << "Environmental  PDU +++ from "
        << sender.host_name << " (" << sender.internet_address << ")" << endl;
   output_PDUHeader ( &(en->pdu_header), lead );
   strcpy ( temp, lead );
   strcat ( temp, "ID ");
   output_EntityID ( &(en->env_id), temp );
   strcpy ( temp, lead );
   strcat ( temp, "Type ");
   output_EntityType ( &(en->env_type), temp );
   strcpy ( temp, lead );
   strcat ( temp, "Model Type ");
   output_Env_model ( en->env_model, temp );
   strcpy ( temp, lead );
   strcat ( temp, "Status ");
   output_Env_status ( en->env_status, temp );
   strcpy ( temp, lead );
   strcat ( temp, "Number of Records ");
   cout << temp << en->number_of_records << "\n";
   strcpy ( temp, lead );
   for (ix = 0; ix < en->number_of_records;ix++){
     output_Env_record(&(en->env_record[ix]), temp );
   }

   cout << endl;
}

int sameID ( EntityID arg1, EntityID arg2 )
   {
   if ( (arg1.address.site == 0) || 
        (arg1.address.site == arg2.address.site) )
      {
      if ( (arg1.address.host == 0) || 
           (arg1.address.host == arg2.address.host) ) 
         {
         if ( (arg1.entity == 0) || 
              (arg1.entity == arg2.entity) )
            return 1;
         }
      }
   return 0;
   }


void
main ( int argc, char **argv )
   {

   extern char *optarg;
   extern int optind, operr;
   EntityID filterID;
   EntityID targetID;

   PDUType type;
   char *pdu;
   FirePDU *firepdu;
   EntityStatePDU *entitystatepdu;
   DetonationPDU *detonationpdu;
   EnvironmentalPDU *environpdu;
   sender_info pdu_sender_info;

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
   int swap_bufs;
   struct FIL im_filters[MAX_PDU_FILTERS];
   int num_filters;

   int letters = 0;
   long letter_count = 0;

   signal ( SIGTERM, (void (*)(int))handler );
   signal ( SIGHUP, (void (*)(int))handler );
   signal ( SIGINT, (void (*)(int))handler );
   signal ( SIGQUIT, (void (*)(int))handler );

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

   initman->query(TARGET_ENTITY, &local, GET);
   targetID.address.site = (u_short)local.ints[0];
   targetID.address.host = (u_short)local.ints[1];
   targetID.entity = (u_short)local.ints[2];

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
      if ( port == (u_short)0 )
         port = DEFAULT_MC_PORT;
      net = new DIS_net_manager ( group, ttl, port, exercise, round, rw,
                                  DEFAULT_BUF_SIZE, net_interface );
      }
   else
      {
      if ( port == (u_short)0 )
         port = DEFAULT_BC_PORT;
      net = new DIS_net_manager ( exercise, round, rw, DEFAULT_BUF_SIZE,
                                  net_interface, port );
      }

   if ( !net->net_open() )
      {
      cerr << "Could not open network." << endl;
      exit(0);
      }
   else {
      net->add_to_receive_list(EntityStatePDU_Type);
      net->add_to_receive_list(FirePDU_Type);
      net->add_to_receive_list(DetonationPDU_Type);
      net->add_to_receive_list(EnvironmentalPDU_Type);
      initman->get_filters((struct FIL**) im_filters, &num_filters);
      for (int i = 0; i < num_filters; i++) {
         if (im_filters[i].filter_type == TYPE_BLOCK) {
            filterID.address.site = (u_short)im_filters[i].Type.Block.site;
            filterID.address.host = (u_short)im_filters[i].Type.Block.host;
            filterID.entity = (u_short)im_filters[i].Type.Block.entity;
            net->add_to_filter_list(filterID);
         }
      }
   }

   while(!G_exit_flag)
      {

      net->read_pdu(&pdu, &type, pdu_sender_info, swap_bufs);

      if ( pdu != NULL )
         {

         switch(type)
           {
           case (FirePDU_Type):
             firepdu = (FirePDU *)pdu;
             if ( sameID ( targetID, firepdu->firing_entity_id ) )
                {
                if ( letters )
                   {
                   cerr << "F";
                   letter_count++;
                   }
                else
                   output_FirePDU ( firepdu, pdu_sender_info, "" );
                }
             break;
           case (EntityStatePDU_Type):
             entitystatepdu = (EntityStatePDU *)pdu;
             if ( sameID ( targetID, entitystatepdu->entity_id) )
                {
                if ( letters )
                   {
                   cerr << "E";
                   letter_count++;
                   }
                else
                   output_EntityStatePDU ( entitystatepdu, pdu_sender_info, "" );
                }
             break;
           case (DetonationPDU_Type):
             detonationpdu = (DetonationPDU *)pdu;
             if ( sameID ( targetID, detonationpdu->firing_entity_id) )
                {
                if ( letters )
                   {
                   cerr << "D";
                   letter_count++;
                   }
                else
                   output_DetonationPDU ( detonationpdu, pdu_sender_info, "" );
                }
             break;
           case (EnvironmentalPDU_Type):
             environpdu = (EnvironmentalPDU *)pdu;
             if ( sameID ( targetID, environpdu->env_id) )
                {
                if ( letters )
                   {
                   cerr << "En";
                   letter_count++;
                   }
                else
                   output_EnvironmentalPDU ( environpdu, pdu_sender_info, "" );
                }
             break;

           default:
              if ( letters )
                 {
                 cerr << ".";
                 letter_count++;
                 }
             break;
           }

        if ( letters && ((letter_count % MAX_LETTERS) == 0) )
           cerr << "[" << letter_count << "]" << endl;

        }

      }
   
   net->net_close();  
   }
