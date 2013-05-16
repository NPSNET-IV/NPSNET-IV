//
//  This is file draw_funcs.c
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <getopt.h>           // for cmdline handler 

#include <X11/keysym.h>       // defines for Key strokes

#include "globals.h"
#include "draw_funcs.h"


// UpdateView() updates the eyepoint based on the information
// placed in shared memory by GetXInput().
 
void UpdateView(void)
{
    // Contents of this whole function has been replaced by Shirley 
    // for better viewing control
    static int firsttime = TRUE;
    static float mx, my; 
    static long mouse_posx, mouse_posy;

    /* big change to keep movement relative to window */
    // note window origin values are wrt lower left corner of window
    // mouse values wrt upper left corner of window
    mouse_posx = Shared->mouseX             // - Shared->winOriginX
        - (Shared->winSizeX/2);
    mouse_posy =  (Shared->winSizeY - Shared->mouseY)  // - Shared->winOriginY
        - (Shared->winSizeY/2);

    if ((labs(mouse_posx) > (Shared->winSizeX/2)) ||
         (labs(mouse_posy) > (Shared->winSizeY/2))) {
         mx = my = 0.0;
    }
    else {
         mx = mouse_posx/((float) Shared->winSizeX);
         my = mouse_posy/((float) Shared->winSizeY);
    }


    if(firsttime) {
       pfSinCos(Shared->view.hpr[PF_H], &Shared->eye.sinH, 
          &Shared->eye.cosH);
       pfSinCos(Shared->view.hpr[PF_P], &Shared->eye.sinP, 
          &Shared->eye.cosP);
       firsttime = FALSE;
    }

    if (!Shared->inWindow) {
        return;
    }

    switch (Shared->mouseButtons) {

    case Button1Mask: /* LEFTMOUSE: rotate about view center */
       // Rotate CCW/CW  
        Shared->view.hpr[PF_H] -= 2.0f*mx;
        if (Shared->view.hpr[PF_H] > 180.0f)
           Shared->view.hpr[PF_H] -= 360.0f;
        else if (Shared->view.hpr[PF_H] < -180.0f)
           Shared->view.hpr[PF_H] += 360.0f;

        Shared->view.hpr[PF_P] += 2.0f*my;
        if (Shared->view.hpr[PF_P] > 270.0f)
           Shared->view.hpr[PF_P] = 270.0f;
        else if (Shared->view.hpr[PF_P] > 90.0f)
           Shared->view.hpr[PF_P] = 90.0f;

       pfSinCos(Shared->view.hpr[PF_H], &Shared->eye.sinH,
          &Shared->eye.cosH);
       pfSinCos(Shared->view.hpr[PF_P], &Shared->eye.sinP,
          &Shared->eye.cosP);

        break;

    case Button2Mask:   /* MIDDLEMOUSE: zoom in */
        if(Shared->eye.radius > 1.0f) {
           Shared->eye.radius -= 1.0f;
        }
        break;

    case Button3Mask: /* RIGHTMOUSE: zoom out */
        Shared->eye.radius += 1.0f;
        break;
    }

    // update view position 
    Shared->view.xyz[PF_X] =   Shared->eye.radius * Shared->eye.sinH 
       * Shared->eye.cosP;
    Shared->view.xyz[PF_Y] = - Shared->eye.radius * Shared->eye.cosH
       * Shared->eye.cosP;
    // offset view to alter vertical view center 
    Shared->view.xyz[PF_Z] = - Shared->eye.radius * Shared->eye.sinP 
       + Shared->eye.offsetZ; 
}

/*
 *	OpenPipeline() -- create a pipeline: setup the window system,
 *	the IRIS GL, and IRIS Performer. this procedure is executed in
 *	the draw process (when there is a separate draw process).
 */

