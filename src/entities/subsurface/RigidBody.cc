////////////////////////////////////////////////////////////////////////////////
/*
 Program:         RigidBody.C

 Author:          Don Brutzman

 Revised:         18 October 94

 System:          Irix 5.2

 Compiler:        ANSI C++

 Compilation:     irix> make dynatest
                  irix> CC RigidBody.C -lm -c -g +w 

                   -c == Produce binaries only, suppressing the link phase.
                   +w == Warn about all questionable constructs.

 Description:     Rigid body class specification.  
                  Note: world coordinate system, NOT body coordinate system,
                        thus Euler angles are used.

 Advisors:        Dr. Mike Zyda, Dr. Bob McGhee and Dr. Tony Healey

 References:      Fu, K.S., Gonzalez, R.C., and Lee, C.S.G.,
                  _Robotics:  Control, Sensing, Vision and Intelligence_,
                  McGraw-Hill Inc., NY, 1987.

                  Cooke, J.C., Zyda, M.J., Pratt, D.R., and McGhee, R.B.,
                  "NPSNET:  Flight Simulation Dynamic Modeling using
                  Quaternions," _PRESENCE_, vol. 1 no. 4, Fall 1992,
                  pp. 404-420.

                  Cooke, Joseph C., "NPSNET:  Flight Simulation Dynamic
                  Modeling using Quaternions," masters thesis, Naval
                  Postgraduate School, December 1993.

*/

////////////////////////////////////////////////////////////////////////////////

#include "RigidBody.h"
//----------------------------------------------------------------------------//
// constructor methods
//----------------------------------------------------------------------------//

RigidBody:: RigidBody () // default constructor
{
   hmatrix.set_identity ();
   time_of_posture = 0.0;
       x_dot       = 0.0;
       y_dot       = 0.0;
       z_dot       = 0.0;
     phi_dot       = 0.0;
   theta_dot       = 0.0;
     psi_dot       = 0.0;
}
//----------------------------------------------------------------------------//

RigidBody:: RigidBody (const Hmatrix initialhmatrix)
{
   hmatrix = Hmatrix (initialhmatrix);
   time_of_posture = 0.0;
       x_dot       = 0.0;
       y_dot       = 0.0;
       z_dot       = 0.0;
     phi_dot       = 0.0;
   theta_dot       = 0.0;
     psi_dot       = 0.0;
}
//----------------------------------------------------------------------------//

RigidBody:: RigidBody (const Hmatrix initialhmatrix, const double t)
{
   hmatrix = Hmatrix (initialhmatrix);
   time_of_posture = t;
       x_dot       = 0.0;
       y_dot       = 0.0;
       z_dot       = 0.0;
     phi_dot       = 0.0;
   theta_dot       = 0.0;
     psi_dot       = 0.0;
}
//----------------------------------------------------------------------------//

RigidBody:: RigidBody (const double x,
                       const double y,    
                       const double z,
                       const double phi,
                       const double theta,
                       const double psi,  
                       const double t)
{
   hmatrix         = Hmatrix (phi, theta, psi, x, y, z);
   time_of_posture = t;
       x_dot       = 0.0;
       y_dot       = 0.0;
       z_dot       = 0.0;
     phi_dot       = 0.0;
   theta_dot       = 0.0;
     psi_dot       = 0.0;
}
//----------------------------------------------------------------------------//

RigidBody& RigidBody:: operator = (const RigidBody  &rb_rhs)     // assignment
{
   hmatrix         = rb_rhs.hmatrix;
   time_of_posture = rb_rhs.time_of_posture;
       x_dot       = rb_rhs.x_dot;
       y_dot       = rb_rhs.y_dot;
       z_dot       = rb_rhs.z_dot;
     phi_dot       = rb_rhs.phi_dot;
   theta_dot       = rb_rhs.theta_dot;
     psi_dot       = rb_rhs.psi_dot;
   return *this;
}
//----------------------------------------------------------------------------//


ostream& operator << (ostream& out, RigidBody& rb)
{
   rb.hmatrix.print_hmatrix ();
   out << "time_of_posture    = "  << rb.time_of_posture << endl;
   out << "< x_dot, y_dot, z_dot, phi_dot, theta_dot, psi_dot> = <"
       << rb.x_dot   << ", " << rb.y_dot     << ", " << rb.z_dot   << ", "
       << rb.phi_dot << ", " << rb.theta_dot << ", " << rb.psi_dot << ">" 
       << endl;
   return out;
}
//----------------------------------------------------------------------------//
// inspection methods
//----------------------------------------------------------------------------//

