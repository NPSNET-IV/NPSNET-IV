#ifndef NPS_IO_STREAM_DOT_H
#define NPS_IO_STREAM_DOT_H

#include <iostream.h>
#include <pf.h>

ostream& operator<< (ostream&, pfCoord &);
ostream& operator<< (ostream&, const pfCoord &);
ostream& operator<< (ostream&, pfVec3);
ostream& operator<< (ostream&, const pfVec3);

#endif NPS_IO_STREAM_DOT_H
