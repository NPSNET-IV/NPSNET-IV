// File: <parse_disconst.h>

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

/* File:	hashtable.h
 * Description:	hash table classes
 * Revision:	1.0 - 05May95
 *
 * Refs:    a)  Military Standard--Protocol Data Units for Entity
 *		Information and Entity Interaction in a Distributed
 *		Interactive Simulation (DIS) (1.0) (30Oct91)
 *
 * Author:	Jiang Zhu
 *		CS Department, Naval Postgraduate School
 * Internet:	zhuj@taurus.cs.nps.navy.mil
 */

#ifndef __NPS_PARSE_DISCONST__
#define __NPS_PARSE_DISCONST__

#include <fstream.h>

#ifdef Short
#undef Short
#endif

typedef short Short;

#ifdef TRUE
#undef TRUE
#endif

#ifdef FALSE
#undef FALSE
#endif

#ifndef Boolean
enum Boolean { FALSE = 0, TRUE = 1 }; 
#endif

/* 
 * For a new disconst.rdr file, the constants in the class ParseDISConst{}: 
 * NUM_FIELDS, NUM_KINDS, NUM_DOMAINS, NUM_WARHEADS, NUM_ENTITIES, may need
 * to be readjusted.
 */

class ParseDISConst {

public:

  ParseDISConst( char* file_name );
  ~ParseDISConst() {}

  Boolean getDISID( const char* model_name, 
		    Short& kind, Short& domain, Short& contry,
	            Short& category, Short& subcategorym, Short& specific );
  Short is_file_valid() { return( file_valid ); }

private:
  
  ifstream infile;
  Boolean file_valid;

  struct NameID {
     char name[40];
     Short id;
  };

  const Short NUM_FIELDS = 8;
  const Short NUM_KINDS = 7;
  const Short NUM_DOMAINS = 5;
  const Short NUM_WARHEADS = 17;

  NameID field_table[NUM_FIELDS];
  NameID kind_table[NUM_KINDS];
  NameID domain_table[NUM_DOMAINS];
  NameID warhead_table[NUM_WARHEADS];

  struct NameDISID {
     char name[40];
     Short kind, domain, contry, category, subcategory, specific;
  };

  const Short NUM_ENTITIES = 600;
  NameDISID disid_table[NUM_ENTITIES];
  Short num_disids;

  void skipComment( char* str );
  void fillTable( NameID table[], Short num_items );
  void skipWarheadsHeader();
  void skipWarheadsFormat();
  void fillTable();

  Short search( NameID table[], Short num_intems, char* name );

};

#endif __NPS_PARSE_DISCONST__
