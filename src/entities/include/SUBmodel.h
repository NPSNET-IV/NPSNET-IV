// File: <SUBmodel.h>

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
 /*
 Description:     Generalized hydrodynamics model constants and variables
                  for NPS AUV Phoenix

 References:      Healey, Anthony J. and Lienard, David, "Multivariable
                  Sliding Mode Control for Autonomous Diving and Steering
                  of Unmanned Underwater Vehicles," IEEE Journal of Oceanic
                  Engineering, vol. 18 no. 3, July 1993, pp. 327-339,

                  Lewis, Edward V., editor, _Principles of Naval
                  Architecture volume III_, second revision, The Society of
                  Naval Architects and Marine Engineers, Jersey City
                  New Jersey, 1988, pp. 188-190 and 418-423.

                  Gertler, Morton and Hagen, Grant R., _Standard Equations
                  of Motion for Submarine Simulation_, Naval Ship 
                  Research and Development Center (NSRDC) Research and
                  Development Report 2510, Washington DC, June 1967.

                  Smith, N.S., Crane J.W. and Summey, D.C., _SDV Simulator
                  Hydrodynamic Coefficients_, Naval Coastal Systems Center 
                  (NCSC), Panama City Florida, June 1978.  Declassified.

                  Marco, David.  "Slow Speed Control and Dynamic Positioning
                  of an Autonomous Vehicle," Ph.D. dissertation,  
                  Naval Postgraduate School, Monterey California, March 1995.

                  Bahrke, Fredric G., "On-Line Identificaton of the Speed,
                  Steering and Diving Response Parameters of an Autonomous
                  Underwater Vehicle from Experimental Data," Master's Thesis,
                  Naval Postgraduate School, Monterey California, March 1992.

                  Warner, David C., "Design, Simulation and Experimental
                  Verification of a Computer Model and Enhanced Position
                  Estimator for the NPS AUV II," Master's Thesis,
                  Naval Postgraduate School, Monterey California, December 1991.

 Notes:           const definitions are for software engineering reliability
                  they can be changed to variables if coefficient modification
                  becomes desirable

*/
////////////////////////////////////////////////////////////////////////////////

#ifndef UUVMODEL_H
#define UUVMODEL_H    // prevent errors if multiple #includes present


// #define SI         // <<<<<<<<<<<<<<<<  uncomment this statement for SI units
                      //                   otherwise standard British units used

//----------------------------------------------------------------------------//
//            term        value         units      description
//----------------------------------------------------------------------------//
const double  H       =    0.79    ; // ft         main hull height 9.50"

//----------------------------------------------------------------------------//
#ifdef SI     // Systeme International (metric) units ------------------------

const double  Weight  = 435.0*0.454; // N          Weight   (0.454 kg/lb == 1)
                   // = 197.49
const double  Buoyancy= 435.0*0.454; // N          Buoyancy (0.454 kg/lb == 1)
                   // = 197.49
const double  L       =    2.23    ; // m          characteristic length 87.625"
const double  g       =    9.81    ; // m/s^2      gravitational constant
const double  rho     = 1000.0     ; // kg/m^3     mass density of fresh water
const double  m       = Weight / g ; // N-s^2/m    vehicle mass incl. free flood
                   // =   20.131498

#define       m4_ft4   (0.305)*(0.305)*(0.305)*(0.305) // (0.305 m/ft == 1)

                                   ; //            Inertia matrix coefficients
const double  I_x     = 2.7*m4_ft4 ; // Nms^2      = I_xx = 0.023364857
const double  I_y     = 2.7*m4_ft4 ; // Nms^2      = I_yy = 0.023364857
const double  I_z     = 2.7*m4_ft4 ; // Nms^2      = I_zz = 0.023364857
const double  I_xy    =     0.0    ; // Nms^2      = I_yx
const double  I_xz    =     0.0    ; // Nms^2      = I_zx
const double  I_yz    =     0.0    ; // Nms^2      = I_zy


#undef        m4_ft4
                                     //            Centers of Gravity & Buoyancy
const double  x_G     =     0.0    ; // cm
const double  y_G     =     0.0    ; // cm
const double  z_G     =     6.1    ; // cm         Note CG below CB   Marco 0.5"
const double  x_B     =     0.0    ; // cm
const double  y_B     =     0.0    ; // cm
const double  z_B     =     0.0    ; // cm         CB at center of UUV

#else // (not SI) standard British units ---------------------------------------

