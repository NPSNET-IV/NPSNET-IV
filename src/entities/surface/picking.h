


#ifndef __PICKING_H__
#define __PICKING_H__



typedef struct pickstruct
{
     pfNode	*root;
     pfChannel	**chan;
     pfNode	*picked;
     long	traverse;     
} Pick;


Pick	*NewPick(pfNode *N, pfChannel **C);
pfNode	*DoPick(Pick *P);
int get_pick_button(int, int&);

#endif
