////////////////////////////////////////////////////////////////////////////////
/*
 Program:         Quaternion.C

 Revisions:       Don Brutzman
 
 Revised:         29 SEP 94

 System:          Irix

 Compiler:        ANSI C++

 Compilation:     irix> make dynatest
                  irix> CC Quaternion.C -lm -c -g +w 

                   -c == Produce binaries only, suppressing the link phase.
                   +w == Warn about all questionable constructs.

 Description:     Quaternion class specifications and implementation
                  All angle parameter values are in radians.

 Advisors:        Dr. Mike Zyda, Dr. Bob McGhee and Dr. Tony Healey

 References:      Haynes, Keith, "Computer Graphics Tools for the
                  Visualization of Spacecraft Dynamics," masters thesis,
                  Naval Postgraduate School, December 1993.  Includes
                  source code used for initial version of quaternion.c

                  Cooke, J.C., Zyda, M.J., Pratt, D.R., and McGhee, R.B.,
                  "NPSNET:  Flight Simulation Dynamic Modeling using
                  Quaternions," _PRESENCE_, vol. 1 no. 4, Fall 1992,
                  pp. 404-420.

                  Cooke, Joseph C., "NPSNET:  Flight Simulation Dynamic
                  Modeling using Quaternions," masters thesis, Naval
                  Postgraduate School, December 1993.

                  Chou, Jack C.K., "Quaternion Kinematic and Dynamic
                  Differential Equations," IEEE Transactions on Robotics
                  and Automation, vol. 8 no. 1, February 1992, pp.53-64.

                  Funda, Janez, Taylor, Russell, and Paul, Richard P.,
                  "On Homogenous Transforms, Quaternions, and Computational
                  Efficiency," IEEE Transactions on Robotics and Automation,
                  vol. 6 no. 3, June 1990, pp. 382-388.

                  Shoemake, Ken, "Animating Rotation with Quaternion Curves,"
                  Association for Computing Machinery _SIGGRAPH_ Proceedings,
                  vol. 19 no. 3, July 22-26 1985, pp. 245-254.

*/
#define  PI    3.1415926535897932

#ifdef TRUE
#undef TRUE
#endif
#ifdef FALSE
#undef FALSE
#endif

#define  TRUE  1
#define  FALSE 0

#include "Quaternion.h"

double sign (double x)
{
   if      (x > 0.0) return  1.0;
   else if (x < 0.0) return -1.0;
   else              return  1.0;
}

//----------------------------------------------------------------------------//

double degrees (double x)    // radians input
{
   return x * 180.0 / PI;
}

//----------------------------------------------------------------------------//

double radians (double x)   // degrees input
{
   return x * PI / 180.0;
}

//----------------------------------------------------------------------------//

double arcclamp (double x)
{
   if      (x >  1.0)
   {
      x =  1.0;
//      cout << " arcclamp reduced " << x << " to 1.0" << endl;
   }
   else if (x < -1.0)
   {
      x = -1.0;
//      cout << " arcclamp raised " << x << " to -1.0" << endl;
   }
   return x;
}
//----------------------------------------------------------------------------//

double dnormalize (double angle_radians)
{
   double new_angle = angle_radians;

   while (new_angle > 2*PI) new_angle -= 2*PI;
   while (new_angle < 0.0 ) new_angle += 2*PI;
   return new_angle;
}

int inormalize (double angle_radians)
{
   return (int) (degrees (angle_radians) + 0.5) % 360;
}


//----------------------------------------------------------------------------//
// constructor methods
//----------------------------------------------------------------------------//

Quaternionn:: Quaternionn ()
{
   q0 = 1.0;
   q1 = 0.0;
   q2 = 0.0;
   q3 = 0.0;
}
//----------------------------------------------------------------------------//

Quaternionn:: Quaternionn (const double phi, 
                         const double theta,  
                         const double psi)
{
   // reference:  Cooke thesis p. 41

   double r, p, y, cosr, cosp, cosy, sinr, sinp, siny;

   r    = (phi / 2.0);    p    = (theta / 2.0);    y    = (psi / 2.0);
   cosr = cos (r);        cosp = cos (p);          cosy = cos (y);
   sinr = sin (r);        sinp = sin (p);          siny = sin (y);
   q0 =   (cosr * cosp * cosy) + (sinr * sinp * siny);
   q1 =   (sinr * cosp * cosy) - (cosr * sinp * siny);
   q2 =   (cosr * sinp * cosy) + (sinr * cosp * siny);
   q3 = - (sinr * sinp * cosy) + (cosr * cosp * siny);
}
//----------------------------------------------------------------------------//

