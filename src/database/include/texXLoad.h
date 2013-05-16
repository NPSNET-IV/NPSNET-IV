#ifndef TEX_X_LOAD_DOT_H
#define TEX_X_LOAD_DOT_H

#include <pf.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PFXTEX_DEFINE 0
#define PFXTEX_SHOW 1

#ifndef PFTEX_RGB5_A1
#define PFTEX_RGB5_A1 GL_RGB5_A1_EXT
#endif

void pfxDownloadTexList (pfList *, int);

#ifdef __cplusplus
}
#endif


#endif /* TEX_X_LOAD_DOT_H */
