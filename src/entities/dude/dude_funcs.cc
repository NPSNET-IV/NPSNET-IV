// File: <dude_funcs.cc>

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
#include "dude_funcs.h"
#include <pfflt.h>  

#define MAXTEXTURES1 2

// Global variables (all caps)
pfTexture *TEXTURES1[2];
pfTexture  *texuniform = NULL;
pfTexture  *texface = NULL;
int        CURRENTTEX1;
    

pfGroup * LoadDude (char *file, int *num_textures, pfTexture *textures)
{
  DUDE_GEOM * dude = NULL;

  char       tag;
  char       comment[200], modelfile[200], uniform[200], face[200];
  int        which, dist;
  pfVec3     center;
  char       filepath[PF_MAXSTRING];

   printf("In create_dude, about to allocate new structure\n");
   if(NULL == (dude = 
               (DUDE_GEOM *) pfMalloc(sizeof(DUDE_GEOM),
                pfGetSharedArena()))){
      cerr << "Unable to malloc Space for dude  " << file <<"\n";
      return(NULL);
   }
  
   // build the structure to contain dudes
   dude->rootGP      = pfNewGroup();
   dude->hullDCS     = pfNewDCS();
   pfNodeName(dude->hullDCS, "main_DCS");

   dude->sw          = pfNewSwitch();
   dude->sw1         = pfNewSwitch();
   dude->sw2         = pfNewSwitch();
   dude->sw3         = pfNewSwitch();
   dude->sw4         = pfNewSwitch();

   pfNodeName(dude->sw, "sw_SW");
   pfNodeName(dude->sw1, "sw1_SW");
   pfNodeName(dude->sw2, "sw2_SW");
   pfNodeName(dude->sw3, "sw3_SW");
   pfNodeName(dude->sw4, "sw4_SW");


   dude->torsoGP     = pfNewGroup();
   dude->far_bodyDCS = pfNewDCS();
   pfNodeName(dude->far_bodyDCS, "far_body_DCS");
   dude->headDCS     = pfNewDCS();
   pfNodeName(dude->headDCS, "head_DCS");
   dude->headGP      = pfNewGroup();

   dude->armrightGP     = pfNewGroup();
   dude->armrightDCS    = pfNewDCS();
   pfNodeName(dude->armrightDCS, "armright_DCS");
   dude->armrightlowDCS = pfNewDCS();
   pfNodeName(dude->armrightlowDCS, "armrightlow_DCS");

   dude->armleftGP      = pfNewGroup();
   dude->armleftDCS     = pfNewDCS();
   pfNodeName(dude->armleftDCS, "armleft_DCS");
   dude->armleftlowDCS  = pfNewDCS();
   pfNodeName(dude->armleftlowDCS, "armleftlow_DCS");

   dude->legrightGP     = pfNewGroup();
   dude->legrightDCS    = pfNewDCS();
   pfNodeName(dude->legrightDCS, "legright_DCS");
   dude->legrightlowDCS = pfNewDCS();
   pfNodeName(dude->legrightlowDCS, "legrightlow_DCS");

   dude->legleftGP      = pfNewGroup();
   dude->legleftDCS     = pfNewDCS();
   pfNodeName(dude->legleftDCS, "legleft_DCS");
   dude->legleftlowDCS  = pfNewDCS();
   pfNodeName(dude->legleftlowDCS, "legleftlow_DCS");

   // determine which model to load after locating it
   ifstream which_dude_file (file);
   if(which_dude_file == NULL)
   {
     //could not open file requested
     cerr << "Unable to open the requested dynamic models file " 
            << file << ".\n";
     return(NULL);
   } 
 
  //read the file
  while (which_dude_file >> tag)
  {
    //what kind of line is it?
    switch (tag) 
    {
      case '#': // a comment line
         which_dude_file.getline(comment,200);
         break;
      case ' ': // empty line
         break;
      case 'L': // L model line
         which_dude_file >> which >> dist >> modelfile 
         >> uniform >> face;

         pfFindFile (modelfile, filepath, R_OK );

         switch (which){
           case 1: // the close in guy

             // load all textures upfront to speed up dude
             CURRENTTEX1 = 0;
             TEXTURES1[0] = textures;
             texuniform = loadTexs(uniform);
             texface = loadTexs(face);
             *num_textures = CURRENTTEX1;

             if(!read_medium_dude(modelfile, dude)){
               cerr << "Error reading " << modelfile << "\n";
               return(NULL);
             } 
             break;
           case 2: // the middle in guy
             if(!read_medium_far_dude(modelfile, dude)){
               cerr << "Error reading "<< modelfile << "\n";
               return(NULL);
             } 
             break;
           case 3: // the far in guy
             if(!read_far_dude(modelfile, dude)){
               cerr << "Error reading "<< modelfile << "\n";
               return(NULL);
             } 
             break;
           default:
             cerr << "Unknown dude type " << which <<" in file "
                  << file << "\n"; 
             return(NULL);
          }
          break;

      default:
         cerr << "Unkown tag" << tag<< "\n";
         return(NULL);

    }
  }

   which_dude_file.close();
   //everthing should have gone ok

   // building the tree by adding the body parts
   pfAddChild(dude->rootGP, dude->hullDCS); 

   // add the first LOD node for far and other dudes
   pfAddChild(dude->hullDCS, dude->sw);

   pfAddChild(dude->sw, dude->torsoGP); 

   // add the far away guy to the tree
   pfAddChild(dude->sw, dude->far_bodyDCS);

   pfAddChild(dude->far_bodyDCS, dude->far_body);

   pfAddChild(dude->torsoGP, dude->body);
   pfAddChild(dude->torsoGP, dude->headDCS);

   pfAddChild(dude->headDCS, dude->headGP);

   pfAddChild(dude->headGP, dude->skull);
   pfAddChild(dude->headGP, dude->face);

   // add arms for med & medfar models
   pfAddChild(dude->torsoGP, dude->armleftDCS);

   pfAddChild(dude->armleftDCS, dude->sw1);

   pfAddChild(dude->sw1, dude->armleftGP); 
   pfAddChild(dude->sw1, dude->armleftmed); 

   pfAddChild(dude->armleftGP, dude->armleftup); 
   pfAddChild(dude->armleftGP, dude->armleftlowDCS); 

   pfAddChild(dude->armleftlowDCS, dude->armleftlow);
 
   pfAddChild(dude->torsoGP, dude->armrightDCS);

   pfAddChild(dude->armrightDCS, dude->sw2);

   pfAddChild(dude->sw2, dude->armrightGP); 
   pfAddChild(dude->sw2, dude->armrightmed); 

   pfAddChild(dude->armrightGP, dude->armrightup); 
   pfAddChild(dude->armrightGP, dude->armrightlowDCS); 

   pfAddChild(dude->armrightlowDCS, dude->armrightlow);

   // add legs for med & medfar models
   pfAddChild(dude->torsoGP, dude->legleftDCS);

   pfAddChild(dude->legleftDCS, dude->sw3);

   pfAddChild(dude->sw3, dude->legleftGP); 
   pfAddChild(dude->sw3, dude->legleftmed); 

   pfAddChild(dude->legleftGP, dude->legleftup); 
   pfAddChild(dude->legleftGP, dude->legleftlowDCS); 

   pfAddChild(dude->legleftlowDCS, dude->legleftlow);
 
   pfAddChild(dude->torsoGP, dude->legrightDCS);

   pfAddChild(dude->legrightDCS, dude->sw4);

   pfAddChild(dude->sw4, dude->legrightGP); 
   pfAddChild(dude->sw4, dude->legrightmed); 

   pfAddChild(dude->legrightGP, dude->legrightup); 
   pfAddChild(dude->legrightGP, dude->legrightlowDCS); 

   pfAddChild(dude->legrightlowDCS, dude->legrightlow);
   pfSwitchVal(dude->sw,0);
   pfSwitchVal(dude->sw1, 0);
   pfSwitchVal(dude->sw2, 0);
   pfSwitchVal(dude->sw3, 0);
   pfSwitchVal(dude->sw4, 0);

   // set the origin of model to that of NPS
   pfDCSTrans(dude->hullDCS, 0.0f, 0.0f, 1.0f);   
   pfDCSRot(dude->hullDCS, 0.0f, 0.0f, 0.0f);
   return(dude->rootGP);
}

