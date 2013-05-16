#ifndef INFO_CLASS_DOT_H
#define INFO_CLASS_DOT_H

class npsInfo {

   public:
      npsInfo (){;}
      ~npsInfo (){;}
      virtual npsInfo *is_transport () {return NULL;}
      virtual int in_zone (pfVec3) {return FALSE;}
};

#endif // INFO_CLASS_DOT_H
