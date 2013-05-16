#ifndef AUTOMATE_CLASS_DOT_H
#define AUTOMATE_CLASS_DOT_H

#include <pf.h>
#include "infoClass.h"

class npsAutomate {

   private:
      void create_list (const char *);

      int       infoNumber;
      int       infoSize;
      npsInfo  *info[50];
      

   public:
      npsAutomate (const char *);
     ~npsAutomate ();
      npsInfo* find_info (pfVec3);

};

#endif // AUTOMATE_CLASS_DOT_H