const double  Weight  =  435.0     ; // lb         Weight  of ship
const double  Buoyancy=  435.0     ; // lb         Buoyancy
const double  L       = 7.30208333 ; // ft         characteristic length 87.625"
const double  g       =   32.174   ; // ft/s^2     gravitational constant
const double  rho     =    1.94    ; // slugs/ft^3 mass density of fresh water
const double  m       = Weight  / g ; // lb/ft-s^2  vehicle mass incl. free flood
                   // =   13.520234

                                   ; //            Inertia matrix coefficients
const double  I_x     =     2.7    ; // lb-ft-sec^2ft^4     =I_xx
const double  I_y     =    42.0    ; // ft^4       =I_yy
const double  I_z     =    45.0    ; // ft^4       =I_zz
const double  I_xy    =     0.0    ; // ft^4       =I_yx
const double  I_xz    =     0.0    ; // ft^4       =I_zx
const double  I_yz    =     0.0    ; // ft^4       =I_zy

                                     //            Centers of Gravity & Buoyancy
const double  x_G     = 0.125/12.0 ; // ft         0.010416667
const double  y_G     =     0.0    ; // ft
const double  z_G     = 1.07 /12.0 ; // ft         Note CG below CB
const double  x_B     = 0.125/12.0 ; // ft         0.010416667
const double  y_B     =     0.0    ; // ft
const double  z_B     =     0.0    ; // ft         CB at center of UUV

// Thruster/propeller distances from centerlines.  Note stern/port are negative.

const double  x_bow_vertical   =   1.41 ; // ft   Marco   17"  measured   13.0"
const double  x_stern_vertical = - 1.41 ; // ft   Marco - 17"  measured - 21.0"
const double  x_bow_lateral    =   1.92 ; // ft   Marco   23"  measured   19.0"
const double  x_stern_lateral  = - 1.92 ; // ft   Marco - 23"  measured - 26.0"

const double  y_port_propeller = - 0.313; // ft   Marco - 4"   measured - 3.75"
const double  y_stbd_propeller =   0.313; // ft   Marco   4"   measured   3.75"

#endif

//----------------------------------------------------------------------------//
//            Surge equation of motion coefficients                           //

const double  X_u_dot =    -2.82E-3  ; //  Linear force coefficients acting in
const double  X_v_dot =     0.0      ; //     the longitudinal body axis
const double  X_w_dot =     0.0      ; //     with respect to subscripted  
const double  X_p_dot =     0.0      ; //           motion components
const double  X_q_dot =     0.0      ; //
const double  X_r_dot =     0.0      ; //

const double  X_uu    =     0.0      ; //
const double  X_vv    =     0.0      ; // old -1.743E-2  SDV-9 holdover?
const double  X_ww    =     0.0      ; //
const double  X_pp    =     0.0      ; //
const double  X_qq    =     0.0      ; //
const double  X_rr    =     0.0      ; // old -7.53E-3   SDV-9 holdover?

const double  X_prop  =     7.78E-3  ; //   X_prop "constant" no longer applicable

const double  X_rb    =     0.283 * L; //
const double  X_rs    =    -0.377 * L; //

const double  X_uu_delta_b_delta_b = -1.018E-2 ; // drag due to  bow  plane
const double  X_uu_delta_s_delta_s = -1.018E-2 ; // drag due to stern plane
const double  X_uu_delta_r_delta_r = -1.018E-2 ; // drag due to rudder

const double  X_pr    =     0.0      ; //  (these aren't in Bahrke thesis model)
const double  X_wq    =     0.0      ; //  
const double  X_vp    =     0.0      ; //  
const double  X_vr    =     0.0      ; //  

const double  X_uq_delta_bow    =    0.0      ; //  
const double  X_uq_delta_stern  =    0.0      ; //  
const double  X_ur_delta_rudder =    0.0      ; //  
const double  X_uv_delta_rudder =    0.0      ; //  
const double  X_uw_delta_bow    =    0.0      ; //  
const double  X_uw_delta_stern  =    0.0      ; //  

const double  X_qdsn           =     0.0      ; //  no longer used in new model
const double  X_wdsn           =     0.0      ; //  no longer used in new model
const double  X_dsdsn          =     0.0      ; //  no longer used in new model

const double  speed_per_rpm = 2.0 / 700.0     ; // steady state:  0.0028571429
                                                // = (2.0 feet/sec) per 700 rpm
const double  C_d0    =     0.00778           ; //  

//----------------------------------------------------------------------------//
//            Sway  equation of motion coefficients                           //

