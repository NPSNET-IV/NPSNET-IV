// *********************************************************************
// File    : UpperBodyClass.cc
// Author  : Scott McMillan
//         : Naval Postgraduate School
//         : Code CSMs
//         : Monterey, CA 93943
//         : mcmillan@cs.nps.navy.mil
// Project : NPSNET - Individual Combatants
// Created : 11 August 1995
// Summary : UpperBodyClass declarations
// Modified: 
// *********************************************************************

#include <string.h>
#include <ctype.h>
#include <bstring.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>         // For C++ file I/O stuff
#include <stdio.h>

#include <Performer/pf.h>

#include "FastrakClass.h"              // FASTRAK include
#include "UpperBodyClass.h"
#include "ArmClass.h"
#include "RightArmClass.h"
#include "LeftArmClass.h"
#include "RifleClass.h"

#include "jointmods.h"
#include "pdu.h"


//----------------------------------------------------------------------
//   Function: 
//    Returns: 
// Parameters: 
//    Summary: 
//----------------------------------------------------------------------
UpperBodyClass::UpperBodyClass(const char *config_filename)
{
   valid = FALSE;

   right_arm = NULL;
   left_arm  = NULL;
   rifle     = NULL;
   fastrak_unit = NULL;

   // open configuration file
   ifstream config_fileobj(config_filename);
   if (!config_fileobj) {
      cerr << "Error: opening configuration file: "
           << config_filename << endl;
      return;
   }

   // initialize matrices
   pfMakeIdentMat(H_tx_to_ts);
   pfMakeIdentMat(H_tx_to_rws);
   pfMakeIdentMat(H_tx_to_lws);
   pfMakeIdentMat(H_tx_to_rs);
   pfMakeIdentMat(H_uw_to_rw);
   pfMakeIdentMat(H_uw_to_lw);
   pfMakeIdentMat(H_hull_to_uw);

   // set up object hierarchy
   right_arm = new RightArmClass();
   left_arm  = new  LeftArmClass();
   rifle     = new    RifleClass(config_fileobj);

   float joint_angles[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
   right_arm->setJointAngles(joint_angles);
   left_arm->setJointAngles(joint_angles);

   // initialize Fastrak
   fastrak_unit = new FastrakClass(config_fileobj);

   if (fastrak_unit->exists()) {
      if (fastrak_unit->getState(FSTK_STATION1))
         torso_sensor = FSTK_STATION1;
      if (fastrak_unit->getState(FSTK_STATION2))
         left_sensor  = FSTK_STATION2;
      if (fastrak_unit->getState(FSTK_STATION3))
         right_sensor = FSTK_STATION3;
      if (fastrak_unit->getState(FSTK_STATION4))
         rifle_sensor = FSTK_STATION4;

      valid = TRUE;
   }
}

//----------------------------------------------------------------------
//   Function: 
//    Summary: 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
UpperBodyClass::~UpperBodyClass()
{
   //closeFastrak();
   if ((fastrak_unit != NULL) && (fastrak_unit->exists())) {
      delete fastrak_unit;
      fastrak_unit = NULL;
   }
}


//----------------------------------------------------------------------
//   Function: output
//    Summary: output a homogeneous transformation matrix (4x4)
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void UpperBodyClass::outputHMatrix(pfMatrix Hmat)
{
   for (int i=0; i<4; i++)
      printf(" %6.3f %6.3f %6.3f %6.3f\n",
             Hmat[i][0], Hmat[i][1], Hmat[i][2], Hmat[i][3]);
   printf("\n");
}


//----------------------------------------------------------------------
//   Function: get_all_inputs
//    Summary: 
// Parameters: 
//    Returns: must be called to copy the latest sample into the second
//             buffer.  Implemented for double buffering of data to
//             reduce lock overhead.  In our Performer app this is called
//             once at the beginning of each frame.
//----------------------------------------------------------------------
void UpperBodyClass::get_all_inputs(pfChannel *)
{
   if (fastrak_unit->exists()) {
      fastrak_unit->copyBuffer();

      fastrak_unit->getHMatrix(torso_sensor, H_tx_to_ts);
      fastrak_unit->getHMatrix(right_sensor, H_tx_to_rws);
      fastrak_unit->getHMatrix(left_sensor,  H_tx_to_lws);
      fastrak_unit->getHMatrix(rifle_sensor, H_tx_to_rs);

      for (int i=0; i<3; i++) {       // change to meters
         H_tx_to_ts[i][3]  /= 100.0;
         H_tx_to_rs[i][3]  /= 100.0;
         H_tx_to_lws[i][3] /= 100.0;
         H_tx_to_rws[i][3] /= 100.0;
      }
   }
}


//----------------------------------------------------------------------
//   Function: calibrate
//    Returns: 
// Parameters: 
//    Summary: 
//----------------------------------------------------------------------
int UpperBodyClass::calibrate(const NPS_VALUATOR,
                              const NPS_CALIBRATION prompt,
                              const float)
{
   int valid = FALSE;
   int i;

   // Calibration positions:
   // 1 - arms straight to side palms down
   // 2 - (NOT USED) elbows out to side/bent 90 degrees and hands forward
   // 3 - arms straight out front shoulder width apart.

   if (fastrak_unit->exists())
   {
      //const char *calibrate_cmd[2] = {"/usr/sbin/sfplay calibrate1.aifc",
      //                                "/usr/sbin/sfplay calibrate2.aifc"};
      pfMatrix H_side_l, H_front_l, H_side_r, H_front_r;
      pfMatrix H_torso_side, H_torso_front, H_rifle;

// Calibration position #1:
      cerr << "Hold arms straight out to the side, palms down.\n";
      cerr << (char) 7;
      //system(calibrate_cmd[0]);

      if (prompt == NPS_UB_DELAY) {
         for (i=0; i<3; i++) {
            sleep(1);
            cerr << (char) 7;
         }
      }
      else if ((prompt == NPS_CAL) || (prompt == NPS_UB_PROMPT)) {
         char str;
         cerr << "Press <Enter> when in position: ";
         cin.get(str);
      }
      else {
         cerr << "Error: unknown NPS_CALIBRATION in UpperBodyClass."
              << endl;
         char str;
         cerr << "Press <ENTER> when in position: ";
         cin.get(str);
      }

      fastrak_unit->copyBuffer();
      fastrak_unit->getHMatrix(torso_sensor, H_torso_side);
      fastrak_unit->getHMatrix(right_sensor, H_side_r);
      fastrak_unit->getHMatrix(left_sensor,  H_side_l);

      //system("/usr/sbin/sfplay calibrate_okay.aifc");

      for (i=0; i<3; i++) {          // change to meters
         H_torso_side[i][3] /= 100.0;
         H_side_r[i][3] /= 100.0;
         H_side_l[i][3] /= 100.0;
      }

#ifdef DEBUG
      printf("Torso sensor side:\n");
      outputHMatrix(H_torso_side);
      printf("Right arm side:\n");
      outputHMatrix(H_side_r);
      printf("Left arm side:\n");
      outputHMatrix(H_side_l);
#endif

// Calibration position #3:
      cerr << "Hold arms straight out front, palms down, "
           << "shoulder width apart.\n";
      cerr << (char) 7;
      //system(calibrate_cmd[1]);

      if (prompt == NPS_UB_DELAY) {
         for (i=0; i<3; i++) {
            sleep(1);
            cerr << (char) 7;
         }
      }
      else if ((prompt == NPS_CAL) || (prompt == NPS_UB_PROMPT)) {
         char str;
         cerr << "Press <ENTER> when in position: ";
         cin.get(str);
      }
      else {
         cerr << "Error: unknown NPS_CALIBRATION in UpperBodyClass."
              << endl;
         char str;
         cerr << "Press <ENTER> when in position: ";
         cin.get(str);
      }

      fastrak_unit->copyBuffer();
      fastrak_unit->getHMatrix(torso_sensor, H_torso_front);
      fastrak_unit->getHMatrix(rifle_sensor, H_rifle);
      fastrak_unit->getHMatrix(right_sensor, H_front_r);
      fastrak_unit->getHMatrix(left_sensor,  H_front_l);

      //system("/usr/sbin/sfplay calibrate_okay.aifc");

      for (i=0; i<3; i++) {          // change to meters
         H_torso_front[i][3] /= 100.0;
         H_rifle[i][3] /= 100.0;
         H_front_r[i][3] /= 100.0;
         H_front_l[i][3] /= 100.0;
      }

#ifdef DEBUG
      printf("Torso sensor front:\n");
      outputHMatrix(H_torso_front);
      printf("Rifle sensor:\n");
      outputHMatrix(H_rifle);
      printf("Right arm front:\n");
      outputHMatrix(H_front_r);
      printf("Left arm front:\n");
      outputHMatrix(H_front_l);
#endif

      // compute transmitter to neck transformation
      pfMatrix H_tx_to_neck;
      pfMakeIdentMat(H_tx_to_neck);

      pfVec3 y_hat, z_hat;
      pfVec3 x_hat = {0.0, 0.0, -1.0};
      for (int i=0; i<3; i++) {
         z_hat[i] = (H_side_r[i][3] - H_side_l[i][3]);
      }
      pfNormalizeVec3(z_hat);
      pfCrossVec3(y_hat, z_hat, x_hat);
      pfNormalizeVec3(y_hat);

      // Assume x_hat was correct and finish by readjusting z_hat to
      // orthogonalize the matrix
      pfCrossVec3(z_hat, x_hat, y_hat);
      pfNormalizeVec3(z_hat);
      for (i=0; i<3; i++) {
         H_tx_to_neck[i][0] = x_hat[i];
         H_tx_to_neck[i][1] = y_hat[i];
         H_tx_to_neck[i][2] = z_hat[i];
         H_tx_to_neck[i][3] = 0.5*(H_side_r[i][3] + H_side_l[i][3]);
      }

      // compute transformations from torso sensor to both shoulders,
      pfMatrix H_tx_to_rsh, H_tx_to_lsh;
      pfCopyMat(H_tx_to_rsh, H_tx_to_neck);
      pfCopyMat(H_tx_to_lsh, H_tx_to_neck);

      for (i=0; i<3; i++) {
         H_tx_to_rsh[i][3] += 0.5*(H_front_r[i][3] - H_front_l[i][3]);
         H_tx_to_lsh[i][3] -= 0.5*(H_front_r[i][3] - H_front_l[i][3]);
      }

      // compute torso sensor to shoulder transformations, H_ts_to_xsh
      pfMatrix H_ts_to_tx;
      pfInvertFullMat(H_ts_to_tx, H_torso_side);
      pfMatrix H_ts_to_rsh, H_ts_to_lsh;
      pfMultMat(H_ts_to_rsh, H_ts_to_tx, H_tx_to_rsh);
      pfMultMat(H_ts_to_lsh, H_ts_to_tx, H_tx_to_lsh);

      valid  =  left_arm->calibrate(H_ts_to_lsh);
      valid &= right_arm->calibrate(H_ts_to_rsh);

      // compute matrix from torso sensor to upwaist coordinate system
      // note that it is different from the shoulder coord sys's used
      for (i=0; i<3; i++) {
         H_tx_to_neck[i][0] = y_hat[i];
         H_tx_to_neck[i][1] = x_hat[i];
         H_tx_to_neck[i][2] = -z_hat[i];
         H_tx_to_neck[i][3] = 0.0;
      }
      pfMatrix H_ts_to_uw;
      pfMultMat(H_ts_to_uw, H_ts_to_tx, H_tx_to_neck);

      valid &= rifle->calibrate(H_rifle, H_ts_to_uw);
   }

   return valid;
}


//----------------------------------------------------------------------
//   Function: set_multival (for placing the rifle)
//    Returns: 
// Parameters: 
//    Summary: 
//----------------------------------------------------------------------
int UpperBodyClass::set_multival(const NPS_MULTIVAL tag,
                                 void *values)
{
   int valid = TRUE;

   switch (tag) {
      case NPS_MV_RIGHTHAND:      // Set the Jack right hand pfMatrix
         float *H_uw2rw = (float *) values;

         pfTransposeMat(H_uw_to_rw, (float (*)[4]) H_uw2rw); 
         break;

      case NPS_MV_LEFTHAND:       // Set the Jack left hand pfMatrix
         float *H_uw2lw = (float *) values;

         pfTransposeMat(H_uw_to_lw, (float (*)[4]) H_uw2lw);
         break;

      default:
         valid = FALSE;
         cerr << "Error: unknown NPS_MULTIVAL encountered in"
              << " UpperBodyClass::set_multival()" << endl;
   }

   return valid;
}


//----------------------------------------------------------------------
//   Function: get_multival
//    Returns: 
// Parameters: 
//    Summary: Note get_all_inputs must run before this to ensure the
//             most up to date data samples.
//----------------------------------------------------------------------
int UpperBodyClass::get_multival(const NPS_MULTIVAL tag,
                                 void *values)
{
   int valid = FALSE;

   switch (tag) {
      case NPS_MV_UPPERBODY: // Get the Jack arm angles:

         fixedDatumRecord *returnAngles = (fixedDatumRecord *) values;

         if (fastrak_unit->exists()) {
            float r_angles[NUMBER_OF_DOFS];
            float l_angles[NUMBER_OF_DOFS];

            // must call fastrak_unit->copyBuffer() or get_all_inputs()
            // first... This sets all the H_tx_to_ matrices and changes
            // the position portions to meters.

            // -------------------------------------------------------
            // compute Jack arm angles using MDH parameters
            valid = right_arm->inverseKinematics5adj(H_tx_to_rws,
                                                     H_tx_to_ts,
                                                     r_angles);
            valid &= left_arm->inverseKinematics5adj(H_tx_to_lws,
                                                     H_tx_to_ts,
                                                     l_angles);

            // -------------------------------------------------------
            // translate to Jack angles before output
            returnAngles[LS2].datum = l_angles[0]*DTOR;
            returnAngles[LS1].datum = (l_angles[1] + 90.0)*DTOR;
            returnAngles[LS0].datum = (l_angles[2] - 90.0)*DTOR;

            returnAngles[LE0].datum = l_angles[3]*DTOR;

            returnAngles[LW2].datum = (l_angles[4] + 90.0)*DTOR;
            returnAngles[LW1].datum = 0.0f;
            returnAngles[LW0].datum = 0.0f;
      

            returnAngles[RS2].datum = r_angles[0]*DTOR;
            returnAngles[RS1].datum = (r_angles[1] + 90.0)*DTOR;
            returnAngles[RS0].datum = (r_angles[2] - 90.0)*DTOR;

            returnAngles[RE0].datum = r_angles[3]*DTOR;

            returnAngles[RW2].datum = (r_angles[4] + 90.0)*DTOR;
            returnAngles[RW1].datum = 0.0f;
            returnAngles[RW0].datum = 0.0f;
         }
         break;

      case NPS_MV_RIFLE: // Get the Jack rifle DCS info

         fixedDatumRecord *returnCoord = (fixedDatumRecord *) values;

         if (fastrak_unit->exists()) {
            // must call get_all_inputs() first
            valid = TRUE;
            pfCoord rifle_coord;

            rifle->computeCoord(H_tx_to_rs,  H_tx_to_ts,
                                H_tx_to_rws, H_tx_to_lws,
                                H_uw_to_rw,  H_uw_to_lw,
                                &rifle_coord);

            returnCoord[RCX].datum = rifle_coord.xyz[PF_X];
            returnCoord[RCY].datum = rifle_coord.xyz[PF_Y];
            returnCoord[RCZ].datum = rifle_coord.xyz[PF_Z];
            returnCoord[RCH].datum = rifle_coord.hpr[PF_H];
            returnCoord[RCP].datum = rifle_coord.hpr[PF_P];
            returnCoord[RCR].datum = rifle_coord.hpr[PF_R];
         }
         else {
            returnCoord[RCX].datum = 0.75;
            returnCoord[RCY].datum = 0.0;
            returnCoord[RCZ].datum = 0.0;
            returnCoord[RCH].datum = 0.0;
            returnCoord[RCP].datum = 0.0;
            returnCoord[RCR].datum = 0.0;
         }
         break;

      case NPS_MV_TARGET_RIFLE: // Get the NPSNET rifle DCS info

         pfCoord *returnRifleCoord = (pfCoord *) values;

         if (fastrak_unit->exists()) {
            // must call get_all_inputs() first
            valid = TRUE;
            rifle->computeTargetCoord(H_tx_to_rs,
                                      H_tx_to_ts,
                                      returnRifleCoord);
         }
         else {
            returnRifleCoord->xyz[PF_X] = 0.0;
            returnRifleCoord->xyz[PF_Y] = 0.0;
            returnRifleCoord->xyz[PF_Z] = 0.0;
            returnRifleCoord->hpr[PF_H] = 0.0;
            returnRifleCoord->hpr[PF_P] = 0.0;
            returnRifleCoord->hpr[PF_R] = 0.0;
         }
         break;

      default:
         cerr << "Error: unknown NPS_MULTIVAL encountered in"
              << " UpperBodyClass::get_multival()" << endl;
   }

   return valid;
}
