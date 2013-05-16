#ifndef TRANSPORT_CLASS_DOT_H
#define TRANSPORT_CLASS_DOT_H

#include "infoClass.h"
#include <pf.h>

class npsTransportInfo : public npsInfo {


   protected:
      pfVec3 min;
      pfVec3 max;
      pfCoord pos;
      int mode;

   public:
      npsTransportInfo (float, float, float,
                        float, float, float,
                        float, float, float,
                        float, float, float,
                        int);
     ~npsTransportInfo (){;}
      int in_zone (pfVec3);
      void get_point (pfCoord &);
      void get_mode (int &);
      npsInfo *is_transport () {return this;}
};

#endif // TRANSPORT_CLASS_DOT_H