void RigidBody:: print_rigidbody ()
const
{
   hmatrix.print_hmatrix ();;
   cout << "time_of_posture    = "  << time_of_posture << endl;
   cout << "< x_dot, y_dot, z_dot, phi_dot, theta_dot, psi_dot> = <"
        <<    x_dot << ", " <<     y_dot << ", " <<   z_dot << ", "
        <<  phi_dot << ", " << theta_dot << ", " << psi_dot << ">" << endl;
}
//----------------------------------------------------------------------------//

Hmatrix RigidBody:: hmatrix_value ()   // note this returns an Hmatrix object,
const                                  //      not a [4][4] array
{
   return hmatrix;
}
//----------------------------------------------------------------------------//

double RigidBody:: time_of_posture_value ()
const
{
   return time_of_posture;
}
//----------------------------------------------------------------------------//

double RigidBody:: x_value ()  
const
{
   return hmatrix.x_value ();
}
//----------------------------------------------------------------------------//

double RigidBody:: y_value ()  
const
{
   return hmatrix.y_value ();
}
//----------------------------------------------------------------------------//

double RigidBody:: z_value ()  
const
{
   return hmatrix.z_value ();
}
//----------------------------------------------------------------------------//

double RigidBody:: phi_value () 
const
{
   return hmatrix.phi_value ();
}
//----------------------------------------------------------------------------//

double RigidBody:: theta_value ()  
const
{
   return hmatrix.theta_value ();
}
//----------------------------------------------------------------------------//

double RigidBody:: psi_value () 
const
{
   return hmatrix.psi_value ();
}
//----------------------------------------------------------------------------//

double RigidBody:: x_dot_value ()
const
{
   return x_dot;
}
//----------------------------------------------------------------------------//

double RigidBody:: y_dot_value ()
const
{
   return y_dot;
}
//----------------------------------------------------------------------------//

double RigidBody:: z_dot_value ()
const
{
   return z_dot;
}
//----------------------------------------------------------------------------//

double RigidBody:: phi_dot_value ()
const
{
   return phi_dot;
}
//----------------------------------------------------------------------------//

double RigidBody:: theta_dot_value ()
const
{
   return theta_dot;
}
//----------------------------------------------------------------------------//

double RigidBody:: psi_dot_value ()
const
{
   return psi_dot;
}
//----------------------------------------------------------------------------//
// modifying methods
//----------------------------------------------------------------------------//

void RigidBody:: RigidBody_initialize ()
{
   hmatrix.set_identity ();
   time_of_posture = 0.0;
       x_dot       = 0.0;
       y_dot       = 0.0;
       z_dot       = 0.0;
     phi_dot       = 0.0;
   theta_dot       = 0.0;
     psi_dot       = 0.0;
}
//----------------------------------------------------------------------------//

void RigidBody:: set_velocities (double new_x_dot, 
                                 double new_y_dot, 
                                 double new_z_dot,
                                 double new_phi_dot, 
                                 double new_theta_dot, 
                                 double new_psi_dot)
{
       x_dot = new_x_dot;
       y_dot = new_y_dot;
       z_dot = new_z_dot;
     phi_dot = new_phi_dot;
   theta_dot = new_theta_dot;
     psi_dot = new_psi_dot;
}
//----------------------------------------------------------------------------//

void RigidBody:: set_linear_velocities
                                (double new_x_dot, 
                                 double new_y_dot, 
                                 double new_z_dot)
{
   x_dot = new_x_dot;
   y_dot = new_y_dot;
   z_dot = new_z_dot;
}
//----------------------------------------------------------------------------//

void RigidBody:: set_angular_velocities
                                (double new_phi_dot, 
                                 double new_theta_dot, 
                                 double new_psi_dot)
{
     phi_dot = new_phi_dot;
   theta_dot = new_theta_dot;
     psi_dot = new_psi_dot;
}
//----------------------------------------------------------------------------//

void RigidBody:: set_time_of_posture (double new_time_of_posture)
{
   time_of_posture = new_time_of_posture;
}
//----------------------------------------------------------------------------//

void RigidBody:: update_Hmatrix     (double delta_t)
{
   hmatrix.incremental_rotation    (  phi_dot, theta_dot, psi_dot,   delta_t);
   hmatrix.incremental_translation (Vector3D ( x_dot, y_dot, z_dot), delta_t);
}
//----------------------------------------------------------------------------//



