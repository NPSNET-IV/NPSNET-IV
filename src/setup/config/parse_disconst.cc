// File: <parse_disconst.cc>

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

/* File:        hashtable.h
 * Description: hash table classes
 * Revision:    1.0 - 05May95
 *
 * Refs:    a)  Military Standard--Protocol Data Units for Entity
 *              Information and Entity Interaction in a Distributed
 *              Interactive Simulation (DIS) (1.0) (30Oct91)
 *
 * Author:      Jiang Zhu
 *              CS Department, Naval Postgraduate School
 * Internet:    zhuj@taurus.cs.nps.navy.mil
 */

#include <stdio.h>
#include <string.h>

#include "parse_disconst.h"


ParseDISConst::ParseDISConst( char* file_name ): 
   infile(file_name), file_valid(TRUE), num_disids(0)
{
   if ( infile == 0 )
   {
      file_valid = FALSE;
      cerr << "invalid file name from ParseDISConst::ParseDISConst()" << endl;
   } else
   {
      // Fill the tables.
      fillTable( field_table, NUM_FIELDS );
      fillTable( kind_table, NUM_KINDS );
      fillTable( domain_table, NUM_DOMAINS );
      fillTable( warhead_table, NUM_WARHEADS );

      skipWarheadsHeader();
      if ( file_valid == TRUE )
      {
         char parenthesis;
         infile >> parenthesis;
         if ( parenthesis != '(' )
         {
            file_valid = FALSE;
            cerr << "invalid file format from ParseDISConst::ParseDISConst()" << endl;
         } else
         {
            skipWarheadsFormat();

            if ( file_valid == TRUE )
	    {
               char diswarheads[20];
   	       skipComment( diswarheads );
  	       if ( strcmp(diswarheads, "DISWARHEADS") != 0 )
               {
      	          file_valid = FALSE;
                  cerr << "invalid file format from ParseDISConst::ParseDISConst()" << endl;
               } else
               {
               	 infile >> parenthesis;

               	 if ( parenthesis != '(' )
               	 {
                    file_valid = FALSE;
                    cerr << "invalid file format from ParseDISConst::ParseDISConst()" << endl;
                 }
               }

               if ( file_valid == TRUE ) fillTable();
	    }
	 }
      }
   } // end if
   
} // end ParseDISConst::ParseDISConst()


void
ParseDISConst::skipComment( char* first_word )
{
   const Short COMMENTLINE_LENGTH = 200;

   char commentline[COMMENTLINE_LENGTH];

   while ( 1 )
   {
      infile >> first_word;
      if ( first_word[0] != ';' ) 
	 break;
      else
	 infile.getline( commentline, COMMENTLINE_LENGTH );
   }
}


void
ParseDISConst::fillTable( NameID table[], Short num_items )
{
   char brace;
 
   skipComment( table[0].name );
   infile >> brace >> table[0].id >> brace;

   for ( Short i = 1; i < num_items; i++ )
      infile >> table[i].name >> brace >> table[i].id >> brace;
}

void
ParseDISConst::skipWarheadsHeader()
{
   const Short HEADERLINE_LENGTH = 200;

   char headerline[HEADERLINE_LENGTH];
   char brace;

   infile >> headerline >> brace;
   if ( (strcmp(headerline, "DISWARHEADS") != 0) || (brace != '{') )
   {
      file_valid = FALSE;
      cerr << "invalid file format from ParseDISConst::skipWarheadsHeader()" << endl;
   } else
   {
      while ( 1 )
      {
        infile >> headerline;
        if ( headerline[0] == '}' )
           break;
        else
           infile.getline( headerline, HEADERLINE_LENGTH );
      }
   } // end if

} // ParseDISConst::skipWarheadsHeader()


void
ParseDISConst::skipWarheadsFormat()
{
   char parenthesis;

   infile >> parenthesis;
   if ( parenthesis != '(' )
   {
      file_valid = FALSE;
      cerr << "invalid file format from ParseDISConst::skipWarheadsFormat()" << endl;
   } else
   {
      const Short FORMATLINE_LENGTH = 200;

      char formatline[FORMATLINE_LENGTH];
      while ( 1 )
      {
        infile >> formatline;
        if ( formatline[0] == ')' )
           break;
        else
           infile.getline( formatline, FORMATLINE_LENGTH );
      }
   } // end if

} // ParseDISConst::skipWarheadsFormat()