Quaternionn:: Quaternionn (const double new_q0,  
                         const double new_q1,
                         const double new_q2,  
                         const double new_q3)
{
   q0 = new_q0;
   q1 = new_q1;
   q2 = new_q2;
   q3 = new_q3;
}
//----------------------------------------------------------------------------//

Quaternionn:: Quaternionn (const Quaternionn& q)
{
   q0 = q.q0;
   q1 = q.q1;
   q2 = q.q2;
   q3 = q.q3;
}
//----------------------------------------------------------------------------//
// operators
//----------------------------------------------------------------------------//

Quaternionn& Quaternionn:: operator = (const Quaternionn& q_rhs)
{
   q0 = q_rhs.q0;
   q1 = q_rhs.q1;
   q2 = q_rhs.q2;
   q3 = q_rhs.q3;
   return *this;
}
//----------------------------------------------------------------------------//

Quaternionn Quaternionn:: operator + (const Quaternionn& q_rhs)
{
   static Quaternionn sum;

   sum.q0 = q0 + q_rhs.q0;
   sum.q1 = q1 + q_rhs.q1;
   sum.q2 = q2 + q_rhs.q2;
   sum.q3 = q3 + q_rhs.q3;
   return sum;
}
//----------------------------------------------------------------------------//

Quaternionn Quaternionn:: operator - (const Quaternionn& q_rhs)
{
   static Quaternionn difference;

   difference.q0 = q0 - q_rhs.q0;
   difference.q1 = q1 - q_rhs.q1;
   difference.q2 = q2 - q_rhs.q2;
   difference.q3 = q3 - q_rhs.q3;
   return difference;
}
//----------------------------------------------------------------------------//

Quaternionn Quaternionn:: operator * (const Quaternionn& q_rhs)
{
   static Quaternionn prod;
   prod.q0 = (q0 * q_rhs.q0) - (q1 * q_rhs.q1) -
             (q2 * q_rhs.q2) - (q3 * q_rhs.q3);
   prod.q1 = (q1 * q_rhs.q0) + (q0 * q_rhs.q1) -
             (q3 * q_rhs.q2) + (q2 * q_rhs.q3);
   prod.q2 = (q2 * q_rhs.q0) + (q3 * q_rhs.q1) -
             (q0 * q_rhs.q2) + (q1 * q_rhs.q3);
   prod.q3 = (q3 * q_rhs.q0) + (q2 * q_rhs.q1) -
             (q1 * q_rhs.q2) + (q0 * q_rhs.q3);
   return prod;
}
//----------------------------------------------------------------------------//

Quaternionn Quaternionn:: operator * (double scalar)
{
   static Quaternionn product;

   product.q0 = q0 * scalar;
   product.q1 = q1 * scalar;
   product.q2 = q2 * scalar;
   product.q3 = q3 * scalar;
   return product;
}
//----------------------------------------------------------------------------//

double& Quaternionn:: operator [] (int n)
{
   if (n == 0)
   {
      return q0;
   }
   if (n == 1)
   {
      return q1;
   }
   if (n == 2)
   {
      return q2;
   }
   if (n == 3)
   {
      return q3;
   }
   cout << "Warning: quaternion[" << n << "] is an invalid accessor"
        << " (only 0..3 allowed), returning value of 0.0" << endl;
   static double dummy_value = 0.0;
   return dummy_value;
}
//----------------------------------------------------------------------------//

ostream& operator << (ostream& out, Quaternionn& q)
{
   out << "[" << q.q0 << ", " << q.q1 << ", "
              << q.q2 << ", " << q.q3 << "]" ;
   return (out);
}
//----------------------------------------------------------------------------//
// inspection methods
//----------------------------------------------------------------------------//

void Quaternionn:: print ()    
{
   cout << "[" << q0 << ", " << q1 << ", "
               << q2 << ", " << q3 << "]" ;
}
//----------------------------------------------------------------------------//

