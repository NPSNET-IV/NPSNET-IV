// File: <Quaternion.h>

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
 *    URL: http://www.cs.nps.navy.mil/research/npsnet/
 *       General code questions, concerns, comments, requests for
 *       distributions and documentation, and bug reports.
 *
 * Thank you to our sponsors:  ARL, ARPA, DMSO, STRICOM, TEC, TRAC and
 *                             NPS Direct Funding
 */

////////////////////////////////////////////////////////////////////////////////

#ifndef  QUATERNION_C
#define  QUATERNION_C    // prevent errors if multiple #includes present

#include "Vector3D.h"


////////////////////////////////////////////////////////////////////////////////
// utility function prototypes

double   sign       (double);
double   degrees    (double); // radians input
double   radians    (double); // degrees input
double   arcclamp   (double);
double   dnormalize (double);  // returns 0..2PI
int      inormalize (double);  // returns 0..359

////////////////////////////////////////////////////////////////////////////////


class Quaternionn : public Vector3D
{
private:

// member data fields

   double      q0;
   double      q1;
   double      q2;
   double      q3;

public:

// constructors and destructor

   Quaternionn                 ();

   Quaternionn                 (const double phi, 
                               const double theta,
                               const double psi);

   Quaternionn                 (const double new_q0, 
                               const double new_q1,
                               const double new_q2, 
                               const double new_q3);

   Quaternionn                 (const Quaternionn& q);

  ~Quaternionn()               { /* null body */ }

// operators

   Quaternionn& operator =     (const  Quaternionn& q_rhs);
   Quaternionn  operator +     (const  Quaternionn& q_rhs);
   Quaternionn  operator -     (const  Quaternionn& q_rhs);
   Quaternionn  operator *     (const  Quaternionn& q_rhs);
   Quaternionn  operator *     (double scalar);
   double&     operator []    (int);

   friend ostream& operator << (ostream& out, Quaternionn& q);

// inspection methods

   void        print          ();

   double        phi_value    () const; // Euler angle roll
   double      theta_value    () const; // Euler angle pitch
   double        psi_value    () const; // Euler angle yaw

   double      magnitude      () const; // Normally 1.0, unit quaternions
   Vector3D    euler_angles   () const; // Euler angle  phi,  theta, psi
                                        //             (roll, pitch, yaw)
                                        // more efficient than individual calls
   Quaternionn  conjugate      () const; // negates vector part
   Quaternionn  inverse        () const; // negates vector part & normalizes

// modifying methods

   void        rotate         (const double delta_phi,
                               const double delta_theta,
                               const double delta_psi  );

   void        incremental_rotate (const double P, const double Q,
                                   const double RR, const double delta_t);

   void        update         (double   P,    double Q,     double RR,
                                                            double seconds);

   void        set            (double   phi,  double theta, double psi,
                                                            double rotation);
   void        normalize      ();

};

#endif   // #ifndef QUATERNION_C
