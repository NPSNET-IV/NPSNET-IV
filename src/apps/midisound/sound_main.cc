// File: <sound_main.cc>

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

/****************************#includes, #defines*******************/

#include "globals.h"
#include "appearance.h"

// Initialization Manager
#include "im.tab.h"
#include "imstructs.h"
#include "imclass.h"
#include "imdefaults.h"

InitManager *initman;

/************************typedefs and structures*****************/

#define SQR(a) ((a)*(a))

IDENTIFY *identify;

void initialize ( int argc, char **argv,
                  ENTITY_REC &er, STATE_REC &sr, 
                  CONFIG_REC &config, SOUND_EVENT_REC *elist )
   {
   int i;

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

   initman->query(MASTER, &local, GET);
   strcpy ( config.master_name, local.string );

   hostent *host;
   host = gethostbyname(config.master_name);
   if ( host == NULL )
      {
      cerr << "Master host error:" << endl;
      cerr << "\t" << config.master_name << endl;
      cerr << "\t" << hstrerror(h_errno) << endl;
      exit(0);
      }
   else
      {
      struct in_addr *host_ip = (struct in_addr *)host->h_addr;
      strncpy ( config.master_net_addr, inet_ntoa ( *host_ip ), 255 );
      strncpy ( config.master_name, host->h_name, 255 );
      strtok ( config.master_name, "." );
      }

   initman->query(ENVIRONMENTAL, &local, GET);
   strcpy ( config.environmental_file, local.string );

   initman->query(ROUND_WORLD_FILE, &local, GET);
   strcpy ( config.round_world_file, local.string );

   config.round = !strcmp( config.round_world_file,"NONE");

   config.rw = NULL;

   if (config.round) {
      config.rw = new roundWorldStruct;
      struct RW initManRW;
      initman->get_round_world_data(&initManRW);
      config.rw->northing = initManRW.northing;
      config.rw->easting = initManRW.easting;
      config.rw->o_zone_num = initManRW.zone_num;
      config.rw->o_zone_letter = initManRW.zone_letter;
      config.rw->map_datum = initManRW.map_datum;
      config.rw->width = initManRW.width;
      config.rw->height = initManRW.height;
   }

   initman->query(NETWORK_FILE, &local, GET);
   strcpy ( config.host_file, local.string );

   initman->query(SEARCH_PATH, &local, GET);
   strcpy ( config.search_path, local.string );

   initman->query(IM_PORT, &local, GET);
   config.port = (u_short) local.ints[0];

   initman->query(MULTICAST, &local, GET);
   config.multicast = local.ints[0];
   initman->query(MULTICAST_TTL, &local, GET);
   config.ttl = (u_char) local.ints[0];
   initman->query(MULTICAST_GROUP, &local, GET);
   strncpy ( config.group, local.string, STRING32 );

   initman->query(DIS_EXERCISE, &local, GET);
   config.exercise = (u_char) local.ints[0];

   //initman->query(SOUND_FILE, &local, GET);
   //strncpy ( config.sound_file, local.string, FILENAME_SIZE );

   initman->query(IDENTIFY_FILE, &local, GET);
   strncpy ( config.identify_file, local.string, FILENAME_SIZE );

   initman->query(ETHER_INTERF, &local, GET);
   strncpy ( config.ether_interf, local.string, 10 );

   initman->query(TARGET_ENTITY, &local, GET);
   config.entity_id.address.site = (u_short)local.ints[0];
   config.entity_id.address.host = (u_short)local.ints[1];
   config.entity_id.entity = (u_short)local.ints[2];

   er.alive = TRUE;
   er.last_es_time = -1;
   er.location.x = 0;
   er.location.y = 0;
   er.location.z = 0;
   er.velocity.x = 0;
   er.velocity.y = 0;
   er.velocity.z = 0;
   er.orientation.theta = 0;
   er.orientation.psi = 0;
   er.orientation.phi = 0;
   er.entity_id.address.site = 0;
   er.entity_id.address.host = 0;
   er.entity_id.entity = 0;
   er.entity_sound = -1;
   er.my_sound_on = FALSE;
   er.speed = 0;
   er.maxspeed = 100;  // was 64
   er.deadreckon.algorithm = DRAlgo_DRM_FVW;
   er.deadreckon.linear_accel[0] = 0.0f;
   er.deadreckon.linear_accel[1] = 0.0f;
   er.deadreckon.linear_accel[2] = 0.0f;
   er.deadreckon.angular_velocity[0] = 0;
   er.deadreckon.angular_velocity[1] = 0;
   er.deadreckon.angular_velocity[2] = 0;
   er.last_DR_time = 0.0;
   strcpy ( er.hostname, "NONE" );
   strcpy ( er.host_net_addr, "" );

   sr.last_es_time = -1;
   sr.current_time = 0;
   sr.mode_change_time = -1;
   sr.mode_change_active = FALSE;
   sr.mode = loading;
   sr.banknum = DEFAULT_PRESET;
   sr.override_req = FALSE;
   sr.done = FALSE;
   sr.debug = FALSE;
   sr.total_volume = MAX_VOLUME;
   sr.graphics = TRUE;

   for ( i = 0; i < MAX_SOUND_EVENTS; i++ )
      {
      elist[i].event_type = (PDUType)0;
      elist[i].event_location.x = 0.0;
      elist[i].event_location.y = 0.0;
      elist[i].event_radius = 1.0;
      elist[i].next_event = i+1;
      }
   elist[MAX_SOUND_EVENTS-1].next_event = NIL;
   sr.freelist = 0;
   sr.headlist = NIL;

   }


