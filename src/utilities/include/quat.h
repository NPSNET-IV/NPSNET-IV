/*
 *  Program:: I-Port
 *  Project:: ISMS
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  (C) Copyright Sarcos Group/University of Utah Research Foundation, 1992
 *
 *  The contents of this file may be reproduced electronically, or in
 *  printed form, IN ITS ENTIRETY with no changes, providing that this
 *  copyright notice is intact and applicable in the copy. No other
 *  copying is permitted without the expressed written consent of the
 *  copyright holder.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Description::
 *
 *
 *  Compile Flags::
 *    flag ; description.
 *
 *****************************************************************************/

#ifndef _NPS_QUAT
#define _NPS_QUAT
#ifdef VW
typedef float Coord;
typedef float Matrix[4][4];
#else
#include <gl/gl.h>
#endif

typedef float (*pMatrix)[4];
typedef Coord Quaternion[4];
typedef Coord *pQuat;

typedef struct frametype {
  double Origin[3];
  double Xaxis[3];
  double Yaxis[3];
} Frame;

typedef Coord Euler[3];
typedef Coord *pEuler;
     
/* A composite representation of a frame.  This includes the matrix
   representation and the quaternion representation of the
   orientation. */
typedef struct coord_sys_type {
  Matrix mat;
  Quaternion orient;
} CoordSys;

pMatrix makeIdent();
pMatrix quat2mat(Quaternion, Matrix);

pQuat makeQuat(double, double, double, double, pQuat);

pQuat negquat(Quaternion, Quaternion);
pQuat quatmult(Quaternion, Quaternion, Quaternion);
pQuat quatnorm(Quaternion, Quaternion);
pQuat quatinit();
pQuat mat2quat(Matrix, Quaternion);
pMatrix frame2mat(Frame, Matrix);
pQuat frame2quat(Frame, Quaternion);
pQuat normquat(Quaternion, Quaternion);

Frame makeFrame(double, double, double, 
		double, double, double, 
		double, double, double);

pEuler mat2euler (Matrix, Euler);
pEuler mat2euler2 (Matrix, Euler);
pEuler quat2euler(Quaternion, Euler);

Coord length3(Coord v[3]);
void printquat(Quaternion);
void printmat(Matrix);
void printeuler(Euler);

void our_boresight (double, double, double, double, Quaternion);
CoordSys *calibrate_polhemus (double, double, double, 
			      double, double, double, 
			      double, double, double,
			      double, double, double);
void get_euler (CoordSys *, Quaternion, Quaternion, float[3]);
void get_euler2 (CoordSys *, Quaternion, Quaternion, float[3]);
void get_euler3 (CoordSys *, Quaternion, Quaternion, float[3], Quaternion);
void trak_trans (CoordSys *, float[3], float[3]);

pQuat slerp (Quaternion, Quaternion, Coord, Quaternion);
pQuat quatav (Quaternion[], int, Quaternion);

#ifndef PI
#define PI 3.141592654
#endif

#define NUM_SAMPLES 4

struct filter
{
  Quaternion samples[NUM_SAMPLES];
  int index;
  int total;
};

pQuat new_filter_val (struct filter *, Quaternion, Quaternion);

#define W 3
#define X 0
#define Y 1
#define Z 2

#define HEADING 0
#define PITCH 1
#define ROLL 2


#endif
