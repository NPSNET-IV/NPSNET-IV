////////////////////////////////////////////////////////////////////////////////
/*
 Program:           UUVBody.C

 Description:       Six degree-of-freedom underwater vehicle hydrodynamics 
                    based on Healey model 

 Revised:           28 October 94

 System:            Irix 5.2

 Compiler:          ANSI C++

 Compilation:       irix> make dynamics
                    irix> CC UUVBody.C -lm -c -g +w 
                    
                    -c == Produce binaries only, suppressing the link phase.
                    +w == Warn about all questionable constructs.

 Advisors:          Dr. Mike Zyda, Dr. Bob McGhee and Dr. Tony Healey

 Author:            Don Brutzman                    brutzman@cs.nps.navy.mil
                    Code OR/Br
                    Naval Postgraduate School       (408) 656-2149 work
                    Monterey CA 93943-5000          (408) 656-2595 fax

 References:        Healey, A.J. and Lienard, D., "Multivariable Sliding Mode
                    Control for Autonomous DIving and Steering of Unmanned
                    Underwater Vehicles," IEEE Journal of Oceanic Engineering,
                    vol. 18 no. 3, July 1993, pp. 327-339.

                    Yuh, J., "Modeling and Control of Underwater Robotic
                    Vehicle," IEEE Transactions on Systems, Man and Cybernetics,
                    vol. 20 no. 6, November/December 1990, pp. 1475-1483.

                    Press, William H., Teukolsky, Saul A., Vetterling,  
                    William T. and Flannery, Brian P., "Numerical Recipes in C,"
                    second edition, Cambridge University Press, Cambridge 
                    England, 1992.

                    Marco, David.  "Slow Speed Control and Dynamic Positioning
                    of an Autonomous Vehicle," Ph.D. dissertation,  
                    Naval Postgraduate School, Monterey California, March 1995.

                    Fossen, Thor I., _Guidance and Control of Ocean Vehicles_,
                    John Wiley and Sons, Chichester England, 1994.


 Status:            Equations of motion tested satisfactorily, 
                    verification against in-water tests remains.
                    Move utilities to math_utilities.c 


 Future work:       Comments and suggestions are welcome!


*/

////////////////////////////////////////////////////////////////////////////////
#include "UUVmodel.h"

const char * DISCLIENT = "~/DIS.mcast/src/client -r &";
double  revisedBuoyancy, revised_x_B;
double  surface_length = 0.0;  // distances (CB to surface) & (CB to nose)
double  nose_length    = 0.0;  // along body axis
double xx [15] = {
   -43.9/12.0,
   -39.2/12.0,
   -35.2/12.0,
   -31.2/12.0,
   -27.2/12.0,
   -10.0/12.0,
    0.0/12.0,
    10.0/12.0,
    26.8/12.0,
    32.0/12.0,
    37.8/12.0,
    40.8/12.0,
    42.3/12.0,
    43.3/12.0,
    43.7/12.0};

double hh [15] = {
   0.0/12.0,
   2.7/12.0,
   5.2/12.0,
   7.6/12.0,
   10.1/12.0,
   10.1/12.0,
   10.1/12.0,
   10.1/12.0,
   10.1/12.0,
   9.6/12.0,
   7.6/12.0,
   5.6/12.0,
   4.2/12.0,
   2.3/12.0,
   0.0/12.0};

double bb [15] = {
   16.5/12.0,
   16.5/12.0,
   16.5/12.0,
   16.5/12.0,
   16.5/12.0,
   16.5/12.0,
   16.5/12.0,
   16.5/12.0,
   16.5/12.0,
   15.5/12.0,
   12.4/12.0,
   9.5/12.0,
   7.0/12.0,
   4.0/12.0,
   0.0/12.0};

//----------------------------------------------------------------------------//
// constructor methods
//----------------------------------------------------------------------------//

UUVBody:: UUVBody () // default constructor
{
   Hprevious = Hmatrix ();

   RigidBody_initialize ();                      // inherited method
  
   current_uuv_time = 0.0;

   U                = 0.0;      
   V                = 0.0;     
   W                = 0.0;
   P                = 0.0;
   Q                = 0.0;
   RR                = 0.0;

   u_dot            = 0.0;
   v_dot            = 0.0;
   w_dot            = 0.0;
   p_dot            = 0.0;
   q_dot            = 0.0;
   r_dot            = 0.0;

   x_dot            = 0.0;
   y_dot            = 0.0;
   z_dot            = 0.0;
   phi_dot          = 0.0;
   theta_dot        = 0.0;
   psi_dot          = 0.0;

  

   for (int index = 0; index <= 5; index++) rhs [index] = 0.0;

   calculate_mass_matrix ();
   invert_mass_matrix    ();


// AUV-provided state variables - - - - - - - - - - - - - - - - - - - - - - - - 

  AUV_time           = 0.0; // mission time
  AUV_delta_rudder   = 0.0; // positive is bow rudder to starboard
  AUV_delta_planes   = 0.0; // positive is bow planes to starboard
 AUV_port_rpm       = 0.0; // propellor revolutions per minute
 AUV_stbd_rpm       = 0.0; // propellor revolutions per minute

 AUV_bow_vertical   = 0.0; // thruster volts 24V = 3820 rpm no load
 AUV_stern_vertical = 0.0; // thruster volts 24V = 3820 rpm no load
 AUV_bow_lateral    = 0.0; // thruster volts 24V = 3820 rpm no load
 AUV_stern_lateral  = 0.0; // thruster volts 24V = 3820 rpm no load

 AUV_depth_cell     = 0.0; // pressure sensor, units are psid
 AUV_heading        = 0.0; // gyrocompass in degrees
 AUV_roll_angle     = 0.0; // matches intertial sensor onboard AUV
 AUV_pitch_angle    = 0.0; // matches intertial sensor onboard AUV 
 AUV_roll_rate      = 0.0; // matches intertial sensor onboard AUV 
 AUV_pitch_rate     = 0.0; // matches intertial sensor onboard AUV 
 AUV_yaw_rate       = 0.0; // matches intertial sensor onboard AUV 

/*static int    AUV_hour           = 0;   // internal AUV OS-9 system time, unused
static int    AUV_minute         = 0;  
static int    AUV_second         = 0;  
*/


// Hydrodynamics-model-provided state variables - - - - - - - - - - - - - - - - 

 AUV_x              =7883.0; // x    position in world coordinates
 AUV_y              =7073.0; // y    position in world coordinates
 AUV_z              =20.0; // z    position in world coordinates
 AUV_phi            = 0.0; // roll  posture in world coordinates
 AUV_theta          = 0.0; // pitch posture in world coordinates
 AUV_psi            = 0.0; // yaw   posture in world coordinates

 AUV_x_dot          = 0.0; //      Euler velocity along North-axis
 AUV_y_dot          = 0.0; //      Euler velocity along  East-axis
 AUV_z_dot          = 0.0; //      Euler velocity along Depth-axis
 AUV_phi_dot        = 0.0; // Euler rotation rate about North-axis
 AUV_theta_dot      = 0.0; // Euler rotation rate about  East-axis
 AUV_psi_dot        = 0.0; // Euler rotation rate about Depth-axis

 AUV_u              = 0.0; // surge  linear velocity along x-axis
 AUV_v              = 0.0; // sway   linear velocity along y-axis
 AUV_w              = 0.0; // heave  linear velocity along x-axis
 AUV_p              = 0.0; // roll  angular velocity about x-axis
 AUV_q              = 0.0; // pitch angular velocity about y-axis
 AUV_r              = 0.0; // yaw   angular velocity about z-axis

 AUV_u_dot          = 0.0; //    linear acceleration along x-axis
 AUV_v_dot          = 0.0; //    linear acceleration along y-axis
 AUV_w_dot          = 0.0; //    linear acceleration along x-axis
 AUV_p_dot          = 0.0; //   angular acceleration about x-axis
 AUV_q_dot          = 0.0; //   angular acceleration about y-axis
 AUV_r_dot          = 0.0; //   angular acceleration about z-axis

 AUV_oceancurrent_u = 0.0; //  Ocean current rate along North-axis
 AUV_oceancurrent_v = 0.0; //  Ocean current rate along  East-axis
 AUV_oceancurrent_w = 0.0; //  Ocean current rate along Depth-axis


// Prior time loop saved variables - - - - - - - - - - - - - - - - - - - - - - - 

 AUV_time_prior     = 0.0; // mission time

 AUV_x_prior        = 0.0; // x    position in world coordinates
 AUV_y_prior        = 0.0; // y    position in world coordinates
 AUV_z_prior        = 0.0; // z    position in world coordinates

 AUV_phi_prior      = 0.0; // roll  posture in world coordinates
 AUV_theta_prior    = 0.0; // pitch posture in world coordinates
 AUV_psi_prior      = 0.0; // yaw   posture in world coordinates


// Sonar model provided state variables  - - - - - - - - - - - - - - - - - - - -

 AUV_ST1000_bearing = 0.0; // ST_1000 conical pencil beam bearing
 AUV_ST1000_range   = 0.0; // ST_1000 conical pencil beam range
 AUV_ST1000_strength= 0.0; // ST_1000 conical pencil beam strength


 AUV_ST725_bearing  = 0.0; // ST_725  1 x 24  sector beam bearing
 AUV_ST725_range    = 0.0; // ST_725  1 x 24  sector beam range
 AUV_ST725_strength = 0.0; // ST_725  1 x 24  sector beam strength

 AUV_ST725_x_offset  = 3.5; // ST_725  longitudinal location in feet
 AUV_ST1000_x_offset  = 3.5; // ST_1000 longitudinal location in feet
TRACE = 0;

Vector3D vector1 = Vector3D (AUV_x, AUV_y, AUV_z);

              hmatrix = Hmatrix (vector1,
                                 radians (AUV_phi), 
                                 radians (AUV_theta),  
                                 radians (AUV_psi));
              hmatrix.print_hmatrix ();

              Hprevious = hmatrix;
}
//----------------------------------------------------------------------------//
// operators
//----------------------------------------------------------------------------//

