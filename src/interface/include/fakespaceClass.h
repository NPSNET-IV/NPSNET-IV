#ifndef __FAKESPACE_CLASS_DOT_H__
#define __FAKESPACE_CLASS_DOT_H__

#include <ulocks.h>       // for datalock definitions

#include "input_device.h" // for input_device class definition
#include "nps_device.h"   // for enum definitions for devices

#ifndef TRUE
#define TRUE 1
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#endif // FALSE

typedef struct {
   pfCoord pos;
   int button1;
   int button2;
} fromFakespace;

typedef struct {
   char tty[32];
   char calfile[128];
} fsInfoStruct;


const int FS_BUFFER_SIZE = 1;
const int FS_OUTBUFF = 0;


void f_sig_handler (int);

class fakespaceClass : virtual public input_device {

    friend void fakespace_update (void *tempThis);

public:

   fakespaceClass (char *);   // Constructor
   virtual ~fakespaceClass (); // Destructor

   virtual int exists () { return TRUE; } // return uniport_detect; }

   virtual  void get_all_inputs (pfChannel *);

   virtual int button_pressed (const NPS_BUTTON, int &);
   virtual int button_pressed (const char, int &) {return FALSE;}

   virtual int get_multival (const NPS_MULTIVAL, void *);

protected:

   fromFakespace recv_buffer[FS_BUFFER_SIZE];
   virtual void print_buffers ();
   char tty[32];
   char calfile[128];
   int boom_opened_ok;

};

#endif // __FAKESPACE_CLASS_DOT_H__
