
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <gtkmm.h>
#include <lv2gui.hpp>
#include "knob.h"
#include "toggle.h"
#include "minaton.peg"
#include <iostream>
#include <lv2.h>
#include <gtkmm/image.h>
#include <gtkmm/box.h>
#include <gtkmm/window.h>
#include <gtkmm/widget.h>
#include "synth.hpp"

using namespace sigc;
using namespace std;
using namespace Gtk;

//------------------------------------------------------------------------------------------

class gui_dco
{
	public:
	Table* gui_panel;
	toggle* gui_active;
	Label* gui_label;
	Knob* gui_inertia;
	Knob* gui_octave;
	HScale* gui_finetune;
	toggle* gui_finetune_centre;
	HScale* gui_wave;

	slot<void> slot_active;
	slot<void> slot_finetune_centre;

	void add(Glib::RefPtr< Gdk::Pixbuf >,string,int,int,int,int,int,int);

	float active_cb();
	float octave_cb();
	float finetune_cb();
	float finetune_centre_cb();
	float wave_cb();
	float inertia_cb();
	string bundle_path;

	LV2UI_Controller controller;
	LV2UI_Write_Function write_function;

	void write_control(uint32_t port_index, float value);

	private:
};

//------------------------------------------

class gui_lfo
{
	public:

	void add(Glib::RefPtr< Gdk::Pixbuf >,int,int,int,int,int,int);

	Fixed* gui_panel;
	HScale* gui_wave;
	Knob* gui_speed;
	VScale* gui_dco1;
	VScale* gui_dco2;	
	VScale* gui_dco3;
	VScale* gui_dcf;

	LV2UI_Controller controller;
	LV2UI_Write_Function write_function;

	void write_control(uint32_t port_index, float value);
};

//------------------------------------------

class gui_dca
{
	public:

	Fixed* gui_panel;
	VScale* gui_attack;
	VScale* gui_decay;
	VScale* gui_sustain;
	VScale* gui_release;	
	VScale* gui_amount1;
	VScale* gui_amount2;

	void add(Glib::RefPtr< Gdk::Pixbuf >,string,int,int,int,int,int,int);

	LV2UI_Controller controller;
	LV2UI_Write_Function write_function;

	void write_control(uint32_t port_index, float value);

};

//------------------------------------------
using namespace sigc;
using namespace Gtk;

class main_window : public HBox {

	public:
		main_window(LV2UI_Write_Function,LV2UI_Controller,string);
		virtual ~main_window();
		Entry* gui_save_name;
		int load_patch (string);
		int save_patch (string);
	
		void gui_port_event(LV2UI_Handle, uint32_t, uint32_t, uint32_t, const void*);

		LV2UI_Controller controller;
		LV2UI_Write_Function write_function;

		// Setting some bg params
		void bg();

		void write_control(uint32_t port_index, float value);

		string plugin_path;
		float active_cb();
		float legato_switch();
		float sync_switch();
		void save_current_patch();
		ComboBoxText* gui_patches;
		gui_dco dco1;
		gui_dco dco2;
		gui_dco dco3;
		gui_lfo lfo1;
		gui_lfo lfo2;	
		gui_dca adsr1;
		gui_dca adsr2;
		Fixed main_box;
		Fixed button_fix;
		Image image_background;
		Table* gui_filter_panel;
		Label* gui_filter_label;
		Knob* gui_filter_frequency;
		Knob* gui_filter_resonance;
		Knob* gui_volume;
		toggle* gui_sync;
		toggle* gui_legato;
};

