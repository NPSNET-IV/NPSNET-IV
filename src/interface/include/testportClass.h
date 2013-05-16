#ifndef __TESTPORT_CLASS_DOT_H__
#define __TESTPORT_CLASS_DOT_H__

#include "iportClass.h" // for input_device class definition
#include "ptpClass.h"   // for ptpClass definition
#include "nps_device.h" // for enum definitions for devices

#ifndef TRUE
#define TRUE 1
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#endif // FALSE


class testportClass : public iportClass {

  // This functions is sproc'd.  It reads and writes to the Uni-Port.
  // For every packet it recives, it gives a response.
   friend void testport_update (void *tempThis);

public:

   testportClass (const char *);  // Constructor
   ~testportClass (); // Destructor

private:

   long      nap_interval;
   ptpClass *ptp;
   
};

#endif // __TESTPORT_CLASS_DOT_H__
