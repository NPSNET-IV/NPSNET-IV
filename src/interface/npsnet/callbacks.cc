// File: <callbacks.cc>

/**********************************************************************
 *                                                                    *
 *	File:         callbacks.c                                     *
 *	Date:         3/14/94                                         *
 *	Developed by: Jose Vazquez                                    *
 *	              NCCOSC RDTE DIV                                 *
 *	              Code 842                                        *
 *                    San Diego, CA  92152-5000                       *
 *                                                                    *

 *     Modification:  by Bryan Croft 8/26/94
 **********************************************************************/

#include "vabs.h"
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrolledW.h>
#include <Xm/MessageB.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <device.h>
#include "input.h"
extern char *labels[];

Widget popup5;
int npsmenu = 0;
int active = 0;
int npstrue = 0;
int counter = 0;
// Labels for System window

/*
 * Labels for the FileSelectionDialog.
 */
char    *file_labels[] = {
	"File\nMenu", "", "", "", "Redisplay\nMain\nMenu", "Toggle\nNumber\nPad On",
	"", "", "", "Filter", "", "", "", "Return", 
	"Help", "Cancel", "Ok",
};

/*
 * Labels for the InformationDialog.
 */
char    *info_labels[] = {
        "Info\nMenu", "", "", "", "Redisplay\nMain\nMenu", "Toggle\nNumber\nPad On", "",
	"", "", "", "", "", 
	"", "Return", "Help", "Cancel", "Ok",
};

/*
 * Labels for the Shell with the Text box.
 */
char	*text_labels[] = {
	"Text\nMenu", "File", "Edit", "", "Redisplay\nMain\nMenu", "Toggle\nNumber\nPad On", "", 
	"", "", "", "", "", "", 
	"Return", "Help", "", "Done",
};

/*
 * Labels for Text field that accept numbers as input.
 */
char	*num_labels[] = {
	"", "7", "4", "1", "0", "Toggle\nNumber\nPad Off", "8", "5",
	"2", "", "9", "6", "3", ".", "Help", "Cancel", "Ok",
};

/*
 * Labels for the Menubar.
 */
char  *rbutton_names[] = {
        "System", " Help "
};

/*
 * Labels for the Push Buttons on the Pulldown Menu.
 */
char  *pd_names[] = {
        "Screen Size", "Resolution", "Window Size", "Colors"
};

/*
 * Labels for the Application Window.
 */
char    *npslabels[] = {
        "Num/\nFunc\nPad", " ", " ",
        "Exit", "OK", "Iconify\nWindow", " ", "NPSNET",
        " ", "Prev\nTAB\nGroup", " ", " ", " ",
        "APPLY", "Next\nTAB\nGroup", "RESET", "CANCEL",
};

char    *npskeylabels[] = {
        "Num/\nFunc\nPad", "Birdseye\nView", "Dead\nDIGUY",
        "Exit", "OK", "Iconify\nWindow", "Kneeling\nDIGUY", "Prone\nDIGUY",
        "Upright\nDIGUY", "Prev\nTAB\nGroup", "Take Aim\nLower\nWeapon", "Fire\nMachine\nGuns", "Fire\nMissile",
        "APPLY", "Next\nTAB\nGroup", "RESET", "CANCEL",
};
char    *npskeylabels1[] = {
        "Num/\nFunc\nPad", "Deceler-\nate\nVehicle", "Acceler-\nate\nVehicle",
        "Exit", "OK", "Iconify\nWindow", "Change\nInput\nDevices", "Reset\nVehicle\nDirect-\nion",
        "Tether\nVehicle", "Prev\nTAB\nGroup", "HUD\nX,Y", "Attach\nVehicle", "Stop",
        "APPLY", "Next\nTAB\nGroup", "RESET", "CANCEL",
};
char    *npskeylabels2[] = {
        "Num/\nFunc\nPad", "Reset\nView", "View\nLeft",
        "Exit", "OK", "Iconify\nWindow", "View\nUp", "Toggle\nPerformer\nStats",
        "View\nDown", "Prev\nTAB\nGroup", "Cycle\nMunition", "View\nRight", "Stop",
        "APPLY", "Next\nTAB\nGroup", "RESET", "CANCEL",
};
/*
 * Widgets for the OSV button for the Application Window.
 */
char    *npsnames[] = {
        "msg", "msg2", "msg3", "msg4", "msg5", "msg6",
        "msg7", "msg8", "msg9", "msg10", "msg11", "msg12",
        "msg13", "msg14", "msg15", "msg16", "msg17",
};


Widget	menu, pull_down[4];

XmStringCharSet char_set = XmSTRING_DEFAULT_CHARSET;


