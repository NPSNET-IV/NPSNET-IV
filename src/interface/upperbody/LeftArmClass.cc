// *********************************************************************
// File    : LeftArmClass.cc
// Author  : Scott McMillan
//         : Naval Postgraduate School
//         : Code CSMs
//         : Monterey, CA 93943
//         : mcmillan@cs.nps.navy.mil
// Project : NPSNET - Individual Combatants
// Created : August 1995
// Summary : This file contains the declarations for a C++ class to
//         : manage the kinematics computations required to instrument
//         : the left arm.
// Modified: 
// *********************************************************************

#include <math.h>
#include <iostream.h>

#include "Performer/pf.h"

#include "jointmods.h"

#include "ArmClass.h"
#include "LeftArmClass.h"

//======================================================================
// LeftArmClass class : public ArmClass
//======================================================================

//----------------------------------------------------------------------
//   Function: LeftArmClass::LeftArmClass
//    Summary: constructor for LeftArmClass object
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
LeftArmClass::LeftArmClass() : ArmClass()
{
   a[0] = a[1] = a[2] = a[3] = a[4] = 0.0;
   alpha[0] =   0.0;
   alpha[1] = -90.0;
   alpha[2] = -90.0;
   alpha[3] =  90.0;
   alpha[4] = -90.0;

   d[0] = d[1] = d[2] = d[3] = d[4] = 0.0;

   pfMatrix H1 = {{0, 0, 1, 0},
                  {0,-1, 0, 0},
                  {1, 0, 0, 0},
                  {0, 0, 0, 1}};
   pfCopyMat(H_ws_to_w, H1);

   pfMatrix H2 = {{1, 0, 0, 0},
                  {0,-1, 0, 0},
                  {0, 0,-1, 0},
                  {0, 0, 0, 1}};
   pfCopyMat(H_w_to_s_side, H2);

   pfMatrix H3 = {{1, 0, 0, 0},
                  {0, 0,-1, 0},
                  {0, 1, 0, 0},
                  {0, 0, 0, 1}};
   pfCopyMat(H_w_to_s_front, H3);
}


//----------------------------------------------------------------------
//   Function: LeftArmClass::~LeftArmClass
//    Summary: destructor for LeftArmClass object
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
LeftArmClass::~LeftArmClass()
{
}


//----------------------------------------------------------------------
//   Function: calibrate
//    Summary: 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
int LeftArmClass::calibrate(pfMatrix H_ts_to_sh)
{
   // compute the arm lengths
   arm_length = UPPER_ARM + LOWER_ARM;  // use model arm lengths

   d3d5sqr = arm_length*arm_length;

   // the upper and lower arm lengths MDH parameters:
   d[2] = UL_RATIO*arm_length;           // upper_arm_length 
   d[4] = (1.0 - UL_RATIO)*arm_length;   // lower_arm_length 

// compute the torso to shoulder H matrix:
   pfInvertFullMat(H_sh_to_ts, H_ts_to_sh);

#ifdef DEBUG
   printf("H_sh_to_ts (right):\n");
   outputHMatrix(H_sh_to_ts);
#endif

   return (calibrated_flag = TRUE);
}

