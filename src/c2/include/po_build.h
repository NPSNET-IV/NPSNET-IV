#ifndef __NPS_PO_BUILD_H_
#define __NPS_PO_BUILD_H_

#include "c2_const.h"
#include "po_funcs.h"

void passMenuPosition(pfVec3);
void addLine(float, float);
void dragPoint(float, float, pfGroup*);
void dropPoint(float, float, pfGroup*);
void dragLine(float, float);
void dropLine(float, float);
//******************************************************************************
//
//  Callbacks.

void buildItCB(MENU_LEVEL*);

void blackCB(MENU_LEVEL*);
void yellowCB(MENU_LEVEL*);
void redCB(MENU_LEVEL*);
void greenCB(MENU_LEVEL*);
void blueCB(MENU_LEVEL*);

void thick1CB(MENU_LEVEL*);
void thick2CB(MENU_LEVEL*);
void thick3CB(MENU_LEVEL*);
void thick4CB(MENU_LEVEL*);
void thick5CB(MENU_LEVEL*);
void thick6CB(MENU_LEVEL*);

void closed_lineCB(MENU_LEVEL* );
void genPointCB(MENU_LEVEL *);
void coordPointCB(MENU_LEVEL *);
void contactPointCB(MENU_LEVEL *);

void pointCB(MENU_LEVEL *);
void lineCB(MENU_LEVEL *);

void standardCB(MENU_LEVEL *);
void rootCB(MENU_LEVEL *);





#endif