pfTexture * loadTexs(char* tex_to_load)
{
   pfTexture *tex = NULL;
   void *arena = pfGetSharedArena();
  
   TEXTURES1[CURRENTTEX1] = pfNewTex(arena);
   pfLoadTexFile(TEXTURES1[CURRENTTEX1],tex_to_load);
   pfTexName(TEXTURES1[CURRENTTEX1],tex_to_load);
   tex = TEXTURES1[CURRENTTEX1++];

   return(tex);
}

int read_medium_dude(char* file, DUDE_GEOM *dude)
{
  // Performer variables to make dude
  pfGeode    *geode;
  pfGeoSet   *gset;
  pfGeoState *gst;
  pfTexEnv   *tenv;
  void       *arena;
  char       tag;
  char       comment[200];
  int        items;
  float      data;

    // set file to appropriate dude model 
    ifstream infile (file);

    //read the file
    while (infile >> tag)
    {
      //what kind of line is it?
      switch (tag) 
      {
        case '#': // a comment line
                  infile.getline(comment,200);
                  break;

        case ' ': // empty line
                  break;

        case 'B': // B color line
                  infile >> items;
                  for (int b=0; b< items; b++) 
                  {  
                    for (int j=0; j< 4; j++)
                    { 
                      infile >> data;
                      data = (float) data;
                      dude->colors_med[b][j] =  data;
                     } 
                  }
                  break;

        case 'C': // C normals
                  infile >> items;
                  for (int c=0; c< items; c++) 
                  {  
                    for (int j=0; j< 3; j++)
                    { 
                      infile >> data;
                      data = (float) data;
                      dude->norms_med[c][j] =  data;
                     } 
                  }
                  break;

        case 'D': // D texture s and t
                  infile >> items;
                  for (int d=0; d< items; d++) 
                  {  
                    for (int j=0; j< 2; j++)
                    { 
                      infile >> data;
                      data = (float) data;
                      dude->texs_med[d][j] =  data;
                      } 
                  }
                  break;

        case 'E': // E vertices 
                  infile >> items;
                  for (int e=0; e< items; e++) 
                  {  
                    for (int j=0; j< 3; j++)
                    { 
                      infile >> data;
                      data = (float) data;
                      dude->verts_med[e][j] =  data;
                    } 
                  }
                  break;

        case 'F': // F index arrangements 
                  infile >> items;
                  for (int f=0; f< items*4; f++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->index_med[f] =  data;
                  }
                  break;

        case 'G': // G correlate normals 
                  infile >> items;
                  for (int g=0; g< items; g++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->normvert_med[g] =  data;
                  }
                  break;
 
        case 'H': // H index arrangements 
                  infile >> items;
                  for (int h=0; h< items*4; h++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->texsvert_med[h] =  data;
                  }
                  break;

        case 'I': // I index arrangements for head
                  infile >> items;
                  for (int i=0; i< items*3; i++) 
                  {  
                    infile >> data;
                    data = (float) data;
                    dude->verts_head_med[i] =  data;
                  }
                  break;

        case 'J': // J index arrangements for head
                  infile >> items;
                  for (int j=0; j< items*4; j++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->index_head_med[j] =  data;
                  }
                  break;

        case 'K': // K normals for head
                  infile >> items;
                  for (int k=0; k< items; k++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->norms_head_med[k] =  data;
                  }
                  break;

        case 'L': // L textures for head
                  infile >> items;
                  for (int l=0; l< items*4; l++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->texs_head_med[l] =  data;
                  }
                  break;

        case 'M': // M index arrangements for arms forearm
                  infile >> items;
                  for (int m=0; m< items*3; m++) 
                  {  
                    infile >> data;
                    data = (float) data;
                    dude->verts_arm_med_low[m] =  data;
                  }
                  break;

        case 'N': // N index arrangements for arms biceps
                  infile >> items;
                  for (int n=0; n< items*3; n++) 
                  {  
                    infile >> data;
                    data = (float) data;
                    dude->verts_arm_med_up[n] =  data;
                  }
                 break;

        case 'O': // O index arrangements for legs calf
                  infile >> items;
                  for (int o=0; o< items*3; o++) 
                  {  
                    infile >> data;
                    data = (float) data;
                    dude->verts_leg_med_low[o] =  data;
                  }
                  break;

        case 'P': // P index arrangements for legs thigh
                  infile >> items;
                  for (int p=0; p< items*3; p++) 
                  {  
                    infile >> data;
                    data = (float) data;
                    dude->verts_leg_med_up[p] =  data;
                  }
                  break;

        case 'U': // U vertices for face
                  infile >> items;
                  for (int u=0; u< items*3; u++) 
                  {  
                    infile >> data;
                    data = (float) data;
                    dude->verts_face_med[u] =  data;
                  }
                  break;

        case 'V': // V index arrangements for face
                  infile >> items;
                  for (int v=0; v< items*4; v++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->index_face_med[v] =  data;
                  }
                  break;

        case 'W': // W normals for face
                  infile >> items;
                  for (int w=0; w< items; w++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->norms_face_med[w] =  data;
                  }
                  break;

        case 'X': // X textures for face
                  infile >> items;
                  for (int x=0; x< items*4; x++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->texs_face_med[x] =  data;
                  }
                  break;

        case 'Z': // Z index arrangements for biceps 
                  infile >> items;
                  for (int z=0; z< items*4; z++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->index_medB[z] =  data;
                  }
                  break;
      }
    }

    // *** create torso of dude_med ***
    arena = pfGetSharedArena();
    gset  = pfNewGSet(arena);
   
    // set the coordinate, normal and color arrays
    // and their cooresponding index arrays
    pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
               dude->verts_med, dude->index_med);
    pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM, 
               dude->norms_med, dude->normvert_med);
    pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX, 
               dude->texs_med, dude->texsvert_med);
    pfGSetPrimType(gset, PFGS_QUADS);
    pfGSetNumPrims(gset, 5);
    
    // create a geostate from shared memory, enable 
    // texturing (in case that's not the default), and
    // set the geostate for this geoset 
    gst = pfNewGState(arena);
    pfGStateMode(gst, PFSTATE_ENTEXTURE, 1);
    pfGSetGState(gset, gst);
     
    // create a new texture from shared memory,
    // load a texture file and add texture to geostate 
    pfGStateAttr(gst, PFSTATE_TEXTURE, texuniform);
    
    // create a new texture environment from shared memory,
    // decal the texture since the geoset has no color to 
    // modulate, set the texture environment for this geostate
    tenv = pfNewTEnv(arena);
    pfTEnvMode(tenv, PFTE_DECAL);
    pfGStateAttr(gst, PFSTATE_TEXENV, tenv);

    dude->body = pfNewGroup();
    geode  = pfNewGeode();
    pfAddGSet(geode, gset);
    
    pfAddChild(dude->body, geode);
    pfDCSRot(dude->hullDCS, 180.0f, 0.0f, 0.0f);
    pfDCSTrans(dude->hullDCS, 0.0f, 0.0f, 0.0f);
 
    // *** create upper arms of dude_med ***
    arena = pfGetSharedArena();
    gset  = pfNewGSet(arena);
   
    // set the coordinate, normal and color arrays
    // and their cooresponding index arrays
    pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
               dude->verts_arm_med_up, dude->index_medB);
    pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM,
               dude->norms_med, dude->normvert_med);
    pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX,
               dude->texs_med, dude->texsvert_med);
    pfGSetPrimType(gset, PFGS_QUADS);
    pfGSetNumPrims(gset, 5);

    // create a geostate from shared memory, enable 
    // texturing (in case that's not the default), and
    // set the geostate for this geoset 
    gst = pfNewGState(arena);
    pfGStateMode(gst, PFSTATE_ENTEXTURE, 1);
    pfGSetGState(gset, gst);
     
    // create a new texture from shared memory,
    // load a texture file and add texture to geostate 
    pfGStateAttr(gst, PFSTATE_TEXTURE, texuniform);
    
    // create a new texture environment from shared memory,
    // decal the texture since the geoset has no color to 
    // modulate, set the texture environment for this geostate
    tenv = pfNewTEnv(arena);
    pfTEnvMode(tenv, PFTE_DECAL);
    pfGStateAttr(gst, PFSTATE_TEXENV, tenv);

    dude->armrightup = pfNewGroup();
    dude->armleftup = pfNewGroup();
    geode   = pfNewGeode();
    pfAddGSet(geode, gset);

    pfAddChild(dude->armrightup, geode);
    pfAddChild(dude->armleftup, geode);
    pfDCSTrans(dude->armleftDCS, -0.1f, -0.12f, 0.8f); 
    pfDCSTrans(dude->armrightDCS, 0.4f, -0.12f, 0.8f); 

    // *** create lower arms of dude_med ***
    arena = pfGetSharedArena();
    gset  = pfNewGSet(arena);
   
    // set the coordinate, normal and color arrays
    // and their cooresponding index arrays
    pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
               dude->verts_arm_med_low, dude->index_med);
    pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM,
               dude->norms_med, dude->normvert_med);
    pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX,
               dude->texs_med, dude->texsvert_med);
    pfGSetPrimType(gset, PFGS_QUADS);
    pfGSetNumPrims(gset, 5);

    // create a geostate from shared memory, enable 
    // texturing (in case that's not the default), and
    // set the geostate for this geoset 
    gst = pfNewGState(arena);
    pfGStateMode(gst, PFSTATE_ENTEXTURE, 1);
    pfGSetGState(gset, gst);
     
    // create a new texture from shared memory,
    // load a texture file and add texture to geostate 
    pfGStateAttr(gst, PFSTATE_TEXTURE, texuniform);
    
    // create a new texture environment from shared memory,
    // decal the texture since the geoset has no color to 
    // modulate, set the texture environment for this geostate
    tenv = pfNewTEnv(arena);
    pfTEnvMode(tenv, PFTE_DECAL);
    pfGStateAttr(gst, PFSTATE_TEXENV, tenv);

    dude->armrightlow = pfNewGroup();
    dude->armleftlow  = pfNewGroup();
    geode = pfNewGeode();
    pfAddGSet(geode, gset);

    pfAddChild(dude->armrightlow, geode);
    pfAddChild(dude->armleftlow, geode);
    pfDCSTrans(dude->armleftlowDCS, 0.0f, 0.0f, -0.5f);
    pfDCSTrans(dude->armrightlowDCS, 0.0f, 0.0f, -0.5f);

    // *** create upper legs of dude_med ***
    arena = pfGetSharedArena();
    gset  = pfNewGSet(arena);
   
    // set the coordinate, normal and color arrays
    // and their cooresponding index arrays
    pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
               dude->verts_leg_med_up, dude->index_med);
    pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM,
               dude->norms_med, dude->normvert_med);
    pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX,
               dude->texs_med, dude->texsvert_med);
    pfGSetPrimType(gset, PFGS_QUADS);
    pfGSetNumPrims(gset, 5);

    // create a geostate from shared memory, enable 
    // texturing (in case that's not the default), and
    // set the geostate for this geoset 
    gst = pfNewGState(arena);
    pfGStateMode(gst, PFSTATE_ENTEXTURE, 1);
    pfGSetGState(gset, gst);
     
    // create a new texture from shared memory,
    // load a texture file and add texture to geostate 
    pfGStateAttr(gst, PFSTATE_TEXTURE, texuniform);
    
    // create a new texture environment from shared memory,
    // decal the texture since the geoset has no color to 
    // modulate, set the texture environment for this geostate
    tenv = pfNewTEnv(arena);
    pfTEnvMode(tenv, PFTE_DECAL);
    pfGStateAttr(gst, PFSTATE_TEXENV, tenv);

    dude->legrightup = pfNewGroup();
    dude->legleftup  = pfNewGroup();
    geode  = pfNewGeode();
    pfAddGSet(geode, gset);

    pfDCSTrans(dude->legleftDCS, 0.1f, -0.12f, 0.0f); 
    pfDCSTrans(dude->legrightDCS, 0.3f, -0.12f, 0.0f); 
    pfDCSRot(dude->legrightDCS, 180.0f, 0.0f, 0.0f);  
 
    pfAddChild(dude->legrightup, geode);
    pfAddChild(dude->legleftup, geode);

    // *** create lower legs of dude_med ***
    arena = pfGetSharedArena();
    gset  = pfNewGSet(arena);

    // create a geostate from shared memory, enable 
    // texturing (in case that's not the default), and
    // set the geostate for this geoset 
    gst = pfNewGState(arena);
    pfGStateMode(gst, PFSTATE_ENTEXTURE, 1);
    pfGSetGState(gset, gst);
     
    // set the coordinate, normal and color arrays
    // and their cooresponding index arrays
    pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
               dude->verts_leg_med_low, dude->index_med);
    pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM,
               dude->norms_med, dude->normvert_med);
    pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX,
               dude->texs_med, dude->texsvert_med);
    pfGSetPrimType(gset, PFGS_QUADS);
    pfGSetNumPrims(gset, 5);

    // create a new texture from shared memory,
    // load a texture file and add texture to geostate 
    pfGStateAttr(gst, PFSTATE_TEXTURE, texuniform);
    
    // create a new texture environment from shared memory,
    // decal the texture since the geoset has no color to 
    // modulate, set the texture environment for this geostate
    tenv = pfNewTEnv(arena);
    pfTEnvMode(tenv, PFTE_DECAL);
    pfGStateAttr(gst, PFSTATE_TEXENV, tenv);

    dude->legrightlow = pfNewGroup();
    dude->legleftlow  = pfNewGroup(); 
    geode  = pfNewGeode();
    pfAddGSet(geode, gset);

    pfAddChild(dude->legrightlow, geode);
    pfAddChild(dude->legleftlow, geode);
    pfDCSTrans(dude->legleftlowDCS, 0.0f, 0.0f, -0.45f);
    pfDCSTrans(dude->legrightlowDCS, 0.0f, 0.0f, -0.45f);

    // *** create skull of dude_med ***
    arena = pfGetSharedArena();
    gset  = pfNewGSet(arena);

    // set the coordinate, normal and color arrays
    // and their cooresponding index arrays
    pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
               dude->verts_head_med, dude->index_head_med);
    pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM,
               dude->norms_med, dude->norms_head_med);
    pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX,
               dude->texs_med, dude->texs_head_med);
    pfGSetPrimType(gset, PFGS_QUADS);
    pfGSetNumPrims(gset, 4);

    // create a geostate from shared memory, enable 
    // texturing (in case that's not the default), and
    // set the geostate for this geoset 
    gst = pfNewGState(arena);
    pfGStateMode(gst, PFSTATE_ENTEXTURE, 1);
    pfGSetGState(gset, gst);

    // create a new texture from shared memory,
    // load a texture file and add texture to geostate 
    pfGStateAttr(gst, PFSTATE_TEXTURE, texuniform);
    
    // create a new texture environment from shared memory,
    // decal the texture since the geoset has no color to 
    // modulate, set the texture environment for this geostate
    tenv = pfNewTEnv(arena);
    pfTEnvMode(tenv, PFTE_DECAL);
    pfGStateAttr(gst, PFSTATE_TEXENV, tenv);

    dude->skull = pfNewGroup();
    geode       = pfNewGeode();
    pfAddGSet(geode, gset);

    pfAddChild(dude->skull, geode);
    pfDCSTrans(dude->headDCS, 0.0625f, 0.0f, .8f); 

    // *** create face of dude_med ***
    arena = pfGetSharedArena();
    gset  = pfNewGSet(arena);

    // set the coordinate, normal and color arrays
    // and their cooresponding index arrays
    pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
               dude->verts_face_med, dude->index_face_med);
    pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM,
               dude->norms_med, dude->norms_face_med);
    pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX,
               dude->texs_med, dude->texs_face_med);
    pfGSetPrimType(gset, PFGS_QUADS);
    pfGSetNumPrims(gset, 1);

    // create a geostate from shared memory, enable 
    // texturing (in case that's not the default), and
    // set the geostate for this geoset 
    gst = pfNewGState(arena);
    pfGStateMode(gst, PFSTATE_ENTEXTURE, 1);
    pfGSetGState(gset, gst);

    // create a new texture from shared memory,
    // load a texture file and add texture to geostate 
    pfGStateAttr(gst, PFSTATE_TEXTURE, texface);
    
    // create a new texture environment from shared memory,
    // decal the texture since the geoset has no color to 
    // modulate, set the texture environment for this geostate
    tenv = pfNewTEnv(arena);
    pfTEnvMode(tenv, PFTE_DECAL);
    pfGStateAttr(gst, PFSTATE_TEXENV, tenv);

    dude->face = pfNewGroup();
    geode      = pfNewGeode();
    pfAddGSet(geode, gset);

    pfAddChild(dude->face, geode);
 
    infile.close();
      return(1);
}

