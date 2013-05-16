
/////////////////////////////////////////////////////////////
//
// Source file for HelmUI
//
//    This class implements the user interface created in 
//    the interface builder. Normally it is not used directly.
//    Instead the subclass, Helm is instantiated
//
//    To extend or alter the behavior of this class, you should
//    modify the Helm files
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


#include "HelmUI.h" // Generated header file for this class
#include <Sgm/Dial.h> 
#include <Xm/Form.h> 
#include <Xm/Frame.h> 
#include <Xm/Label.h> 
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

String  HelmUI::_defaultHelmUIResources[] = {
        (char*)NULL
};

HelmUI::HelmUI(const char *name) : VkComponent(name) 
{ 
    // No widgets are created by this constructor.
    // If an application creates a component using this constructor,
    // It must explictly call create at a later time.
    // This is mostly useful when adding pre-widget creation
    // code to a derived class constructor.

    //---- End generated code section

} // End Constructor




HelmUI::HelmUI(const char *name, Widget parent) : VkComponent(name) 
{ 
    // Call creation function to build the widget tree.

     create(parent);

    //---- End generated code section

} // End Constructor


HelmUI::~HelmUI() 
{
    // Base class destroys widgets

    //-- End generated code section

} // End destructor



void HelmUI::create ( Widget parent )
{
    Arg      args[16];
    Cardinal count;
    count = 0;

    // Load any class-defaulted resources for this object

    setDefaultResources(parent, _defaultHelmUIResources  );


    // Create an unmanaged widget as the top of the widget hierarchy

    _baseWidget = _helm = XtVaCreateWidget ( _name,
                                             xmFormWidgetClass,
                                             parent, 
                                             XmNresizePolicy, XmRESIZE_GROW, 
                                             (XtPointer) NULL ); 

    // install a callback to guard against unexpected widget destruction

    installDestroyHandler();


    // Create widgets used in this component
    // All variables are data members of this class

    _actual_helm_frame = XtVaCreateManagedWidget  ( "actual_helm_frame",
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


    _actual_helm_form = XtVaCreateManagedWidget  ( "actual_helm_form",
                                                    xmFormWidgetClass,
                                                    _actual_helm_frame, 
                                                    XmNresizePolicy, XmRESIZE_GROW, 
                                                    XmNwidth, 114, 
                                                    XmNheight, 254, 
                                                    (XtPointer) NULL ); 


    _actual_h_speed_textfield = XtVaCreateManagedWidget  ( "actual_h_speed_textfield",
                                                            xmTextFieldWidgetClass,
                                                            _actual_helm_form, 
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
    XtVaSetValues ( _actual_h_speed_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _actual_h_speed_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _actual_h_depth_textfield = XtVaCreateManagedWidget  ( "actual_h_depth_textfield",
                                                            xmTextFieldWidgetClass,
                                                            _actual_helm_form, 
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
    XtVaSetValues ( _actual_h_depth_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _actual_h_depth_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _actual_h_course_textfield = XtVaCreateManagedWidget  ( "actual_h_course_textfield",
                                                             xmTextFieldWidgetClass,
                                                             _actual_helm_form, 
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
    XtVaSetValues ( _actual_h_course_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _actual_h_course_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _actual_helm_label = XtVaCreateManagedWidget  ( "actual_helm_label",
                                                     xmLabelWidgetClass,
                                                     _actual_helm_form, 
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


    _actual_helm_separator = XtVaCreateManagedWidget  ( "actual_helm_separator",
                                                         xmSeparatorWidgetClass,
                                                         _actual_helm_form, 
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


    _actual_h_speed_label = XtVaCreateManagedWidget  ( "actual_h_speed_label",
                                                        xmLabelWidgetClass,
                                                        _actual_helm_form, 
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


    _actual_h_depth_label = XtVaCreateManagedWidget  ( "actual_h_depth_label",
                                                        xmLabelWidgetClass,
                                                        _actual_helm_form, 
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


    _actual_h_course_label = XtVaCreateManagedWidget  ( "actual_h_course_label",
                                                         xmLabelWidgetClass,
                                                         _actual_helm_form, 
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


    _ordered_helm_frame = XtVaCreateManagedWidget  ( "ordered_helm_frame",
                                                      xmFrameWidgetClass,
                                                      _baseWidget, 
                                                      XmNshadowType, XmSHADOW_OUT, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 10, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 10, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 120, 
                                                      XmNheight, 260, 
                                                      (XtPointer) NULL ); 


    _ordered_helm_form = XtVaCreateManagedWidget  ( "ordered_helm_form",
                                                     xmFormWidgetClass,
                                                     _ordered_helm_frame, 
                                                     XmNresizePolicy, XmRESIZE_GROW, 
                                                     XmNwidth, 114, 
                                                     XmNheight, 254, 
                                                     (XtPointer) NULL ); 


    _order_h_speed_textfield = XtVaCreateManagedWidget  ( "order_h_speed_textfield",
                                                           xmTextFieldWidgetClass,
                                                           _ordered_helm_form, 
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
    XtVaSetValues ( _order_h_speed_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _order_h_speed_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _order_h_depth_textfield = XtVaCreateManagedWidget  ( "order_h_depth_textfield",
                                                           xmTextFieldWidgetClass,
                                                           _ordered_helm_form, 
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
    XtVaSetValues ( _order_h_depth_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _order_h_depth_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _order_h_course_textfield = XtVaCreateManagedWidget  ( "order_h_course_textfield",
                                                            xmTextFieldWidgetClass,
                                                            _ordered_helm_form, 
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
    XtVaSetValues ( _order_h_course_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _order_h_course_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _order_helm_label = XtVaCreateManagedWidget  ( "order_helm_label",
                                                    xmLabelWidgetClass,
                                                    _ordered_helm_form, 
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


    _order_helm_separator = XtVaCreateManagedWidget  ( "order_helm_separator",
                                                        xmSeparatorWidgetClass,
                                                        _ordered_helm_form, 
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


    _order_h_speed_label = XtVaCreateManagedWidget  ( "order_h_speed_label",
                                                       xmLabelWidgetClass,
                                                       _ordered_helm_form, 
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


    _order_h_depth_label = XtVaCreateManagedWidget  ( "order_h_depth_label",
                                                       xmLabelWidgetClass,
                                                       _ordered_helm_form, 
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


    _order_h_course_label = XtVaCreateManagedWidget  ( "order_h_course_label",
                                                        xmLabelWidgetClass,
                                                        _ordered_helm_form, 
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


    _telegraph_frame = XtVaCreateManagedWidget  ( "telegraph_frame",
                                                   xmFrameWidgetClass,
                                                   _baseWidget, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_FORM, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_FORM, 
                                                   XmNbottomPosition, 0, 
                                                   XmNrightPosition, 0, 
                                                   XmNtopOffset, 2, 
                                                   XmNbottomOffset, 4, 
                                                   XmNleftOffset, 150, 
                                                   XmNrightOffset, 964, 
                                                   XmNwidth, 150, 
                                                   XmNheight, 273, 
                                                   (XtPointer) NULL ); 


    _rowcolumn = XtVaCreateManagedWidget  ( "rowcolumn",
                                             xmRowColumnWidgetClass,
                                             _telegraph_frame, 
                                             XmNwidth, 144, 
                                             XmNheight, 267, 
                                             (XtPointer) NULL ); 


    _radiobox = XtVaCreateManagedWidget  ( "radiobox",
                                            xmRowColumnWidgetClass,
                                            _rowcolumn, 
                                            XmNpacking, XmPACK_COLUMN, 
                                            XmNradioBehavior, True, 
                                            XmNradioAlwaysOne, True, 
                                            XmNwidth, 138, 
                                            XmNheight, 251, 
                                            (XtPointer) NULL ); 


    _flank_toggle = XtVaCreateManagedWidget  ( "flank_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox, 
                                                XmNlabelType, XmSTRING, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _flank_toggle,
                    XmNvalueChangedCallback,
                    &HelmUI::flank_actualCallback,
                    (XtPointer) this ); 


    _full_toggle = XtVaCreateManagedWidget  ( "full_toggle",
                                               xmToggleButtonWidgetClass,
                                               _radiobox, 
                                               XmNlabelType, XmSTRING, 
                                               (XtPointer) NULL ); 

    XtAddCallback ( _full_toggle,
                    XmNvalueChangedCallback,
                    &HelmUI::full_actualCallback,
                    (XtPointer) this ); 


    _stnd_toggle1 = XtVaCreateManagedWidget  ( "stnd_toggle1",
                                                xmToggleButtonWidgetClass,
                                                _radiobox, 
                                                XmNlabelType, XmSTRING, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _stnd_toggle1,
                    XmNvalueChangedCallback,
                    &HelmUI::stnd_actualCallback,
                    (XtPointer) this ); 


    _half_toggle = XtVaCreateManagedWidget  ( "half_toggle",
                                               xmToggleButtonWidgetClass,
                                               _radiobox, 
                                               XmNlabelType, XmSTRING, 
                                               (XtPointer) NULL ); 

    XtAddCallback ( _half_toggle,
                    XmNvalueChangedCallback,
                    &HelmUI::half_actualCallback,
                    (XtPointer) this ); 


    _stop_toggle = XtVaCreateManagedWidget  ( "stop_toggle",
                                               xmToggleButtonWidgetClass,
                                               _radiobox, 
                                               XmNlabelType, XmSTRING, 
                                               XmNset, True, 
                                               (XtPointer) NULL ); 

    XtAddCallback ( _stop_toggle,
                    XmNvalueChangedCallback,
                    &HelmUI::stop_actualCallback,
                    (XtPointer) this ); 


    _back_half_toggle = XtVaCreateManagedWidget  ( "back_half_toggle",
                                                    xmToggleButtonWidgetClass,
                                                    _radiobox, 
                                                    XmNlabelType, XmSTRING, 
                                                    (XtPointer) NULL ); 

    XtAddCallback ( _back_half_toggle,
                    XmNvalueChangedCallback,
                    &HelmUI::back_half_actualCallback,
                    (XtPointer) this ); 


    _back_stnd_toggle = XtVaCreateManagedWidget  ( "back_stnd_toggle",
                                                    xmToggleButtonWidgetClass,
                                                    _radiobox, 
                                                    XmNlabelType, XmSTRING, 
                                                    (XtPointer) NULL ); 

    XtAddCallback ( _back_stnd_toggle,
                    XmNvalueChangedCallback,
                    &HelmUI::back_stnd_actualCallback,
                    (XtPointer) this ); 


    _back_full_toggle = XtVaCreateManagedWidget  ( "back_full_toggle",
                                                    xmToggleButtonWidgetClass,
                                                    _radiobox, 
                                                    XmNlabelType, XmSTRING, 
                                                    (XtPointer) NULL ); 

    XtAddCallback ( _back_full_toggle,
                    XmNvalueChangedCallback,
                    &HelmUI::back_full_actualCallback,
                    (XtPointer) this ); 


    _compass_depth_frame = XtVaCreateManagedWidget  ( "compass_depth_frame",
                                                       xmFrameWidgetClass,
                                                       _baseWidget, 
                                                       XmNshadowType, XmSHADOW_OUT, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 115, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 840, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 220, 
                                                       XmNheight, 170, 
                                                       (XtPointer) NULL ); 


    _compas_depth_form = XtVaCreateManagedWidget  ( "compas_depth_form",
                                                     xmFormWidgetClass,
                                                     _compass_depth_frame, 
                                                     XmNresizePolicy, XmRESIZE_GROW, 
                                                     XmNwidth, 214, 
                                                     XmNheight, 164, 
                                                     (XtPointer) NULL ); 


    _depth_textfield = XtVaCreateManagedWidget  ( "depth_textfield",
                                                   xmTextFieldWidgetClass,
                                                   _compas_depth_form, 
                                                   XmNeditable, False, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_NONE, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_NONE, 
                                                   XmNtopOffset, 130, 
                                                   XmNbottomOffset, 0, 
                                                   XmNleftOffset, 110, 
                                                   XmNrightOffset, 0, 
                                                   XmNwidth, 102, 
                                                   XmNheight, 30, 
                                                   (XtPointer) NULL ); 
    XtVaSetValues ( _depth_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _depth_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _compass_textfield = XtVaCreateManagedWidget  ( "compass_textfield",
                                                     xmTextFieldWidgetClass,
                                                     _compas_depth_form, 
                                                     XmNeditable, False, 
                                                     XmNtopAttachment, XmATTACH_FORM, 
                                                     XmNbottomAttachment, XmATTACH_NONE, 
                                                     XmNleftAttachment, XmATTACH_FORM, 
                                                     XmNrightAttachment, XmATTACH_NONE, 
                                                     XmNtopOffset, 130, 
                                                     XmNbottomOffset, 0, 
                                                     XmNleftOffset, 4, 
                                                     XmNrightOffset, 0, 
                                                     XmNwidth, 102, 
                                                     XmNheight, 30, 
                                                     (XtPointer) NULL ); 
    XtVaSetValues ( _compass_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _compass_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _depth_label = XtVaCreateManagedWidget  ( "depth_label",
                                               xmLabelWidgetClass,
                                               _compas_depth_form, 
                                               XmNlabelType, XmSTRING, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_NONE, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_FORM, 
                                               XmNrightPosition, 0, 
                                               XmNtopOffset, 0, 
                                               XmNbottomOffset, 0, 
                                               XmNleftOffset, 110, 
                                               XmNrightOffset, 8, 
                                               XmNwidth, 96, 
                                               XmNheight, 20, 
                                               (XtPointer) NULL ); 


    _compass_label = XtVaCreateManagedWidget  ( "compass_label",
                                                 xmLabelWidgetClass,
                                                 _compas_depth_form, 
                                                 XmNlabelType, XmSTRING, 
                                                 XmNtopAttachment, XmATTACH_FORM, 
                                                 XmNbottomAttachment, XmATTACH_NONE, 
                                                 XmNleftAttachment, XmATTACH_FORM, 
                                                 XmNrightAttachment, XmATTACH_FORM, 
                                                 XmNrightPosition, 0, 
                                                 XmNtopOffset, 0, 
                                                 XmNbottomOffset, 0, 
                                                 XmNleftOffset, 8, 
                                                 XmNrightOffset, 110, 
                                                 XmNwidth, 96, 
                                                 XmNheight, 20, 
                                                 (XtPointer) NULL ); 


    _depth_dial = XtVaCreateManagedWidget  ( "depth_dial",
                                              sgDialWidgetClass,
                                              _compas_depth_form, 
                                              SgNdialMarkers, 32, 
                                              XmNmaximum, 310, 
                                              SgNstartAngle, 200, 
                                              SgNangleRange, 310, 
                                              XmNvalue, 0, 
                                              SgNdialVisual, SgPOINTER, 
                                              XmNtopAttachment, XmATTACH_FORM, 
                                              XmNbottomAttachment, XmATTACH_NONE, 
                                              XmNleftAttachment, XmATTACH_FORM, 
                                              XmNrightAttachment, XmATTACH_NONE, 
                                              XmNtopOffset, 30, 
                                              XmNbottomOffset, 0, 
                                              XmNleftOffset, 110, 
                                              XmNrightOffset, 0, 
                                              XmNwidth, 101, 
                                              XmNheight, 100, 
                                              (XtPointer) NULL ); 


    _compass_dial = XtVaCreateManagedWidget  ( "compass_dial",
                                                sgDialWidgetClass,
                                                _compas_depth_form, 
                                                SgNdialVisual, SgPOINTER, 
                                                XmNtopAttachment, XmATTACH_FORM, 
                                                XmNbottomAttachment, XmATTACH_NONE, 
                                                XmNleftAttachment, XmATTACH_FORM, 
                                                XmNrightAttachment, XmATTACH_NONE, 
                                                XmNtopOffset, 30, 
                                                XmNbottomOffset, 0, 
                                                XmNleftOffset, 4, 
                                                XmNrightOffset, 0, 
                                                XmNwidth, 100, 
                                                XmNheight, 100, 
                                                (XtPointer) NULL ); 


    _planes_frame = XtVaCreateManagedWidget  ( "planes_frame",
                                                xmFrameWidgetClass,
                                                _baseWidget, 
                                                XmNshadowType, XmSHADOW_OUT, 
                                                XmNtopAttachment, XmATTACH_FORM, 
                                                XmNbottomAttachment, XmATTACH_NONE, 
                                                XmNleftAttachment, XmATTACH_FORM, 
                                                XmNrightAttachment, XmATTACH_NONE, 
                                                XmNtopOffset, 115, 
                                                XmNbottomOffset, 0, 
                                                XmNleftOffset, 320, 
                                                XmNrightOffset, 0, 
                                                XmNwidth, 350, 
                                                XmNheight, 170, 
                                                (XtPointer) NULL ); 


    _planes_form = XtVaCreateManagedWidget  ( "planes_form",
                                               xmFormWidgetClass,
                                               _planes_frame, 
                                               XmNresizePolicy, XmRESIZE_GROW, 
                                               XmNwidth, 344, 
                                               XmNheight, 164, 
                                               (XtPointer) NULL ); 


    _stern_plns_dial = XtVaCreateManagedWidget  ( "stern_plns_dial",
                                                   sgDialWidgetClass,
                                                   _planes_form, 
                                                   XmNminimum, -60, 
                                                   XmNmaximum, 60, 
                                                   SgNstartAngle, 30, 
                                                   SgNangleRange, 120, 
                                                   XmNvalue, 0, 
                                                   SgNdialVisual, SgPOINTER, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_NONE, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_NONE, 
                                                   XmNtopOffset, 30, 
                                                   XmNbottomOffset, 0, 
                                                   XmNleftOffset, 5, 
                                                   XmNrightOffset, 0, 
                                                   XmNwidth, 100, 
                                                   XmNheight, 100, 
                                                   (XtPointer) NULL ); 


    _rudder_textfield = XtVaCreateManagedWidget  ( "rudder_textfield",
                                                    xmTextFieldWidgetClass,
                                                    _planes_form, 
                                                    XmNeditable, False, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_NONE, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_FORM, 
                                                    XmNrightPosition, 0, 
                                                    XmNtopOffset, 130, 
                                                    XmNbottomOffset, 0, 
                                                    XmNleftOffset, 230, 
                                                    XmNrightOffset, 12, 
                                                    XmNwidth, 102, 
                                                    XmNheight, 30, 
                                                    (XtPointer) NULL ); 
    XtVaSetValues ( _rudder_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _rudder_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _fairwater_textfield = XtVaCreateManagedWidget  ( "fairwater_textfield",
                                                       xmTextFieldWidgetClass,
                                                       _planes_form, 
                                                       XmNeditable, False, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_FORM, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_FORM, 
                                                       XmNbottomPosition, 0, 
                                                       XmNrightPosition, 0, 
                                                       XmNtopOffset, 130, 
                                                       XmNbottomOffset, 5, 
                                                       XmNleftOffset, 110, 
                                                       XmNrightOffset, 136, 
                                                       XmNwidth, 98, 
                                                       XmNheight, 29, 
                                                       (XtPointer) NULL ); 
    XtVaSetValues ( _fairwater_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _fairwater_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _stern_textfield = XtVaCreateManagedWidget  ( "stern_textfield",
                                                   xmTextFieldWidgetClass,
                                                   _planes_form, 
                                                   XmNeditable, False, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_FORM, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_FORM, 
                                                   XmNbottomPosition, 0, 
                                                   XmNrightPosition, 0, 
                                                   XmNtopOffset, 130, 
                                                   XmNbottomOffset, 5, 
                                                   XmNleftOffset, 6, 
                                                   XmNrightOffset, 240, 
                                                   XmNwidth, 98, 
                                                   XmNheight, 29, 
                                                   (XtPointer) NULL ); 
    XtVaSetValues ( _stern_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _stern_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _rudder_label = XtVaCreateManagedWidget  ( "rudder_label",
                                                xmLabelWidgetClass,
                                                _planes_form, 
                                                XmNlabelType, XmSTRING, 
                                                XmNtopAttachment, XmATTACH_FORM, 
                                                XmNbottomAttachment, XmATTACH_NONE, 
                                                XmNleftAttachment, XmATTACH_FORM, 
                                                XmNrightAttachment, XmATTACH_FORM, 
                                                XmNrightPosition, 0, 
                                                XmNtopOffset, 0, 
                                                XmNbottomOffset, 0, 
                                                XmNleftOffset, 230, 
                                                XmNrightOffset, 20, 
                                                XmNwidth, 94, 
                                                XmNheight, 30, 
                                                (XtPointer) NULL ); 


    _fwtr_label = XtVaCreateManagedWidget  ( "fwtr_label",
                                              xmLabelWidgetClass,
                                              _planes_form, 
                                              XmNlabelType, XmSTRING, 
                                              XmNtopAttachment, XmATTACH_FORM, 
                                              XmNbottomAttachment, XmATTACH_NONE, 
                                              XmNleftAttachment, XmATTACH_FORM, 
                                              XmNrightAttachment, XmATTACH_FORM, 
                                              XmNrightPosition, 0, 
                                              XmNtopOffset, 0, 
                                              XmNbottomOffset, 0, 
                                              XmNleftOffset, 110, 
                                              XmNrightOffset, 140, 
                                              XmNwidth, 94, 
                                              XmNheight, 30, 
                                              (XtPointer) NULL ); 


    _stern_label = XtVaCreateManagedWidget  ( "stern_label",
                                               xmLabelWidgetClass,
                                               _planes_form, 
                                               XmNlabelType, XmSTRING, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_NONE, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_FORM, 
                                               XmNrightPosition, 0, 
                                               XmNtopOffset, 0, 
                                               XmNbottomOffset, 0, 
                                               XmNleftOffset, 0, 
                                               XmNrightOffset, 230, 
                                               XmNwidth, 114, 
                                               XmNheight, 30, 
                                               (XtPointer) NULL ); 


    _rudder_dial = XtVaCreateManagedWidget  ( "rudder_dial",
                                               sgDialWidgetClass,
                                               _planes_form, 
                                               XmNminimum, 120, 
                                               XmNmaximum, 240, 
                                               SgNstartAngle, 119, 
                                               SgNangleRange, 120, 
                                               XmNvalue, 180, 
                                               SgNdialVisual, SgPOINTER, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_NONE, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_NONE, 
                                               XmNtopOffset, 30, 
                                               XmNbottomOffset, 0, 
                                               XmNleftOffset, 230, 
                                               XmNrightOffset, 0, 
                                               XmNwidth, 101, 
                                               XmNheight, 100, 
                                               (XtPointer) NULL ); 


    _fwtr_planes_dial = XtVaCreateManagedWidget  ( "fwtr_planes_dial",
                                                    sgDialWidgetClass,
                                                    _planes_form, 
                                                    XmNminimum, -60, 
                                                    XmNmaximum, 60, 
                                                    SgNstartAngle, 30, 
                                                    SgNangleRange, 120, 
                                                    XmNvalue, 0, 
                                                    SgNdialVisual, SgPOINTER, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_NONE, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_NONE, 
                                                    XmNtopOffset, 30, 
                                                    XmNbottomOffset, 0, 
                                                    XmNleftOffset, 110, 
                                                    XmNrightOffset, 0, 
                                                    XmNwidth, 100, 
                                                    XmNheight, 100, 
                                                    (XtPointer) NULL ); 


}

const char * HelmUI::className()
{
    return ("HelmUI");
} // End className()




/////////////////////////////////////////////////////////////// 
// The following functions are static member functions used to 
// interface with Motif.
/////////////////////////////////// 

void HelmUI::back_full_actualCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    HelmUI* obj = ( HelmUI * ) clientData;
    obj->back_full_actual ( w, callData );
}

void HelmUI::back_half_actualCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    HelmUI* obj = ( HelmUI * ) clientData;
    obj->back_half_actual ( w, callData );
}

void HelmUI::back_stnd_actualCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    HelmUI* obj = ( HelmUI * ) clientData;
    obj->back_stnd_actual ( w, callData );
}

void HelmUI::flank_actualCallback ( Widget    w,
                                    XtPointer clientData,
                                    XtPointer callData ) 
{ 
    HelmUI* obj = ( HelmUI * ) clientData;
    obj->flank_actual ( w, callData );
}

void HelmUI::full_actualCallback ( Widget    w,
                                   XtPointer clientData,
                                   XtPointer callData ) 
{ 
    HelmUI* obj = ( HelmUI * ) clientData;
    obj->full_actual ( w, callData );
}

void HelmUI::half_actualCallback ( Widget    w,
                                   XtPointer clientData,
                                   XtPointer callData ) 
{ 
    HelmUI* obj = ( HelmUI * ) clientData;
    obj->half_actual ( w, callData );
}

void HelmUI::stnd_actualCallback ( Widget    w,
                                   XtPointer clientData,
                                   XtPointer callData ) 
{ 
    HelmUI* obj = ( HelmUI * ) clientData;
    obj->stnd_actual ( w, callData );
}

void HelmUI::stop_actualCallback ( Widget    w,
                                   XtPointer clientData,
                                   XtPointer callData ) 
{ 
    HelmUI* obj = ( HelmUI * ) clientData;
    obj->stop_actual ( w, callData );
}



/////////////////////////////////////////////////////////////// 
// The following functions are called from the menu items 
// in this window.
/////////////////////////////////// 

void HelmUI::back_full_actual ( Widget, XtPointer ) 
{
    // This virtual function is called from back_full_actualCallback.
    // This function is normally overriden by a derived class.

}

void HelmUI::back_half_actual ( Widget, XtPointer ) 
{
    // This virtual function is called from back_half_actualCallback.
    // This function is normally overriden by a derived class.

}

void HelmUI::back_stnd_actual ( Widget, XtPointer ) 
{
    // This virtual function is called from back_stnd_actualCallback.
    // This function is normally overriden by a derived class.

}

void HelmUI::flank_actual ( Widget, XtPointer ) 
{
    // This virtual function is called from flank_actualCallback.
    // This function is normally overriden by a derived class.

}

void HelmUI::full_actual ( Widget, XtPointer ) 
{
    // This virtual function is called from full_actualCallback.
    // This function is normally overriden by a derived class.

}

void HelmUI::half_actual ( Widget, XtPointer ) 
{
    // This virtual function is called from half_actualCallback.
    // This function is normally overriden by a derived class.

}

void HelmUI::stnd_actual ( Widget, XtPointer ) 
{
    // This virtual function is called from stnd_actualCallback.
    // This function is normally overriden by a derived class.

}

void HelmUI::stop_actual ( Widget, XtPointer ) 
{
    // This virtual function is called from stop_actualCallback.
    // This function is normally overriden by a derived class.

}

//---- End generated code section