void add_to_event_list ( STATE_REC &sr, SOUND_EVENT_REC *elist, PDUType type, 
                       EntityLocation location, double radius, int the_sound )
   {

   int i = 0;

   if ( sr.freelist != NIL )
      {
      i = sr.freelist;
      sr.freelist = elist[i].next_event;
      elist[i].next_event = sr.headlist;
      sr.headlist = i;
      elist[i].event_type = type;
      elist[i].event_location = location;
      elist[i].event_radius = radius;
      elist[i].event_time = get_time();
      elist[i].event_snd = the_sound;
      }
   else
      cerr << "Unable to find an empty slot in the event list!!!\n";

   }

void update_event_list ( ENTITY_REC &my_info, STATE_REC &state, 
                         SOUND_EVENT_REC *elist )
   {
   int i = NIL;
   int last = NIL;
   int temp = NIL;
   double current_time = get_time();
   double distance;

   i = state.headlist;
   last = NIL;
   while ( i != NIL )
      {
      elist[i].event_radius = 
         ( (current_time - elist[i].event_time) * SPEED_OF_SOUND);

      distance = sqrt( SQR(my_info.location.x - elist[i].event_location.x) +
                       SQR(my_info.location.y - elist[i].event_location.y) +
                       SQR(my_info.location.z - elist[i].event_location.z) );

      if ( distance > double (RANGE) )
         {
         temp = i;
         i = elist[i].next_event;
         elist[temp].event_type = (PDUType)0;
         if ( temp == state.headlist )
            state.headlist = i;
         else
            elist[last].next_event = i;
         elist[temp].next_event = state.freelist;
         state.freelist = temp;
         }

      else if (elist[i].event_radius > distance )
         {
         if ( !state.debug )
            trigger_3d_sound ( state.midifd, my_info.location,
                               elist[i].event_location, my_info.orientation,
                               elist[i].event_snd, state.total_volume);
         temp = i;
         i = elist[i].next_event;
         elist[temp].event_type = (PDUType)0;
         if ( temp == state.headlist )
            state.headlist = i;
         else
            elist[last].next_event = i;
         elist[temp].next_event = state.freelist;
         state.freelist = temp;
         }
      else
         {
         last = i;
         i = elist[i].next_event;
         }
      }

   }

void load_bank ( STATE_REC &sr )
   {
   cerr << "Loading EMAX bank (hex)" << hex << sr.banknum << dec << endl;
   if ( !sr.debug )
      {
      send_midi_command ( sr.midifd, CHANGE_PRESET );
      send_midi_command ( sr.midifd, DEFAULT_LOAD_PRESET );
      send_midi_command ( sr.midifd, CHANGE_PRESET );
      send_midi_command ( sr.midifd, (unsigned char)sr.banknum );
      }
   sleep(15);
   cerr << "   Bank loaded.\n";
   }


int is_me ( EntityID arg1, EntityID arg2 )
   {
   if ( (arg1.entity  == arg2.entity) &&
        (arg1.address.host == arg2.address.host) &&
        (arg1.address.site == arg2.address.site) )
      return TRUE;
   else
      return FALSE;
   }

int is_master ( char *master_net_addr, EntityID &master_entity_id,
                char *entity_net_addr, EntityID &entity_entity_id )
   {
   int match = FALSE;
   if ( ((strcmp(master_net_addr, "")) == 0) ||
        ((strcmp(master_net_addr,entity_net_addr)) == 0) )
      {
      if ((master_entity_id.address.site == (unsigned short)0) ||
          (master_entity_id.address.site == entity_entity_id.address.site))
         {
         if ((master_entity_id.address.host == (unsigned short)0) ||
             (master_entity_id.address.host == entity_entity_id.address.host))
            {
            match = ((master_entity_id.entity == (unsigned short)0) ||
                     (master_entity_id.entity == entity_entity_id.entity));
            }
         }
      }
   return match;
   }