const double  Y_u_dot =     0.0      ; //  Linear force coefficients acting in
const double  Y_v_dot =    -3.43E-2  ; //     the athwartships body axis
const double  Y_w_dot =     0.0      ; //     with respect to subscripted  
const double  Y_p_dot =     0.0      ; //           motion components
const double  Y_q_dot =     0.0      ; //
const double  Y_r_dot =    -1.78E-3  ; //

const double  Y_uu    =     0.0      ; //
const double  Y_uv    =    -1.07E-1  ; //
const double  Y_uw    =     0.0      ; //
const double  Y_up    =     0.0      ; //
const double  Y_uq    =     0.0      ; //
const double  Y_ur    =     0.0      ; // Warner = 1.187E-2;  Bahrke = 0.0

const double  Y_uu_delta_rb =  1.18E-2  ; // Marco = 1.241E-2;  Bahrke = 1.18E-2
const double  Y_uu_delta_rs =  1.18E-2  ; // Marco = 1.241E-2;  Bahrke = 1.18E-2

const double  Y_pq    =     0.0      ; //  (these aren't in Bahrke thesis model)
const double  Y_qr    =     0.0      ; //  
const double  Y_vq    =     0.0      ; //  
const double  Y_wp    =     0.0      ; //  
const double  Y_wr    =     0.0      ; //  
const double  Y_vw    =     0.0      ; //  

const double  C_dy    =     0.5      ; //

//----------------------------------------------------------------------------//
//            Heave equation of motion coefficients                           //

const double  Z_u_dot =     0.0      ; //  Linear force coefficients acting in
const double  Z_v_dot =     0.0      ; //       the vertical body axis
const double  Z_w_dot =    -9.34E-2  ; //     with respect to subscripted  
const double  Z_p_dot =     0.0      ; //           motion components
const double  Z_q_dot =    -2.53E-3  ; //
const double  Z_r_dot =     0.0      ; //

const double  Z_vv    =     0.0      ; //
const double  Z_uw    =    -7.844E-1 ; //
const double  Z_up    =     0.0      ; //
const double  Z_uq    =     0.0      ; // Marco = -7.013E-2;  Bahrke = 0.0
const double  Z_rr    =     0.0      ; //
const double  Z_pp    =     0.0      ; //

const double  Z_uu_delta_b = - 2.11E-2  ; //
const double  Z_uu_delta_s = - 2.11E-2  ; //

const double  Z_pr    =     0.0      ; //  (these aren't in Bahrke thesis model)
const double  Z_vp    =     0.0      ; //  
const double  Z_vr    =     0.0      ; //  

const double  Z_qn    =     0.0      ; //    no longer used in new model
const double  Z_wn    =     0.0      ; //    no longer used in new model
const double  Z_dsn   =     0.0      ; //    no longer used in new model 

const double  C_dz    =     0.6      ; //

//----------------------------------------------------------------------------//
//            Roll  equation of motion coefficients                           //

const double  K_u_dot =     0.0      ; //  Angular force coefficient acting
const double  K_v_dot =     0.0      ; //  about the longitudinal body axis
const double  K_w_dot =     0.0      ; //    with respect to subscripted  
const double  K_p_dot =    -2.4E-4   ; //          motion components
const double  K_q_dot =     0.0      ; //
const double  K_r_dot =     0.0      ; //

const double  K_uu    =     0.0      ; //
const double  K_uv    =     0.0      ; //
const double  K_uw    =     0.0      ; //
const double  K_up    =    -5.4E-3   ; // surge-related roll damping drag
const double  K_uq    =     0.0      ; //
const double  K_ur    =     0.0      ; //

const double  K_uu_planes = 0.0      ; //  (these aren't in Bahrke thesis model)
const double  K_pq    =     0.0      ; //
const double  K_qr    =     0.0      ; //
const double  K_vq    =     0.0      ; //
const double  K_wp    =     0.0      ; //
const double  K_wr    =     0.0      ; //
const double  K_vw    =     0.0      ; //
const double  K_prop  =     0.0      ; // K_prop "constant" no longer applicable

const double  K_pn    =     0.0      ; // no longer used in new model

const double  K_pp    =    -2.02E-2  ; // test value for p-squared damping 
                                       //   static roll damping drag
                                       // 100 * Healey minimal estimate
                                       //       (-2.02E-4)

const double  K_p     =   K_pp/57.3  ; // estimate based on quadratic term 
                                       // (K_pp) equivalent damping at 1 deg/sec

                                       
//----------------------------------------------------------------------------//
//            Pitch equation of motion coefficients                           //

