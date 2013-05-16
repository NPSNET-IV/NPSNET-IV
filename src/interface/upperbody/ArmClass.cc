// *********************************************************************
// File    : ArmClass.cc
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "ArmClass.h"


//======================================================================
// ArmClass class
//======================================================================

//----------------------------------------------------------------------
//   Function: ArmClass::ArmClass
//    Summary: constructor for ArmClass object
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
ArmClass::ArmClass()
{
   calibrated_flag = FALSE;
}


//----------------------------------------------------------------------
//   Function: ArmClass::~ArmClass
//    Summary: destructor for ArmClass object
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
ArmClass::~ArmClass()
{

}


//----------------------------------------------------------------------
//   Function: getJointAngles
//    Summary: destructor for ArmClass object
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void ArmClass::getJointAngles(float angles[NUMBER_OF_DOFS])
{
   for (int i=0; i<NUMBER_OF_DOFS; i++) {
      angles[i] = theta[i];
   }
}


//----------------------------------------------------------------------
//   Function: setJointAngles
//    Summary: destructor for ArmClass object
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void ArmClass::setJointAngles(float angles[NUMBER_OF_DOFS])
{
   for (int i=0; i<NUMBER_OF_DOFS; i++) {
      theta[i] = angles[i];
   }
}


//----------------------------------------------------------------------
//   Function: output
//    Summary: output a homogeneous transformation matrix (3x4 only)
// Parameters: 
//    Returns: 
//----------------------------------------------------------------------
void ArmClass::outputHMatrix(pfMatrix Hmat)
{
   for (int i=0; i<4; i++)
      printf(" %6.3f %6.3f %6.3f %6.3f\n",
             Hmat[i][0], Hmat[i][1], Hmat[i][2], Hmat[i][3]);
   printf("\n");
}


//----------------------------------------------------------------------
//   Function: computeMDHMatrix
//    Summary: generate an H-matrix from MDH parameters ^{i-1}H_i
// Parameters: MDH parameters (angles in degrees)
//    Returns: 
//----------------------------------------------------------------------
void ArmClass::computeMDHMatrix(float a, float alpha,
                                float d, float theta,
                                pfMatrix Hmatrix)
{
   float sa, ca, st, ct;
   pfSinCos(alpha, &sa, &ca);
   pfSinCos(theta, &st, &ct);

   Hmatrix[0][0] = ct;
   Hmatrix[1][0] = st*ca;
   Hmatrix[2][0] = st*sa;

   Hmatrix[0][1] = -st;
   Hmatrix[1][1] = ct*ca;
   Hmatrix[2][1] = ct*sa;

   Hmatrix[0][2] = 0.0;
   Hmatrix[1][2] = -sa;
   Hmatrix[2][2] = ca;

   Hmatrix[0][3] = a;
   Hmatrix[1][3] = -sa*d;
   Hmatrix[2][3] = ca*d;

   Hmatrix[3][0] = 0.0;
   Hmatrix[3][1] = 0.0;
   Hmatrix[3][2] = 0.0;
   Hmatrix[3][3] = 1.0;
}


//----------------------------------------------------------------------
//   Function: computeMDHMatrixInv
//    Summary: generate inverse H-matrix from MDH parameters ^{i}H_{i-1}
// Parameters: MDH parameters (angles in degrees)
//    Returns: 
//----------------------------------------------------------------------
void ArmClass::computeMDHMatrixInv(float a, float alpha,
                                   float d, float theta,
                                   pfMatrix Hmatrix)
{
   float sa, ca, st, ct;
   pfSinCos(alpha, &sa, &ca);
   pfSinCos(theta, &st, &ct);

   Hmatrix[0][0] = ct;
   Hmatrix[1][0] = -st;
   Hmatrix[2][0] = 0.0;

   Hmatrix[0][1] = st*ca;
   Hmatrix[1][1] = ct*ca;
   Hmatrix[2][1] = -sa;

   Hmatrix[0][2] = st*sa;
   Hmatrix[1][2] = ct*sa;
   Hmatrix[2][2] = ca;

   Hmatrix[0][3] = -a*ct;
   Hmatrix[1][3] =  a*st;
   Hmatrix[2][3] = -d;

   Hmatrix[3][0] = 0.0;
   Hmatrix[3][1] = 0.0;
   Hmatrix[3][2] = 0.0;
   Hmatrix[3][3] = 1.0;
}


//----------------------------------------------------------------------
//   Function: forwardKinematics
//    Summary: compute Hmatrix corresponding to a set of MDH parameters
// Parameters: MDH parameters (angles in degrees)
//    Returns: 
//----------------------------------------------------------------------
void ArmClass::forwardKinematics(int num_joints,
                                 float a[], float alpha[],
                                 float d[], float theta[],
                                 pfMatrix Hmat)
{
   computeMDHMatrix(a[0], alpha[0], d[0], theta[0], Hmat);

   for (int i=1; i<num_joints; i++) {
      pfMatrix Htemp;

      computeMDHMatrix(a[i], alpha[i], d[i], theta[i], Htemp);
      pfPostMultMat(Hmat, Htemp);
   }
}


//----------------------------------------------------------------------
//   Function: forwardKinematics
//    Summary: compute Hmatrix corresponding to a set of MDH parameters
// Parameters: MDH parameters (angles in degrees)
//    Returns: 
//----------------------------------------------------------------------
void ArmClass::forwardKinematics(pfMatrix Hmat)
{
   computeMDHMatrix(a[0], alpha[0], d[0], theta[0], Hmat);

   for (int i=1; i<NUMBER_OF_DOFS; i++) {
      pfMatrix Htemp;

      computeMDHMatrix(a[i], alpha[i], d[i], theta[i], Htemp);
      pfPostMultMat(Hmat, Htemp);
   }
}
