
/////////////////////////////////////////////////////////////
//
// Source file for OodUI
//
//    This class implements the user interface created in 
//    the interface builder. Normally it is not used directly.
//    Instead the subclass, Ood is instantiated
//
//    To extend or alter the behavior of this class, you should
//    modify the Ood files
//
//    If you do modify this class, limit your changes to 
//    changing the bodies of functions or adding 
//    members below the "//--- End generated code section" markers
//
//    This will allow the builder to integrate changes more easily
//
//    This class is a ViewKit user interface "component".
//    For more information on how components are used, see the
//    "ViewKit Programmers' Manual", and the RapidApp
//    User's Guide.
//
//
/////////////////////////////////////////////////////////////


#include "OodUI.h" // Generated header file for this class
#include <Xm/ArrowB.h> 
#include <Xm/Form.h> 
#include <Xm/Frame.h> 
#include <Xm/Label.h> 
#include <Xm/PushB.h> 
#include <Xm/RowColumn.h> 
#include <Xm/Separator.h> 
#include <Xm/TextF.h> 
#include <Xm/ToggleB.h> 
#include <Vk/VkResource.h>
//---- End generated headers


// These are default resources for widgets in objects of this class
// All resources will be prepended by *<name> at instantiation,
// where <name> is the name of the specific instance, as well as the
// name of the baseWidget. These are only defaults, and may be overriden
// in a resource file by providing a more specific resource name

String  OodUI::_defaultOodUIResources[] = {
        (char*)NULL
};

OodUI::OodUI(const char *name) : VkComponent(name) 
{ 
    // No widgets are created by this constructor.
    // If an application creates a component using this constructor,
    // It must explictly call create at a later time.
    // This is mostly useful when adding pre-widget creation
    // code to a derived class constructor.

    //---- End generated code section

} // End Constructor




OodUI::OodUI(const char *name, Widget parent) : VkComponent(name) 
{ 
    // Call creation function to build the widget tree.

     create(parent);

    //---- End generated code section

} // End Constructor


OodUI::~OodUI() 
{
    // Base class destroys widgets

    //-- End generated code section

} // End destructor