const double  M_u_dot =     0.0      ; //  Angular force coefficient acting
const double  M_v_dot =     0.0      ; //  about the athwartships body axis
const double  M_w_dot =    -2.53E-3  ; //    with respect to subscripted  
const double  M_p_dot =     0.0      ; //          motion components
const double  M_q_dot =    -6.25E-3  ; //
const double  M_r_dot =     0.0      ; //

const double  M_uu    =     0.0      ; //
const double  M_vv    =     0.0      ; //
const double  M_uw    =     0.0      ; //
const double  M_pp    =     0.0      ; //
const double  M_rr    =     0.0      ; //

const double  M_uq    =    -1.53E-2  ; // surge-related pitch damping drag ***
                                       // stable pitch on large depth change?

const double  M_uu_delta_bow   =  - 0.283 * L * Z_uu_delta_b; 
                         //            note (-) Z_uu_delta_b
                         //    =    0.043602433

const double  M_uu_delta_stern =  + 0.377 * L * Z_uu_delta_s; 
                         //            note (-) Z_uu_delta_s
                         //    =  - 0.058085219

const double  M_pr    =     0.0      ; //  (these aren't in Bahrke thesis model)
const double  M_vp    =     0.0      ; //
const double  M_vr    =     0.0      ; //
const double  M_prop  =     0.0      ; // M_prop "constant" no longer applicable

const double  M_qn    =     0.0      ; // no longer used in new model
const double  M_wn    =     0.0      ; // no longer used in new model
const double  M_dsn   =     0.0      ; // no longer used in new model

const double  M_qq    =    -7.00E-3  ; // slightly larger than N_rr estimate
                                       // test value for q-squared  
                                       //   static pitch damping drag
                                       // estimated M_qq ~ K_pp * length / width
                                       // Torsiello  ~ 0.005* 7.3'/ 10.1" = .005

const double  M_q     =  M_qq / 57.3;  // estimate based on quadratic term 
                                       // (M_qq) equivalent damping at 1 deg/sec


//----------------------------------------------------------------------------//
//            Yaw   equation of motion coefficients                           //

const double  N_u_dot =     0.0      ; //  Angular force coefficient acting
const double  N_v_dot =    -1.78E-3  ; //    about the vertical body axis
const double  N_w_dot =     0.0      ; //    with respect to subscripted 
const double  N_p_dot =     0.0      ; //          motion components
const double  N_q_dot =     0.0      ; //
const double  N_r_dot =    -4.7E-4   ; //

const double  N_uu    =     0.0      ; //
const double  N_uv    =     0.0      ; //  Marco = -7.69E-3; 
const double  N_uw    =     0.0      ; //
const double  N_up    =     0.0      ; //
const double  N_uq    =     0.0      ; //
const double  N_ur    =    -3.90E-3  ; // surge-related yaw damping drag

// N_uu_delta_rb and N_uu_delta_rs not symmetric due to different moment arms

// const double N_uu_delta_rb = 1.3259   ; // Marco
// const double N_uu_delta_rs = 1.7663   ; // Marco

const double  N_uu_delta_rb =  0.283 * L * Y_uu_delta_rb; // Bahrke  = 0.02437762
const double  N_uu_delta_rs =  0.377 * L * Y_uu_delta_rs; // Bahrke  = 0.03247478

const double  N_prop  =     0.0      ; // Normally 0.0 yaw moment due to paired
                                       // counter-rotating propellors;
           // however N_prop is not zero if propellor rpms are independent
           // thus yaw equation of motion now has yaw moments due to propellers
           // and N_prop "constant" is no longer applicable

const double  N_pq    =     0.0      ; //  (these aren't in Bahrke thesis model)
const double  N_qr    =     0.0      ; //
const double  N_vq    =     0.0      ; //
const double  N_wp    =     0.0      ; //
const double  N_wr    =     0.0      ; //
const double  N_vw    =     0.0      ; //

const double  N_rr    =    -5.48E-3  ; // Torsiello value p.113 adjusted for L^5
                                       // correction;  static yaw damping drag
                                       // estimated  N_rr ~ M_qq * height/ width
                                       //                 = .040 * 10.1" / 16.5"
                                       //                 = 0.005
                                       // Torsiello:  0.005
                                       // Healey:     N_rr ~ M_qq

// alternate N_rr = 2 * 2# * 1.92' /(rho/2 L^5 * r_max * r_max) => 0.0048473244 
//                  using r_max = 16 deg/sec Torsiello which is consistent

const double  N_r     =  N_rr / 57.3;  // estimate based on quadratic term 
                                       // (N_rr) equivalent damping at 1 deg/sec

//----------------------------------------------------------------------------//

#endif     // UUVMODEL_H