void turn_off_all_sounds( ENTITY_REC &er, STATE_REC &sr )
   {
   /* This function should send midi commands to stop all notes and */
   /*    sequences.                                                 */
   if ( !sr.debug )
      {
      //*****send_midi_command ( sr.midifd, (unsigned char)STOP_SEQUENCE );
      send_midi_command ( sr.midifd, (unsigned char)ALL_NOTES_OFF );
      }
   if ( (er.entity_sound != -1) && (er.my_sound_on) )
      {
      if ( !sr.debug )
         my_sound_off ( sr.midifd, (unsigned char)er.entity_sound );
      er.my_sound_on = FALSE;
      }
   //*****my_attribute_off ( sr.midifd, (unsigned char) GUN_MOVING_SOUND );
   //*****my_attribute_off ( sr.midifd, (unsigned char) TURRET_MOVING_SOUND );

   }


void play_on_message( STATE_REC &sr )
   {
   /* Trigger the sequence to announce the system is on. */
   if ( !sr.debug )
      {
      send_midi_command ( sr.midifd, (unsigned char)SONG_SELECT );
      send_midi_command ( sr.midifd, (unsigned char)SONG_ON_MESSAGE);
      }
   sleep(1);
   sr.current_time = get_time();
   if ( !sr.debug )
      send_midi_command ( sr.midifd, (unsigned char)START_SEQUENCE );
   }


void play_off_message( STATE_REC &sr )
   {
   /* Trigger the sequence to announce the system is shutting down. */
   if ( !sr.debug )
      {
      send_midi_command ( sr.midifd, (unsigned char)SONG_SELECT );
      send_midi_command ( sr.midifd, (unsigned char)SONG_OFF_MESSAGE);
      }
   sleep(1);
   sr.current_time = get_time();
   if ( !sr.debug )
      send_midi_command ( sr.midifd, (unsigned char)START_SEQUENCE );
   }


void play_theme ( STATE_REC &sr )
   {
   /* Trigger the sequence to play the theme song. */
   if ( !sr.debug )
      {
      send_midi_command ( sr.midifd, (unsigned char)SONG_SELECT );
      send_midi_command ( sr.midifd, (unsigned char)SONG_THEME);
      }
   sleep(1);
   sr.current_time = get_time();
   if ( !sr.debug )
      send_midi_command ( sr.midifd, (unsigned char)START_SEQUENCE );
   }


void play_dead ( ENTITY_REC &er, STATE_REC &sr )
   {
   er.maxspeed = 64;  //This is another warning clean-up
   
   /* Trigger the sequence to play the "dead" message. */
   if ( !sr.debug )
      {
   trigger_sound(127,DEAD_SOUND,sr.midifd, 1);
   trigger_sound(127,DEAD_SOUND,sr.midifd, 0);
   trigger_sound(127,DEAD_SOUND,sr.midifd, 3);
   trigger_sound(127,DEAD_SOUND,sr.midifd, 2);
   trigger_sound(127,DEAD_SOUND,sr.midifd, 5);
   trigger_sound(127,DEAD_SOUND,sr.midifd, 4);
   trigger_sound(127,DEAD_SOUND,sr.midifd, 7);
   trigger_sound(127,DEAD_SOUND,sr.midifd, 6);

    //  send_midi_command ( sr.midifd, (unsigned char)SONG_SELECT );
    //  send_midi_command ( sr.midifd, (unsigned char)SONG_DEAD_MESSAGE);
      }
   sleep(2);
   sr.current_time = get_time();
   if ( !sr.debug )
      send_midi_command ( sr.midifd, (unsigned char)START_SEQUENCE );
   }


void play_my_vehicle ( ENTITY_REC &er, STATE_REC &sr )
   {
   /* Trigger the sequence to play the vehicle sound for me. */
   if ( !sr.debug )
      {
      send_midi_command ( sr.midifd, (unsigned char)SONG_SELECT);
      send_midi_command ( sr.midifd, (unsigned char)SONG_DIS);
      }
   if ( er.entity_sound != -1 )
      {
      sleep(1);
      sr.current_time = get_time();
      if ( !sr.debug )
         my_sound_on( sr.midifd, (unsigned char)er.entity_sound,
                      (sr.total_volume - 10) );
      er.my_sound_on = TRUE;
      }
   }