int read_medium_far_dude(char* file, DUDE_GEOM *dude)
{
  // Performer variables to make dude
  pfGeode    *geode;
  pfGeoSet   *gset;
  pfGeoState *gst;
  pfTexEnv   *tenv;
  void       *arena;

  char       tag;
  char       comment[200];
  int   items;
  float data;

 
    // set file to appropriate dude model 
    ifstream infile (file);

    //read the file
    while (infile >> tag)
    {
      //what kind of line is it?
      switch (tag) 
      {
        case '#': // a comment line
                  infile.getline(comment,200);
                  break;

        case ' ': // empty line
                  break;

        case 'M': // M index arrangements for arms
                  infile >> items;
                  for (int m=0; m< items*3; m++) 
                  {  
                    infile >> data;
                    data = (float) data;
                    dude->verts_arm[m] =  data;
                  }
                  break;

        case 'Q': // Q index arrangements for arms
                  infile >> items;
                  for (int q=0; q< items*3; q++) 
                  {  
                    infile >> data;
                    data = (float) data;
                    dude->verts_leg[q] =  data;
                  }
                  break;

     }
    } 
    // *** create arms of dude_medfar ***
    arena = pfGetSharedArena();
    gset = pfNewGSet(arena);

    // set the coordinate, normal and color arrays
    // and their cooresponding index arrays
    pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
               dude->verts_arm, dude->index_medB);
    pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM,
               dude->norms_med, dude->normvert_med);
    pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX,
               dude->texs_med, dude->texsvert_med);
    pfGSetPrimType(gset, PFGS_QUADS);
    pfGSetNumPrims(gset, 5);
    
    // create a geostate from shared memory, enable 
    // texturing (in case that's not the default), and
    // set the geostate for this geoset 
    gst = pfNewGState(arena);
    pfGStateMode(gst, PFSTATE_ENTEXTURE, 1);
    pfGSetGState(gset, gst);
     
    // create a new texture from shared memory,
    // load a texture file and add texture to geostate 
    pfGStateAttr(gst, PFSTATE_TEXTURE, texuniform);
    
    // create a new texture environment from shared memory,
    // decal the texture since the geoset has no color to 
    // modulate, set the texture environment for this geostate
    tenv = pfNewTEnv(arena);
    pfTEnvMode(tenv, PFTE_DECAL);
    pfGStateAttr(gst, PFSTATE_TEXENV, tenv);

    dude->armleftmed  = pfNewGroup(); 
    dude->armrightmed = pfNewGroup();
    geode = pfNewGeode();
    pfAddGSet(geode, gset);
    pfAddChild(dude->armleftmed, geode);
    pfAddChild(dude->armrightmed, geode);

    // *** create legs of dude_medfar ***
    arena = pfGetSharedArena();
    gset = pfNewGSet(arena);
   
    // set the coordinate, normal and color arrays
    // and their cooresponding index arrays
    pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
               dude->verts_leg, dude->index_medB);
    pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM,
               dude->norms_med, dude->normvert_med);
    pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX,
               dude->texs_med, dude->texsvert_med);
    pfGSetPrimType(gset, PFGS_QUADS);
    pfGSetNumPrims(gset, 5);

    dude->legleftmed  = pfNewGroup();
    dude->legrightmed = pfNewGroup();
    geode  = pfNewGeode();
    pfAddGSet(geode, gset);
    pfAddChild(dude->legleftmed, geode);
    pfAddChild(dude->legrightmed, geode);

    infile.close();
      return(1);
}

