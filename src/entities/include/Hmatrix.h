// File: <Hamtrix.h>

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

#ifndef HMATRIX_C
#define HMATRIX_C    // prevent errors if multiple #includes present
#include "Quaternion.h"

class Hmatrix
{
private:

// member data fields

   double   htmatrix [4][4];

public:

// constructors and destructor

   Hmatrix                  ();

   Hmatrix                  (const double       x,
                             const double       y,    
                             const double       z,    
                             const double     phi,  
                             const double   theta,
                             const double     psi);

   Hmatrix                  (const Vector3D   position_Vector3D,
                             const double     phi,  
                             const double   theta,
                             const double     psi);

   Hmatrix                  (const Vector3D   position_Vector3D,
                             const Vector3D   eulerangles3D);

   Hmatrix                  (const Vector3D   position_Vector3D,
                             const Quaternionn posture);

   Hmatrix                  (const Hmatrix&   input_hmatrix);

  ~Hmatrix                  ()                { /* null body */ }

// operators

   Hmatrix&   operator =    (const Hmatrix  &h_rhs);       // assignment
   Hmatrix&   operator *    (const Hmatrix  &h_rhs);       // Hmatrix multiply
   Hmatrix&   operator *    (const Vector3D &v_rhs);       // Vector3D multiply

   friend ostream& operator << (ostream& os, Hmatrix& h);

// inspection methods

   void       print_hmatrix () const;

   double     x_value       () const;
   double     y_value       () const;
   double     z_value       () const;
   double   phi_value       () const;
   double theta_value       () const;
   double   psi_value       () const;

   Quaternionn quaternion_value
                            () const;

   Vector3D   position      () const;
   Vector3D   camera        () const;
   double     scale         () const;

   double     element       (const int row,           // accessor returns lvalue
                             const int column) const; //   for row/column [1..4]

// modifying methods

   void       rotate        (const double   phi,
                             const double   theta,
                             const double   psi);

   void       rotate        (const Vector3D rotation3D);

   void       rotate_x      (const double   phi);
   void       rotate_y      (const double   theta);
   void       rotate_z      (const double   psi);

   void       incremental_rotation
                            (const double   phi_dot,   
                             const double theta_dot,
                             const double   psi_dot,   
                             const double delta_t  );

   void       translate     (const double   delta_x,
                             const double   delta_y,
                             const double   delta_z);

   void       translate     (const Vector3D translation3D);

   void       incremental_translation
                            (const double   x_dot,  const double y_dot,
                             const double   z_dot,  const double delta_t);

   void       incremental_translation
                            (const Vector3D velocities3D,
                             const double   delta_t);

   void       set_identity  ();

   void       set_posture   (const double phi, 
                             const double theta,  
                             const double psi);

   void       set_position  ( Vector3D translation3D);

   void       set_camera    (const Vector3D translation3D);
   void       move_camera   (const Vector3D translation3D);

   void       set_all_scales(const double scale_x,  
                             const double scale_y,  
                             const double scale_z,
                             const double scale_global);
   void       set_scale     (const double scale_global);

};





#endif    // #ifndef HMATRIX_C