void process_firePDU ( FirePDU *pdu, ENTITY_REC &my_info, 
                       STATE_REC &state, SOUND_EVENT_REC *elist )
   {
   static EFFECT_REC effect;
   static int Fsound;

   Fsound = identify->effect(&pdu->burst_descriptor.munition, &effect);

   if ( effect.fire_midi_snd >= 0 )
      { /* Recognized the fire */

      if ( !state.debug )
         {
         if ( is_me (pdu->firing_entity_id, my_info.entity_id) )
            trigger_3d_sound ( state.midifd, my_info.location,
                               my_info.location, my_info.orientation,
                               effect.fire_midi_snd , state.total_volume); 
         else
            {

            add_to_event_list ( state, elist, FirePDU_Type, 
                                pdu->location_in_world,
                                1.0f, effect.fire_midi_snd );
            }
         }
      

      } /* Recognized the fire */
   } /* process_firePDU */


void process_detonationPDU ( DetonationPDU *pdu, 
                             STATE_REC &state, SOUND_EVENT_REC *elist )
   {
   static EFFECT_REC effect;
   static int Dsound;

  
   Dsound = identify->effect(&pdu->burst_descriptor.munition, &effect);
   if ( effect.det_midi_snd >= 0 )
      { /* Recognized the fire */

      add_to_event_list ( state, elist, DetonationPDU_Type, 
                          pdu->location_in_world, 1.0f, effect.det_midi_snd );

      } /* Recognized the fire */

   } /* process_detonationPDU */


void process_vehicle_sound ( ENTITY_REC &er, STATE_REC &sr )
   {
   /* Bend pitch, etc. */
   int pitch;
   unsigned char first_byte;
   unsigned char second_byte;

   pitch = (int)((er.speed/(1.0*er.maxspeed)) * (double)0x3FFF);
   if ( pitch < 0 )
      pitch = 0;
   else if ( pitch > 0x3FFF )
      pitch = 0x3FFF;
// used to be 0x007f in the below 2 lines
//********************************************************************************* 

first_byte = (unsigned char) (pitch & 0x007f);
second_byte = (unsigned char) ((pitch >> 7) & 0x007f);

   if ( !sr.debug )
      {
      send_midi_command(sr.midifd, (unsigned char) PITCH_WHEEL_RANGE );
      send_midi_command(sr.midifd, first_byte);

//cerr << "First Byte = " << (int) first_byte << endl;

      send_midi_command(sr.midifd, second_byte);

//cerr << "Second Byte = " << (int) second_byte << endl << endl;
      }
   }


void process_articulations ( STATE_REC &sr, int numartics, ArticulatParams *APNptr )
   {
   static float old_turret = 0.0f;
   static float old_gun = 0.0f;
   static float turret = 0.0f;
   static float gun = 0.0f;;
   static int gun_moving = 0;
   static int turret_moving = 0;
   float temp_float;
   int count = 0;

   while ( count < numartics )
      {
      if ( (APNptr->ID == (unsigned short)0x0) &&
           (APNptr->parameter_type == (unsigned int)4107) )
         {
         bcopy ( &(APNptr->parameter_value),
                 &temp_float, 4);
         turret = (360.0f - (temp_float*RAD_TO_DEG));
         while ( turret < 0.0f )
            turret += 360.0f;
         turret = fmod ( turret, 360.0f );
         }
      else if ( (APNptr->ID == (unsigned short)0x1) &&
           (APNptr->parameter_type == (unsigned int)4429) )
         {
         bcopy ( &(APNptr->parameter_value),
                 &temp_float, 4);
         gun = float(-temp_float * RAD_TO_DEG);
         }
      APNptr++;
      count++;
      }

   if ( old_gun != gun )
      {
      old_gun = gun;
      if ( !gun_moving )
         {
         if ( !sr.debug )
            my_attribute_on( sr.midifd, (unsigned char) GUN_MOVING_SOUND,
                             sr.total_volume);
#ifdef MASTER_DEBUG
         cerr << "Gun moving ON.\n";
#endif
         }
      gun_moving = 1;
      }
   else
      {
      if ( gun_moving )
         {
         if ( !sr.debug )
            my_attribute_off ( sr.midifd, (unsigned char) GUN_MOVING_SOUND );
#ifdef MASTER_DEBUG
         cerr << "Gun moving OFF.\n";
#endif
         }
      gun_moving = 0;
      }

   if ( old_turret != turret )
      {
      old_turret = turret;
      if ( !turret_moving )
         {
         if ( !sr.debug )
            my_attribute_on( sr.midifd, (unsigned char) TURRET_MOVING_SOUND,
                             sr.total_volume); 
#ifdef MASTER_DEBUG
         cerr << "Turret moving ON.\n";
#endif
         }
      turret_moving = 1;
      }
   else
      {
      if ( turret_moving )
         {
         if ( !sr.debug )
            my_attribute_off ( sr.midifd, (unsigned char) TURRET_MOVING_SOUND );
#ifdef MASTER_DEBUG
         cerr << "Turret moving OFF.\n";
#endif
         }
      turret_moving = 0;
      }

   }/*process articulations*/