void OpenPipeline(pfPipe *p)
{
    static const pfVec3  clrt = { 0.7f, 0.7f, 0.8f};
    float xSize = 800;
    float ySize = 500;
    long mapped = 0;

    XSetWindowAttributes swa;
    XColor gray;
    GLXconfig *config;
    
    if (XDpy == NULL)
    {
	XDpy = XOpenDisplay(":0.0");
	if (!XDpy)
	    pfNotify(PFNFY_FATAL, PFNFY_RESOURCE, 
		     "OpenPipeline: couldn't open display\n");
    }
    
    /*
     * First create the top level X window.  The background will be
     * grey, and the window is only interested in key press events.
     */
    gray.red = 0x7fff; gray.green = 0x7fff; gray.blue = 0x7fff;
    XAllocColor(XDpy, DefaultColormap(XDpy, DefaultScreen(XDpy)), &gray);
    swa.background_pixel = gray.pixel;
    Shared->xWin = XCreateWindow(XDpy, RootWindow(XDpy, DefaultScreen(XDpy)),
				 100, 100, xSize, ySize,
				 0, CopyFromParent, 
				 InputOutput, CopyFromParent,
				 CWBackPixel, &swa);
    
    XStoreName(XDpy, Shared->xWin, "Dude models in Performer");
    
    /* create GLX window for GL rendering 
     * note: this can be done automatically by using the
     * libpfutil routine pfuGLXWinopen() - see utilui.c for example.
     */
    if (getgdesc(GD_MULTISAMPLE) > 0)
	config = GLXCreateWindow(XDpy, Shared->xWin, 0, 0, xSize, ySize, 0,
				 GLX_NORMAL, GLX_DOUBLE, TRUE,
				 GLX_NORMAL, GLX_RGB, TRUE,
				 GLX_NORMAL, GLX_ZSIZE, 0,
				 GLX_NORMAL, GLX_STENSIZE, 0,
				 GLX_NORMAL, GLX_MSSAMPLE, 8,
				 GLX_NORMAL, GLX_MSZSIZE, 24,
				 GLX_NORMAL, GLX_MSSSIZE, 1,
				 GLX_OVERLAY, GLX_BUFSIZE, 2,
				 0, 0, 0);
    else
	config = GLXCreateWindow(XDpy, Shared->xWin, 0, 0, xSize, ySize, 0,
				 GLX_NORMAL, GLX_DOUBLE, TRUE,
				 GLX_NORMAL, GLX_RGB, TRUE,
				 GLX_NORMAL, GLX_ZSIZE, GLX_NOCONFIG,
				 GLX_NORMAL, GLX_STENSIZE, 1,
				 GLX_OVERLAY, GLX_BUFSIZE, 2,
				 0, 0, 0);
    
    Shared->glWin = GLXgetvalue(config, GLX_NORMAL, GLX_WINDOW);
    Shared->overWin = GLXgetvalue(config, GLX_OVERLAY, GLX_WINDOW);
    XSelectInput(XDpy, Shared->glWin, ExposureMask);
    XMapWindow(XDpy, Shared->glWin);
    XMapWindow(XDpy, Shared->xWin);
    XFlush(XDpy);
    
    /* spin until get MapNotify Event on GL window */
    while (!mapped)
    {
	XEvent	 event;
	XNextEvent(XDpy, &event);
	switch (event.type) 
	{
	    case Expose:
		XSelectInput(XDpy, Shared->glWin, 0x0);
		mapped = 1;
		break;
	}
    }
    /* initialize Performer for GLX rendering */
    pfInitGLXGfx(p, (void*)XDpy, Shared->xWin, Shared->glWin, Shared->overWin, 
		    WinConfig);
    c3f(clrt);
    clear();
    swapbuffers();
    
    /* create a light source in the "south-west" (QIII) */
    Sun = pfNewLight(NULL);
    pfLightPos(Sun, -0.3f, -0.3f, 1.0f, 0.0f);
    
    /* create a default texture environment */
    pfApplyTEnv(pfNewTEnv(NULL));
    
    /* create a default lighting model */
    pfApplyLModel(pfNewLModel(NULL));
    
    pfApplyMtl(pfNewMtl(NULL));
    
    /* enable culling of back-facing polygons */
    //pfCullFace(PFCF_BACK); // disable this -- need back-facing polygons to
    // see back of dude guys
    pfCullFace(PFCF_OFF);
    
    /*
     * These enables should be set to reflect the majority of the
     * database. If most geometry is not textured, then texture
     * should be disabled. However, you then need to change the
     * FLIGHT-format file reader. (pfflt.c)
     */
    pfEnable(PFEN_TEXTURE);
    pfEnable(PFEN_LIGHTING);
    pfEnable(PFEN_FOG);
}

/*
 *	CullChannel() -- traverse the scene graph and generate a
 * 	display list for the draw process.  This procedure is 
 *	executed in the cull process.
 */

void CullChannel(pfChannel *channel, void *data)
{
    (channel, data); /* remove compiler warn about unused var */
    
    /* 
     * pfDrawGeoSet or other display listable Performer routines
     * could be invoked before or after pfCull()
     */
    pfCull();
    
    
}

/*
 *	DrawChannel() -- draw a channel and read input queue. this
 *	procedure is executed in the draw process (when there is a
 *	separate draw process).
 */
