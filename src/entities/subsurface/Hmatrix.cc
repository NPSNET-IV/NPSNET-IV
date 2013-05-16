////////////////////////////////////////////////////////////////////////////////
/*
 Program:         Hmatrix.C

 Author:          Don Brutzman

 Revised:         12 October 94

 System:          Iris

 Compiler:        ANSI C++

 Compilation:     irix> make dynatest
                  irix> CC Hmatrix.C -lm -c -g +w 

                   -c == Produce binaries only, suppressing the link phase.
                   +w == Warn about all questionable constructs.

 Description:     Homogenous transform matrix class specification
                  All angle parameter values are in radians

                  rotations    are in world coordinate system
                  translations are in world coordinate system

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

#include "Hmatrix.h"

// rotation matrix conventions:  Cooke et al. Figure 10

// rotation matrix
#define a1     htmatrix[0][0]
#define a2     htmatrix[1][0]
#define a3     htmatrix[2][0]
#define b1     htmatrix[0][1]
#define b2     htmatrix[1][1]
#define b3     htmatrix[2][1]
#define c1     htmatrix[0][2]
#define c2     htmatrix[1][2]
#define c3     htmatrix[2][2]

// position translation vector
#define p_x    htmatrix[0][3]
#define p_y    htmatrix[1][3]
#define p_z    htmatrix[2][3]

// camera perspective transformation
#define c_x    htmatrix[3][0]
#define c_y    htmatrix[3][1]
#define c_z    htmatrix[3][2]

// global scale coefficient
#define hscale htmatrix[3][3]


//----------------------------------------------------------------------------//
// constructor methods
//----------------------------------------------------------------------------//

Hmatrix:: Hmatrix () // default constructor
{
   set_identity ();
}
//----------------------------------------------------------------------------//

Hmatrix:: Hmatrix (const double       x,
                   const double       y,       
                   const double       z,     
                   const double     phi,   
                   const double   theta,
                   const double     psi) // constructor
{
   set_identity ();
   rotate       (phi, theta, psi);
   translate    (Vector3D (x, y, z));
}
//----------------------------------------------------------------------------//

Hmatrix:: Hmatrix (const Vector3D   position_Vector3D,
                   const double     phi, 
                   const double   theta,
                   const double     psi)
{
   set_identity ();
   rotate       (phi, theta, psi);
   translate    (position_Vector3D);
}
//----------------------------------------------------------------------------//

Hmatrix:: Hmatrix (const Vector3D position_Vector3D,
                   const Vector3D eulerangles3D)
{
   set_identity ();
   rotate       (eulerangles3D);
   translate    (position_Vector3D);
}
//----------------------------------------------------------------------------//

Hmatrix:: Hmatrix (const Vector3D position_Vector3D, const Quaternionn posture)
{
   set_identity ();
   rotate       (posture.euler_angles ());
   translate    (position_Vector3D);
}
//----------------------------------------------------------------------------//

Hmatrix:: Hmatrix (const Hmatrix& input_hmatrix)
{
   for (int row = 0; row <= 3; row++)
   {
       for (int column = 0; column <= 3; column++)
       {
           htmatrix [row][column] = input_hmatrix.element (row+1, column+1);
       }
   }
}
//----------------------------------------------------------------------------//
// operators
//----------------------------------------------------------------------------//

Hmatrix& Hmatrix:: operator = (const Hmatrix &h_rhs)       // assignment
{
   for (int row = 0; row <= 3; row++)
   {
       for (int column = 0; column <= 3; column++)
       {
           htmatrix [row][column] = h_rhs.htmatrix [row][column];
       }
   }
   return *this;
}
//----------------------------------------------------------------------------//

Hmatrix& Hmatrix:: operator * (const Hmatrix &h_rhs)   // Hmatrix multiplication
{
   static Hmatrix hresult = Hmatrix ();

   int row, column, index;

   for (row = 0; row <= 3; row++)
   {
       for (column = 0; column <= 3; column++)
       {
           hresult.htmatrix [row][column] = 0.0;
       }
   }

   for (row = 0; row <= 3; row++)
   {
       for (column = 0; column <= 3; column++)
       {
           for (index = 0; index <= 3; index++)
           {
               hresult.htmatrix [row][column] = hresult.htmatrix [row][column] +
                       htmatrix [row][index]  * h_rhs.htmatrix [index][column];
           }
       }
   }
   return hresult;
}
//----------------------------------------------------------------------------//

Hmatrix& Hmatrix:: operator * (const Vector3D &v_rhs)      // Vector3D multiply
{
   static Hmatrix hresult = Hmatrix ();

   int    row, column;
   double position_vector [4];

   position_vector [0] = v_rhs [1];
   position_vector [1] = v_rhs [2];
   position_vector [2] = v_rhs [3];
   position_vector [3] = 1.0;

   for (row = 0; row <= 3; row++)
   {
       for (column = 0; column <= 3; column++)
       {
           hresult.htmatrix [row][column] = 0.0;
       }
   }

   for (row = 0; row <= 3; row++)
   {
       for (column = 0; column <= 3; column++)
       {
           hresult.htmatrix [row][column] =
                   htmatrix [row][column] * position_vector [column];
       }
   }
   return hresult;
}
//----------------------------------------------------------------------------//

ostream& operator << (ostream& out, Hmatrix& h)
{
   int row;

   for (row = 0; row <= 3; row++)
   {
       out  << endl << "["
            << h.htmatrix [row][0] << ", " << h.htmatrix [row][1] << ", "
            << h.htmatrix [row][2] << ", " << h.htmatrix [row][3] << "]";
   }
   out << endl;
   return (out);
}
//----------------------------------------------------------------------------//
// inspection methods
//----------------------------------------------------------------------------//

void Hmatrix:: print_hmatrix ()
const
{
   int row;

   for (row = 0; row <= 3; row++)
   {
       cout << endl << "["
            << htmatrix [row][0] << ", " << htmatrix [row][1] << ", "
            << htmatrix [row][2] << ", " << htmatrix [row][3] << "]";
   }
   cout << endl;
}
//----------------------------------------------------------------------------//

double Hmatrix:: phi_value ()                                   // Cooke (8.17)
const
{
   double theta  = theta_value ();
   double result;

   if ((theta == radians ( 90.0)) || (theta == radians (-90.0)))
   {
      cout << "phi_value () warning: theta == " << theta << endl;
   }

   result = acos (arcclamp (c3 / cos (theta))) * sign (b3);
   return result;
}

//----------------------------------------------------------------------------//

double Hmatrix:: theta_value ()                                 // Cooke (8.14)
const
{
   return asin (arcclamp (-a3));
}

//----------------------------------------------------------------------------//

double Hmatrix:: psi_value ()                                   // Cooke (8.16)
const
{
   double theta  = theta_value ();
   double result;

   if ((theta == radians ( 90.0)) || (theta == radians (-90.0)))
   {
      cout << "psi_value () warning: theta == " << theta << endl;
   }

   result = acos (arcclamp (a1 / cos (theta_value ()))) * sign (a2);
   return result;
}

//----------------------------------------------------------------------------//

double Hmatrix:: x_value ()
const
{
   return p_x;
}

//----------------------------------------------------------------------------//

double Hmatrix:: y_value ()
const
{
   return p_y;
}

//----------------------------------------------------------------------------//

double Hmatrix:: z_value ()
const
{
   return p_z;
}

//----------------------------------------------------------------------------//

Quaternionn Hmatrix:: quaternion_value ()
const
{
   Quaternionn qresult = Quaternionn ();
/*
   double     q0, q1, q2, q3;

   q0 = (0.5) * sqrt (a1 + b2 + c3 + 1);
   q1 = 0.0;
   q2 = 0.0;
   q3 = 0.0;

   // <<<<<<<< Shoemake p. 253, FUNDA or p.41 cooke <<<<<<<<<<
*/
   return qresult;
}

