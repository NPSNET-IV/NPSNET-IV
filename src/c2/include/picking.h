#ifndef __PICKING_H__
#define __PICKING_H__

#include "c2_const.h"

typedef struct pickstruct
{
     pfScene	*scene;
     pfChannel	**chan;
     pfNode	*picked;
     pfGroup    *group;
     pfVec3     point;                        //SAK.
     pfPath	*path;                         //SAK.
     char	*pathname;
     //char pathname[300];
     float    	pickX;
     float    	pickY;
     long	traverse;
} Pick;

Pick	*NewPick(pfScene *S, pfChannel **C);
float	GetPickedX(Pick *P);
float	GetPickedY(Pick *P);
pfNode	*GetPickNode(Pick *P);
pfNode	*DoPick(Pick *P, long mousex, long mousey);
pfNode	*doMenuPick(Pick *P, long mousex, long mousey);

char	*GetPickedPathname(Pick *P);
//void HLPick(long mousex, long mousey);  //SAK.
void HLPick();
void menuPick();
void ballPick();
void mouseTest(Pick *);
void drag(pfGroup*, Pick*);
void processDrag(pfGroup*, Pick*);
#endif
