// File: <pdu_screen.cc>

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

#include <sys/types.h>
#include <time.h>

#include "pdu_screen.h"
#include "appearance.h"

/***** Subroutines *****/

static WINDOW *L_master_window = NULL;
static int L_master_max_line = 0;
static int L_master_max_col = 0;
static WINDOW *L_entity_window = NULL;
static int L_entity_num_line = 0;
static int L_entity_start_line = 0;
static WINDOW *L_detonation_window = NULL;
static int L_detonation_num_line = 0;
static int L_detonation_start_line = 0;
static WINDOW *L_fire_window = NULL;
static int L_fire_num_line = 0;
static int L_fire_start_line = 0;
static WINDOW *L_status_window = NULL;
static int L_status_num_line = 0;
static int L_status_start_line = 0;

static EntityRec *L_entity_list = NULL;
static int L_entity_count = 0;
static FireRec *L_fire_head = NULL;
static FireRec *L_fire_tail = NULL;
static int L_fire_count = 0;
static DetonationRec *L_detonation_head = NULL;
static DetonationRec *L_detonation_tail = NULL;
static int L_detonation_count = 0;

static int L_cache_size = 0;
static int L_cache_misses = 0;

static int L_scroll_lock = FALSE;

int locate_entity ( sender_info &, EntityStatePDU *,
                    int &, EntityRec **, EntityRec ** );


int lookup_name_from_address ( sender_info &sender )
   {
   int success = FALSE;
   struct hostent *entity;
   unsigned long netaddress = inet_addr(sender.internet_address);
   static sender_info local_cache[100];
   int current_count = 0;
   int found = FALSE;

   while ( (current_count < L_cache_size) && !found )
      {
      if ( (strcmp(sender.internet_address,
                   local_cache[current_count].internet_address)) == 0 )
         found = TRUE;
      else
         current_count++;
      }

   if ( found )
      {
      strcpy ( sender.host_name,
               local_cache[current_count].host_name );
      success = TRUE;
      }
   else
      {
      entity = gethostbyaddr ( (void *)&netaddress,
                               sizeof(struct in_addr), AF_INET );
      L_cache_misses++;
      if ( entity != NULL )
         {
         strcpy ( sender.host_name, entity->h_name );
         strtok (sender.host_name, ".");
         if ( L_cache_size < 99 )
            {
            strcpy ( local_cache[L_cache_size].host_name,
                     sender.host_name );
            strcpy ( local_cache[L_cache_size].internet_address,
                     sender.internet_address );
            L_cache_size++;
            }
         success = TRUE;
         }
      else
         strcpy ( sender.host_name, "Unknown" );
      update_status_window(CACHE,NULL);
      }

   return success;
   }


int text_appearance ( unsigned int appearance, char *text )
   {
   int attribute = -1;
   switch ( (appearance & Appearance_Damage_Mask) )
      {
      case Appearance_DamageSlight:
         sprintf(text,"Slight Damage");
         attribute = (int)A_BLINK;
         break;
      case Appearance_DamageModerate:
         sprintf(text,"Moderate Damage");
         attribute = (int)A_BLINK;
         break;
      case Appearance_DamageDestroyed:
         sprintf(text,"Destroyed");
         attribute = (int)A_DIM;
         break;
      default:
         sprintf(text,"");
         if ( appearance & Appearance_FirePowerKill )
            strcat(text,"Fire");
         if ( appearance & Appearance_MobilityKill )
            strcat(text,"Mob");
         if ( (strlen(text)) != 0 )
            {
            strcat(text,"Kill");
            attribute = (int)A_DIM;
            }
         else
            {
            sprintf(text,"Alive & OK");
            }
         break;
      }
   return attribute;
   }

int open_master_window ()
   {
   int success = FALSE;

   if ( L_master_window == NULL )
      {
      L_master_window= initscr(); 
      if ( L_master_window != NULL )
         {
         getmaxyx(L_master_window,L_master_max_line,L_master_max_col);
         cbreak();
         noecho();
         nonl();
         intrflush (stdscr, FALSE);
         keypad(stdscr, TRUE);
         scrollok(L_master_window,TRUE);
         nodelay(L_master_window,TRUE);
         curs_set(0);
         success = TRUE;
         clear();
         attron(A_UNDERLINE);
         mvaddstr(0,0,"PDU Display 1.0 by Paul T. Barham");
         attroff(A_UNDERLINE);
         attron(A_REVERSE);
         mvaddstr(L_status_start_line,40, "R)eset, S)croll, <Ctrl C> quits.");
         attroff(A_REVERSE);
         refresh();
         }
      }

   return success;
   }


