// File: <identify.cc>

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "identify.h"
#include "display_const.h"
#include "local_snd_code.h"
#include "midi_snd_code.h"
#include "environment_const.h"

#define NONE -1

#ifdef DEBUG_EFFECTS
#define LOCALERROR -21
#endif

#define CATEGORY_OTHER 0
#define CATEGORY_GUIDED 1
#define CATEGORY_BALLISTIC 2
#define CATEGORY_FIXED 3

#define TYPE_PLATFORM 0
#define TYPE_LIFEFORM 1

#define TARGET_OTHER 0
#define TARGET_AIR 1
#define TARGET_ARMOR 2
#define TARGET_GUIDED 3
#define TARGET_RADAR 4
#define TARGET_SATELLITE 5
#define TARGET_SHIP 6
#define TARGET_SUBMARINE 7
#define TARGET_PERSONNEL 8
#define TARGET_BATTLEFIELD 9
#define TARGET_STRATEGIC 10
#define TARGET_TACTICAL 11

#define D_OTHER 0
#define D_LAND 1
#define D_AIR 2
#define D_SURFACE 3
#define D_SUBSURFACE 4
#define D_SPACE 5
#define D_DEFAULT 0

IDENTIFY::IDENTIFY(char *id_filename) {
  FILE *id_file;
  int country=-1, category=0, target=0, subcategory=-1, temp, cat_index,
      function=-1, entity_type=0, domain=0;
  char buffer[256], buffer2[256], *buffer_index;
	
  memset(&id_table,0x11,sizeof(id_table));
  memset(&id_entity,0x11,sizeof(id_entity));
  
  if((id_file=fopen(id_filename,"rb"))==NULL) {
    cerr << "Error loading identify.dat!\n"; return;
  }
  while(!feof(id_file)) {
    fgets(buffer,255,id_file);
    if(buffer[0]!='#' && buffer[0]!='\n') {
      strcpy(buffer2,buffer);
      buffer_index=strtok(buffer2," \t\n");
      if(buffer_index!=NULL && buffer2[0]!='#') {
        if(!strcmp(buffer_index,"FOR_IDENTIFY_EFFECT:")) function=0;
        else if(!strcmp(buffer_index,"FOR_IDENTIFY_ENTITY:")) function=1;
	else if(!strcmp(buffer_index,"GUIDED")) category=CATEGORY_GUIDED;
	else if(!strcmp(buffer_index,"BALLISTIC")) category=CATEGORY_BALLISTIC;
	else if(!strcmp(buffer_index,"FIXED")) category=CATEGORY_FIXED;
	else if(!strcmp(buffer_index,"T_AIR")) target=TARGET_AIR;
	else if(!strcmp(buffer_index,"T_ARMOR")) target=TARGET_ARMOR;
	else if(!strcmp(buffer_index,"T_GUIDED")) target=TARGET_GUIDED;
	else if(!strcmp(buffer_index,"T_RADAR")) target=TARGET_RADAR;
	else if(!strcmp(buffer_index,"T_SATELLITE")) target=TARGET_SATELLITE;
	else if(!strcmp(buffer_index,"T_SHIP")) target=TARGET_SHIP;
	else if(!strcmp(buffer_index,"T_SUBMARINE")) target=TARGET_SUBMARINE;
	else if(!strcmp(buffer_index,"T_PERSONNEL")) target=TARGET_PERSONNEL;
	else if(!strcmp(buffer_index,"T_BATTLEFIELD")) target=TARGET_BATTLEFIELD;
	else if(!strcmp(buffer_index,"T_STRATEGIC")) target=TARGET_STRATEGIC;
        else if(!strcmp(buffer_index,"T_TACTICAL")) target=TARGET_TACTICAL;
	else if(!strcmp(buffer_index,"COUNTRY")) {
	  buffer_index=strtok(NULL," \t\n");
	  if(!strcmp(buffer_index,"DEF")) country=-1;
	  else if(atoi(buffer_index)>0) country=atoi(buffer_index);
	}
	else if(!strcmp(buffer_index,"PLATFORM")) entity_type=TYPE_PLATFORM;
	else if(!strcmp(buffer_index,"LIFEFORM")) entity_type=TYPE_LIFEFORM;
	else if(!strcmp(buffer_index,"LAND")) domain=D_LAND;
	else if(!strcmp(buffer_index,"AIR")) domain=D_AIR;
	else if(!strcmp(buffer_index,"SURFACE")) domain=D_SURFACE;
	else if(!strcmp(buffer_index,"SUBSURFACE")) domain=D_SUBSURFACE;
	else if(!strcmp(buffer_index,"SPACE")) domain=D_SPACE;
	else if(!strcmp(buffer_index,"DEFAULT")) domain=D_DEFAULT; 
	else if(function==0) {
	  subcategory=999999;
	  if(!strcmp(buffer_index,"DEF")) subcategory=-1;
	  else if(atoi(buffer_index)>0) subcategory=atoi(buffer_index);
	  if(subcategory!=999999) {
	    for(temp=0;temp<MAX_SUBCATS;temp++) {
	      if(id_table[category][target][temp].subcategory==0x11111111)
	        { cat_index=temp; temp=999999; }
	    }
	    if(temp>=999999) {
	      id_table[category][target][cat_index].country=country;
	      id_table[category][target][cat_index].subcategory=subcategory;
	      sscanf(buffer," %s %d %d %d %d %d %d",buffer2,
	      &id_table[category][target][cat_index].effect_data.fire_effect,
	      &id_table[category][target][cat_index].effect_data.fire_local_snd,
	      &id_table[category][target][cat_index].effect_data.fire_midi_snd,
	      &id_table[category][target][cat_index].effect_data.det_effect,
	      &id_table[category][target][cat_index].effect_data.det_local_snd,
	      &id_table[category][target][cat_index].effect_data.det_midi_snd);
	      if(strchr(buffer,'S')!=NULL)
	      id_table[category][target][cat_index].effect_data.smoke_effect=23;
	    }
	  } 
	}
	else if(function==1) {
	  subcategory=999999;
	  if(!strcmp(buffer_index,"DEF")) subcategory=-1;
	  else if(atoi(buffer_index)>0) subcategory=atoi(buffer_index);
          if(subcategory!=999999) {
	    for(temp=0;temp<MAX_ENTCATS;temp++) {
	      if(id_entity[entity_type][domain][temp].category==0x11111111)
	         { cat_index=temp; temp=999999; }
	    }
	    if(temp>=999999) {
	      id_entity[entity_type][domain][cat_index].category=subcategory;
	      sscanf(buffer," %s %d",buffer2,
	      &id_entity[entity_type][domain][cat_index].midi_veh_snd);
	    }
	  }
	}
      }
    }
  }
}

