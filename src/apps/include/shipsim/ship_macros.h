// File: <ship_macros.h>
// Ref: npsnetIV macros.h



#ifndef _SHIP_MACROS_H
#define _SHIP_MACROS_H

#include "ship_defs.h"


#define isstealth(a)        (a == 0)
#define isalive(a)          (a< 100)
#define isgroundveh(a)      ((a >  0) && (a <  34))
#define isairveh(a)         ((a > 33) && (a <  67))
#define isdeadairveh(a)     ((a > 133) && (a <  167))
#define isseaveh(a)        ((a > 66) && (a < 88))
#define isguess(a)        ((a > 87) && (a < 100))

//-1 if negative, 1 if positive
#define POSNEG(a)    ((a<0.0f)?-1:1)

#define IS_ZERO(f)   ((f > -REAL_SMALL) && (f < REAL_SMALL))
#define IS_SMALL(f)  ((f > -SMALL) && (f < SMALL))

#define sqrd(num) ((num)*(num))

/*the difference functions between the DR and Real positions*/
#define MY_ABS(a,b)   ((((a)<(b))? ((b)-(a)):((a)-(b))))
#define ABS(a)   (((a < 0) ? (-a):(a)))

#endif
