
#ifndef __NPS_BBOX__
#define __NPS_BBOX__

#include <pf.h>

pfSwitch *
make_bounding_box ( const pfBox *box, 
                    int wireframe = TRUE,
                    pfVec4 *color = NULL );

#endif