ostream& operator << (ostream& out, UUVBody& uuvb)
{
   int row, col;

   uuvb.print_rigidbody ();
   out  << "current_uuv_time = "  << uuvb.current_uuv_time << endl; 
   out  << "<u_dot, v_dot, w_dot, p_dot, q_dot, r_dot> = " << "<"
        << uuvb.u_dot << ", " << uuvb.v_dot << ", " << uuvb.w_dot << ", "
        << uuvb.p_dot << ", " << uuvb.q_dot << ", " << uuvb.r_dot << ">" 
        << endl;

   out  << "Mass matrix:" << endl;
   for (row = 0; row <= 5; row ++)
   {
        out << "[";
        for (col = 0; col <= 5; col ++)
        {
             out << uuvb.mass [row][col];
             if   (col < 5) out << ", ";
             else           out << "]" << endl;
        }
   }
   out  << "Mass inverse matrix:" << endl;
   for (row = 0; row <= 5; row ++)
   {
        out << "[";
        for (col = 0; col <= 5; col ++)
        {
             out << uuvb.mass_inverse [row][col];
             if   (col < 5) out << ", ";
             else           out << "]" << endl;
        }
   }
   return (out);
}
//----------------------------------------------------------------------------//

void UUVBody:: multiply6x6_6 (double   left_matrix   [6][6],
                              double   right_matrix  [6],
                              double   result_matrix [6])
{
   int row, col;
   
   for (row = 0; row <= 5; row++)
   {
       result_matrix [row] = 0.0;

       for (col = 0; col <= 5; col++)
       {
               result_matrix [row] = result_matrix [row] +
                                    left_matrix [row][col] * right_matrix [row];
       }
   }
}
//----------------------------------------------------------------------------//

void UUVBody:: multiply6x6_6x6 (double   left_matrix   [6][6],
                                double   right_matrix  [6][6],
                                double   result_matrix [6][6])
{
   int row, col, index;
   
   for (row = 0; row <= 5; row++)
   {
       for (col = 0; col <= 5; col++)
       {
           result_matrix [row][col] = 0.0;

           for (index = 0; index <= 5; index++)
           {
               result_matrix [row][col] = result_matrix [row][col] +

                           left_matrix [row][index] * right_matrix [index][col];
           }
       }
   }
}
//----------------------------------------------------------------------------//
// inspection methods
//----------------------------------------------------------------------------//

void UUVBody:: print_uuvbody () 
{
   print_rigidbody ();
   cout << "current_uuv_time   = "  << current_uuv_time << endl; 

   cout << "<U, U, W, P, Q, RR> body = " << "<"
        << U << ", " << V << ", " << W << ", "
        << P << ", " << Q << ", " << RR << ">" << endl;

   cout << "<u_dot, v_dot, w_dot, p_dot, q_dot, r_dot> body = " << "<"
        << u_dot << ", " << v_dot << ", " << w_dot << ", "
        << p_dot << ", " << q_dot << ", " << r_dot << ">" << endl;

   Vector3D euler_position_rates =  Vector3D (x_dot, y_dot, z_dot);
   cout  << "<x_dot,   y_dot,     z_dot>   = " << euler_position_rates << endl;

   Vector3D euler_angular_rates =  Vector3D (phi_dot, theta_dot, psi_dot);
   cout  << "<phi_dot, theta_dot, psi_dot> = " << euler_angular_rates  << endl;

   cout  << "Mass matrix:" << endl;
   print_matrix6x6 (mass); 

   cout  << "Mass inverse matrix:" << endl;
   print_matrix6x6 (mass_inverse);

   cout  << "Right-hand side (RHS) of equations of motion:  ";
   print_matrix6 (rhs);
}
//----------------------------------------------------------------------------//

void UUVBody:: print_matrix6 (double output_matrix [6])
{
   int row;

   cout << "[";
   for (row = 0; row <= 5; row ++)
   {
       cout << output_matrix [row];
       if (row < 5) cout << ", ";
   }
   cout << "]" << endl;
}
//----------------------------------------------------------------------------//

void UUVBody:: print_matrix6x6 (double output_matrix [6][6])
{
   int row, col;

   for (row = 0; row <= 5; row ++)
   {
        cout << "[";
        for (col = 0; col <= 5; col ++)
        {
             cout << output_matrix [row][col];
             if   (col < 5) cout << ", ";
             else           cout << "]" << endl;
        }
   }
}
//----------------------------------------------------------------------------//

double UUVBody:: current_uuv_time_value ()
const
{
   return current_uuv_time;
}
//----------------------------------------------------------------------------//

double UUVBody:: u_dot_value ()
const
{
   return u_dot;
}
//----------------------------------------------------------------------------//

double UUVBody:: v_dot_value ()
const
{
   return v_dot;
}
//----------------------------------------------------------------------------//

double UUVBody:: w_dot_value ()
const
{
   return w_dot;
}
//----------------------------------------------------------------------------//

double UUVBody:: p_dot_value ()
const
{
   return p_dot;
}
//----------------------------------------------------------------------------//

double UUVBody:: q_dot_value ()
const
{
   return q_dot;
}
//----------------------------------------------------------------------------//

double UUVBody:: r_dot_value ()
const
{
   return r_dot;
}
//----------------------------------------------------------------------------//

double UUVBody:: x_dot_value ()
const
{
   return x_dot;
}
//----------------------------------------------------------------------------//

double UUVBody:: y_dot_value ()
const
{
   return y_dot;
}
//----------------------------------------------------------------------------//

double UUVBody:: z_dot_value ()
const
{
   return z_dot;
}
//----------------------------------------------------------------------------//
// modifying methods
//----------------------------------------------------------------------------//

void UUVBody:: UUVBody_initialize ()
{
   RigidBody_initialize ();                      // inherited method
   

   current_uuv_time = 0.0;

   U                = 0.0;      
   V                = 0.0;     
   W                = 0.0;
   P                = 0.0;
   Q                = 0.0;
   RR                = 0.0;

   u_dot            = 0.0;
   v_dot            = 0.0;
   w_dot            = 0.0;
   p_dot            = 0.0;
   q_dot            = 0.0;
   r_dot            = 0.0;

   x_dot            = 0.0;
   y_dot            = 0.0;
   z_dot            = 0.0;
   phi_dot          = 0.0;
   theta_dot        = 0.0;
   psi_dot          = 0.0;

   for (int index = 0; index <= 5; index++) rhs [index] = 0.0;

   calculate_mass_matrix ();
   invert_mass_matrix    ();

   AUV_ST1000_bearing = 0.0; // ST_1000 conical pencil beam bearing
   AUV_ST1000_range   = 0.0; // ST_1000 conical pencil beam range
   AUV_ST1000_strength= 0.0; // ST_1000 conical pencil beam strength

   AUV_ST725_bearing  = 0.0; // ST_725  1 x 24  sector beam bearing
   AUV_ST725_range    = 0.0; // ST_725  1 x 24  sector beam range
   AUV_ST725_strength = 0.0; // ST_725  1 x 24  sector beam strength

   AUV_bow_vertical   = 0.0; // thruster volts 24V = 3820 rpm no load
   AUV_stern_vertical = 0.0; // thruster volts 24V = 3820 rpm no load
   AUV_bow_lateral    = 0.0; // thruster volts 24V = 3820 rpm no load
   AUV_stern_lateral  = 0.0; // thruster volts 24V = 3820 rpm no load

   AUV_delta_rudder   = 0.0; // positive is bow rudder to starboard
   AUV_delta_planes   = 0.0; // positive is bow planes to starboard
   AUV_port_rpm       = 0.0; // propellor revolutions per minute
   AUV_stbd_rpm       = 0.0; // propellor revolutions per minute

}
//----------------------------------------------------------------------------//
//NEW
void UUVBody:: UUVBody_initialize (float Xposit, float Yposit, float Zposit)
{
   RigidBody_initialize ();                      // inherited method
   

   current_uuv_time = 0.0;

   U                = 0.0;      
   V                = 0.0;     
   W                = 0.0;
   P                = 0.0;
   Q                = 0.0;
   RR                = 0.0;

   u_dot            = 0.0;
   v_dot            = 0.0;
   w_dot            = 0.0;
   p_dot            = 0.0;
   q_dot            = 0.0;
   r_dot            = 0.0;

   x_dot            = 0.0;
   y_dot            = 0.0;
   z_dot            = 0.0;
   phi_dot          = 0.0;
   theta_dot        = 0.0;
   psi_dot          = 0.0;

   for (int index = 0; index <= 5; index++) rhs [index] = 0.0;

   calculate_mass_matrix ();
   invert_mass_matrix    ();

   AUV_x              = Xposit; // x    position in world coordinates
   AUV_y              = Yposit; // y    position in world coordinates
   AUV_z              = Zposit; // z    position in world coordinates

   AUV_ST1000_bearing = 0.0; // ST_1000 conical pencil beam bearing
   AUV_ST1000_range   = 0.0; // ST_1000 conical pencil beam range
   AUV_ST1000_strength= 0.0; // ST_1000 conical pencil beam strength

   AUV_ST725_bearing  = 0.0; // ST_725  1 x 24  sector beam bearing
   AUV_ST725_range    = 0.0; // ST_725  1 x 24  sector beam range
   AUV_ST725_strength = 0.0; // ST_725  1 x 24  sector beam strength

   AUV_bow_vertical   = 0.0; // thruster volts 24V = 3820 rpm no load
   AUV_stern_vertical = 0.0; // thruster volts 24V = 3820 rpm no load
   AUV_bow_lateral    = 0.0; // thruster volts 24V = 3820 rpm no load
   AUV_stern_lateral  = 0.0; // thruster volts 24V = 3820 rpm no load

   AUV_delta_rudder   = 0.0; // positive is bow rudder to starboard
   AUV_delta_planes   = 0.0; // positive is bow planes to starboard
   AUV_port_rpm       = 0.0; // propellor revolutions per minute
   AUV_stbd_rpm       = 0.0; // propellor revolutions per minute

}
//----------------------------------------------------------------------------//

void UUVBody:: set_current_uuv_time (double new_current_uuv_time)
{
   current_uuv_time = new_current_uuv_time;
}
//----------------------------------------------------------------------------//

