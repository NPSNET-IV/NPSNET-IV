// *********************************************************************
// File    : RifleClass.cc
// Author  : Scott McMillan
//         : Naval Postgraduate School
//         : Code CSMs
//         : Monterey, CA 93943
//         : mcmillan@cs.nps.navy.mil
// Project : NPSNET - Individual Combatants
// Created : August 1995
// Summary : This file contains the declarations for a C++ class to
//         : hold arm parameters that apply to both arms, and some MDH
//         : convenience functions.
// Modified: 
// *********************************************************************

#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fstream.h>         // For C++ file I/O stuff

#include "Performer/pf.h"

#include "jointmods.h"
#include "RifleClass.h"

#define MAX_CONFIGFILE_LINESIZE        255
#define CONFIGFILE_COMMENT_CHAR        '#'

//======================================================================
// RifleClass class
//======================================================================

//----------------------------------------------------------------------
//   Function: RifleClass::RifleClass
//    Summary: constructor for RifleClass object
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
RifleClass::RifleClass(ifstream &config_fileobj)
{
   calibrated_flag = FALSE;

   int i,j;
   int done = FALSE;
   char tmp_str[MAX_CONFIGFILE_LINESIZE];
   char param_str[30];

   pfMakeIdentMat(H_rs_to_r);

   while ((!done) && (config_fileobj >> tmp_str)) {
      // When a comment char is read, skip the rest of the line.
      if (tmp_str[0] == CONFIGFILE_COMMENT_CHAR) {
         config_fileobj.getline(tmp_str, MAX_CONFIGFILE_LINESIZE);
      }
      else if (strncmp(tmp_str, "RIFLE", 5) == 0) {

         config_fileobj >> param_str;
         for (i=0; i<4; i++) {
            for (j=0; j<4; j++) {
               config_fileobj >> H_rs_to_r[i][j];
            }
         }
         
         config_fileobj >> param_str;
         for (i=0; i<3; i++) {
            config_fileobj >> p_r_to_ratt[i];
          }

         config_fileobj >> param_str;
         for (i=0; i<3; i++) {
            config_fileobj >> p_r_to_latt[i];
        }

         done = TRUE;
      }
      else {
         cerr << (char) 7 << "Error in reading config file.\n"
              << "  in *RifleClass.cc*RifleClass::RifleClass;"
              << " illegal string: " << tmp_str << endl;
      }
   }

   // Compute the attachment points with respect to the rifle's sensor CS
   for (i=0; i<3; i++) {
      p_rs_to_ratt[i] = H_rs_to_r[i][3];
      p_rs_to_latt[i] = H_rs_to_r[i][3];
      for (j=0; j<3; j++) {
         p_rs_to_ratt[i] += H_rs_to_r[i][j]*p_r_to_ratt[j];
         p_rs_to_latt[i] += H_rs_to_r[i][j]*p_r_to_latt[j];
      }
   }

   // Added for targeting - is the transformation from the rifle models
   // coordinate system to NPSNet's version of it.  Used for the
   // targeting.
   pfMatrix H_r_to_nps = {{ 0, 1, 0, 0.0},
                          {-1, 0, 0, 0.0},
                          { 0, 0, 1, 0.0},
                          { 0, 0, 0, 1   }};
   pfMultMat(H_rs_to_npsrifle, H_rs_to_r, H_r_to_nps);
}


//----------------------------------------------------------------------
//   Function: RifleClass::~RifleClass
//    Summary: destructor for RifleClass object
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
RifleClass::~RifleClass()
{

}


//----------------------------------------------------------------------
//   Function: calibrate
//    Summary: latest and greatest 9/30/95
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
int RifleClass::calibrate(pfMatrix H_tx_to_rs,
                          pfMatrix H_ts_to_uw)
{
   // I. compute rifle sensor to model transformation - constant for now see
   // the constructor (read in from config file).
   (H_tx_to_rs);

   // II. Compute ts wrt uw calibration matrix
   pfInvertFullMat(H_uw_to_ts, H_ts_to_uw);

   // Estimate the approximate position of the torso sensor wrt uw CS
   H_uw_to_ts[0][3] = -0.04;
   H_uw_to_ts[1][3] = -0.015;
   H_uw_to_ts[2][3] =  0.0;

#ifdef DEBUG
   printf("H_uw_to_ts (rifle):\n");
   for (int i=0; i<4; i++)
      printf(" %6.3f %6.3f %6.3f %6.3f\n",
             H_uw_to_ts[i][0], H_uw_to_ts[i][1],
             H_uw_to_ts[i][2], H_uw_to_ts[i][3]);
   printf("\n");
#endif

   return (calibrated_flag = TRUE);
}