int close_master_window ()
   {
   int success = TRUE;
   if (L_entity_window != NULL)
      success = close_entity_window();
   if ( success && (L_detonation_window != NULL) )
      success = close_detonation_window();
   if ( success && (L_fire_window != NULL) )
      success = close_fire_window();
   if ( success && (L_status_window != NULL) )
      success = close_status_window();
   if ( success )
      {
      curs_set(1);
      success = ((endwin()) == OK);
      L_master_max_line = 0;
      L_master_max_col = 0;
      L_master_window = NULL;
      }
   return success;
   }
     

int open_entity_window ()
   {
   int success = FALSE;
   if ( L_master_window != NULL )
      {
      L_entity_num_line = L_master_max_line -
                          L_detonation_num_line -
                          L_fire_num_line -
                          L_status_num_line - 1;
      if ( L_entity_num_line < 0 )
         L_entity_num_line = 0;
      L_entity_start_line = L_detonation_num_line +
                            L_fire_num_line +
                            L_status_num_line + 1;
      L_entity_window = subwin(L_master_window,L_entity_num_line,
                               L_master_max_col, L_entity_start_line, 0);
      if ( L_entity_window != NULL )
         {
         success = TRUE;
         scrollok(L_entity_window,TRUE);
         wclear(L_entity_window);
         attron(A_STANDOUT);
         mvaddstr(L_entity_start_line,0,"Currently Active Entities");
         attroff(A_STANDOUT);
         refresh();
         wrefresh(L_entity_window);
         }
      else
         {
         L_entity_num_line = 0;
         L_entity_start_line = 0;
         }
      }
   return success;
   } 


int close_entity_window ()
   {
   int success = TRUE;
   if ( L_entity_window != NULL )
      {
      refresh();
      wrefresh(L_entity_window);
      success = ((delwin(L_entity_window)) == OK);
      L_entity_window = NULL;
      L_entity_num_line = 0;
      L_entity_start_line = 0;
      }
   return success;
   }


int open_detonation_window ( int &num_lines )
   {
   int success = FALSE;
   int real_lines = num_lines + 1;
   if ( L_master_window != NULL )
      {
      L_detonation_num_line = L_master_max_line -
                              L_fire_num_line -
                              L_status_num_line;
      if ( L_detonation_num_line < 0 )
         L_detonation_num_line = 0;
      else if ( L_detonation_num_line >= real_lines )
         L_detonation_num_line = real_lines;
      L_detonation_start_line = L_fire_num_line +
                                L_status_num_line + 1;
      L_detonation_window = subwin(L_master_window,L_detonation_num_line,
                                   L_master_max_col, L_detonation_start_line,
                                   0);
      if ( L_detonation_window != NULL )
         {
         int reopen_entity = (L_entity_window != NULL);
         success = TRUE;
         if ( reopen_entity )
            close_entity_window();
         scrollok(L_detonation_window,TRUE);
         wclear(L_detonation_window);
         attron(A_STANDOUT);
         mvaddstr(L_detonation_start_line,0,"Recent Detonations");
         attroff(A_STANDOUT);
         refresh();
         wrefresh(L_detonation_window);
         if ( reopen_entity )
            open_entity_window();
         }
      else
         {
         L_detonation_num_line = 0;
         L_detonation_start_line = 0;
         }
      }
   num_lines = (L_detonation_num_line > 0) ? (L_detonation_num_line-1) :
                                             (0);
   return success;
   }


int close_detonation_window ()
   {
   int success = TRUE;
   if ( L_detonation_window != NULL )
      {
      int reopen_entity = (L_entity_window != NULL);
      refresh();
      wrefresh(L_detonation_window);
      success = ((delwin(L_detonation_window)) == OK);
      if ( success && reopen_entity )
         {
         refresh();
         close_entity_window();
         open_entity_window();
         }
      L_detonation_window = NULL;
      L_detonation_num_line = 0;
      L_detonation_start_line = 0;
      }
   return success;
   }

int open_fire_window ( int &num_lines )
   {
   int success = FALSE;
   int real_lines = num_lines + 1;
   if ( L_master_window != NULL )
      {
      L_fire_num_line = L_master_max_line -
                        L_status_num_line;
      if ( L_fire_num_line < 0 )
         L_fire_num_line = 0;
      else if ( L_fire_num_line >= real_lines )
         L_fire_num_line = real_lines;
      L_fire_start_line = L_status_num_line + 1;
      L_fire_window = subwin(L_master_window,L_fire_num_line,
                             L_master_max_col, L_fire_start_line, 0);
      if ( L_fire_window != NULL )
         {
         int reopen_detonation = (L_detonation_window != NULL);
         int old_deton_lines = L_detonation_num_line;
         success = TRUE;
         if ( reopen_detonation )
            close_detonation_window();
         scrollok(L_fire_window,TRUE);
         wclear(L_fire_window);
         attron(A_STANDOUT);
         mvaddstr(L_fire_start_line,0,"Recent Fires");
         attroff(A_STANDOUT);
         refresh();
         wrefresh(L_fire_window);
         if ( reopen_detonation )
            open_detonation_window(old_deton_lines);
         }
      else
         {
         L_fire_num_line = 0;
         L_fire_start_line = 0;
         }
      }
   num_lines = (L_fire_num_line > 0) ? (L_fire_num_line-1) : (0);
   return success;
   }