void UUVBody:: set_accelerations          (double new_u_dot, double new_v_dot,
                                           double new_w_dot, double new_p_dot,
                                           double new_q_dot, double new_r_dot)
{
   u_dot = new_u_dot;
   v_dot = new_v_dot;
   w_dot = new_w_dot;
   p_dot = new_p_dot;
   q_dot = new_q_dot;
   r_dot = new_r_dot;
}
//----------------------------------------------------------------------------//

void UUVBody:: set_linear_accelerations  (double new_u_dot, double new_v_dot,
                                          double new_w_dot)
{
   u_dot = new_u_dot;
   v_dot = new_v_dot;
   w_dot = new_w_dot;
}
//----------------------------------------------------------------------------//

void UUVBody:: set_angular_accelerations (double new_p_dot, double new_q_dot,
                                          double new_r_dot)
{
   p_dot = new_p_dot;
   q_dot = new_q_dot;
   r_dot = new_r_dot;
}
//----------------------------------------------------------------------------//
// vehicle socket communications tests
//----------------------------------------------------------------------------//

void UUVBody:: loop_test_with_execution_level ()
{
   int read_from_socket_result = 0;
   int write_to_socket_result  = 0;
 
   // print_uuvbody ();  // diagnostic

   if (TRACE) cout << "[loop_test_with_execution_level start]" << endl;

  //cut open_execution_level_socket ();      // repeated calls should not reopen it

  //cut cout << endl;
  //cut cout << "To listen on default multicast port (on another machine):  " 
  //cut      << endl;
  //cut cout << DISCLIENT << endl;
  //cut cout << endl;

  //cut cout << "DIS_net_open ():" << endl;
  //cut DIS_net_open ();

   while (TRUE)  // loop until break
   {
     //cut read_from_socket_result = read_from_execution_level_socket ();

     /*cut if (read_from_socket_result == -4)   // time/position/orientation received
      {
              cout << "position or orientation command received:  <"
                   << AUV_time << ">  <"
                   << AUV_x    << ", " << AUV_y     << ", " << AUV_z   << ">  <"
                   << AUV_phi  << ", " << AUV_theta << ", " << AUV_psi << ">"
                   << endl;

              RigidBody_initialize ();

              Vector3D vector1 = Vector3D (AUV_x, AUV_y, AUV_z);

              hmatrix = Hmatrix (vector1,
                                 radians (AUV_phi), 
                                 radians (AUV_theta),  
                                 radians (AUV_psi));
              hmatrix.print_hmatrix ();

              Hprevious = hmatrix;
      }
      else if (read_from_socket_result == TRUE) // valid report received
      { cut*/
         integrate_equations_of_motion    ();
      
   //cut      if (DIS_net_write () == FALSE)  break; // send out PDU, otherwise done

    //cut     test_tank_sonar_model ();  // parallelize & generalize the sonar model

    //cut     write_to_execution_level_socket  ();     // send back full telemetry
    //cut  }

      if ((read_from_socket_result == -3) || //   KILL   signal was received
          (read_from_socket_result == -4)  ) // position signal was received
      {
          cout << "KILL/position/orientation signal received, " << endl;
          cout << "freeze the AUV where it is." << endl;

          AUV_u         = 0.0;          // surge  linear velocity along x-axis
          AUV_v         = 0.0;          // sway   linear velocity along y-axis
          AUV_w         = 0.0;          // heave  linear velocity along x-axis
          AUV_p         = 0.0;          // roll  angular velocity about x-axis
          AUV_q         = 0.0;          // pitch angular velocity about y-axis
          AUV_r         = 0.0;          // yaw   angular velocity about z-axis

          AUV_u_dot     = 0.0;          //    linear acceleration along x-axis
          AUV_v_dot     = 0.0;          //    linear acceleration along y-axis
          AUV_w_dot     = 0.0;          //    linear acceleration along x-axis
          AUV_p_dot     = 0.0;          //   angular acceleration about x-axis
          AUV_q_dot     = 0.0;          //   angular acceleration about y-axis
          AUV_r_dot     = 0.0;          //   angular acceleration about z-axis

          AUV_x_dot     = 0.0;          //      Euler velocity along North-axis
          AUV_y_dot     = 0.0;          //      Euler velocity along  East-axis
          AUV_z_dot     = 0.0;          //      Euler velocity along Depth-axis
          AUV_phi_dot   = 0.0;          // Euler rotation rate about North-axis
          AUV_theta_dot = 0.0;          // Euler rotation rate about  East-axis
          AUV_psi_dot   = 0.0;          // Euler rotation rate about Depth-axis

   //cut       clock_t start_busywait_clock = clock ();
   //cut       while  (clock () < start_busywait_clock + 1.0 * CLOCKS_PER_SEC)
   //cut       {
    //cut         /* busy wait */
    //cut      }
    //cut      write_to_socket_result = DIS_net_write ();
          cout << "freeze AUV PDU sent:  " << write_to_socket_result << endl;
      }

      if (read_from_socket_result == -3)   // KILL signal was received
      {
          cout << "KILL signal received, exit "
               << "UUVBody.loop_test_with_execution_level" << endl;

          if (TRACE) cout << "[loop_test_with_execution_level complete]" 
                          << endl;

          return; // all done 
      }
   }
   // unreachable exit point
   // shutdown_socket () must have been invoked already to exit preceding loop

}  // loop_test_with_execution_level complete
//----------------------------------------------------------------------------//

void UUVBody:: dead_reckon_test_with_execution_level ()
{
   int read_from_socket_result = 0;
   int write_to_socket_result  = 0;
 
   cout << endl;
   cout << "To listen on default multicast port:  " << endl;
   cout << DISCLIENT << endl;
   cout << endl;

 //cut  open_execution_level_socket ();

   cout << "DIS_net_open ()" << endl;
  //cut DIS_net_open ();

   while (TRUE)  // loop until break
   {
     //cut read_from_socket_result = read_from_execution_level_socket ();

      if ((read_from_socket_result == -2) || // shutdown signal was received
          (read_from_socket_result == -3))   //   quit   signal was received
      {
          cout << "shutdown signal received, freeze the AUV where it is" 
               << endl;

          AUV_u         = 0.0;          // surge  linear velocity along x-axis
          AUV_v         = 0.0;          // sway   linear velocity along y-axis
          AUV_w         = 0.0;          // heave  linear velocity along x-axis
          AUV_p         = 0.0;          // roll  angular velocity about x-axis
          AUV_q         = 0.0;          // pitch angular velocity about y-axis
          AUV_r         = 0.0;          // yaw   angular velocity about z-axis

          AUV_u_dot     = 0.0;          //    linear acceleration along x-axis
          AUV_v_dot     = 0.0;          //    linear acceleration along y-axis
          AUV_w_dot     = 0.0;          //    linear acceleration along x-axis
          AUV_p_dot     = 0.0;          //   angular acceleration about x-axis
          AUV_q_dot     = 0.0;          //   angular acceleration about y-axis
          AUV_r_dot     = 0.0;          //   angular acceleration about z-axis

          AUV_x_dot     = 0.0;          //      Euler velocity along North-axis
          AUV_y_dot     = 0.0;          //      Euler velocity along  East-axis
          AUV_z_dot     = 0.0;          //      Euler velocity along Depth-axis
          AUV_phi_dot   = 0.0;          // Euler rotation rate about North-axis
          AUV_theta_dot = 0.0;          // Euler rotation rate about  East-axis
          AUV_psi_dot   = 0.0;          // Euler rotation rate about Depth-axis

   //cut       clock_t start_busywait_clock = clock ();
   //cut       while  (clock () < start_busywait_clock + 1.0 * CLOCKS_PER_SEC)
    //cut      {
    //cut         /* busy wait */
    //cut      }
    //cut      write_to_socket_result = DIS_net_write ();
          cout << "freeze AUV PDU sent:  " << write_to_socket_result << endl;

          if (read_from_socket_result == -3) // quit signal was received
          {
             cout << "quit signal received, freeze the AUV where it is" << endl;
             cout << "  and rezero for another loop if necessary" << endl;

             UUVBody_initialize ();
          }
          break; 
      }

      // equations of motion not integrated, execution level does dead reckoning
 
     //cut if (DIS_net_write () == FALSE)   break;  // send out a PDU, otherwise done

     //cut write_to_execution_level_socket  ();     // note state vector preserved
   }
   // shutdown_socket () must have been invoked already to exit preceding loop
   
   // cout << "DIS_net_close ()" << endl;
   // DIS_net_close ();
}
//----------------------------------------------------------------------------//
// vehicle dynamics functions 
//----------------------------------------------------------------------------//

void UUVBody:: swap (double * a, double * b) 
{
   double temp = * a; 
        * a    = * b;
        * b    = temp;
}
//----------------------------------------------------------------------------//

double UUVBody:: square (double value) 
{
   return (value * value);
}
//----------------------------------------------------------------------------//

double UUVBody:: nonzerosign (double value) 
{
   if (value != 0.0) return  sign (value);    
   else              return      + 1.0;  
}
//----------------------------------------------------------------------------//

void UUVBody:: clamp (double * clampee, double absolute_min, 
                                        double absolute_max, char * name) 
{
   double new_value, local_min, local_max;
   
   if ((absolute_max == 0.0) && (absolute_min == 0.0)) return;  // no clamp

   if (absolute_max >= absolute_min) // ensure min & max used in proper order
   {
       local_min = absolute_min;
       local_max = absolute_max;
   } 
   else
   {
       local_min = absolute_max;
       local_max = absolute_min;
   } 
   if ((* clampee) > local_max) 
   {
       new_value = local_max;

       cout << "clamping " << name << " from " 
            << * clampee << " to " << new_value << endl;

       * clampee = new_value;
   }
   if ((* clampee) < local_min) 
   {
       new_value = local_min;

       cout << "clamping " << name << " from " 
            << * clampee << " to " << new_value << endl;

       * clampee = new_value;
   }
}
//----------------------------------------------------------------------------//