void default_nps(Widget w, int data, XmAnyCallbackStruct *call_data)
{
    XEvent       key_event;
    Pixel        fg_color;

printf("I am in default_nps\n ");

     if (counter <= 0)
        counter = 0;

    /*
     * If the Numeric mode(1) is enable, send an event to the
     * last selected numeric entry field.
     */
    if (receive && (call_data->reason == XmCR_ACTIVATE))
    {
        /*
         * Inform the window that have input focus.
         */
        key_event.type = KeyPress;
        key_event.xkey.type = KeyPress;
        key_event.xkey.send_event = True;
        key_event.xkey.display = display;
        key_event.xkey.window = XtWindow(receive);
        key_event.xkey.state = 0;
        switch(data)
        {
            case 0:
                break;
            case 2:
                key_event.xkey.keycode = XKeysymToKeycode(display, XK_4);
                break;
            case 3:
                key_event.xkey.keycode = XKeysymToKeycode(display, XK_1);
                break;
            case 4:
                key_event.xkey.keycode = XKeysymToKeycode(display, XK_0);
                break;
            case 5:
                printf("I'm in case 5, default_nps \n");
                key_event.xkey.keycode = XKeysymToKeycode(display,
                    XK_Clear);
                key_event.xkey.state = 5;
                mode ^= 1;
                if (mode)
                    number_focus(w, receive, NULL);
                else
                    default_focus(w, NULL, NULL);
                return;
            case 6:
                key_event.xkey.keycode = XKeysymToKeycode(display, XK_8);
                break;
            case 7:
                key_event.xkey.keycode = XKeysymToKeycode(display, XK_5);
                break;
            case 8:
                key_event.xkey.keycode = XKeysymToKeycode(display, XK_2);
                break;
            case 10:
                key_event.xkey.keycode = XKeysymToKeycode(display, XK_9);
                break;
            case 11:
                key_event.xkey.keycode = XKeysymToKeycode(display, XK_6);
                break;
            case 12:
                key_event.xkey.keycode = XKeysymToKeycode(display, XK_3);
                break;
	    case 9:
                printf("I am in case state 15 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_KP_Multiply);
		break;
	    case 14:
                printf("I am in case state 14 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_KP_Subtract);
		break;
            default:
                return;
        }
        /*
         * Send the event and flush the output buffer.
         */
        if (mode != 0)
        {
            XSendEvent(display, InputFocus, False, KeyPressMask, &key_event);
            XFlush(display);
            XSetInputFocus(display, key_event.xkey.window,
                RevertToPointerRoot, CurrentTime);
            return;
        }
    }

    if (npsmenu == 1)
    {
    /*
     * Regular mode(0).
     */
    switch(data)
    {

        case 1: active = 7; 
                npsfunction();        /* #7 key */
                break;
        case 5: create_icon(toplevel);         /* / key  */
                 break;
        case 8: active = 2;
                npsfunction();           /* #2 key  */
                break;
        case 10:active = 9;
                npsfunction();         /* #9 key */
                break;
        case 2: active = 4;
                npsfunction();          /* #4 key */
                break;
        case 3: kill_application();     /* #1 key */
                break;
        case 6: active = 8;
                npsfunction();    /* #8 key */
                break;
        case 12:active = 3;
                npsfunction();            /* #3 key */
                break;
        case 7: active = 6;
                npsfunction();            /* #5 key */
                break;
        case 9: printf("I'm in case 12 \n");
                create_prev();            /* * key */
                break;
        case 14: printf("I'm in case 14 \n");
                create_next();            /* - key */
                break;
    }
   }
  else
    /*
     * Regular mode(0).
     */
    switch(data)
    {

        case 1: create_npsmessage();        /* #7 key */
                break;
        case 5: create_icon(toplevel);         /* / key  */
                 break;
        case 8: create_npsmessage();           /* #2 key  */
                break;
        case 10: create_npsmessage();         /* #9 key */
                break;
        case 2: create_npsmessage();          /* #4 key */
                break;
        case 3: kill_application();     /* #1 key */
                break;
        case 6:create_npsmessage();    /* #8 key */
                break;
        case 12:create_npsmessage();            /* #3 key */
                break;
        case 7:create_npsmenu();            /* #5 key */
                break;
        case 9: printf("I'm in case 12 \n");
                create_prev();            /* * key */
                break;
        case 14: printf("I'm in case 14 \n");
                create_next();            /* - key */
                break;
    }
}

/********************************************************************
 *  Callback executed when any of the OSV buttons is pressed with   *
 *  the keyboard or selected with the mouse.  For most of the       *
 *  buttons, the callback do nothing at all.  Some buttons will     *
 *  create popup windows.                                           *
 ********************************************************************/
void default_callback(Widget w, int data, XmAnyCallbackStruct *call_data)
{
    XEvent       key_event;
    Pixel	 fg_color;

printf("I am in default_callback \n");

     if (counter <= 0)
        counter = 0;
    /*
     * If the Numeric mode(1) is enable, send an event to the
     * last selected numeric entry field.
     */
    if (receive && (call_data->reason == XmCR_ACTIVATE))
    {
	/*
         * Inform the window that have input focus.
         */
        key_event.type = KeyPress;
        key_event.xkey.type = KeyPress;
        key_event.xkey.send_event = True;
        key_event.xkey.display = display;
        key_event.xkey.window = XtWindow(receive);
        key_event.xkey.state = 0;
	switch(data)
	{
	    case 0:
                printf("I am in case state 0 \n");
		break;
	    case 1:
                printf("I am in case state 1 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_7);
		break;
	    case 2:
                printf("I am in case state 2 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_4);
		break;
	    case 3:
                printf("I am in case state 3 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_1);
		break;
	    case 4:
                printf("I am in case state 4 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_0);
		break;
	    case 5:
                printf("I'm in case 5, default_callback \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, 
		    XK_Clear);
		key_event.xkey.state = 5;
        	mode ^= 1;
        	if (mode)
        	    number_focus(w, receive, NULL);
        	else
       		    default_focus(w, NULL, NULL);
    		return;
	    case 6:
                printf("I am in case state 6 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_8);
		break;
	    case 7:
                printf("I am in case state 7 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_5);
		break;
	    case 8:
                printf("I am in case state 8 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_2);
		break;
	    case 10:
                printf("I am in case state 10 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_9);
		break;
	    case 11:
                printf("I am in case state 11 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_6);
		break;
	    case 12:
                printf("I am in case state 12 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_3);
		break;
	    case 9:
                printf("I am in case state 15 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_KP_Multiply);
		break;
	    case 14:
                printf("I am in case state 14 \n");
		key_event.xkey.keycode = XKeysymToKeycode(display, XK_KP_Subtract);
		break;
	    default:
                printf("I am in default case state \n");
		return;
	}
        /*
         * Send the event and flush the output buffer.
         */
	if (mode != 0)
	{
            XSendEvent(display, InputFocus, False, KeyPressMask, &key_event);
            XFlush(display);
	    XSetInputFocus(display, key_event.xkey.window, 
		RevertToPointerRoot, CurrentTime);
	    return;
	}
    }

    if (npsmenu == 1)
    {
    /*
     * Regular mode(0).
     */
    switch(data)
    {

        case 1: active = 7;
                npsfunction();        /* #7 key */
                break;
        case 5: create_icon(toplevel);         /* / key  */
                 break;
        case 8: active = 2;
                npsfunction();           /* #2 key  */
                break;
        case 10:active = 9;
                npsfunction();         /* #9 key */
                break;
        case 2: active = 4;
                npsfunction();          /* #4 key */
                break;
        case 3: kill_application();     /* #1 key */
                break;
        case 6: active = 8;
                npsfunction();    /* #8 key */
                break;
        case 12:active = 3;
                npsfunction();            /* #3 key */
                break;
        case 7: active = 5;
                npsfunction();            /* #5 key */
                break;
        case 9: printf("I'm in case 12 \n");
                create_prev();            /* * key */
                break;
        case 14: printf("I'm in case 14 \n");
                create_next();            /* - key */
                break;
    }
   }

    /*
     * Regular mode(0).
     */
    switch(data)
    {

 	case 1: create_system();	/* #7 key */
                break;
        case 5: create_icon(toplevel);         /* / key  */
                 break;
	case 8: create_fsb();		/* #2 key  */
		break;
	case 10: create_info();		/* #9 key */
		break;
	case 2: create_text();		/* #4 key */
		break;
	case 3: kill_application();	/* #1 key */
		break;
	case 6:create_message_box();	/* #8 key */
		break;
	case 12:create_nps();    	/* #3 key */
		break;
	case 7:create_npsmenu();    	/* #5 key */
		break;
        case 9:printf("I'm in case 12 \n");
                create_prev();            /* * key */
                break;
        case 14:printf("I'm in case 14 \n");
                create_next();            /* - key */
                break;
    }
}

void npsfunction()
{

printf("Active = %d\n");
     if (active == 7)
      {
       printf("Bird's eye code goes here \n");
       qdevice(SIXKEY);
       qenter(SIXKEY, 1);
       printf("Look at me man   SIXKEY = %d\n", isqueued(SIXKEY));
       qenter(SIXKEY, 0);
       active = 0;
      }
     if (active == 8)
       {
       printf("Kneeling DIGUY code goes here \n");
       qdevice(EIGHTKEY);
       qenter(EIGHTKEY, 1);
       printf("Kneel man kneel  EIGHTKEY = %d\n", isqueued(EIGHTKEY));
       qenter(EIGHTKEY, 0);
       active - 0;
       }
     if (active == 9)
       {
       printf("Mount/Unmount rifle code goes here \n");
       qdevice(FIVEKEY);
       qenter(FIVEKEY, 1);
       printf("Move that rifle man  FIVEKEY = %d\n", isqueued(FIVEKEY));
       qenter(FIVEKEY, 0);
       active = 0;
       }
     if (active == 4)
       {
       printf("Dead DIGUY code goes here \n");
       qdevice(SEVENKEY);
       qenter(SEVENKEY, 1);
       printf("Die man die  SEVENKEY = %d\n", isqueued(SEVENKEY));
       qenter(SEVENKEY, 0);
       active = 0;
       }
     if (active == 5)
       {
       printf("Prone DIGUY code goes here \n");
       qdevice(NINEKEY);
       qenter(NINEKEY, 1);
       printf("Prone man prone  NINEKEY = %d\n", isqueued(NINEKEY));
       qenter(NINEKEY, 0);
       active = 0;
       }
     if (active == 6)
       {
       printf("Fire Gun code goes here \n");
       qdevice(HOMEKEY);
       qenter(HOMEKEY, 1);
       printf("Shoot the rifle man shoot  HOMEKEY = %d\n", isqueued(HOMEKEY));
       qenter(HOMEKEY, 0);
       active = 0;
       }
     if (active == 2)
       {
       printf("Upright DIGUY code goes here \n");
       qdevice(EKEY);
       qenter(EKEY, 1);
       printf("Get up man get up  EKEY = %d\n", isqueued(EKEY));
       qenter(EKEY, 0);
       active = 0;
       }
     if (active == 3)
       {
       printf("Fire Missile code goes here \n");
       qdevice(ENDKEY);
       qenter(ENDKEY, 1);
       printf("Fire that missle man  ENDKEY = %d\n", isqueued(ENDKEY));
       qenter(ENDKEY, 0);
       active = 0;
       }

}

//
// Create a simple hide routine
void create_icon(Widget w)
  {
  int iconStatus;

   printf("I'm in create_icon \n");

  iconStatus = XIconifyWindow(XtDisplay(w), XtWindow(w), DefaultScreen(XtDisplay(w)));
  }


// Create system window
void create_system()
  {
  static Widget syswindow, dshell, frame, label, rowcol, menu, filemenu;

   printf("I'm in create_system /n");

  dshell = XtCreatePopupShell("shell", topLevelShellWidgetClass, toplevel, NULL, 0);
  XtVaSetValues(dshell,
        XmNtitle,           "System Window",
        XmNminWidth,        320,
        XmNminHeight,       261,
        XmNmaxWidth,        780,
        XmNmaxHeight,       540,
        XmNx,               200,
        XmNy,               115,
        XmNdeleteResponse,  XmDESTROY,
        XmNmwmDecorations,  MWM_DECOR_BORDER|MWM_DECOR_TITLE|MWM_DECOR_MENU,
        NULL);
 printf("I am in create_system \n"); 
  syswindow = XtCreateManagedWidget("sysWindow", xmMainWindowWidgetClass, dshell, NULL, 0);
  frame = XtCreateManagedWidget("frame", xmFrameWidgetClass, syswindow,
                                NULL, 0);



	XtAddCallback(frame, XmNokCallback, (XtCallbackProc) XtUnmanageChild, NULL);
 	XtAddCallback(frame, XmNcancelCallback, (XtCallbackProc) XtUnmanageChild, NULL);
     
        XtAddEventHandler(frame, KeyPressMask, False, (XtEventHandler) ok_handler, NULL);


  menu = XmCreateMenuBar(syswindow, "menu", NULL, 0);
  XtManageChild(menu); 
  filemenu = XmCreatePulldownMenu(menu, "filemenu", NULL, 0);
  XtVaSetValues(syswindow,
                XmNtitle,           "System Window",
                XmNshowSeparator,   True,
		XmNmenuBar,		menu,
		XmNscrollBarDisplayPolicy,	XmSTATIC,
                XmNdefaultPosition, False,
                NULL);
printf("npstrue = %d\n",npstrue);

if (npstrue == 0)
 {
    XtManageChild(syswindow);
    XtPopup(XtParent(syswindow), XtGrabNone);
    XtRealizeWidget(dshell);
    XtPopdown(XtParent(syswindow));
    XtPopup(XtParent(syswindow), XtGrabNone);
 }
else
    XtUnmanageChild(syswindow);





  }



/*******************************************************************
 *  Create a Motif FileSelectionDialog with some callbacks.        *
 *******************************************************************/
void create_fsb()
{
    static Widget popup;
   
    printf("I'm in create_fsb \n");
 
    /*
     * Avoid creating multiples copies of the dialog.
     */
    if (!popup)
    {
	popup = XmCreateFileSelectionDialog(toplevel, "FileSelectionBox", 
	    NULL, 0);
    	XtVaSetValues(popup, 
	    XmNtitle,           "File Selection Dialog",
	    XmNx, 		25, 
	    XmNy, 		60, 
	    XmNdefaultPosition, False, 
	    NULL);
	/*
	 * Add callbacks for the Ok and Cancel buttons along with
	 * callback that handle input focus.
	 */
	XtAddCallback(popup, XmNokCallback, (XtCallbackProc) XtUnmanageChild, NULL);
 	XtAddCallback(popup, XmNcancelCallback, (XtCallbackProc) XtUnmanageChild, NULL);
	XtAddCallback(popup, XmNfocusCallback, (XtCallbackProc) file_focus, NULL);
        /*
         * Add event handler for OSV Ok button.
         */
        XtAddEventHandler(popup, KeyPressMask, False, (XtEventHandler) ok_handler, NULL);
        /*
	 * Set Tab groups.
	 */
	XtVaSetValues(
	    (XmFileSelectionBoxGetChild(popup, XmDIALOG_OK_BUTTON)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
	XtVaSetValues(
	    (XmFileSelectionBoxGetChild(popup, XmDIALOG_APPLY_BUTTON)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
	XtVaSetValues(
	    (XmFileSelectionBoxGetChild(popup, XmDIALOG_CANCEL_BUTTON)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
	XtVaSetValues(
	    (XmFileSelectionBoxGetChild(popup, XmDIALOG_HELP_BUTTON)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
	XtVaSetValues(
	    (XmFileSelectionBoxGetChild(popup, XmDIALOG_DIR_LIST)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
	XtVaSetValues(
	    (XmFileSelectionBoxGetChild(popup, XmDIALOG_FILTER_TEXT)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
	XtVaSetValues((XmFileSelectionBoxGetChild(popup, XmDIALOG_TEXT)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
	XtVaSetValues((XmFileSelectionBoxGetChild(popup, XmDIALOG_LIST)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
    	/*
    	 * Override the program-defined translations
    	 */
    	XtOverrideTranslations(popup, trans_table);
    	XtOverrideTranslations(
	    (XmFileSelectionBoxGetChild(popup, XmDIALOG_DIR_LIST)),
	    trans_table);
    	XtOverrideTranslations(
	    (XmFileSelectionBoxGetChild(popup, XmDIALOG_FILTER_TEXT)),
	    trans_table);
    	XtOverrideTranslations(
	    (XmFileSelectionBoxGetChild(popup, XmDIALOG_TEXT)),
	    trans_table);
    	XtOverrideTranslations(
	    (XmFileSelectionBoxGetChild(popup, XmDIALOG_LIST)),
	    trans_table);
    }

if (npstrue == 0)
    XtManageChild(popup);
else
    XtUnmanageChild(popup);

}
void create_next()
{
     int index;
     XmString label;

     counter++;

printf("I am in create_next \n");
    
     if ((counter == 4) || (counter == 12))
   {
    /*
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /*
     * Change the labels on the OSV window.
     */
    for (index = 0; index < 17; index++)
    {
        if ((strstr(npskeylabels1[index], "\n")) == NULL)
            label = XmStringCreateSimple(npskeylabels1[index]);
        else
            label = XmStringCreateLtoR(npskeylabels1[index], XmSTRING_DEFAULT_CHARSET);
        XtVaSetValues(widget[index],
        XmNlabelString, label,
        XmNresizable,   False,
        NULL);
     }
    /*
     * Changing callbacks .....
     */

    for (index = 0; index < 17; index++)
    {
       XtAddCallback(widget[index], XmNactivateCallback,
                   (XtCallbackProc) default_nps, (XtPointer)index);
    }

    /*
     * Set the foreground color of the Toggle key to grey(12).
     */
    XtVaSetValues(widget[5], XmNforeground, 12, NULL);

  }
 else if ((counter == 9) || (counter == 27))
  {

    /*
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /*
     * Change the labels on the OSV window.
     */
    for (index = 0; index < 17; index++)
    {
        if ((strstr(npskeylabels2[index], "\n")) == NULL)
            label = XmStringCreateSimple(npskeylabels2[index]);
        else
            label = XmStringCreateLtoR(npskeylabels2[index], XmSTRING_DEFAULT_CHARSET);
        XtVaSetValues(widget[index],
        XmNlabelString, label,
        XmNresizable,   False,
        NULL);
     }
    /*
     * Changing callbacks .....
     */

    for (index = 0; index < 17; index++)
    {
       XtAddCallback(widget[index], XmNactivateCallback,
                   (XtCallbackProc) default_nps, (XtPointer)index);
    }

    /*
     * Set the foreground color of the Toggle key to grey(12).
     */
    XtVaSetValues(widget[5], XmNforeground, 12, NULL);

  }

}
void create_prev()
{
     int index;
     XmString label;

     counter--;

printf("I'm in create_prev \n");

     if (counter <= 0)
        counter = 0;
 
     if (counter == 0) 
   {
    /*
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /*
     * Change the labels on the OSV window.
     */
    for (index = 0; index < 17; index++)
    {
        if ((strstr(npskeylabels[index], "\n")) == NULL)
            label = XmStringCreateSimple(npskeylabels[index]);
        else
            label = XmStringCreateLtoR(npskeylabels[index], XmSTRING_DEFAULT_CHARSET);
        XtVaSetValues(widget[index],
        XmNlabelString, label,
        XmNresizable,   False,
        NULL);
     }
    /*
     * Changing callbacks .....
     */

    for (index = 0; index < 17; index++)
    {
       XtAddCallback(widget[index], XmNactivateCallback,
                   (XtCallbackProc) default_nps, (XtPointer)index);
    }

    /*
     * Set the foreground color of the Toggle key to grey(12).
     */
    XtVaSetValues(widget[5], XmNforeground, 12, NULL);

  }


     if (counter == 3) 
   {
    /*
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /*
     * Change the labels on the OSV window.
     */
    for (index = 0; index < 17; index++)
    {
        if ((strstr(npskeylabels1[index], "\n")) == NULL)
            label = XmStringCreateSimple(npskeylabels1[index]);
        else
            label = XmStringCreateLtoR(npskeylabels1[index], XmSTRING_DEFAULT_CHARSET);
        XtVaSetValues(widget[index],
        XmNlabelString, label,
        XmNresizable,   False,
        NULL);
     }
    /*
     * Changing callbacks .....
     */

    for (index = 0; index < 17; index++)
    {
       XtAddCallback(widget[index], XmNactivateCallback,
                   (XtCallbackProc) default_nps, (XtPointer)index);
    }

    /*
     * Set the foreground color of the Toggle key to grey(12).
     */
    XtVaSetValues(widget[5], XmNforeground, 12, NULL);

  }


}
void create_npsmenu()
{
    int         index;
    XmString    label;


    printf("I'm in create_npsmenu \n");

    npsmenu = 1;

    printf("npsmenu = %d\n", npsmenu);
    /*
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /*
     * Change the labels on the OSV window.
     */
    for (index = 0; index < 17; index++)
    {
        if ((strstr(npskeylabels[index], "\n")) == NULL)
            label = XmStringCreateSimple(npskeylabels[index]);
        else
            label = XmStringCreateLtoR(npskeylabels[index], XmSTRING_DEFAULT_CHARSET);
        XtVaSetValues(widget[index],
        XmNlabelString, label,
        XmNresizable,   False,
        NULL);
    }

    /*
     * Changing callbacks .....
     */

    for (index = 0; index < 17; index++)
    {
       XtAddCallback(widget[index], XmNactivateCallback,
                   (XtCallbackProc) default_nps, (XtPointer)index);
    }

    /*
     * Set the foreground color of the Toggle key to grey(12).
     */
    XtVaSetValues(widget[5], XmNforeground, 12, NULL);

}
/*******************************************************************
 *  Create a Motif InformationDialog for nps callbacks.            *
 *******************************************************************/
void create_npsmessage()
{
    
printf("I am in create_npsmessage \n");   
    static Widget popup5;
    XmString      message;



    /*
     * Avoid creating multiples copies of the dialog.
     */
    if (!popup5)
    {
        popup5 = XmCreateInformationDialog(toplevel, "SelectionBox", NULL, 0);
        message = XmStringCreateSimple("This button is inactive.");
        XtVaSetValues(popup5,
            XmNtitle,           "Information Dialog",
            XmNmessageString,   message,
            XmNx,               350,
            XmNy,               500,
            XmNdefaultPosition, False,
            NULL);
        /*
         * Add callbacks for the Ok and Cancel buttons along with
         * callback that handle input focus.
        */
        XtAddCallback(popup5, XmNokCallback, (XtCallbackProc) XtUnmanageChild, NULL);
        XtAddCallback(popup5, XmNcancelCallback, (XtCallbackProc) XtUnmanageChild, NULL);
        /*
         * Add event handler for OSV Ok button.
         */
        XtAddEventHandler(popup5, KeyPressMask, False, (XtEventHandler) ok_handler, NULL);
        /*
         * Set Tab groups.
         */
        XtVaSetValues(
            (XmMessageBoxGetChild(popup5, XmDIALOG_OK_BUTTON)),
            XmNnavigationType, XmTAB_GROUP, NULL);
        XtVaSetValues(
            (XmMessageBoxGetChild(popup5, XmDIALOG_CANCEL_BUTTON)),
            XmNnavigationType, XmTAB_GROUP, NULL);
        XtVaSetValues(
            (XmMessageBoxGetChild(popup5, XmDIALOG_HELP_BUTTON)),
            XmNnavigationType, XmTAB_GROUP, NULL);
    }

    XtManageChild(popup5);
    XtPopup(XtParent(popup5), XtGrabNone);


    
}
/*******************************************************************
 *  Create a Motif InformationDialog with some callbacks.          *
 *******************************************************************/
void create_info()
{
    static Widget popup2;
    XmString	  message;
   
    printf("I'm in create_info \n"); 
    /*
     * Avoid creating multiples copies of the dialog.
     */
    if (!popup2)
    {
    	popup2 = XmCreateInformationDialog(toplevel, "SelectionBox", NULL, 0);
	message = XmStringCreateSimple("This is an information window.");
    	XtVaSetValues(popup2, 
	    XmNtitle,           "Information Dialog",
	    XmNmessageString,	message,
	    XmNx, 		350, 
	    XmNy, 		500, 
	    XmNdefaultPosition, False, 
	    NULL);
	/*
	 * Add callbacks for the Ok and Cancel buttons along with
	 * callback that handle input focus.
	*/
	XtAddCallback(popup2, XmNokCallback, (XtCallbackProc) XtUnmanageChild, NULL);
 	XtAddCallback(popup2, XmNcancelCallback, (XtCallbackProc) XtUnmanageChild, NULL);
	XtAddCallback(popup2, XmNfocusCallback, (XtCallbackProc) info_focus, NULL);
        /*
         * Add event handler for OSV Ok button.
         */
        XtAddEventHandler(popup2, KeyPressMask, False, (XtEventHandler) ok_handler, NULL);
        /*
	 * Set Tab groups.
	 */
	XtVaSetValues(
	    (XmMessageBoxGetChild(popup2, XmDIALOG_OK_BUTTON)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
	XtVaSetValues(
	    (XmMessageBoxGetChild(popup2, XmDIALOG_CANCEL_BUTTON)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
	XtVaSetValues(
	    (XmMessageBoxGetChild(popup2, XmDIALOG_HELP_BUTTON)),
	    XmNnavigationType, XmTAB_GROUP, NULL);
    	/*
    	 * Override the program-defined translations
    	 */
    	XtOverrideTranslations(popup2, trans_table);
    }

if (npstrue == 0)
  {
    XtManageChild(popup2);
    XtPopup(XtParent(popup2), XtGrabNone);
  }
else
    XtUnmanageChild(popup2);

}
/*******************************************************************
* Create the callback for keypress 3                               *
********************************************************************/
void create_nps()
{
printf("I am in create_nps \n");
    int         index;
    XmString    label;
    npstrue = 1;

    /*
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /*
     * Change the labels on the OSV window.
     */
    for (index = 0; index < 17; index++)
    {
        if ((strstr(npslabels[index], "\n")) == NULL)
            label = XmStringCreateSimple(npslabels[index]);
        else
            label = XmStringCreateLtoR(npslabels[index], XmSTRING_DEFAULT_CHARSET);
        XtVaSetValues(widget[index],
        XmNlabelString, label,
        XmNresizable,   False,
        NULL);
    }

    /*
     * Changing callbacks .....
     */

    for (index = 0; index < 17; index++)
    {
       XtAddCallback(widget[index], XmNactivateCallback,
                   (XtCallbackProc) default_nps, (XtPointer)index);
    }
printf("Just after default_nps was called\n");
    /*
     * Set the foreground color of the Toggle key to grey(12).
     */
    XtVaSetValues(widget[5], XmNforeground, 12, NULL);

}
/*******************************************************************
 *  Create a Motif Dialog Shell with a Text box and a push button. *
 *******************************************************************/
void create_text()
{
    static Widget	dshell, menubar, menu, popup3; 
    static Widget       shell_form, help_widget, pushb;
    
printf("I'm in create_text \n");

    XmString  file, edit, help, new1, quit, open, copy, del;  
    Position	x, y;
    int		n;
    
    /*
     * Avoid creating multiples copies of the shell widget.
     */
    if ((dshell != 0) && (XtIsRealized(dshell) == True))
	return;

    /*
     * Get the position of the OSV window.
     */
    XtVaGetValues(toplevel, XmNx, &x, XmNy, &y, NULL);
    dshell = XtCreateApplicationShell("shell", 
        topLevelShellWidgetClass, NULL, 0);
    /*
     * Create the shell.
     */
    XtVaSetValues(dshell, 
	XmNtitle,           "Shell with Text Box",
	XmNminWidth,	    261,
	XmNminHeight,	    320,
	XmNmaxWidth,	    261,
	XmNmaxHeight,	    320,
	XmNx, 		    400, 
	XmNy, 		    115, 
	XmNdeleteResponse,  XmDESTROY,
	XmNmwmDecorations,  MWM_DECOR_BORDER|MWM_DECOR_TITLE|MWM_DECOR_MENU,
	NULL);
    /*
     * Add callback to unrealize the window when the Close item from
     * the window menu is selected.
     */
    XtAddCallback(dshell, XmNdestroyCallback, (XtCallbackProc) pushb_callback, dshell);
    
    /*
     * Create a XmForm widget to hold the XmText and 
     * XmMenuBar widgets.
     */
    shell_form = XtCreateWidget("shell_form", xmFormWidgetClass,
        dshell, NULL, 0);
    /*
     * Create a simple MenuBar that contains three menus 
     */
    file = XmStringCreateSimple("File");
    edit = XmStringCreateSimple("Edit");
    help = XmStringCreateSimple("Help");
    menubar = XmVaCreateSimpleMenuBar(shell_form, "menubar",
        XmVaCASCADEBUTTON, file, 'F',
	XmVaCASCADEBUTTON, edit, 'E',
	XmVaCASCADEBUTTON, help, 'H',
	XmNleftAttachment,  XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNnavigationType,  XmTAB_GROUP,
	XmNmenuAccelerator, "Ctrl<Key>m",
	NULL);
    XmStringFree(file);
    XmStringFree(edit);

    /*
     * Tell the menubar which button is the help menu 
     */
    if (help_widget = XtNameToWidget(menubar, "button_2"))
        XtVaSetValues(menubar, XmNmenuHelpWidget, help_widget, NULL);

    /*
     * First menu is the File menu -- callback is file_cb()
     */
    new1  = XmStringCreateSimple("New ...");
    quit = XmStringCreateSimple("Quit");
    XmVaCreateSimplePulldownMenu(menubar, "file_menu", 0, (XtCallbackProc) file_cb,
        XmVaPUSHBUTTON, new1, 'N', NULL, NULL,
        XmVaSEPARATOR, 
	XmVaPUSHBUTTON, quit, 'Q', NULL, NULL,
	NULL);
    XmStringFree(new1);
    XmStringFree(quit);

    /*
     * Second menu is the Edit menu -- callback is edit_cb() 
     */
    open = XmStringCreateSimple("Open");
    copy = XmStringCreateSimple("Copy");
    del  = XmStringCreateSimple("Delete");
    menu = XmVaCreateSimplePulldownMenu(menubar, "edit_menu", 1, (XtCallbackProc) edit_cb,
	XmVaPUSHBUTTON, open, 'O', NULL, NULL,
	XmVaPUSHBUTTON, copy, 'C', NULL, NULL,
	XmVaSEPARATOR,
        XmVaPUSHBUTTON, del, 'D', NULL, NULL,
	NULL);
    XmStringFree(open);
    XmStringFree(copy);
    XmStringFree(del);

    /*
     * Third menu is help_menu -- callback is the help_cb() 
     */
    XmVaCreateSimplePulldownMenu(menubar, "help_menu", 2, (XtCallbackProc) help_cb,
        XmVaPUSHBUTTON, help, 'H', NULL, NULL,
        NULL);
    XmStringFree(help);
	
    popup3 = XmCreateText(shell_form, "Text3", NULL, 0);
    XtVaSetValues(popup3,
	XmNy,	    	    30,
        XmNrows,            18,
        XmNcolumns,         40, 
	XmNnavigationType,  XmTAB_GROUP,
        NULL);
    /*
     * Create the XmPushButton widget.
     */
    n = set_properties("Done", 64, 32);
    XtSetArg(wargs[n], XmNx, 102);
    n++;
    XtSetArg(wargs[n], XmNy, 285);
    n++;
    pushb = XtCreateManagedWidget("push", xmPushButtonWidgetClass,
        shell_form, wargs, n);
    /*
     * Add callback
     */
    XtAddCallback(pushb, XmNactivateCallback, (XtCallbackProc) pushb_callback, dshell);
    /*
     * Add event handler for OSV Ok button.
     */
    XtAddEventHandler(pushb, KeyPressMask, False, (XtEventHandler) ok_handler2, NULL);
    /*
     * Add an in and out focus callback.
     */
    XtAddCallback(popup3, XmNfocusCallback, (XtCallbackProc) text_focus, NULL);
    XtAddCallback(popup3, XmNlosingFocusCallback, (XtCallbackProc) default_focus, NULL);
    /*
     * Override the program-defined translations
     */
    XtOverrideTranslations(popup3, trans_table);
    XtOverrideTranslations(menubar, trans_table);
    XtOverrideTranslations(pushb, trans_table);
    /*
     * Realize all widgets and set the initial position of the
     * shell widget.
     */

if (npstrue == 0)
  {
    XtManageChild(menubar);
    XtManageChild(popup3);
    XtManageChild(shell_form);
    XtRealizeWidget(dshell);
    XtVaSetValues(dshell, XmNx, 425, XmNy, 100, NULL);
  }
else
    XtUnmanageChild(shell_form);
}

/******************************************************************
 *  Callback for the push button on the "shell" window.           *
 ******************************************************************/
void pushb_callback(Widget w, Widget window, XmAnyCallbackStruct *call_data)
{
printf("I'm in pushb_callback \n");
    /*
     * Destroy the "shell" window.
     */
    XtUnrealizeWidget(window);
}

/****************************************************************
 *  Callback for the File menu on the "shell" window.           *
 ****************************************************************/
void file_cb(Widget w, int item_no)
{

  printf("I'm in file_cb \n");
    /*
     * Check which item was selected.
     */
    if (item_no == 1)
	printf("Quit item was selected\n");
    else
	printf("New item was selected\n");
}

/****************************************************************
 *  Callback for the Edit menu on the "shell" window.           *
 ****************************************************************/
void edit_cb(Widget w, int item_no)
{

  printf("I'm in edit_cb \n");
    /*
     * Check which item was selected.
     */
    if (item_no == 0)
	printf("Open item was selected\n");
    if (item_no == 1)
        printf("Copy item was selected\n");
    if (item_no == 2)
        printf("Del item was selected\n");
}

/****************************************************************
 *  Callback for the Help menu on the "shell" window.           *
 ****************************************************************/
void help_cb()
{
    printf("In help_cb\n");
}

/****************************************************************
 *  Callback for the #1 button on the OSV.                      *
 ****************************************************************/
void kill_application()
{
 printf("I'm in kill_application \n");
    /*
     * Destroy the OSV window and all its children.
     */
    XtCloseDisplay(XtDisplay(toplevel));
    /* exit(0); */
    return;
}

/****************************************************************
 *  Callback for the #0 button on the OSV.                      *
 ****************************************************************/
void default_labels(Widget w, int client_data, XmAnyCallbackStruct *call_data)
{
printf("I'm in default_labels \n");

    /*
     * Display the default labels on the OSV buttons.
     */
    default_focus(w, (caddr_t) client_data, call_data);
}

/*******************************************************************
*  Create a Motif PromptDialog with some callbacks                *
 *******************************************************************/
void create_message_box()
{
/*
    static Widget  popup4;
*/
   printf("I'm in create_message_box \n"); 
    /*
     * Avoid creating multiples copies of the dialog.
     */
    if (!popup4)
    {
	popup4 = XmCreatePromptDialog(toplevel, "MessageBox", NULL, 0);
    	XtVaSetValues(popup4, 
	    XmNtitle, 			"Prompt Dialog",
	    XmNx, 			850, 
    	    XmNy, 			150, 
	    XmNdefaultPosition, 	False, 
	    XmNselectionLabelString,	XmStringCreateSimple("Range:"),
	    NULL);
        /*
         * Add event handler for OSV Ok button.
         */
        XtAddEventHandler(popup4, KeyPressMask, False, (XtEventHandler) ok_handler, NULL);
        /*
         * Set Tab groups.
         */
        XtVaSetValues((XmSelectionBoxGetChild(popup4, XmDIALOG_OK_BUTTON)),
            XmNnavigationType, XmTAB_GROUP, NULL);
        XtVaSetValues((XmSelectionBoxGetChild(popup4, XmDIALOG_CANCEL_BUTTON)),
            XmNnavigationType, XmTAB_GROUP, NULL);
        XtVaSetValues((XmSelectionBoxGetChild(popup4, XmDIALOG_HELP_BUTTON)),
            XmNnavigationType, XmTAB_GROUP, NULL);
        XtVaSetValues((XmSelectionBoxGetChild(popup4, XmDIALOG_TEXT)),
            XmNnavigationType, XmTAB_GROUP, NULL);
	/*
	 * Add a callback for the Cancel button along with
	 * callbacks that handle input focus.
	 */
 	XtAddCallback(popup4, XmNcancelCallback, (XtCallbackProc) done_callback, popup4);
	XtAddCallback(popup4, XmNfocusCallback, (XtCallbackProc) file_focus, NULL);
 	XtAddCallback((XmSelectionBoxGetChild(popup4, XmDIALOG_OK_BUTTON)),
	    XmNactivateCallback, (XtCallbackProc) done_callback, popup4);
	XtAddCallback(XmSelectionBoxGetChild(popup4, XmDIALOG_TEXT), 
	    XmNfocusCallback, (XtCallbackProc) number_focus, popup4);
    	/*
	XtAddCallback(XmSelectionBoxGetChild(popup4, XmDIALOG_TEXT), 
	    XmNlosingFocusCallback, file_focus, NULL);
    	 * Override the program-defined translations
    	 */
        XtOverrideTranslations(popup4, trans_table);
    	XtOverrideTranslations(XmSelectionBoxGetChild(popup4, XmDIALOG_TEXT), trans_table3);
    }

if (npstrue == 0)
    XtManageChild(popup4);
else
    XtUnmanageChild(popup4);
}

/******************************************************************
 *  Callback executed when either the Ok or Cancel buttons        *
 *  are selected.                                                 *
 ******************************************************************/
void done_callback(Widget w, Widget window, XmAnyCallbackStruct *call_data)
{
printf("I'm in done_callback \n");

    /*
     * Unmanage the dialog shell.
     */
    XtUnmanageChild(window);
    file_focus(w, NULL, call_data);
    receive = 0;
}


/********************************************************************
 * The Ok button was selected from the FileSelectionDialog (or, the *
 * user double-clicked on a file selection).                        *
 ********************************************************************/
void load_file(Widget dialog, XtPointer client_data)
{
        printf("In load_file\n");
}

/********************************************************************
 * Callback executed when the FileSelectionDialog gets input focus. *
 ********************************************************************/
void file_focus(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
{
    int		index;
    XmString	label;

printf("I'm in file_focus \n");
    /* 
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /* 
     * Change the labels on the OSV window.
     */
    for (index = 0; index < 17; index++)
    {
	if ((strstr(file_labels[index], "\n")) == NULL)
	    label = XmStringCreateSimple(file_labels[index]);
	else
	    label = XmStringCreateLtoR(file_labels[index], XmSTRING_DEFAULT_CHARSET);
    	XtVaSetValues(widget[index],
	XmNlabelString,	label,
	XmNresizable,	False,
	NULL);
    }
    /*
     * Set the foreground color of the Toggle key to grey(12).
     */
    XtVaSetValues(widget[5], XmNforeground, 12, NULL);
}

/********************************************************************
 * Callback executed when the InformationDialog gets input focus.   *
 ********************************************************************/
void info_focus(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
{
    int         index;
    XmString    label;

printf("I'm in info_focus \n");
    /*
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /* 
     * Change the labels on the OSV window.
     */
    for (index = 0; index < 17; index++)
    {
        if ((strstr(info_labels[index], "\n")) == NULL)
            label = XmStringCreateSimple(info_labels[index]);
        else
            label = XmStringCreateLtoR(info_labels[index], XmSTRING_DEFAULT_CHARSET);
        XtVaSetValues(widget[index],
        XmNlabelString, label,
        XmNresizable,   False,
        NULL);
    }
    /*
     * Set the foreground color of the Toggle key to grey(12).
     */
    XtVaSetValues(widget[5], XmNforeground, 12, NULL);
}

/********************************************************************
 * Callback executed when the Shell with the Text box gets input    *
 * focus.                                                           *
 ********************************************************************/
void text_focus(Widget w, caddr_t client_data, XmAnyCallbackStruct * call_data)
{
    int         index;
    XmString    label;


printf("I'm in text_focus \n"); 
    /*
     * Disable the numeric mode.
     */
    mode = 0;
    receive = 0;
    /* 
     * Change the labels on the OSV window.
     */
    for (index = 0; index < 17; index++)
    {
        if ((strstr(text_labels[index], "\n")) == NULL)
            label = XmStringCreateSimple(text_labels[index]);
        else
            label = XmStringCreateLtoR(text_labels[index], XmSTRING_DEFAULT_CHARSET);
        XtVaSetValues(widget[index],
        XmNlabelString, label,
        XmNresizable,   False,
        NULL);
    }
    /*
     * Set the foreground color of the Toggle key to grey(12).
     */
    XtVaSetValues(widget[5], XmNforeground, 12, NULL);
}

/********************************************************************
 * Callback executed when the Shell with the Text box loses input   *
 * focus or the #0 button on the OSV window is selected.            *
 ********************************************************************/
void default_focus(Widget w, caddr_t client_data, XmAnyCallbackStruct *call_data)
{
    int         index;
    XmString    label;


printf("I'm in default_focus \n"); 
    /*
     * Disable the numeric mode.
     */
    mode = 0;
    /* 
     * Change the labels on the OSV window to their default value.
     */
    for (index = 0; index < 17; index++)
    {
        if ((strstr(labels[index], "\n")) == NULL)
            label = XmStringCreateSimple(labels[index]);
        else
            label = XmStringCreateLtoR(labels[index], XmSTRING_DEFAULT_CHARSET);
        XtVaSetValues(widget[index],
        XmNlabelString, label,
        XmNresizable,   False,
        NULL);
    }
    if (receive == NULL)
    {
        /*
         * Set the foreground color of the Toggle key to grey(12).
         */
        XtVaSetValues(widget[5], XmNforeground, 12, NULL);
    }
}

/********************************************************************
 * Callback executed when a Text field that accept numbers as input *
 * get input focus.                                                 *
 ********************************************************************/
void number_focus(Widget w, Widget client_data, XmAnyCallbackStruct *call_data)
{
    int         index;
    XmString    label;


printf("I'm in number_focus \n"); 
    /*
     * Enable the numeric mode.
     */
    mode = 1;
    receive = w;
    /* 
     * Change the labels on the OSV window.
     */
    for (index = 0; index < 17; index++)
    {
        if ((strstr(num_labels[index], "\n")) == NULL)
            label = XmStringCreateSimple(num_labels[index]);
        else
            label = XmStringCreateLtoR(num_labels[index], XmSTRING_DEFAULT_CHARSET);
        XtVaSetValues(widget[index],
        XmNlabelString, label,
        XmNresizable,   False,
        NULL);
    }
    /*
     * Set the foreground color of the Toggle key to white(1)
     * for the TAC-3 or black(1) for Sparcstation.
     */
    XtVaSetValues(widget[5], XmNforeground, 1, NULL);
}

/****************************************************************
 *  Creates a cascade button and attach it to the form widget.  *
 ****************************************************************/
Widget cascade_button(char *name, Widget menu_bar, int index)
{
    int    n;
    Arg    arg[5];
    Widget cascade;


printf("I'm in cascade_button \n");
    n = 0;
    if (index == 0)
    {
        menu = XmCreatePulldownMenu(menu_bar, rbutton_names[index], NULL, 0);
        XtSetArg(arg[n], XmNsubMenuId, menu);
        n++;
    }
    XtSetArg(arg[n], XmNlabelString,
        XmStringCreateLtoR(name, char_set));
    n++;
    XtSetArg(arg[n], XmNwidth, 64);
    n++;
    XtSetArg(arg[n], XmNheight, 32);
    n++;
    cascade = XmCreateCascadeButton(menu_bar, name, arg, n);
    XtManageChild(cascade);

    return(cascade);
}

/*************************************************************
 *  Attach a pulldown menu to the specified cascade button.  *
 *************************************************************/
void add_pulldown()
{
    int i, n, sub_index;
    Arg arg[10];


printf("I'm in add_pulldown \n");
    for (i = 0; i < XtNumber(pd_names); i++)
    {
        n = 0;
        XtSetArg(arg[n], XmNlabelString,
            XmStringCreateLtoR(pd_names[i], char_set));
        n++;
        pull_down[i] = XmCreatePushButton(menu, pd_names[i], arg, n);
        XtManageChild(pull_down[i]);
        XtAddCallback(pull_down[i], XmNactivateCallback,
            (XtCallbackProc) pulldown_callback, (XtPointer) i);
        XtSetSensitive(pull_down[i], True);
    }
}

/***********************************************************
 *  Callback executed when any of the cascade buttons in   *
 *  the menubar of the resizable window is selected.       * 
 ***********************************************************/
void rbutton_callback(Widget w, int client_data, XmPushButtonCallbackStruct *call_data)
{
    printf("Button #%d selected\n", client_data);
}

/***********************************************************
 *  Callback executed when any of the push buttons in the  *
 *  pulldown menu of the resizable window is selected.     * 
 ***********************************************************/
void pulldown_callback(Widget w, int client_data, XmPushButtonCallbackStruct *call_data)
{
    printf("Button #%d selected\n", client_data);
}
