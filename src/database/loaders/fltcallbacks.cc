// File: <fltcallbacks.cc>

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

#include <string.h>
#include <iostream.h>

#include "fltcallbacks.h"
#include "common_defs2.h"
#include "fileio.h"

// Globals used by this library
// Globals used by this library
extern volatile DYNAMIC_DATA    *G_dynamic_data;
extern VEHTYPE_LIST *G_vehtype;
extern int           NU_mod_num;

// Shared global for determining phase of model loading
volatile loading_phase FLT_load_phase;

// Local routine prototype
static void dynamic_flt_model ( pfNode *, int, int *, COMMENTcb * );
static void init_articulation ( JNT_INFO *, pfDCS *, const char * );
static void parse_for_articulation ( COMMENTcb *, JNT_INFO * );
static int advance_ptr_past_nl ( char **text );



void init_flt_loader ()
{
   FLT_load_phase = DEFAULT_LOAD_PHASE;

   /* Initialize MultiGen Flight Loader Modes */
//   LoadFltMode ( PFFLT_FLATTEN, FALSE );
//   LoadFltMode ( PFFLT_CLEAN, FALSE );
   pfdConverterMode_flt ( PFFLT_COMBINELODS, FALSE );
//   LoadFltMode ( PFFLT_SEQUENCES, FALSE );
//   LoadFltMode ( PFFLT_AMBIENT_IS_DIFFUSE, FALSE );
//   LoadFltMode ( PFFLT_SHOWTSTRIPS, TRUE );
//   LoadFltMode ( PFFLT_COMPUTENORMALS, FALSE );
//   LoadFltMode ( PFFLT_MONOCHROME, TRUE );
//   LoadFltMode ( PFFLT_LAYER, PFDECAL_BASE_HIGH_QUALITY );
//     pfdConverterMode_flt ( PFFLT_LAYER, 
//                           PFDECAL_LAYER_FAST);
//                           PFDECAL_BASE_STENCIL);
//                           PFDECAL_BASE_DISPLACE | PFDECAL_LAYER_OFFSET);
//   LoadFltMode ( PFFLT_USELONGIDS, TRUE );
//   LoadFltMode ( PFFLT_USEUNITS, FLT_KILOMETERS );
//   LoadFltMode ( PFFLT_INHERIT_GSTATE, TRUE );


   if ( pfdGetConverterMode_flt(PFFLT_FLATTEN) )
      pfNotify ( PFNFY_INFO, PFNFY_PRINT, " Flight Loader: Flatten ON.\n" );
   else
      pfNotify ( PFNFY_INFO, PFNFY_PRINT, " Flight Loader: Flatten OFF.\n" );

   if ( pfdGetConverterMode_flt(PFFLT_CLEAN) )
      pfNotify ( PFNFY_INFO, PFNFY_PRINT, " Flight Loader: Clean ON.\n" );
   else
      pfNotify ( PFNFY_INFO, PFNFY_PRINT, " Flight Loader: Clean OFF.\n" );

   if ( pfdGetConverterMode_flt(PFFLT_COMBINELODS) )
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Combine LODS ON.\n" );
   else
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Combine LODS OFF.\n" );

   if ( pfdGetConverterMode_flt(PFFLT_SEQUENCES) )
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Sequences ON.\n" );
   else
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Sequences OFF.\n" );

   if ( pfdGetConverterMode_flt(PFFLT_AMBIENT_IS_DIFFUSE) )
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Ambient is diffuse ON.\n" );
   else
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Ambient is diffuse OFF.\n" );

   if ( pfdGetConverterMode_flt(PFFLT_SHOWTSTRIPS) )
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Show Tstrips ON.\n" );
   else
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Show Tstrips OFF.\n" );

   if ( pfdGetConverterMode_flt(PFFLT_COMPUTENORMALS) )
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Compute Normals ON.\n" );
   else
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Compute Normals OFF.\n" );

   if ( pfdGetConverterMode_flt(PFFLT_MONOCHROME) )
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Monochrome ON.\n" );
   else
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Monochrome OFF.\n" );

   if ( pfdGetConverterMode_flt(PFFLT_USELONGIDS) )
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Use long IDs ON.\n" );
   else
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Use long IDs OFF.\n" );

   if ( pfdGetConverterMode_flt(PFFLT_INHERIT_GSTATE) )
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Inherit GState ON.\n" );
   else
      pfNotify ( PFNFY_INFO, PFNFY_PRINT,
                 " Flight Loader: Inherit GState OFF.\n" );


}