void OodUI::create ( Widget parent )
{
    Arg      args[13];
    Cardinal count;
    count = 0;

    // Load any class-defaulted resources for this object

    setDefaultResources(parent, _defaultOodUIResources  );


    // Create an unmanaged widget as the top of the widget hierarchy

    _baseWidget = _ood = XtVaCreateWidget ( _name,
                                            xmFormWidgetClass,
                                            parent, 
                                            XmNresizePolicy, XmRESIZE_GROW, 
                                            (XtPointer) NULL ); 

    // install a callback to guard against unexpected widget destruction

    installDestroyHandler();


    // Create widgets used in this component
    // All variables are data members of this class

    _scope_main_frame = XtVaCreateManagedWidget  ( "scope_main_frame",
                                                    xmFrameWidgetClass,
                                                    _baseWidget, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_FORM, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_FORM, 
                                                    XmNbottomPosition, 0, 
                                                    XmNrightPosition, 0, 
                                                    XmNtopOffset, 10, 
                                                    XmNbottomOffset, 170, 
                                                    XmNleftOffset, 1080, 
                                                    XmNrightOffset, 10, 
                                                    XmNwidth, 174, 
                                                    XmNheight, 99, 
                                                    (XtPointer) NULL ); 


    _scope_main_form = XtVaCreateManagedWidget  ( "scope_main_form",
                                                   xmFormWidgetClass,
                                                   _scope_main_frame, 
                                                   XmNresizePolicy, XmRESIZE_GROW, 
                                                   XmNwidth, 168, 
                                                   XmNheight, 93, 
                                                   (XtPointer) NULL ); 


    _scope_main_label = XtVaCreateManagedWidget  ( "scope_main_label",
                                                    xmLabelWidgetClass,
                                                    _scope_main_form, 
                                                    XmNlabelType, XmSTRING, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_NONE, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_NONE, 
                                                    XmNtopOffset, 35, 
                                                    XmNbottomOffset, 0, 
                                                    XmNleftOffset, 90, 
                                                    XmNrightOffset, 0, 
                                                    XmNwidth, 72, 
                                                    XmNheight, 25, 
                                                    (XtPointer) NULL ); 


    _radiobox4 = XtVaCreateManagedWidget  ( "radiobox4",
                                             xmRowColumnWidgetClass,
                                             _scope_main_form, 
                                             XmNpacking, XmPACK_COLUMN, 
                                             XmNradioBehavior, True, 
                                             XmNradioAlwaysOne, True, 
                                             XmNtopAttachment, XmATTACH_FORM, 
                                             XmNbottomAttachment, XmATTACH_NONE, 
                                             XmNleftAttachment, XmATTACH_FORM, 
                                             XmNrightAttachment, XmATTACH_NONE, 
                                             XmNtopOffset, 20, 
                                             XmNbottomOffset, 0, 
                                             XmNleftOffset, 5, 
                                             XmNrightOffset, 0, 
                                             XmNwidth, 86, 
                                             XmNheight, 65, 
                                             (XtPointer) NULL ); 


    _scope_up_toggle = XtVaCreateManagedWidget  ( "scope_up_toggle",
                                                   xmToggleButtonWidgetClass,
                                                   _radiobox4, 
                                                   XmNlabelType, XmSTRING, 
                                                   (XtPointer) NULL ); 

    XtAddCallback ( _scope_up_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::scope_raiseCallback,
                    (XtPointer) this ); 


    _scope_down_toggle = XtVaCreateManagedWidget  ( "scope_down_toggle",
                                                     xmToggleButtonWidgetClass,
                                                     _radiobox4, 
                                                     XmNlabelType, XmSTRING, 
                                                     XmNset, True, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _scope_down_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::scope_lowerCallback,
                    (XtPointer) this ); 


    _periscope_label = XtVaCreateManagedWidget  ( "periscope_label",
                                                   xmLabelWidgetClass,
                                                   _baseWidget, 
                                                   XmNlabelType, XmSTRING, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_NONE, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_FORM, 
                                                   XmNrightPosition, 0, 
                                                   XmNtopOffset, 200, 
                                                   XmNbottomOffset, 0, 
                                                   XmNleftOffset, 850, 
                                                   XmNrightOffset, 180, 
                                                   XmNwidth, 234, 
                                                   XmNheight, 30, 
                                                   (XtPointer) NULL ); 


    _scope_info_frame = XtVaCreateManagedWidget  ( "scope_info_frame",
                                                    xmFrameWidgetClass,
                                                    _baseWidget, 
                                                    XmNshadowType, XmSHADOW_OUT, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_FORM, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_FORM, 
                                                    XmNbottomPosition, 0, 
                                                    XmNrightPosition, 0, 
                                                    XmNtopOffset, 230, 
                                                    XmNbottomOffset, 5, 
                                                    XmNleftOffset, 880, 
                                                    XmNrightOffset, 60, 
                                                    XmNwidth, 324, 
                                                    XmNheight, 44, 
                                                    (XtPointer) NULL ); 


    _scope_info_form = XtVaCreateManagedWidget  ( "scope_info_form",
                                                   xmFormWidgetClass,
                                                   _scope_info_frame, 
                                                   XmNresizePolicy, XmRESIZE_GROW, 
                                                   XmNwidth, 318, 
                                                   XmNheight, 38, 
                                                   (XtPointer) NULL ); 


    _scope_by_textfield = XtVaCreateManagedWidget  ( "scope_by_textfield",
                                                      xmTextFieldWidgetClass,
                                                      _scope_info_form, 
                                                      XmNeditable, False, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_FORM, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNbottomPosition, 0, 
                                                      XmNtopOffset, 5, 
                                                      XmNbottomOffset, 5, 
                                                      XmNleftOffset, 230, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 73, 
                                                      XmNheight, 28, 
                                                      (XtPointer) NULL ); 
    XtVaSetValues ( _scope_by_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _scope_by_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _bearing_label = XtVaCreateManagedWidget  ( "bearing_label",
                                                 xmLabelWidgetClass,
                                                 _scope_info_form, 
                                                 XmNlabelType, XmSTRING, 
                                                 XmNtopAttachment, XmATTACH_FORM, 
                                                 XmNbottomAttachment, XmATTACH_NONE, 
                                                 XmNleftAttachment, XmATTACH_FORM, 
                                                 XmNrightAttachment, XmATTACH_NONE, 
                                                 XmNtopOffset, 10, 
                                                 XmNbottomOffset, 0, 
                                                 XmNleftOffset, 170, 
                                                 XmNrightOffset, 0, 
                                                 XmNwidth, 58, 
                                                 XmNheight, 20, 
                                                 (XtPointer) NULL ); 


    _scope_range_textfield = XtVaCreateManagedWidget  ( "scope_range_textfield",
                                                         xmTextFieldWidgetClass,
                                                         _scope_info_form, 
                                                         XmNeditable, False, 
                                                         XmNtopAttachment, XmATTACH_FORM, 
                                                         XmNbottomAttachment, XmATTACH_FORM, 
                                                         XmNleftAttachment, XmATTACH_FORM, 
                                                         XmNrightAttachment, XmATTACH_NONE, 
                                                         XmNbottomPosition, 0, 
                                                         XmNtopOffset, 5, 
                                                         XmNbottomOffset, 5, 
                                                         XmNleftOffset, 60, 
                                                         XmNrightOffset, 0, 
                                                         XmNwidth, 96, 
                                                         XmNheight, 28, 
                                                         (XtPointer) NULL ); 
    XtVaSetValues ( _scope_range_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _scope_range_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _range_label = XtVaCreateManagedWidget  ( "range_label",
                                               xmLabelWidgetClass,
                                               _scope_info_form, 
                                               XmNlabelType, XmSTRING, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_NONE, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_NONE, 
                                               XmNtopOffset, 10, 
                                               XmNbottomOffset, 0, 
                                               XmNleftOffset, 10, 
                                               XmNrightOffset, 0, 
                                               XmNwidth, 49, 
                                               XmNheight, 20, 
                                               (XtPointer) NULL ); 


    _scope_frame = XtVaCreateManagedWidget  ( "scope_frame",
                                               xmFrameWidgetClass,
                                               _baseWidget, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_FORM, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_FORM, 
                                               XmNbottomPosition, 0, 
                                               XmNrightPosition, 0, 
                                               XmNtopOffset, 10, 
                                               XmNbottomOffset, 85, 
                                               XmNleftOffset, 850, 
                                               XmNrightOffset, 184, 
                                               XmNwidth, 230, 
                                               XmNheight, 184, 
                                               (XtPointer) NULL ); 


    _scope_form = XtVaCreateManagedWidget  ( "scope_form",
                                              xmFormWidgetClass,
                                              _scope_frame, 
                                              XmNresizePolicy, XmRESIZE_GROW, 
                                              XmNwidth, 224, 
                                              XmNheight, 178, 
                                              (XtPointer) NULL ); 


    _scope_right_arrow = XtVaCreateManagedWidget  ( "scope_right_arrow",
                                                     xmArrowButtonWidgetClass,
                                                     _scope_form, 
                                                     XmNarrowDirection, XmARROW_RIGHT, 
                                                     XmNtopAttachment, XmATTACH_FORM, 
                                                     XmNbottomAttachment, XmATTACH_NONE, 
                                                     XmNleftAttachment, XmATTACH_FORM, 
                                                     XmNrightAttachment, XmATTACH_NONE, 
                                                     XmNtopOffset, 70, 
                                                     XmNbottomOffset, 0, 
                                                     XmNleftOffset, 140, 
                                                     XmNrightOffset, 0, 
                                                     XmNwidth, 50, 
                                                     XmNheight, 50, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _scope_right_arrow,
                    XmNactivateCallback,
                    &OodUI::scope_view_rightCallback,
                    (XtPointer) this ); 


    _scope_down_arrow = XtVaCreateManagedWidget  ( "scope_down_arrow",
                                                    xmArrowButtonWidgetClass,
                                                    _scope_form, 
                                                    XmNarrowDirection, XmARROW_DOWN, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_NONE, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_NONE, 
                                                    XmNtopOffset, 120, 
                                                    XmNbottomOffset, 0, 
                                                    XmNleftOffset, 90, 
                                                    XmNrightOffset, 0, 
                                                    XmNwidth, 50, 
                                                    XmNheight, 50, 
                                                    (XtPointer) NULL ); 

    XtAddCallback ( _scope_down_arrow,
                    XmNactivateCallback,
                    &OodUI::scope_view_downCallback,
                    (XtPointer) this ); 


    _scope_left_arrow = XtVaCreateManagedWidget  ( "scope_left_arrow",
                                                    xmArrowButtonWidgetClass,
                                                    _scope_form, 
                                                    XmNarrowDirection, XmARROW_LEFT, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_NONE, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_NONE, 
                                                    XmNtopOffset, 70, 
                                                    XmNbottomOffset, 0, 
                                                    XmNleftOffset, 40, 
                                                    XmNrightOffset, 0, 
                                                    XmNwidth, 50, 
                                                    XmNheight, 50, 
                                                    (XtPointer) NULL ); 

    XtAddCallback ( _scope_left_arrow,
                    XmNactivateCallback,
                    &OodUI::scope_view_leftCallback,
                    (XtPointer) this ); 


    _scope_up_arrow = XtVaCreateManagedWidget  ( "scope_up_arrow",
                                                  xmArrowButtonWidgetClass,
                                                  _scope_form, 
                                                  XmNtopAttachment, XmATTACH_FORM, 
                                                  XmNbottomAttachment, XmATTACH_NONE, 
                                                  XmNleftAttachment, XmATTACH_FORM, 
                                                  XmNrightAttachment, XmATTACH_NONE, 
                                                  XmNtopOffset, 20, 
                                                  XmNbottomOffset, 0, 
                                                  XmNleftOffset, 90, 
                                                  XmNrightOffset, 0, 
                                                  XmNwidth, 50, 
                                                  XmNheight, 50, 
                                                  (XtPointer) NULL ); 

    XtAddCallback ( _scope_up_arrow,
                    XmNactivateCallback,
                    &OodUI::scope_view_upCallback,
                    (XtPointer) this ); 


    _scope_mark_button = XtVaCreateManagedWidget  ( "scope_mark_button",
                                                     xmPushButtonWidgetClass,
                                                     _scope_form, 
                                                     XmNlabelType, XmSTRING, 
                                                     XmNtopAttachment, XmATTACH_FORM, 
                                                     XmNbottomAttachment, XmATTACH_NONE, 
                                                     XmNleftAttachment, XmATTACH_FORM, 
                                                     XmNrightAttachment, XmATTACH_NONE, 
                                                     XmNtopOffset, 70, 
                                                     XmNbottomOffset, 0, 
                                                     XmNleftOffset, 90, 
                                                     XmNrightOffset, 0, 
                                                     XmNwidth, 50, 
                                                     XmNheight, 50, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _scope_mark_button,
                    XmNactivateCallback,
                    &OodUI::scope_markCallback,
                    (XtPointer) this ); 


    _mbt_frame = XtVaCreateManagedWidget  ( "mbt_frame",
                                             xmFrameWidgetClass,
                                             _baseWidget, 
                                             XmNtopAttachment, XmATTACH_FORM, 
                                             XmNbottomAttachment, XmATTACH_NONE, 
                                             XmNleftAttachment, XmATTACH_FORM, 
                                             XmNrightAttachment, XmATTACH_NONE, 
                                             XmNtopOffset, 5, 
                                             XmNbottomOffset, 0, 
                                             XmNleftOffset, 370, 
                                             XmNrightOffset, 0, 
                                             XmNwidth, 240, 
                                             XmNheight, 161, 
                                             (XtPointer) NULL ); 


    _mbt_form = XtVaCreateManagedWidget  ( "mbt_form",
                                            xmFormWidgetClass,
                                            _mbt_frame, 
                                            XmNresizePolicy, XmRESIZE_GROW, 
                                            XmNwidth, 234, 
                                            XmNheight, 155, 
                                            (XtPointer) NULL ); 


    _mbt_label = XtVaCreateManagedWidget  ( "mbt_label",
                                             xmLabelWidgetClass,
                                             _mbt_form, 
                                             XmNlabelType, XmSTRING, 
                                             XmNtopAttachment, XmATTACH_FORM, 
                                             XmNbottomAttachment, XmATTACH_NONE, 
                                             XmNleftAttachment, XmATTACH_FORM, 
                                             XmNrightAttachment, XmATTACH_NONE, 
                                             XmNtopOffset, 100, 
                                             XmNbottomOffset, 0, 
                                             XmNleftOffset, 90, 
                                             XmNrightOffset, 0, 
                                             XmNwidth, 114, 
                                             XmNheight, 25, 
                                             (XtPointer) NULL ); 


    _embt_label = XtVaCreateManagedWidget  ( "embt_label",
                                              xmLabelWidgetClass,
                                              _mbt_form, 
                                              XmNlabelType, XmSTRING, 
                                              XmNtopAttachment, XmATTACH_FORM, 
                                              XmNbottomAttachment, XmATTACH_NONE, 
                                              XmNleftAttachment, XmATTACH_FORM, 
                                              XmNrightAttachment, XmATTACH_FORM, 
                                              XmNrightPosition, 0, 
                                              XmNtopOffset, 30, 
                                              XmNbottomOffset, 0, 
                                              XmNleftOffset, 90, 
                                              XmNrightOffset, 28, 
                                              XmNwidth, 116, 
                                              XmNheight, 25, 
                                              (XtPointer) NULL ); 


    _radiobox3 = XtVaCreateManagedWidget  ( "radiobox3",
                                             xmRowColumnWidgetClass,
                                             _mbt_form, 
                                             XmNpacking, XmPACK_COLUMN, 
                                             XmNradioBehavior, True, 
                                             XmNradioAlwaysOne, True, 
                                             XmNtopAttachment, XmATTACH_FORM, 
                                             XmNbottomAttachment, XmATTACH_NONE, 
                                             XmNleftAttachment, XmATTACH_FORM, 
                                             XmNrightAttachment, XmATTACH_NONE, 
                                             XmNtopOffset, 10, 
                                             XmNbottomOffset, 0, 
                                             XmNleftOffset, 10, 
                                             XmNrightOffset, 0, 
                                             XmNwidth, 62, 
                                             XmNheight, 65, 
                                             (XtPointer) NULL ); 


    _embt_on_toggle = XtVaCreateManagedWidget  ( "embt_on_toggle",
                                                  xmToggleButtonWidgetClass,
                                                  _radiobox3, 
                                                  XmNlabelType, XmSTRING, 
                                                  (XtPointer) NULL ); 

    XtAddCallback ( _embt_on_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::embt_blow_onCallback,
                    (XtPointer) this ); 


    _embt_off_toggle = XtVaCreateManagedWidget  ( "embt_off_toggle",
                                                   xmToggleButtonWidgetClass,
                                                   _radiobox3, 
                                                   XmNlabelType, XmSTRING, 
                                                   XmNset, True, 
                                                   (XtPointer) NULL ); 

    XtAddCallback ( _embt_off_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::embt_blow_offCallback,
                    (XtPointer) this ); 


    _radiobox2 = XtVaCreateManagedWidget  ( "radiobox2",
                                             xmRowColumnWidgetClass,
                                             _mbt_form, 
                                             XmNpacking, XmPACK_COLUMN, 
                                             XmNradioBehavior, True, 
                                             XmNradioAlwaysOne, True, 
                                             XmNtopAttachment, XmATTACH_FORM, 
                                             XmNbottomAttachment, XmATTACH_NONE, 
                                             XmNleftAttachment, XmATTACH_FORM, 
                                             XmNrightAttachment, XmATTACH_NONE, 
                                             XmNtopOffset, 90, 
                                             XmNbottomOffset, 0, 
                                             XmNleftOffset, 10, 
                                             XmNrightOffset, 0, 
                                             XmNwidth, 74, 
                                             XmNheight, 65, 
                                             (XtPointer) NULL ); 


    _mbts_open_toggle = XtVaCreateManagedWidget  ( "mbts_open_toggle",
                                                    xmToggleButtonWidgetClass,
                                                    _radiobox2, 
                                                    XmNlabelType, XmSTRING, 
                                                    (XtPointer) NULL ); 

    XtAddCallback ( _mbts_open_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::mbt_vents_openCallback,
                    (XtPointer) this ); 


    _mbts_shut_toggle = XtVaCreateManagedWidget  ( "mbts_shut_toggle",
                                                    xmToggleButtonWidgetClass,
                                                    _radiobox2, 
                                                    XmNlabelType, XmSTRING, 
                                                    XmNset, True, 
                                                    (XtPointer) NULL ); 

    XtAddCallback ( _mbts_shut_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::mbt_vents_shutCallback,
                    (XtPointer) this ); 


    _separator = XtVaCreateManagedWidget  ( "separator",
                                             xmSeparatorWidgetClass,
                                             _mbt_form, 
                                             XmNtopAttachment, XmATTACH_FORM, 
                                             XmNbottomAttachment, XmATTACH_NONE, 
                                             XmNleftAttachment, XmATTACH_FORM, 
                                             XmNrightAttachment, XmATTACH_FORM, 
                                             XmNrightPosition, 0, 
                                             XmNtopOffset, 70, 
                                             XmNbottomOffset, 0, 
                                             XmNleftOffset, 0, 
                                             XmNrightOffset, 4, 
                                             XmNwidth, 230, 
                                             XmNheight, 20, 
                                             (XtPointer) NULL ); 


    _depth_ood_frame = XtVaCreateManagedWidget  ( "depth_ood_frame",
                                                   xmFrameWidgetClass,
                                                   _baseWidget, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_NONE, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_FORM, 
                                                   XmNrightPosition, 0, 
                                                   XmNtopOffset, 138, 
                                                   XmNbottomOffset, 0, 
                                                   XmNleftOffset, 0, 
                                                   XmNrightOffset, 1045, 
                                                   XmNwidth, 219, 
                                                   XmNheight, 122, 
                                                   (XtPointer) NULL ); 


    _depth_ood_form = XtVaCreateManagedWidget  ( "depth_ood_form",
                                                  xmFormWidgetClass,
                                                  _depth_ood_frame, 
                                                  XmNresizePolicy, XmRESIZE_GROW, 
                                                  XmNwidth, 213, 
                                                  XmNheight, 116, 
                                                  (XtPointer) NULL ); 


    _o_dpth_1s_textfield = XtVaCreateManagedWidget  ( "o_dpth_1s_textfield",
                                                       xmTextFieldWidgetClass,
                                                       _depth_ood_form, 
                                                       XmNeditable, False, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 40, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 180, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 
    XtVaSetValues ( _o_dpth_1s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _o_dpth_1s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _o_dpth_1s_dwn_arrow = XtVaCreateManagedWidget  ( "o_dpth_1s_dwn_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _depth_ood_form, 
                                                       XmNarrowDirection, XmARROW_DOWN, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 70, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 180, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _o_dpth_1s_dwn_arrow,
                    XmNactivateCallback,
                    &OodUI::o_dpth_1s_minusCallback,
                    (XtPointer) this ); 


    _o_dpth_1s_up_arrow = XtVaCreateManagedWidget  ( "o_dpth_1s_up_arrow",
                                                      xmArrowButtonWidgetClass,
                                                      _depth_ood_form, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 10, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 180, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 30, 
                                                      XmNheight, 30, 
                                                      (XtPointer) NULL ); 

    XtAddCallback ( _o_dpth_1s_up_arrow,
                    XmNactivateCallback,
                    &OodUI::o_dpth_1s_addCallback,
                    (XtPointer) this ); 


    _depth_ood_label = XtVaCreateManagedWidget  ( "depth_ood_label",
                                                   xmLabelWidgetClass,
                                                   _depth_ood_form, 
                                                   XmNlabelType, XmSTRING, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_NONE, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_NONE, 
                                                   XmNtopOffset, 40, 
                                                   XmNbottomOffset, 0, 
                                                   XmNleftOffset, 0, 
                                                   XmNrightOffset, 0, 
                                                   XmNwidth, 52, 
                                                   XmNheight, 20, 
                                                   (XtPointer) NULL ); 


    _o_dpth_10s_textfield = XtVaCreateManagedWidget  ( "o_dpth_10s_textfield",
                                                        xmTextFieldWidgetClass,
                                                        _depth_ood_form, 
                                                        XmNeditable, False, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_NONE, 
                                                        XmNtopOffset, 40, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 140, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 30, 
                                                        XmNheight, 30, 
                                                        (XtPointer) NULL ); 
    XtVaSetValues ( _o_dpth_10s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _o_dpth_10s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _o_dpth_100s_textfield = XtVaCreateManagedWidget  ( "o_dpth_100s_textfield",
                                                         xmTextFieldWidgetClass,
                                                         _depth_ood_form, 
                                                         XmNeditable, False, 
                                                         XmNtopAttachment, XmATTACH_FORM, 
                                                         XmNbottomAttachment, XmATTACH_NONE, 
                                                         XmNleftAttachment, XmATTACH_FORM, 
                                                         XmNrightAttachment, XmATTACH_NONE, 
                                                         XmNtopOffset, 40, 
                                                         XmNbottomOffset, 0, 
                                                         XmNleftOffset, 100, 
                                                         XmNrightOffset, 0, 
                                                         XmNwidth, 30, 
                                                         XmNheight, 30, 
                                                         (XtPointer) NULL ); 
    XtVaSetValues ( _o_dpth_100s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _o_dpth_100s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _o_dpth_1000s_textfield = XtVaCreateManagedWidget  ( "o_dpth_1000s_textfield",
                                                          xmTextFieldWidgetClass,
                                                          _depth_ood_form, 
                                                          XmNeditable, False, 
                                                          XmNtopAttachment, XmATTACH_FORM, 
                                                          XmNbottomAttachment, XmATTACH_NONE, 
                                                          XmNleftAttachment, XmATTACH_FORM, 
                                                          XmNrightAttachment, XmATTACH_NONE, 
                                                          XmNtopOffset, 40, 
                                                          XmNbottomOffset, 0, 
                                                          XmNleftOffset, 60, 
                                                          XmNrightOffset, 0, 
                                                          XmNwidth, 30, 
                                                          XmNheight, 30, 
                                                          (XtPointer) NULL ); 
    XtVaSetValues ( _o_dpth_1000s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _o_dpth_1000s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _o_dpth_10s_dwn_arrow = XtVaCreateManagedWidget  ( "o_dpth_10s_dwn_arrow",
                                                        xmArrowButtonWidgetClass,
                                                        _depth_ood_form, 
                                                        XmNarrowDirection, XmARROW_DOWN, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_NONE, 
                                                        XmNtopOffset, 70, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 140, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 30, 
                                                        XmNheight, 30, 
                                                        (XtPointer) NULL ); 

    XtAddCallback ( _o_dpth_10s_dwn_arrow,
                    XmNactivateCallback,
                    &OodUI::o_dpth_10s_minusCallback,
                    (XtPointer) this ); 


    _o_dpth_100s_dwn_arrow = XtVaCreateManagedWidget  ( "o_dpth_100s_dwn_arrow",
                                                         xmArrowButtonWidgetClass,
                                                         _depth_ood_form, 
                                                         XmNarrowDirection, XmARROW_DOWN, 
                                                         XmNtopAttachment, XmATTACH_FORM, 
                                                         XmNbottomAttachment, XmATTACH_NONE, 
                                                         XmNleftAttachment, XmATTACH_FORM, 
                                                         XmNrightAttachment, XmATTACH_NONE, 
                                                         XmNtopOffset, 70, 
                                                         XmNbottomOffset, 0, 
                                                         XmNleftOffset, 100, 
                                                         XmNrightOffset, 0, 
                                                         XmNwidth, 30, 
                                                         XmNheight, 30, 
                                                         (XtPointer) NULL ); 

    XtAddCallback ( _o_dpth_100s_dwn_arrow,
                    XmNactivateCallback,
                    &OodUI::o_dpth_100s_minusCallback,
                    (XtPointer) this ); 


    _o_dpth_1000s_dwn_arrow = XtVaCreateManagedWidget  ( "o_dpth_1000s_dwn_arrow",
                                                          xmArrowButtonWidgetClass,
                                                          _depth_ood_form, 
                                                          XmNarrowDirection, XmARROW_DOWN, 
                                                          XmNtopAttachment, XmATTACH_FORM, 
                                                          XmNbottomAttachment, XmATTACH_NONE, 
                                                          XmNleftAttachment, XmATTACH_FORM, 
                                                          XmNrightAttachment, XmATTACH_NONE, 
                                                          XmNtopOffset, 70, 
                                                          XmNbottomOffset, 0, 
                                                          XmNleftOffset, 60, 
                                                          XmNrightOffset, 0, 
                                                          XmNwidth, 30, 
                                                          XmNheight, 30, 
                                                          (XtPointer) NULL ); 

    XtAddCallback ( _o_dpth_1000s_dwn_arrow,
                    XmNactivateCallback,
                    &OodUI::o_dpth_1000s_minusCallback,
                    (XtPointer) this ); 


    _o_dpth_10s_up_arrow = XtVaCreateManagedWidget  ( "o_dpth_10s_up_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _depth_ood_form, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 10, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 140, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _o_dpth_10s_up_arrow,
                    XmNactivateCallback,
                    &OodUI::o_dpth_10s_addCallback,
                    (XtPointer) this ); 


    _o_dpth_100s_up_arrow = XtVaCreateManagedWidget  ( "o_dpth_100s_up_arrow",
                                                        xmArrowButtonWidgetClass,
                                                        _depth_ood_form, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_NONE, 
                                                        XmNtopOffset, 10, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 100, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 30, 
                                                        XmNheight, 30, 
                                                        (XtPointer) NULL ); 

    XtAddCallback ( _o_dpth_100s_up_arrow,
                    XmNactivateCallback,
                    &OodUI::o_dpth_100s_addCallback,
                    (XtPointer) this ); 


    _o_dpth_1000s_up_arrow = XtVaCreateManagedWidget  ( "o_dpth_1000s_up_arrow",
                                                         xmArrowButtonWidgetClass,
                                                         _depth_ood_form, 
                                                         XmNtopAttachment, XmATTACH_FORM, 
                                                         XmNbottomAttachment, XmATTACH_NONE, 
                                                         XmNleftAttachment, XmATTACH_FORM, 
                                                         XmNrightAttachment, XmATTACH_NONE, 
                                                         XmNtopOffset, 10, 
                                                         XmNbottomOffset, 0, 
                                                         XmNleftOffset, 60, 
                                                         XmNrightOffset, 0, 
                                                         XmNwidth, 30, 
                                                         XmNheight, 30, 
                                                         (XtPointer) NULL ); 

    XtAddCallback ( _o_dpth_1000s_up_arrow,
                    XmNactivateCallback,
                    &OodUI::o_dpth_1000s_addCallback,
                    (XtPointer) this ); 


    _course_ood_frame = XtVaCreateManagedWidget  ( "course_ood_frame",
                                                    xmFrameWidgetClass,
                                                    _baseWidget, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_NONE, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_FORM, 
                                                    XmNrightPosition, 0, 
                                                    XmNtopOffset, 10, 
                                                    XmNbottomOffset, 0, 
                                                    XmNleftOffset, 20, 
                                                    XmNrightOffset, 1045, 
                                                    XmNwidth, 199, 
                                                    XmNheight, 126, 
                                                    (XtPointer) NULL ); 


    _course_ood_form = XtVaCreateManagedWidget  ( "course_ood_form",
                                                   xmFormWidgetClass,
                                                   _course_ood_frame, 
                                                   XmNresizePolicy, XmRESIZE_GROW, 
                                                   XmNwidth, 193, 
                                                   XmNheight, 120, 
                                                   (XtPointer) NULL ); 


    _course_ood_label = XtVaCreateManagedWidget  ( "course_ood_label",
                                                    xmLabelWidgetClass,
                                                    _course_ood_form, 
                                                    XmNlabelType, XmSTRING, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_NONE, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_NONE, 
                                                    XmNtopOffset, 40, 
                                                    XmNbottomOffset, 0, 
                                                    XmNleftOffset, 0, 
                                                    XmNrightOffset, 0, 
                                                    XmNwidth, 68, 
                                                    XmNheight, 20, 
                                                    (XtPointer) NULL ); 


    _o_course_1s_textfield = XtVaCreateManagedWidget  ( "o_course_1s_textfield",
                                                         xmTextFieldWidgetClass,
                                                         _course_ood_form, 
                                                         XmNeditable, False, 
                                                         XmNtopAttachment, XmATTACH_FORM, 
                                                         XmNbottomAttachment, XmATTACH_NONE, 
                                                         XmNleftAttachment, XmATTACH_FORM, 
                                                         XmNrightAttachment, XmATTACH_NONE, 
                                                         XmNtopOffset, 40, 
                                                         XmNbottomOffset, 0, 
                                                         XmNleftOffset, 160, 
                                                         XmNrightOffset, 0, 
                                                         XmNwidth, 30, 
                                                         XmNheight, 30, 
                                                         (XtPointer) NULL ); 
    XtVaSetValues ( _o_course_1s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _o_course_1s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _o_course_10s_textfield = XtVaCreateManagedWidget  ( "o_course_10s_textfield",
                                                          xmTextFieldWidgetClass,
                                                          _course_ood_form, 
                                                          XmNeditable, False, 
                                                          XmNtopAttachment, XmATTACH_FORM, 
                                                          XmNbottomAttachment, XmATTACH_NONE, 
                                                          XmNleftAttachment, XmATTACH_FORM, 
                                                          XmNrightAttachment, XmATTACH_NONE, 
                                                          XmNtopOffset, 40, 
                                                          XmNbottomOffset, 0, 
                                                          XmNleftOffset, 120, 
                                                          XmNrightOffset, 0, 
                                                          XmNwidth, 30, 
                                                          XmNheight, 30, 
                                                          (XtPointer) NULL ); 
    XtVaSetValues ( _o_course_10s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _o_course_10s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _o_course_100s_textfield = XtVaCreateManagedWidget  ( "o_course_100s_textfield",
                                                           xmTextFieldWidgetClass,
                                                           _course_ood_form, 
                                                           XmNeditable, False, 
                                                           XmNtopAttachment, XmATTACH_FORM, 
                                                           XmNbottomAttachment, XmATTACH_NONE, 
                                                           XmNleftAttachment, XmATTACH_FORM, 
                                                           XmNrightAttachment, XmATTACH_NONE, 
                                                           XmNtopOffset, 40, 
                                                           XmNbottomOffset, 0, 
                                                           XmNleftOffset, 80, 
                                                           XmNrightOffset, 0, 
                                                           XmNwidth, 30, 
                                                           XmNheight, 30, 
                                                           (XtPointer) NULL ); 
    XtVaSetValues ( _o_course_100s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _o_course_100s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _o_crse_1s_dwn_arrow = XtVaCreateManagedWidget  ( "o_crse_1s_dwn_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _course_ood_form, 
                                                       XmNarrowDirection, XmARROW_DOWN, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 70, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 160, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _o_crse_1s_dwn_arrow,
                    XmNactivateCallback,
                    &OodUI::o_crse_1s_minusCallback,
                    (XtPointer) this ); 


    _o_crse_10s_dwn_arrow = XtVaCreateManagedWidget  ( "o_crse_10s_dwn_arrow",
                                                        xmArrowButtonWidgetClass,
                                                        _course_ood_form, 
                                                        XmNarrowDirection, XmARROW_DOWN, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_NONE, 
                                                        XmNtopOffset, 70, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 120, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 30, 
                                                        XmNheight, 30, 
                                                        (XtPointer) NULL ); 

    XtAddCallback ( _o_crse_10s_dwn_arrow,
                    XmNactivateCallback,
                    &OodUI::o_crse_10s_minusCallback,
                    (XtPointer) this ); 


    _o_crse_100s_dwn_arrow = XtVaCreateManagedWidget  ( "o_crse_100s_dwn_arrow",
                                                         xmArrowButtonWidgetClass,
                                                         _course_ood_form, 
                                                         XmNarrowDirection, XmARROW_DOWN, 
                                                         XmNtopAttachment, XmATTACH_FORM, 
                                                         XmNbottomAttachment, XmATTACH_NONE, 
                                                         XmNleftAttachment, XmATTACH_FORM, 
                                                         XmNrightAttachment, XmATTACH_NONE, 
                                                         XmNtopOffset, 70, 
                                                         XmNbottomOffset, 0, 
                                                         XmNleftOffset, 80, 
                                                         XmNrightOffset, 0, 
                                                         XmNwidth, 30, 
                                                         XmNheight, 30, 
                                                         (XtPointer) NULL ); 

    XtAddCallback ( _o_crse_100s_dwn_arrow,
                    XmNactivateCallback,
                    &OodUI::o_crse_100s_minusCallback,
                    (XtPointer) this ); 


    _o_crse_1s_up_arrow = XtVaCreateManagedWidget  ( "o_crse_1s_up_arrow",
                                                      xmArrowButtonWidgetClass,
                                                      _course_ood_form, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 10, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 160, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 30, 
                                                      XmNheight, 30, 
                                                      (XtPointer) NULL ); 

    XtAddCallback ( _o_crse_1s_up_arrow,
                    XmNactivateCallback,
                    &OodUI::o_crse_1s_addCallback,
                    (XtPointer) this ); 


    _o_crse_10s_up_arrow = XtVaCreateManagedWidget  ( "o_crse_10s_up_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _course_ood_form, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 10, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 120, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _o_crse_10s_up_arrow,
                    XmNactivateCallback,
                    &OodUI::o_crse_10s_addCallback,
                    (XtPointer) this ); 


    _o_crs_100s_up_arrow = XtVaCreateManagedWidget  ( "o_crs_100s_up_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _course_ood_form, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 10, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 80, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _o_crs_100s_up_arrow,
                    XmNactivateCallback,
                    &OodUI::o_crs_100s_addCallback,
                    (XtPointer) this ); 


    _ood_order_label = XtVaCreateManagedWidget  ( "ood_order_label",
                                                   xmLabelWidgetClass,
                                                   _baseWidget, 
                                                   XmNlabelType, XmSTRING, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_NONE, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_NONE, 
                                                   XmNtopOffset, 260, 
                                                   XmNbottomOffset, 0, 
                                                   XmNleftOffset, 160, 
                                                   XmNrightOffset, 0, 
                                                   XmNwidth, 101, 
                                                   XmNheight, 25, 
                                                   (XtPointer) NULL ); 


    _speed_ood_frame = XtVaCreateManagedWidget  ( "speed_ood_frame",
                                                   xmFrameWidgetClass,
                                                   _baseWidget, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_FORM, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_NONE, 
                                                   XmNbottomPosition, 0, 
                                                   XmNtopOffset, 0, 
                                                   XmNbottomOffset, 25, 
                                                   XmNleftOffset, 220, 
                                                   XmNrightOffset, 0, 
                                                   XmNwidth, 126, 
                                                   XmNheight, 254, 
                                                   (XtPointer) NULL ); 


    _rowcolumn1 = XtVaCreateManagedWidget  ( "rowcolumn1",
                                              xmRowColumnWidgetClass,
                                              _speed_ood_frame, 
                                              XmNwidth, 120, 
                                              XmNheight, 248, 
                                              (XtPointer) NULL ); 


    _radiobox1 = XtVaCreateManagedWidget  ( "radiobox1",
                                             xmRowColumnWidgetClass,
                                             _rowcolumn1, 
                                             XmNpacking, XmPACK_COLUMN, 
                                             XmNradioBehavior, True, 
                                             XmNradioAlwaysOne, True, 
                                             XmNwidth, 114, 
                                             XmNheight, 251, 
                                             (XtPointer) NULL ); 


    _o_flank_toggle = XtVaCreateManagedWidget  ( "o_flank_toggle",
                                                  xmToggleButtonWidgetClass,
                                                  _radiobox1, 
                                                  XmNlabelType, XmSTRING, 
                                                  (XtPointer) NULL ); 

    XtAddCallback ( _o_flank_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::flank_orderCallback,
                    (XtPointer) this ); 


    _o_full_toggle = XtVaCreateManagedWidget  ( "o_full_toggle",
                                                 xmToggleButtonWidgetClass,
                                                 _radiobox1, 
                                                 XmNlabelType, XmSTRING, 
                                                 (XtPointer) NULL ); 

    XtAddCallback ( _o_full_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::full_orderCallback,
                    (XtPointer) this ); 


    _o_stnd_toggle = XtVaCreateManagedWidget  ( "o_stnd_toggle",
                                                 xmToggleButtonWidgetClass,
                                                 _radiobox1, 
                                                 XmNlabelType, XmSTRING, 
                                                 (XtPointer) NULL ); 

    XtAddCallback ( _o_stnd_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::stnd_orderCallback,
                    (XtPointer) this ); 


    _o_half_toggle = XtVaCreateManagedWidget  ( "o_half_toggle",
                                                 xmToggleButtonWidgetClass,
                                                 _radiobox1, 
                                                 XmNlabelType, XmSTRING, 
                                                 (XtPointer) NULL ); 

    XtAddCallback ( _o_half_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::half_orderCallback,
                    (XtPointer) this ); 


    _o_stop_toggle = XtVaCreateManagedWidget  ( "o_stop_toggle",
                                                 xmToggleButtonWidgetClass,
                                                 _radiobox1, 
                                                 XmNlabelType, XmSTRING, 
                                                 XmNset, True, 
                                                 (XtPointer) NULL ); 

    XtAddCallback ( _o_stop_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::stop_orderCallback,
                    (XtPointer) this ); 


    _o_backhalf_toggle = XtVaCreateManagedWidget  ( "o_backhalf_toggle",
                                                     xmToggleButtonWidgetClass,
                                                     _radiobox1, 
                                                     XmNlabelType, XmSTRING, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _o_backhalf_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::back_half_orderCallback,
                    (XtPointer) this ); 


    _o_backstnd_toggle = XtVaCreateManagedWidget  ( "o_backstnd_toggle",
                                                     xmToggleButtonWidgetClass,
                                                     _radiobox1, 
                                                     XmNlabelType, XmSTRING, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _o_backstnd_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::back_stnd_orderCallback,
                    (XtPointer) this ); 


    _o_backfull_toggle = XtVaCreateManagedWidget  ( "o_backfull_toggle",
                                                     xmToggleButtonWidgetClass,
                                                     _radiobox1, 
                                                     XmNlabelType, XmSTRING, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _o_backfull_toggle,
                    XmNvalueChangedCallback,
                    &OodUI::back_full_orderCallback,
                    (XtPointer) this ); 


    _actual_OOD_frame = XtVaCreateManagedWidget  ( "actual_OOD_frame",
                                                    xmFrameWidgetClass,
                                                    _baseWidget, 
                                                    XmNshadowType, XmSHADOW_OUT, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_NONE, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_NONE, 
                                                    XmNtopOffset, 20, 
                                                    XmNbottomOffset, 0, 
                                                    XmNleftOffset, 700, 
                                                    XmNrightOffset, 0, 
                                                    XmNwidth, 120, 
                                                    XmNheight, 260, 
                                                    (XtPointer) NULL ); 


    _actual_OOD_form = XtVaCreateManagedWidget  ( "actual_OOD_form",
                                                   xmFormWidgetClass,
                                                   _actual_OOD_frame, 
                                                   XmNresizePolicy, XmRESIZE_GROW, 
                                                   XmNwidth, 114, 
                                                   XmNheight, 254, 
                                                   (XtPointer) NULL ); 


    _actual_O_speed_textfield = XtVaCreateManagedWidget  ( "actual_O_speed_textfield",
                                                            xmTextFieldWidgetClass,
                                                            _actual_OOD_form, 
                                                            XmNeditable, False, 
                                                            XmNtopAttachment, XmATTACH_FORM, 
                                                            XmNbottomAttachment, XmATTACH_NONE, 
                                                            XmNleftAttachment, XmATTACH_FORM, 
                                                            XmNrightAttachment, XmATTACH_FORM, 
                                                            XmNrightPosition, 0, 
                                                            XmNtopOffset, 150, 
                                                            XmNbottomOffset, 0, 
                                                            XmNleftOffset, 20, 
                                                            XmNrightOffset, 18, 
                                                            XmNwidth, 76, 
                                                            XmNheight, 29, 
                                                            (XtPointer) NULL ); 
    XtVaSetValues ( _actual_O_speed_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _actual_O_speed_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _actual_O_depth_textfield = XtVaCreateManagedWidget  ( "actual_O_depth_textfield",
                                                            xmTextFieldWidgetClass,
                                                            _actual_OOD_form, 
                                                            XmNeditable, False, 
                                                            XmNtopAttachment, XmATTACH_FORM, 
                                                            XmNbottomAttachment, XmATTACH_NONE, 
                                                            XmNleftAttachment, XmATTACH_FORM, 
                                                            XmNrightAttachment, XmATTACH_FORM, 
                                                            XmNrightPosition, 0, 
                                                            XmNtopOffset, 90, 
                                                            XmNbottomOffset, 0, 
                                                            XmNleftOffset, 20, 
                                                            XmNrightOffset, 21, 
                                                            XmNwidth, 73, 
                                                            XmNheight, 29, 
                                                            (XtPointer) NULL ); 
    XtVaSetValues ( _actual_O_depth_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _actual_O_depth_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _actual_O_course_textfield = XtVaCreateManagedWidget  ( "actual_O_course_textfield",
                                                             xmTextFieldWidgetClass,
                                                             _actual_OOD_form, 
                                                             XmNeditable, False, 
                                                             XmNtopAttachment, XmATTACH_FORM, 
                                                             XmNbottomAttachment, XmATTACH_NONE, 
                                                             XmNleftAttachment, XmATTACH_FORM, 
                                                             XmNrightAttachment, XmATTACH_FORM, 
                                                             XmNrightPosition, 0, 
                                                             XmNtopOffset, 30, 
                                                             XmNbottomOffset, 0, 
                                                             XmNleftOffset, 20, 
                                                             XmNrightOffset, 21, 
                                                             XmNwidth, 73, 
                                                             XmNheight, 29, 
                                                             (XtPointer) NULL ); 
    XtVaSetValues ( _actual_O_course_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _actual_O_course_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _actual_OOD_label = XtVaCreateManagedWidget  ( "actual_OOD_label",
                                                    xmLabelWidgetClass,
                                                    _actual_OOD_form, 
                                                    XmNlabelType, XmSTRING, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_NONE, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_FORM, 
                                                    XmNrightPosition, 0, 
                                                    XmNtopOffset, 220, 
                                                    XmNbottomOffset, 0, 
                                                    XmNleftOffset, 0, 
                                                    XmNrightOffset, 0, 
                                                    XmNwidth, 114, 
                                                    XmNheight, 25, 
                                                    (XtPointer) NULL ); 


    _actual_OOD_separator = XtVaCreateManagedWidget  ( "actual_OOD_separator",
                                                        xmSeparatorWidgetClass,
                                                        _actual_OOD_form, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_FORM, 
                                                        XmNrightPosition, 0, 
                                                        XmNtopOffset, 180, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 0, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 114, 
                                                        XmNheight, 20, 
                                                        (XtPointer) NULL ); 


    _actual_O_speed_label = XtVaCreateManagedWidget  ( "actual_O_speed_label",
                                                        xmLabelWidgetClass,
                                                        _actual_OOD_form, 
                                                        XmNlabelType, XmSTRING, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_FORM, 
                                                        XmNrightPosition, 0, 
                                                        XmNtopOffset, 130, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 0, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 114, 
                                                        XmNheight, 20, 
                                                        (XtPointer) NULL ); 


    _actual_O_depth_label = XtVaCreateManagedWidget  ( "actual_O_depth_label",
                                                        xmLabelWidgetClass,
                                                        _actual_OOD_form, 
                                                        XmNlabelType, XmSTRING, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_FORM, 
                                                        XmNrightPosition, 0, 
                                                        XmNtopOffset, 70, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 0, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 114, 
                                                        XmNheight, 20, 
                                                        (XtPointer) NULL ); 


    _actual_O_course_label = XtVaCreateManagedWidget  ( "actual_O_course_label",
                                                         xmLabelWidgetClass,
                                                         _actual_OOD_form, 
                                                         XmNlabelType, XmSTRING, 
                                                         XmNtopAttachment, XmATTACH_FORM, 
                                                         XmNbottomAttachment, XmATTACH_NONE, 
                                                         XmNleftAttachment, XmATTACH_FORM, 
                                                         XmNrightAttachment, XmATTACH_FORM, 
                                                         XmNrightPosition, 0, 
                                                         XmNtopOffset, 10, 
                                                         XmNbottomOffset, 0, 
                                                         XmNleftOffset, 0, 
                                                         XmNrightOffset, 0, 
                                                         XmNwidth, 114, 
                                                         XmNheight, 20, 
                                                         (XtPointer) NULL ); 


}

const char * OodUI::className()
{
    return ("OodUI");
} // End className()




/////////////////////////////////////////////////////////////// 
// The following functions are static member functions used to 
// interface with Motif.
/////////////////////////////////// 

void OodUI::back_full_orderCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->back_full_order ( w, callData );
}

void OodUI::back_half_orderCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->back_half_order ( w, callData );
}

void OodUI::back_stnd_orderCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->back_stnd_order ( w, callData );
}

void OodUI::embt_blow_offCallback ( Widget    w,
                                    XtPointer clientData,
                                    XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->embt_blow_off ( w, callData );
}

void OodUI::embt_blow_onCallback ( Widget    w,
                                   XtPointer clientData,
                                   XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->embt_blow_on ( w, callData );
}

void OodUI::flank_orderCallback ( Widget    w,
                                  XtPointer clientData,
                                  XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->flank_order ( w, callData );
}

void OodUI::full_orderCallback ( Widget    w,
                                 XtPointer clientData,
                                 XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->full_order ( w, callData );
}

void OodUI::half_orderCallback ( Widget    w,
                                 XtPointer clientData,
                                 XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->half_order ( w, callData );
}

void OodUI::mbt_vents_openCallback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->mbt_vents_open ( w, callData );
}

void OodUI::mbt_vents_shutCallback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->mbt_vents_shut ( w, callData );
}

void OodUI::o_crs_100s_addCallback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_crs_100s_add ( w, callData );
}

void OodUI::o_crse_100s_minusCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_crse_100s_minus ( w, callData );
}

void OodUI::o_crse_10s_addCallback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_crse_10s_add ( w, callData );
}

void OodUI::o_crse_10s_minusCallback ( Widget    w,
                                       XtPointer clientData,
                                       XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_crse_10s_minus ( w, callData );
}

void OodUI::o_crse_1s_addCallback ( Widget    w,
                                    XtPointer clientData,
                                    XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_crse_1s_add ( w, callData );
}

void OodUI::o_crse_1s_minusCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_crse_1s_minus ( w, callData );
}

void OodUI::o_dpth_1000s_addCallback ( Widget    w,
                                       XtPointer clientData,
                                       XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_dpth_1000s_add ( w, callData );
}

void OodUI::o_dpth_1000s_minusCallback ( Widget    w,
                                         XtPointer clientData,
                                         XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_dpth_1000s_minus ( w, callData );
}

void OodUI::o_dpth_100s_addCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_dpth_100s_add ( w, callData );
}

void OodUI::o_dpth_100s_minusCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_dpth_100s_minus ( w, callData );
}

void OodUI::o_dpth_10s_addCallback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_dpth_10s_add ( w, callData );
}

void OodUI::o_dpth_10s_minusCallback ( Widget    w,
                                       XtPointer clientData,
                                       XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_dpth_10s_minus ( w, callData );
}

void OodUI::o_dpth_1s_addCallback ( Widget    w,
                                    XtPointer clientData,
                                    XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_dpth_1s_add ( w, callData );
}

void OodUI::o_dpth_1s_minusCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->o_dpth_1s_minus ( w, callData );
}