void DrawChannel (pfChannel *channel, void *data)
{
    (data); /* remove compiler warn about unused var */
    
    /* rebind light so it stays fixed in position */
    pfLightOn(Sun);
    
    /* erase framebuffer and draw Earth-Sky model */
    pfClearChan(channel);
    
    /* invoke Performer draw-processing for this frame */
    pfDraw();
    
    /* draw Performer throughput statistics */
    if (Shared->drawStats)
	pfDrawChanStats(channel);
    
    /* read window origin and size (it may have changed) */
    pfGetPipeSize(pfGetChanPipe(channel),
		    &Shared->winSizeX, &Shared->winSizeY);
    pfGetPipeOrigin(pfGetChanPipe(channel), 
		      &Shared->winOriginX, &Shared->winOriginY);

    if (Shared->redrawOverlay)
    {
	Shared->redrawOverlay = 0;
	DrawOverlay();
    }
}

/* leave FocusChange in XINMASK, since doesn't work in GLINMASK */
#define XINMASK (FocusChangeMask)

/*
 * want motion and button presses only within GL window area, 
 * parent X window focus includes window border
 */
#define GLINMASK (KeyPressMask	    | \
		  ButtonPressMask   | \
		  ButtonReleaseMask | \
		  ExposureMask      | \
		  PointerMotionMask)
void GetXInput(void)
{
    static long first = 1;
    XEvent ev;
    long newMouse = 0;
    long x,y;

    /* Display already open in single process mode */
    if (XDpy == NULL)
    {
	XDpy = XOpenDisplay(":0.0");
	if (!XDpy)
	    pfNotify(PFNFY_FATAL, PFNFY_RESOURCE, 
		     "GetXInput: couldn't open display\n");
    }
    if (first)
    {
	XSelectInput(XDpy, Shared->glWin, GLINMASK);
	XSelectInput(XDpy, Shared->xWin, XINMASK);
	first = 0;
    }
    while (XCheckWindowEvent(XDpy, Shared->glWin, GLINMASK, &ev))
    {
	switch (ev.type)
	{
	case Expose:
	    Shared->redrawOverlay = 1;
	    break;
	case ButtonPress:
	    switch (ev.xbutton.button)
	    {
	    case (1):
                newMouse = 1;
                x = ev.xmotion.x;
                y = ev.xmotion.y;

                Shared->mouseButtons = ev.xbutton.state | Button1Mask;
                break;
            case (2):
                newMouse = 1;
                x = ev.xmotion.x;
                y = ev.xmotion.y;
                Shared->distance = Shared->distance -10.0;
                Shared->mouseButtons = ev.xbutton.state | Button2Mask;
                break;
            case (3):
                newMouse = 1;
                x = ev.xmotion.x;
                y = ev.xmotion.y;
                Shared->distance = Shared->distance + 10.0;
                Shared->mouseButtons = ev.xbutton.state | Button3Mask;
               break;
	    }
	    break;
	case ButtonRelease:
	    switch (ev.xbutton.button)
	    {
	    case (1):
		Shared->mouseButtons = ev.xbutton.state & ~Button1Mask;
		break;
	    case (2):
		Shared->mouseButtons = ev.xbutton.state & ~Button2Mask;
		break;
	    case (3):
		Shared->mouseButtons = ev.xbutton.state & ~Button3Mask;
		break;
	    }
	    break;
	case MotionNotify:
	    {
		newMouse = 1;
		x = ev.xmotion.x;
		y = ev.xmotion.y;
	    }
	    break;
	case KeyPress: 
	    { // look at /usr/include/X11/keysym.h for other defined 
              // key stroke values
               
		KeySym ks;
		
		/*
		 * On a keypress, either rotate and re-display
		 * or exit.
		 */
		ks = XLookupKeysym(&ev.xkey, 0);
		switch (ks)
		{
		case XK_F1:
		    {
			Shared->drawStats = !Shared->drawStats;
			break;
		    } 
		case XK_Escape:
		    {
			Shared->exitFlag = 1;
			break;
		    }
//   
//   New code keyboard inputs
//
		case XK_s: //Snap to Standing posture
		    {
			Shared->standing = 1;
			Shared->walking = 0;
			Shared->prone = 0;
			Shared->kneeling = 0;
			break;
		    }
  		case XK_k:  //Snap to Kneeling posture
		    {
			Shared->kneeling = 1;
			Shared->walking = 0;
			Shared->standing = 0;
			Shared->prone = 0;
			break;
		    }
  		case XK_p:  //Snap to Prone posture
		    {
			Shared->prone = 1;
			Shared->walking = 0;
			Shared->standing = 0;
			Shared->kneeling = 0;
			break;
		    }
  		case XK_w:  //Start dudes walking
		    {
			Shared->walking = 1;
			Shared->standing = 0;
			Shared->prone = 0;
			Shared->kneeling = 0;
			break;
		    }
                case XK_z:  //Stop dudes walking
		    {
			Shared->walking = 0;
			Shared->standing = 1;
			Shared->prone = 0;
			Shared->kneeling = 0;
                        break;
		    }
		case XK_e: //Snap to Standing posture
		    {
			Shared->standing1 = 1;
			Shared->walking1 = 0;
			Shared->prone1 = 0;
			Shared->kneeling1 = 0;
			break;
		    }
  		case XK_o:  //Snap to Kneeling posture
		    {
			Shared->kneeling1 = 1;
			Shared->walking1 = 0;
			Shared->standing1 = 0;
			Shared->prone1 = 0;
			break;
		    }
  		case XK_l:  //Snap to Prone posture
		    {
			Shared->prone1 = 1;
			Shared->walking1 = 0;
			Shared->standing1 = 0;
			Shared->kneeling1 = 0;
			break;
		    }
  		case XK_a:  //Start dudes walking
		    {
			Shared->walking1 = 1;
			Shared->standing1 = 0;
			Shared->prone1 = 0;
			Shared->kneeling1 = 0;
			break;
		    }
                case XK_x:  //Stop dudes walking
		    {
			Shared->walking1 = 0;
			Shared->standing1 = 1;
			Shared->prone1 = 0;
			Shared->kneeling1 = 0;
                        break;
		    }
                case XK_Left:
                    {
                    Shared->view.hpr[PF_H] -= 1.0f;
                    if (Shared->view.hpr[PF_H] > 180.0f)
                       Shared->view.hpr[PF_H] -= 360.0f;
                    else if (Shared->view.hpr[PF_H] < -180.0f)
                       Shared->view.hpr[PF_H] += 360.0f;
                    pfSinCos(Shared->view.hpr[PF_H], &Shared->eye.sinH,
                       &Shared->eye.cosH);
                    break;
                    }
                case XK_Right:
                    {
                    Shared->view.hpr[PF_H] += 1.0f;
                    if (Shared->view.hpr[PF_H] > 180.0f)
                       Shared->view.hpr[PF_H] -= 360.0f;
                    else if (Shared->view.hpr[PF_H] < -180.0f)
                       Shared->view.hpr[PF_H] += 360.0f;
                    pfSinCos(Shared->view.hpr[PF_H], &Shared->eye.sinH,
                       &Shared->eye.cosH);
                    break;
                    }

                case XK_Up:
                    {
                    Shared->eye.offsetZ += 0.5f;
                    break;
                    }
                case XK_Down:
                    {
                    Shared->eye.offsetZ -= 0.5f;
                    break;
                    }

		}
		break;
	    }
	}
    }
    while (XCheckWindowEvent(XDpy, Shared->xWin, XINMASK, &ev))
    {
	switch (ev.type)
	{
	case FocusIn:
	    break;
	case FocusOut:
	    Shared->inWindow = 0;
	    break;
	}
    }
    if (newMouse)
    {
// Changed by Shirley for easier viewing interface
//	Shared->mouseX =  2.0f*x/(float)Shared->winSizeX - 1.0f;
//	Shared->mouseY = -2.0f*y/(float)Shared->winSizeY + 1.0f;
        Shared->mouseX = x;
        Shared->mouseY = y;
	Shared->inWindow = 1;
    }
}