void
ParseDISConst::fillTable()
{
   char platform_name[40];
   skipComment( platform_name ); 
   strncpy( disid_table[0].name, platform_name+1, strlen(platform_name) - 1 );

   char parenthesis;
   Boolean first_line = TRUE;	// first line of the platforms
   Boolean done = FALSE;
   while ( done == FALSE )
   {
     if ( first_line != TRUE ) 
	infile >>  disid_table[num_disids].name; 
     else
        first_line = FALSE;

     // Skip the first line of each entity.
     const Short LINE_LENGTH = 200;
     char platformline[LINE_LENGTH];
     infile.getline( platformline, LINE_LENGTH );

     char kind[30], domain[30];
     infile >> parenthesis >> kind >> domain;

     disid_table[num_disids].kind = search( kind_table, NUM_KINDS, kind );
     if ( strcmp(kind, "DISMunition") == 0 )
     {
	// Somehow, a temp var has to be introduced for sscanf() to work ???
	int temp;
        sscanf( domain, "%d", &temp );
        disid_table[num_disids].domain = temp;
     }
     else if ( domain[0] == '0' )
        disid_table[num_disids].domain = 0;
     else
        disid_table[num_disids].domain = search( domain_table, NUM_DOMAINS, domain );

     infile >> disid_table[num_disids].contry
            >> disid_table[num_disids].category
            >> disid_table[num_disids].subcategory
            >> disid_table[num_disids].specific;

     Short extra;
     char warhead[50];
     infile >> extra >> warhead;

     Short length = strlen( warhead );
     if ( warhead[length - 1] != ')' )
        infile >> parenthesis >> parenthesis >> parenthesis;
     else
	warhead[length - 3] = '\0';

     /* In case that someday, the wardhead field is needed.
     if ( strcmp( kind, "DISMunition" ) == 0 )
     {
        if ( warhead[0] != '0' )
          search( warhead_table, NUM_WARHEADS, warhead );
     }
     */

     num_disids++;

     infile >> parenthesis;
     if ( parenthesis == ';' )
     {
        while ( 1 )
        {
          infile.getline( platformline, LINE_LENGTH );
          infile >> parenthesis;
	  if ( parenthesis != ';' ) break;
 	}
     } 

     if ( parenthesis == ')' ) done = TRUE;

   } // end while

   infile >> parenthesis;
   if ( parenthesis != ')' )
   {
      file_valid = FALSE;
      cerr << "invalid file format from ParseDISConst::fillTable()" << endl;
   }

} // end ParseDISConst::fillTable()


Short
ParseDISConst::search( NameID table[], Short num_items, char* name )
{
   for ( Short i = 0; i < num_items; i++ )
   {
      if ( strcmp(name, table[i].name) == 0 ) return( table[i].id );
   }

   cerr << "no such name from ParseDISConst::search(): " << name << endl;
   return( -1 );
}


Boolean 
ParseDISConst::getDISID( const char* model_name,
                    	 Short& kind, Short& domain, Short& contry,
                    	 Short& category, Short& subcategory, Short& specific )
{
   Boolean found = FALSE;

   for ( Short i = 0; i < num_disids; i++ )
      if ( strcmp(model_name, disid_table[i].name) == 0 )
      {
         found = TRUE;

	 kind = disid_table[i].kind;
	 domain = disid_table[i].domain;
	 contry = disid_table[i].contry;
	 category = disid_table[i].category;
	 subcategory = disid_table[i].subcategory;
	 specific = disid_table[i].specific;

	 break;
      }

   return( found );

} // ParseDISConst::getDISID()


void main()
{

   ParseDISConst disid_parser("disconst.rdr");

   if ( disid_parser.is_file_valid() )
   {
      Short kind, domain, contry, category, subcategory, specific;

      disid_parser.getDISID( "vehicle_US_M1", kind, domain, contry, category, subcategory, specific );
      cerr << "(vehicle_US_M1 " 
	        << kind << " " << domain << " " << contry << " "
	        << category << " " << subcategory << " " << specific << ")" << endl;

      disid_parser.getDISID( "vehicle_US_M1A1", kind, domain, contry, category, subcategory, specific );
      cerr << "(vehicle_US_M1A1 " 
	        << kind << " " << domain << " " << contry << " "
	        << category << " " << subcategory << " " << specific << ")" << endl;
   }

}
