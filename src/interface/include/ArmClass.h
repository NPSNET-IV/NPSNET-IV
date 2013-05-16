// *********************************************************************
// File    : ArmClass.h
// Author  : Scott McMillan
//         : Naval Postgraduate School
//         : Code CSMs
//         : Monterey, CA 93943
//         : mcmillan@cs.nps.navy.mil
// Project : NPSNET - Individual Combatants
// Created : August 1995
// Summary : This file contains the definitions for a C++ class to
//         : manage the kinematics computations required to instrument
//         : the arms.
// Modified: 
// *********************************************************************

#ifndef __ARM_CLASS__
#define __ARM_CLASS__

#include "Performer/pf.h"

// MDH parameters for the Jack model (yes it can be done!)
#define UPPER_ARM 0.328
#define LOWER_ARM 0.2593
#define UL_RATIO  UPPER_ARM/(LOWER_ARM + UPPER_ARM)

#define NUMBER_OF_DOFS 5
#define EPSILON 0.001   // threshold to avoid large roundoff errors in
                        // some of then inverse kinematics computations

// Notation:
//   rx - polhemus receiver
//   tx - polhemus transmitter
//   w  - wrist MDH CS
//   s  - shoulder MDH CS

//======================================================================
// ArmClass definitions
//======================================================================
class ArmClass
{
protected:
   // MDH parameters
   float a[NUMBER_OF_DOFS], alpha[NUMBER_OF_DOFS];
   float d[NUMBER_OF_DOFS], theta[NUMBER_OF_DOFS];
   float arm_length;
   float d3d5sqr;

   // calibration information:
   int calibrated_flag;
   pfMatrix H_ws_to_w;       // wrist sensor to wrist
   pfMatrix H_sh_to_ts;      // torso sensor to shoulder transformation
                             //   computed with the newer calibrate function.

   // Homogeneous transformation matrices once used during calibration:
   pfMatrix H_w_to_s_side;   // wrist to shoulder when out to side
   pfMatrix H_w_to_s_front;  // wrist to shoulder when out front

public:
   ArmClass();
   ~ArmClass();

   inline int isCalibrated() {return calibrated_flag;}
   void getJointAngles(float angles[NUMBER_OF_DOFS]);
   void setJointAngles(float angles[NUMBER_OF_DOFS]);

   // MDH matrix convenience functions
   void outputHMatrix(pfMatrix Hmat);
   void computeMDHMatrix(float a, float alpha, float d, float theta,
                         pfMatrix Hmat);
   void computeMDHMatrixInv(float a, float alpha, float d, float theta,
                            pfMatrix Hmat);
   void forwardKinematics(int num_joints,
                          float a[], float alpha[],
                          float d[], float theta[],
                          pfMatrix Hmat);
   void forwardKinematics(pfMatrix Hmat);

   // virtual functions
   virtual int calibrate(pfMatrix H_ts_to_sh) = 0;
   virtual int calibrate(pfMatrix H_ws_side, pfMatrix H_ws_front,
                         pfMatrix H_ts_side, pfMatrix H_ts_front,
                         pfMatrix H_ts_to_sh,
                         pfVec3 p_shoulder) = 0;

   virtual int inverseKinematics5(pfMatrix H_tx_to_ws,
                                  pfMatrix H_tx_to_ts,
                                  float angles[5]) = 0;
   virtual int inverseKinematics5adj(pfMatrix H_tx_to_ws,
                                     pfMatrix H_tx_to_ts,
                                     float angles[5]) = 0;
};

#endif