void process_entitystatePDU ( EntityStatePDU *pdu, ENTITY_REC &my_info, 
                              STATE_REC &state,
                              CONFIG_REC &config, sender_info &sender )
   {
   static ID_ENTITY_REC entity_info;
   
   state.last_es_time = state.current_time;

   if (!((state.mode == my_vehicle_alive) || (state.mode == my_vehicle_dead)))
      {
      /* Look for the first platform (vehicle) or lifeform entity state */
      /* from the master machine to determine which entity is master.   */
      if ( is_master ( config.master_net_addr, config.entity_id,
                       sender.internet_address, pdu->entity_id ) && 
           ( (pdu->entity_type.entity_kind == EntityKind_Platform) || 
             (pdu->entity_type.entity_kind == EntityKind_LifeForm) ) )
         {
#ifdef MASTER_DEBUG
         cerr << "The master, " << config.master_net_addr
              << ", has been found." << endl;
#endif
         my_info.entity_id.address.site = pdu->entity_id.address.site;
         my_info.entity_id.address.host = pdu->entity_id.address.host;
         my_info.entity_id.entity = pdu->entity_id.entity;
         if ( identify->entity(&pdu->entity_type,&entity_info) )
            my_info.entity_sound = entity_info.midi_veh_snd;
         else
            my_info.entity_sound = -1;

         struct hostent *entity;
         unsigned long netaddress = inet_addr(sender.internet_address);
         entity = gethostbyaddr ( (void *)&netaddress,
                                  sizeof(struct in_addr), AF_INET );
         if ( entity != NULL )
            strcpy ( my_info.hostname, entity->h_name );
         else
            strcpy ( my_info.hostname, "Unknown" );

         strtok(my_info.hostname,".");

         }
      }

   if ( is_me (pdu->entity_id,my_info.entity_id) )
      { /* info from master */
      
      my_info.last_es_time = state.current_time;
      my_info.last_DR_time = state.current_time;

      my_info.location.x = pdu->entity_location.x;
      my_info.location.y = pdu->entity_location.y;
      my_info.location.z = pdu->entity_location.z;
 
      my_info.velocity.x = pdu->entity_velocity.x;
      my_info.velocity.y = pdu->entity_velocity.y;
      my_info.velocity.z = pdu->entity_velocity.z;
      
      my_info.deadreckon = pdu->dead_reckon_params;

      my_info.speed = (int)sqrt ( (1.0 * SQR(my_info.velocity.x)) +
                                  (1.0 * SQR(my_info.velocity.y)) +
                                  (1.0 * SQR(my_info.velocity.z)) );

      my_info.orientation.psi = -1 * pdu->entity_orientation.psi;
      my_info.orientation.theta = pdu->entity_orientation.theta;
      my_info.orientation.phi = pdu->entity_orientation.phi;

      process_articulations ( state, pdu->num_articulat_params, &(pdu->articulat_params[0]) );

      if ( (pdu->entity_appearance & Appearance_Damage_Mask) == 
           Appearance_DamageDestroyed )
         my_info.alive = FALSE;
      else
         my_info.alive = TRUE;

      if ( (pdu->entity_appearance & Appearance_Platform_Status_Mask) ==
           Appearance_Platform_Deactivated )
         my_info.last_es_time = 0.0f;

      } /* info from master */


   } /* process_entitystatePDU */