//----------------------------------------------------------------------------//

Vector3D Hmatrix:: position ()
const
{
   return Vector3D (p_x, p_y, p_z);
}

//----------------------------------------------------------------------------//

Vector3D Hmatrix:: camera ()
const
{
   return Vector3D (c_x, c_y, c_z);
}

//----------------------------------------------------------------------------//

double Hmatrix:: scale ()
const
{
   return hscale;
}

//----------------------------------------------------------------------------//

double Hmatrix:: element (const int row,       // accessor returns value
                          const int column)    // with row/column [1..4]
const
{
   if ((row < 1) || (row > 4) || (column < 1) || (column > 4))
   {
      cout << "Warning: Hmatrix.element (" << row << ", " << column <<
              ") is an invalid accessor (only 1..4 allowed), "      <<
              " returning value of 0.0" << endl;

      static double dummy_value = 0.0;
      return        dummy_value;
   }

   return htmatrix [row-1][column-1];
}
//----------------------------------------------------------------------------//
// modifying methods
//----------------------------------------------------------------------------//

void Hmatrix:: rotate (const double phi, const double theta, const double psi)
{
   double hrotation [3][3];
   double hresult   [3][3];

   int row, column, index;    // reference:  Cooke et al. Figure 10

   double  sinphi   = sin (phi);
   double  cosphi   = cos (phi);
   double  sintheta = sin (theta);
   double  costheta = cos (theta);
   double  sinpsi   = sin (psi);
   double  cospsi   = cos (psi);

   hrotation [0][0] =   costheta * cospsi;                              // a1
   hrotation [1][0] =   costheta * sinpsi;                              // a2
   hrotation [2][0] = - sintheta;                                       // a3
   hrotation [0][1] =   sinphi   * sintheta * cospsi - cosphi * sinpsi; // b1
   hrotation [1][1] =   sinphi   * sintheta * sinpsi + cosphi * cospsi; // b2
   hrotation [2][1] =   sinphi   * costheta;                            // b3
   hrotation [0][2] =   cosphi   * sintheta * cospsi + sinphi * sinpsi; // c1
   hrotation [1][2] =   cosphi   * sintheta * sinpsi - sinphi * cospsi; // c2
   hrotation [2][2] =   cosphi   * costheta;                            // c3

   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           hresult [row][column] = 0.0; // zero accumulators first

           for (index = 0; index <= 2; index++)
           {
               hresult [row][column] = hresult [row][column] +
                              htmatrix [row][index] * hrotation [index][column];
           }
       }
   }
   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           htmatrix [row][column] = hresult [row][column];
       }
   }
}
//----------------------------------------------------------------------------//

