
//////////////////////////////////////////////////////////////
//
// Header file for WeapsUI
//
//    This file is generated by RapidApp
//
//    This class implements the user interface potion of a class
//    the interface builder. Normally it is not used directly.
//    Instead the subclass, Weaps is instantiated
//
//    To extend or alter the behavior of this class, you should
//    modify the Weaps files
//
//    If you do modify this class, limit your changes to adding
//    members below the "//--- End generated code section" markers
//
//    This will allow the builder to integrate changes more easily
//
//    This class is a ViewKit user interface "component".
//    For more information on how components are used, see the
//    "ViewKit Programmers' Manual", and the RapidApp
//    User's Guide.
//
//////////////////////////////////////////////////////////////
#ifndef WEAPSUI_H
#define WEAPSUI_H
#include <Vk/VkComponent.h>


//---- End generated headers

class WeapsUI : public VkComponent
{ 

  public:

    WeapsUI(const char *, Widget);
    WeapsUI(const char *);
    ~WeapsUI();
    void create ( Widget );
    const char *  className();

    //--- End generated code section

  protected:


    // Widgets created by this class

    Widget  _actual_w_course_label;
    Widget  _actual_w_course_textfield;
    Widget  _actual_w_depth_label;
    Widget  _actual_w_depth_textfield;
    Widget  _actual_w_speed_label;
    Widget  _actual_w_speed_textfield;
    Widget  _actual_weap_form;
    Widget  _actual_weap_frame;
    Widget  _actual_weap_label;
    Widget  _actual_weap_separator;
    Widget  _form;
    Widget  _pt_course_100s_textfield;
    Widget  _pt_course_10s_textfield;
    Widget  _pt_course_1s_textfield;
    Widget  _pt_crs_100s_dwn_arrow0;
    Widget  _pt_crs_100s_up_arrow;
    Widget  _pt_crs_10s_dwn_arrow1;
    Widget  _pt_crs_10s_up_arrow;
    Widget  _pt_crs_1s_dwn_arrow;
    Widget  _pt_crs_1s_up_arrow;
    Widget  _radiobox5;
    Widget  _radiobox6;
    Widget  _radiobox7;
    Widget  _ready_label;
    Widget  _ready_label1;
    Widget  _select_label;
    Widget  _select_label1;
    Widget  _separator1;
    Widget  _separator2;
    Widget  _shoot_port_button;
    Widget  _shoot_stbd_button;
    Widget  _shoot_tlam_button;
    Widget  _st_course_100s_textfield;
    Widget  _st_course_10s_textfield;
    Widget  _st_course_1s_textfield;
    Widget  _st_crs_100s_dwn_arrow;
    Widget  _st_crs_100s_up_arrow;
    Widget  _st_crs_10s_dwn_arrow;
    Widget  _st_crs_10s_up_arrow;
    Widget  _st_crs_1s_dwn_arrow;
    Widget  _st_crs_1s_up_arrow;
    Widget  _tlam1_toggle;
    Widget  _tlam2_toggle;
    Widget  _tlam3_toggle;
    Widget  _tlam4_toggle;
    Widget  _tlam5_toggle;
    Widget  _tlam6_toggle;
    Widget  _tomahawk_frame;
    Widget  _torp_form;
    Widget  _torp_frame;
    Widget  _torpedo_label;
    Widget  _torpedo_label1;
    Widget  _tube1_label;
    Widget  _tube1_ready_toggle;
    Widget  _tube1_toggle;
    Widget  _tube2_label;
    Widget  _tube2_ready_toggle;
    Widget  _tube2_toggle;
    Widget  _tube3_label;
    Widget  _tube3_ready_toggle;
    Widget  _tube3_toggle;
    Widget  _tube4_label;
    Widget  _tube4_ready_toggle;
    Widget  _tube4_toggle;
    Widget  _weaps;




    // These virtual functions are called from the private callbacks (above)
    // Intended to be overriden in derived classes to define actions

