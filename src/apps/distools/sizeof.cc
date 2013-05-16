
#include <iostream.h>
#include <stdlib.h>

void
main ()
{
   void *pointer;

   cout << "SIZE OF STANDARD DATA TYPES (bytes)" << endl;
   cout << "-----------------------------------" << endl;

   cout << "char\t\t"    << sizeof(char)           << endl;
   cout << "short\t\t"   << sizeof(short)          << endl;
   cout << "int\t\t"     << sizeof(int)            << endl;
   cout << "long\t\t"    << sizeof(long)           << endl;
   cout << "uchar\t\t"   << sizeof(unsigned char)  << endl;
   cout << "ushort\t\t"  << sizeof(unsigned short) << endl;
   cout << "uint\t\t"    << sizeof(unsigned int)   << endl;
   cout << "ulong\t\t"   << sizeof(unsigned long)  << endl;
   cout << "float\t\t"   << sizeof(float)          << endl;
   cout << "double\t\t"  << sizeof(double)         << endl;
   cout << "ldouble\t\t" << sizeof(long double)    << endl;
   cout << "pointer\t\t" << sizeof(pointer)        << endl;
}
