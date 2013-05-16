#ifndef _NPS_SIGNAL_H_
#define _NPS_SIGNAL_H_

#include "jointmods.h"

#define MAX_SIGNALS 5

int signalType(int);


class signalClass {         // Class for signal info 

protected:
   int index;              // Array of signal indices  
   int status;             // TRUE if starting to execute signal
   float start_time;       // start time variable for hand signals
   int pdu_sent;           // TRUE if sent PDU with signal info
   int queued;             // count of signal(s) queued
   int queuedSignals[MAX_SIGNALS]; // array of queued signals

public:
   signalClass();
   void set_signal(int, JointMods*, int);
   void set_index(int signalIndex) {index = signalIndex; status = DONE;}
   int check_signal(JointMods*, int);
   int get_index() {return (index);} 
   int get_type() {return(signalType(index));}
   int get_pduFlag() {return (pdu_sent);}
   void set_status(int value) 
      {status = value;}
   int get_status() { return (status);}
   void reset() {index = NOSIGNAL; status = DONE;}
   void set_start(float time) {start_time = time;}
   void set_pduFlag(int flag) {pdu_sent = flag;}

};




#endif