void Hmatrix:: rotate (const Vector3D rotation3D)
{
     rotate (rotation3D [1], rotation3D [2],rotation3D [3]);  // note not 0 1 2
}
//----------------------------------------------------------------------------//

void Hmatrix:: rotate_x (const double phi)
{
   double hrotation [3][3];
   double hresult   [3][3];

   int row, column, index;

   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           if  (row == column) hrotation [row][column] = 1.0;
           else                hrotation [row][column] = 0.0;
       }
   }
   hrotation [1][1] =   cos (phi);
   hrotation [2][2] =   cos (phi);
   hrotation [2][1] =   sin (phi);
   hrotation [1][2] = - sin (phi);

   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           hresult [row][column] = 0.0; // zero accumulators first

           for (index = 0; index <= 2; index++)
           {
               hresult [row][column] = hresult [row][column] +
                              htmatrix [row][index] * hrotation [index][column];
           }
       }
   }
   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           htmatrix [row][column] = hresult [row][column];
       }
   }
}
//----------------------------------------------------------------------------//

void Hmatrix:: rotate_y (const double theta)
{
   double hrotation [3][3];
   double hresult   [3][3];

   int row, column, index;

   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           if  (row == column) hrotation [row][column] = 1.0;
           else                hrotation [row][column] = 0.0;
       }
   }
   hrotation [0][0] =   cos (theta);
   hrotation [2][2] =   cos (theta);
   hrotation [2][0] = - sin (theta);
   hrotation [0][2] =   sin (theta);

   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           hresult [row][column] = 0.0; // zero accumulators first

           for (index = 0; index <= 2; index++)
           {
               hresult [row][column] = hresult [row][column] +
                              htmatrix [row][index] * hrotation [index][column];
           }
       }
   }
   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           htmatrix [row][column] = hresult [row][column];
       }
   }
}
//----------------------------------------------------------------------------//

