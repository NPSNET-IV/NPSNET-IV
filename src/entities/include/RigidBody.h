// File: <RigidBody.h>

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

#ifndef RIGIDBODY_C
#define RIGIDBODY_C    // prevent errors if multiple #includes present

#include "Hmatrix.h"

class RigidBody : public Hmatrix
{ 
private:

// member data fields

   double    time_of_posture;

   double        x_dot;         // linear  velocity along x axis
   double        y_dot;         // linear  velocity along y axis
   double        z_dot;         // linear  velocity along z axis
   double      phi_dot;         // angular velocity about x axis
   double    theta_dot;         // angular velocity about y axis
   double      psi_dot;         // angular velocity about z axis


public:

   Hmatrix   hmatrix;           // need to access print & set routines

// constructors and destructor

   RigidBody                    ();
   RigidBody                    (const Hmatrix initialhmatrix);
  
   RigidBody                    (const Hmatrix initialhmatrix, const double t);

   RigidBody                    (const double x,
                                 const double y,        
                                 const double z,
                                 const double phi,   
                                 const double theta,
                                 const double psi,      
                                 const double t);

  ~RigidBody                    ()                 { /* null body */ }

// operators

   RigidBody&   operator =      (const RigidBody  &rb_rhs);       // assignment

   friend ostream & operator << (ostream& out, RigidBody& rb);

// inspection methods

   void       print_rigidbody        () const;

   Hmatrix    hmatrix_value          () const;
   double     time_of_posture_value  () const;

   double         x_value            () const;
   double         y_value            () const;
   double         z_value            () const;
   double       phi_value            () const;
   double     theta_value            () const;
   double       psi_value            () const;

   double         x_dot_value        () const;
   double         y_dot_value        () const;
   double         z_dot_value        () const;
   double       phi_dot_value        () const;
   double     theta_dot_value        () const;
   double       psi_dot_value        () const;


// modifying methods

   void       RigidBody_initialize   ();

   void       set_velocities         (double new_x_dot,
                                      double new_y_dot,
                                      double new_z_dot,
                                      double new_phi_dot, 
                                      double new_theta_dot, 
                                      double new_psi_dot);

   void       set_linear_velocities  (double new_x_dot, 
                                      double new_y_dot, 
                                      double new_z_dot);

   void       set_angular_velocities (double new_phi_dot, 
                                      double new_theta_dot, 
                                      double new_psi_dot);

   void       set_time_of_posture    (double new_time_of_posture);

   void       update_Hmatrix         (double delta_t);

};
#endif // RIGIDBODY_C