//----------------------------------------------------------------------
//   Function: computeCoord
//    Summary: latest and greatest 9/30/95
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void RifleClass::computeCoord(pfMatrix H_tx_to_rs,  pfMatrix H_tx_to_ts,
                              pfMatrix H_tx_to_rws, pfMatrix H_tx_to_lws,
                              pfMatrix H_uw_to_rw,  pfMatrix H_uw_to_lw,
                              pfCoord *coord)
{
   int i, j;

   pfVec3 p_tx_to_lws, p_tx_to_rws, p_tx_to_rs;
   for (i=0; i<3; i++) {
      p_tx_to_rws[i] = H_tx_to_rws[i][3];
      p_tx_to_lws[i] = H_tx_to_lws[i][3];
      p_tx_to_rs[i]  = H_tx_to_rs[i][3];
   }

   // compute rifle CS wrt torso CS
   pfMatrix H_ts_to_tx;
   pfInvertFullMat(H_ts_to_tx, H_tx_to_ts);

   pfMatrix H_uw_to_r;
   pfMultMat(H_uw_to_r, H_uw_to_ts, H_ts_to_tx);  // ^uw H_tx
   pfPostMultMat(H_uw_to_r, H_tx_to_rs);          // ^uw H_rs
   pfPostMultMat(H_uw_to_r, H_rs_to_r);           // ^uw H_r

   // ===================================================
   // Implement rifle snap-to to Jack model here:

   // compute proximity to arm attachment positions
   pfVec3 p2r, p2l;

   for (i=0; i<3; i++) {
      p2r[i] = H_tx_to_rws[i][3];
      p2l[i] = H_tx_to_lws[i][3];
      for (j=0; j<3; j++) {
         p2r[i] -= H_tx_to_rs[i][j]*p_rs_to_ratt[j];
         p2l[i] -= H_tx_to_rs[i][j]*p_rs_to_latt[j];
      }
   }

   // compute mode: no, one, or both arm snap to.
   float left_dist, right_dist;
   int near_flag_r = FALSE, near_flag_l = FALSE;

   if ((right_dist = pfDistancePt3(p_tx_to_rs, p2r)) < 0.25) {
      //cerr << "r " << right_dist << endl;
      near_flag_r = TRUE;
   }
   if ((left_dist  = pfDistancePt3(p_tx_to_rs, p2l)) < 0.25) {
      //cerr << "l " << left_dist << endl;
      near_flag_l = TRUE;
   }

   // perform snap-to
   if (near_flag_r && near_flag_l) {
      // change the rotation matrix to lie along BOTH hands
      pfVec3 x_hat, y_hat, z, z_hat;
      for (i=0; i<3; i++) {
         x_hat[i] = H_uw_to_lw[i][3] - H_uw_to_rw[i][3];
         z[i] = H_uw_to_r[i][2];
      }

      pfNormalizeVec3(x_hat);
      pfCrossVec3(y_hat, z, x_hat);
      pfNormalizeVec3(y_hat);
      pfCrossVec3(z_hat, x_hat, y_hat);
      pfNormalizeVec3(z_hat);

      for (i=0; i<3; i++) {
         H_uw_to_r[i][0] = x_hat[i];
         H_uw_to_r[i][1] = y_hat[i];
         H_uw_to_r[i][2] = z_hat[i];
      }

      // now adjust the position
      for (i=0; i<3; i++) {
         H_uw_to_r[i][3] = H_uw_to_rw[i][3];
         for (j=0; j<3; j++) {
            H_uw_to_r[i][3] -= H_uw_to_r[i][j]*p_r_to_ratt[j];
         }
      }
   }
   else if (near_flag_r) {
      for (i=0; i<3; i++) {
         H_uw_to_r[i][3] = H_uw_to_rw[i][3];
         for (j=0; j<3; j++) {
            H_uw_to_r[i][3] -= H_uw_to_r[i][j]*p_r_to_ratt[j];
         }
      }
   }
   else if (near_flag_l) {
      for (i=0; i<3; i++) {
         H_uw_to_r[i][3] = H_uw_to_lw[i][3];
         for (j=0; j<3; j++) {
            H_uw_to_r[i][3] -= H_uw_to_r[i][j]*p_r_to_latt[j];
         }
      }
   }

   // IV. Compute rifle coord struct to pass back
   pfMatrix H_temp;
   pfTransposeMat(H_temp, H_uw_to_r);
   pfGetOrthoMatCoord(H_temp, coord);
}


//----------------------------------------------------------------------
//   Function: computeTargetCoord
//    Summary: compute pfCoord for orientation (ONLY) of rifle in NPSNET
//           : world - for targeting. 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void RifleClass::computeTargetCoord(pfMatrix H_tx_to_rs,
                                    pfMatrix H_tx_to_ts,
                                    pfCoord *coord_hull_to_npsrifle)
{
//   int i;

   pfMatrix H_hull_to_npsrifle;
   pfMultMat(H_hull_to_npsrifle, H_tx_to_rs, H_rs_to_npsrifle);

   // zero out offsets from transmitter and model.
/*
   for (i=0; i<3; i++) {
      H_hull_to_npsrifle[i][3] = 0.0;
   }
   // view offset (hardcoded for now)
   H_hull_to_npsrifle[2][3] = 1.5;
   cerr << "RIFLE X = " << H_hull_to_npsrifle[0][3]
        << ", Y = " << H_hull_to_npsrifle[1][3]
        << ", Z = " << H_hull_to_npsrifle[2][3] << endl;
*/
   H_hull_to_npsrifle[0][3] = H_tx_to_rs[0][3] - H_tx_to_ts[0][3];
   H_hull_to_npsrifle[1][3] = H_tx_to_rs[1][3] - H_tx_to_ts[1][3];
   H_hull_to_npsrifle[2][3] = H_tx_to_rs[2][3] - H_tx_to_ts[2][3];

   H_hull_to_npsrifle[2][3] += 1.33f;


   // IV. Compute rifle coord struct to pass back
   pfMatrix H_temp;
   pfTransposeMat(H_temp, H_hull_to_npsrifle);

   pfGetOrthoMatCoord(H_temp, coord_hull_to_npsrifle);
}