double UUVBody:: epsilon ()  // not used in revised surge EOM  <<<<<<<<<<<<<<<<<
                             // due to problems in analytic derivation
                             // retained for archival purposes
{ 
   double        average_rpm = (AUV_port_rpm + AUV_stbd_rpm) / 2.0;

   double n    = average_rpm;

   double eta;

   eta = speed_per_rpm * n / nonzerosign ( U );    

// cout << "eta    = " << eta << endl;

   double C_t  = 0.008 * L*L * eta * fabs (eta) / 2.0;
 
   double C_t1 = 0.008 * L*L                    / 2.0;

   double result;

// X_prop redefinition: net pushing force on vehicle, account for water flow
// X_prop = C_d0 * (eta * fabs (eta) - 1.0);
// X_prop = C_d0 * (eta * fabs (eta));
// cout << "X_prop = " << X_prop << endl;

   result =  -1.0 + (sign (n) / nonzerosign ( U ))

                    * (sqrt (C_t + 1.0) - 1.0) / (sqrt (C_t1 + 1.0) - 1.0);  

// kill this function due to rewritten X_propulsion terms
result = 0.0;

// cout << "epsilon () = " << result << endl; 
 
   return result;
}
//----------------------------------------------------------------------------//

void UUVBody:: invert_mass_matrix ()   // adapted from Numerical Recipes in C
                                       // 2nd edition pp. 39-40
{
   int        indxc [6], indxr [6], ipiv [6];
   int        i, j, k, row, col, l, ll;
   double     big, dummy, pivinv;
   double     mass_copy [6][6];

   for (i = 0; i <= 5; i++)
   {
       for (j = 0; j <= 5; j++)
       {
           mass_copy [i][j] = mass [i][j];
           if    (i == j) mass_inverse [i][j] = 1.0;
           else           mass_inverse [i][j] = 0.0;
       }
   }
   // int arrays ipiv, indxr and indxc are used for bookkeeping on the pivoting

   for (j = 0; j <= 5; j++) ipiv [j] = 0;   // initialize loop

   for (i = 0; i <= 5; i++)                 // main loop over columns reduced
   {
       big = 0.0;
       for (j = 0; j <= 5; j++)             // outer loop of pivot search
       {
            if (ipiv [j] != 1)
            {
                for (k = 0; k <= 5; k++) 
                {
                     if (ipiv [k] == 0)
                     {
                         if (fabs (mass_copy [j][k]) >= big)
                         {
                             big = fabs (mass_copy [j][k]);
                             row = j;
                             col = k;
                         }
                     }
                     else if (ipiv [k] > 1) 
                              cout << "Error:  singular mass matrix" << endl;
                 }
             }
       }
       ++ (ipiv [col]);
       // see detailed comments in reference code for pivot bookkeeping scheme
       
       if (row != col)
       {
           for (l = 0; l <= 5; l++) swap (& mass_copy    [row][l], 
                                          & mass_copy    [col][l]);
           for (l = 0; l <= 5; l++) swap (& mass_inverse [row][l], 
                                          & mass_inverse [col][l]);
       }
       indxr [i] = row; // we are now ready to divide pivot row by pivot element
       indxc [i] = col; //    which is located at [row, col]

       if (mass_copy [col][col] == 0.0) 
           cout << "invert error, singular matrix" << endl;

       pivinv = 1.0 / mass_copy [col][col];
       mass_copy [col][col] = 1.0;

       for (l  = 0; l  <= 5;  l++)  mass_copy    [col][l] *= pivinv;
       for (l  = 0; l  <= 5;  l++)  mass_inverse [col][l] *= pivinv;

       for (ll = 0; ll <= 5; ll++)  // next we reduce the rows
       {
            if (ll != col)          // except for the pivot row, of course
            {
                dummy = mass_copy [ll][col]; 
                mass_copy [ll][col] = 0.0;
                for (l = 0; l <= 5; l++) 
                          mass_copy    [ll][l] -= mass_copy    [col][l] * dummy;
 
                for (l = 0; l <= 5; l++) 
                          mass_inverse [ll][l] -= mass_inverse [col][l] * dummy;
            }
       }
   } 
   // we now unscramble the columns by interchanging in reverse order
   
   for (l = 5; l >= 0; l--) 
   {
        if (indxr [l] != indxc [l])
        {
            for (k = 0; k <= 5; k++) 
            {
               swap (& mass_copy [k][indxr [l]], & mass_copy [k][indxc [l]]);
            }
        }    
   }
}
//----------------------------------------------------------------------------//

void UUVBody:: test_invert_matrix () 
{
   int i, j;

   calculate_mass_matrix ();

   cout << "Original mass matrix:" << endl;
   print_matrix6x6 (mass);

   invert_mass_matrix    ();

   cout << endl;
   cout << "Inverted mass matrix:" << endl;
   print_matrix6x6 (mass_inverse);

   for (i = 0; i <= 5; i++)
   {
       for (j = 0; j <= 5; j++)
       {
           mass [i][j] = mass_inverse [i][j];
       }
   }
   invert_mass_matrix    (); 

   cout << endl;
   cout << "Double invert_mass_matrix () should get back to "
        << "original mass matrix:" << endl;

   print_matrix6x6 (mass_inverse);

   calculate_mass_matrix ();  // restore
   invert_mass_matrix    ();
}
//----------------------------------------------------------------------------//

void UUVBody:: calculate_mass_matrix ()
{
#define UDOT    0          // matrix indices
#define VDOT    1
#define WDOT    2
#define PDOT    3
#define QDOT    4
#define RDOT    5
#define SURGE   0
#define SWAY    1
#define HEAVE   2
#define Roll    3
#define Pitch   4
#define YAW     5

   mass [SURGE][UDOT] =  m          - 0.5 * rho * L*L*L       * X_u_dot;
   mass [SURGE][VDOT] =  0.0;
   mass [SURGE][WDOT] =  0.0;
   mass [SURGE][PDOT] =  0.0;
   mass [SURGE][QDOT] =  m * ( z_G);
   mass [SURGE][RDOT] =  m * (-y_G);

   mass [SWAY ][UDOT] =  0.0;
   mass [SWAY ][VDOT] =  m          - 0.5 * rho * L*L*L       * Y_v_dot;
   mass [SWAY ][WDOT] =  0.0;
   mass [SWAY ][PDOT] =  m * (-z_G) - 0.5 * rho * L*L*L*L     * Y_p_dot;
   mass [SWAY ][QDOT] =  0.0;
   mass [SWAY ][RDOT] =  m * ( x_G) - 0.5 * rho * L*L*L*L     * Y_r_dot;

   mass [HEAVE][UDOT] =  0.0;
   mass [HEAVE][VDOT] =  0.0;
   mass [HEAVE][WDOT] =  m          - 0.5 * rho * L*L*L       * Z_w_dot;
   mass [HEAVE][PDOT] =  m * ( y_G);
   mass [HEAVE][QDOT] =  m * (-x_G) - 0.5 * rho * L*L*L*L     * Z_q_dot;
   mass [HEAVE][RDOT] =  0.0;

   mass [Roll ][UDOT] =  0.0;
   mass [Roll ][VDOT] =  m * (-z_G) - 0.5 * rho * L*L*L*L     * K_v_dot;
   mass [Roll ][WDOT] =  m * ( y_G);
   mass [Roll ][PDOT] =  I_x        - 0.5 * rho * L*L*L*L*L   * K_p_dot;
   mass [Roll ][QDOT] = -I_xy;
   mass [Roll ][RDOT] = -I_xz       - 0.5 * rho * L*L*L*L*L   * K_r_dot;

   mass [Pitch][UDOT] =  m * ( z_G);
   mass [Pitch][VDOT] =  0.0;
   mass [Pitch][WDOT] =  m * (-x_G) - 0.5 * rho * L*L*L*L     * M_w_dot;
   mass [Pitch][PDOT] = -I_xy;
   mass [Pitch][QDOT] =  I_y        - 0.5 * rho * L*L*L*L*L   * M_q_dot;
   mass [Pitch][RDOT] = -I_yz;

   mass [YAW  ][UDOT] =  m * (-y_G);
   mass [YAW  ][VDOT] =  m * ( x_G) - 0.5 * rho * L*L*L*L     * N_v_dot;
   mass [YAW  ][WDOT] =  0.0;
   mass [YAW  ][PDOT] = -I_xz       - 0.5 * rho * L*L*L*L*L   * N_p_dot;
   mass [YAW  ][QDOT] = -I_yz;
   mass [YAW  ][RDOT] =  I_z        - 0.5 * rho * L*L*L*L*L   * N_r_dot;
}

//----------------------------------------------------------------------------//