    virtual void port_tube_shoot ( Widget, XtPointer );
    virtual void pt_crs_100s_add ( Widget, XtPointer );
    virtual void pt_crs_100s_minus ( Widget, XtPointer );
    virtual void pt_crs_10s_add ( Widget, XtPointer );
    virtual void pt_crs_10s_minus ( Widget, XtPointer );
    virtual void pt_crs_1s_add ( Widget, XtPointer );
    virtual void pt_crs_1s_minus ( Widget, XtPointer );
    virtual void select_tlam1 ( Widget, XtPointer );
    virtual void select_tlam2 ( Widget, XtPointer );
    virtual void select_tlam3 ( Widget, XtPointer );
    virtual void select_tlam4 ( Widget, XtPointer );
    virtual void select_tlam5 ( Widget, XtPointer );
    virtual void select_tlam6 ( Widget, XtPointer );
    virtual void select_tube1 ( Widget, XtPointer );
    virtual void select_tube2 ( Widget, XtPointer );
    virtual void select_tube3 ( Widget, XtPointer );
    virtual void select_tube4 ( Widget, XtPointer );
    virtual void shoot_tlam ( Widget, XtPointer );
    virtual void st_crs_100s_add ( Widget, XtPointer );
    virtual void st_crs_100s_minus ( Widget, XtPointer );
    virtual void st_crs_10s_add ( Widget, XtPointer );
    virtual void st_crs_10s_minus ( Widget, XtPointer );
    virtual void st_crs_1s_add ( Widget, XtPointer );
    virtual void st_crs_1s_minus ( Widget, XtPointer );
    virtual void stbd_tube_shoot ( Widget, XtPointer );

    //--- End generated code section

  private: 
    // Array of default resources

    static String      _defaultWeapsUIResources[];

    // Callbacks to interface with Motif

    static void port_tube_shootCallback ( Widget, XtPointer, XtPointer );
    static void pt_crs_100s_addCallback ( Widget, XtPointer, XtPointer );
    static void pt_crs_100s_minusCallback ( Widget, XtPointer, XtPointer );
    static void pt_crs_10s_addCallback ( Widget, XtPointer, XtPointer );
    static void pt_crs_10s_minusCallback ( Widget, XtPointer, XtPointer );
    static void pt_crs_1s_addCallback ( Widget, XtPointer, XtPointer );
    static void pt_crs_1s_minusCallback ( Widget, XtPointer, XtPointer );
    static void select_tlam1Callback ( Widget, XtPointer, XtPointer );
    static void select_tlam2Callback ( Widget, XtPointer, XtPointer );
    static void select_tlam3Callback ( Widget, XtPointer, XtPointer );
    static void select_tlam4Callback ( Widget, XtPointer, XtPointer );
    static void select_tlam5Callback ( Widget, XtPointer, XtPointer );
    static void select_tlam6Callback ( Widget, XtPointer, XtPointer );
    static void select_tube1Callback ( Widget, XtPointer, XtPointer );
    static void select_tube2Callback ( Widget, XtPointer, XtPointer );
    static void select_tube3Callback ( Widget, XtPointer, XtPointer );
    static void select_tube4Callback ( Widget, XtPointer, XtPointer );
    static void shoot_tlamCallback ( Widget, XtPointer, XtPointer );
    static void st_crs_100s_addCallback ( Widget, XtPointer, XtPointer );
    static void st_crs_100s_minusCallback ( Widget, XtPointer, XtPointer );
    static void st_crs_10s_addCallback ( Widget, XtPointer, XtPointer );
    static void st_crs_10s_minusCallback ( Widget, XtPointer, XtPointer );
    static void st_crs_1s_addCallback ( Widget, XtPointer, XtPointer );
    static void st_crs_1s_minusCallback ( Widget, XtPointer, XtPointer );
    static void stbd_tube_shootCallback ( Widget, XtPointer, XtPointer );

    //--- End generated code section

};
#endif