//----------------------------------------------------------------------
//   Function: calibrate - old one used at AUSA '95
//    Summary: 
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
int LeftArmClass::calibrate(pfMatrix H_side, pfMatrix H_front,
                            pfMatrix H_ts_side, pfMatrix H_ts_front,
                            pfMatrix H_ts_to_sh,
                            pfVec3 p_shoulder)
{
   register int i,j;
   pfMatrix H_tx_to_sh, H_sh_to_tx;
   pfVec3 p_side, p_front;

   // measure arm length using the calibrate routine
   for (i=0; i<3; i++) {
      p_side[i] = H_side[i][3];
      H_side[i][3] = 0.0;
      p_front[i] = H_front[i][3];
      H_front[i][3] = 0.0;
   }

// II. compute the arm lengths
   // compute distance b/w samples
   float len_s = pfDistancePt3(p_shoulder, p_side);
   float len_f = pfDistancePt3(p_shoulder, p_front);

   d3d5sqr = 0.5*pfSqrDistancePt3(p_side, p_front);
   float len = sqrt(d3d5sqr);

   arm_length = 0.5*(len_s + len_f)*1.02;

   /* FIXME */ // this overrides calculation
   arm_length = UPPER_ARM + LOWER_ARM; // temporary

   d3d5sqr = arm_length*arm_length;

#ifdef DEBUG
   printf(" left arm_length = %f %f %f %f\n",len, len_s, len_f, arm_length);
#endif

   // estimate the upper and lower arm lengths:
   d[2] = UL_RATIO*arm_length;           // upper_arm_length 
   d[4] = (1.0 - UL_RATIO)*arm_length;   // lower_arm_length 

// I. Compute H-matrix from shoulder to transmitter: H_sh_to_tx
   pfCopyMat(H_tx_to_sh, H_side);
   pfPostMultMat(H_tx_to_sh, H_ws_to_w);
   pfPostMultMat(H_tx_to_sh, H_w_to_s_side);

   pfTransposeMat(H_sh_to_tx, H_tx_to_sh);

   for (j=0; j<3; j++) {
      H_sh_to_tx[j][3] = 0.0;
      for (i=0; i<3; i++) {
          H_sh_to_tx[j][3] -= H_sh_to_tx[j][i]*p_shoulder[i];
      }
   }

#ifdef DEBUG
   printf("H_sh_to_tx (left):\n");
   outputHMatrix(H_sh_to_tx);
#endif

// compute the torso to shoulder H matrix:
   (H_ts_front);
   pfMultMat(H_sh_to_ts, H_sh_to_tx, H_ts_side);

#ifdef DEBUG
   printf("H_sh_to_ts (left):\n");
   outputHMatrix(H_sh_to_ts);
#endif

   pfInvertFullMat(H_sh_to_ts, H_ts_to_sh);

#ifdef DEBUG
   printf("H_sh_to_ts (left - new):\n");
   outputHMatrix(H_sh_to_ts);
#endif

   return (calibrated_flag = TRUE);
}


//----------------------------------------------------------------------
//   Function: inverseKinematics5
//    Summary: 5 DOF inverse kinematics routine
// Parameters: Hmatrix of wrist wrt shoulder
//    Returns: 5 MDH joint angles (degrees),
//           : valid flag (i.e., NOT out of reach)
//----------------------------------------------------------------------
int LeftArmClass::inverseKinematics5(pfMatrix H_tx_to_ws,
                                     pfMatrix H_tx_to_ts,
                                     float angles[NUMBER_OF_DOFS])
{
   if (calibrated_flag) {
      int valid = TRUE;

      // --------- New code: -----------
      // get matrix from transmitter to shoulder via torso sensor:
      pfMatrix H_sh_to_tx;
      pfInvertFullMat(H_sh_to_tx, H_tx_to_ts);
      pfPreMultMat(H_sh_to_tx, H_sh_to_ts);
      // ------- end New code: ---------

      // get matrix from shoulder to wrist.
      pfMatrix H_sh_to_w;
      pfMultMat(H_sh_to_w, H_sh_to_tx, H_tx_to_ws);
      pfPostMultMat(H_sh_to_w, H_ws_to_w);

      // Step I. Compute theta_4 (elbow angle)
      float pmagsqr = (H_sh_to_w[0][3]*H_sh_to_w[0][3] +
                       H_sh_to_w[1][3]*H_sh_to_w[1][3] +
                       H_sh_to_w[2][3]*H_sh_to_w[2][3]);

      if (pmagsqr > d3d5sqr) {
         // cerr << "Warning: out of reach." << endl;
         valid = FALSE;

         angles[3] = theta[3] = 0.0;  // elbow = 0.0 -> full extension
         angles[4] = theta[4];        // forearm roll: use prev. value.
      }
      else {
         float tem = (pmagsqr - d[2]*d[2] - d[4]*d[4])/(2.0*d[2]*d[4]);

         // In certain bizarre instances roundoff in the computation of
         // tem will result in a magnitude greater than one.  So...
         if (tem > 1.0) 
            tem = 1.0;
         else if (tem < -1.0)
            tem = -1.0;

         angles[3] = theta[3] = RTOD*acosf(tem);
      }

      // Step II. Compute theta_5 (forearm roll):
      float s4 = sin(DTOR*theta[3]);
      if (fabs(s4) > EPSILON) {
         pfVec3 pw;
         for (int i=0; i<3; i++) {
            pw[i] = 0.0;
            for (int j=0; j<3; j++) {
               pw[i] += (-H_sh_to_w[j][i]*H_sh_to_w[j][3]);
            }
         }

         angles[4] = theta[4] = RTOD*atan2( pw[PF_Y]/(d[2]*s4),
                                               -pw[PF_X]/(d[2]*s4));
      }
      else {
         angles[4] = theta[4];    // forearm roll: use prev. value.
      }

      // Step III. compute shoulder angles: theta_1,2,3
      // Taken from inverseKinematics3() above.
      pfMatrix H_temp, H_shoulder;
      computeMDHMatrixInv(a[3], alpha[3], d[3], theta[3], H_shoulder);
      computeMDHMatrixInv(a[4], alpha[4], d[4], theta[4], H_temp);
      pfPreMultMat(H_shoulder, H_temp);
      pfPreMultMat(H_shoulder, H_sh_to_w);

      // compute the second of three shoulder angles:
      float s2 = sqrt(H_shoulder[0][2]*H_shoulder[0][2] +
                      H_shoulder[1][2]*H_shoulder[1][2]);
      angles[1] = theta[1] = RTOD*atan2(s2, -H_shoulder[2][2]);

      // compute the other two angles
      if (fabs(s2) > EPSILON) {
         angles[2] = theta[2] = RTOD*atan2(H_shoulder[2][1]/s2,
                                              -H_shoulder[2][0]/s2);
         angles[0] = theta[0] = RTOD*atan2(-H_shoulder[1][2]/s2,
                                               -H_shoulder[0][2]/s2);
      }
      else {
         float tmp13 = RTOD*atan2(H_shoulder[1][0], H_shoulder[0][0]);

         angles[2] = theta[2];  // use previous value
         angles[0] = theta[0] = tmp13 + theta[2];
      }

      return valid;
   }
   else {
      angles[0] = angles[1] = angles[2] = angles[3] = angles[4] = 0.0;
       theta[0] =  theta[1] =  theta[2] =  theta[3] =  theta[4] = 0.0;
      return FALSE;
   }
}


