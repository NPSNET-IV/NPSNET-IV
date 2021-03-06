
/////////////////////////////////////////////////////////////
//
// Source file for Ood
//
//    This file is generated by RapidApp
//
//    This class is derived from OodUI which 
//    implements the user interface created in 
//    the interface builder. This class contains virtual
//    functions that are called from the user interface.
//
//    When you modify this source, limit your changes to 
//    modifying the emtpy virtual functions. You can also add
//    new functions below the "//--- End generated code section" markers
//
//    This will allow the builder to integrate changes more easily
//
//    This class is a ViewKit user interface "component".
//    For more information on how components are used, see the
//    "ViewKit Programmers' Manual", and the RapidApp
//    User's Guide.
/////////////////////////////////////////////////////////////

#include "global.h"
#include "Ood.h"
#include <Vk/VkEZ.h>
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

#include <Vk/VkFormat.h> 
#include <stdlib.h>
#include <iostream.h>

extern void VkUnimplemented(Widget, const char *);

///////////////////////////////////////////////////////////////////////////////
// The following non-container widgets are created by OodUI and are
// available as protected data members inherited by this class
//
//  XmLabel		    _scope_main_label
//  XmToggleButton		    _scope_up_toggle
//  XmToggleButton		    _scope_down_toggle
//  XmLabel		    _periscope_label
//  XmTextField		    _scope_by_textfield
//  XmLabel		    _bearing_label
//  XmTextField		    _scope_range_textfield
//  XmLabel		    _range_label
//  XmArrowButton		    _scope_right_arrow
//  XmArrowButton		    _scope_down_arrow
//  XmArrowButton		    _scope_left_arrow
//  XmArrowButton		    _scope_up_arrow
//  XmPushButton		    _scope_mark_button
//  XmLabel		    _mbt_label
//  XmLabel		    _embt_label
//  XmToggleButton		    _embt_on_toggle
//  XmToggleButton		    _embt_off_toggle
//  XmToggleButton		    _mbts_open_toggle
//  XmToggleButton		    _mbts_shut_toggle
//  XmSeparator		    _separator
//  XmTextField		    _o_dpth_1s_textfield
//  XmArrowButton		    _o_dpth_1s_dwn_arrow
//  XmArrowButton		    _o_dpth_1s_up_arrow
//  XmLabel		    _depth_ood_label
//  XmTextField		    _o_dpth_10s_textfield
//  XmTextField		    _o_dpth_100s_textfield
//  XmTextField		    _o_dpth_1000s_textfield
//  XmArrowButton		    _o_dpth_10s_dwn_arrow
//  XmArrowButton		    _o_dpth_100s_dwn_arrow
//  XmArrowButton		    _o_dpth_1000s_dwn_arrow
//  XmArrowButton		    _o_dpth_10s_up_arrow
//  XmArrowButton		    _o_dpth_100s_up_arrow
//  XmArrowButton		    _o_dpth_1000s_up_arrow
//  XmLabel		    _course_ood_label
//  XmTextField		    _o_course_1s_textfield
//  XmTextField		    _o_course_10s_textfield
//  XmTextField		    _o_course_100s_textfield
//  XmArrowButton		    _o_crse_1s_dwn_arrow
//  XmArrowButton		    _o_crse_10s_dwn_arrow
//  XmArrowButton		    _o_crse_100s_dwn_arrow
//  XmArrowButton		    _o_crse_1s_up_arrow
//  XmArrowButton		    _o_crse_10s_up_arrow
//  XmArrowButton		    _o_crs_100s_up_arrow
//  XmLabel		    _ood_order_label
//  XmToggleButton		    _o_flank_toggle
//  XmToggleButton		    _o_full_toggle
//  XmToggleButton		    _o_stnd_toggle
//  XmToggleButton		    _o_half_toggle
//  XmToggleButton		    _o_stop_toggle
//  XmToggleButton		    _o_backhalf_toggle
//  XmToggleButton		    _o_backstnd_toggle
//  XmToggleButton		    _o_backfull_toggle
//  XmTextField		    _actual_O_speed_textfield
//  XmTextField		    _actual_O_depth_textfield
//  XmTextField		    _actual_O_course_textfield
//  XmLabel		    _actual_OOD_label
//  XmSeparator		    _actual_OOD_separator
//  XmLabel		    _actual_O_speed_label
//  XmLabel		    _actual_O_depth_label
//  XmLabel		    _actual_O_course_label
//
///////////////////////////////////////////////////////////////////////////////


