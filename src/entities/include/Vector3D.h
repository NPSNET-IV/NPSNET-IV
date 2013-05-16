// File: <Vector3D.h>

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

#ifndef VECTOR3D_C
#define VECTOR3D_C      // prevent errors in multiple #includes present

#include <iostream.h>
#include <iomanip.h>
#include <math.h>

class Vector3D
{
// member data fields

	double x;
	double y;
	double z;

public:

// member constructor and destructor functions

        Vector3D               ();
        Vector3D               (double a, double b, double c);
        Vector3D               (const Vector3D&);
       ~Vector3D ()            { /* null body */ }

// operators

        Vector3D & operator =  (const Vector3D&);
        Vector3D   operator +  (const Vector3D&);
        Vector3D   operator -  (const Vector3D&);
        double 	   operator *  (const Vector3D&);  // dot product
        Vector3D   operator *  (double);           // scalar multiplication
        Vector3D   operator /  (double);           // scalar division
        Vector3D   operator ^  (const Vector3D&);  // cross product
        double &   operator [] (int) const;


// inspection methods

        double     magnitude   ();
        friend     ostream & operator << (ostream& os, Vector3D& v);
        void       print ();

// modifying methods

        void 	   normalize   ();
        void       normalize   (double);
};
#endif   // #ifndef VECTOR3D_C