double Quaternionn:: phi_value ()    // Euler angle roll

const
{
   return   acos (arcclamp ((q0*q0 - q1*q1 - q2*q2 + q3*q3)
                             / cos (theta_value ())))

          * sign  (q2*q3 + q0*q1);
}
//----------------------------------------------------------------------------//

double Quaternionn:: theta_value ()  // Euler angle pitch

const
{
   return asin (arcclamp (-2.0 * (q1*q3 - q0*q2)));
}
//----------------------------------------------------------------------------//

double Quaternionn:: psi_value ()    // Euler angle yaw

const
{
   return   acos (arcclamp ((q0*q0 + q1*q1 - q2*q2 - q3*q3)
                             / cos (theta_value ())))

          * sign  (q1*q2 + q0*q3);
}
//----------------------------------------------------------------------------//

double Quaternionn:: magnitude ()

const
{
   return sqrt((q0 * q0) + (q1 * q1) + (q2 * q2) + (q3 * q3));
}
//----------------------------------------------------------------------------//

Vector3D Quaternionn:: euler_angles ()    // Euler angle  phi,  theta, psi
                                         //             (roll, pitch, yaw)
                                         // more efficient than individual calls
const
{
   double qq0, qq1, qq2, qq3, phi, theta, psi, costheta; // locals hide methods

   qq0      = q0 * q0; // force optimization of squaring computations
   qq1      = q1 * q1;
   qq2      = q2 * q2;
   qq3      = q3 * q3;

   theta    = asin (arcclamp (-2.0 * (q1*q3 - q0*q2)));

   costheta = cos (theta);

   phi      =    acos (arcclamp ((qq0 - qq1 - qq2 + qq3) / costheta))

               * sign  (q2*q3 + q0*q1);

   psi      =    acos (arcclamp ((qq0 + qq1 - qq2 - qq3) / costheta))

               * sign  (q1*q2 + q0*q3);

   return Vector3D (phi, theta, psi);
}
//----------------------------------------------------------------------------//

Quaternionn Quaternionn:: conjugate ()   // negates vector part

const
{
   return Quaternionn (q0, - q1, - q2, - q3);
}
//----------------------------------------------------------------------------//

Quaternionn Quaternionn:: inverse ()   // negates vector part

const
{
   static Quaternionn qresult;

   qresult = conjugate ();
   qresult.normalize ();
   return qresult;
}
//----------------------------------------------------------------------------//
// modifying methods
//----------------------------------------------------------------------------//

void Quaternionn:: rotate (const double delta_phi,
                          const double delta_theta,
                          const double delta_psi  )
{
   Quaternionn rotation;

   rotation.q0= -0.5*((q1 * delta_phi)  +(q2 * delta_theta)+(q3 * delta_psi));
   rotation.q1=  0.5*((q0 * delta_phi)  +(q2 * delta_psi)  -(q3 * delta_theta));
   rotation.q2=  0.5*((q0 * delta_theta)+(q3 * delta_phi)  -(q1 * delta_psi));
   rotation.q3=  0.5*((q0 * delta_psi)  +(q1 * delta_theta)-(q2 * delta_phi));

   q0 += rotation.q0;
   q1 += rotation.q1;
   q2 += rotation.q2;
   q3 += rotation.q3;
   normalize ();
   return;
}
//----------------------------------------------------------------------------//

void Quaternionn:: incremental_rotate (const double P, const double Q,
                                      const double RR, const double delta_t)
{
   Quaternionn rotation;

   rotation.q0 = -0.5 * delta_t * ((q1 * P) + (q2 * Q) + (q3 * RR ));
   rotation.q1 =  0.5 * delta_t * ((q0 * P) + (q2 * RR) - (q3 * Q));
   rotation.q2 =  0.5 * delta_t * ((q0 * Q) + (q3 * P) - (q1 * RR));
   rotation.q3 =  0.5 * delta_t * ((q0 * RR) + (q1 * Q) - (q2 * P));

   q0 += rotation.q0;
   q1 += rotation.q1;
   q2 += rotation.q2;
   q3 += rotation.q3;
   normalize ();
   return;
}
//----------------------------------------------------------------------------//

