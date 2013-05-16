//#define NODIGUY

#include <iostream.h>
#include <string.h>
#include "diguyTypeLookup.h"
#ifndef NODIGUY
#include "libbdihml.h"
#include "libhmlchar.h"

extern char* diguy_bdu_m16_equipment[];
extern char* diguy_bdu_m203_equipment[];
extern char* diguy_bdu_ak47_equipment[];
extern char* diguy_dcu_m16_equipment[];
extern char* diguy_dcu_m203_equipment[];
extern char* diguy_dcu_ak47_equipment[];
extern char* diguy_land_warrior_equipment[];
#endif

char *diguy_hostile_equipment[] = {"soldier_bdu",
                                  "ak47",
                                  NULL};
char *diguy_friendly_m16_equipment[] = {"land_warrior",
                                      "m16",
                                      NULL};
char *diguy_friendly_m203_equipment[] = {"land_warrior",
                                        "m203",
                                        NULL};

char *diguy_lw_modified[] = {"land_warrior",
                             "lw_equip:eye",
                             "lw_equip:mic",
                             "lw_equip:frame",
                             "lw_equip:butt",
                             "lw_equip:canteen1",
                             "lw_equip:canteen2",
                             "lw_glasses",
                             "lw_m4c",
                             "lw_m4c_attachments:cam2",
                             "lw_m4c_attachments:laser",
                             NULL};


void print_quip (char **);

diguyTypeLookup::diguyTypeLookup ()
{
   for (int ix = 0; ix <= LU_ARRAY_SIZE; ix++) {
      stuff[ix].equipment = (char **)NULL;
   }
#ifndef NODIGUY
/*
   cerr << "diguy_bdu_m16_equipment:" << endl;
   print_quip (diguy_bdu_m16_equipment);
   cerr << "diguy_bdu_m203_equipment:" <<  endl;
   print_quip (diguy_bdu_m203_equipment);
   cerr << "diguy_bdu_ak47_equipment" << endl;
   print_quip (diguy_bdu_ak47_equipment);
   cerr << "diguy_land_warrior_equipment" << endl;
   print_quip (diguy_land_warrior_equipment);
   cerr << "diguy_lw_modified" << endl;
   print_quip (diguy_lw_modified);
*/
#endif
}

int
diguyTypeLookup::addType (int place, char *theType)
{

   int result = FALSE;
#ifndef NODIGUY
   if (place > LU_ARRAY_SIZE) {
      cerr << "Out of Range in diguyTypeLookup::addType." << endl;
   }
   else {
      if (!strcmp ("us_diguy", theType)) {
         if (stuff[place].equipment) {
            cerr << "Error: Di Guy equipment already deffined for this"
                 << " enumeration:" << place << endl;
         }
         else {
            result = TRUE;
            stuff[place].equipment = diguy_friendly_m16_equipment;
//diguy_bdu_m16_equipment;
         }
      }
      else if (!strcmp ("m203_diguy", theType)) {
         if (stuff[place].equipment) {
            cerr << "Error: Di Guy equipment already deffined for this"
                 << " enumeration:" << place << endl;
         }
         else {
            result = TRUE;
            stuff[place].equipment = diguy_friendly_m203_equipment;
// diguy_bdu_m203_equipment;
         }
      }
      else if (!strcmp ("dcu_diguy", theType)) {
         if (stuff[place].equipment) {
            cerr << "Error: Di Guy equipment already deffined for this"
                 << " enumeration:" << place << endl;
         }
         else {
            result = TRUE;
            stuff[place].equipment = diguy_dcu_m16_equipment;
         }
      }
      else if (!strcmp ("dcu_m203_diguy", theType)) {
         if (stuff[place].equipment) {
            cerr << "Error: Di Guy equipment already deffined for this"
                 << " enumeration:" << place << endl;
         }
         else {
            result = TRUE;
            stuff[place].equipment = diguy_dcu_m203_equipment;
         }
      }
      else if (!strcmp ("hostile_diguy", theType)) {
         if (stuff[place].equipment) {
            cerr << "Error: Di Guy equipment already deffined for this"
                 << " enumeration:" << place << endl;
         }
         else {
            result = TRUE;
            stuff[place].equipment = diguy_bdu_ak47_equipment;
         }
      }
      else if (!strcmp ("dcu_hostile_diguy", theType)) {
         if (stuff[place].equipment) {
            cerr << "Error: Di Guy equipment already deffined for this"
                 << " enumeration:" << place << endl;
         }
         else {
            result = TRUE;
            stuff[place].equipment = diguy_dcu_ak47_equipment;
         }
      }
      else if (!strcmp ("lw_diguy", theType)) {
         if (stuff[place].equipment) {
            cerr << "Error: Di Guy equipment already deffined for this"
                 << " enumeration:" << place << endl;
         }
         else {
            result = TRUE;
            stuff[place].equipment = diguy_lw_modified;
// diguy_land_warrior_equipment;
         }
      }
      else {
         if (stuff[place].equipment) {
            cerr << "Error: Di Guy equipment already deffined for this"
                 << " enumeration:" << place << endl;
         }
         else {
            cerr << "Unkown DI Guy type: " << theType << endl;
            result = TRUE;
            stuff[place].equipment = diguy_friendly_m16_equipment;
//diguy_bdu_m16_equipment;
         }
      }
   }
#endif
   return result;
}

char **
diguyTypeLookup::lookUpType (int place)
{
   return stuff[place].equipment;
}

void print_quip (char ** quip)
{

   char **temp = quip;

   while (*temp != NULL) {
      cerr << "\t" <<  *temp << endl;
      temp++;
   }
   cerr << endl;
}
