#ifndef __HIGHLIGHT_H__
#define __HIGHLIGHT_H__

#include "c2_const.h"

typedef struct hlight
{
    pfHighlight	*hl;
    pfNode	*node;
    pfVec3	color;
    long	mode;
    long	dirty;
    long 	nChans;
} Highlight;

Highlight	*NewHighlight(void);
void		HighlightNode(Highlight *HL, pfNode *N);
pfNode		*GetHighlightNode(Highlight *HL);
void		HighlightMode(Highlight *HL, long mode);
long		GetHighlightMode(Highlight *HL);
void		HlightColor(Highlight *HL, pfVec3 c);
void		GetHlightColor(Highlight *HL, pfVec3 dest);
void 		HighlightChannels(Highlight *HL, long n);
long 		GetHighlightChannels(Highlight *HL);

/* Update Must be called from the draw process (pre-draw callback)*/
void		UpdateHighlight(Highlight *HL);
void            poHighLight(pfGeoSet *gset);

#endif