void Quaternionn:: update (double P, double Q, double RR, double seconds)
{
   double     hh = seconds * 0.5;
   double     h6 = seconds / 6.0;
   Quaternionn y  = *this, dym, dyt, yt, dydx;

   dydx.q0 = -0.5 * ((q1 * P) + (q2 * Q) + (q3 * RR));
   dydx.q1 =  0.5 * ((q0 * P) + (q2 * RR) - (q3 * Q));
   dydx.q2 =  0.5 * ((q0 * Q) + (q3 * P) - (q1 * RR));
   dydx.q3 =  0.5 * ((q0 * RR) + (q1 * Q) - (q2 * P));

   yt.q0 = y.q0 + hh * dydx.q0;
   yt.q1 = y.q1 + hh * dydx.q1;
   yt.q2 = y.q2 + hh * dydx.q2;
   yt.q3 = y.q3 + hh * dydx.q3;

   dyt.q0 = -0.5 * ((yt.q1 * P) + (yt.q2 * Q) + (yt.q3 * RR));
   dyt.q1 =  0.5 * ((yt.q0 * P) + (yt.q2 * RR) - (yt.q3 * Q));
   dyt.q2 =  0.5 * ((yt.q0 * Q) + (yt.q3 * P) - (yt.q1 * RR));
   dyt.q3 =  0.5 * ((yt.q0 * RR) + (yt.q1 * Q) - (yt.q2 * P));

   yt.q0 = y.q0 + hh * dyt.q0;
   yt.q1 = y.q1 + hh * dyt.q1;
   yt.q2 = y.q2 + hh * dyt.q2;
   yt.q3 = y.q3 + hh * dyt.q3;

   dym.q0 = -0.5 * ((yt.q1 * P) + (yt.q2 * Q) + (yt.q3 * RR));
   dym.q1 =  0.5 * ((yt.q0 * P) + (yt.q2 * RR) - (yt.q3 * Q));
   dym.q2 =  0.5 * ((yt.q0 * Q) + (yt.q3 * P) - (yt.q1 * RR));
   dym.q3 =  0.5 * ((yt.q0 * RR) + (yt.q1 * Q) - (yt.q2 * P));

   yt.q0 = y.q0 + seconds * dym.q0;
   yt.q1 = y.q1 + seconds * dym.q1;
   yt.q2 = y.q2 + seconds * dym.q2;
   yt.q3 = y.q3 + seconds * dym.q3;

   dym.q0 = dym.q0 + dyt.q0;
   dym.q1 = dym.q1 + dyt.q1;
   dym.q2 = dym.q2 + dyt.q2;
   dym.q3 = dym.q3 + dyt.q3;

   dyt.q0 = -0.5 * ((yt.q1 * P) + (yt.q2 * Q) + (yt.q3 * RR));
   dyt.q1 =  0.5 * ((yt.q0 * P) + (yt.q2 * RR) - (yt.q3 * Q));
   dyt.q2 =  0.5 * ((yt.q0 * Q) + (yt.q3 * P) - (yt.q1 * RR));
   dyt.q3 =  0.5 * ((yt.q0 * RR) + (yt.q1 * Q) - (yt.q2 * P));

   q0 = y.q0 + h6 * (dydx.q0 + dyt.q0 + 2.0 * dym.q0);
   q1 = y.q1 + h6 * (dydx.q1 + dyt.q1 + 2.0 * dym.q1);
   q2 = y.q2 + h6 * (dydx.q2 + dyt.q2 + 2.0 * dym.q2);
   q3 = y.q3 + h6 * (dydx.q3 + dyt.q3 + 2.0 * dym.q3);

   normalize ();
}
//----------------------------------------------------------------------------//

void Quaternionn:: set (double phi, double theta, double psi, double twist)
{
   q0 =               cos (0.5 * twist);
   q1 = cos (phi)   * sin (0.5 * twist);
   q2 = cos (theta) * sin (0.5 * twist);
   q3 = cos (psi)   * sin (0.5 * twist);
}
//----------------------------------------------------------------------------//

void Quaternionn:: normalize()
{
double m = magnitude ();
if (m > 0.0)
{
    q0 /= m;
    q1 /= m;
    q2 /= m;
    q3 /= m;
}
}
//----------------------------------------------------------------------------//

////////////////////////////////////////////////////////////////////////////////