int close_fire_window ()
   {
   int success = TRUE;
   if ( L_fire_window != NULL )
      {
      int reopen_detonation = (L_detonation_window != NULL);
      refresh();
      wrefresh(L_fire_window);
      success = ((delwin(L_fire_window)) == OK);
      if ( success && reopen_detonation )
         {
         int old_deton_lines = L_detonation_num_line;
         refresh();
         close_detonation_window();
         open_detonation_window(old_deton_lines);
         }
      L_fire_window = NULL;
      L_fire_num_line = 0;
      L_fire_start_line = 0;
      }
   return success;
   }


int open_status_window ()
   {
   int success = FALSE;
   if ( (L_master_window != NULL) && (L_master_max_line >= 7 ) )
      {
      L_status_num_line = 6;
      L_status_start_line = 1;
      L_status_window = subwin(L_master_window,L_status_num_line,
                               L_master_max_col, L_status_start_line, 0);
      if ( L_status_window != NULL )
         {
         int reopen_fire = (L_fire_window != NULL);
         int old_fire_lines = L_fire_num_line;
         success = TRUE;
         if ( reopen_fire )
            close_fire_window();
         scrollok(L_status_window,TRUE);
         wclear(L_status_window);
         attron(A_STANDOUT);
         mvaddstr(L_status_start_line,0,"Status Summary");
         attroff(A_STANDOUT);
         update_status_window(S_REFRESH,NULL);
         refresh();
         wrefresh(L_status_window);
         if ( reopen_fire )
            open_fire_window(old_fire_lines);
         }
      else
         {
         L_status_num_line = 0;
         L_status_start_line = 0;
         }
      }
   return success;
   }


int close_status_window ()
   {
   int success = TRUE;
   if ( L_status_window != NULL )
      {
      int reopen_fire = (L_fire_window != NULL);
      refresh();
      wrefresh(L_status_window);
      success = ((delwin(L_status_window)) == OK);
      if ( success && reopen_fire )
         {
         int old_fire_lines = L_fire_num_line;
         refresh();
         close_fire_window();
         open_fire_window(old_fire_lines);
         }
      L_status_window = NULL;
      L_status_num_line = 0;
      L_status_start_line = 0;
      }
   return success;
   }