void process_state ( ENTITY_REC &er, STATE_REC &sr )
   { /* process state */

   MODE current_mode;
   static int override_mode = FALSE;
   static MODE old_mode;
   static int old_volume = -1;

   if ( (old_volume != sr.total_volume) && er.my_sound_on )
      {
      turn_off_all_sounds(er,sr);
      play_my_vehicle(er,sr);
      old_volume = sr.total_volume;
      }


   if ( sr.override_req )
      {
      sr.override_req = FALSE;
      override_mode = !override_mode;
      if ( override_mode )
         {
         old_mode = sr.mode;
         sr.mode = override;
         turn_off_all_sounds(er,sr);
         play_theme(sr);         
         cerr << "OVERRIDE MODE NOW ON...\n";
         }
      else
         {
         turn_off_all_sounds(er,sr);
         cerr << "OVERRIDE MODE NOW OFF...\n";
         }
      }

   if ( !override_mode )
      {


      /* ************* Determine current mode ************** */

      current_mode = sr.mode;
   
      if ( sr.mode == loading )
         sr.mode = no_vehicle;
   
      if ( (sr.current_time - er.last_es_time) > TIMEOUT )
         {
         if ( (sr.current_time - sr.last_es_time) > TIMEOUT )
            sr.mode = no_vehicle;
         else
            sr.mode = a_vehicle;
         }
      else
         {
         if ( er.alive )
            sr.mode = my_vehicle_alive;
         else
            sr.mode = my_vehicle_dead;
         }
  
      /* ************ Process mode changes if any ************* */
 
      if ( current_mode != sr.mode )
         { /* State change */
  
         if ( current_mode == loading )
            {
            turn_off_all_sounds(er,sr);
#ifdef MASTER_DEBUG
            cerr << "Playing activated message...\n";
#endif
            play_on_message(sr);
            sleep(5);
            sr.current_time = get_time();
            turn_off_all_sounds(er,sr);
            }
   
         sr.mode_change_time = sr.current_time;
         sr.mode_change_active = FALSE;

         switch ( sr.mode )
            { /* Switch mode */

            case no_vehicle:
#ifdef MASTER_DEBUG
               cerr << "Switched delayed state to NO VEHICLES "
                    << DELAY_STATE_TIME << " seconds until action.\n";
#endif
               strcpy ( er.hostname, "NONE" );
               break;

            case a_vehicle:
#ifdef MASTER_DEBUG
               cerr << "Switched delayed state to A VEHICLE "
                    << DELAY_STATE_TIME << " seconds until action.\n";
#endif
               strcpy ( er.hostname, "NONE" );
               break;

            case my_vehicle_dead:
#ifdef MASTER_DEBUG
               cerr << "Switched state to MY VEHICLE DEAD.\n";
#endif
               turn_off_all_sounds(er,sr);
               play_dead(er,sr);
               sr.mode_change_active = TRUE;
               break;

            case my_vehicle_alive:
#ifdef MASTER_DEBUG
               cerr << "Switched state to MY VEHICLE ALIVE.\n";
#endif
               turn_off_all_sounds(er,sr);
               play_my_vehicle(er,sr);
               sr.mode_change_active = TRUE;
               break;

            default:
               cerr << "ERROR:  I am in an unknown mode ->" << sr.mode << endl;
               break;

            } /* Switch mode */
   
         } /* State change */

      if ( ( sr.mode == a_vehicle) || ( sr.mode == no_vehicle ) )
         {
         if ( ((sr.current_time - sr.mode_change_time) > DELAY_STATE_TIME) &&
              (!sr.mode_change_active) )
            {
            sr.mode_change_active = TRUE;
            turn_off_all_sounds(er,sr);
            play_theme(sr);
            er.alive = TRUE;
            er.last_es_time = -1;
            er.location.x = 0;
            er.location.y = 0;
            er.location.z = 0;
            er.velocity.x = 0;
            er.velocity.y = 0;
            er.velocity.z = 0;
            er.orientation.theta = 0;
            er.orientation.psi = 0;
            er.orientation.phi = 0;
            er.entity_id.address.site = 0;
            er.entity_id.address.host = 0;
            er.entity_id.entity = 0;
            er.entity_sound = -1;
            er.my_sound_on = FALSE;
            er.speed = 0;
            er.maxspeed = 64;
            er.deadreckon.algorithm = DRAlgo_DRM_FVW;
            er.deadreckon.linear_accel[0] = 0.0f;
            er.deadreckon.linear_accel[1] = 0.0f;
            er.deadreckon.linear_accel[2] = 0.0f;
            er.deadreckon.angular_velocity[0] = 0;
            er.deadreckon.angular_velocity[1] = 0;
            er.deadreckon.angular_velocity[2] = 0;
            er.last_DR_time = 0.0;
            strcpy ( er.hostname, "NONE" );
            strcpy ( er.host_net_addr, "" );

#ifdef MASTER_DEBUG
            cerr << "Acting on delayed mode now.\n";
#endif
            }
         }

      } /* not in override mode */
   
   } /* process state */


