
// matrix.cc

// Defines matrix operations used by objects in object.cc

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bstring.h>
#include <math.h>

#include <Performer/pf.h>
#include "prmath.h"
#include "pfutil.h"
#include "pfdb/pfflt.h"
#include "matrix.h"

//	Stack of matrices for manipulating DCS nodes.  
pfMatStack	*walkMStack = NULL;

/************************************************************************
 *									*		
 * 			rotateObject					*
 *									*
 ************************************************************************/

void 
rotateObject( pfMatrix rotationMatrix, float delta, DOFcb  *pDof, int reset )
{
    float	temp = 0.0f;

    pfResetMStack( walkMStack );

    if (pDof->minazim != pDof->maxazim)
    {
	temp = pDof->curazim + delta;

	if ((temp >= pDof->minazim) && (temp <= pDof->maxazim))
    
	{
	   pDof->curazim = temp;
	}
	else 
	{
	   if(reset) 
	   {
	       if (temp > pDof->maxazim) pDof->curazim = pDof->minazim;
	       else pDof->curazim = pDof->maxazim;
	   }
	}
   }

    if (pDof->minincl != pDof->maxincl)
    {
	temp = pDof->curincl + delta;

	if ((temp >= pDof->minincl) && (temp <= pDof->maxincl))
    
	{
	   pDof->curincl = temp;
	}
	else 
	{
	   if(reset) 
	   {
	       if (temp > pDof->maxincl) pDof->curincl = pDof->minincl;
	       else pDof->curincl = pDof->maxincl;
	   }
	}
   }

    if (pDof->mintwist != pDof->maxtwist)
    {
	temp = pDof->curtwist + delta;

	if ((temp >= pDof->mintwist) && (temp <= pDof->maxtwist))
    
	{
	   pDof->curtwist += delta;
	}
	else 
	{
	   if(reset) 
	   {
	       if (temp > pDof->maxtwist) pDof->curtwist = pDof->mintwist;
	       else pDof->curtwist = pDof->maxtwist;
	   }
	}
   }

    // ------------------------------------------------------------------------  
    // DOF rotation order: (1) twist, (2) inclination, (3) azimuth.		 
    // ------------------------------------------------------------------------  

    pfPreRotMStack( walkMStack, pDof->curazim, 1.0, 0.0, 0.0 ); 
    pfPreRotMStack( walkMStack, pDof->curincl, 0.0, 1.0, 0.0 );
    pfPreRotMStack( walkMStack, pDof->curtwist, 0.0, 0.0, 1.0 ); 
    
    pfGetMStack( walkMStack, rotationMatrix );
}


/************************************************************************
 *									*		
 * 			translateObject					*
 *									*
 ************************************************************************/

void translateObject( pfMatrix translationMatrix, float perCent, DOFcb *pDof )
{
    float	temp = 0.0f;
    float	range = 0.0f;


    if ( pDof->minx != pDof->maxx )
    {
	range = (pDof->maxx - pDof->minx);
	temp = pDof->minx + (range * perCent);
	if ((temp >= pDof->minx) && (temp <= pDof->maxx))
	   pDof->curx = temp;
    }

    if ( pDof->miny != pDof->maxy )
    {
	range = (pDof->maxy - pDof->miny);			
	temp = pDof->miny + (range * perCent);
	if ((temp >= pDof->miny) && (temp <= pDof->maxy))
	   pDof->cury = temp;
    }

    if ( pDof->minz != pDof->maxz )
    {
	range = (pDof->maxz - pDof->minz);			
	temp = pDof->minz + (range * perCent);
	if ((temp >= pDof->minz) && (temp <= pDof->maxz))
	   pDof->curz = temp;
    }

    pfMakeTransMat( translationMatrix, pDof->curx, pDof->cury, pDof->curz );
}


/************************************************************************
 *									*		
 * 			updateObject					*
 *									*
 ************************************************************************/

void updateObject(pfNode *node, DOFcb *dof, 
                  float transDist, float rotDeg, int reset)
{
    pfMatrix	finalMatrix;
    pfMatrix	translationMatrix;
    pfMatrix	rotationMatrix;

    if (walkMStack == NULL)
       walkMStack = pfNewMStack ( 16, pfGetSharedArena() );

    translateObject(translationMatrix, transDist, dof);
    rotateObject(rotationMatrix, rotDeg, dof, reset);

    // recompose the DCS matrix from the DOF information			 
    pfLoadMStack( walkMStack, dof->putinvmat );
    pfPreMultMStack( walkMStack, translationMatrix );
    pfPreMultMStack( walkMStack, rotationMatrix );
    pfPreMultMStack( walkMStack, dof->putmat );
    pfGetMStack( walkMStack, finalMatrix );
    pfDCSMat( (pfDCS *)node, finalMatrix );

}

