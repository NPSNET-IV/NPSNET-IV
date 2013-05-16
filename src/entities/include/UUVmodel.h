// File: <UUVmodel.h>

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
 *
 *
 *References:      Healey, Anthony J. and Lienard, David, "Multivariable
 *                 Sliding Mode Control for Autonomous Diving and Steering
 *                 of Unmanned Underwater Vehicles," IEEE Journal of Oceanic
 *                 Engineering, vol. 18 no. 3, July 1993, pp. 327-339,
 *
 *                 Lewis, Edward V., editor, _Principles of Naval
 *                 Architecture volume III_, second revision, The Society of
 *                 Naval Architects and Marine Engineers, Jersey City
 *                 New Jersey, 1988, pp. 188-190 and 418-423.
 *
 *                 Gertler, Morton and Hagen, Grant R., _Standard Equations
 *                 of Motion for Submarine Simulation_, Naval Ship 
 *                 Research and Development Center (NSRDC) Research and
 *                 Development Report 2510, Washington DC, June 1967.
 *
 *                 Smith, N.S., Crane J.W. and Summey, D.C., _SDV Simulator
 *                 Hydrodynamic Coefficients_, Naval Coastal Systems Center 
 *                 (NCSC), Panama City Florida, June 1978.  Declassified.
 *
 *                 Marco, David.  "Slow Speed Control and Dynamic Positioning
 *                 of an Autonomous Vehicle," Ph.D. dissertation,  
 *                 Naval Postgraduate School, Monterey California, March 1995.
 *
 *                 Bahrke, Fredric G., "On-Line Identificaton of the Speed,
 *                 Steering and Diving Response Parameters of an Autonomous
 *                 Underwater Vehicle from Experimental Data," Master's Thesis,
 *                 Naval Postgraduate School, Monterey California, March 1992.
 *
 *                 Warner, David C., "Design, Simulation and Experimental
 *                 Verification of a Computer Model and Enhanced Position
 *                 Estimator for the NPS AUV II," Master's Thesis,
 *                 Naval Postgraduate School, Monterey California, December 1991.
 *
 * Notes:          const definitions are for software engineering reliability
 *                 they can be changed to variables if coefficient modification
 *                 becomes desirable
 *
 */

////////////////////////////////////////////////////////////////////////////////

#ifndef UUVBODY_C
#define UUVBODY_C    // prevent errors if multiple #includes present

//cut #include "SonarModel.C"

#include "SUBmodel.h"




#include "RigidBody.h"

////////////////////////////////////////////////////////////////////////////////

class UUVBody : public RigidBody
{
private:

Hmatrix Hprevious ;

// member data fields

   double    current_uuv_time;

   double    U;                     // Linear & angular velocities
   double    V;                     //   in body coordinates 
   double    W;                     //   (note:  RigidBody is world coordinates)
   double    P;
   double    Q;
   double    RR;

   double    u_dot;                 // Linear & angular accelerations
   double    v_dot;                 //   in body coordinates
   double    w_dot; 
   double    p_dot;
   double    q_dot;
   double    r_dot;

   double    x_dot;                 // Euler velocities, world coordinates
   double    y_dot;
   double    z_dot;

   double    phi_dot;               // Euler rotation rates, world coordinates
   double    theta_dot;
   double    psi_dot;

  

   double    mass [6][6];           // mass matrix acceleration coefficients

   double    mass_inverse [6][6];   // mass matrix inverted

   double    rhs [6];               // right-hand-sides of equations of motion

   double    new_acceleration [6];  // product [mass_inverse][rhs]

   double    new_velocity [6];      // (averaged_accelerations * dt) 
                                    // + old_velocities

   // AUV telemetry state vector is located in "AUV_globals.h" file

public:

// member constructor and destructor functions

   UUVBody                                   ();

  ~UUVBody                                   ()     { /* null body */ }

// operators

   void       multiply6x6_6                  (double   left_matrix   [6][6],
                                              double   right_matrix  [6],
                                              double   result_matrix [6]);

   void       multiply6x6_6x6                (double   left_matrix   [6][6],
                                              double   right_matrix  [6][6],
                                              double   result_matrix [6][6]);

   friend ostream& operator <<               (ostream& out, UUVBody& uuvb);

// inspection methods

   void       print_uuvbody                  (); 

   void       print_matrix6                  (double output_matrix [6]);
   void       print_matrix6x6                (double output_matrix [6][6]);

   double     current_uuv_time_value         () const;

   double     u_dot_value                    () const;
   double     v_dot_value                    () const;
   double     w_dot_value                    () const;
   double     p_dot_value                    () const;
   double     q_dot_value                    () const;
   double     r_dot_value                    () const;

   double     x_dot_value                    () const;
   double     y_dot_value                    () const;
   double     z_dot_value                    () const;

// modifying methods
  
   void       UUVBody_initialize             ();
 
   void       UUVBody_initialize             (float Xposit,
                                              float Yposit,
                                              float Zposit); //NEW

   void       set_current_uuv_time           (double new_current_uuv_time);

   void       set_accelerations              (double new_u_dot,
                                              double new_v_dot,
                                              double new_w_dot,
                                              double new_p_dot,
                                              double new_q_dot,
                                              double new_r_dot);

   void       set_linear_accelerations       (double new_u_dot,
                                              double new_v_dot,
                                              double new_w_dot);

   void       set_angular_accelerations      (double new_p_dot,
                                              double new_q_dot,
                                              double new_r_dot);

           // vehicle socket communications with dynamics
   void       loop_test_with_execution_level ();

