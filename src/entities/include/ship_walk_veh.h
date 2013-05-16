
#ifndef __NPS_SHIP_WALK_VEH__
#define __NPS_SHIP_WALK_VEH__

#include "ship_veh.h"
#include "casualties.h"

const unsigned int HIRES_DATA_PDU_SHIP_TAG = ~(0xFFFFFFFF & 0x5);

enum {
   DR0 = 0, DR1, DR2, DR3, DR4, DR5, DR6, DR7,
            VL0, VL1, NZ0, NZ1, NZ2, NZ3,
            STL, FOL, FIR, HAL, VEN,
   NUM_SHIP_HIRES_VALUES
};

const unsigned int DR0_TAG = ~(0xFFFFFFFF & 0x01);
const unsigned int DR1_TAG = ~(0xFFFFFFFF & 0x02);
const unsigned int DR2_TAG = ~(0xFFFFFFFF & 0x03);
const unsigned int DR3_TAG = ~(0xFFFFFFFF & 0x04);
const unsigned int DR4_TAG = ~(0xFFFFFFFF & 0x05);
const unsigned int DR5_TAG = ~(0xFFFFFFFF & 0x06);
const unsigned int DR6_TAG = ~(0xFFFFFFFF & 0x07);
const unsigned int DR7_TAG = ~(0xFFFFFFFF & 0x08);
const unsigned int VL0_TAG = ~(0xFFFFFFFF & 0x09);
const unsigned int VL1_TAG = ~(0xFFFFFFFF & 0x0A);
const unsigned int NZ0_TAG = ~(0xFFFFFFFF & 0x0B);
const unsigned int NZ1_TAG = ~(0xFFFFFFFF & 0x0C);
const unsigned int NZ2_TAG = ~(0xFFFFFFFF & 0x0D);
const unsigned int NZ3_TAG = ~(0xFFFFFFFF & 0x0E);
const unsigned int STL_TAG = ~(0xFFFFFFFF & 0x0F);
const unsigned int FOL_TAG = ~(0xFFFFFFFF & 0x10);
const unsigned int FIR_TAG = ~(0xFFFFFFFF & 0x11);
const unsigned int HAL_TAG = ~(0xFFFFFFFF & 0x12);
const unsigned int VEN_TAG = ~(0xFFFFFFFF & 0x13);


//************************************************************************//
//*************** class SHIP_WALK_VEH ************************************//
//************************************************************************//
class SHIP_WALK_VEH : public SHIP_VEH {

protected:
   pfGroup      	*Locator;
   pfSwitch     	*vehRamp;
   CASUALTY_STEAM 	*steam_cas;
   CASUALTY_OIL_LEAK 	*oil_cas;
   CASUALTY_FIRE 	*fire_cas;

   fixedDatumRecord hiResObjects[NUM_SHIP_HIRES_VALUES];
   int okToSendHiRes;
   int okToUpdateHiRes;
   unsigned int numHiResObjects;
   float hiResTimeStamp;

   void ObjectRelatedEvent(pfNode *node, int direction);

public:
   SHIP_WALK_VEH(int id, int atype, ForceID the_force);
   ~SHIP_WALK_VEH();

   virtual SHIP_VEH *is_WALK_CLASS() {return this;}
   virtual void create_entity(int atype, ForceID the_force);
   void changestatus(unsigned int); // change the appearance status
   movestat move();
   movestat moveDR(float, float);
   virtual void class_specific_keyboard();
   virtual void mount(VEHICLE *, int);
   virtual void dismount(int);

   void update_locator();
   void update_ship_picking();
   void startSteamLeak();
   void startOilLeak();

   virtual void apply_hi_res ();
   virtual void set_hi_res (fixedDatumRecord *, int);
#ifndef NOHIRESNET
   void ok_to_send () { okToSendHiRes = TRUE; }
#endif // NOHIRESNET
   void ok_to_update () { okToUpdateHiRes = TRUE; }
#ifndef NOHIRESNET
   void set_hi_res_time_stamp (float the_time) { hiResTimeStamp = the_time; }
#endif // NOHIRESNET
};

#endif