int update_status_window ( status_field field, char *val )
   {
   static int total_pdus = 0;
   static int es_pdus = 0;
   static int f_pdus = 0;
   static int d_pdus = 0;
   static int o_pdus = 0;
   static char text[10];

   int valid = TRUE;
   int *force_int = (int *)val;
   int es_dirty = FALSE;
   int f_dirty = FALSE;
   int d_dirty = FALSE;
   int total_dirty = FALSE;
   int frame_dirty = FALSE;
   int first_dirty = 10;
   int last_dirty = 0;
   int entity_dirty = FALSE;
   int cache_dirty = FALSE;
   int scroll_dirty = FALSE;

   switch ( field )
      {
      case ES_COUNT:
         total_pdus++;
         if ( val == NULL )
            es_pdus++;
         else
            es_pdus = *force_int;
         es_dirty = TRUE;
         total_dirty = TRUE;
         break;
      case F_COUNT:
         total_pdus++;
         if ( val == NULL )
            f_pdus++;
         else
            f_pdus = *force_int;
         f_dirty = TRUE;
         total_dirty = TRUE;
         break;
      case D_COUNT:
         total_pdus++;
         if ( val == NULL )
            d_pdus++;
         else
            d_pdus = *force_int;
         d_dirty = TRUE;
         total_dirty = TRUE;
         break;
      case O_COUNT:
         total_pdus++;
         break;
      case ENTITY_COUNT:
         entity_dirty = TRUE;
         break;
      case CACHE:
         cache_dirty = TRUE;
         break;
      case S_SCROLL:
         scroll_dirty = TRUE;
         break;
      case S_RESET:
         total_pdus = 0;
         es_pdus = 0;
         f_pdus = 0;
         d_pdus = 0;
         o_pdus = 0;
         /* no break is intentional */
      case S_REFRESH:
         es_dirty = TRUE;
         f_dirty = TRUE;
         d_dirty = TRUE;
         total_dirty = TRUE;
         entity_dirty = TRUE;
         frame_dirty = TRUE;
         cache_dirty = TRUE;
         scroll_dirty = TRUE;
         break;
      default:
         valid = FALSE;
         break;
      }

   if ( frame_dirty )
      {
      wmove(L_status_window,1,0);
      wclrtobot(L_status_window);
      mvwaddstr(L_status_window,1,2,"Total PDUs ........ ");
      mvwaddstr(L_status_window,1,30,"Total Hosts ....... ");
      mvwaddstr(L_status_window,2,2,"Entity State PDUs . ");
      mvwaddstr(L_status_window,2,30,"Host Lookups ...... ");
      mvwaddstr(L_status_window,3,2,"Fire PDUs ......... ");
      mvwaddstr(L_status_window,4,2,"Detonation PDUs ... ");
      mvwaddstr(L_status_window,5,2,"Entity Count ...... ");
      first_dirty = 1;
      last_dirty = 5;
      }

   if ( es_dirty )
      {
      sprintf(text,"%d", es_pdus);
      mvwaddstr(L_status_window,2,22,text);
      first_dirty = (first_dirty > 2) ? 2 : first_dirty;
      last_dirty = (last_dirty < 2) ? 2 : last_dirty;
      }
   if ( f_dirty )
      {
      sprintf(text,"%d", f_pdus);
      mvwaddstr(L_status_window,3,22,text);
      first_dirty = (first_dirty > 3) ? 3 : first_dirty;
      last_dirty = (last_dirty < 3) ? 3 : last_dirty;
      }
   if ( d_dirty )
      {
      sprintf(text,"%d", d_pdus);
      mvwaddstr(L_status_window,4,22,text);
      first_dirty = (first_dirty > 4) ? 4 : first_dirty;
      last_dirty = (last_dirty < 4) ? 4 : last_dirty;
      }
   if ( entity_dirty )
      {
      wmove(L_status_window,5,22);
      wclrtoeol(L_status_window);
      sprintf(text,"%d", L_entity_count);
      mvwaddstr(L_status_window,5,22,text);
      first_dirty = (first_dirty > 5) ? 5 : first_dirty;
      last_dirty = (last_dirty < 5) ? 5 : last_dirty;
      }
   if ( scroll_dirty )
      {
      wmove(L_status_window,0,50);
      wclrtoeol(L_status_window);
      if ( L_scroll_lock )
         {
         wattron(L_status_window,A_STANDOUT);
         mvwaddstr(L_status_window,0,65,"SCROLL LOCK");
         wattroff(L_status_window,A_STANDOUT);
         }
      first_dirty = 0;
      last_dirty = (last_dirty < 1) ? 1 : last_dirty;
      }
   if ( total_dirty )
      {
      sprintf(text,"%d",total_pdus);
      mvwaddstr(L_status_window,1,22,text);
      first_dirty = (first_dirty > 1) ? 1 : first_dirty;
      last_dirty = (last_dirty < 1) ? 1 : last_dirty;
      }
   if ( cache_dirty )
      {
      sprintf(text,"%d",L_cache_size);
      mvwaddstr(L_status_window,1,51,text);
      sprintf(text,"%d",L_cache_misses);
      mvwaddstr(L_status_window,2,51,text);
      first_dirty = (first_dirty > 1) ? 1 : first_dirty;
      last_dirty = (last_dirty < 2) ? 2 : last_dirty;
      }

   if ( valid )
      {
      touchline(L_master_window,first_dirty,(last_dirty-first_dirty)+1);
      refresh();
      wrefresh(L_status_window);
      }

   return valid;
   }