int read_far_dude(char* file, DUDE_GEOM *dude)
{
  // Performer variables to make dude
  pfGeode    *geode;
  pfGeoSet   *gset;
  pfGeoState *gst;
  pfTexEnv   *tenv;
  void       *arena;
  char       tag;
  char       comment[200];
  int   items;
  float data;

  // set file to appropriate dude model 
  ifstream infile (file);

  //read the file
  while (infile >> tag)
  {
    //what kind of line is it?
    switch (tag) 
    {
        case '#': // a comment line
                  infile.getline(comment,200);
                  break;

        case ' ': // empty line
                  break;

        case 'D': // D vertices 
                  infile >> items;
                  for (int d=0; d< items; d++) 
                  {  
                    for (int j=0; j< 3; j++)
                    { 
                      infile >> data;
                      data = (float) data;
                      dude->verts_far[d][j] =  data;
                     } 
                  }
                  break;

        case 'E': // E index arrangements 
                  infile >> items;
                  for (int e=0; e< items*4; e++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->index_far[e] =  data;
                  }
                  break;

        case 'F': // F correlate normals 
                  infile >> items;
                  for (int f=0; f< items; f++) 
                  {  
                    infile >> data;
                    data = (unsigned short) data;
                    dude->normvert_far[f] =  data;
                  }
                  break;
    }
  }
 
  arena = pfGetSharedArena();
  gset  = pfNewGSet(arena);
   

  // set the coordinate, normal and color arrays
  // and their cooresponding index arrays
  pfGSetAttr(gset, PFGS_COORD3, PFGS_PER_VERTEX, 
             dude->verts_far, dude->index_far);
  pfGSetAttr(gset, PFGS_NORMAL3, PFGS_PER_PRIM, 
             dude->norms_med, dude->normvert_far);
  pfGSetAttr(gset, PFGS_TEXCOORD2, PFGS_PER_VERTEX, 
             dude->texs_med, dude->texsvert_med);
  pfGSetPrimType(gset, PFGS_QUADS);
  pfGSetNumPrims(gset, 5);
    
    
  // create a geostate from shared memory, enable 
  // texturing (in case that's not the default), and
  // set the geostate for this geoset 
  gst = pfNewGState(arena);
  pfGStateMode(gst, PFSTATE_ENTEXTURE, 1);
  pfGSetGState(gset, gst);
     
  // create a new texture from shared memory,
  // load a texture file and add texture to geostate 
  pfGStateAttr(gst, PFSTATE_TEXTURE, texuniform);     
    
  // create a new texture environment from shared memory,
  // decal the texture since the geoset has no color to 
  // modulate, set the texture environment for this geostate
  tenv = pfNewTEnv(arena);
  pfTEnvMode(tenv, PFTE_DECAL);
  pfGStateAttr(gst, PFSTATE_TEXENV, tenv);

  dude->far_body = pfNewGroup();
  geode          = pfNewGeode();
  pfAddGSet(geode, gset);
  pfAddChild(dude->far_body, geode);
  pfDCSRot(dude->far_bodyDCS, 0.0f, 90.0f, 0.0f);  
  pfDCSTrans(dude->far_bodyDCS, 0.18f, -0.1f, -1.00f);
     
  infile.close();
  return(1);
}