//---- End generated headers

extern NPSNETToSubIDU      *updatesubidu;
extern HelmToNPSNETIDU      helmidu;
extern SubOodToNPSNETIDU    oodidu;
extern WeapsToNPSNETIDU     weapsidu;
extern IDU_net_manager     *net;

//---- Ood Constructor

Ood::Ood(const char *name, Widget parent) : 
                   OodUI(name, parent) 
{ 
    // This constructor is called after the component's interface has been built.

    //--- Add application code here:


} // End Constructor



Ood::Ood(const char *name) : 
                   OodUI(name) 
 { 
    // This constructor calls OodUI(name)
    // which does not create any widgets. Usually, this
    // constructor is not used

    //--- Add application code here:


} // End Constructor



Ood::~Ood()
{
    // The base class destructors are responsible for
    // destroying all widgets and objects used in this component.
    // Only additional items created directly in this class
    // need to be freed here.

    //--- Add application destructor code here:


}


const char * Ood::className() // classname
{
    return ("Ood");
} // End className()

void Ood::update() 
{
    //actual depth,course,speed
    XmTextFieldSetString(_actual_O_depth_textfield,
                          (char *) VkFormat("%d",(int)updatesubidu->sub_depth));
    XmTextFieldSetString(_actual_O_course_textfield,
                         (char *) VkFormat("%d", (int)updatesubidu->sub_course));
    XmTextFieldSetString(_actual_O_speed_textfield,
                         (char *) VkFormat("%d", (int)updatesubidu->sub_speed));

    //periscope raise lower
    EZ(_scope_down_toggle) = !((int)updatesubidu->sub_scope_raiselower);
    EZ(_scope_up_toggle) =   (int)updatesubidu->sub_scope_raiselower;
    oodidu.ood_scope_raiselower = updatesubidu->sub_scope_raiselower;

    //perisope bearing and range
    XmTextFieldSetString(_scope_by_textfield,
                          (char *) VkFormat("%d",(int)updatesubidu->sub_scope_by));
    XmTextFieldSetString(_scope_range_textfield,
                         (char *) VkFormat("%d", (int)updatesubidu->sub_scope_rh));

    //embt blow
    EZ(_embt_off_toggle) = !((int)updatesubidu->sub_embtblow_onoff);
    EZ(_embt_on_toggle) =  (int)updatesubidu->sub_embtblow_onoff;
    oodidu.ood_embtblow_onoff = updatesubidu->sub_embtblow_onoff;

    //mbt vents
    EZ(_mbts_open_toggle) = !((int)updatesubidu->sub_mbtvents_openshut);
    EZ(_mbts_shut_toggle) =  (int)updatesubidu->sub_mbtvents_openshut;
    oodidu.ood_mbtvents_openshut = updatesubidu->sub_mbtvents_openshut;
    
} // End update()

void Ood::back_full_order ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::back_full_order is implemented:

    //::VkUnimplemented ( w, "Ood::back_full_order" );


    //--- Add application code for Ood::back_full_order here:
    oodidu.ood_speed = -3;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::back_full_order()


void Ood::back_half_order ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::back_half_order is implemented:

    //::VkUnimplemented ( w, "Ood::back_half_order" );


    //--- Add application code for Ood::back_half_order here:
    oodidu.ood_speed = -1;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::back_half_order()


void Ood::back_stnd_order ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::back_stnd_order is implemented:

    //::VkUnimplemented ( w, "Ood::back_stnd_order" );


    //--- Add application code for Ood::back_stnd_order here:
    oodidu.ood_speed = -2;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::back_stnd_order()


void Ood::embt_blow_off ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::embt_blow_off is implemented:

    //::VkUnimplemented ( w, "Ood::embt_blow_off" );


    //--- Add application code for Ood::embt_blow_off here:
    oodidu.ood_embtblow_onoff = 0;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::embt_blow_off()


void Ood::embt_blow_on ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::embt_blow_on is implemented:

    //::VkUnimplemented ( w, "Ood::embt_blow_on" );


    //--- Add application code for Ood::embt_blow_on here:
    oodidu.ood_embtblow_onoff = 1;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::embt_blow_on()