void redraw_entity_list ( int starting_position )
   {
   EntityRec *curr = L_entity_list;
   int local_count = 0;
   int attribute = -1;

   if ( (!L_scroll_lock) &&
        (L_entity_window != NULL) &&
        (starting_position < L_entity_num_line) )
      {
      while ( ( curr != NULL) && (local_count < starting_position) )
         {
         curr = (EntityRec *)curr->next_entity;
         local_count++;
         }
      if ( curr != NULL )
         {
         static char temp_markings[MARKINGS_LEN+1];
         wmove(L_entity_window,local_count+1,0);
         wclrtobot(L_entity_window);
         while ( (curr != NULL) && (local_count < (L_entity_num_line-1)) )
            {
            char info[255];
            sprintf(info,"%12s", curr->sender.host_name);
            mvwaddstr(L_entity_window,local_count+1,0,info);
            sprintf(info,"%d/%d/%d",
                    curr->epdu.entity_id.address.site,
                    curr->epdu.entity_id.address.host,
                    curr->epdu.entity_id.entity);
            mvwaddstr(L_entity_window,local_count+1,14,info);
            strncpy ( temp_markings,
                      (char *)curr->epdu.entity_marking.markings,
                      MARKINGS_LEN);
            temp_markings[MARKINGS_LEN] = '\0';
            if ( curr->epdu.entity_marking.character_set == CharSet_ASCII )
               sprintf(info, "'%s'", temp_markings);
            else
               sprintf(info, "No Marking");
            mvwaddstr(L_entity_window,local_count+1,28,info);
            sprintf(info,"%d.%d.%d.%d.%d.%d",
                    curr->epdu.entity_type.entity_kind,
                    curr->epdu.entity_type.domain,
                    curr->epdu.entity_type.country,
                    curr->epdu.entity_type.category,
                    curr->epdu.entity_type.subcategory,
                    curr->epdu.entity_type.specific);
            if ( curr->epdu.entity_type.entity_kind == EntityKind_Munition )
               wattron(L_entity_window,A_REVERSE);
            mvwaddstr(L_entity_window,local_count+1,45,info);
            if ( curr->epdu.entity_type.entity_kind == EntityKind_Munition )
               wattroff(L_entity_window,A_REVERSE);
            attribute = text_appearance ( curr->epdu.entity_appearance, info);
            if ( attribute != -1 )
               wattron(L_entity_window,attribute);
            mvwaddstr(L_entity_window,local_count+1,62,info);
            if ( attribute != -1 )
               wattroff(L_entity_window,attribute);
            curr = (EntityRec *)curr->next_entity;
            local_count++;
            }
         }
      else
         {
         wmove(L_entity_window,local_count+1,0);
         wclrtobot(L_entity_window);
         }
      refresh();
      wrefresh(L_entity_window);
      }
   }


void redraw_fires()
   {
   FireRec *curr = L_fire_tail;
   int temp_count = L_fire_count;
   char info[255];

   if ( (!L_scroll_lock) &&
        (L_fire_window != NULL) )
      {
      wmove(L_fire_window,1,0);
      wclrtobot(L_fire_window);

      while ( (temp_count > 0) && (curr != NULL) )
         {
         if ( temp_count < L_fire_num_line )
            {
            sprintf(info,"%12s", curr->sender.host_name);
            mvwaddstr(L_fire_window,temp_count,0,info);
            sprintf(info,"%d/%d/%d",
                    curr->fpdu.firing_entity_id.address.site,
                    curr->fpdu.firing_entity_id.address.host,
                    curr->fpdu.firing_entity_id.entity);
            mvwaddstr(L_fire_window,temp_count,14,info);
            mvwaddstr(L_fire_window,temp_count,28,"->");
            sprintf(info,"%d/%d/%d",
                    curr->fpdu.target_entity_id.address.site,
                    curr->fpdu.target_entity_id.address.host,
                    curr->fpdu.target_entity_id.entity);
            mvwaddstr(L_fire_window,temp_count,31,info);
            sprintf(info,"%d.%d.%d.%d.%d.%d",
                    curr->fpdu.burst_descriptor.munition.entity_kind,
                    curr->fpdu.burst_descriptor.munition.domain,
                    curr->fpdu.burst_descriptor.munition.country,
                    curr->fpdu.burst_descriptor.munition.category,
                    curr->fpdu.burst_descriptor.munition.subcategory,
                    curr->fpdu.burst_descriptor.munition.specific);
            mvwaddstr(L_fire_window,temp_count,45,info);
            switch ( curr->fpdu.burst_descriptor.warhead )
               {
               case WarheadMunition_HighExp:
                  sprintf(info,"HighExp");
                  break;
               case WarheadMunition_HighExpPlastic:
                  sprintf(info,"HighExpPlas");
                  break;
               case WarheadMunition_HighExpIncendiary:
                  sprintf(info,"HighExpIncen");
                  break;
               case WarheadMunition_HighExpFragment:
                  sprintf(info,"HighExpFrag");
                  break;
               case WarheadMunition_HighExpAntiTank:
                  sprintf(info,"HighExpTank");
                  break;
               case WarheadMunition_HighExpBomblets:
                  sprintf(info,"HighExpBets");
                  break;
               case WarheadMunition_HighExpShapedCharg:
                  sprintf(info,"HighExpShape");
                  break;
               case WarheadMunition_Smoke:
                  sprintf(info,"Smoke");
                  break;
               case WarheadMunition_Illumination:
                  sprintf(info,"Illumination");
                  break;
               case WarheadMunition_Practice:
                  sprintf(info,"Practice");
                  break;
               case WarheadMunition_Kinetic:
                  sprintf(info,"Kinetic");
                  break;
               case WarheadMunition_Unused:
                  sprintf(info,"Unused");
                  break;
               case WarheadMunition_Nuclear:
                  sprintf(info,"Nuclear");
                  break;
               case WarheadMunition_ChemGeneral:
                  sprintf(info,"ChemGeneral");
                  break;
               case WarheadMunition_ChemBlister:
                  sprintf(info,"ChemBlister");
                  break;
               case WarheadMunition_ChemBlood:
                  sprintf(info,"ChemBlood");
                  break;
               case WarheadMunition_ChemNerve:
                  sprintf(info,"ChemNerve");
                  break;
               case WarheadMunition_BiologicalGeneral:
                  sprintf(info,"BioGeneral");
                  break;
               default:
                  sprintf(info,"Other");
                  break;
               }
            mvwaddstr(L_fire_window,temp_count,62,info);
            }
         temp_count--;
         curr = (FireRec *)curr->next_fire;
         }

      refresh();
      wrefresh(L_fire_window);
      }

   }