int IDENTIFY::entity (EntityType *et, ID_ENTITY_REC *entity)
  {
  int success = TRUE, entity_type=0, domain=0, cat_index=0, temp, def_index=-1;
  
  switch(et->entity_kind) {
    case (unsigned char)EntityKind_Platform: entity_type=TYPE_PLATFORM; break;
    case (unsigned char)EntityKind_LifeForm: entity_type=TYPE_LIFEFORM; break;
    default: entity->midi_veh_snd=-1; return FALSE;
  }
  switch(et->domain) {
    case (unsigned char)Domain_Land: domain=D_LAND; break;
    case (unsigned char)Domain_Air: domain=D_AIR; break;
    case (unsigned char)Domain_Surface: domain=D_SURFACE; break;
    case (unsigned char)Domain_Subsurface: domain=D_SUBSURFACE; break;
    case (unsigned char)Domain_Space: domain=D_SPACE; break;
    default: domain=D_DEFAULT;
  }
  
  for(temp=0;temp<MAX_ENTCATS;temp++) {
    if(id_entity[entity_type][domain][temp].category==et->category) {
      cat_index=temp; temp=999999;
    }
    else if(id_entity[entity_type][domain][temp].category==-1) def_index=temp;
  }
  if(temp<999999) { success=FALSE; cat_index=def_index; }
  entity->midi_veh_snd=id_entity[entity_type][domain][cat_index].midi_veh_snd;
  
#ifdef DEBUG_EFFECTS
  cerr << "Kind: " << (int)et->entity_kind << ", Domain: " << (int)et->domain
       << ", Category: " << (int)et->category << ", MIDI: " << (int)entity->midi_veh_snd
       << endl;
#endif  
  
  return success;
  }


