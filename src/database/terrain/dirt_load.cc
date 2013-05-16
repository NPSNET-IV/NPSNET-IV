// File: <dirt_load.cc>

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

#include <math.h>
#include <pf.h>
#include <pfutil.h>
#include <string.h>
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>

#include "common_defs2.h"
#include "fileio.h"
#include "bbox.h"

#ifndef NONPSSHIP
#include "pvs.h"
#endif

#include "database_const.h"
#include "collision_const.h"
#include "dirt_flt.h"
#ifndef NOS1kLOADER
#include <sys/types.h>
#include <sys/stat.h>
#include "dirt_s1k.h"
#endif

#include "dirt_intersect.h"

#include "im.tab.h"
#include "imstructs.h"
#include "imclass.h"

extern InitManager *initman;
#define TEMPUTMFILE "/tmp/npsnet.temp.utm"
// Globals used by this library
extern volatile DYNAMIC_DATA *G_dynamic_data;
extern volatile STATIC_DATA  *G_static_data;
extern          pfGroup      *G_fixed;
extern          VEHTYPE_LIST *G_vehtype;
extern          OBJTYPE_LIST *G_objtype;

int load_terrain()
{
   /*
   char fileName[FILENAME_SIZE];
   char mask_val[FILENAME_SIZE];
   char option[FILENAME_SIZE];
   */
   char		*dot;
   pfGroup	*terrain_node; 
   struct TM	im_masks[MAX_MASKS];
   int		num_masks;
#ifndef NONPSSHIP
   pfSwitch     *water_switch;
#endif

   initman->get_terrain_masks((struct TM**) im_masks, &num_masks);

   for (int i = 0; i < num_masks; i++) {
      //what kind of file do we have to load
      //code lifted and hacked from SGI loadfile.c
      // look for final "." in filename
      if ((dot = strrchr(im_masks[i].filename, '.')) == NULL) {
         pfNotify(PFNFY_NOTICE, PFNFY_PRINT,
              "load_terrain(): No file type extension, loading not possible.");
         return NULL;
      }
      if (strcmp(dot, ".flt") == 0) {
         // Flight file
         terrain_node = LoadDirtFlt(im_masks[i].filename);
      }
#ifndef NOS1kLOADER
      else if (strcmp(dot, ".s1k") == 0) {
         if (G_dynamic_data->TDBoverride) {
            // command line override
            cerr << "Reading the Overridden database area\n"; 
            terrain_node = LoadDirtS1kOverride(im_masks[i].filename);
         } else {
            // s1000 static databases
            terrain_node = LoadDirtS1k(im_masks[i].filename);
         }
      } 
      else if (strcmp(dot, ".s1000") == 0) {
           // paged s1k databases
           terrain_node = LoadDirtS1kPaged(im_masks[i].filename);
      }
#endif
      else {
         pfNotify(PFNFY_FATAL, PFNFY_PRINT,
               "File \"%s\" not loaded -- unknown file type",
               im_masks[i].filename);
         return NULL;
      }
      if (terrain_node == NULL) {
         pfNotify(PFNFY_FATAL, PFNFY_PRINT,
               "File \"%s\" not loaded -- Due to an error",
               im_masks[i].filename);
         return NULL;
      }

      pfAddChild(G_fixed, terrain_node);

      switch (im_masks[i].mask) {
         case (BRIDGE_MASK):
            set_intersect_mask(terrain_node,
                        (PERMANENT_BRIDGE<<PERMANENT_SHIFT), COLLIDE_DYNAMIC);
            break;
         case (FOLIAGE_MASK):
            set_intersect_mask(terrain_node,
                        (PERMANENT_FOILAGE<<PERMANENT_SHIFT), COLLIDE_DYNAMIC);
            break;
         case (TERRAIN_MASK):
            set_intersect_mask(terrain_node,
                        (PERMANENT_DIRT<<PERMANENT_SHIFT), COLLIDE_DYNAMIC);
            break;
         case (TUNNEL_MASK):
            set_intersect_mask(terrain_node,
                        (PERMANENT_TUNNEL<<PERMANENT_SHIFT), COLLIDE_DYNAMIC);
            break;
         case (WATER_MASK):
#ifndef NONPSSHIP
//**New - Add switch to turn water on/off during isect & draw of ship_veh
            water_switch = pfNewSwitch();
            pfAddChild(water_switch,terrain_node);
            pfSwitchVal(water_switch,PFSWITCH_ON);
            addWaterCell(water_switch);
            pfRemoveChild(G_fixed, terrain_node);
            pfAddChild(G_fixed, water_switch);
#endif
            set_intersect_mask(terrain_node,
                        (PERMANENT_WATER<<PERMANENT_SHIFT), COLLIDE_DYNAMIC);
            break;
         default:
            cerr << "Unknown Terrain Mask, using TERRAIN_MASK\n";
            set_intersect_mask(terrain_node,
                        (PERMANENT_MASK<<PERMANENT_SHIFT), COLLIDE_DYNAMIC);
      }
      pfSync();
   }

   return (0);

/*
//   pfBox  *mybox;
   ifstream terrain_file((char *)G_static_data->terrain_file);
   if (!terrain_file) {
      cerr << "Could not open requested terrain file, using default\n";
      terrain_file.clear();
      terrain_file.open (D_TERRAIN_FILE, ios::in);
      if (!terrain_file) { 
         cerr << "No terrain file found, Exiting\n";  
         return 1;
         }
      }


   while ( read_string ( terrain_file, option ) ) {
      if ( !strncmp(option, CONFIG_COMMENT, 1) )
         read_string_to_EOL ( terrain_file, fileName );
      else if ( !strncmp(option, TERRAIN_FILE, 1) ) {
         terrain_file >> fileName;

         //what kind of file do we have to load
         //code lifted and hacked from SGI loadfile.c
         // look for final "." in filename
         if ((dot = strrchr(fileName, '.')) == NULL) {
           pfNotify(PFNFY_NOTICE, PFNFY_PRINT,
             "LoadFile(): No file type extension. loading not possible.");
           return NULL;
         }
         if (strcmp(dot, ".flt") == 0){
           //flight files
           terrain_node = LoadDirtFlt(fileName);
//           pfuTravCalcBBox(terrain_node, mybox);
cerr<<fileName<<":";
//cerr<<"bbox min/max[X] "<<mybox->min[PF_X]<<"/"<<mybox->max[PF_X];
//cerr<<" bbox min/max[Y] "<<mybox->min[PF_Y]<<"/"<<mybox->max[PF_Y]<<endl;
cerr<<"num_textures "<<G_dynamic_data->number_of_textures<<endl;

         }
#ifndef NOS1kLOADER
         else
         if (strcmp(dot, ".s1k") == 0)
           if(G_dynamic_data->TDBoverride){
             //command line override
             cerr << "Reading the Overridden database area\n"; 

             terrain_node = LoadDirtS1kOverride(fileName);
           } else{
             //s1000 static databases
             terrain_node = LoadDirtS1k(fileName);
           }
         else if (strcmp(dot, ".s1000") == 0)
           //paged s1k databases
           terrain_node = LoadDirtS1kPaged(fileName);
#endif
         else {
           pfNotify(PFNFY_FATAL, PFNFY_PRINT,
               "File \"%s\" not loaded -- unknown file type", fileName);
           return NULL;
         }
         if(terrain_node == NULL){
           pfNotify(PFNFY_FATAL, PFNFY_PRINT,
               "File \"%s\" not loaded -- Due to an error", fileName);
           return NULL;
         }
   
         read_string_to_EOL (terrain_file, mask_val); 
   
   #ifdef DEBUG
               cerr << mask_val  << endl;
   #endif
         
         //set up the intersection masks
         if (!strcmp(mask_val,"TERRAIN_MASK"))
                  set_intersect_mask(terrain_node,
                                     (PERMANENT_DIRT<<PERMANENT_SHIFT),
                                     COLLIDE_DYNAMIC);
         else if (!strcmp(mask_val,"BRIDGE_MASK"))
                  set_intersect_mask(terrain_node,
                                     (PERMANENT_BRIDGE<<PERMANENT_SHIFT),
                                     COLLIDE_DYNAMIC);
         else if (!strcmp(mask_val,"WATER_MASK"))
                  set_intersect_mask(terrain_node,
                                     (PERMANENT_WATER<<PERMANENT_SHIFT),
                                     COLLIDE_DYNAMIC);
         else if (!strcmp(mask_val,"FOLIAGE_MASK"))
                  set_intersect_mask(terrain_node,
                                     (PERMANENT_FOILAGE<<PERMANENT_SHIFT),
                                     COLLIDE_DYNAMIC);
         else if (!strcmp(mask_val,"TUNNEL_MASK"))
                  set_intersect_mask(terrain_node,
                                     (PERMANENT_TUNNEL<<PERMANENT_SHIFT),
                                     COLLIDE_DYNAMIC);
         else {
                  cerr << "Unknown Terrain Mask, using TERRAIN_MASK\n";
                  set_intersect_mask(terrain_node,
                                     (PERMANENT_MASK<<PERMANENT_SHIFT),
                                     COLLIDE_DYNAMIC);
         }
         pfAddChild (G_fixed, terrain_node);
//         pfAddChild (G_fixed,terrain_node);
         pfSync();
      }
   }
   return 0;
*/
}