void process_environmentals( ENTITY_REC &my_info, STATE_REC &state,
                             ENVIRON_REC environ_list[MAX_ENVIRONMENTALS] )
   {
   ENVIRON_REC *environ = environ_list;
   int count = 0;
   float distance;
   
  
   while ( count < state.num_environ )
      {
     
      distance = ( SQR(my_info.location.x - environ->location.x) +
                   SQR(my_info.location.y - environ->location.y) +
                   SQR(my_info.location.z - environ->location.z) );
      if ( (distance < environ->range_sqrd) &&
           ((state.current_time - environ->last_play_time) > 
            environ->repeat_time) )
         {
         environ->last_play_time = state.current_time;
#ifdef MASTER_DEBUG
         cerr << "Triggered environmental sound # " << count+1 << endl;
#endif
         if ( !state.debug )
            trigger_3d_sound ( state.midifd, my_info.location,
                               environ->location, my_info.orientation,
                               environ->midi_note_num, state.total_volume );
         }
	 //possibly alter signal processors	 

      count++;
      environ++;

      }
 
   }




void process_keyboard ( STATE_REC &state, double &event_radius )
   {
   short                val;
   long                 device;

   /* this loops checks to see if the user has presssed the ESC key to */
   /* get out of the program */
   while (qtest())
      {
      device = qread ( &val );
      if ( ((device == ESCKEY) && (!val)) || 
            (device == WINSHUT) || (device == WINQUIT) )
         {
         state.done = TRUE;
         state.mode = exiting;
         }
      else if ((device == OKEY) && (!val) )
         state.override_req = TRUE;
      else if ((device == PADMINUS) && (!val) )
         {
         event_radius -= 5.0;
         if ( event_radius < MIN_RADIUS )
            event_radius = MIN_RADIUS;
         }
      else if ((device == PADPLUSKEY) && (!val) )
         {
         event_radius += 5.0;
         if ( event_radius > MAX_RADIUS )
            event_radius = MAX_RADIUS;
         }
      else if ((device == DOWNARROWKEY) && (!val) )
         {
	 state.total_volume -= 1;
         if ( state.total_volume < MIN_VOLUME )
		state.total_volume = MIN_VOLUME;
         }
      else if ((device == UPARROWKEY) && (!val) )
         {
         state.total_volume += 1;
         if ( state.total_volume > MAX_VOLUME )
            state.total_volume = MAX_VOLUME;
         }
	 

      }

   }