void DrawOverlay(void)
{
    if (GLXwinset(XDpy, Shared->overWin) < 0)
    {
	pfNotify(PFNFY_WARN, PFNFY_SYSERR, 
		 "GLXwinset for overlay failed\n");
	return;
    }

    pfPushState();
    pfBasicState();
    zbuffer(FALSE);
    pfPushIdentMatrix();
    
    mapcolor(0, 0, 0, 0);
    mapcolor(1, 71, 9, 82);
    mapcolor(2, 0, 0, 0);
    gflush();
    
    color(0);
    clear();
    ortho2(-0.5, Shared->winSizeX - 0.5, -0.5, Shared->winSizeY - 0.5);
    cmov2(2, 2);
    color(1);
    charstr("Left mouse - tilt/rotate view, Middle mouse - zoom in, Right mouse - zoom out");
/*
    cmov2(5, 15);
    color(1);
    charstr("k or o (kneel) p or l (prone) s or e(stand) w or a (walk) z  or x (stop)");
*/
    pfPopMatrix();
    zbuffer(TRUE);
    pfPopState();

    if (GLXwinset(XDpy, Shared->glWin) < 0)
    {
	pfNotify(PFNFY_WARN, PFNFY_SYSERR, 
		 "GLXwinset for NORMAL window failed\n");
    }
}

