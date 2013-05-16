////////////////////////////////////////////////////////////////////////////////
/*
 Program:         Vector3D.C

 Original Author: Keith Haynes

 Revisions:       Don Brutzman

 Revised:         12 FEB 94

 System:          Iris/PC

 Compiler:        ANSI C++

 Compilation:     irix> CC Vector3D.C -lm -c -g +w 

                   -c == Produce binaries only, suppressing the link phase.
                   +w == Warn about all questionable constructs.

 Description:     Vector3D class specifications and implementation

 Advisors:        Dr. Mike Zyda, Dr. Bob McGhee and Dr. Tony Healey

*/
////////////////////////////////////////////////////////////////////////////////
#include "Vector3D.h"

////////////////////////////////////////////////////////////////////////////////

//default constructor
Vector3D::Vector3D()
{
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

//constructor using three doubles
Vector3D::Vector3D(double a, double b, double c)
{
	x = a;
	y = b;
	z = c;
}

//constructor using another Vector3D
Vector3D::Vector3D(const Vector3D& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

//Assignment operator - the function must return a reference to a Vector
//instead of a Vector for assignment to work properly
Vector3D& Vector3D::operator=(const Vector3D& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

//Vector addition operator
Vector3D Vector3D::operator+(const Vector3D& v)
{
	Vector3D sum;
	sum.x = v.x + x;
	sum.y = v.y + y;
	sum.z = v.z + z;
	return sum;
}

//Vector substraction
Vector3D Vector3D::operator-(const Vector3D& v)
{
	Vector3D diff;
	diff.x = x - v.x;
	diff.y = y - v.y;
	diff.z = z - v.z;
	return diff;
}

//Vector dot product
double Vector3D::operator*(const Vector3D& v)
{
	double dot;
	dot = (v.x * x) + (v.y * y) + (v.z * z);
	return dot;
}

//scalar multiplication
Vector3D Vector3D::operator*(double n)
{
	Vector3D mult;
	mult.x = x * n;
	mult.y = y * n;
	mult.z = z * n;
	return mult;
}

//scalar division - it is the user responsibility to make sure that n is not zero
Vector3D Vector3D::operator/(double n)
{
	Vector3D result;
	result.x = x / n;
	result.y = y / n;
	result.z = z / n;
	return result;
}

//Vector cross product
Vector3D Vector3D::operator^(const Vector3D& v)
{
	Vector3D cross;
	cross.x =   (y * v.z) - (v.y * z);
	cross.y = -((x * v.z) - (v.x * z));
	cross.z =   (x * v.y) - (v.x * y);
	return cross;
}

//the << operator is to be used with output stream out
ostream& operator << (ostream& out, Vector3D& v)
{
   out << "[" << v.x << ", " << v.y << ", " << v.z << "]";
	return (out);
}

void Vector3D:: print ()
{
   cout << "[" << x << ", " << y << ", " << z << "]";
}

//allows access to the components of the Vector3D.  it must return a reference
//in order for assignment to work
double& Vector3D::operator[](int n)
const
{
   static double result;

	if (n == 1)
	{
      result = x;
		return result;
	}
	if (n == 2)
	{
      result = y;
		return result;
	}
	if (n == 3)
	{
      result = z;
		return result;
	}
	cout << "Warning: Vector3D[" << n << "] is an invalid accessor"
	     << " (only 1..3 allowed), returning value of 0.0" << endl;

	static double dummy_value = 0.0;
	return dummy_value;
}

//returns the magnitude of the Vector
double Vector3D::magnitude()
{
	return sqrt((x * x) + (y * y) + (z * z));
}

//normalizes the Vector to one
void Vector3D::normalize()
{
	double m = sqrt((x * x) + (y * y) + (z * z));
	if (m)
	{
	x = x / m;
	y = y / m;
	z = z / m;
	}
}

//normalize the Vector to d
void Vector3D::normalize(double d)
{
	double m = sqrt((x * x) + (y * y) + (z * z));
	if (m)
	{
	x = d * x / m;
	y = d * y / m;
	z = d * z / m;
	}
}

////////////////////////////////////////////////////////////////////////////////