void myregisterfunc ( pfNode *node, int mgOp, int *cbs, COMMENTcb *cbcom, void *)
{
#if PF_MAJOR_VERSION == 1 && PF_MINOR_VERSION == 2
   // See pfflt.h for why this is necessary - bug in Performer 1.2
   static int bug_counter = 0;
   if ( bug_counter >= 25 ) {
      bug_counter = 0;
      prevent_PFHASHTABLE_crash();
   }
#endif
      
   if ( FLT_load_phase == DYNAMIC_MODELS ) {
      dynamic_flt_model ( node, mgOp, cbs, cbcom );
   }

   if ( cbs ) {
      pfFree ( cbs );
   }
   if ( cbcom ) {
      pfFree ( cbs );
   }

}
       
 
void dynamic_flt_model ( pfNode *node, int mgOp, int *cbs, COMMENTcb *cbcom )
{
   int *tempint;
   static int previous_mgOp = CB_HEADER;
   static int num_joints = -1;
   tempint = cbs;

   switch ( mgOp ) {
      case CB_HEADER:
         num_joints = -1;
         G_vehtype[NU_mod_num].num_joints = 0;
         break;
      case CB_GROUP:
         if ( previous_mgOp == CB_DOF ) {
            parse_for_articulation(cbcom,
                                   &(G_vehtype[NU_mod_num].joints[num_joints]));
         }
         break;
      case CB_DOF:
         if (pfGetDCSClassType() == pfGetType((pfObject *)node)) {
            num_joints++;
            if ( (num_joints >= MAXJOINTS ) ) {
               cerr << "Too many articulation joints in model:  "
                    << G_vehtype[NU_mod_num].name << endl;
               cerr << "   Only " << MAXJOINTS << " unique joints allowed."
                    << endl;
               num_joints--;
            }
            else {
               G_vehtype[NU_mod_num].num_joints++;
               init_articulation ( &(G_vehtype[NU_mod_num].joints[num_joints]),
                                   (pfDCS *)node, pfGetNodeName(node) );
            }
         }
         else {
            cerr << "\tDOF is not a DCS, but is of type = " << pfGetType(node)
                 << endl;
         }
         break;
      case CB_LOD:
      case CB_OBJECT:
      case CB_PUSH:
      case CB_POP:
      case CB_EYEPOINTS:
      case CB_SOUND:
      case CB_PATH:
      case CB_SWITCH:
      case CB_LIGHTSRC:
      case CB_CLEANNODE:
      case CB_IMPOSSIBLE:
      default:
         break;
   }

   if ( (mgOp != CB_PUSH) && (mgOp != CB_POP) ) {
      previous_mgOp = mgOp;
   }

}
   

void init_articulation ( JNT_INFO *joint, pfDCS *dcs, const char *name )
{
   // Assume no constraints until proven from comment field of next group node.
   // Used 360 as constraints for all values, even X,Y,Z for lack of
   //    a better constant.
   joint->min_range.xyz[PF_X] = -360.0f;
   joint->min_range.xyz[PF_Y] = -360.0f;
   joint->min_range.xyz[PF_Z] = -360.0f;
   joint->min_range.hpr[PF_H] = -360.0f;
   joint->min_range.hpr[PF_P] = -360.0f;
   joint->min_range.hpr[PF_R] = -360.0f;
   joint->max_range.xyz[PF_X] = +360.0f;
   joint->max_range.xyz[PF_Y] = +360.0f;
   joint->max_range.xyz[PF_Z] = +360.0f;
   joint->max_range.hpr[PF_H] = +360.0f;
   joint->max_range.hpr[PF_P] = +360.0f;
   joint->max_range.hpr[PF_R] = +360.0f;
   joint->current_setting.xyz[PF_X] = 0.0f;
   joint->current_setting.xyz[PF_Y] = 0.0f;
   joint->current_setting.xyz[PF_Z] = 0.0f;
   joint->current_setting.hpr[PF_H] = 0.0f;
   joint->current_setting.hpr[PF_P] = 0.0f;
   joint->current_setting.hpr[PF_R] = 0.0f;
   joint->articulate[0] = TRUE;
   joint->articulate[1] = TRUE;
   joint->articulate[2] = TRUE;
   joint->articulate[3] = TRUE;
   joint->articulate[4] = TRUE;
   joint->articulate[5] = TRUE;
   joint->dcs = dcs;
   strcpy ( joint->name, name );

}


int advance_ptr_past_nl ( char **text )
{
   char *current = *text;
   int past_newline = FALSE;

   // Advance the paramter pointer 'text' past the first newline
   // character in the string.
   if ( current != NULL ) {
      while ( (*current != '\0') && (!past_newline) ) {
         if (*current == '\n') {
            past_newline = TRUE;
         }
         current++;
      }
   }
   *text = current; 
   return ( *current != '\0' );
}