void Ood::flank_order ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::flank_order is implemented:

    //::VkUnimplemented ( w, "Ood::flank_order" );


    //--- Add application code for Ood::flank_order here:
    oodidu.ood_speed = 4;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::flank_order()


void Ood::full_order ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::full_order is implemented:

    //::VkUnimplemented ( w, "Ood::full_order" );


    //--- Add application code for Ood::full_order here:
    oodidu.ood_speed = 3;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::full_order()


void Ood::half_order ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::half_order is implemented:

    //::VkUnimplemented ( w, "Ood::half_order" );


    //--- Add application code for Ood::half_order here:
    oodidu.ood_speed = 1;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::half_order()


void Ood::mbt_vents_open ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::mbt_vents_open is implemented:

    //::VkUnimplemented ( w, "Ood::mbt_vents_open" );


    //--- Add application code for Ood::mbt_vents_open here:
    oodidu.ood_mbtvents_openshut = 1;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::mbt_vents_open()


void Ood::mbt_vents_shut ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::mbt_vents_shut is implemented:

    //::VkUnimplemented ( w, "Ood::mbt_vents_shut" );


    //--- Add application code for Ood::mbt_vents_shut here:
    oodidu.ood_mbtvents_openshut = 0;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::mbt_vents_shut()



void Ood::o_crs_100s_add ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_crs_100s_add is implemented:

   // ::VkUnimplemented ( w, "Ood::o_crs_100s_add" );


    //--- Add application code for Ood::o_crs_100s_add here:
    int a = atoi (XmTextFieldGetString(_o_course_100s_textfield));
    int b = atoi (XmTextFieldGetString(_o_course_10s_textfield));
    int c = atoi (XmTextFieldGetString(_o_course_1s_textfield));
    if ( a == 3 )
       a = -1;
    if ( (a+1) == 3 )
      {
       if ( b >= 6 )
         {
           XmTextFieldSetString(_o_course_10s_textfield, (char *) VkFormat("%d", 0));
           XmTextFieldSetString(_o_course_1s_textfield, (char *) VkFormat("%d", 0));
           b = 0;
           c = 0;
         }
      }
    XmTextFieldSetString(_o_course_100s_textfield, (char *) VkFormat("%d", a+1));
    oodidu.ood_course = ((a+1)*100) + (b*10) + c;
    cerr<<oodidu.ood_course<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_crs_100s_add()


void Ood::o_crse_100s_minus ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_crse_100s_minus is implemented:

    //::VkUnimplemented ( w, "Ood::o_crse_100s_minus" );


    //--- Add application code for Ood::o_crse_100s_minus here:
    int a = atoi (XmTextFieldGetString(_o_course_100s_textfield));
    int b = atoi (XmTextFieldGetString(_o_course_10s_textfield));
    int c = atoi (XmTextFieldGetString(_o_course_1s_textfield));
    if ( a == 0 )
      {
       a = 4;
       if ( b >= 6 )
         {
           XmTextFieldSetString(_o_course_10s_textfield, (char *) VkFormat("%d", 0));
           XmTextFieldSetString(_o_course_1s_textfield, (char *) VkFormat("%d", 0));
           b = 0;
           c = 0;
         }
      }
    XmTextFieldSetString(_o_course_100s_textfield, (char *) VkFormat("%d", a-1));
    oodidu.ood_course = ((a-1)*100) + (b*10) + c;
    cerr<<oodidu.ood_course<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_crse_100s_minus()


void Ood::o_crse_10s_add ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_crse_10s_add is implemented:

    //::VkUnimplemented ( w, "Ood::o_crse_10s_add" );


    //--- Add application code for Ood::o_crse_10s_add here:
    int a = atoi (XmTextFieldGetString(_o_course_100s_textfield));
    int b = atoi (XmTextFieldGetString(_o_course_10s_textfield));
    int c = atoi (XmTextFieldGetString(_o_course_1s_textfield));
    if (a < 3)
      {if ( b == 9 )
         b = -1;
      }
    else
      {if ( b >=5 )
         b = -1;
      }
    XmTextFieldSetString(_o_course_10s_textfield, (char *) VkFormat("%d", b+1));
    oodidu.ood_course = (a*100) + ((b+1)*10) + c;
    cerr<<oodidu.ood_course<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_crse_10s_add()


