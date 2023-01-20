// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef gui_h
#define gui_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tile.H>
extern Fl_Tile* dco1_panel;
extern Fl_Tile* dco2_panel;
extern Fl_Tile* dco3_panel;
extern Fl_Tile* lfo1_panel;
extern Fl_Tile* lfo2_panel;
#include <FL/Fl_Button.H>
extern Fl_Button* legato_button;
extern Fl_Button* sync_button;
#include <FL/Fl_Value_Slider.H>
extern Fl_Value_Slider* master_volume;
extern Fl_Value_Slider* midi_channel;
#include <FL/Fl_Dial.H>
extern Fl_Dial* inertia1;
extern Fl_Value_Slider* pitch1;
extern Fl_Value_Slider* wave1;
extern Fl_Button* active1;
#include <FL/Fl_Box.H>
extern Fl_Dial* inertia2;
extern Fl_Value_Slider* pitch2;
extern Fl_Value_Slider* wave2;
extern Fl_Button* active2;
extern Fl_Dial* inertia3;
extern Fl_Value_Slider* pitch3;
extern Fl_Value_Slider* wave3;
extern Fl_Button* active3;
extern Fl_Value_Slider* freq;
extern Fl_Value_Slider* res;
extern Fl_Value_Slider* wave4;
extern Fl_Value_Slider* lfo1_dco1_pitch;
extern Fl_Value_Slider* lfo1_dco2_pitch;
extern Fl_Value_Slider* lfo1_dco3_pitch;
extern Fl_Value_Slider* lfo1_dcf;
extern Fl_Dial* lfo1speed;
extern Fl_Value_Slider* wave5;
extern Fl_Value_Slider* lfo2_dco1_pitch;
extern Fl_Value_Slider* lfo2_dco2_pitch;
extern Fl_Value_Slider* lfo2_dco3_pitch;
extern Fl_Value_Slider* lfo2_dcf;
extern Fl_Dial* lfo2speed;
extern Fl_Value_Slider* adsr_amp_amount1;
extern Fl_Value_Slider* attack1;
extern Fl_Value_Slider* decay1;
extern Fl_Value_Slider* sustain1;
extern Fl_Value_Slider* release1;
extern Fl_Value_Slider* adsr_osc2_amount1;
extern Fl_Value_Slider* attack2;
extern Fl_Value_Slider* decay2;
extern Fl_Value_Slider* sustain2;
extern Fl_Value_Slider* release2;
extern Fl_Value_Slider* adsr_filter_amount2;
extern Fl_Value_Slider* adsr_osc3_amount2;
extern Fl_Dial* octave1;
extern Fl_Dial* octave2;
extern Fl_Dial* octave3;
Fl_Double_Window* make_window();
#include <FL/Fl_Browser.H>
#include <FL/Fl_Group.H>
extern Fl_Browser* patch_browser;
#include <FL/Fl_Input.H>
extern Fl_Input* save_name;
Fl_Double_Window* patch_window();
#endif
