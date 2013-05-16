// File: <vabs.cc>


/**********************************************************************
 *                                                                    *
 *      File:         vabs.c                                          *
 *      Date:         3/29/93                                         *
 *      Developed by: Jose Vazquez                                    *
 *                    NCCOSC RDTE DIV                                 *
 *                    Code 842                                        *
 *                    San Diego, CA  92152-5000                       *
 *      Modification: Start date 8/22/94                              *
 *                    Programmer Bryan Croft                          *
 *                    Project  CommandVu                              *
 *                                                                    *
 **********************************************************************/

#include "vabs.h"
#include <stdio.h>
#include <stdlib.h>
/*
 * Default labels on the OSV buttons 
 */
char    *labels[] = {
        "Num/\nFunc\nPad", "System", "Overlays",
        "Exit", "OK", "Iconify\nWindow", "Terrain", "Object\nControl",
        "3D\nNav", "Prev\nTAB\nGroup", "Display\nControl", "Analysis", "Appli-\ncations",
        "APPLY", "Next\nTAB\nGroup", "RESET", "CANCEL",
};
 

 
/*
 * Widget names for the OSV buttons 
 */
char    *names[] = {
        "msg", "msg2", "msg3", "msg4", "msg5", "msg6",
        "msg7", "msg8", "msg9", "msg10", "msg11", "msg12",
        "msg13", "msg14", "msg15", "msg16", "msg17",
};
 
 
/*
 * Window ID
 */
char *ATOM_NAME = "atom1";

/*main(argc, argv)
int	 argc;
char	*argv[]; */

void init_vabs(int argc, char *argv[])
{
    Widget	separator[3];
    int		n, index = 0;

     printf("In the init_vabs routine\n");

    /*
     * Initialize the Intrinsics
     */
     toplevel = XtInitialize(argv[0], "Vabs", NULL, 0, &argc, argv); 
/* width 257 height 320 max w = 514 h = 640 */
    XtVaSetValues(toplevel,
	XmNminWidth,	257,
	XmNminHeight,	320,
	XmNmaxWidth,	257,
	XmNmaxHeight,	320,
        XmNkeyboardFocusPolicy, XmEXPLICIT,
	NULL);
    
    /*
     * Get display variable.
     */
    display = XtDisplay(toplevel);
    /*
     * Register the new actions, and compile translation tables
     */
    XtAddActions(actionsTable, XtNumber(actionsTable));
    trans_table = XtParseTranslationTable(defaultTranslations);
    trans_table3 = XtParseTranslationTable(numericTranslations);
    
    /* 
     * Create a XmForm widget to hold the XmPushButton widgets.
     */
    form = XtCreateWidget("form", xmFormWidgetClass,
		toplevel, NULL, 0);
    /*
     * Create the first XmPushButton widget. was 64 by 64
     */
    n = set_properties(labels[index], 64, 64);
    XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_FORM);
    n++;
    XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_FORM);
    n++;
    widget[index] = XtCreateManagedWidget("msg", xmPushButtonWidgetClass,
		form, wargs, n);
    /*
     * Add callback
     */
    XtAddCallback(widget[index], XmNactivateCallback,
		(XtCallbackProc) default_callback, (XtPointer)index);
    index++;
    
    /*
     * Create four more buttons
     */
    for (;index < 5; index++)
    { /* 64 by 64 or 128 by 64 */
	if (index == 4)
    	    n = set_properties(labels[index], 128, 64);
	else
    	n = set_properties(labels[index], 64, 64);

    	XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_WIDGET);
    	n++;
    	XtSetArg(wargs[n], XmNtopWidget, widget[index-1]);
    	n++;
    	XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_FORM);
    	n++;
    	/*
    	 * Create the XmPushButton widget.
    	 */
    	widget[index] = XtCreateManagedWidget(names[index], xmPushButtonWidgetClass,
		form, wargs, n);
    	/*
    	 * Add callback
    	 */
    	XtAddCallback(widget[index], XmNactivateCallback,
		(XtCallbackProc) default_callback, (XtPointer)index);
    }
    /*
     * Create the sixth button
     */
    n = set_properties(labels[index], 64, 64);
    XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(wargs[n], XmNleftWidget, widget[0]);
    n++;
    XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);
    n++;
    XtSetArg(wargs[n], XmNtopWidget, widget[0]);
    n++;
    widget[index] = XtCreateManagedWidget(names[index], xmPushButtonWidgetClass,
		form, wargs, n);
    /*
     * Add callback
     */
    XtAddCallback(widget[index], XmNactivateCallback,
		(XtCallbackProc) default_callback, (XtPointer)index);
    index++;
    /*
     * Create three more buttons
     */
    for (;index < 9; index++)
    {
    	n = set_properties(labels[index], 64, 64);
    	XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_WIDGET);
    	n++;
    	XtSetArg(wargs[n], XmNleftWidget, widget[index-5]);
    	n++;
    	XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_WIDGET);
    	n++;
    	XtSetArg(wargs[n], XmNtopWidget, widget[index-1]);
    	n++;
    	/*
    	 * Create the XmPushButton widget.
    	 */
    	widget[index] = XtCreateManagedWidget(names[index], xmPushButtonWidgetClass,
		form, wargs, n);
    	/*
     	* Add callback
     	*/
    	XtAddCallback(widget[index], XmNactivateCallback,
		(XtCallbackProc) default_callback, (XtPointer)index);
     }
    /*
     * Create the tenth button
     */
    n = set_properties(labels[index], 64, 64);
    XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(wargs[n], XmNleftWidget, widget[5]);
    n++;
    XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);
    n++;
    XtSetArg(wargs[n], XmNtopWidget, widget[5]);
    n++;
    widget[index] = XtCreateManagedWidget(names[index], xmPushButtonWidgetClass,
		form, wargs, n);
    /*
     * Add callback
     */
    XtAddCallback(widget[index], XmNactivateCallback,
		(XtCallbackProc) default_callback, (XtPointer) index);
    index++;
    /*
     * Create four more buttons
     */
    for (;index < 14; index++)
    {
    	n = set_properties(labels[index], 64, 64);
    	XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_WIDGET);
    	n++;
        if (index == 13)
	    XtSetArg(wargs[n], XmNleftWidget, widget[4]);
	else
    	    XtSetArg(wargs[n], XmNleftWidget, widget[index-4]);
    	n++;
    	XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_WIDGET);
    	n++;
    	XtSetArg(wargs[n], XmNtopWidget, widget[index-1]);
    	n++;
    	/*
    	 * Create the XmPushButton widget.
    	 */
    	widget[index] = XtCreateManagedWidget(names[index], xmPushButtonWidgetClass,
		form, wargs, n);
    	/*
     	* Add callback
     	*/
    	XtAddCallback(widget[index], XmNactivateCallback,
		(XtCallbackProc) default_callback, (XtPointer)index);
    }
    /*
     * Create a fifteenth button
     */
    n = set_properties(labels[index], 64, 64);
    XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_WIDGET);
    n++;
    XtSetArg(wargs[n], XmNleftWidget, widget[9]);
    n++;
    XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET);
    n++;
    XtSetArg(wargs[n], XmNtopWidget, widget[9]);
    n++;
    widget[index] = XtCreateManagedWidget(names[index], xmPushButtonWidgetClass,
                form, wargs, n);
    /*
     * Add callback
     */
    XtAddCallback(widget[index], XmNactivateCallback,
                (XtCallbackProc) default_callback, (XtPointer)index);
    index++;
    /*
     * Create the last two buttons
     */
    for (;index < 17; index++)
    {
    	n = set_properties(labels[index], 64, 128);
    	XtSetArg(wargs[n], XmNleftAttachment, XmATTACH_WIDGET);
    	n++;
    	XtSetArg(wargs[n], XmNleftWidget, widget[index-5]);
    	n++;
    	XtSetArg(wargs[n], XmNtopAttachment, XmATTACH_WIDGET);
    	n++;
    	XtSetArg(wargs[n], XmNtopWidget, widget[index-1]);
    	n++;
    	/*
    	 * Create the XmPushButton widget.
    	 */
    	widget[index] = XtCreateManagedWidget(names[index], xmPushButtonWidgetClass,
                form, wargs, n);
    	/*
    	 * Add callback
    	 */
    	XtAddCallback(widget[index], XmNactivateCallback,
                (XtCallbackProc) default_callback, (XtPointer)index);
    }

    /*
     * Override the program-defined translations with 
     * existing translations for the main window.
     */
    for (index = 0; index < 17; index++)
    	XtOverrideTranslations(widget[index], trans_table);
    

    /*
     * Realize all widgets and enter the event loop.
     * Set the initial position of the two shell widgets.
     */
    XtManageChild(form);

    XtRealizeWidget(toplevel);
    XtVaSetValues(toplevel, XmNx, 860, XmNy, 50, NULL);
    /*
     * Register window ID to window manager.
     */
    register_atom(toplevel, ATOM_NAME);
    /*
     * Set the foreground color of the Toggle key to grey(12).
     */
    // Commented out to leave as is
    // XtVaSetValues(widget[5], XmNforeground, 12, NULL);

    /* No sproc forking done but do a XtDispatchEvent in cvmain. */
    /*  XtMainLoop(); */
    return;
}

