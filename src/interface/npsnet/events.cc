// File: <events.cc>


/**********************************************************************
 *                                                                    *
 *      File:         events.c                                        *
 *      Date:         3/14/94                                         *
 *      Developed by: Jose Vazquez                                    *
 *                    NCCOSC RDTE DIV                                 *
 *                    Code 842                                        *
 *                    San Diego, CA  92152-5000                       *
 *                                                                    *
 **********************************************************************/

#include "vabs.h"
#include <stdlib.h>
#include <unistd.h>

#include <Xm/TextF.h>

/***************************************************************
 * Event handler called when the Ok button on the OSV window   *
 * is pressed with the keyboard and another window has input   *
 * focus.                               		       *
 ***************************************************************/
void ok_handler( Widget w, XtPointer client_data, XEvent *event)
{
    KeySym	keysym;
   printf("I am in the ok_handler \n"); 
    /*
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /*
     * Check which key was pressed.
     */
    keysym = XKeycodeToKeysym(display, event->xkey.keycode, 0);
    /*
     * If the Enter key on the numerical keypad was pressed,
     * makes the button that is currently highlighted looks
     * like it was selected with the mouse.  The "Return"
     * action only works when a "complex" window is created
     * with a Motif call(like XmCreateFileSelectionDialog).
     */
    if (keysym == XK_KP_Enter)
    	XtCallActionProc(w, "Return", event, NULL, 0);
    else
    /*
     * Otherwise, let the window manager handle the event.
     */
	XAllowEvents(display, ReplayKeyboard, CurrentTime);
}

/***************************************************************
 * Event handler called when the Ok button on the OSV window   *
 * is pressed with the keyboard and another window has input   *
 * focus.                               		       *
 ***************************************************************/
void ok_handler2(Widget w, XtPointer client_data, XEvent *event)
{
    KeySym	keysym;
    
   printf("I am in the ok_handler2 \n"); 
    /*
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /*
     * Check which key was pressed.
     */
    keysym = XKeycodeToKeysym(display, event->xkey.keycode, 0);
    /*
     * If the Enter key on the numerical keypad was pressed,
     * makes the button that is currently highlighted looks
     * like it was selected with the mouse.  The 
     * "ArmAndActivate" action only works for the cases where
     * the programmer creates the buttons himself.  In these
     * cases, the buttons have an XmNactivate callback.
     */
    if (keysym == XK_KP_Enter)
    	XtCallActionProc(w, "ArmAndActivate", event, NULL, 0);
    else
    /*
     * Otherwise, let the window manager handle the event.
     */
	XAllowEvents(display, ReplayKeyboard, CurrentTime);
}

/***************************************************************
 * Event handler called when the #1 key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void quit(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[3], "ArmAndActivate", event, NULL, 0);
}

/***************************************************************
 * Event handler called when the #8 key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_up(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[6], "ArmAndActivate", event, NULL, 0);
}

/***************************************************************
 * Event handler called when the #4 key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_left(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[2], "ArmAndActivate", event, NULL, 0);
}

/***************************************************************
 * Event handler called when the #5 key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_middle(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[7], "ArmAndActivate", event, NULL, 0);
}

/***************************************************************
 * Event handler called when the #6 key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_right(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[11], "ArmAndActivate", event, NULL, 0);
}

/***************************************************************
 * Event handler called when the #2 key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_down(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[8], "ArmAndActivate", event, NULL, 0);
}

/***************************************************************
 * Event handler called when the = key on the OSV window is    *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_file(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[0], "ArmAndActivate", event, NULL, 0);
}

/***************************************************************
 * Event handler called when the #7 key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_info(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[1], "ArmAndActivate", event, NULL, 0);
}

/***************************************************************
 * Event handler called when the /  key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_divide(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[5], "ArmAndActivate", event, NULL, 0);
}
/***************************************************************
 * Event handler called when the *  key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_prev(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[9], "ArmAndActivate", event, NULL, 0);
}
/***************************************************************
 * Event handler called when the -  key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_next(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[14], "ArmAndActivate", event, NULL, 0);
}
/***************************************************************
 * Event handler called when the #9 key on the OSV window is   *
 * is pressed with the keyboard or selected with the mouse.    *
 ***************************************************************/
void do_message(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[10], "ArmAndActivate", event, NULL, 0);
}

/****************************************************************
 * Event handler called when either 1, 3, 7, or 9 is pressed    *
 * in the numeric pad after the NumLock key has been activated. *
 ****************************************************************/
void do_number(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    Widget	 action_widget;
    XEvent	 key_event;
    int		 result;

    switch(atoi(*params))
    {
	case 0: action_widget = widget[4];
		break;
	case 1:	action_widget = widget[3];
		break;
	case 2: action_widget = widget[8];
		break;
	case 3: action_widget = widget[12];
		break;
	case 4: action_widget = widget[2];
		break;
	case 5: action_widget = widget[7];
		break;
	case 6: action_widget = widget[11];
		break;
	case 7: action_widget = widget[1];
		break;
	case 8: action_widget = widget[6];
		break;
	case 9: action_widget = widget[10];
		break;
	case 20:action_widget = widget[5];
		break;
	case 21:action_widget = widget[15];
		break;
	case 22:action_widget = widget[14];
		break;
 	default:printf("Unknown widget = [%s]\n", params);
		return;
    }
     
    XtCallActionProc(action_widget, "ArmAndActivate", event, NULL, 0);
}

/***************************************************************
 * Event handler for the #3 key on the OSV window              *
 ***************************************************************/
void do_npshdds(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    XtCallActionProc(widget[12], "ArmAndActivate", event, NULL, 0);
}

/***************************************************************
 * Default event handler for the buttons on the OSV window.    *
 ***************************************************************/
void do_default(CompositeWidget w, XEvent *event, String *params, int *num_params)
{
    Widget	 action_widget;
    XEvent	 key_event;
    int		 result;

    switch(atoi(*params))
    {
	case 0:	action_widget = widget[4];
		sleep(10);
		break;
	case 1:	action_widget = widget[5];
		break;
	case 2: action_widget = widget[9];
		break;
	case 3: action_widget = widget[12];
		break;
	case 4: action_widget = widget[13];
		break;
	case 5: action_widget = widget[14];
		break;
	case 6: action_widget = widget[15];
		break;
	case 7: action_widget = widget[16];
		/*
		 * The Enter key was pressed.  Inform the window
		 * that have input focus.
		 */
		key_event.type = KeyPress;
		key_event.xkey.type = KeyPress;
		key_event.xkey.send_event = True;
		key_event.xkey.display = display;
		key_event.xkey.window = XtWindow((Widget) display);
		key_event.xkey.state = 0;
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_KP_Enter);
		/* 
		 * Send the event and flush the output buffer.
		 */
		XSendEvent(display, InputFocus, False, 
		    KeyPressMask, &key_event);
		XFlush(display);
		break;
        case 21: action_widget = widget[15];
		 break;
        case 22: action_widget = widget[14];
		 break;
 	default:printf("Unknown widget = [%s]\n", params);
		return;
    }
    XtCallActionProc(action_widget, "ArmAndActivate", event, NULL, 0);
}