           // vehicle socket communications with no dynamics
   void       dead_reckon_test_with_execution_level (); 
                                               
   void       swap                           (double * a, double * b);

   double     square                         (double value);

   double     nonzerosign                    (double value);

   void       clamp                          (double * clampee,  
                                              double   absolute_min, 
                                              double   absolute_max,  
                                              char   * name);

   double     epsilon                        ();

   void       invert_mass_matrix             ();

   void       test_invert_matrix             ();

   void       calculate_mass_matrix          ();

   void       integrate_equations_of_motion  ();

   void       set_position_to( float Xposit,float Yposit,float Zposit);  //NEW


// AUV-provided state variables - - - - - - - - - - - - - - - - - - - - - - - - 

 double AUV_time           ; // mission time
 double AUV_delta_rudder   ; // positive is bow rudder to starboard
 double AUV_delta_planes   ; // positive is bow planes to starboard
 double AUV_port_rpm       ; // propellor revolutions per minute
 double AUV_stbd_rpm       ; // propellor revolutions per minute

 double AUV_bow_vertical   ; // thruster volts 24V = 3820 rpm no load
 double AUV_bow_lateral    ; // thruster volts 24V = 3820 rpm no load
 double AUV_stern_vertical ; // thruster volts 24V = 3820 rpm no load
 double AUV_stern_lateral  ; // thruster volts 24V = 3820 rpm no load

 double AUV_depth_cell     ; // pressure sensor, units are psid
 double AUV_heading        ; // gyrocompass in degrees
 double AUV_roll_angle     ; // matches intertial sensor onboard AUV
 double AUV_pitch_angle    ; // matches intertial sensor onboard AUV 
 double AUV_roll_rate      ; // matches intertial sensor onboard AUV 
 double AUV_pitch_rate     ; // matches intertial sensor onboard AUV 
 double AUV_yaw_rate       ; // matches intertial sensor onboard AUV static int    AUV_hour           = 0;   // internal AUV OS-9 system time, unused
 int    AUV_minute         ;  
 int    AUV_second         ;  


// Hydrodynamics-model-provided state variables - - - - - - - - - - - - - - - - 

 double AUV_x                ; // x    position in world coordinates
 double AUV_y              ; // y    position in world coordinates
 double AUV_z              ; // z    position in world coordinates
 double AUV_phi            ; // roll  posture in world coordinates
 double AUV_theta          ; // pitch posture in world coordinates
 double AUV_psi            ; // yaw   posture in world coordinates

 double AUV_x_dot          ; //      Euler velocity along North-axis
 double AUV_y_dot          ; //      Euler velocity along  East-axis
 double AUV_z_dot          ; //      Euler velocity along Depth-axis
 double AUV_phi_dot        ; // Euler rotation rate about North-axis
 double AUV_theta_dot      ; // Euler rotation rate about  East-axis
 double AUV_psi_dot        ; // Euler rotation rate about Depth-axis

 double AUV_u              ; // surge  linear velocity along x-axis
 double AUV_v              ; // sway   linear velocity along y-axis
 double AUV_w              ; // heave  linear velocity along x-axis
 double AUV_p              ; // roll  angular velocity about x-axis
 double AUV_q              ; // pitch angular velocity about y-axis
 double AUV_r              ; // yaw   angular velocity about z-axis

 double AUV_u_dot          ; //    linear acceleration along x-axis
 double AUV_v_dot          ; //    linear acceleration along y-axis
 double AUV_w_dot          ; //    linear acceleration along x-axis
 double AUV_p_dot          ; //   angular acceleration about x-axis
 double AUV_q_dot          ; //   angular acceleration about y-axis
 double AUV_r_dot          ; //   angular acceleration about z-axis

 double AUV_oceancurrent_u ; //  Ocean current rate along North-axis
 double AUV_oceancurrent_v ; //  Ocean current rate along  East-axis
 double AUV_oceancurrent_w ; //  Ocean current rate along Depth-axis


// Prior time loop saved variables - - - - - - - - - - - - - - - - - - - - - - - 

 double AUV_time_prior     ; // mission time

 double AUV_x_prior        ; // x    position in world coordinates
 double AUV_y_prior        ; // y    position in world coordinates
 double AUV_z_prior        ; // z    position in world coordinates

 double AUV_phi_prior      ; // roll  posture in world coordinates
 double AUV_theta_prior    ; // pitch posture in world coordinates
 double AUV_psi_prior      ; // yaw   posture in world coordinates


// Sonar model provided state variables  - - - - - - - - - - - - - - - - - - - -

 double AUV_ST1000_bearing ; // ST_1000 conical pencil beam bearing
 double AUV_ST1000_range   ; // ST_1000 conical pencil beam range
 double AUV_ST1000_strength; // ST_1000 conical pencil beam strength
  double AUV_ST1000_x_offset; // ST_1000 longitudinal location in feet

 double AUV_ST725_bearing  ; // ST_725  1 x 24  sector beam bearing
 double AUV_ST725_range    ; // ST_725  1 x 24  sector beam range
 double AUV_ST725_strength ; // ST_725  1 x 24  sector beam strength
  double AUV_ST725_x_offset ; // ST_725  longitudinal location in feet


// Model-wide global variables and constants - - - - - - - - - - - - - - - - - - 

 int    TRACE              ;          /* 1 = trace on, 0 = trace off */

#define TRUE  1
#define FALSE 0

#define PI    3.1415926535897932




};

#endif  // UUVBODY_C 