void Ood::o_crse_10s_minus ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_crse_10s_minus is implemented:

    //::VkUnimplemented ( w, "Ood::o_crse_10s_minus" );


    //--- Add application code for Ood::o_crse_10s_minus here:
    int a = atoi (XmTextFieldGetString(_o_course_100s_textfield));
    int b = atoi (XmTextFieldGetString(_o_course_10s_textfield));
    int c = atoi (XmTextFieldGetString(_o_course_1s_textfield));
    if ((atoi (XmTextFieldGetString(_o_course_100s_textfield))) < 3)
      {if ( b == 0 )
         b = 10;
      }
    else
      {if ( b == 0 )
         b = 6;
       if ( b > 6 )
         b = 6;
      }
    XmTextFieldSetString(_o_course_10s_textfield, (char *) VkFormat("%d", b-1));
    oodidu.ood_course = (a*100) + ((b-1)*10) + c;
    cerr<<oodidu.ood_course<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_crse_10s_minus()


void Ood::o_crse_1s_add ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_crse_1s_add is implemented:

   // ::VkUnimplemented ( w, "Ood::o_crse_1s_add" );


    //--- Add application code for Ood::o_crse_1s_add here:
    int a = atoi (XmTextFieldGetString(_o_course_100s_textfield));
    int b = atoi (XmTextFieldGetString(_o_course_10s_textfield));
    int c = atoi (XmTextFieldGetString(_o_course_1s_textfield));
    if ( c == 9 )
      c = -1;
    XmTextFieldSetString(_o_course_1s_textfield, (char *) VkFormat("%d", c+1));
    oodidu.ood_course = (a*100) + (b*10) + c+1;
    cerr<<oodidu.ood_course<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_crse_1s_add()


void Ood::o_crse_1s_minus ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_crse_1s_minus is implemented:

    //::VkUnimplemented ( w, "Ood::o_crse_1s_minus" );


    //--- Add application code for Ood::o_crse_1s_minus here:
    int a = atoi (XmTextFieldGetString(_o_course_100s_textfield));
    int b = atoi (XmTextFieldGetString(_o_course_10s_textfield));
    int c = atoi (XmTextFieldGetString(_o_course_1s_textfield));
    if ( c == 0 )
      c = 10;
    XmTextFieldSetString(_o_course_1s_textfield, (char *) VkFormat("%d", c-1));
    oodidu.ood_course = (a*100) + (b*10) + c-1;
    cerr<<oodidu.ood_course<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_crse_1s_minus()


void Ood::o_dpth_1000s_add ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_dpth_1000s_add is implemented:

    //::VkUnimplemented ( w, "Ood::o_dpth_1000s_add" );


    //--- Add application code for Ood::o_dpth_1000s_add here:
    int a = atoi (XmTextFieldGetString(_o_dpth_1000s_textfield));
    int b = atoi (XmTextFieldGetString(_o_dpth_100s_textfield));
    int c = atoi (XmTextFieldGetString(_o_dpth_10s_textfield));
    int d = atoi (XmTextFieldGetString(_o_dpth_1s_textfield));
    if ( a == 9 )
      a = -1;
    XmTextFieldSetString(_o_dpth_1000s_textfield, (char *) VkFormat("%d", a+1));
    oodidu.ood_depth = ((a+1)*1000) + (b*100) + (c*10) + d;
    cerr<<oodidu.ood_depth<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_dpth_1000s_add()


void Ood::o_dpth_1000s_minus ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_dpth_1000s_minus is implemented:

    //::VkUnimplemented ( w, "Ood::o_dpth_1000s_minus" );


    //--- Add application code for Ood::o_dpth_1000s_minus here:
    int a = atoi (XmTextFieldGetString(_o_dpth_1000s_textfield));
    int b = atoi (XmTextFieldGetString(_o_dpth_100s_textfield));
    int c = atoi (XmTextFieldGetString(_o_dpth_10s_textfield));
    int d = atoi (XmTextFieldGetString(_o_dpth_1s_textfield));
    if ( a == 0 )
      a = 10;
    XmTextFieldSetString(_o_dpth_1000s_textfield, (char *) VkFormat("%d", a-1));
    oodidu.ood_depth = ((a-1)*1000) + (b*100) + (c*10) + d;
    cerr<<oodidu.ood_depth<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_dpth_1000s_minus()


