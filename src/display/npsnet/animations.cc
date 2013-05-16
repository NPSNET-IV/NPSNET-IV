// File: <animations.cc>

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

#include <fstream.h>
#include <iostream.h>
#include <pf.h>
#include <pfdu.h>
#include <pfdb/pfflt.h>
#include <string.h>

#include "animations.h"
#include "fileio.h"
#include "loadBBoard.h"
#include "imdefaults.h"
#include "imstructs.h"


pfSequence *load_sequence(long mode, long reps, int &anim_count,
                               int num_effects, struct Eff *effects)
{
   pfSequence	*animation;
   pfGroup	*frame;

   anim_count = 0;
   animation = pfNewSeq();

   for (int i = 0; i < num_effects; i++, effects++) {
      switch (effects->effect_type) {
         case (TYPE_MODEL):
            frame = (pfGroup *) pfdLoadFile(effects->filename);
            break;
         case (TYPE_BB):
            frame = (pfGroup *) LoadBBoard(effects->filename);
            break;
         default:
            frame = NULL;
            cerr << "Unsupported model type in load_sequence()\n";
      }
      if (frame != NULL) {
         pfAddChild(animation, frame);
         pfSeqTime(animation, anim_count, effects->delay);
         anim_count++;
      }
   }

   if (anim_count > 0) {
      pfSeqInterval(animation, mode, 0, long(anim_count-1));
      pfSeqDuration(animation, 1.0f, reps);
      pfSeqMode(animation, PFSEQ_STOP);
   } else if (animation != NULL) {
      cerr << "ERROR:\tCould not load sequence" << endl;
      pfDelete(animation);
      animation = NULL;
   }

   return animation;
}


/* PRE-INITMAN VERSION
pfSequence *load_sequence ( char *filename, long mode, long reps, 
                            int &anim_count )
{
   char temp[80];
   char anim_name[50];
   double anim_time;
   pfSequence *animation = NULL;
   pfGroup *frame;
   char actual_filename[PF_MAXSTRING];
   int valid_read = FALSE;
   char model_type[255];
   int quit = FALSE;

   pfFindFile(filename,actual_filename,R_OK);
   ifstream anim_file ( actual_filename );

   anim_count = 0;
   if ( !anim_file )
      return NULL;
   else
      {
      animation = pfNewSeq();
      while ( !quit )
         {
         valid_read = npsfscanf ( anim_file, "%s%s%d",
                                  model_type, anim_name, &anim_time );
         switch ( valid_read ) {
            case NPSFSCANF_EOF:
               quit = TRUE;
               break;
            case NPSFSCANF_UNDERFLOW:
                  print_npsfscanf_error ( valid_read, 
                                          "While loading sequence:");
               cerr << "Error in animation file:  "
                    << actual_filename << ".\n";
               cerr << "   Incorrect format, line data: " << temp << endl;
               break;
            case TRUE:
               break;
            default:
               print_npsfscanf_error ( valid_read,
                                       "While loading sequence:");
               break;
         }

         if ( valid_read && !quit ) 
            {

            if ( (strncmp(model_type,"MODEL_SEQ",3) == 0) ) {
               frame = (pfGroup *)pfdLoadFile(anim_name);
            }
            else if ( (strncmp(model_type,"BB_SEQ",3) == 0) ) {
               frame = (pfGroup *)LoadBBoard(anim_name);
            }
            else {
               frame = NULL;
               cerr << "Unsupported model_type in animation file:" << endl;
               cerr << "\tModel type = " << model_type << endl;
            }

            if ( !quit && (frame != NULL) )
               {
               pfAddChild ( animation, frame );
               pfSeqTime(animation,anim_count,anim_time);
               anim_count++;
               }
            else
               {
               cerr << "Individual animation file not found:  "
                    << "->" << anim_name << "<-" << endl;
               }
            }
         }
      }
   if ( anim_count > 0 )
      {
      pfSeqInterval(animation,mode,0,long(anim_count-1));
      pfSeqDuration(animation,1.0f,reps);
      pfSeqMode(animation,PFSEQ_STOP);
      }
   else if ( animation != NULL )
      {
      cerr << "ERROR:\tCould not load sequence from " << filename << endl;
      pfDelete(animation);
      animation = NULL;
      }

   return animation;
}
*/
