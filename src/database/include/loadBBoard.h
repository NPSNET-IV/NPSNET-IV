#ifndef __LOADBBOARD_H
#define __LOADBBOARD_H


enum BBflagsEnum {BNONE,MODULATE,DECAL,AXIS,POINT};
const int MAXBVERTS = 4;

int readBBfile(char filename[],BBflagsEnum *texapp, BBflagsEnum *btype,
               float rotvec[3],float vert[MAXBVERTS][3],
               char texname[],float texcoord[MAXBVERTS][2]);

pfGroup * LoadBBoard(char filename[]);
pfBillboard * placeBBoard (pfGroup *root,pfGroup *bboard,float x,float y,float z);

#endif