int IDENTIFY::effect(EntityType *et, EFFECT_REC *effect) {

  int success = TRUE, category=0, target=0, cat_index=-1, def_index=-1, temp,
      def_country_index=-1, def_country_def_index=-1;

#ifdef DEBUG_EFFECTS
  effect->fire_effect = LOCALERROR;
  effect->fire_local_snd = LOCALERROR;
  effect->fire_midi_snd = LOCALERROR;
  effect->det_effect = LOCALERROR;
  effect->det_local_snd = LOCALERROR;
  effect->det_midi_snd = LOCALERROR;
#endif

  effect->smoke_effect = NO_SMOKE;

  if ( et->entity_kind == (unsigned char)EntityKind_Munition )
    { /* Munition */
   
  switch(et->category) {
    case GUIDED: category=CATEGORY_GUIDED; break;
    case BALLISTIC: category=CATEGORY_BALLISTIC; break;
    case FIXED: category=CATEGORY_FIXED; break;
    default: category=CATEGORY_BALLISTIC; success=FALSE; break; 
  }
  switch(et->domain) {
    case (unsigned char)TargetDomain_AntiAir: target=TARGET_AIR; break;
    case (unsigned char)TargetDomain_AntiArmor: target=TARGET_ARMOR; break;
    case (unsigned char)TargetDomain_AntiGuided: target=TARGET_GUIDED; break;
    case (unsigned char)TargetDomain_AntiRadar: target=TARGET_RADAR; break;
    case (unsigned char)TargetDomain_AntiSatellite: target=TARGET_SATELLITE; break;
    case (unsigned char)TargetDomain_AntiShip: target=TARGET_SHIP; break;
    case (unsigned char)TargetDomain_AntiSubmarine: target=TARGET_SUBMARINE; break;
    case (unsigned char)TargetDomain_AntiPersonnel: target=TARGET_PERSONNEL; break;
    case (unsigned char)TargetDomain_BattlefieldSupp: target=TARGET_BATTLEFIELD; break;
    case (unsigned char)TargetDomain_Strategic: target=TARGET_STRATEGIC; break;
    default: target=TARGET_BATTLEFIELD; success=FALSE; break;
  }
  for(temp=0;temp<MAX_SUBCATS;temp++) {
    if(id_table[category][target][temp].country==et->country) {
      if(id_table[category][target][temp].subcategory==et->subcategory) {
        cat_index=temp; temp=999999;
      }
      else if(id_table[category][target][temp].subcategory==-1) {
        def_index=temp;
      }
    }
    else if(id_table[category][target][temp].country==-1) {
      if(id_table[category][target][temp].subcategory==et->subcategory) {
        def_country_index=temp;
      }
      else if(id_table[category][target][temp].subcategory==-1) {
        def_country_def_index=temp;
      }
    } 
  }   
  if(cat_index==-1) {
    if(def_index>-1) cat_index=def_index;
    else if(def_country_index>-1) cat_index=def_country_index;
    else if(def_country_def_index>-1) cat_index=def_country_def_index;
  }

  memcpy(effect,&id_table[category][target][cat_index].effect_data,
         sizeof(EFFECT_REC));
	
  if(id_table[category][target][cat_index].effect_data.smoke_effect==23) {
    switch(et->specific) {
      case BLACK_SMOKE: case RED_SMOKE: case GREEN_SMOKE:
      case YELLOW_SMOKE: case WHITE_SMOKE:
	effect->smoke_effect=int(et->specific); break;
      default: 
	effect->smoke_effect=BLACK_SMOKE; break;
    }
  }
  else effect->smoke_effect=NO_SMOKE;
 
#ifdef DEBUG_EFFECTS
  if((effect->fire_effect == LOCALERROR)   || 
     (effect->fire_local_snd == LOCALERROR)||
     (effect->fire_midi_snd == LOCALERROR) ||
     (effect->det_effect == LOCALERROR)    ||
     (effect->det_local_snd == LOCALERROR) ||
     (effect->det_midi_snd == LOCALERROR)) {
     cerr << "*** Cannot identify munition for effect:\n";
  }
  cerr << "    Kind = " << (int)et->entity_kind
       << ", Dom = " << (int)et->domain
       << ", Count = " << (int)et->country
       << ", Cat = " << (int)et->category
       << ", Scat = " << (int)et->subcategory
       << ", Spec = " << (int)et->specific << endl;
  cerr << "    Fire effect = " << effect->fire_effect 
       << ", Local Fire = " <<  effect->fire_local_snd
       << ", Midi Fire = " << effect->fire_midi_snd << "," << endl
       << "    Det effect = " << effect->det_effect
       << ", Local Det = " <<  effect->det_local_snd
       << ", Midi Det = " << effect->det_midi_snd << "," << endl
       << "    Smoke Color = " << effect->smoke_effect
       << endl;
#endif

  } /* if EntityKind_Munition */
  else if(et->entity_kind==(unsigned char)99) 
    success=process_special(et,effect); 
  else if(et->entity_kind==EntityKind_Platform) {
    effect->fire_effect = NONE;
    effect->fire_local_snd = NONE;
    effect->fire_midi_snd = NONE;
    effect->det_effect = FIREBALL;
    effect->det_local_snd = LRG_GUN_2_DINDEX;
    effect->det_midi_snd = D_LRG_GUN_5;
    effect->smoke_effect = BLACK_SMOKE;
    success = TRUE;
  }
  else {
    effect->fire_effect = NONE;
    effect->fire_local_snd = NONE;
    effect->fire_midi_snd = NONE;
    effect->det_effect = NONE;
    effect->det_local_snd = NONE;
    effect->det_midi_snd = NONE;
    effect->smoke_effect = NONE;
  }
   
  return success;

}



int IDENTIFY::process_special (EntityType *et, EFFECT_REC *effect) {
  int success = FALSE;

  switch (et->subcategory) {
    default:
       effect->fire_effect = UNKNOWN;
       effect->fire_local_snd = UNKNOWN;
       effect->fire_midi_snd = UNKNOWN;
       effect->det_effect = UNKNOWN;
       effect->det_local_snd = 21;
       effect->det_midi_snd = UNKNOWN;
       success = TRUE;
       break;
  }

  return success;
}


 
