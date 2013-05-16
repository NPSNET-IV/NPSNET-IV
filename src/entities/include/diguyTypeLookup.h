#ifndef DIGUY_TYPE_LOOK_UP_DOT_H
#define DIGUY_TYPE_LOOK_UP_DOT_H

const int LU_ARRAY_SIZE = 255;

typedef struct _luStorage {
   char **equipment;
} luStorage;

#ifndef TRUE
#define TRUE (int)1
#endif

#ifndef FALSE
#define FALSE (int)NULL
#endif

class diguyTypeLookup {

   private:
      luStorage stuff[LU_ARRAY_SIZE + 1];
   
   public:
      diguyTypeLookup ();
     ~diguyTypeLookup () {;}
      
      int addType (int, char *);
      char **lookUpType (int);
};

#endif
