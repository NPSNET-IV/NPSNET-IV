#ifndef __UNIPORT_CLASS_DOT_H__
#define __UNIPORT_CLASS_DOT_H__

#include "iportClass.h"   // for input_device class definition
#include "nps_device.h"   // for enum definitions for devices
#include "DI_comm.h"      // for uni-port communication structures
#include "Mcast.h"        // for McastGroup definition

#ifndef TRUE
#define TRUE 1
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#endif // FALSE

// The uniportClass is derived from the base class iportClass 
// The uniportClass listens for a multicast packet
// from the physical Uni-Port device. This packet contains the Uni-Port's
// location and posture in the VE.  The uniportClass then responds to the 
// Uni-Port with a broadcast package conaining ground normal 
// and a status flag. All spelling mistakes are mine... rb

class uniportClass : public iportClass {

  // This functions is sproc'd.  It reads and writes to the Uni-Port.
  // For every packet it recives, it gives a response.
   friend void uniport_update (void *tempThis);

public:

   uniportClass ();  // Constructor
   ~uniportClass (); // Destructor


private:
   
   McastGroup     mcg;
   int            talk_back;
   int            uniport_detect;

};

#endif // __UNIPORT_CLASS_DOT_H__