void OodUI::scope_lowerCallback ( Widget    w,
                                  XtPointer clientData,
                                  XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->scope_lower ( w, callData );
}

void OodUI::scope_markCallback ( Widget    w,
                                 XtPointer clientData,
                                 XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->scope_mark ( w, callData );
}

void OodUI::scope_raiseCallback ( Widget    w,
                                  XtPointer clientData,
                                  XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->scope_raise ( w, callData );
}

void OodUI::scope_view_downCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->scope_view_down ( w, callData );
}

void OodUI::scope_view_leftCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->scope_view_left ( w, callData );
}

void OodUI::scope_view_rightCallback ( Widget    w,
                                       XtPointer clientData,
                                       XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->scope_view_right ( w, callData );
}

void OodUI::scope_view_upCallback ( Widget    w,
                                    XtPointer clientData,
                                    XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->scope_view_up ( w, callData );
}

void OodUI::stnd_orderCallback ( Widget    w,
                                 XtPointer clientData,
                                 XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->stnd_order ( w, callData );
}

void OodUI::stop_orderCallback ( Widget    w,
                                 XtPointer clientData,
                                 XtPointer callData ) 
{ 
    OodUI* obj = ( OodUI * ) clientData;
    obj->stop_order ( w, callData );
}



/////////////////////////////////////////////////////////////// 
// The following functions are called from the menu items 
// in this window.
/////////////////////////////////// 