void Hmatrix:: rotate_z (const double psi)
{
   double hrotation [3][3];
   double hresult   [3][3];

   int row, column, index;

   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           if  (row == column) hrotation [row][column] = 1.0;
           else                hrotation [row][column] = 0.0;
       }
   }
   hrotation [0][0] =   cos (psi);
   hrotation [1][1] =   cos (psi);
   hrotation [1][0] =   sin (psi);
   hrotation [0][1] = - sin (psi);

   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           hresult [row][column] = 0.0; // zero accumulators first

           for (index = 0; index <= 2; index++)
           {
               hresult [row][column] = hresult [row][column] +
                              htmatrix [row][index] * hrotation [index][column];
           }
       }
   }
   for (row = 0; row <= 2; row++)
   {
       for (column = 0; column <= 2; column++)
       {
           htmatrix [row][column] = hresult [row][column];
       }
   }
}
//----------------------------------------------------------------------------//

void Hmatrix:: incremental_rotation (const double   phi_dot,   
                                     const double theta_dot,
                                     const double   psi_dot,   
                                     const double delta_t  )
{
     rotate (phi_dot * delta_t, theta_dot * delta_t, psi_dot * delta_t);
}
//----------------------------------------------------------------------------//

void Hmatrix:: translate (const double delta_x,
                          const double delta_y,
                          const double delta_z)
{
    p_x += delta_x;
    p_y += delta_y;
    p_z += delta_z;
}
//----------------------------------------------------------------------------//

void Hmatrix:: translate (const Vector3D translation3D)
{
    p_x += translation3D [1];
    p_y += translation3D [2];
    p_z += translation3D [3];
}
//----------------------------------------------------------------------------//

void Hmatrix:: incremental_translation (const double x_dot,
                                        const double y_dot,
                                        const double z_dot,
                                        const double delta_t)
{
    p_x += x_dot * delta_t;
    p_y += y_dot * delta_t;
    p_z += z_dot * delta_t;
}
//----------------------------------------------------------------------------//

void Hmatrix:: incremental_translation (const Vector3D velocities3D,
                                        const double   delta_t)
{
    p_x += velocities3D [1] * delta_t;
    p_y += velocities3D [2] * delta_t;
    p_z += velocities3D [3] * delta_t;
}
//----------------------------------------------------------------------------//

void Hmatrix:: set_identity () // default constructorset to identity matrix
{
   for (int row = 0; row <= 3; row++)
   {
       for (int column = 0; column <= 3; column++)
       {
         if   (row == column) htmatrix [row][column] = 1.0;
         else                 htmatrix [row][column] = 0.0;
       }
   }
}
//----------------------------------------------------------------------------//

void Hmatrix:: set_posture (const double phi, 
                            const double theta,
                            const double psi)
{
     set_identity ();
     rotate (phi, theta, psi);
}
//----------------------------------------------------------------------------//

void Hmatrix:: set_position  ( Vector3D translation3D)
{
     p_x = translation3D [1];
     p_y = translation3D [2];
     p_z = translation3D [3];
}
//----------------------------------------------------------------------------//

void Hmatrix:: set_camera    (const Vector3D translation3D)
{
     c_x = translation3D [1];
     c_y = translation3D [2];
     c_z = translation3D [3];
}
//----------------------------------------------------------------------------//

void Hmatrix:: move_camera    (const Vector3D translation3D)
{
     c_x += translation3D [1];
     c_y += translation3D [2];
     c_z += translation3D [3];
}
//----------------------------------------------------------------------------//

void Hmatrix:: set_all_scales (const double scale_x, 
                               const double scale_y,
                               const double scale_z, 
                               const double scale_global)
{
     a1     *= scale_x;
     b2     *= scale_y;
     c3     *= scale_z;
     hscale *= scale_global;
}
//----------------------------------------------------------------------------//

void Hmatrix:: set_scale (const double scale_global)
{
     hscale *= scale_global;
}
////////////////////////////////////////////////////////////////////////////////
// restore defines

#undef a1
#undef a2
#undef a3
#undef b1
#undef b2
#undef b3
#undef c1
#undef c2
#undef c3
#undef p_x
#undef p_y
#undef p_z
#undef c_x
#undef c_y
#undef c_z
#undef hscale

////////////////////////////////////////////////////////////////////////////////