void
main(int argc, char **argv)
{
   /* variable declarations*/
   ENTITY_REC my_info;
   STATE_REC  state;
   CONFIG_REC config;
   ENVIRON_REC environ_list[MAX_ENVIRONMENTALS];
   int pdus_processed;
   PDUType type;
   char *pdu;
   int switched_buffers = 0;
   sender_info sender;
   SOUND_EVENT_REC event_list[MAX_SOUND_EVENTS];
   double event_radius = DEFAULT_RADIUS;
   DIS_net_manager *net;
   double vehiclePlayTime; 



#ifdef MASTER_DEBUG
   cerr << "Starting program...initializing.\n";
#endif

   initialize ( argc, argv, my_info, state, config, event_list );


#ifdef MASTER_DEBUG
   if ( state.graphics )
      cerr << "Opening graphics window.\n";
#endif


   /* open the window*/
   if ( state.graphics ) {
      setupwin(&state.gwindow);
      update_window ( state );
   }

#ifdef MASTER_DEBUG
   cerr << "Loading environmentals sounds from "
        << config.environmental_file << endl;
#endif

   state.num_environ = read_environmentals ( config.environmental_file,
                                             environ_list );
#ifdef MASTER_DEBUG
   cerr << "  Number of environmental sounds loaded:  "
        << state.num_environ << endl; 
#endif

 
#ifdef MASTER_DEBUG
   cerr << "***  Attempting to open network with Exercise " 
        << int(config.exercise) 
        << ", Round world = " << config.round_world
        << "\n***     Round file " << config.round_world_file << endl;
#endif

   if ( config.multicast )
      {
      if ( config.port == 0 )
         config.port = DEFAULT_MC_PORT;
      net = new DIS_net_manager ( config.group, config.ttl, config.port,
                                  config.exercise, 
                                  config.round,
                                  config.rw, 500,
                                  config.ether_interf );
      if (!net->net_open())
         {
         cerr << "Main(): Net_Open failed\n";
         if ( state.graphics )
            winclose(state.gwindow);
         exit(1);
         }
      }
   else
      {
      if ( config.port == 0 )
         config.port = DEFAULT_BC_PORT;
      net = new DIS_net_manager ( config.exercise, 
                                 config.round,
                                 config.rw, 500,
                                 config.ether_interf, config.port );
      if (!net->net_open())
         {
         cerr << "main(): net_open(\"" << config.ether_interf << "\") failed\n";
         if ( state.graphics )
            winclose(state.gwindow);
         exit(1);
         }
      }

#ifdef MASTER_DEBUG
   cerr << "Opening serial port for MIDI communications.\n";
#endif

   /* set up midi port */
   state.midifd = initialize_port(MIDI_PORT_NAME);
   if (state.midifd < 0)
      {
      cerr << "Cannot open the port for the Midi inteface!" << endl;
      net->net_close();
      if ( state.graphics )
         winclose(state.gwindow);
      exit(1);
      }


#ifdef MASTER_DEBUG
   cerr << "Loading EMAX bank.\n";
#endif

   if ( state.graphics )
      update_window ( state );

   load_bank(state);   // ************************* put back

   if ( state.graphics )
      update_window ( state );

/*
   if (init.test_flag)
      {
      cerr << "Testing directions!" << endl;
      if ( !state.debug )
         test(state.midifd,TEST_SOUND);
      cerr << "   Done testing " << endl;
      }
*/

   identify = new IDENTIFY(config.identify_file);

   vehiclePlayTime = get_time();


#ifdef MASTER_DEBUG
   cerr << "Starting main loop.\n";
#endif
   /* start  main loop */
   while (!state.done)
      {

      state.current_time = get_time();

      pdus_processed = 0;

      while ( (pdus_processed < MAX_PDUS_PER_LOOP) &&
              (net->read_pdu(&pdu, &type, sender, switched_buffers)) )
         {

         pdus_processed++;

         switch ( type )
            {
            case (FirePDU_Type):
               if ( state.mode == my_vehicle_alive )
                  process_firePDU ( (FirePDU *)pdu, my_info, 
                                    state, event_list );
               break;
            case (DetonationPDU_Type):
               if ( state.mode == my_vehicle_alive )
                  process_detonationPDU((DetonationPDU *)pdu,
                                        state, event_list);
               break;
            case (EntityStatePDU_Type):
               process_entitystatePDU ( (EntityStatePDU *)pdu, my_info, 
                                        state, config, sender );
               if ( (state.mode == my_vehicle_alive) &&
                    (my_info.last_es_time == state.current_time) )
                  process_vehicle_sound(my_info,state);
               break;
            default:
               break;
            }

         process_state ( my_info, state );

         }
     
      if ( (state.mode == my_vehicle_alive) ||
           (state.mode == my_vehicle_dead) )
         move_dead_reckon(my_info);
 
      if ( pdus_processed == 0 )
         {
         process_state ( my_info, state );
         }
 
      update_event_list ( my_info, state, event_list );
      
      if ( state.graphics )
         draw_event_map ( my_info, state, config, event_radius, event_list );

      if ( state.mode == my_vehicle_alive )
         {
         process_environmentals( my_info, state, environ_list );



if (fabs(vehiclePlayTime - get_time()) > 1.0)
{
   send_midi_command ( state.midifd, (unsigned char)STOP_SEQUENCE );
   my_sound_off ( state.midifd, (unsigned char)my_info.entity_sound );
   send_midi_command ( state.midifd, (unsigned char)SONG_SELECT);
   send_midi_command ( state.midifd, (unsigned char)SONG_DIS);
   my_sound_on( state.midifd, (unsigned char)my_info.entity_sound,
                      (state.total_volume - 10) ); 
   vehiclePlayTime = get_time();
} 


         }

      process_keyboard ( state, event_radius );
      

      }// end of main loop

   if ( state.graphics )
      update_window ( state );

#ifdef MASTER_DEBUG
   cerr << "Closing network.\n";
#endif
   /* close the net */
   net->net_close();

#ifdef MASTER_DEBUG
   cerr << "Playing deactivated message...\n";
#endif

   turn_off_all_sounds(my_info,state);
   play_off_message (state);
   sleep(8);
   turn_off_all_sounds(my_info,state);

#ifdef MASTER_DEBUG
   cerr << "Closing midi port.\n";
#endif

   /* close midi port */
   close_port(state.midifd);

#ifdef MASTER_DEBUG
   if ( state.graphics )
      cerr << "Closing graphics window.\n";
#endif
   /* close window */
   if ( state.graphics )
      winclose(state.gwindow);

#ifdef MASTER_DEBUG
   cerr << "The program is now done - so long.\n";
#endif

   exit(0);
   } /* main */