void redraw_detonations()
   {
   DetonationRec *curr = L_detonation_tail;
   int temp_count = L_detonation_count;
   char info[255];

   if ( (!L_scroll_lock) &&
        (L_detonation_window != NULL) )
      {
      wmove(L_detonation_window,1,0);
      wclrtobot(L_detonation_window);

      while ( (temp_count > 0) && (curr != NULL) )
         {
         if ( temp_count < L_detonation_num_line )
            {
            sprintf(info,"%12s", curr->sender.host_name);
            mvwaddstr(L_detonation_window,temp_count,0,info);
            sprintf(info,"%d/%d/%d",
                    curr->dpdu.firing_entity_id.address.site,
                    curr->dpdu.firing_entity_id.address.host,
                    curr->dpdu.firing_entity_id.entity);
            mvwaddstr(L_detonation_window,temp_count,14,info);
            mvwaddstr(L_detonation_window,temp_count,28,"->");
            sprintf(info,"%d/%d/%d",
                    curr->dpdu.target_entity_id.address.site,
                    curr->dpdu.target_entity_id.address.host,
                    curr->dpdu.target_entity_id.entity);
            mvwaddstr(L_detonation_window,temp_count,31,info);
            sprintf(info,"%d.%d.%d.%d.%d.%d",
                    curr->dpdu.burst_descriptor.munition.entity_kind,
                    curr->dpdu.burst_descriptor.munition.domain,
                    curr->dpdu.burst_descriptor.munition.country,
                    curr->dpdu.burst_descriptor.munition.category,
                    curr->dpdu.burst_descriptor.munition.subcategory,
                    curr->dpdu.burst_descriptor.munition.specific);
            mvwaddstr(L_detonation_window,temp_count,45,info);
            switch ( curr->dpdu.detonation_result )
               {
               case DetResult_EntityImpact:
                  sprintf(info,"EntityImpact");
                  break;
               case DetResult_EntityProxDet:
                  sprintf(info,"EntityProxDet");
                  break;
               case DetResult_GroundImpact:
                  sprintf(info,"GroundImpact");
                  break;
               case DetResult_GroundProxDet:
                  sprintf(info,"GroundProxDet");
                  break;
               case DetResult_Detonation:
                  sprintf(info,"Detonation");
                  break;
               case DetResult_None:
                  sprintf(info,"None");
                  break;
               default:
                  sprintf(info,"Other");
                  break;
               } 
            mvwaddstr(L_detonation_window,temp_count,62,info);
            }
         temp_count--;
         curr = (DetonationRec *)curr->next_detonation;
         }

      refresh();
      wrefresh(L_detonation_window);
      }

   }


void update_entity_list ( int position, EntityRec *curr, 
                          EntityStatePDU *epdu )
   {
   /* Move to position and update entity_info stuff */
   int attribute = -1;

   if ( (!L_scroll_lock) &&
        (L_entity_window != NULL) &&
        (position < L_entity_num_line) &&
        (curr != NULL) )
      {
      if ( epdu->entity_appearance !=
           curr->epdu.entity_appearance )
         {
         char info[355];
         attribute = text_appearance ( epdu->entity_appearance, info );
         wmove(L_entity_window,position+1,62);
         wclrtoeol(L_entity_window);
         if ( attribute != -1 )
            wattron(L_entity_window,attribute);
         mvwaddstr(L_entity_window,position+1,62,info);
         if ( attribute != -1 )
            wattroff(L_entity_window,attribute);
         touchline(L_master_window,L_entity_start_line+position+1,1);
         refresh();
         wrefresh(L_entity_window);
         }
      }

   }


