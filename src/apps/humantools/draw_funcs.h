//
// This is file draw_funcs.h
//

#ifndef __DRAW_FUNCS__
#define __DRAW_FUNCS__

void CullChannel(pfChannel *chan, void *data);

void DrawChannel(pfChannel *chan, void *data);

void OpenPipeline(pfPipe *p);

void DrawOverlay(void);

void UpdateView(void);

void GetXInput(void);

/* 
 * Prototypes for GLX utility code from 
 * /usr/people/4DGifts/examples/GLX/gl-Xlib/glxh2.c
 * which appears at the end of this file
 */

ulong	   GLXgetvalue(GLXconfig* conf, int buffer, int mode);
GLXconfig* GLXCreateWindow(Display* dpy,Window parent,
			   int x,int y,
			   int w,int h,
			   int borderWidth,
			   ...);

#endif


