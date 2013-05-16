#ifndef HUMAN_CONSTANTS_DOT_H
#define HUMAN_CONSTANTS_DOT_H

const float        MAX_SAFE_FALL_DISTANCE = -10.0f; // this is in meters
const float        MAX_STEP_HIGHT = 0.75f; // this is in meters

const float        HI_RES_TIMEOUT = 0.5f;

const unsigned int MIN_NUM_HIRES_JOINTS = 14;
const unsigned int HIRES_DATA_PDU_TAG = ~(0xFFFFFFFF & 0x3);
const unsigned int HIRES_DATA_PDU_MOUNT_TAG = ~(0xFFFFFFFF & 0x4);
const unsigned int HIRES_DATA_PDU_MIMIC_TAG = ~(0xFFFFFFFF & 0x5);

enum mimicEnum { EPX = 0, EPY, EPZ, 
                 EPH,     EPP, EPR, 
                 RVX,     RVY, RVZ, 
                 RVH,     RVP, RVR,
                 HRTAR};

// Entity Posture Tag
const unsigned int EPX_TAG = ~(0xFFFFFFFF & 0x01);
const unsigned int EPY_TAG = ~(0xFFFFFFFF & 0x02);
const unsigned int EPZ_TAG = ~(0xFFFFFFFF & 0x03);
const unsigned int EPH_TAG = ~(0xFFFFFFFF & 0x04);
const unsigned int EPP_TAG = ~(0xFFFFFFFF & 0x05);
const unsigned int EPR_TAG = ~(0xFFFFFFFF & 0x06);

// Rifle Tag
const unsigned int RVX_TAG = ~(0xFFFFFFFF & 0x07);
const unsigned int RVY_TAG = ~(0xFFFFFFFF & 0x08);
const unsigned int RVZ_TAG = ~(0xFFFFFFFF & 0x09);
const unsigned int RVH_TAG = ~(0xFFFFFFFF & 0x0A);
const unsigned int RVP_TAG = ~(0xFFFFFFFF & 0x0B);
const unsigned int RXR_TAG = ~(0xFFFFFFFF & 0x0C);

const unsigned int T_NONE  = ~(0xFFFFFFFF & 0x0D);
const unsigned int T_BLUE  = ~(0xFFFFFFFF & 0x0E);
const unsigned int T_RED   = ~(0xFFFFFFFF & 0x0F);
const unsigned int T_WHITE = ~(0xFFFFFFFF & 0x10);
const unsigned int T_OTHER = ~(0xFFFFFFFF & 0x11);


#endif // HUMAN_CONSTANTS_DOT_H
