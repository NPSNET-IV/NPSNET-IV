#ifndef __AHA_H__
#define __AHA_H__

#include <pf.h>
#include <pfutil.h>

//#define __AHA_DEBUG__

/* public functions */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { AHA_OFF, AHA_TWO_D, AHA_THREE_D, AHA_PICK, AHA_MIXED} AhaMode;

void	ahaConfig();
void	ahaInit( pfChannel * );
void	ahaExit();
void	ahaUpdate( long, long );
void	ahaAddSelectableObject( pfNode *, int, long );
void	ahaRemoveSelectableObject( pfNode * );
void	ahaSelectObject();
void	ahaDeselectObject();
void	ahaSetMode( AhaMode );
void	ahaSetClass( long );
pfNode*	ahaGetSelectedObject();
int	ahaGetSelectedObjIndex();
int     ahaGetPreSelectedObjIndex();

#ifdef __cplusplus
}
#endif

#endif
