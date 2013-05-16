
//////////////////////////////////////////////////////////////
//
// Header file for DeckUI
//
//    This file is generated by RapidApp
//
//    This class implements the user interface potion of a class
//    the interface builder. Normally it is not used directly.
//    Instead the subclass, Deck is instantiated
//
//    To extend or alter the behavior of this class, you should
//    modify the Deck files
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
#ifndef DECKUI_H
#define DECKUI_H
#include <Vk/VkComponent.h>


//---- End generated headers

class DeckUI : public VkComponent
{ 

  public:

    DeckUI(const char *, Widget);
    DeckUI(const char *);
    ~DeckUI();
    void create ( Widget );
    const char *  className();

    //--- End generated code section

  protected:


    // Widgets created by this class

    Widget  _actual_OOD_form;
    Widget  _actual_OOD_frame;
    Widget  _actual_OOD_label;
    Widget  _actual_OOD_separator;
    Widget  _actual_O_course_label;
    Widget  _actual_O_course_label1;
    Widget  _actual_O_course_textfield;
    Widget  _actual_O_speed_label;
    Widget  _actual_O_speed_textfield;
    Widget  _actual_rudder_textfield;
    Widget  _compass_dial;
    Widget  _deck;
    Widget  _o_back_one_third_toggle;
    Widget  _o_back_one_third_toggle1;
    Widget  _o_back_two_third_toggle;
    Widget  _o_back_two_third_toggle1;
    Widget  _o_backfull_toggle;
    Widget  _o_backfull_toggle1;
    Widget  _o_flank_toggle;
    Widget  _o_flank_toggle1;
    Widget  _o_full_toggle;
    Widget  _o_full_toggle1;
    Widget  _o_one_third_toggle;
    Widget  _o_one_third_toggle1;
    Widget  _o_rpm_100s_dwn_arrow;
    Widget  _o_rpm_100s_textfield;
    Widget  _o_rpm_100s_up_arrow;
    Widget  _o_rpm_10s_dwn_arrow;
    Widget  _o_rpm_10s_textfield;
    Widget  _o_rpm_10s_up_arrow;
    Widget  _o_rpm_1s_dwn_arrow;
    Widget  _o_rpm_1s_textfield;
    Widget  _o_rpm_1s_up_arrow;
    Widget  _o_stnd_toggle;
    Widget  _o_stnd_toggle1;
    Widget  _o_stop_toggle;
    Widget  _o_stop_toggle1;
    Widget  _o_two_third_toggle;
    Widget  _o_two_third_toggle1;
    Widget  _ood;
    Widget  _ood_order_label;
    Widget  _port_ood_label;
    Widget  _radiobox1;
    Widget  _radiobox2;
    Widget  _rowcolumn1;
    Widget  _rowcolumn2;
    Widget  _rpm_ood_form;
    Widget  _rpm_ood_frame;
    Widget  _rpm_ood_label;
    Widget  _rudder_dial;
    Widget  _rudder_label;
    Widget  _speed_ood_frame;
    Widget  _speed_ood_frame1;
    Widget  _stbd_ood_label;
    Widget  _voice_cmd_textfield;




    // These virtual functions are called from the private callbacks (above)
    // Intended to be overriden in derived classes to define actions

    virtual void back_full_order ( Widget, XtPointer );
    virtual void back_one_third_order ( Widget, XtPointer );
    virtual void back_two_third_order ( Widget, XtPointer );
    virtual void flank_order ( Widget, XtPointer );
    virtual void full_order ( Widget, XtPointer );
    virtual void o_rpm_100s_add ( Widget, XtPointer );
    virtual void o_rpm_100s_minus ( Widget, XtPointer );
    virtual void o_rpm_10s_add ( Widget, XtPointer );
    virtual void o_rpm_10s_minus ( Widget, XtPointer );
    virtual void o_rpm_1s_add ( Widget, XtPointer );
    virtual void o_rpm_1s_minus ( Widget, XtPointer );
    virtual void one_third_order ( Widget, XtPointer );
    virtual void process_voice_cmd ( Widget, XtPointer );
    virtual void rudder_angle ( Widget, XtPointer );
    virtual void s_back_full_order ( Widget, XtPointer );
    virtual void s_back_one_third_order ( Widget, XtPointer );
    virtual void s_back_two_third_order ( Widget, XtPointer );
    virtual void s_flank_order ( Widget, XtPointer );
    virtual void s_full_order ( Widget, XtPointer );
    virtual void s_one_third_order ( Widget, XtPointer );
    virtual void s_stnd_order ( Widget, XtPointer );
    virtual void s_stop_order ( Widget, XtPointer );
    virtual void s_two_third_order ( Widget, XtPointer );
    virtual void stnd_order ( Widget, XtPointer );
    virtual void stop_order ( Widget, XtPointer );
    virtual void two_third_order ( Widget, XtPointer );

    //--- End generated code section

  private: 
    // Array of default resources

    static String      _defaultDeckUIResources[];

    // Callbacks to interface with Motif

    static void back_full_orderCallback ( Widget, XtPointer, XtPointer );
    static void back_one_third_orderCallback ( Widget, XtPointer, XtPointer );
    static void back_two_third_orderCallback ( Widget, XtPointer, XtPointer );
    static void flank_orderCallback ( Widget, XtPointer, XtPointer );
    static void full_orderCallback ( Widget, XtPointer, XtPointer );
    static void o_rpm_100s_addCallback ( Widget, XtPointer, XtPointer );
    static void o_rpm_100s_minusCallback ( Widget, XtPointer, XtPointer );
    static void o_rpm_10s_addCallback ( Widget, XtPointer, XtPointer );
    static void o_rpm_10s_minusCallback ( Widget, XtPointer, XtPointer );
    static void o_rpm_1s_addCallback ( Widget, XtPointer, XtPointer );
    static void o_rpm_1s_minusCallback ( Widget, XtPointer, XtPointer );
    static void one_third_orderCallback ( Widget, XtPointer, XtPointer );
    static void process_voice_cmdCallback ( Widget, XtPointer, XtPointer );
    static void rudder_angleCallback ( Widget, XtPointer, XtPointer );
    static void s_back_full_orderCallback ( Widget, XtPointer, XtPointer );
    static void s_back_one_third_orderCallback ( Widget, XtPointer, XtPointer );
    static void s_back_two_third_orderCallback ( Widget, XtPointer, XtPointer );
    static void s_flank_orderCallback ( Widget, XtPointer, XtPointer );
    static void s_full_orderCallback ( Widget, XtPointer, XtPointer );
    static void s_one_third_orderCallback ( Widget, XtPointer, XtPointer );
    static void s_stnd_orderCallback ( Widget, XtPointer, XtPointer );
    static void s_stop_orderCallback ( Widget, XtPointer, XtPointer );
    static void s_two_third_orderCallback ( Widget, XtPointer, XtPointer );
    static void stnd_orderCallback ( Widget, XtPointer, XtPointer );
    static void stop_orderCallback ( Widget, XtPointer, XtPointer );
    static void two_third_orderCallback ( Widget, XtPointer, XtPointer );

    //--- End generated code section

};
#endif