void OodUI::back_full_order ( Widget, XtPointer ) 
{
    // This virtual function is called from back_full_orderCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::back_half_order ( Widget, XtPointer ) 
{
    // This virtual function is called from back_half_orderCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::back_stnd_order ( Widget, XtPointer ) 
{
    // This virtual function is called from back_stnd_orderCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::embt_blow_off ( Widget, XtPointer ) 
{
    // This virtual function is called from embt_blow_offCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::embt_blow_on ( Widget, XtPointer ) 
{
    // This virtual function is called from embt_blow_onCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::flank_order ( Widget, XtPointer ) 
{
    // This virtual function is called from flank_orderCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::full_order ( Widget, XtPointer ) 
{
    // This virtual function is called from full_orderCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::half_order ( Widget, XtPointer ) 
{
    // This virtual function is called from half_orderCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::mbt_vents_open ( Widget, XtPointer ) 
{
    // This virtual function is called from mbt_vents_openCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::mbt_vents_shut ( Widget, XtPointer ) 
{
    // This virtual function is called from mbt_vents_shutCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_crs_100s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from o_crs_100s_addCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_crse_100s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from o_crse_100s_minusCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_crse_10s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from o_crse_10s_addCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_crse_10s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from o_crse_10s_minusCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_crse_1s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from o_crse_1s_addCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_crse_1s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from o_crse_1s_minusCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_dpth_1000s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from o_dpth_1000s_addCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_dpth_1000s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from o_dpth_1000s_minusCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_dpth_100s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from o_dpth_100s_addCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_dpth_100s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from o_dpth_100s_minusCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_dpth_10s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from o_dpth_10s_addCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_dpth_10s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from o_dpth_10s_minusCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_dpth_1s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from o_dpth_1s_addCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::o_dpth_1s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from o_dpth_1s_minusCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::scope_lower ( Widget, XtPointer ) 
{
    // This virtual function is called from scope_lowerCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::scope_mark ( Widget, XtPointer ) 
{
    // This virtual function is called from scope_markCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::scope_raise ( Widget, XtPointer ) 
{
    // This virtual function is called from scope_raiseCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::scope_view_down ( Widget, XtPointer ) 
{
    // This virtual function is called from scope_view_downCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::scope_view_left ( Widget, XtPointer ) 
{
    // This virtual function is called from scope_view_leftCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::scope_view_right ( Widget, XtPointer ) 
{
    // This virtual function is called from scope_view_rightCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::scope_view_up ( Widget, XtPointer ) 
{
    // This virtual function is called from scope_view_upCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::stnd_order ( Widget, XtPointer ) 
{
    // This virtual function is called from stnd_orderCallback.
    // This function is normally overriden by a derived class.

}

void OodUI::stop_order ( Widget, XtPointer ) 
{
    // This virtual function is called from stop_orderCallback.
    // This function is normally overriden by a derived class.

}

//---- End generated code section

