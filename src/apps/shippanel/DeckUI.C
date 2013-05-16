
/////////////////////////////////////////////////////////////
//
// Source file for DeckUI
//
//    This class implements the user interface created in 
//    the interface builder. Normally it is not used directly.
//    Instead the subclass, Deck is instantiated
//
//    To extend or alter the behavior of this class, you should
//    modify the Deck files
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


#include "DeckUI.h" // Generated header file for this class
#include <Sgm/Dial.h> 
#include <Xm/ArrowB.h> 
#include <Xm/BulletinB.h> 
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

String  DeckUI::_defaultDeckUIResources[] = {
        (char*)NULL
};

DeckUI::DeckUI(const char *name) : VkComponent(name) 
{ 
    // No widgets are created by this constructor.
    // If an application creates a component using this constructor,
    // It must explictly call create at a later time.
    // This is mostly useful when adding pre-widget creation
    // code to a derived class constructor.

    //---- End generated code section

} // End Constructor




DeckUI::DeckUI(const char *name, Widget parent) : VkComponent(name) 
{ 
    // Call creation function to build the widget tree.

     create(parent);

    //---- End generated code section

} // End Constructor


DeckUI::~DeckUI() 
{
    // Base class destroys widgets

    //-- End generated code section

} // End destructor



void DeckUI::create ( Widget parent )
{
    Arg      args[18];
    Cardinal count;
    count = 0;

    // Load any class-defaulted resources for this object

    setDefaultResources(parent, _defaultDeckUIResources  );


    // Create an unmanaged widget as the top of the widget hierarchy

    _baseWidget = _deck = XtVaCreateWidget ( _name,
                                             xmBulletinBoardWidgetClass,
                                             parent, 
                                             (XtPointer) NULL ); 

    // install a callback to guard against unexpected widget destruction

    installDestroyHandler();


    // Create widgets used in this component
    // All variables are data members of this class

    _ood = XtVaCreateManagedWidget  ( "ood",
                                       xmFormWidgetClass,
                                       _baseWidget, 
                                       XmNresizePolicy, XmRESIZE_GROW, 
                                       XmNx, 10, 
                                       XmNy, 10, 
                                       XmNwidth, 1270, 
                                       XmNheight, 320, 
                                       (XtPointer) NULL ); 


    _voice_cmd_textfield = XtVaCreateManagedWidget  ( "voice_cmd_textfield",
                                                       xmTextFieldWidgetClass,
                                                       _ood, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 50, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 930, 
                                                       XmNrightOffset, 0, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _voice_cmd_textfield,
                    XmNvalueChangedCallback,
                    &DeckUI::process_voice_cmdCallback,
                    (XtPointer) this ); 


    _port_ood_label = XtVaCreateManagedWidget  ( "port_ood_label",
                                                  xmLabelWidgetClass,
                                                  _ood, 
                                                  XmNlabelType, XmSTRING, 
                                                  XmNtopAttachment, XmATTACH_FORM, 
                                                  XmNbottomAttachment, XmATTACH_FORM, 
                                                  XmNleftAttachment, XmATTACH_FORM, 
                                                  XmNrightAttachment, XmATTACH_FORM, 
                                                  XmNbottomPosition, 0, 
                                                  XmNrightPosition, 0, 
                                                  XmNtopOffset, 293, 
                                                  XmNbottomOffset, 11, 
                                                  XmNleftOffset, 59, 
                                                  XmNrightOffset, 1178, 
                                                  XmNwidth, 33, 
                                                  XmNheight, 16, 
                                                  (XtPointer) NULL ); 


    _stbd_ood_label = XtVaCreateManagedWidget  ( "stbd_ood_label",
                                                  xmLabelWidgetClass,
                                                  _ood, 
                                                  XmNlabelType, XmSTRING, 
                                                  XmNtopAttachment, XmATTACH_FORM, 
                                                  XmNbottomAttachment, XmATTACH_FORM, 
                                                  XmNleftAttachment, XmATTACH_FORM, 
                                                  XmNrightAttachment, XmATTACH_FORM, 
                                                  XmNbottomPosition, 0, 
                                                  XmNrightPosition, 0, 
                                                  XmNtopOffset, 292, 
                                                  XmNbottomOffset, 12, 
                                                  XmNleftOffset, 346, 
                                                  XmNrightOffset, 892, 
                                                  XmNwidth, 32, 
                                                  XmNheight, 16, 
                                                  (XtPointer) NULL ); 


    _actual_O_course_label1 = XtVaCreateManagedWidget  ( "actual_O_course_label1",
                                                          xmLabelWidgetClass,
                                                          _ood, 
                                                          XmNlabelType, XmSTRING, 
                                                          XmNtopAttachment, XmATTACH_FORM, 
                                                          XmNbottomAttachment, XmATTACH_NONE, 
                                                          XmNleftAttachment, XmATTACH_FORM, 
                                                          XmNrightAttachment, XmATTACH_FORM, 
                                                          XmNrightPosition, 0, 
                                                          XmNtopOffset, 18, 
                                                          XmNbottomOffset, 0, 
                                                          XmNleftOffset, 920, 
                                                          XmNrightOffset, 144, 
                                                          XmNwidth, 206, 
                                                          XmNheight, 20, 
                                                          (XtPointer) NULL ); 


    _actual_rudder_textfield = XtVaCreateManagedWidget  ( "actual_rudder_textfield",
                                                           xmTextFieldWidgetClass,
                                                           _ood, 
                                                           XmNeditable, False, 
                                                           XmNtopAttachment, XmATTACH_FORM, 
                                                           XmNbottomAttachment, XmATTACH_NONE, 
                                                           XmNleftAttachment, XmATTACH_FORM, 
                                                           XmNrightAttachment, XmATTACH_NONE, 
                                                           XmNtopOffset, 238, 
                                                           XmNbottomOffset, 0, 
                                                           XmNleftOffset, 516, 
                                                           XmNrightOffset, 0, 
                                                           XmNwidth, 76, 
                                                           XmNheight, 29, 
                                                           (XtPointer) NULL ); 
    XtVaSetValues ( _actual_rudder_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _actual_rudder_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _rudder_label = XtVaCreateManagedWidget  ( "rudder_label",
                                                xmLabelWidgetClass,
                                                _ood, 
                                                XmNlabelType, XmSTRING, 
                                                XmNtopAttachment, XmATTACH_FORM, 
                                                XmNbottomAttachment, XmATTACH_NONE, 
                                                XmNleftAttachment, XmATTACH_FORM, 
                                                XmNrightAttachment, XmATTACH_NONE, 
                                                XmNtopOffset, 55, 
                                                XmNbottomOffset, 0, 
                                                XmNleftOffset, 510, 
                                                XmNrightOffset, 0, 
                                                XmNwidth, 94, 
                                                XmNheight, 30, 
                                                (XtPointer) NULL ); 


    _rudder_dial = XtVaCreateManagedWidget  ( "rudder_dial",
                                               sgDialWidgetClass,
                                               _ood, 
                                               SgNdialMarkers, 16, 
                                               XmNminimum, 0, 
                                               XmNmaximum, 359, 
                                               SgNstartAngle, 0, 
                                               SgNangleRange, 360, 
                                               XmNvalue, 0, 
                                               SgNdialVisual, SgKNOB, 
                                               XmNtopAttachment, XmATTACH_FORM, 
                                               XmNbottomAttachment, XmATTACH_FORM, 
                                               XmNleftAttachment, XmATTACH_FORM, 
                                               XmNrightAttachment, XmATTACH_NONE, 
                                               XmNbottomPosition, 0, 
                                               XmNtopOffset, 50, 
                                               XmNbottomOffset, 89, 
                                               XmNleftOffset, 460, 
                                               XmNrightOffset, 0, 
                                               XmNwidth, 190, 
                                               XmNheight, 181, 
                                               (XtPointer) NULL ); 

    XtAddCallback ( _rudder_dial,
                    XmNvalueChangedCallback,
                    &DeckUI::rudder_angleCallback,
                    (XtPointer) this ); 


    _speed_ood_frame1 = XtVaCreateManagedWidget  ( "speed_ood_frame1",
                                                    xmFrameWidgetClass,
                                                    _ood, 
                                                    XmNtopAttachment, XmATTACH_FORM, 
                                                    XmNbottomAttachment, XmATTACH_FORM, 
                                                    XmNleftAttachment, XmATTACH_FORM, 
                                                    XmNrightAttachment, XmATTACH_FORM, 
                                                    XmNbottomPosition, 0, 
                                                    XmNrightPosition, 0, 
                                                    XmNtopOffset, 0, 
                                                    XmNbottomOffset, 30, 
                                                    XmNleftOffset, 305, 
                                                    XmNrightOffset, 845, 
                                                    XmNwidth, 120, 
                                                    XmNheight, 290, 
                                                    (XtPointer) NULL ); 


    _rowcolumn2 = XtVaCreateManagedWidget  ( "rowcolumn2",
                                              xmRowColumnWidgetClass,
                                              _speed_ood_frame1, 
                                              XmNwidth, 114, 
                                              XmNheight, 284, 
                                              (XtPointer) NULL ); 


    _radiobox2 = XtVaCreateManagedWidget  ( "radiobox2",
                                             xmRowColumnWidgetClass,
                                             _rowcolumn2, 
                                             XmNpacking, XmPACK_COLUMN, 
                                             XmNradioBehavior, True, 
                                             XmNradioAlwaysOne, True, 
                                             XmNwidth, 108, 
                                             XmNheight, 282, 
                                             (XtPointer) NULL ); 


    _o_flank_toggle1 = XtVaCreateManagedWidget  ( "o_flank_toggle1",
                                                   xmToggleButtonWidgetClass,
                                                   _radiobox2, 
                                                   XmNlabelType, XmSTRING, 
                                                   (XtPointer) NULL ); 

    XtAddCallback ( _o_flank_toggle1,
                    XmNvalueChangedCallback,
                    &DeckUI::s_flank_orderCallback,
                    (XtPointer) this ); 


    _o_full_toggle1 = XtVaCreateManagedWidget  ( "o_full_toggle1",
                                                  xmToggleButtonWidgetClass,
                                                  _radiobox2, 
                                                  XmNlabelType, XmSTRING, 
                                                  (XtPointer) NULL ); 

    XtAddCallback ( _o_full_toggle1,
                    XmNvalueChangedCallback,
                    &DeckUI::s_full_orderCallback,
                    (XtPointer) this ); 


    _o_stnd_toggle1 = XtVaCreateManagedWidget  ( "o_stnd_toggle1",
                                                  xmToggleButtonWidgetClass,
                                                  _radiobox2, 
                                                  XmNlabelType, XmSTRING, 
                                                  (XtPointer) NULL ); 

    XtAddCallback ( _o_stnd_toggle1,
                    XmNvalueChangedCallback,
                    &DeckUI::s_stnd_orderCallback,
                    (XtPointer) this ); 


    _o_two_third_toggle1 = XtVaCreateManagedWidget  ( "o_two_third_toggle1",
                                                       xmToggleButtonWidgetClass,
                                                       _radiobox2, 
                                                       XmNlabelType, XmSTRING, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _o_two_third_toggle1,
                    XmNvalueChangedCallback,
                    &DeckUI::s_two_third_orderCallback,
                    (XtPointer) this ); 


    _o_one_third_toggle1 = XtVaCreateManagedWidget  ( "o_one_third_toggle1",
                                                       xmToggleButtonWidgetClass,
                                                       _radiobox2, 
                                                       XmNlabelType, XmSTRING, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _o_one_third_toggle1,
                    XmNvalueChangedCallback,
                    &DeckUI::s_one_third_orderCallback,
                    (XtPointer) this ); 


    _o_stop_toggle1 = XtVaCreateManagedWidget  ( "o_stop_toggle1",
                                                  xmToggleButtonWidgetClass,
                                                  _radiobox2, 
                                                  XmNlabelType, XmSTRING, 
                                                  XmNset, True, 
                                                  (XtPointer) NULL ); 

    XtAddCallback ( _o_stop_toggle1,
                    XmNvalueChangedCallback,
                    &DeckUI::s_stop_orderCallback,
                    (XtPointer) this ); 


    _o_back_one_third_toggle1 = XtVaCreateManagedWidget  ( "o_back_one_third_toggle1",
                                                            xmToggleButtonWidgetClass,
                                                            _radiobox2, 
                                                            XmNlabelType, XmSTRING, 
                                                            (XtPointer) NULL ); 

    XtAddCallback ( _o_back_one_third_toggle1,
                    XmNvalueChangedCallback,
                    &DeckUI::s_back_one_third_orderCallback,
                    (XtPointer) this ); 


    _o_back_two_third_toggle1 = XtVaCreateManagedWidget  ( "o_back_two_third_toggle1",
                                                            xmToggleButtonWidgetClass,
                                                            _radiobox2, 
                                                            XmNlabelType, XmSTRING, 
                                                            (XtPointer) NULL ); 

    XtAddCallback ( _o_back_two_third_toggle1,
                    XmNvalueChangedCallback,
                    &DeckUI::s_back_two_third_orderCallback,
                    (XtPointer) this ); 


    _o_backfull_toggle1 = XtVaCreateManagedWidget  ( "o_backfull_toggle1",
                                                      xmToggleButtonWidgetClass,
                                                      _radiobox2, 
                                                      XmNlabelType, XmSTRING, 
                                                      (XtPointer) NULL ); 

    XtAddCallback ( _o_backfull_toggle1,
                    XmNvalueChangedCallback,
                    &DeckUI::s_back_full_orderCallback,
                    (XtPointer) this ); 


    _rpm_ood_frame = XtVaCreateManagedWidget  ( "rpm_ood_frame",
                                                 xmFrameWidgetClass,
                                                 _ood, 
                                                 XmNtopAttachment, XmATTACH_FORM, 
                                                 XmNbottomAttachment, XmATTACH_NONE, 
                                                 XmNleftAttachment, XmATTACH_FORM, 
                                                 XmNrightAttachment, XmATTACH_FORM, 
                                                 XmNrightPosition, 0, 
                                                 XmNtopOffset, 20, 
                                                 XmNbottomOffset, 0, 
                                                 XmNleftOffset, 150, 
                                                 XmNrightOffset, 974, 
                                                 XmNwidth, 146, 
                                                 XmNheight, 160, 
                                                 (XtPointer) NULL ); 


    _rpm_ood_form = XtVaCreateManagedWidget  ( "rpm_ood_form",
                                                xmFormWidgetClass,
                                                _rpm_ood_frame, 
                                                XmNresizePolicy, XmRESIZE_GROW, 
                                                XmNwidth, 140, 
                                                XmNheight, 154, 
                                                (XtPointer) NULL ); 


    _rpm_ood_label = XtVaCreateManagedWidget  ( "rpm_ood_label",
                                                 xmLabelWidgetClass,
                                                 _rpm_ood_form, 
                                                 XmNlabelType, XmSTRING, 
                                                 XmNtopAttachment, XmATTACH_FORM, 
                                                 XmNbottomAttachment, XmATTACH_NONE, 
                                                 XmNleftAttachment, XmATTACH_FORM, 
                                                 XmNrightAttachment, XmATTACH_NONE, 
                                                 XmNtopOffset, 20, 
                                                 XmNbottomOffset, 0, 
                                                 XmNleftOffset, 49, 
                                                 XmNrightOffset, 0, 
                                                 XmNwidth, 40, 
                                                 XmNheight, 20, 
                                                 (XtPointer) NULL ); 


    _o_rpm_1s_textfield = XtVaCreateManagedWidget  ( "o_rpm_1s_textfield",
                                                      xmTextFieldWidgetClass,
                                                      _rpm_ood_form, 
                                                      XmNcolumns, 1, 
                                                      XmNeditable, False, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 80, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 90, 
                                                      XmNrightOffset, 0, 
                                                      XmNheight, 33, 
                                                      (XtPointer) NULL ); 
    XtVaSetValues ( _o_rpm_1s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _o_rpm_1s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _o_rpm_10s_textfield = XtVaCreateManagedWidget  ( "o_rpm_10s_textfield",
                                                       xmTextFieldWidgetClass,
                                                       _rpm_ood_form, 
                                                       XmNcolumns, 1, 
                                                       XmNeditable, False, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 80, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 50, 
                                                       XmNrightOffset, 0, 
                                                       XmNheight, 33, 
                                                       (XtPointer) NULL ); 
    XtVaSetValues ( _o_rpm_10s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _o_rpm_10s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _o_rpm_100s_textfield = XtVaCreateManagedWidget  ( "o_rpm_100s_textfield",
                                                        xmTextFieldWidgetClass,
                                                        _rpm_ood_form, 
                                                        XmNcolumns, 1, 
                                                        XmNeditable, False, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_NONE, 
                                                        XmNtopOffset, 80, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 10, 
                                                        XmNrightOffset, 0, 
                                                        XmNheight, 33, 
                                                        (XtPointer) NULL ); 
    XtVaSetValues ( _o_rpm_100s_textfield, 
                    XmNbackground, 
                    (Pixel) VkGetResource ( _o_rpm_100s_textfield,
                                            "readOnlyBackground",
                                            "ReadOnlyBackground",
                                            XmRPixel, "Black" ),
                    NULL );

    _o_rpm_1s_dwn_arrow = XtVaCreateManagedWidget  ( "o_rpm_1s_dwn_arrow",
                                                      xmArrowButtonWidgetClass,
                                                      _rpm_ood_form, 
                                                      XmNarrowDirection, XmARROW_DOWN, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 110, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 90, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 30, 
                                                      XmNheight, 30, 
                                                      (XtPointer) NULL ); 

    XtAddCallback ( _o_rpm_1s_dwn_arrow,
                    XmNactivateCallback,
                    &DeckUI::o_rpm_1s_minusCallback,
                    (XtPointer) this ); 


    _o_rpm_10s_dwn_arrow = XtVaCreateManagedWidget  ( "o_rpm_10s_dwn_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _rpm_ood_form, 
                                                       XmNarrowDirection, XmARROW_DOWN, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 110, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 50, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _o_rpm_10s_dwn_arrow,
                    XmNactivateCallback,
                    &DeckUI::o_rpm_10s_minusCallback,
                    (XtPointer) this ); 


    _o_rpm_100s_dwn_arrow = XtVaCreateManagedWidget  ( "o_rpm_100s_dwn_arrow",
                                                        xmArrowButtonWidgetClass,
                                                        _rpm_ood_form, 
                                                        XmNarrowDirection, XmARROW_DOWN, 
                                                        XmNtopAttachment, XmATTACH_FORM, 
                                                        XmNbottomAttachment, XmATTACH_NONE, 
                                                        XmNleftAttachment, XmATTACH_FORM, 
                                                        XmNrightAttachment, XmATTACH_NONE, 
                                                        XmNtopOffset, 110, 
                                                        XmNbottomOffset, 0, 
                                                        XmNleftOffset, 10, 
                                                        XmNrightOffset, 0, 
                                                        XmNwidth, 30, 
                                                        XmNheight, 30, 
                                                        (XtPointer) NULL ); 

    XtAddCallback ( _o_rpm_100s_dwn_arrow,
                    XmNactivateCallback,
                    &DeckUI::o_rpm_100s_minusCallback,
                    (XtPointer) this ); 


    _o_rpm_1s_up_arrow = XtVaCreateManagedWidget  ( "o_rpm_1s_up_arrow",
                                                     xmArrowButtonWidgetClass,
                                                     _rpm_ood_form, 
                                                     XmNtopAttachment, XmATTACH_FORM, 
                                                     XmNbottomAttachment, XmATTACH_NONE, 
                                                     XmNleftAttachment, XmATTACH_FORM, 
                                                     XmNrightAttachment, XmATTACH_NONE, 
                                                     XmNtopOffset, 50, 
                                                     XmNbottomOffset, 0, 
                                                     XmNleftOffset, 90, 
                                                     XmNrightOffset, 0, 
                                                     XmNwidth, 30, 
                                                     XmNheight, 30, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _o_rpm_1s_up_arrow,
                    XmNactivateCallback,
                    &DeckUI::o_rpm_1s_addCallback,
                    (XtPointer) this ); 


    _o_rpm_10s_up_arrow = XtVaCreateManagedWidget  ( "o_rpm_10s_up_arrow",
                                                      xmArrowButtonWidgetClass,
                                                      _rpm_ood_form, 
                                                      XmNtopAttachment, XmATTACH_FORM, 
                                                      XmNbottomAttachment, XmATTACH_NONE, 
                                                      XmNleftAttachment, XmATTACH_FORM, 
                                                      XmNrightAttachment, XmATTACH_NONE, 
                                                      XmNtopOffset, 50, 
                                                      XmNbottomOffset, 0, 
                                                      XmNleftOffset, 50, 
                                                      XmNrightOffset, 0, 
                                                      XmNwidth, 30, 
                                                      XmNheight, 30, 
                                                      (XtPointer) NULL ); 

    XtAddCallback ( _o_rpm_10s_up_arrow,
                    XmNactivateCallback,
                    &DeckUI::o_rpm_10s_addCallback,
                    (XtPointer) this ); 


    _o_rpm_100s_up_arrow = XtVaCreateManagedWidget  ( "o_rpm_100s_up_arrow",
                                                       xmArrowButtonWidgetClass,
                                                       _rpm_ood_form, 
                                                       XmNtopAttachment, XmATTACH_FORM, 
                                                       XmNbottomAttachment, XmATTACH_NONE, 
                                                       XmNleftAttachment, XmATTACH_FORM, 
                                                       XmNrightAttachment, XmATTACH_NONE, 
                                                       XmNtopOffset, 50, 
                                                       XmNbottomOffset, 0, 
                                                       XmNleftOffset, 10, 
                                                       XmNrightOffset, 0, 
                                                       XmNwidth, 30, 
                                                       XmNheight, 30, 
                                                       (XtPointer) NULL ); 

    XtAddCallback ( _o_rpm_100s_up_arrow,
                    XmNactivateCallback,
                    &DeckUI::o_rpm_100s_addCallback,
                    (XtPointer) this ); 


    _ood_order_label = XtVaCreateManagedWidget  ( "ood_order_label",
                                                   xmLabelWidgetClass,
                                                   _ood, 
                                                   XmNlabelType, XmSTRING, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_NONE, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_NONE, 
                                                   XmNtopOffset, 180, 
                                                   XmNbottomOffset, 0, 
                                                   XmNleftOffset, 170, 
                                                   XmNrightOffset, 0, 
                                                   XmNwidth, 100, 
                                                   XmNheight, 30, 
                                                   (XtPointer) NULL ); 


    _speed_ood_frame = XtVaCreateManagedWidget  ( "speed_ood_frame",
                                                   xmFrameWidgetClass,
                                                   _ood, 
                                                   XmNtopAttachment, XmATTACH_FORM, 
                                                   XmNbottomAttachment, XmATTACH_FORM, 
                                                   XmNleftAttachment, XmATTACH_FORM, 
                                                   XmNrightAttachment, XmATTACH_FORM, 
                                                   XmNbottomPosition, 0, 
                                                   XmNrightPosition, 0, 
                                                   XmNtopOffset, 0, 
                                                   XmNbottomOffset, 30, 
                                                   XmNleftOffset, 20, 
                                                   XmNrightOffset, 1130, 
                                                   XmNwidth, 120, 
                                                   XmNheight, 290, 
                                                   (XtPointer) NULL ); 


    _rowcolumn1 = XtVaCreateManagedWidget  ( "rowcolumn1",
                                              xmRowColumnWidgetClass,
                                              _speed_ood_frame, 
                                              XmNwidth, 114, 
                                              XmNheight, 284, 
                                              (XtPointer) NULL ); 


    _radiobox1 = XtVaCreateManagedWidget  ( "radiobox1",
                                             xmRowColumnWidgetClass,
                                             _rowcolumn1, 
                                             XmNpacking, XmPACK_COLUMN, 
                                             XmNradioBehavior, True, 
                                             XmNradioAlwaysOne, True, 
                                             XmNwidth, 108, 
                                             XmNheight, 282, 
                                             (XtPointer) NULL ); 


    _o_flank_toggle = XtVaCreateManagedWidget  ( "o_flank_toggle",
                                                  xmToggleButtonWidgetClass,
                                                  _radiobox1, 
                                                  XmNlabelType, XmSTRING, 
                                                  (XtPointer) NULL ); 

    XtAddCallback ( _o_flank_toggle,
                    XmNvalueChangedCallback,
                    &DeckUI::flank_orderCallback,
                    (XtPointer) this ); 


    _o_full_toggle = XtVaCreateManagedWidget  ( "o_full_toggle",
                                                 xmToggleButtonWidgetClass,
                                                 _radiobox1, 
                                                 XmNlabelType, XmSTRING, 
                                                 (XtPointer) NULL ); 

    XtAddCallback ( _o_full_toggle,
                    XmNvalueChangedCallback,
                    &DeckUI::full_orderCallback,
                    (XtPointer) this ); 


    _o_stnd_toggle = XtVaCreateManagedWidget  ( "o_stnd_toggle",
                                                 xmToggleButtonWidgetClass,
                                                 _radiobox1, 
                                                 XmNlabelType, XmSTRING, 
                                                 (XtPointer) NULL ); 

    XtAddCallback ( _o_stnd_toggle,
                    XmNvalueChangedCallback,
                    &DeckUI::stnd_orderCallback,
                    (XtPointer) this ); 


    _o_two_third_toggle = XtVaCreateManagedWidget  ( "o_two_third_toggle",
                                                      xmToggleButtonWidgetClass,
                                                      _radiobox1, 
                                                      XmNlabelType, XmSTRING, 
                                                      (XtPointer) NULL ); 

    XtAddCallback ( _o_two_third_toggle,
                    XmNvalueChangedCallback,
                    &DeckUI::two_third_orderCallback,
                    (XtPointer) this ); 


    _o_one_third_toggle = XtVaCreateManagedWidget  ( "o_one_third_toggle",
                                                      xmToggleButtonWidgetClass,
                                                      _radiobox1, 
                                                      XmNlabelType, XmSTRING, 
                                                      (XtPointer) NULL ); 

    XtAddCallback ( _o_one_third_toggle,
                    XmNvalueChangedCallback,
                    &DeckUI::one_third_orderCallback,
                    (XtPointer) this ); 


    _o_stop_toggle = XtVaCreateManagedWidget  ( "o_stop_toggle",
                                                 xmToggleButtonWidgetClass,
                                                 _radiobox1, 
                                                 XmNlabelType, XmSTRING, 
                                                 XmNset, True, 
                                                 (XtPointer) NULL ); 

    XtAddCallback ( _o_stop_toggle,
                    XmNvalueChangedCallback,
                    &DeckUI::stop_orderCallback,
                    (XtPointer) this ); 


    _o_back_one_third_toggle = XtVaCreateManagedWidget  ( "o_back_one_third_toggle",
                                                           xmToggleButtonWidgetClass,
                                                           _radiobox1, 
                                                           XmNlabelType, XmSTRING, 
                                                           (XtPointer) NULL ); 

    XtAddCallback ( _o_back_one_third_toggle,
                    XmNvalueChangedCallback,
                    &DeckUI::back_one_third_orderCallback,
                    (XtPointer) this ); 


    _o_back_two_third_toggle = XtVaCreateManagedWidget  ( "o_back_two_third_toggle",
                                                           xmToggleButtonWidgetClass,
                                                           _radiobox1, 
                                                           XmNlabelType, XmSTRING, 
                                                           (XtPointer) NULL ); 

    XtAddCallback ( _o_back_two_third_toggle,
                    XmNvalueChangedCallback,
                    &DeckUI::back_two_third_orderCallback,
                    (XtPointer) this ); 


    _o_backfull_toggle = XtVaCreateManagedWidget  ( "o_backfull_toggle",
                                                     xmToggleButtonWidgetClass,
                                                     _radiobox1, 
                                                     XmNlabelType, XmSTRING, 
                                                     (XtPointer) NULL ); 

    XtAddCallback ( _o_backfull_toggle,
                    XmNvalueChangedCallback,
                    &DeckUI::back_full_orderCallback,
                    (XtPointer) this ); 


    _actual_OOD_frame = XtVaCreateManagedWidget  ( "actual_OOD_frame",
                                                    xmFrameWidgetClass,
                                                    _ood, 
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


    _compass_dial = XtVaCreateManagedWidget  ( "compass_dial",
                                                sgDialWidgetClass,
                                                _actual_OOD_form, 
                                                SgNdialVisual, SgPOINTER, 
                                                XmNtopAttachment, XmATTACH_FORM, 
                                                XmNbottomAttachment, XmATTACH_NONE, 
                                                XmNleftAttachment, XmATTACH_FORM, 
                                                XmNrightAttachment, XmATTACH_NONE, 
                                                XmNtopOffset, 50, 
                                                XmNbottomOffset, 0, 
                                                XmNleftOffset, 7, 
                                                XmNrightOffset, 0, 
                                                XmNwidth, 100, 
                                                XmNheight, 100, 
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
                                                            XmNtopOffset, 179, 
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

    _actual_O_course_textfield = XtVaCreateManagedWidget  ( "actual_O_course_textfield",
                                                             xmTextFieldWidgetClass,
                                                             _actual_OOD_form, 
                                                             XmNeditable, False, 
                                                             XmNtopAttachment, XmATTACH_FORM, 
                                                             XmNbottomAttachment, XmATTACH_NONE, 
                                                             XmNleftAttachment, XmATTACH_FORM, 
                                                             XmNrightAttachment, XmATTACH_FORM, 
                                                             XmNrightPosition, 0, 
                                                             XmNtopOffset, 17, 
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
                                                    XmNtopOffset, 223, 
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
                                                        XmNtopOffset, 204, 
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
                                                        XmNtopOffset, 160, 
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
                                                         XmNtopOffset, 0, 
                                                         XmNbottomOffset, 0, 
                                                         XmNleftOffset, 0, 
                                                         XmNrightOffset, 0, 
                                                         XmNwidth, 114, 
                                                         XmNheight, 20, 
                                                         (XtPointer) NULL ); 


}

const char * DeckUI::className()
{
    return ("DeckUI");
} // End className()




/////////////////////////////////////////////////////////////// 
// The following functions are static member functions used to 
// interface with Motif.
/////////////////////////////////// 

void DeckUI::back_full_orderCallback ( Widget    w,
                                       XtPointer clientData,
                                       XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->back_full_order ( w, callData );
}

void DeckUI::back_one_third_orderCallback ( Widget    w,
                                            XtPointer clientData,
                                            XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->back_one_third_order ( w, callData );
}

void DeckUI::back_two_third_orderCallback ( Widget    w,
                                            XtPointer clientData,
                                            XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->back_two_third_order ( w, callData );
}

void DeckUI::flank_orderCallback ( Widget    w,
                                   XtPointer clientData,
                                   XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->flank_order ( w, callData );
}

void DeckUI::full_orderCallback ( Widget    w,
                                  XtPointer clientData,
                                  XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->full_order ( w, callData );
}

void DeckUI::o_rpm_100s_addCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->o_rpm_100s_add ( w, callData );
}

void DeckUI::o_rpm_100s_minusCallback ( Widget    w,
                                        XtPointer clientData,
                                        XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->o_rpm_100s_minus ( w, callData );
}

void DeckUI::o_rpm_10s_addCallback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->o_rpm_10s_add ( w, callData );
}

void DeckUI::o_rpm_10s_minusCallback ( Widget    w,
                                       XtPointer clientData,
                                       XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->o_rpm_10s_minus ( w, callData );
}

void DeckUI::o_rpm_1s_addCallback ( Widget    w,
                                    XtPointer clientData,
                                    XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->o_rpm_1s_add ( w, callData );
}

void DeckUI::o_rpm_1s_minusCallback ( Widget    w,
                                      XtPointer clientData,
                                      XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->o_rpm_1s_minus ( w, callData );
}

void DeckUI::one_third_orderCallback ( Widget    w,
                                       XtPointer clientData,
                                       XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->one_third_order ( w, callData );
}

void DeckUI::process_voice_cmdCallback ( Widget    w,
                                         XtPointer clientData,
                                         XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->process_voice_cmd ( w, callData );
}

void DeckUI::rudder_angleCallback ( Widget    w,
                                    XtPointer clientData,
                                    XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->rudder_angle ( w, callData );
}

void DeckUI::s_back_full_orderCallback ( Widget    w,
                                         XtPointer clientData,
                                         XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->s_back_full_order ( w, callData );
}

void DeckUI::s_back_one_third_orderCallback ( Widget    w,
                                              XtPointer clientData,
                                              XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->s_back_one_third_order ( w, callData );
}

void DeckUI::s_back_two_third_orderCallback ( Widget    w,
                                              XtPointer clientData,
                                              XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->s_back_two_third_order ( w, callData );
}

void DeckUI::s_flank_orderCallback ( Widget    w,
                                     XtPointer clientData,
                                     XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->s_flank_order ( w, callData );
}

void DeckUI::s_full_orderCallback ( Widget    w,
                                    XtPointer clientData,
                                    XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->s_full_order ( w, callData );
}

void DeckUI::s_one_third_orderCallback ( Widget    w,
                                         XtPointer clientData,
                                         XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->s_one_third_order ( w, callData );
}

void DeckUI::s_stnd_orderCallback ( Widget    w,
                                    XtPointer clientData,
                                    XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->s_stnd_order ( w, callData );
}

void DeckUI::s_stop_orderCallback ( Widget    w,
                                    XtPointer clientData,
                                    XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->s_stop_order ( w, callData );
}

void DeckUI::s_two_third_orderCallback ( Widget    w,
                                         XtPointer clientData,
                                         XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->s_two_third_order ( w, callData );
}

void DeckUI::stnd_orderCallback ( Widget    w,
                                  XtPointer clientData,
                                  XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->stnd_order ( w, callData );
}

void DeckUI::stop_orderCallback ( Widget    w,
                                  XtPointer clientData,
                                  XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->stop_order ( w, callData );
}

void DeckUI::two_third_orderCallback ( Widget    w,
                                       XtPointer clientData,
                                       XtPointer callData ) 
{ 
    DeckUI* obj = ( DeckUI * ) clientData;
    obj->two_third_order ( w, callData );
}



/////////////////////////////////////////////////////////////// 
// The following functions are called from the menu items 
// in this window.
/////////////////////////////////// 

void DeckUI::back_full_order ( Widget, XtPointer ) 
{
    // This virtual function is called from back_full_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::back_one_third_order ( Widget, XtPointer ) 
{
    // This virtual function is called from back_one_third_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::back_two_third_order ( Widget, XtPointer ) 
{
    // This virtual function is called from back_two_third_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::flank_order ( Widget, XtPointer ) 
{
    // This virtual function is called from flank_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::full_order ( Widget, XtPointer ) 
{
    // This virtual function is called from full_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::o_rpm_100s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from o_rpm_100s_addCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::o_rpm_100s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from o_rpm_100s_minusCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::o_rpm_10s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from o_rpm_10s_addCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::o_rpm_10s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from o_rpm_10s_minusCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::o_rpm_1s_add ( Widget, XtPointer ) 
{
    // This virtual function is called from o_rpm_1s_addCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::o_rpm_1s_minus ( Widget, XtPointer ) 
{
    // This virtual function is called from o_rpm_1s_minusCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::one_third_order ( Widget, XtPointer ) 
{
    // This virtual function is called from one_third_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::process_voice_cmd ( Widget, XtPointer ) 
{
    // This virtual function is called from process_voice_cmdCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::rudder_angle ( Widget, XtPointer ) 
{
    // This virtual function is called from rudder_angleCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::s_back_full_order ( Widget, XtPointer ) 
{
    // This virtual function is called from s_back_full_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::s_back_one_third_order ( Widget, XtPointer ) 
{
    // This virtual function is called from s_back_one_third_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::s_back_two_third_order ( Widget, XtPointer ) 
{
    // This virtual function is called from s_back_two_third_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::s_flank_order ( Widget, XtPointer ) 
{
    // This virtual function is called from s_flank_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::s_full_order ( Widget, XtPointer ) 
{
    // This virtual function is called from s_full_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::s_one_third_order ( Widget, XtPointer ) 
{
    // This virtual function is called from s_one_third_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::s_stnd_order ( Widget, XtPointer ) 
{
    // This virtual function is called from s_stnd_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::s_stop_order ( Widget, XtPointer ) 
{
    // This virtual function is called from s_stop_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::s_two_third_order ( Widget, XtPointer ) 
{
    // This virtual function is called from s_two_third_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::stnd_order ( Widget, XtPointer ) 
{
    // This virtual function is called from stnd_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::stop_order ( Widget, XtPointer ) 
{
    // This virtual function is called from stop_orderCallback.
    // This function is normally overriden by a derived class.

}

void DeckUI::two_third_order ( Widget, XtPointer ) 
{
    // This virtual function is called from two_third_orderCallback.
    // This function is normally overriden by a derived class.

}

//---- End generated code section