/**********************************************************
 *   Define the resources for the push button widgets.    *
 **********************************************************/
int set_properties(char *string, Dimension width, Dimension height)
{
    int  	 n;
    XmString	 message;

    n = 0;
    /*
     * Create the labels for the push buttons.  For multiple lines,
     * the XmStringCreateLtoR function is needed.
     */
    if ((strstr(string, "\n")) == NULL)
    	message = XmStringCreateSimple(string);
    else
    	message = XmStringCreateLtoR(string, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(wargs[n], XmNlabelString, message);
    n++;
    XtSetArg(wargs[n], XmNwidth, width);
    n++;
    XtSetArg(wargs[n], XmNheight, height);
    n++;
    XtSetArg(wargs[n], XmNshadowType, XmSHADOW_ETCHED_OUT);
    n++;
    XtSetArg(wargs[n], XmNpushButtonEnabled, True);
    n++;
    XtSetArg(wargs[n], XmNnavigationType, XmTAB_GROUP);
    n++;

    /*
     * Return the number of resources that has been set up.
     */


    return(n);
}



/***********************************************************
 *  Store the window ID of a widget specified by w in the  *
 *  property of the root window in the form of atoms. The  *
 *  parameter atomname is just a string which represents   *
 *  the atom.                                              *
 *                                                         *
 *  Author: Jimmy Lam                                      *
 ***********************************************************/
Atom register_atom(Widget w, char *atomname)
{
        Window window = XtWindow(w);
        Display *dpy = XtDisplay(w);
        Atom WINDOW_ATOM = XInternAtom(dpy, atomname, 0);

        XChangeProperty(dpy, DefaultRootWindow(dpy), WINDOW_ATOM, XA_WINDOW,
                32, PropModeReplace, (unsigned char *)&window, 1);

        return WINDOW_ATOM;
}