inline int
entity_id_greater ( EntityID &a, EntityID &b )
   {
   if ( a.address.site > b.address.site )
      return TRUE;
   else if ( a.address.site == b.address.site )
      {
      if ( a.address.host > b.address.host )
         return TRUE;
      else if ( a.address.host == b.address.host )
         return ( a.entity > b.entity );
      else
         return FALSE;
      }
   else
      return FALSE;
   }
      
int locate_entity ( sender_info &sender, EntityStatePDU *epdu, 
                    int &pos, EntityRec **prev, EntityRec **curr )
   {
   EntityRec *lcurr;
   EntityRec *lprev;
   pos = 0; 
   lcurr = L_entity_list;
   lprev = NULL;
   while ( (lcurr != NULL) && 
           entity_id_greater(epdu->entity_id,lcurr->epdu.entity_id) )
      {
      lprev = lcurr;
      lcurr = (EntityRec *)lcurr->next_entity;
      pos++;
      }
  
   *curr = lcurr;
   *prev = lprev;
 
   if ( (lcurr != NULL) && 
        ((bcmp((char *)&epdu->entity_id,
               (char *)&lcurr->epdu.entity_id,
               sizeof(EntityID))) == 0) &&
        ((strcmp(sender.internet_address,lcurr->sender.internet_address)) == 0))
      return TRUE;
   else
      return FALSE;
   
   }

int update_list ( sender_info &sender, EntityStatePDU *epdu )
   {
   EntityRec *prev;
   EntityRec *curr; 
   EntityRec *temp;
   int success = FALSE;
   int count = 0;
   int new_entity = FALSE;
 
   if ( locate_entity(sender,epdu,count,&prev,&curr) )
      { 
      /* Already in list, update info */
      success = TRUE;
      temp = curr;
      update_entity_list ( count, curr, epdu );

/*
      wprintw(L_entity_window, "Updating existing %d/%d/%d from %s\n",
                curr->epdu.entity_id.address.site,
                curr->epdu.entity_id.address.host,
                curr->epdu.entity_id.entity,
                curr->sender.host_name );
*/
      }
   else
      {
      /* New to list, create node and link it in */
      temp = new EntityRec;
      if ( temp != NULL )
         {
         temp->next_entity = curr;
         if ( L_entity_list == curr )
            L_entity_list = temp;
         else
            prev->next_entity = (void *)temp; 
         success = TRUE;
         L_entity_count++;
         lookup_name_from_address(sender);
         bcopy ( (char *)sender.host_name, 
                 (char *)temp->sender.host_name,
                 sizeof(sender.host_name) );
         new_entity = TRUE;
/*
         wprintw(L_entity_window, "New entity %d/%d/%d from %s\n",
                epdu->entity_id.address.site,
                epdu->entity_id.address.host,
                epdu->entity_id.entity,
                sender.host_name );
*/
         update_status_window(ENTITY_COUNT,NULL);
         }
      }

   if ( success )
      {
      bcopy ( (char *)sender.internet_address, 
              (char *)temp->sender.internet_address, 
              sizeof(sender.internet_address) );
      bcopy ( (char *)epdu, (char *)&temp->epdu, sizeof(EntityStatePDU) );
      temp->last_time = time(0);      
      if ( new_entity )
         redraw_entity_list(count);
      }

   return success;

   }


int remove_fire ()
   {
   FireRec *temp;
   if ( L_fire_tail != NULL )
      {
      temp = L_fire_tail;
      L_fire_tail = (FireRec *)L_fire_tail->next_fire;
      if ( temp == L_fire_head )
         L_fire_head = NULL;
      temp->next_fire = NULL;
      delete temp;
      L_fire_count--;
      return TRUE;
      }
   else
      return FALSE;
   }


void free_fire_list ()
   {
   while ( remove_fire() );
   L_fire_count = 0;
   }


int update_list ( sender_info &sender, FirePDU *fpdu )
   {
   FireRec *temp;
   while ( (L_fire_count > L_fire_num_line) &&
           (L_fire_tail != NULL) )
      remove_fire();
   temp = new FireRec;
   if ( temp != NULL )
      {
      lookup_name_from_address(sender);
      bcopy ( (char *)&sender, (char *)&temp->sender, sizeof(sender_info) );
      bcopy ( (char *)fpdu, (char *)&temp->fpdu, sizeof(FirePDU) );
      temp->last_time = time(0);
      temp->next_fire = NULL;
      if ( L_fire_head == NULL )
         L_fire_tail = temp;
      else
         L_fire_head->next_fire = (void *)temp;
      L_fire_head = temp;
      L_fire_count++;
      redraw_fires();
      return TRUE;
      }
   else   
      {
      redraw_fires();
      return FALSE;
      }
   }