#ifndef NOS1kLOADER
void modifyUTMfile(char *filename)
//read the given filename and adjust it to reflect the desired coordinates
{
  FILE *newUTM, *orgUTM;
  char tempString[255];
  float northing, easting, width, height;
  char letter[5];
  int zone,datum;

  //the format of the the file has two comement lines on top
  // the third line has the info we need, we have to be careful since we are
  // faking a few things out here
  if(NULL == (orgUTM = fopen(filename,"r"))){
    printf("Unable to open the UTM file %s\n",filename);
    exit(0);
  }
  if(NULL == (newUTM = fopen(TEMPUTMFILE,"w"))){
    printf("Unable to open the temp UTM file %s\n",TEMPUTMFILE);
    exit(0);
  }

  //the two comment lines
  fgets(tempString,255,orgUTM);
  fprintf(newUTM,"%s",tempString);
  fgets(tempString,255,orgUTM);
  fprintf(newUTM,"%s",tempString);

  //now the good stuff
  fscanf(orgUTM, "%f %f %d %s %d %f %f ",
    &northing, &easting, &zone, letter, &datum, &width, &height);
  fclose(orgUTM);

  fprintf(newUTM,"%10.0f %10.0f %3d %1s %1d %10.0f %10.0f\n",
     northing + G_dynamic_data->TDBoffsets[Y],
     easting  + G_dynamic_data->TDBoffsets[X],
     zone, letter, datum,
     G_dynamic_data->bounding_box.xsize,
     G_dynamic_data->bounding_box.ysize);
   
  fclose(newUTM);

  chmod( TEMPUTMFILE, S_IRWXU|S_IRWXG|S_IRWXO);
    
  strcpy(filename,TEMPUTMFILE);
  
}

int readSubsection(char *filename)
//read the file that contains the bounding box parameters
{
  int cnt;
  FILE *subfile;

  if(NULL == (subfile = fopen(filename,"r"))){
    printf("Unable to open the terrain subsection file %s\n",filename);
    exit(0);
  }

  cnt = fscanf(subfile, "%f %f %f %f ",
                &G_dynamic_data->bounding_box.xmin,
                &G_dynamic_data->bounding_box.ymin,
                &G_dynamic_data->bounding_box.xmax, 
                &G_dynamic_data->bounding_box.ymax);
  fclose(subfile);

  printf("\tFrom %s:\n",filename);
  printf("\t\tSW X value = %10.1f\n",G_dynamic_data->bounding_box.xmin);
  printf("\t\tSW Y value = %10.1f\n",G_dynamic_data->bounding_box.ymin);
  printf("\t\tNE X value = %10.1f\n",G_dynamic_data->bounding_box.xmax);
  printf("\t\tNE Y value = %10.1f\n",G_dynamic_data->bounding_box.ymax);
  
  return(cnt);
} 
    
#endif