void Ood::o_dpth_100s_add ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_dpth_100s_add is implemented:

    //::VkUnimplemented ( w, "Ood::o_dpth_100s_add" );


    //--- Add application code for Ood::o_dpth_100s_add here:
    int a = atoi (XmTextFieldGetString(_o_dpth_1000s_textfield));
    int b = atoi (XmTextFieldGetString(_o_dpth_100s_textfield));
    int c = atoi (XmTextFieldGetString(_o_dpth_10s_textfield));
    int d = atoi (XmTextFieldGetString(_o_dpth_1s_textfield));
    if ( b == 9 )
      b = -1;
    XmTextFieldSetString(_o_dpth_100s_textfield, (char *) VkFormat("%d", b+1));
    oodidu.ood_depth = (a*1000) + ((b+1)*100) + (c*10) + d;
    cerr<<oodidu.ood_depth<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_dpth_100s_add()


void Ood::o_dpth_100s_minus ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_dpth_100s_minus is implemented:

    //::VkUnimplemented ( w, "Ood::o_dpth_100s_minus" );


    //--- Add application code for Ood::o_dpth_100s_minus here:
    int a = atoi (XmTextFieldGetString(_o_dpth_1000s_textfield));
    int b = atoi (XmTextFieldGetString(_o_dpth_100s_textfield));
    int c = atoi (XmTextFieldGetString(_o_dpth_10s_textfield));
    int d = atoi (XmTextFieldGetString(_o_dpth_1s_textfield));
    if ( b == 0 )
      b = 10;
    XmTextFieldSetString(_o_dpth_100s_textfield, (char *) VkFormat("%d", b-1));
    oodidu.ood_depth = (a*1000) + ((b-1)*100) + (c*10) + d;
    cerr<<oodidu.ood_depth<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_dpth_100s_minus()


void Ood::o_dpth_10s_add ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_dpth_10s_add is implemented:

    //::VkUnimplemented ( w, "Ood::o_dpth_10s_add" );


    //--- Add application code for Ood::o_dpth_10s_add here:
    int a = atoi (XmTextFieldGetString(_o_dpth_1000s_textfield));
    int b = atoi (XmTextFieldGetString(_o_dpth_100s_textfield));
    int c = atoi (XmTextFieldGetString(_o_dpth_10s_textfield));
    int d = atoi (XmTextFieldGetString(_o_dpth_1s_textfield));
    if ( c == 9 )
      c = -1;
    XmTextFieldSetString(_o_dpth_10s_textfield, (char *) VkFormat("%d", c+1));
    oodidu.ood_depth = (a*1000) + (b*100) + ((c+1)*10) + d;
    cerr<<oodidu.ood_depth<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_dpth_10s_add()


void Ood::o_dpth_10s_minus ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_dpth_10s_minus is implemented:

    //::VkUnimplemented ( w, "Ood::o_dpth_10s_minus" );


    //--- Add application code for Ood::o_dpth_10s_minus here:
    int a = atoi (XmTextFieldGetString(_o_dpth_1000s_textfield));
    int b = atoi (XmTextFieldGetString(_o_dpth_100s_textfield));
    int c = atoi (XmTextFieldGetString(_o_dpth_10s_textfield));
    int d = atoi (XmTextFieldGetString(_o_dpth_1s_textfield));
    if ( c == 0 )
      c = 10;
    XmTextFieldSetString(_o_dpth_10s_textfield, (char *) VkFormat("%d", c-1));
    oodidu.ood_depth = (a*1000) + (b*100) + ((c-1)*10) + d;
    cerr<<oodidu.ood_depth<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_dpth_10s_minus()


void Ood::o_dpth_1s_add ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_dpth_1s_add is implemented:

    //::VkUnimplemented ( w, "Ood::o_dpth_1s_add" );


    //--- Add application code for Ood::o_dpth_1s_add here:
    int a = atoi (XmTextFieldGetString(_o_dpth_1000s_textfield));
    int b = atoi (XmTextFieldGetString(_o_dpth_100s_textfield));
    int c = atoi (XmTextFieldGetString(_o_dpth_10s_textfield));
    int d = atoi (XmTextFieldGetString(_o_dpth_1s_textfield));
    if ( d == 9 )
      d = -1;
    XmTextFieldSetString(_o_dpth_1s_textfield, (char *) VkFormat("%d", d+1));
    oodidu.ood_depth = (a*1000) + (b*100) + (c*10) + d+1;
    cerr<<oodidu.ood_depth<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_dpth_1s_add()