void parse_for_articulation ( COMMENTcb *cbcom, JNT_INFO *joint )
{
   int first_constraint = TRUE;
   char *temp_ptr;
   int temp_int;
   int done = FALSE;
   int id_found = FALSE;
   char kind[30];
   float min,max;

   if ( (cbcom != NULL) && (cbcom->length > 0) ) {
      remove_excessive_spaces ( cbcom->text );
      upcase ( cbcom->text );
      temp_ptr = cbcom->text;
      while (!done) {
         if ( strncmp("@DIS ARTICULATED_PART",temp_ptr,21) == 0 ) {
            temp_ptr = cbcom->text + 21;
            if ( ((sscanf(temp_ptr,"%d",&temp_int)) == 1) &&
                 (temp_int >= 0) ) {
               joint->dis_id = (unsigned short)temp_int;
               id_found = TRUE;
            }
            else {
               cerr << "Invalid WRM-EFS articulated part format." << endl;
            }
         }
         else if ( strncmp("@DIS MOTION",temp_ptr,11) == 0 ) {
            if ( id_found ) {
               temp_ptr = temp_ptr + 11;
               if ( (sscanf(temp_ptr,"%s%f%f",&kind,&min,&max)) == 3 ) {
                  if ( strcmp(kind,"X") == 0 ) {
                     joint->min_range.xyz[PF_X] = min;
                     joint->max_range.xyz[PF_X] = max;
                     joint->articulate[0] = TRUE;
                     if ( first_constraint ) {
                        first_constraint = FALSE;
                        joint->articulate[1] = FALSE;
                        joint->articulate[2] = FALSE;
                        joint->articulate[3] = FALSE;
                        joint->articulate[4] = FALSE;
                        joint->articulate[5] = FALSE;
                     }
                  }
                  else if ( strcmp(kind,"Y") == 0 ) {
                     joint->min_range.xyz[PF_Y] = min;
                     joint->max_range.xyz[PF_Y] = max;
                     joint->articulate[1] = TRUE;
                     if ( first_constraint ) {
                        first_constraint = FALSE;
                        joint->articulate[0] = FALSE;
                        joint->articulate[2] = FALSE;
                        joint->articulate[3] = FALSE;
                        joint->articulate[4] = FALSE;
                        joint->articulate[5] = FALSE;
                     }
                  }
                  else if ( strcmp(kind,"Z") == 0 ) {
                     joint->min_range.xyz[PF_Z] = min;
                     joint->max_range.xyz[PF_Z] = max;
                     joint->articulate[2] = TRUE;
                     if ( first_constraint ) {
                        first_constraint = FALSE;
                        joint->articulate[0] = FALSE;
                        joint->articulate[1] = FALSE;
                        joint->articulate[3] = FALSE;
                        joint->articulate[4] = FALSE;
                        joint->articulate[5] = FALSE;
                     }
                  }
                  else if ( strcmp(kind,"AZIMUTH") == 0 ) {
                     joint->min_range.hpr[PF_H] = min;
                     joint->max_range.hpr[PF_H] = max;
                     joint->articulate[3] = TRUE;
                     if ( first_constraint ) {
                        first_constraint = FALSE;
                        joint->articulate[0] = FALSE;
                        joint->articulate[1] = FALSE;
                        joint->articulate[2] = FALSE;
                        joint->articulate[4] = FALSE;
                        joint->articulate[5] = FALSE;
                     }
                  }
                  else if ( strcmp(kind,"ELEVATION") == 0 ) {
                     joint->min_range.hpr[PF_P] = min;
                     joint->max_range.hpr[PF_P] = max;
                     joint->articulate[4] = TRUE;
                     if ( first_constraint ) {
                        first_constraint = FALSE;
                        joint->articulate[0] = FALSE;
                        joint->articulate[1] = FALSE;
                        joint->articulate[2] = FALSE;
                        joint->articulate[3] = FALSE;
                        joint->articulate[5] = FALSE;
                     }
                  }
                  else if ( strcmp(kind,"ROTATION") == 0 ) {
                     joint->min_range.hpr[PF_R] = min;
                     joint->max_range.hpr[PF_R] = max;
                     joint->articulate[5] = TRUE;
                     if ( first_constraint ) {
                        first_constraint = FALSE;
                        joint->articulate[0] = FALSE;
                        joint->articulate[1] = FALSE;
                        joint->articulate[2] = FALSE;
                        joint->articulate[3] = FALSE;
                        joint->articulate[4] = FALSE;
                     }
                  }
                  else {
                     cerr << "WRM-EFS error:" << endl;
                     cerr << "\tUnknown articulation constraint kind: "
                          << kind << endl;
                  }
               }
               else {
                  cerr << "Invalid WRM-EFS articulated motion format." << endl;
               }
            }
            else {
               cerr << "WRM-EFS violation:" << endl;
               cerr << "\t'@dis motion ...' command must be proceeded by a"
                    << endl;
               cerr << "\t'@dis articulated_part ...' command." << endl;
            }
         }
  
      done = !(advance_ptr_past_nl(&temp_ptr));

      }
   }

}


void get_flt_texture_list ()
{
   fltSharedPalette *fltpalette;
   pfList *texture_list;
   long texture_count;
   fltSharedObj *shared_texture;

   fltpalette = fltGetCurSharedPalette();
   texture_list = fltpalette->texList;
   texture_count = pfGetNum(texture_list);
   for ( int i = 0; i < texture_count; i++ ) {
      shared_texture = (fltSharedObj *)pfGet(texture_list,i);
      G_dynamic_data->texture_list_ptr[i] = (pfTexture *)shared_texture->object;
   }

   G_dynamic_data->number_of_textures = texture_count;
}