void UUVBody:: integrate_equations_of_motion ()
{  
  
  
  
//CUT?   calculate_mass_matrix ();
//CUT?   invert_mass_matrix    ();
   

   current_uuv_time = AUV_time;

   double dt        = current_uuv_time - time_of_posture_value ();

   if (dt < 0.0)    // mission clock was reset, rezero the dynamics model
   { 
       current_uuv_time   = AUV_time;
       set_time_of_posture (AUV_time);
       set_velocities    (0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
       set_accelerations (0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
       dt = 0.0;
       U  = 0.0;
       V  = 0.0;
       W  = 0.0;
       P  = 0.0;
       Q  = 0.0;
       RR  = 0.0;
   }

   double rho2      = rho / 2.0;
   double L2        = L * L;
   double L3        = L * L * L;
   double L4        = L * L * L * L;
   double L5        = L * L * L * L * L;

   // note that sign is not preserved in the following squared variables
   //      in order to present consistent naming with Healey reference paper.
   //      To preserve sign, use (U * fabs (U)) etc.
   double P2        = P * P;
   double Q2        = Q * Q;
   double R2        = RR * RR;
//   double U2        = U * U;
   double V2        = V * V;
   double W2        = W * W;

   // calculate world coordinate posture rates, use holding variables for speed

   double PHI       = phi_value   ();
   double THETA     = theta_value ();
   double PSI       = psi_value   ();

   double sinPHI    = sin (   PHI );
   double cosPHI    = cos (   PHI );
   double sinTHETA  = sin ( THETA );
   double cosTHETA  = cos ( THETA );
   double sinPSI    = sin (   PSI );
   double cosPSI    = cos (   PSI );

// double EPSILON   = epsilon (); // no longer used in revised model

   double delta_planes_stern =   AUV_delta_planes;
   double delta_planes_bow   = - AUV_delta_planes;


   double delta_rudder_stern =   AUV_delta_rudder;
   double delta_rudder_bow   = - AUV_delta_rudder;

   // calculate buoyancy and center of buoyancy near surface - - - - - - - - -//
   float setting = 2.0;
   if      ((AUV_z-setting) <= H / 2.0)           /* transition */
   {
      if   ((AUV_z-setting) >= -(H / 2.0))        /* starting to broach */

           revisedBuoyancy = Buoyancy * ((AUV_z-setting) + H/2.0)/ H;

      else revisedBuoyancy = 0.0;       /* out of the water */
   }
   else    revisedBuoyancy = Buoyancy;  /* normal submerged */

   // if boat is broaching and pitch is positive, perform an approximate
   // calculation of how center of buoyancy CB moves back towards stern
  
   nose_length = (0.90 * L) / 2.0;

   if      (THETA  == 0.0) 
   {
      revised_x_B  = x_B; // prevent divide by zero case
   }
   else if (THETA  > 0.0) 
   {
      surface_length = (AUV_z-(2.0*setting)) / (sinTHETA );
 //  cerr<<"THETA  is >0.0, surface length = "<<surface_length<<endl;
   }
   else if (THETA  < 0.0) 
   {
      surface_length = (AUV_z-(2.0*setting)) / ( -sinTHETA );
  //  cerr<<"THETA  is <0.0, surface length = "<<surface_length<<endl;
   }
   else  
   {
      cout << "Unexpected case in revised CB calculation!" << endl;
      revised_x_B  = x_B; // prevent divide by zero case
   }

   if ((THETA  != 0.0) && (surface_length < nose_length)) 
   // move x_CB back but only if nose (stern) broaches the surface
   {
      revised_x_B  = x_B - (nose_length - surface_length) * sinTHETA  / 2.0; 
//cerr<<"AFFECTED"<<endl;
   }

   if (FALSE)
   {
      cout << "surface_length = " << surface_length << ", ";
      cout << "nose_length = "    << nose_length    << ", ";
      cout << "revised_x_B = "    << revised_x_B    << endl;
   }
   
   // integrate drag forces over the vehicle - - - - - - - - - - - - - - - - -//

   double  sway_integral = 0.0;
   double heave_integral = 0.0;
   double pitch_integral = 0.0;
   double   yaw_integral = 0.0;

   double dx, U_cf_x; 
 
   for (int x_index = 0; x_index < 14; x_index ++) // longitudinal centerline
   {
        dx = fabs (xx [x_index] - xx [x_index + 1]);

        U_cf_x = sqrt (  square (V + xx [x_index] * RR)

                       + square (W - xx [x_index] * Q));

        if (U_cf_x > 1.0E-6) // arbitrary small non-0 minimum
        {
            sway_integral  +=   rho2 * ( C_dy * hh [x_index] 

                                              * square ((V + xx [x_index] * RR))

                                       + C_dz * bb [x_index] 
 
                                              * square ((W - xx [x_index] * Q)))

                                     * (V + xx [x_index] * RR) * dx / U_cf_x;


            heave_integral +=   rho2 * ( C_dy * hh [x_index] 

                                              * square ((V + xx [x_index] * RR))

                                       + C_dz * bb [x_index] 
 
                                              * square ((W - xx [x_index] * Q)))

                                     * (W - xx [x_index] * Q) * dx / U_cf_x;


            pitch_integral +=   rho2 * ( C_dy * hh [x_index] 

                                              * square ((V + xx [x_index] * RR))

                                       + C_dz * bb [x_index] 
 
                                              * square ((W - xx [x_index] * Q)))

                                     * (W - xx [x_index] * Q)  

                                       // ^ note sign correction

                                     * xx [x_index] * dx / U_cf_x;


              yaw_integral +=   rho2 * ( C_dy * hh [x_index] 

                                              * square ((V + xx [x_index] * RR))

                                       + C_dz * bb [x_index] 
 
                                              * square ((W - xx [x_index] * Q)))

                                     * (V + xx [x_index] * RR) 

                                     * xx [x_index] * dx / U_cf_x;
        }
   }
   if (TRACE) 
   {
      cout << "dx = " << dx << ", U_cf_x = " << U_cf_x 
           << ", sway_integral  = " << sway_integral << endl;

      cout << "dx = " << dx << ", U_cf_x = " << U_cf_x 
           << ", heave_integral = " << heave_integral << endl;

      cout << "dx = " << dx << ", U_cf_x = " << U_cf_x 
           << ", pitch_integral = " << pitch_integral << endl;

      cout << "dx = " << dx << ", U_cf_x = " << U_cf_x 
           << ", yaw_integral   = " << yaw_integral << endl;
   } 

////////////////////////////////////////////////////////////////////////////////
// calculate Equations of Motion right-hand sides                             //
////////////////////////////////////////////////////////////////////////////////

   rhs [SURGE] = // Surge Motion Equation right hand side --------------------//

         m * ((V * RR) - (W * Q) + x_G * (Q2 + R2) - y_G * P*Q - z_G * P*RR)

       + rho2 * L4 * (  X_pp * P2  + X_qq * Q2  
                      + X_rr * R2  + X_pr * P*RR)

       + rho2 * L3 * (  X_wq * W*Q + X_vp * V*P + X_vr * V*RR

                      + U*Q * (  X_uq_delta_bow     * delta_planes_bow  
                               + X_uq_delta_stern   * delta_planes_stern)

                      + U*RR * (  X_ur_delta_rudder  * delta_rudder_bow
                               + X_ur_delta_rudder  * delta_rudder_stern)
                     ) 

       + rho2 * L2 * (  X_vv * V2  + X_ww * W2   

                      + U*V * (  X_uv_delta_rudder  * delta_rudder_stern)

                      + U*W * (  X_uw_delta_bow     * delta_planes_bow  
                               + X_uw_delta_stern   * delta_planes_stern)

                      + U * fabs (U)  * (  X_uu_delta_b_delta_b  
                                           * delta_planes_bow 
                                           * delta_planes_bow    

                                         + X_uu_delta_s_delta_s 
                                           * delta_planes_stern
                                           * delta_planes_stern     

                                         + X_uu_delta_r_delta_r  
                                           * delta_rudder_bow 
                                           * delta_rudder_bow

                                         + X_uu_delta_r_delta_r  
                                           * delta_rudder_stern 
                                           * delta_rudder_stern)
                     )

       - (Weight - revisedBuoyancy) * sinTHETA

       // EPSILON terms have been removed due to revised equations of motion

       // + rho2 * L3 *   X_qdsn  * U*Q * delta_planes_stern * EPSILON
       // + rho2 * L2 * EPSILON * ( X_wdsn  * U*W * delta_planes_stern
       // 
       //                          + X_dsdsn * U2       * delta_planes_stern
       //                                               * delta_planes_stern)

       // X_propulsion surge force (derived using expressions in Healey paper)
       // note that speed_per_rpm is associated with work of two propellors
           
       + rho2 * L2 * C_d0 *  square (speed_per_rpm) 

                   *  0.5 * (  AUV_port_rpm * fabs (AUV_port_rpm)

                             + AUV_stbd_rpm * fabs (AUV_stbd_rpm))

       // X_resistance surge drag (derived using expressions in Healey paper)

       - rho2 * L2 * C_d0 * U * fabs (U);
                          
////////////////////////////////////////////////////////////////////////////////

if (TRACE)   // Surge TRACE
{
cout << "* surge term1=" <<  m * ((V * RR) - (W * Q) 
         + x_G * (Q2 + R2) - y_G * P*Q - z_G * P*RR)<< endl;

cout << "term2=" <<   + rho2 * L4 * (  X_pp * P2  + X_qq * Q2  
                      + X_rr * R2  + X_pr * P*RR)
     << endl;

cout << "term3=" <<   + rho2 * L3 * (  X_wq * W*Q + X_vp * V*P + X_vr * V*RR

                      + U*Q * (  X_uq_delta_bow     * delta_planes_bow    
                               + X_uq_delta_stern   * delta_planes_stern)

                      + U*RR * (  X_ur_delta_rudder  * delta_rudder_stern
                               + X_ur_delta_rudder  * delta_rudder_bow)
                     )  
     << endl;

cout << "term4=" <<   + rho2 * L2 * (  X_vv * V2  + X_ww * W2   

                      + U*V * (  X_uv_delta_rudder  * delta_rudder_stern)

                      + U*W * (  X_uw_delta_bow     * delta_planes_bow  
                               + X_uw_delta_stern   * delta_planes_stern)

                      + U * fabs (U)  * (  X_uu_delta_b_delta_b  
                                           * delta_planes_bow 
                                           * delta_planes_bow    

                                         + X_uu_delta_s_delta_s  
                                           * delta_planes_stern 
                                           * delta_planes_stern   

                                         + X_uu_delta_r_delta_r  
                                           * delta_rudder_bow 
                                           * delta_rudder_bow 

                                         + X_uu_delta_r_delta_r  
                                           * delta_rudder_stern 
                                           * delta_rudder_stern)
                     )
     << endl;

cout << "term5=" << - (Weight - revisedBuoyancy) * sinTHETA
     << endl;

cout << "term6,term7=" <<   "EPSILON terms, no longer used"
     << endl;

// cout << "term6=" <<  rho2 * L3 *   X_qdsn  * U*Q * delta_planes_stern
//      * EPSILON << endl;
//   
// cout << "term7=" <<  rho2 * L2 * EPSILON * ( X_wdsn  * U*W
//                                                       * delta_planes_stern
//                                + X_dsdsn * U2         * delta_planes_stern
//                                                       * delta_planes_stern)
//      << endl;

cout << "term8=" <<  + rho2 * L2 * C_d0 *  square (speed_per_rpm) 

                   *  0.5 * (  AUV_port_rpm * fabs (AUV_port_rpm)

                             + AUV_stbd_rpm * fabs (AUV_stbd_rpm))
     << endl;
  
cout << "term9=" << - rho2 * L2 * C_d0 * U * fabs (U)
     << endl;
}
  
////////////////////////////////////////////////////////////////////////////////

    rhs [SWAY ] = // Sway  Motion Equation right hand side -------------------//

         m * (- (U * RR) + (W * P) - x_G * (P * Q) 

                                  + y_G * (P2 + R2)

                                  - z_G * (Q  * RR))

       + rho2 * L4 * (  Y_pq    * P*Q    + Y_qr   * Q*RR)

       + rho2 * L3 * (  Y_up    * U*P    + Y_ur   * U*RR 

                      + Y_vq    * V*Q    + Y_wp   * W*P    +  Y_wr * W*RR)

       + rho2 * L2 * (  Y_uv    * U*V    + Y_vw   * V*W      
  
                      + U*fabs(U) * Y_uu_delta_rb * delta_rudder_bow
 
                      + U*fabs(U) * Y_uu_delta_rs * delta_rudder_stern)
  
       - sway_integral

       + (Weight - revisedBuoyancy) * cosTHETA * sinPHI

       -  (2.0 / (24.0 * 24.0))  // each thruster 2.0 lb per 24V signal squared

          * (  AUV_bow_lateral   * fabs (AUV_bow_lateral) 
             + AUV_stern_lateral * fabs (AUV_stern_lateral));


////////////////////////////////////////////////////////////////////////////////

if (TRACE)   // Sway TRACE
{
cout << "* sway term1=" <<  m * (- (U * RR) + (W * P) 

                                 - x_G * (P * Q)  

                                 + y_G * (P2 + R2) 

                                 - z_G * (Q  * RR))
     << endl;

cout << "term2=" <<   + rho2 * L4 * (  Y_pq    * P*Q    + Y_qr   * Q*RR)
     << endl;

cout << "term3=" <<   + rho2 * L3 * (  Y_up    * U*P    +  Y_ur  * U*RR 

                      + Y_vq    * V*Q    + Y_wp   * W*P    +  Y_wr * W*RR)
     << endl;

cout << "term4=" <<   + rho2 * L2 * (  Y_uv     * U*V    + Y_vw   * V*W 
  
                      + U*fabs(U) * Y_uu_delta_rb * delta_rudder_bow     
 
                      + U*fabs(U) * Y_uu_delta_rs * delta_rudder_stern)
     << endl;

cout << "term5=" <<   - sway_integral
     << endl;

cout << "term6=" <<   + (Weight - revisedBuoyancy) * cosTHETA * sinPHI
     << endl;

cout << "term7=" <<   -  (2.0 / (24.0 * 24.0)) 
                     // each thruster 2.0 lb per 24V signal squared

                      * (  AUV_bow_lateral   * fabs (AUV_bow_lateral) 
                         + AUV_stern_lateral * fabs (AUV_stern_lateral))  
     << endl;
}

////////////////////////////////////////////////////////////////////////////////
  
   rhs [HEAVE] = // Heave Motion Equation right hand side --------------------//

         m * (  (U * Q) - (V * P) - x_G * (P * RR) - y_G * (Q * RR) 

                                                  + z_G * (P2 + Q2))

       + rho2 * L4 * (  Z_pp    * P2     + Z_pr * P*RR    + Z_rr * R2)

       + rho2 * L3 * (  Z_uq    * U*Q    + Z_vp * V*P    + Z_vr * V*RR)

       + rho2 * L2 * (  Z_uw    * U*W    + Z_vv * V2          

                       + ( U*fabs(U) * Z_uu_delta_b * delta_planes_bow  )
 
                       + ( U*fabs(U) * Z_uu_delta_s * delta_planes_stern))

       - heave_integral

       + (Weight - revisedBuoyancy) * cosTHETA * cosPHI
  
       // EPSILON terms have been removed due to revised equations of motion

       // + rho2 * L3 *    Z_qn  * U*Q * EPSILON 
       // + rho2 * L2 * (  Z_wn  * U*W 
       //                + Z_dsn * U*fabs(U) * delta_planes_stern) * EPSILON
 
       +  (2.0 / (24.0 * 24.0))  // each thruster 2.0 lb per 24V signal squared

          * ( AUV_bow_vertical   * fabs (AUV_bow_vertical)  + 
              AUV_stern_vertical * fabs (AUV_stern_vertical));


////////////////////////////////////////////////////////////////////////////////

if (TRACE)   // Heave TRACE
{
cout << "* heave term1=" <<  m * (  (U * Q) - (V * P) - x_G * (P * RR) 
                                                  - y_G * (Q * RR) 
                                                  + z_G * (P2 + Q2))
     << endl;

cout << "term2=" <<   + rho2 * L4 * (  Z_pp    * P2     + Z_pr    * P*RR 
        + Z_rr * R2) << endl;

cout << "term3=" <<   + rho2 * L3 * (  Z_uq    * U*Q    + Z_vp * V*P     
        + Z_vr * V*RR) << endl;

cout << "term4=" <<   + rho2 * L2 * (  Z_uw    * U*W    + Z_vv  * V2   

                       + ( U*fabs(U) * Z_uu_delta_b * delta_planes_bow  )       
 
                       + ( U*fabs(U) * Z_uu_delta_s * delta_planes_stern))
     << endl;

cout << "term5=" <<   - heave_integral
     << endl;

cout << "term6=" <<   + (Weight - revisedBuoyancy) * cosTHETA * cosPHI
     << endl;

cout << "term7=" <<   "EPSILON terms, no longer used"
     << endl;

cout << "term8=" <<   + rho2 * L2 * ( Z_wn  * U*W )
     << endl;

cout << "term9=" <<   +  (2.0 / (24.0 * 24.0))     
         // each thruster 2.0 lb per 24V signal squared

                         * ( AUV_bow_vertical   * fabs (AUV_bow_vertical)  + 
                             AUV_stern_vertical * fabs (AUV_stern_vertical) )
     << endl;
}

////////////////////////////////////////////////////////////////////////////////
  
   rhs [Roll ] = // Roll  Motion Equation right hand side --------------------//

       - (I_z - I_y) * Q*RR - I_xy * P*RR + I_yz * (Q2 - R2) + I_xz * P*Q

       - m * ( y_G * ( -U*Q + V*P) - z_G  * ( U*RR - W*P))

       + rho2 * L5  * (  K_pq * P*Q  + K_qr * Q*RR  

                       + K_pp * P * fabs(P) 
                       + K_p  * P           )  // hovering roll drag

       + rho2 * L4  * ( K_up * fabs(U)*P  + K_ur * U*RR  + K_vq * V*Q 

                      + K_wp * W*P  + K_wr * W*RR)

       + rho2 * L3  * (  K_uv * U*V  + K_vw * V*W  
 
                       - U*fabs(U) * 0.5 * (  K_uu_planes * delta_planes_bow
                                            + K_uu_planes * delta_planes_stern))
       // expected: opposed plane directions ^ cause negation & cancellation

       + (y_G * Weight - y_B * revisedBuoyancy) * cosTHETA * cosPHI

       - (z_G * Weight - z_B * revisedBuoyancy) * cosTHETA * sinPHI;

       // EPSILON terms have been removed due to revised equations of motion
       // + rho2 * L4 * K_pn * U*P * EPSILON

       // + rho2 * L3 * U*fabs(U)  * K_prop; // oversimplified, in error

////////////////////////////////////////////////////////////////////////////////

if (TRACE)   // Roll TRACE
{
cout << "* roll term1=" << - (I_z - I_y) * Q*RR - I_xy * P*RR + I_yz * (Q2 - R2) 
                         + I_xz * P*Q
     << endl;

cout << "term2=" << - m * ( y_G * ( -U*Q + V*P) - z_G  * ( U*RR - W*P))
     << endl;

cout << "term3=" << + rho2 * L5  * (  K_pq * P*Q  + K_qr * Q*RR  

                       + K_pp * P * fabs(P) 
                       + K_p  * P           )  // hovering roll drag
     << endl;

cout << "term4=" << + rho2 * L4  * ( K_up  * fabs(U)*P  + K_ur * U*RR

                        + K_vq * V*Q + K_wp * W*P  + K_wr * W*RR)
     << endl;

cout << "term5=" << + rho2 * L3  * (  K_uv * U*V  + K_vw * V*W  
 
                       - U*fabs(U) * 0.5 * (  K_uu_planes * delta_planes_bow
                                            + K_uu_planes * delta_planes_stern))
       // expected: opposed plane directions ^ cause negation & cancellation
     << endl;

cout << "term6=" << + (y_G * Weight - y_B * revisedBuoyancy) * cosTHETA * cosPHI
     << endl;

cout << "term7=" << - (z_G * Weight - z_B * revisedBuoyancy) * cosTHETA * sinPHI
     << endl;
  
cout << "term8,term9=" <<   "EPSILON terms, no longer used"
     << endl;

// cout << "term8=" << + rho2 * L4 * K_pn * U*P * EPSILON
//      << endl;

// cout << "term9=" << + rho2 * L3 * U*fabs(U)  * K_prop
//     << endl;
}
 
////////////////////////////////////////////////////////////////////////////////
  
   rhs [Pitch] = // Pitch Motion Equation right hand side --------------------//

       - (I_x - I_z) * P*RR + I_xy * Q*RR - I_yz * P*Q - I_xz * (P2 - R2)

       +  m * ( x_G * ( -U*Q + V*P) - z_G  * ( - V*RR + W*Q))

       + rho2 * L5  * (  M_pp * P2   + M_pr * P*RR  + M_rr * RR*fabs (RR)  
  
                       + M_q  * Q 
                       + M_qq * Q * fabs(Q))  // hovering pitch drag

       + rho2 * L4  * ( M_uq * U*Q  + M_vp * V*P  + M_vr * V*RR)

       + rho2 * L3  * ( M_uw * U*W  + M_vv * V2   
 
                      + U*fabs(U) * (  M_uu_delta_bow   * delta_planes_bow 
                                     + M_uu_delta_stern * delta_planes_stern))

       + pitch_integral  // note sign corrections to Healey pitch_integral

       - (x_G * Weight   -  x_B * revisedBuoyancy) * cosTHETA * cosPHI
  
       - (z_G * Weight   -  z_B * revisedBuoyancy) * sinTHETA  

       +  (2.0 / (24.0 * 24.0))  // each thruster 2.0 lb per 24V signal squared
                                 // multiplied by respective moment arms
                                 // x_bow_vertical (+), x_stern_vert (-)

       * ( (AUV_bow_vertical   * fabs (AUV_bow_vertical)   * x_bow_vertical)  
          +(AUV_stern_vertical * fabs (AUV_stern_vertical) * x_stern_vertical));
  
       // EPSILON terms have been removed due to revised equations of motion
       // + rho2 * L4  *  M_qn * U*Q * EPSILON
       // + rho2 * L3  * (M_wn * U*W + M_dsn * U*fabs(U) * delta_planes_stern) 
       //              * EPSILON;

////////////////////////////////////////////////////////////////////////////////

if (TRACE)   // Pitch TRACE
{
cout << "* pitch term1=" << - (I_x - I_z) * P*RR + I_xy * Q*RR - I_yz * P*Q
         - I_xz * (P2 - R2) << endl;

cout << "term2=" << +  m * ( x_G * ( -U*Q + V*P) - z_G  * ( - V*RR + W*Q))
     << endl;

cout << "term3=" << + rho2 * L5  * (  M_pp * P2   + M_pr * P*RR  + M_rr 
                                                                  * RR*fabs (RR) 
                       + M_q  * Q 
                       + M_qq * Q * fabs(Q))  // hovering pitch drag
     << endl;

cout << "term4=" << + rho2 * L4  * ( M_uq * U*Q  + M_vp * V*P  + M_vr * V*RR)
     << endl;

cout << "term5=" << + rho2 * L3  * ( M_uw * U*W  + M_vv * V2   
 
                      + U*fabs(U) * (  M_uu_delta_bow   * delta_planes_bow   
                                     + M_uu_delta_stern * delta_planes_stern))
     << endl;

cout << "term6=" << + pitch_integral
     << endl;

cout << "term7=" << - (x_G * Weight   -  x_B * revisedBuoyancy) * cosTHETA * cosPHI
     << endl; 
  
cout << "term8=" << - (z_G * Weight   -  z_B * revisedBuoyancy) * sinTHETA
     << endl;
  
cout << "term9=" << +  (2.0 / (24.0 * 24.0))
                                 // each thruster 2.0 lb per 24V signal squared
                                 // multiplied by respective moment arms
                                 // x_bow_vertical (+). x_stern_vert (-)

        * ( (AUV_bow_vertical   * fabs (AUV_bow_vertical)   * x_bow_vertical)  
           +(AUV_stern_vertical * fabs (AUV_stern_vertical) * x_stern_vertical))
     << endl;
  
cout << "term10,term11=" <<   "EPSILON terms, no longer used"
     << endl;

// cout << "term10=" << + rho2 * L4  *  M_qn * U*Q * EPSILON
//      << endl;

// cout << "term11=" << + rho2 * L3  * (M_wn * U*W + M_dsn * U*fabs(U) 
//                                                        * delta_planes_stern) 
//                     * EPSILON
//     << endl;
}
 
////////////////////////////////////////////////////////////////////////////////
  
   rhs [YAW  ] = // Yaw   Motion Equation right hand side --------------------//

       - (I_y - I_x) * P*Q + I_xy * (P2 - Q2) + I_yz * P*RR - I_xz * Q*RR

       -  m * ( x_G * ( U*RR - W*P) - y_G   * ( - V*RR + W*Q))

       + rho2 * L5  * (  N_pq * P*Q  + N_qr * Q*RR  
 
                       + N_r  * RR 
                       + N_rr * RR * fabs (RR))  // hovering yaw drag

       + rho2 * L4  * ( N_up * U*P  + N_ur * U*RR  + N_vq * V*Q  

                      + N_wp * W*P  + N_wr * W*RR)

       + rho2 * L3  * ( N_uv * U*V  + N_vw * V*W 
 
                      + U*fabs(U) * N_uu_delta_rb * delta_rudder_bow
                      - U*fabs(U) * N_uu_delta_rs * delta_rudder_stern)

       - yaw_integral

       + (x_G * Weight  -  x_B * revisedBuoyancy) * cosTHETA * sinPHI
  
       + (y_G * Weight  -  y_B * revisedBuoyancy) * sinTHETA

       -  (2.0 / (24.0 * 24.0)) // each thruster 2.0 lb per 24V signal squared
                                // multiplied by respective moment arms
          * ( (AUV_bow_lateral   * fabs (AUV_bow_lateral)   * x_bow_lateral   )  
             +(AUV_stern_lateral * fabs (AUV_stern_lateral) * x_stern_lateral ))

       - rho2 * L2 * C_d0 

                   * ( square (speed_per_rpm) *  0.5 // propeller yaw

                      * (  AUV_port_rpm * fabs(AUV_port_rpm) * y_port_propeller

                         + AUV_stbd_rpm * fabs(AUV_stbd_rpm) * y_stbd_propeller)

                      );

////////////////////////////////////////////////////////////////////////////////

if (TRACE)   // Yaw TRACE
{
cout << "* yaw term1=" <<  - (I_y - I_x) * P*Q + I_xy * (P2 - Q2) 
                         + I_yz * P*RR - I_xz * Q*RR
     << endl;

cout << "term2=" <<   -  m * ( x_G * ( U*RR - W*P) - y_G   * ( - V*RR + W*Q))
     << endl;

cout << "term3=" <<   + rho2 * L5  * (  N_pq * P*Q  + N_qr * Q*RR  
 
                       + N_r  * RR 
                       + N_rr * RR * fabs (RR))  // hovering yaw drag
     << endl;

cout << "term4=" <<   + rho2 * L4  * ( N_up * U*P  + N_ur * U*RR  + N_vq * V*Q  

                      + N_wp * W*P  + N_wr * W*RR)
     << endl;

cout << "term5=" << + rho2 * L3  * ( N_uv * U*V  + N_vw * V*W 
 
                      + U*fabs(U) * N_uu_delta_rb * delta_rudder_bow
                      - U*fabs(U) * N_uu_delta_rs * delta_rudder_stern)
     << endl;

cout << "term6=" <<  - yaw_integral
     << endl;
  
cout << "term7=" <<  + (x_G * Weight  -  x_B * revisedBuoyancy) * cosTHETA * sinPHI
     << endl;
  
cout << "term8=" <<  + (y_G * Weight  -  y_B * revisedBuoyancy) * sinTHETA
     << endl;
  
cout << "term9=" << -  (2.0 / (24.0 * 24.0))     
                       // each thruster 2.0 lb per 24V signal squared
                       // multiplied by respective moment arms
          * ( (AUV_bow_lateral   * fabs (AUV_bow_lateral)   * x_bow_lateral   )  
             +(AUV_stern_lateral * fabs (AUV_stern_lateral) * x_stern_lateral ))
     << endl;
  
cout << "term10=" << - rho2 * L2 * C_d0 

                   * ( square (speed_per_rpm) *  0.5 // propeller yaw

                      * (  AUV_port_rpm * fabs(AUV_port_rpm) * y_port_propeller

                         + AUV_stbd_rpm * fabs(AUV_stbd_rpm) * y_stbd_propeller)

                      )
     << endl;
}

////////////////////////////////////////////////////////////////////////////////
  
// debug:

if (TRACE) 
{
   cout << "     SURGE  = " <<      SURGE  << endl;
   cout << "     SWAY   = " <<      SWAY   << endl;
   cout << "     HEAVE  = " <<      HEAVE  << endl;
   cout << "     Roll   = " <<      Roll   << endl;
   cout << "     Pitch  = " <<      Pitch  << endl;
   cout << "     YAW    = " <<      YAW    << endl;

   cout << "rhs [SURGE] = " << rhs [SURGE] << endl;
   cout << "rhs [SWAY ] = " << rhs [SWAY ] << endl;
   cout << "rhs [HEAVE] = " << rhs [HEAVE] << endl;
   cout << "rhs [Roll ] = " << rhs [Roll ] << endl;
   cout << "rhs [Pitch] = " << rhs [Pitch] << endl;
   cout << "rhs [YAW  ] = " << rhs [YAW  ] << endl;

// cout << "mass_inverse:" << endl;   print_matrix6x6 (mass_inverse);


   cout << "velocities:     <" << U << ", " << V << ", " << W << ", " 
                               << P << ", " << Q << ", " << RR << ">" << endl;
   
   cout << "RHS:            ";        print_matrix6 (rhs);
}

// rhs [SURGE] = 0.0;
// rhs [SWAY ] = 0.0;
// rhs [HEAVE] = 0.0;
// rhs [Roll ] = 0.0;
// rhs [Pitch] = 0.0;
// rhs [YAW  ] = 0.0;

////////////////////////////////////////////////////////////////////////////////
  
   // calculate new accelerations matrix using mass_inverse & rhs, print -----//
   
   multiply6x6_6 (mass_inverse, rhs, new_acceleration);

   if (TRACE) 
   {
       cout << "Accelerations:  ";   print_matrix6 (new_acceleration);
   }

   // limit accelerations ----------------------------------------------------//

   clamp(& new_acceleration [SURGE], -5.0, 5.0, "new_acceleration [SURGE]");
   clamp(& new_acceleration [SWAY ], -5.0, 5.0, "new_acceleration [SWAY ]");
   clamp(& new_acceleration [HEAVE], -5.0, 5.0, "new_acceleration [HEAVE]");

   clamp(& new_acceleration [Roll ], -5.0, 5.0, "new_acceleration [Roll ]");
   clamp(& new_acceleration [Pitch], -5.0, 5.0, "new_acceleration [Pitch]");
   clamp(& new_acceleration [YAW  ], -5.0, 5.0, "new_acceleration [YAW  ]");


   // find velocities by integrating averaged accelerations -----------------//
   //      (Heun integration)

   new_velocity [SURGE] = 0.5 * (u_dot + new_acceleration [SURGE]) * dt + U;
   new_velocity [SWAY ] = 0.5 * (v_dot + new_acceleration [SWAY ]) * dt + V;
   new_velocity [HEAVE] = 0.5 * (w_dot + new_acceleration [HEAVE]) * dt + W;
   new_velocity [Roll ] = 0.5 * (p_dot + new_acceleration [Roll ]) * dt + P;
   new_velocity [Pitch] = 0.5 * (q_dot + new_acceleration [Pitch]) * dt + Q;
   new_velocity [YAW  ] = 0.5 * (r_dot + new_acceleration [YAW  ]) * dt + RR;

   // find velocities by integrating instantaneous accelerations 
   //      (this method is less accurate and is not used, although at small
   //       timesteps the difference is negligible)
   //      (Euler integration)

   // new_velocity [SURGE] = (new_acceleration [SURGE]) * dt + U;
   // new_velocity [SWAY ] = (new_acceleration [SWAY ]) * dt + V;
   // new_velocity [HEAVE] = (new_acceleration [HEAVE]) * dt + W;
   // new_velocity [Roll ] = (new_acceleration [Roll ]) * dt + P;
   // new_velocity [Pitch] = (new_acceleration [Pitch]) * dt + Q;
   // new_velocity [YAW  ] = (new_acceleration [YAW  ]) * dt + RR;


   // note that surge velocity may be negative under to model constraints
   // but this is a problem so it is clamped to be non-negative

   clamp (& new_velocity [SURGE], -3.0, 3.0, "new_velocity [SURGE] velocity"); 
                                     // 3.0 is set arbitrarily high

   // update UUVBody state accelerations to newly-calculated values   --------//

   u_dot = new_acceleration [SURGE];
   v_dot = new_acceleration [SWAY ];
   w_dot = new_acceleration [HEAVE];
   p_dot = new_acceleration [Roll ];
   q_dot = new_acceleration [Pitch];
   r_dot = new_acceleration [YAW  ];

   // calculate world coordinate system linear & angular velocities   --------//

   //  see Cooke Figure 10 for corrections to Healey equations for x/y/z_dot:
   //  also Healey course notes eqn (26) and Frank-McGhee corrected paper (A.8)

   x_dot = AUV_oceancurrent_u 

         + U *  cos (PSI) * cos (THETA)

         + V * (cos (PSI) * sin (THETA) * sin (PHI) - sin (PSI) * cos(PHI))

         + W * (cos (PSI) * sin (THETA) * cos (PHI) + sin (PSI) * sin(PHI)); 

   y_dot = AUV_oceancurrent_v 
  
         + U *  sin (PSI) * cos (THETA)

         + V * (sin (PSI) * sin (THETA) * sin (PHI) + cos (PSI) * cos(PHI))

         + W * (sin (PSI) * sin (THETA) * cos (PHI) - cos (PSI) * sin(PHI)); 

   z_dot = AUV_oceancurrent_w 

         - U * sin (THETA)

         + V * cos (THETA) * sin (PHI)

         + W * cos (THETA) * cos (PHI); 


   phi_dot   = P + Q * sin (PHI) * tan (THETA)
 
                 + RR * cos (PHI) * tan (THETA);

   theta_dot =     Q * cos (PHI) 

                 - RR * sin (PHI);


   if  (cos (THETA) == 0.0)
   {
        cout << "UUVBody::integrate_equations_of_motion ():  "        << endl;
        cout << "   cos (THETA) == 0.0 so psi_dot set equal to zero." << endl; 
        psi_dot = 0.0;  
   }
   else psi_dot = (Q * sin (PHI) + RR * cos (PHI)) / cos (THETA);


   Vector3D linear_rates =  Vector3D (x_dot, y_dot, z_dot);
   if (TRACE) 
   {
        cout << endl;
        cout << "<x_dot, y_dot, z_dot>         = " << linear_rates << endl;
        cout << "                    magnitude = " << linear_rates.magnitude ()
             << endl;
   }

   Vector3D euler_rates =  Vector3D (phi_dot, theta_dot, psi_dot);
   if (TRACE) 
   {
        cout << "<phi_dot, theta_dot, psi_dot> = " << euler_rates << endl;
        cout << "                    magnitude = " << euler_rates.magnitude ()
             << endl;
   }

   // calculate world coordinate system homogenous transform matrix   --------//

   Hmatrix Hincremental = Hmatrix (); // default initialization
   Hincremental.set_posture ( P * dt, Q * dt, RR * dt );
   Hincremental.rotate      ( PHI,    THETA,  PSI    );
   double w_x = Hincremental.phi_value   ();
   double w_y = Hincremental.theta_value ();
   double w_z = Hincremental.psi_value   ();


   Vector3D world_rates =  Vector3D (w_x, w_y, w_z);
   if (TRACE) 
   {
        cout << "<w_x, w_y, w_z>               = " << world_rates << endl;
        cout << "                    magnitude = " << world_rates.magnitude ()
             << endl;
   }

   Hmatrix Hrevised1 = Hmatrix ();   // default initialization
   Hrevised1.incremental_rotation    ( phi_dot, theta_dot, psi_dot, dt );
   Hrevised1.incremental_translation ( U, V, W, dt );

   Hmatrix Hproduct1 = Hprevious * Hrevised1;

   Hmatrix Hrevised2 = Hmatrix (   x_dot * dt,     y_dot * dt,   z_dot * dt, 
                                 phi_dot * dt, theta_dot * dt, psi_dot * dt);

   Hprevious = Hproduct1;

   // translate and rotate and update time in RigidBody state ----------------//
   // note world coordinate system is used by RigidBody:

   set_angular_velocities (phi_dot, theta_dot, psi_dot);

   set_linear_velocities  (  x_dot,     y_dot,   z_dot);
              
   set_time_of_posture    (current_uuv_time); 

   update_Hmatrix         (dt);

   if (TRACE) 
   {
       cout << "incremental hmatrix = ";
       Hincremental.print_hmatrix ();
       cout << "revised1 hmatrix = ";
       Hrevised1.print_hmatrix ();
       cout << "revised2 hmatrix = ";
       Hrevised2.print_hmatrix ();
       cout << "product1 hmatrix = ";
       Hproduct1.print_hmatrix ();

       cout << "original hmatrix = ";
       hmatrix.print_hmatrix ();
   }

  // cout << "substituting product1 hmatrix" << endl;
   hmatrix = Hproduct1;

   // ------------------------------------------------------------------------

   // Save body-coordinate-system velocities for the next loop:

   U = new_velocity [SURGE];
   V = new_velocity [SWAY ];
   W = new_velocity [HEAVE];
   P = new_velocity [Roll ];
   Q = new_velocity [Pitch];
   RR = new_velocity [YAW  ];

// cout << "world U =" << U << ", x_dot     = " << x_dot     << endl;
// cout << "world V =" << V << ", y_dot     = " << y_dot     << endl;
// cout << "world W =" << W << ", z_dot     = " << z_dot     << endl;
// cout << "world P =" << P << ", phi_dot   = " << phi_dot   << endl;
// cout << "world Q =" << Q << ", theta_dot = " << theta_dot << endl;
// cout << "world R =" << RR << ", psi_dot   = " << psi_dot   << endl;
              
   // ------------------------------------------------------------------------
   // update all hydrodynamics-model-provided state variables in AUV_globals.h
   //        prior to retransmittal to AUV via AUVsocket

   AUV_time      = current_uuv_time; // mission time

   AUV_x         = x_value     (); //  x    position in world coordinates
   AUV_y         = y_value     (); //  y    position in world coordinates
   AUV_z         = z_value     (); //  z    position in world coordinates
   AUV_phi       = phi_value   (); //  roll  posture in world coordinates
   AUV_theta     = theta_value (); //  pitch posture in world coordinates
   AUV_psi       = psi_value   (); //  yaw   posture in world coordinates

   AUV_u   = new_velocity [SURGE]; //  surge  linear velocity along x-axis
   AUV_v   = new_velocity [SWAY ]; //  sway   linear velocity along y-axis
   AUV_w   = new_velocity [HEAVE]; //  heave  linear velocity along x-axis
   AUV_p   = new_velocity [Roll ]; //  roll  angular velocity about x-axis
   AUV_q   = new_velocity [Pitch]; //  pitch angular velocity about y-axis
   AUV_r   = new_velocity [YAW  ]; //  yaw   angular velocity about z-axis

   AUV_u_dot     = u_dot;          //     linear acceleration along x-axis
   AUV_v_dot     = v_dot;          //     linear acceleration along y-axis
   AUV_w_dot     = w_dot;          //     linear acceleration along x-axis
   AUV_p_dot     = p_dot;          //    angular acceleration about x-axis
   AUV_q_dot     = q_dot;          //    angular acceleration about y-axis
   AUV_r_dot     = r_dot;          //    angular acceleration about z-axis

   AUV_x_dot     = x_dot;          //      Euler velocity along North-axis
   AUV_y_dot     = y_dot;          //      Euler velocity along  East-axis
   AUV_z_dot     = z_dot;          //      Euler velocity along Depth-axis
   AUV_phi_dot   = phi_dot;        // Euler rotation rate about North-axis
   AUV_theta_dot = theta_dot;      // Euler rotation rate about  East-axis
   AUV_psi_dot   = psi_dot;        // Euler rotation rate about Depth-axis
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
//NEW
void UUVBody:: set_position_to( float Xposit,float Yposit,float Zposit)
{
   AUV_x         = Xposit; //  x    position in world coordinates
   AUV_y         = Yposit; //  y    position in world coordinates
   AUV_z         = Zposit; //  z    position in world coordinates

Vector3D vector1 = Vector3D (AUV_x, AUV_y, AUV_z);

              hmatrix = Hmatrix (vector1,
                                 radians (AUV_phi), 
                                 radians (AUV_theta),  
                                 radians (AUV_psi));
              cerr<<"setting position to :"<<endl;
              hmatrix.print_hmatrix ();

              Hprevious = hmatrix;
}


//----------------------------------------------------------------------------//
#undef UDOT
#undef VDOT
#undef WDOT
#undef PDOT
#undef QDOT
#undef RDOT
#undef SURGE
#undef SWAY
#undef HEAVE
#undef Roll
#undef Pitch
#undef YAW

//----------------------------------------------------------------------------//


