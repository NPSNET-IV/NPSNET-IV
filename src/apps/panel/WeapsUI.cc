
/////////////////////////////////////////////////////////////
//
// Source file for WeapsUI
//
//    This class implements the user interface created in 
//    the interface builder. Normally it is not used directly.
//    Instead the subclass, Weaps is instantiated
//
//    To extend or alter the behavior of this class, you should
//    modify the Weaps files
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


#include "WeapsUI.h" // Generated header file for this class
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

String  WeapsUI::_defaultWeapsUIResources[] = {
        (char*)NULL
};

WeapsUI::WeapsUI(const char *name) : VkComponent(name) 
{ 
    // No widgets are created by this constructor.
    // If an application creates a component using this constructor,
    // It must explictly call create at a later time.
    // This is mostly useful when adding pre-widget creation
    // code to a derived class constructor.

    //---- End generated code section

} // End Constructor




WeapsUI::WeapsUI(const char *name, Widget parent) : VkComponent(name) 
{ 
    // Call creation function to build the widget tree.

     create(parent);

    //---- End generated code section

} // End Constructor


WeapsUI::~WeapsUI() 
{
    // Base class destroys widgets

    //-- End generated code section

} // End destructor



void WeapsUI::create ( Widget parent )
{
    Arg      args[14];
    Cardinal count;
    count = 0;

    // Load any class-defaulted resources for this object

    setDefaultResources(parent, _defaultWeapsUIResources  );


    // Create an unmanaged widget as the top of the widget hierarchy

    _baseWidget = _weaps = XtVaCreateWidget ( _name,
                                              xmFormWidgetClass,
                                              parent, 
                                              XmNresizePolicy, XmRESIZE_GROW, 
                                              (XtPointer) NULL ); 

    // install a callback to guard against unexpected widget destruction

    installDestroyHandler();


    // Create widgets used in this component
    // All variables are data members of this class

    _torpedo_label1 = XtVaCreateManagedWidget  ( "torpedo_label1",
                                                  xmLabelWidgetClass,
                                                  _baseWidget, 
                                                  XmNlabelType, XmSTRING, 
                                                  XmNtopAttachment, XmATTACH_FORM, 
                                                  XmNbottomAttachment, XmATTACH_NONE, 
                                                  XmNleftAttachment, XmATTACH_FORM, 
                                                  XmNrightAttachment, XmATTACH_NONE, 
                                                  XmNtopOffset, 250, 
                                                  XmNbottomOffset, 0, 
                                                  XmNleftOffset, 940, 
                                                  XmNrightOffset, 0, 
                                                  XmNwidth, 230, 
                                                  XmNheight, 25, 
                                                  (XtPointer) NULL ); 


    _tomahawk_frame = XtVaCreateManagedWidget  ( "tomahawk_frame",
                                                  xmFrameWidgetClass,
                                                  _baseWidget, 
                                                  XmNtopAttachment, XmATTACH_FORM, 
                                                  XmNbottomAttachment, XmATTACH_NONE, 
                                                  XmNleftAttachment, XmATTACH_FORM, 
                                                  XmNrightAttachment, XmATTACH_NONE, 
                                                  XmNtopOffset, 20, 
                                                  XmNbottomOffset, 0, 
                                                  XmNleftOffset, 880, 
                                                  XmNrightOffset, 0, 
                                                  XmNwidth, 360, 
                                                  XmNheight, 220, 
                                                  (XtPointer) NULL ); 


    _form = XtVaCreateManagedWidget  ( "form",
                                        xmFormWidgetClass,
                                        _tomahawk_frame, 
                                        XmNresizePolicy, XmRESIZE_GROW, 
                                        XmNwidth, 354, 
                                        XmNheight, 214, 
                                        (XtPointer) NULL ); 


    _separator2 = XtVaCreateManagedWidget  ( "separator2",
                                              xmSeparatorWidgetClass,
                                              _form, 
                                              XmNorientation, XmVERTICAL, 
                                              XmNtopAttachment, XmATTACH_FORM, 
                                              XmNbottomAttachment, XmATTACH_FORM, 
                                              XmNleftAttachment, XmATTACH_FORM, 
                                              XmNrightAttachment, XmATTACH_NONE, 
                                              XmNbottomPosition, 0, 
                                              XmNtopOffset, 0, 
                                              XmNbottomOffset, 0, 
                                              XmNleftOffset, 150, 
                                              XmNrightOffset, 0, 
                                              XmNwidth, 20, 
                                              XmNheight, 214, 
                                              (XtPointer) NULL ); 


    _shoot_tlam_button = XtVaCreateManagedWidget  ( "shoot_tlam_button",
                                                     xmPushButtonWidgetClass,
                                                     _form, 
                                                     XmNlabelType, XmSTRING, 
                                                     XmNtopAttachment, XmATTACH_FORM, 
                                                     XmNbottomAttachment, XmATTACH_NONE, 
                                                     XmNleftAttachment, XmATTACH_FORM, 
                                                     XmNrightAttachment, XmATTACH_NONE, 
                                                     XmNtopOffset, 90, 
                                                     XmNbottomOffset, 0, 
                                                     XmNleftOffset, 220, 
                                                     XmNrightOffset, 0, 
                                                     XmNwidth, 70, 
                                                     XmNheight, 30, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _shoot_tlam_button,
                    XmNactivateCallback,
                    &WeapsUI::shoot_tlamCallback,
                    (XtPointer) this ); 


    _radiobox7 = XtVaCreateManagedWidget  ( "radiobox7",
                                             xmRowColumnWidgetClass,
                                             _form, 
                                             XmNpacking, XmPACK_COLUMN, 
                                             XmNradioBehavior, True, 
                                             XmNradioAlwaysOne, True, 
                                             XmNtopAttachment, XmATTACH_FORM, 
                                             XmNbottomAttachment, XmATTACH_NONE, 
                                             XmNleftAttachment, XmATTACH_FORM, 
                                             XmNrightAttachment, XmATTACH_NONE, 
                                             XmNtopOffset, 10, 
                                             XmNbottomOffset, 0, 
                                             XmNleftOffset, 20, 
                                             XmNrightOffset, 0, 
                                             XmNwidth, 93, 
                                             XmNheight, 189, 
                                             (XtPointer) NULL ); 


    _tlam1_toggle = XtVaCreateManagedWidget  ( "tlam1_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox7, 
                                                XmNlabelType, XmSTRING, 
                                                XmNset, True, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _tlam1_toggle,
                    XmNvalueChangedCallback,
                    &WeapsUI::select_tlam1Callback,
                    (XtPointer) this ); 


    _tlam2_toggle = XtVaCreateManagedWidget  ( "tlam2_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox7, 
                                                XmNlabelType, XmSTRING, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _tlam2_toggle,
                    XmNvalueChangedCallback,
                    &WeapsUI::select_tlam2Callback,
                    (XtPointer) this ); 


    _tlam3_toggle = XtVaCreateManagedWidget  ( "tlam3_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox7, 
                                                XmNlabelType, XmSTRING, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _tlam3_toggle,
                    XmNvalueChangedCallback,
                    &WeapsUI::select_tlam3Callback,
                    (XtPointer) this ); 


    _tlam4_toggle = XtVaCreateManagedWidget  ( "tlam4_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox7, 
                                                XmNlabelType, XmSTRING, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _tlam4_toggle,
                    XmNvalueChangedCallback,
                    &WeapsUI::select_tlam4Callback,
                    (XtPointer) this ); 


    _tlam5_toggle = XtVaCreateManagedWidget  ( "tlam5_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox7, 
                                                XmNlabelType, XmSTRING, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _tlam5_toggle,
                    XmNvalueChangedCallback,
                    &WeapsUI::select_tlam5Callback,
                    (XtPointer) this ); 


    _tlam6_toggle = XtVaCreateManagedWidget  ( "tlam6_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox7, 
                                                XmNlabelType, XmSTRING, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _tlam6_toggle,
                    XmNvalueChangedCallback,
                    &WeapsUI::select_tlam6Callback,
                    (XtPointer) this ); 


    _torpedo_label = XtVaCreateManagedWidget  ( "torpedo_label",
                                                 xmLabelWidgetClass,
                                                 _baseWidget, 
                                                 XmNlabelType, XmSTRING, 
                                                 XmNtopAttachment, XmATTACH_FORM, 
                                                 XmNbottomAttachment, XmATTACH_NONE, 
                                                 XmNleftAttachment, XmATTACH_FORM, 
                                                 XmNrightAttachment, XmATTACH_NONE, 
                                                 XmNtopOffset, 250, 
                                                 XmNbottomOffset, 0, 
                                                 XmNleftOffset, 130, 
                                                 XmNrightOffset, 0, 
                                                 XmNwidth, 210, 
                                                 XmNheight, 30, 
                                                 (XtPointer) NULL ); 


    _torp_frame = XtVaCreateManagedWidget  ( "torp_frame",
                                              xmFrameWidgetClass,
                                              _baseWidget, 
                                              XmNtopAttachment, XmATTACH_FORM, 
                                              XmNbottomAttachment, XmATTACH_NONE, 
                                              XmNleftAttachment, XmATTACH_FORM, 
                                              XmNrightAttachment, XmATTACH_NONE, 
                                              XmNtopOffset, 10, 
                                              XmNbottomOffset, 0, 
                                              XmNleftOffset, 10, 
                                              XmNrightOffset, 0, 
                                              XmNwidth, 530, 
                                              XmNheight, 230, 
                                              (XtPointer) NULL ); 


    _torp_form = XtVaCreateManagedWidget  ( "torp_form",
                                             xmFormWidgetClass,
                                             _torp_frame, 
                                             XmNresizePolicy, XmRESIZE_GROW, 
                                             XmNwidth, 524, 
                                             XmNheight, 224, 
                                             (XtPointer) NULL ); 


    _select_label1 = XtVaCreateManagedWidget  ( "select_label1",
                                                 xmLabelWidgetClass,
                                                 _torp_form, 
                                                 XmNlabelType, XmSTRING, 
                                                 XmNtopAttachment, XmATTACH_FORM, 
                                                 XmNbottomAttachment, XmATTACH_NONE, 
                                                 XmNleftAttachment, XmATTACH_FORM, 
                                                 XmNrightAttachment, XmATTACH_NONE, 
                                                 XmNtopOffset, 120, 
                                                 XmNbottomOffset, 0, 
                                                 XmNleftOffset, 130, 
                                                 XmNrightOffset, 0, 
                                                 XmNwidth, 59, 
                                                 XmNheight, 20, 
                                                 (XtPointer) NULL ); 


    _ready_label1 = XtVaCreateManagedWidget  ( "ready_label1",
                                                xmLabelWidgetClass,
                                                _torp_form, 
                                                XmNlabelType, XmSTRING, 
                                                XmNtopAttachment, XmATTACH_FORM, 
                                                XmNbottomAttachment, XmATTACH_NONE, 
                                                XmNleftAttachment, XmATTACH_FORM, 
                                                XmNrightAttachment, XmATTACH_NONE, 
                                                XmNtopOffset, 120, 
                                                XmNbottomOffset, 0, 
                                                XmNleftOffset, 20, 
                                                XmNrightOffset, 0, 
                                                XmNwidth, 55, 
                                                XmNheight, 20, 
                                                (XtPointer) NULL ); 


    _tube4_ready_toggle = XtVaCreateManagedWidget  ( "tube4_ready_toggle",
                                                      xmToggleButtonWidgetClass,
                                                      _torp_form, 
                                                      XmNalignment, XmALIGNMENT_BEGINNING, 
                                                      XmNlabelType, XmSTRING, 
                                                      XmNset, True, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 185, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 30, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 30, 
                                                      XmNheight, 20, 
                                                      (XtPointer) NULL ); 


    _tube2_ready_toggle = XtVaCreateManagedWidget  ( "tube2_ready_toggle",
                                                      xmToggleButtonWidgetClass,
                                                      _torp_form, 
                                                      XmNalignment, XmALIGNMENT_BEGINNING, 
                                                      XmNlabelType, XmSTRING, 
                                                      XmNset, True, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 155, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 30, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 30, 
                                                      XmNheight, 20, 
                                                      (XtPointer) NULL ); 


    _tube4_label = XtVaCreateManagedWidget  ( "tube4_label",
                                               xmLabelWidgetClass,
                                               _torp_form, 
                                               XmNlabelType, XmSTRING, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_NONE, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_NONE, 
                                               XmNtopOffset, 185, 
                                               XmNbottomOffset, 0, 
                                               XmNleftOffset, 70, 
                                               XmNrightOffset, 0, 
                                               XmNwidth, 47, 
                                               XmNheight, 20, 
                                               (XtPointer) NULL ); 


    _tube2_label = XtVaCreateManagedWidget  ( "tube2_label",
                                               xmLabelWidgetClass,
                                               _torp_form, 
                                               XmNlabelType, XmSTRING, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_NONE, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_NONE, 
                                               XmNtopOffset, 155, 
                                               XmNbottomOffset, 0, 
                                               XmNleftOffset, 70, 
                                               XmNrightOffset, 0, 
                                               XmNwidth, 47, 
                                               XmNheight, 20, 
                                               (XtPointer) NULL ); 


    _radiobox6 = XtVaCreateManagedWidget  ( "radiobox6",
                                             xmRowColumnWidgetClass,
                                             _torp_form, 
                                             XmNentryAlignment, XmALIGNMENT_END, 
                                             XmNpacking, XmPACK_COLUMN, 
                                             XmNradioBehavior, True, 
                                             XmNradioAlwaysOne, True, 
                                             XmNtopAttachment, XmATTACH_FORM, 
                                             XmNbottomAttachment, XmATTACH_NONE, 
                                             XmNleftAttachment, XmATTACH_FORM, 
                                             XmNrightAttachment, XmATTACH_NONE, 
                                             XmNtopOffset, 150, 
                                             XmNbottomOffset, 0, 
                                             XmNleftOffset, 140, 
                                             XmNrightOffset, 0, 
                                             XmNwidth, 35, 
                                             XmNheight, 65, 
                                             (XtPointer) NULL ); 


    _tube2_toggle = XtVaCreateManagedWidget  ( "tube2_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox6, 
                                                XmNlabelType, XmSTRING, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _tube2_toggle,
                    XmNvalueChangedCallback,
                    &WeapsUI::select_tube2Callback,
                    (XtPointer) this ); 


    _tube4_toggle = XtVaCreateManagedWidget  ( "tube4_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox6, 
                                                XmNlabelType, XmSTRING, 
                                                XmNset, True, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _tube4_toggle,
                    XmNvalueChangedCallback,
                    &WeapsUI::select_tube4Callback,
                    (XtPointer) this ); 


    _shoot_port_button = XtVaCreateManagedWidget  ( "shoot_port_button",
                                                     xmPushButtonWidgetClass,
                                                     _torp_form, 
                                                     XmNlabelType, XmSTRING, 
                                                     XmNtopAttachment, XmATTACH_FORM, 
                                                     XmNbottomAttachment, XmATTACH_NONE, 
                                                     XmNleftAttachment, XmATTACH_FORM, 
                                                     XmNrightAttachment, XmATTACH_NONE, 
                                                     XmNtopOffset, 160, 
                                                     XmNbottomOffset, 0, 
                                                     XmNleftOffset, 360, 
                                                     XmNrightOffset, 0, 
                                                     XmNwidth, 72, 
                                                     XmNheight, 32, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _shoot_port_button,
                    XmNactivateCallback,
                    &WeapsUI::port_tube_shootCallback,
                    (XtPointer) this ); 


    _separator1 = XtVaCreateManagedWidget  ( "separator1",
                                              xmSeparatorWidgetClass,
                                              _torp_form, 
                                              XmNtopAttachment, XmATTACH_FORM, 
                                              XmNbottomAttachment, XmATTACH_NONE, 
                                              XmNleftAttachment, XmATTACH_FORM, 
                                              XmNrightAttachment, XmATTACH_FORM, 
                                              XmNrightPosition, 0, 
                                              XmNtopOffset, 105, 
                                              XmNbottomOffset, 0, 
                                              XmNleftOffset, 0, 
                                              XmNrightOffset, 0, 
                                              XmNwidth, 524, 
                                              XmNheight, 23, 
                                              (XtPointer) NULL ); 


    _pt_crs_1s_dwn_arrow = XtVaCreateManagedWidget  ( "pt_crs_1s_dwn_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _torp_form, 
                                                       XmNarrowDirection, XmARROW_DOWN, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 190, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 280, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _pt_crs_1s_dwn_arrow,
                    XmNactivateCallback,
                    &WeapsUI::pt_crs_1s_minusCallback,
                    (XtPointer) this ); 


    _pt_crs_10s_dwn_arrow1 = XtVaCreateManagedWidget  ( "pt_crs_10s_dwn_arrow1",
                                                         xmArrowButtonWidgetClass,
                                                         _torp_form, 
                                                         XmNarrowDirection, XmARROW_DOWN, 
                                                         XmNtopAttachment, XmATTACH_FORM, 
                                                         XmNbottomAttachment, XmATTACH_NONE, 
                                                         XmNleftAttachment, XmATTACH_FORM, 
                                                         XmNrightAttachment, XmATTACH_NONE, 
                                                         XmNtopOffset, 190, 
                                                         XmNbottomOffset, 0, 
                                                         XmNleftOffset, 240, 
                                                         XmNrightOffset, 0, 
                                                         XmNwidth, 30, 
                                                         XmNheight, 30, 
                                                         (XtPointer) NULL ); 

    XtAddCallback ( _pt_crs_10s_dwn_arrow1,
                    XmNactivateCallback,
                    &WeapsUI::pt_crs_10s_minusCallback,
                    (XtPointer) this ); 


    _pt_crs_100s_dwn_arrow0 = XtVaCreateManagedWidget  ( "pt_crs_100s_dwn_arrow0",
                                                          xmArrowButtonWidgetClass,
                                                          _torp_form, 
                                                          XmNarrowDirection, XmARROW_DOWN, 
                                                          XmNtopAttachment, XmATTACH_FORM, 
                                                          XmNbottomAttachment, XmATTACH_NONE, 
                                                          XmNleftAttachment, XmATTACH_FORM, 
                                                          XmNrightAttachment, XmATTACH_NONE, 
                                                          XmNtopOffset, 190, 
                                                          XmNbottomOffset, 0, 
                                                          XmNleftOffset, 200, 
                                                          XmNrightOffset, 0, 
                                                          XmNwidth, 30, 
                                                          XmNheight, 30, 
                                                          (XtPointer) NULL ); 

    XtAddCallback ( _pt_crs_100s_dwn_arrow0,
                    XmNactivateCallback,
                    &WeapsUI::pt_crs_100s_minusCallback,
                    (XtPointer) this ); 


    _pt_course_1s_textfield = XtVaCreateManagedWidget  ( "pt_course_1s_textfield",
                                                          xmTextFieldWidgetClass,
                                                          _torp_form, 
                                                          XmNeditable, False, 
                                                          XmNtopAttachment, XmATTACH_FORM, 
                                                          XmNbottomAttachment, XmATTACH_NONE, 
                                                          XmNleftAttachment, XmATTACH_FORM, 
                                                          XmNrightAttachment, XmATTACH_NONE, 
                                                          XmNtopOffset, 160, 
                                                          XmNbottomOffset, 0, 
                                                          XmNleftOffset, 280, 
                                                          XmNrightOffset, 0, 
                                                          XmNwidth, 30, 
                                                          XmNheight, 30, 
                                                          (XtPointer) NULL ); 
    XtVaSetValues ( _pt_course_1s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _pt_course_1s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _pt_course_10s_textfield = XtVaCreateManagedWidget  ( "pt_course_10s_textfield",
                                                           xmTextFieldWidgetClass,
                                                           _torp_form, 
                                                           XmNeditable, False, 
                                                           XmNtopAttachment, XmATTACH_FORM, 
                                                           XmNbottomAttachment, XmATTACH_NONE, 
                                                           XmNleftAttachment, XmATTACH_FORM, 
                                                           XmNrightAttachment, XmATTACH_NONE, 
                                                           XmNtopOffset, 160, 
                                                           XmNbottomOffset, 0, 
                                                           XmNleftOffset, 240, 
                                                           XmNrightOffset, 0, 
                                                           XmNwidth, 30, 
                                                           XmNheight, 30, 
                                                           (XtPointer) NULL ); 
    XtVaSetValues ( _pt_course_10s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _pt_course_10s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _pt_course_100s_textfield = XtVaCreateManagedWidget  ( "pt_course_100s_textfield",
                                                            xmTextFieldWidgetClass,
                                                            _torp_form, 
                                                            XmNeditable, False, 
                                                            XmNtopAttachment, XmATTACH_FORM, 
                                                            XmNbottomAttachment, XmATTACH_NONE, 
                                                            XmNleftAttachment, XmATTACH_FORM, 
                                                            XmNrightAttachment, XmATTACH_NONE, 
                                                            XmNtopOffset, 160, 
                                                            XmNbottomOffset, 0, 
                                                            XmNleftOffset, 200, 
                                                            XmNrightOffset, 0, 
                                                            XmNwidth, 30, 
                                                            XmNheight, 30, 
                                                            (XtPointer) NULL ); 
    XtVaSetValues ( _pt_course_100s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _pt_course_100s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _pt_crs_1s_up_arrow = XtVaCreateManagedWidget  ( "pt_crs_1s_up_arrow",
                                                      xmArrowButtonWidgetClass,
                                                      _torp_form, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 130, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 280, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 30, 
                                                      XmNheight, 30, 
                                                      (XtPointer) NULL ); 

    XtAddCallback ( _pt_crs_1s_up_arrow,
                    XmNactivateCallback,
                    &WeapsUI::pt_crs_1s_addCallback,
                    (XtPointer) this ); 


    _pt_crs_10s_up_arrow = XtVaCreateManagedWidget  ( "pt_crs_10s_up_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _torp_form, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 130, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 240, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _pt_crs_10s_up_arrow,
                    XmNactivateCallback,
                    &WeapsUI::pt_crs_10s_addCallback,
                    (XtPointer) this ); 


    _pt_crs_100s_up_arrow = XtVaCreateManagedWidget  ( "pt_crs_100s_up_arrow",
                                                        xmArrowButtonWidgetClass,
                                                        _torp_form, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_NONE, 
                                                        XmNtopOffset, 130, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 200, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 30, 
                                                        XmNheight, 30, 
                                                        (XtPointer) NULL ); 

    XtAddCallback ( _pt_crs_100s_up_arrow,
                    XmNactivateCallback,
                    &WeapsUI::pt_crs_100s_addCallback,
                    (XtPointer) this ); 


    _shoot_stbd_button = XtVaCreateManagedWidget  ( "shoot_stbd_button",
                                                     xmPushButtonWidgetClass,
                                                     _torp_form, 
                                                     XmNlabelType, XmSTRING, 
                                                     XmNtopAttachment, XmATTACH_FORM, 
                                                     XmNbottomAttachment, XmATTACH_NONE, 
                                                     XmNleftAttachment, XmATTACH_FORM, 
                                                     XmNrightAttachment, XmATTACH_NONE, 
                                                     XmNtopOffset, 40, 
                                                     XmNbottomOffset, 0, 
                                                     XmNleftOffset, 360, 
                                                     XmNrightOffset, 0, 
                                                     XmNwidth, 72, 
                                                     XmNheight, 32, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _shoot_stbd_button,
                    XmNactivateCallback,
                    &WeapsUI::stbd_tube_shootCallback,
                    (XtPointer) this ); 


    _select_label = XtVaCreateManagedWidget  ( "select_label",
                                                xmLabelWidgetClass,
                                                _torp_form, 
                                                XmNlabelType, XmSTRING, 
                                                XmNtopAttachment, XmATTACH_FORM, 
                                                XmNbottomAttachment, XmATTACH_NONE, 
                                                XmNleftAttachment, XmATTACH_FORM, 
                                                XmNrightAttachment, XmATTACH_NONE, 
                                                XmNtopOffset, 10, 
                                                XmNbottomOffset, 0, 
                                                XmNleftOffset, 130, 
                                                XmNrightOffset, 0, 
                                                XmNwidth, 59, 
                                                XmNheight, 20, 
                                                (XtPointer) NULL ); 


    _st_crs_1s_dwn_arrow = XtVaCreateManagedWidget  ( "st_crs_1s_dwn_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _torp_form, 
                                                       XmNarrowDirection, XmARROW_DOWN, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 70, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 280, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _st_crs_1s_dwn_arrow,
                    XmNactivateCallback,
                    &WeapsUI::st_crs_1s_minusCallback,
                    (XtPointer) this ); 


    _st_crs_10s_dwn_arrow = XtVaCreateManagedWidget  ( "st_crs_10s_dwn_arrow",
                                                        xmArrowButtonWidgetClass,
                                                        _torp_form, 
                                                        XmNarrowDirection, XmARROW_DOWN, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_NONE, 
                                                        XmNtopOffset, 70, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 240, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 30, 
                                                        XmNheight, 30, 
                                                        (XtPointer) NULL ); 

    XtAddCallback ( _st_crs_10s_dwn_arrow,
                    XmNactivateCallback,
                    &WeapsUI::st_crs_10s_minusCallback,
                    (XtPointer) this ); 


    _st_crs_100s_dwn_arrow = XtVaCreateManagedWidget  ( "st_crs_100s_dwn_arrow",
                                                         xmArrowButtonWidgetClass,
                                                         _torp_form, 
                                                         XmNarrowDirection, XmARROW_DOWN, 
                                                         XmNtopAttachment, XmATTACH_FORM, 
                                                         XmNbottomAttachment, XmATTACH_NONE, 
                                                         XmNleftAttachment, XmATTACH_FORM, 
                                                         XmNrightAttachment, XmATTACH_NONE, 
                                                         XmNtopOffset, 70, 
                                                         XmNbottomOffset, 0, 
                                                         XmNleftOffset, 200, 
                                                         XmNrightOffset, 0, 
                                                         XmNwidth, 30, 
                                                         XmNheight, 30, 
                                                         (XtPointer) NULL ); 

    XtAddCallback ( _st_crs_100s_dwn_arrow,
                    XmNactivateCallback,
                    &WeapsUI::st_crs_100s_minusCallback,
                    (XtPointer) this ); 


    _st_crs_1s_up_arrow = XtVaCreateManagedWidget  ( "st_crs_1s_up_arrow",
                                                      xmArrowButtonWidgetClass,
                                                      _torp_form, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 10, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 280, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 30, 
                                                      XmNheight, 30, 
                                                      (XtPointer) NULL ); 

    XtAddCallback ( _st_crs_1s_up_arrow,
                    XmNactivateCallback,
                    &WeapsUI::st_crs_1s_addCallback,
                    (XtPointer) this ); 


    _st_crs_10s_up_arrow = XtVaCreateManagedWidget  ( "st_crs_10s_up_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _torp_form, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 10, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 240, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _st_crs_10s_up_arrow,
                    XmNactivateCallback,
                    &WeapsUI::st_crs_10s_addCallback,
                    (XtPointer) this ); 


    _st_crs_100s_up_arrow = XtVaCreateManagedWidget  ( "st_crs_100s_up_arrow",
                                                        xmArrowButtonWidgetClass,
                                                        _torp_form, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_NONE, 
                                                        XmNtopOffset, 10, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 200, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 30, 
                                                        XmNheight, 30, 
                                                        (XtPointer) NULL ); 

    XtAddCallback ( _st_crs_100s_up_arrow,
                    XmNactivateCallback,
                    &WeapsUI::st_crs_100s_addCallback,
                    (XtPointer) this ); 


    _st_course_1s_textfield = XtVaCreateManagedWidget  ( "st_course_1s_textfield",
                                                          xmTextFieldWidgetClass,
                                                          _torp_form, 
                                                          XmNeditable, False, 
                                                          XmNtopAttachment, XmATTACH_FORM, 
                                                          XmNbottomAttachment, XmATTACH_NONE, 
                                                          XmNleftAttachment, XmATTACH_FORM, 
                                                          XmNrightAttachment, XmATTACH_NONE, 
                                                          XmNtopOffset, 40, 
                                                          XmNbottomOffset, 0, 
                                                          XmNleftOffset, 280, 
                                                          XmNrightOffset, 0, 
                                                          XmNwidth, 30, 
                                                          XmNheight, 30, 
                                                          (XtPointer) NULL ); 
    XtVaSetValues ( _st_course_1s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _st_course_1s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _st_course_10s_textfield = XtVaCreateManagedWidget  ( "st_course_10s_textfield",
                                                           xmTextFieldWidgetClass,
                                                           _torp_form, 
                                                           XmNeditable, False, 
                                                           XmNtopAttachment, XmATTACH_FORM, 
                                                           XmNbottomAttachment, XmATTACH_NONE, 
                                                           XmNleftAttachment, XmATTACH_FORM, 
                                                           XmNrightAttachment, XmATTACH_NONE, 
                                                           XmNtopOffset, 40, 
                                                           XmNbottomOffset, 0, 
                                                           XmNleftOffset, 240, 
                                                           XmNrightOffset, 0, 
                                                           XmNwidth, 30, 
                                                           XmNheight, 30, 
                                                           (XtPointer) NULL ); 
    XtVaSetValues ( _st_course_10s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _st_course_10s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _st_course_100s_textfield = XtVaCreateManagedWidget  ( "st_course_100s_textfield",
                                                            xmTextFieldWidgetClass,
                                                            _torp_form, 
                                                            XmNeditable, False, 
                                                            XmNtopAttachment, XmATTACH_FORM, 
                                                            XmNbottomAttachment, XmATTACH_NONE, 
                                                            XmNleftAttachment, XmATTACH_FORM, 
                                                            XmNrightAttachment, XmATTACH_NONE, 
                                                            XmNtopOffset, 40, 
                                                            XmNbottomOffset, 0, 
                                                            XmNleftOffset, 200, 
                                                            XmNrightOffset, 0, 
                                                            XmNwidth, 30, 
                                                            XmNheight, 30, 
                                                            (XtPointer) NULL ); 
    XtVaSetValues ( _st_course_100s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _st_course_100s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _ready_label = XtVaCreateManagedWidget  ( "ready_label",
                                               xmLabelWidgetClass,
                                               _torp_form, 
                                               XmNlabelType, XmSTRING, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_NONE, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_NONE, 
                                               XmNtopOffset, 10, 
                                               XmNbottomOffset, 0, 
                                               XmNleftOffset, 20, 
                                               XmNrightOffset, 0, 
                                               XmNwidth, 55, 
                                               XmNheight, 20, 
                                               (XtPointer) NULL ); 


    _tube3_ready_toggle = XtVaCreateManagedWidget  ( "tube3_ready_toggle",
                                                      xmToggleButtonWidgetClass,
                                                      _torp_form, 
                                                      XmNalignment, XmALIGNMENT_BEGINNING, 
                                                      XmNlabelType, XmSTRING, 
                                                      XmNset, True, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 55, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 30, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 30, 
                                                      XmNheight, 20, 
                                                      (XtPointer) NULL ); 


    _tube1_ready_toggle = XtVaCreateManagedWidget  ( "tube1_ready_toggle",
                                                      xmToggleButtonWidgetClass,
                                                      _torp_form, 
                                                      XmNalignment, XmALIGNMENT_BEGINNING, 
                                                      XmNlabelType, XmSTRING, 
                                                      XmNset, True, 
                                                      XmNindicatorOn, True, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 35, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 30, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 30, 
                                                      XmNheight, 20, 
                                                      (XtPointer) NULL ); 


    _tube3_label = XtVaCreateManagedWidget  ( "tube3_label",
                                               xmLabelWidgetClass,
                                               _torp_form, 
                                               XmNlabelType, XmSTRING, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_NONE, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_NONE, 
                                               XmNtopOffset, 55, 
                                               XmNbottomOffset, 0, 
                                               XmNleftOffset, 70, 
                                               XmNrightOffset, 0, 
                                               XmNwidth, 47, 
                                               XmNheight, 20, 
                                               (XtPointer) NULL ); 


    _tube1_label = XtVaCreateManagedWidget  ( "tube1_label",
                                               xmLabelWidgetClass,
                                               _torp_form, 
                                               XmNlabelType, XmSTRING, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_NONE, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_NONE, 
                                               XmNtopOffset, 33, 
                                               XmNbottomOffset, 0, 
                                               XmNleftOffset, 70, 
                                               XmNrightOffset, 0, 
                                               XmNwidth, 47, 
                                               XmNheight, 20, 
                                               (XtPointer) NULL ); 


    _radiobox5 = XtVaCreateManagedWidget  ( "radiobox5",
                                             xmRowColumnWidgetClass,
                                             _torp_form, 
                                             XmNentryAlignment, XmALIGNMENT_END, 
                                             XmNpacking, XmPACK_COLUMN, 
                                             XmNradioBehavior, True, 
                                             XmNradioAlwaysOne, True, 
                                             XmNtopAttachment, XmATTACH_FORM, 
                                             XmNbottomAttachment, XmATTACH_NONE, 
                                             XmNleftAttachment, XmATTACH_FORM, 
                                             XmNrightAttachment, XmATTACH_NONE, 
                                             XmNtopOffset, 30, 
                                             XmNbottomOffset, 0, 
                                             XmNleftOffset, 140, 
                                             XmNrightOffset, 0, 
                                             XmNwidth, 35, 
                                             XmNheight, 65, 
                                             (XtPointer) NULL ); 


    _tube1_toggle = XtVaCreateManagedWidget  ( "tube1_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox5, 
                                                XmNlabelType, XmSTRING, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _tube1_toggle,
                    XmNvalueChangedCallback,
                    &WeapsUI::select_tube1Callback,
                    (XtPointer) this ); 


    _tube3_toggle = XtVaCreateManagedWidget  ( "tube3_toggle",
                                                xmToggleButtonWidgetClass,
                                                _radiobox5, 
                                                XmNlabelType, XmSTRING, 
                                                XmNset, True, 
                                                (XtPointer) NULL ); 

    XtAddCallback ( _tube3_toggle,
                    XmNvalueChangedCallback,
                    &WeapsUI::select_tube3Callback,
                    (XtPointer) this ); 


    _actual_weap_frame = XtVaCreateManagedWidget  ( "actual_weap_frame",
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


    _actual_weap_form = XtVaCreateManagedWidget  ( "actual_weap_form",
                                                    xmFormWidgetClass,
                                                    _actual_weap_frame, 
                                                    XmNresizePolicy, XmRESIZE_GROW, 
                                                    XmNwidth, 114, 
                                                    XmNheight, 254, 
                                                    (XtPointer) NULL ); 


    _actual_w_speed_textfield = XtVaCreateManagedWidget  ( "actual_w_speed_textfield",
                                                            xmTextFieldWidgetClass,
                                                            _actual_weap_form, 
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
    XtVaSetValues ( _actual_w_speed_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _actual_w_speed_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _actual_w_depth_textfield = XtVaCreateManagedWidget  ( "actual_w_depth_textfield",
                                                            xmTextFieldWidgetClass,
                                                            _actual_weap_form, 
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
    XtVaSetValues ( _actual_w_depth_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _actual_w_depth_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _actual_w_course_textfield = XtVaCreateManagedWidget  ( "actual_w_course_textfield",
                                                             xmTextFieldWidgetClass,
                                                             _actual_weap_form, 
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
    XtVaSetValues ( _actual_w_course_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _actual_w_course_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _actual_weap_label = XtVaCreateManagedWidget  ( "actual_weap_label",
                                                     xmLabelWidgetClass,
                                                     _actual_weap_form, 
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


    _actual_weap_separator = XtVaCreateManagedWidget  ( "actual_weap_separator",
                                                         xmSeparatorWidgetClass,
                                                         _actual_weap_form, 
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


    _actual_w_speed_label = XtVaCreateManagedWidget  ( "actual_w_speed_label",
                                                        xmLabelWidgetClass,
                                                        _actual_weap_form, 
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


    _actual_w_depth_label = XtVaCreateManagedWidget  ( "actual_w_depth_label",
                                                        xmLabelWidgetClass,
                                                        _actual_weap_form, 
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


    _actual_w_course_label = XtVaCreateManagedWidget  ( "actual_w_course_label",
                                                         xmLabelWidgetClass,
                                                         _actual_weap_form, 
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

const char * WeapsUI::className()
{
    return ("WeapsUI");
} // End className()




/////////////////////////////////////////////////////////////// 
// The following functions are static member functions used to 
// interface with Motif.
/////////////////////////////////// 

void WeapsUI::port_tube_shootCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->port_tube_shoot ( w, callData );
}

void WeapsUI::pt_crs_100s_addCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->pt_crs_100s_add ( w, callData );
}

void WeapsUI::pt_crs_100s_minusCallback ( Widget    w,
                                          XtPointer clientData,
                                          XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->pt_crs_100s_minus ( w, callData );
}

void WeapsUI::pt_crs_10s_addCallback ( Widget    w,
                                       XtPointer clientData,
                                       XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->pt_crs_10s_add ( w, callData );
}

void WeapsUI::pt_crs_10s_minusCallback ( Widget    w,
                                         XtPointer clientData,
                                         XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->pt_crs_10s_minus ( w, callData );
}

void WeapsUI::pt_crs_1s_addCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->pt_crs_1s_add ( w, callData );
}

void WeapsUI::pt_crs_1s_minusCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->pt_crs_1s_minus ( w, callData );
}

void WeapsUI::select_tlam1Callback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->select_tlam1 ( w, callData );
}

void WeapsUI::select_tlam2Callback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->select_tlam2 ( w, callData );
}

void WeapsUI::select_tlam3Callback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->select_tlam3 ( w, callData );
}

void WeapsUI::select_tlam4Callback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->select_tlam4 ( w, callData );
}

void WeapsUI::select_tlam5Callback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->select_tlam5 ( w, callData );
}

void WeapsUI::select_tlam6Callback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->select_tlam6 ( w, callData );
}

void WeapsUI::select_tube1Callback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->select_tube1 ( w, callData );
}

void WeapsUI::select_tube2Callback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->select_tube2 ( w, callData );
}

void WeapsUI::select_tube3Callback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->select_tube3 ( w, callData );
}

void WeapsUI::select_tube4Callback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->select_tube4 ( w, callData );
}

void WeapsUI::shoot_tlamCallback ( Widget    w,
                                   XtPointer clientData,
                                   XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->shoot_tlam ( w, callData );
}

void WeapsUI::st_crs_100s_addCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->st_crs_100s_add ( w, callData );
}

void WeapsUI::st_crs_100s_minusCallback ( Widget    w,
                                          XtPointer clientData,
                                          XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->st_crs_100s_minus ( w, callData );
}

void WeapsUI::st_crs_10s_addCallback ( Widget    w,
                                       XtPointer clientData,
                                       XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->st_crs_10s_add ( w, callData );
}

void WeapsUI::st_crs_10s_minusCallback ( Widget    w,
                                         XtPointer clientData,
                                         XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->st_crs_10s_minus ( w, callData );
}

void WeapsUI::st_crs_1s_addCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->st_crs_1s_add ( w, callData );
}

void WeapsUI::st_crs_1s_minusCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->st_crs_1s_minus ( w, callData );
}

void WeapsUI::stbd_tube_shootCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    WeapsUI* obj = ( WeapsUI * ) clientData;
    obj->stbd_tube_shoot ( w, callData );
}



/////////////////////////////////////////////////////////////// 
// The following functions are called from the menu items 
// in this window.
/////////////////////////////////// 

void WeapsUI::port_tube_shoot ( Widget, XtPointer ) 
{
    // This virtual function is called from port_tube_shootCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::pt_crs_100s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from pt_crs_100s_addCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::pt_crs_100s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from pt_crs_100s_minusCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::pt_crs_10s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from pt_crs_10s_addCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::pt_crs_10s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from pt_crs_10s_minusCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::pt_crs_1s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from pt_crs_1s_addCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::pt_crs_1s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from pt_crs_1s_minusCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::select_tlam1 ( Widget, XtPointer ) 
{
    // This virtual function is called from select_tlam1Callback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::select_tlam2 ( Widget, XtPointer ) 
{
    // This virtual function is called from select_tlam2Callback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::select_tlam3 ( Widget, XtPointer ) 
{
    // This virtual function is called from select_tlam3Callback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::select_tlam4 ( Widget, XtPointer ) 
{
    // This virtual function is called from select_tlam4Callback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::select_tlam5 ( Widget, XtPointer ) 
{
    // This virtual function is called from select_tlam5Callback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::select_tlam6 ( Widget, XtPointer ) 
{
    // This virtual function is called from select_tlam6Callback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::select_tube1 ( Widget, XtPointer ) 
{
    // This virtual function is called from select_tube1Callback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::select_tube2 ( Widget, XtPointer ) 
{
    // This virtual function is called from select_tube2Callback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::select_tube3 ( Widget, XtPointer ) 
{
    // This virtual function is called from select_tube3Callback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::select_tube4 ( Widget, XtPointer ) 
{
    // This virtual function is called from select_tube4Callback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::shoot_tlam ( Widget, XtPointer ) 
{
    // This virtual function is called from shoot_tlamCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::st_crs_100s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from st_crs_100s_addCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::st_crs_100s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from st_crs_100s_minusCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::st_crs_10s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from st_crs_10s_addCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::st_crs_10s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from st_crs_10s_minusCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::st_crs_1s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from st_crs_1s_addCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::st_crs_1s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from st_crs_1s_minusCallback.
    // This function is normally overriden by a derived class.

}

void WeapsUI::stbd_tube_shoot ( Widget, XtPointer ) 
{
    // This virtual function is called from stbd_tube_shootCallback.
    // This function is normally overriden by a derived class.

}

//---- End generated code section