void Ood::o_dpth_1s_minus ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::o_dpth_1s_minus is implemented:

    //::VkUnimplemented ( w, "Ood::o_dpth_1s_minus" );


    //--- Add application code for Ood::o_dpth_1s_minus here:
    int a = atoi (XmTextFieldGetString(_o_dpth_1000s_textfield));
    int b = atoi (XmTextFieldGetString(_o_dpth_100s_textfield));
    int c = atoi (XmTextFieldGetString(_o_dpth_10s_textfield));
    int d = atoi (XmTextFieldGetString(_o_dpth_1s_textfield));
    if ( d == 0 )
      d = 10;
    XmTextFieldSetString(_o_dpth_1s_textfield, (char *) VkFormat("%d", d-1));
    oodidu.ood_depth = (a*1000) + (b*100) + (c*10) + d-1;
    cerr<<oodidu.ood_depth<<endl;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::o_dpth_1s_minus()


void Ood::scope_lower ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::scope_lower is implemented:

    //::VkUnimplemented ( w, "Ood::scope_lower" );


    //--- Add application code for Ood::scope_lower here:
    oodidu.ood_scope_raiselower = 0;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::scope_lower()


void Ood::scope_mark ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::scope_mark is implemented:

    //::VkUnimplemented ( w, "Ood::scope_mark" );


    //--- Add application code for Ood::scope_mark here:
    oodidu.ood_scope_mark = 1;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
    oodidu.ood_scope_mark = 0; //reset 
} // End Ood::scope_mark()


void Ood::scope_raise ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::scope_raise is implemented:

    //::VkUnimplemented ( w, "Ood::scope_raise" );


    //--- Add application code for Ood::scope_raise here:
    oodidu.ood_scope_raiselower = 1;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::scope_raise()


void Ood::scope_view_down ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::scope_view_down is implemented:

    //::VkUnimplemented ( w, "Ood::scope_view_down" );


    //--- Add application code for Ood::scope_view_down here:
    oodidu.ood_scope_updown = -1;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
    oodidu.ood_scope_updown = 0; //reset 
} // End Ood::scope_view_down()


void Ood::scope_view_left ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::scope_view_left is implemented:

    //::VkUnimplemented ( w, "Ood::scope_view_left" );


    //--- Add application code for Ood::scope_view_left here:
    oodidu.ood_scope_leftright = 1;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
    oodidu.ood_scope_leftright = 0; //reset 
} // End Ood::scope_view_left()


void Ood::scope_view_right ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::scope_view_right is implemented:

    //::VkUnimplemented ( w, "Ood::scope_view_right" );


    //--- Add application code for Ood::scope_view_right here:
    oodidu.ood_scope_leftright = -1;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
    oodidu.ood_scope_leftright = 0; //reset 
} // End Ood::scope_view_right()


void Ood::scope_view_up ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::scope_view_up is implemented:

    //::VkUnimplemented ( w, "Ood::scope_view_up" );


    //--- Add application code for Ood::scope_view_up here:
    oodidu.ood_scope_updown = 1;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
    oodidu.ood_scope_updown = 0; //reset 
} // End Ood::scope_view_up()


void Ood::stnd_order ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::stnd_order is implemented:

    //::VkUnimplemented ( w, "Ood::stnd_order" );


    //--- Add application code for Ood::stnd_order here:
    oodidu.ood_speed = 2;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::stnd_order()


void Ood::stop_order ( Widget w, XtPointer callData )
{
    XmAnyCallbackStruct *cbs = (XmAnyCallbackStruct*) callData;

    //--- Comment out the following line when Ood::stop_order is implemented:

    //::VkUnimplemented ( w, "Ood::stop_order" );


    //--- Add application code for Ood::stop_order here:
    oodidu.ood_speed = 0;
    if ( !(net->write_idu((char *)&oodidu,SUB_Ood_To_NPSNET_Type)) )
             cerr << "ERROR:\tCould not send OOD IDU." << endl;
} // End Ood::stop_order()




//---- End generated code section