//----------------------------------------------------------------------
//   Function: inverseKinematics5adj
//    Summary: 5 DOF inverse kinematics routine with position adjust
// Parameters: Hmatrix of wrist wrt shoulder
//    Returns: 5 MDH joint angles (degrees),
//           : valid flag (i.e., NOT out of reach)
//----------------------------------------------------------------------
int LeftArmClass::inverseKinematics5adj(pfMatrix H_tx_to_ws,
                                        pfMatrix H_tx_to_ts,
                                        float angles[5])
{
   if (calibrated_flag) {
      int valid = TRUE;

      // --------- New code: -----------
      // get matrix from transmitter to shoulder via torso sensor:
      pfMatrix H_sh_to_tx;
      pfInvertFullMat(H_sh_to_tx, H_tx_to_ts);
      pfPreMultMat(H_sh_to_tx, H_sh_to_ts);
      // ------- end New code: ---------

      // get matrix from shoulder to wrist.
      pfMatrix H_sh_to_w;
      pfMultMat(H_sh_to_w, H_sh_to_tx, H_tx_to_ws);
      pfPostMultMat(H_sh_to_w, H_ws_to_w);

      // Step I. Compute theta_4 (elbow angle)
      float pmagsqr = (H_sh_to_w[0][3]*H_sh_to_w[0][3] +
                       H_sh_to_w[1][3]*H_sh_to_w[1][3] +
                       H_sh_to_w[2][3]*H_sh_to_w[2][3]);

      if (pmagsqr > d3d5sqr) {
         // cerr << "Warning: out of reach." << endl;
         valid = FALSE;

         angles[3] = theta[3] = 0.0;  // elbow = 0.0 -> full extension
         angles[4] = theta[4];        // forearm roll: use prev. value.
      }
      else {
         float tem = (pmagsqr - d[2]*d[2] - d[4]*d[4])/(2.0*d[2]*d[4]);

         // In certain bizarre instances roundoff in the computation of
         // tem will result in a magnitude greater than one.  So...
         if (tem > 1.0) 
            tem = 1.0;
         else if (tem < -1.0)
            tem = -1.0;

         angles[3] = theta[3] = RTOD*acosf(tem);
      }

      // Step II. Compute theta_5 (forearm roll):
      float s4 = sin(DTOR*theta[3]);
      if (fabs(s4) > EPSILON) {
         pfVec3 pw;
         for (int i=0; i<3; i++) {
            pw[i] = 0.0;
            for (int j=0; j<3; j++) {
               pw[i] += (-H_sh_to_w[j][i]*H_sh_to_w[j][3]);
            }
         }

         angles[4] = theta[4] = RTOD*atan2( pw[PF_Y]/(d[2]*s4),
                                               -pw[PF_X]/(d[2]*s4));
      }
      else {
         angles[4] = theta[4];    // forearm roll: use prev. value.
      }

      // Step III. compute shoulder angles: theta_1,2,3
      // Taken from inverseKinematics3() above.
      pfMatrix H_sh_to_el, H_5_to_3, H_5_to_4, H_4_to_3;
      computeMDHMatrixInv(a[3], alpha[3], d[3], theta[3], H_4_to_3);
      computeMDHMatrixInv(a[4], alpha[4], d[4], theta[4], H_5_to_4);
      pfMultMat(H_5_to_3, H_5_to_4, H_4_to_3);
      pfMultMat(H_sh_to_el, H_sh_to_w, H_5_to_3);

      // compute the second of three shoulder angles:
      float s2 = sqrt(H_sh_to_el[0][2]*H_sh_to_el[0][2] +
                      H_sh_to_el[1][2]*H_sh_to_el[1][2]);
      angles[1] = theta[1] = RTOD*atan2(s2, -H_sh_to_el[2][2]);

      // compute the other two angles
      if (fabs(s2) > EPSILON) {
         angles[2] = theta[2] = RTOD*atan2(H_sh_to_el[2][1]/s2,
                                              -H_sh_to_el[2][0]/s2);
         angles[0] = theta[0] = RTOD*atan2(-H_sh_to_el[1][2]/s2,
                                               -H_sh_to_el[0][2]/s2);
      }
      else {
         float tmp13 = RTOD*atan2(H_sh_to_el[1][0], H_sh_to_el[0][0]);

         angles[2] = theta[2];  // use previous value
         angles[0] = theta[0] = tmp13 + theta[2];
      }

      // Step IV. Adjust so that position is correct (but not necessarily
      // orientation). 

      // Get homogeneous transformation matrix for the above solution:
      pfMatrix Hcalc_sh_to_w;
      forwardKinematics(Hcalc_sh_to_w);

      pfVec3 p_orig, p_calc;
      for (int i=0; i<3; i++) {
         p_orig[i] = H_sh_to_w[i][3];
         p_calc[i] = Hcalc_sh_to_w[i][3];
      }

      // Compute the delta rotation matrix
      pfMatrix H_delta;
      /* XXX PF2.0 input vectors must already be normalized */
      pfNormalizeVec3 (p_orig);
      pfNormalizeVec3 (p_calc);
      pfMakeVecRotVecMat(H_delta, p_orig, p_calc);
      pfPreMultMat(H_sh_to_el, H_delta);

      // compute the second of three shoulder angles:
      s2 = sqrt(H_sh_to_el[0][2]*H_sh_to_el[0][2] +
                H_sh_to_el[1][2]*H_sh_to_el[1][2]);
      angles[1] = theta[1] = RTOD*atan2(s2, -H_sh_to_el[2][2]);

      // compute the other two angles
      if (fabs(s2) > EPSILON) {
         angles[2] = theta[2] = RTOD*atan2(H_sh_to_el[2][1]/s2,
                                               -H_sh_to_el[2][0]/s2);
         angles[0] = theta[0] = RTOD*atan2(-H_sh_to_el[1][2]/s2,
                                               -H_sh_to_el[0][2]/s2);
      }
      else {
         float tmp13 = RTOD*atan2(H_sh_to_el[1][0], H_sh_to_el[0][0]);

         angles[2] = theta[2];  // use previous value
         angles[0] = theta[0] = tmp13 + theta[2];
      }

      return valid;
   }
   else {
      angles[0] = angles[1] = angles[2] = angles[3] = angles[4] = 0.0;
       theta[0] =  theta[1] =  theta[2] =  theta[3] =  theta[4] = 0.0;
      return FALSE;
   }
}