int remove_detonation ()
   {
   DetonationRec *temp;
   if ( L_detonation_tail != NULL )
      {
      temp = L_detonation_tail;
      L_detonation_tail = (DetonationRec *)L_detonation_tail->next_detonation;
      if ( temp == L_detonation_head )
         L_detonation_head = NULL;
      temp->next_detonation = NULL;
      delete temp;
      L_detonation_count--;
      return TRUE;
      }
   else
      return FALSE;
   }


void free_detonation_list ()
   {
   while ( remove_detonation() );
   L_detonation_count = 0;
   }


int update_list ( sender_info &sender, DetonationPDU *dpdu )
   {
   DetonationRec *temp;
   while ( (L_detonation_count > L_detonation_num_line) &&
           (L_detonation_tail != NULL) )
      remove_detonation();
   temp = new DetonationRec;
   if ( temp != NULL )
      {
      lookup_name_from_address(sender);
      bcopy ( (char *)&sender, (char *)&temp->sender, sizeof(sender_info) );
      bcopy ( (char *)dpdu, (char *)&temp->dpdu, sizeof(DetonationPDU) );
      temp->last_time = time(0);
      temp->next_detonation = NULL;
      if ( L_detonation_head == NULL )
         L_detonation_tail = temp;
      else
         L_detonation_head->next_detonation = (void *)temp;
      L_detonation_head = temp;
      L_detonation_count++;
      redraw_detonations();
      return TRUE;
      }
   else
      {
      redraw_detonations();
      return FALSE;
      } 
   }


void free_entity_list ()
   {
   EntityRec *curr = L_entity_list;
   EntityRec *temp;
   while ( curr != NULL )
      {
      temp = curr;
      curr = (EntityRec *)curr->next_entity;
      temp->next_entity = NULL;
      delete temp;
      }
   L_entity_list = NULL;
   L_entity_count = 0;
   }


void review_lists ()
   {
   time_t current_time = time(0);
   int in_char;
   EntityRec *prev;
   EntityRec *curr;
   EntityRec *temp;
   int count = 0;
   int fire_changed = FALSE;
   int detonation_changed = FALSE;


   if ( (in_char = getch()) != ERR )
      {
      /* Do input processing */
      switch ( char(in_char) )
         {
         case 'r':
         case 'R':
            /* Reset */
            free_fire_list();
            free_detonation_list();
            free_entity_list();
            redraw_entity_list(0);
            fire_changed = TRUE;
            detonation_changed = TRUE;
            L_cache_size = 0;
            L_cache_misses = 0;
            update_status_window(S_RESET,NULL);
            break;
         case 's':
         case 'S':
            L_scroll_lock = !L_scroll_lock;
            update_status_window(S_SCROLL,NULL);
            if ( !L_scroll_lock )
               {
               fire_changed = TRUE;
               detonation_changed = TRUE;
               redraw_entity_list(0);
               }
            break;
         default:
            break;
         }
      }

   curr = L_entity_list;
   prev = NULL;
   while ( curr != NULL )
      {
      if ( ((curr->epdu.entity_appearance & Appearance_Platform_Status_Mask)==
            Appearance_Platform_Deactivated) ||
           ((curr->last_time+12) < current_time) )
         {
         temp = curr;
         if ( prev == NULL )
            L_entity_list = (EntityRec *)curr->next_entity;
         else
            prev->next_entity = curr->next_entity;
         curr = (EntityRec *)curr->next_entity;
         temp->next_entity = NULL;
/*
         wprintw(L_text_window, "Bye to entity %d/%d/%d from %s\n",
                temp->epdu.entity_id.address.site,
                temp->epdu.entity_id.address.host,
                temp->epdu.entity_id.entity,
                temp->sender.host_name );
*/
         delete temp;
         redraw_entity_list(count);
         L_entity_count--;
         update_status_window(ENTITY_COUNT,NULL);
         }
      else
         {
         prev = curr;
         curr = (EntityRec *)curr->next_entity;
         count++;
         }
      }

   while ( (L_fire_tail != NULL) &&
           ( (L_fire_count > L_fire_num_line) ||
             ((L_fire_tail->last_time+60) < current_time) ) )
      {
      remove_fire();
      fire_changed = TRUE;
      }
   if ( fire_changed )
      redraw_fires();

   while ( (L_detonation_tail != NULL) &&
           ( (L_detonation_count > L_detonation_num_line) ||
             ((L_detonation_tail->last_time+60) < current_time) ) )
      {
      remove_detonation();
      detonation_changed = TRUE;
      }
   if ( detonation_changed )
      redraw_detonations();


   }


/***** End of pdu_screen.cc *****/

