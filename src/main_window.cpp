/* Minaton - LV2 synthesizer Plugin
 *
 * 2011 Nick Bailey - ThunderOx Software
 * knobs borrowed from Nekobee & Abgate
*/

#include "main_window.hpp"


//--------------------------------------------------------------------------------------

main_window::main_window(LV2UI_Write_Function wf,LV2UI_Controller cntrl,string bundle_path) {
	
	write_function = wf;
	controller = cntrl;
	stringstream file_name;
	plugin_path = bundle_path;



// Asking Glib if it is already handling threads and if it isn’t we initialize the threading system.
// This has the additional perk of aborting the program if threads are not supported at all. 
	if(!Glib::thread_supported()) { Glib::thread_init(); }

// Changing the background
	signal_realize().connect(mem_fun(*this, &main_window::bg), false);

//-----------------------------------------------------------------------------------------------
// LOAD IMAGES

	file_name.str("");
	file_name  << bundle_path << "gfx/newknob.png";
	Glib::RefPtr< Gdk::Pixbuf > knob_png = Gdk::Pixbuf::create_from_file(file_name.str().c_str());

	file_name.str("");
	file_name  << bundle_path << "gfx/volknob.png";
	Glib::RefPtr< Gdk::Pixbuf > volknob_png = Gdk::Pixbuf::create_from_file(file_name.str().c_str());

//-------------------------------------------------------------------------------------------------
// ADD PATCH SELECTOR GUI

 	// Editable* pickle = manage(new GtkEditable());
	gui_save_name = manage(new Entry());
	gui_save_name->set_size_request(220, 40);

	Button* gui_save_button = manage(new Button("SAVE PATCH"));
	gui_save_button->set_size_request(220,40);
	gui_save_button->signal_clicked().connect(mem_fun(*this, &main_window::save_current_patch));

	gui_patches = manage(new ComboBoxText());
	gui_patches->set_size_request(220, 40);

	file_name.str("");
	file_name  << bundle_path << "patches";

	DIR *dp;
	struct dirent *dirp;
	if((dp  = opendir(file_name.str().c_str())) == NULL) {
        	cout << "Error(" << errno << ") opening " << file_name.str() << endl;
 	}
	else 
	{
    		while ((dirp = readdir(dp)) != NULL) {
			if (dirp->d_name[0] != '.')
			{
				dirp->d_name[strlen(dirp->d_name)-8] = 0;
 		       		gui_patches->insert_text(0,dirp->d_name);
			}
		}
    	}
    	closedir(dp);
	gui_patches->insert_text(0,"RESTORE");

	gui_patches->set_active(0);
        slot<void> slot_patches = compose(bind<0>(mem_fun(*this, &main_window::write_control),  p_patches),mem_fun(*gui_patches, &ComboBoxText::get_active_row_number));
	gui_patches->signal_changed().connect(slot_patches);	

//------------------------------------------------------------------------------------------------
// ADD FILTER GUI

// FILTER FREQUENCY KNOB
	
	gui_filter_frequency = new Knob();
	gui_filter_frequency->setFrames(knob_png,46,50,36);
	gui_filter_frequency->set_range(p_ports[p_frequency].min,p_ports[p_frequency].max,65535);
	
    
	slot<void> slot_freq = compose(bind<0>(mem_fun(*this, &main_window::write_control),  p_frequency),mem_fun(*gui_filter_frequency, &Knob::get_value));
	gui_filter_frequency->adj->signal_value_changed().connect(slot_freq);


// FILTER RESONANCE KNOB

	gui_filter_resonance = new Knob();
	gui_filter_resonance->setFrames(knob_png,46,50,36);
	gui_filter_resonance->set_range(p_ports[p_resonance].min,p_ports[p_resonance].max,65535);
	slot<void> slot_res = compose(bind<0>(mem_fun(*this, &main_window::write_control),  p_resonance),mem_fun(*gui_filter_resonance, &Knob::get_value));
	gui_filter_resonance->adj->signal_value_changed().connect(slot_res);


//-----------------------------------------------------------------------------------------------
// ADD OSCILLATORS 1-3 GUI

	dco1.controller = controller;
	dco2.controller = controller;
	dco3.controller = controller;

	dco1.write_function = write_function;
	dco2.write_function = write_function;
	dco3.write_function = write_function;

	dco1.add(knob_png,"    ",p_active_one,p_octave_one,p_finetune_one,p_finetune_centre_one,p_inertia_one,p_wave_one);
	dco2.add(knob_png,"    ",p_active_two,p_octave_two,p_finetune_two,p_finetune_centre_two,p_inertia_two,p_wave_two);
	dco3.add(knob_png,"    ",p_active_three,p_octave_three,p_finetune_three,p_finetune_centre_three,p_inertia_three,p_wave_three);

//-----------------------------------------------------------------------------------------------
// ADD LFOs 1-2 GUI

	lfo1.controller = controller;
	lfo2.controller = controller;

	lfo1.write_function = write_function;
	lfo2.write_function = write_function;

	lfo1.add(knob_png,p_lfo1_wave,p_lfo1_speed,p_lfo1_dco1_pitch,p_lfo1_dco2_pitch,p_lfo1_dco3_pitch, p_lfo1_dcf);
	lfo2.add(knob_png,p_lfo2_wave,p_lfo2_speed,p_lfo2_dco1_pitch,p_lfo2_dco2_pitch,p_lfo2_dco3_pitch, p_lfo2_dcf);

	
//-------------------------------------------------------------------------------------------------
// ADD ADSR ENVELOPES 1-2 GUI

	adsr1.controller = controller;
	adsr2.controller = controller;
	adsr1.write_function = write_function;
	adsr2.write_function = write_function;

	adsr1.add(knob_png,"     ",p_attack_one,p_decay_one,p_sustain_one,p_release_one,p_adsr1_amp_amount,p_adsr1_osc2_amount);
	adsr2.add(knob_png,"     ",p_attack_two,p_decay_two,p_sustain_two,p_release_two,p_adsr2_dcf_amount,p_adsr2_osc3_amount);

//--------------------------------------------------------------------------------------------------
// ADD TOGGLE SWITCHES FOR LEGATO & SYNC

	slot<void> slot_sync  = compose(bind<0>(mem_fun(*this, &main_window::write_control), p_sync), mem_fun(*this, &main_window::sync_switch));
	gui_sync = manage(new toggle(slot_sync));

	slot<void> slot_legato  = compose(bind<0>(mem_fun(*this, &main_window::write_control), p_legato), mem_fun(*this, &main_window::legato_switch));
	gui_legato = manage(new toggle(slot_legato));

//-------------------------------------------------------------------------------------------------
// VOLUME KNOB

	gui_volume = new Knob();
	gui_volume->setFrames(volknob_png,100,100,50);
	gui_volume->set_range(p_ports[p_master_volume].min,p_ports[p_master_volume].max,65535);
	gui_volume->set_value(p_ports[p_master_volume].default_value);
	// gui_volume->set_inverted(true);

	slot<void> slot_volume = compose(bind<0>(mem_fun(*this, &main_window::write_control),  p_master_volume),mem_fun(*gui_volume, &Knob::get_value));
	gui_volume->adj->signal_value_changed().connect(slot_volume);

//-------------------------------------------------------------------------------------------------
// ADD ALL ELEMENTS TO MAIN BOX OF TYPE *FIXED

	// Setting the size of the box
	main_box.set_size_request(660, 580);

	// Adding to the main box
	main_box.put(*gui_patches,0,0);
	main_box.put(*gui_save_name,220,0);
	main_box.put(*gui_save_button,440,0);
	main_box.put(*dco1.gui_panel,0,32);
	main_box.put(*dco2.gui_panel,0,32+(110*1));
	main_box.put(*dco3.gui_panel,0,32+(110*2));
	main_box.put(*adsr1.gui_panel,0,70+(100*3));
	main_box.put(*adsr2.gui_panel,0,70+(100*4));
	main_box.put(*lfo1.gui_panel,440,50);
	main_box.put(*lfo2.gui_panel,440,250);
	main_box.put(*gui_filter_frequency,376,370);
	main_box.put(*gui_filter_resonance,376,370+(50*1));
	main_box.put(*gui_sync,376,370+(50*2));
	main_box.put(*gui_legato,376,370+(50*3));
	main_box.put(*gui_volume,520,440);

	// Adding to the GUI
	add(main_box);
  	show_all_children();
}

//-------------------------------

void main_window::save_current_patch()
{
	save_patch(gui_save_name->get_text());
}

//-------------------------------

float main_window::legato_switch()

{
	return gui_legato->get_value();
}

//-------------------------------

float main_window::sync_switch()

{
	return gui_sync->get_value();
}


// Changing background image and window resizable settings
void main_window::bg() {
	Window *this_win = &((Window&)(*this->get_toplevel()));
	this_win->set_resizable(false);

	int winx,winy;
	int& v1 = winx;
	int& v2 = winy;
	this_win->get_size (v1,v2);
	cout << winx << "x" << winy << endl;

	stringstream ss;
	ss.str("");

	if (winy<600)
	{
		ss << plugin_path << "gfx/back.png";
		this_win->set_size_request(660, 580);
	}
	else
	{
		ss << plugin_path << "gfx/back-ardour.png";
		this_win->set_size_request(660, 619);
	}

	this_win->modify_bg_pixmap(STATE_NORMAL , ss.str().c_str());	

}

void main_window::write_control(uint32_t port_index, float value) {

	write_function(controller, port_index, sizeof(float), 0, &value);
}


//------------------------------------------------------------------------

// Destructor
main_window::~main_window() {
}

//----------------------------------------------------------------------------------------------------------------

		// Informing GUI about changes in the control ports 
		void main_window::gui_port_event(LV2UI_Handle ui, uint32_t port_index, uint32_t buffer_size, uint32_t format, const void * buffer) {

      			float val = * static_cast<const float*>(buffer);
			stringstream ss;	

			switch(port_index){

				case p_patches:
					ss.str("");
					if (gui_patches->get_active_text()!="RESTORE")
					{
						ss << plugin_path << "patches/" << gui_patches->get_active_text() << ".minaton";
					}
					else {
					      	ss << "/tmp/minaton.temp";
					      }
					load_patch (ss.str());
	

				case p_master_volume:
					gui_volume->set_value(val);
					gui_volume->queue_draw();
					break;

				case p_sync:
					gui_sync->set_toggle_value(val);
					gui_sync->queue_draw();
					break;

				case p_legato:
					gui_legato->set_toggle_value(val);
					gui_legato->queue_draw();
					break;
				
				case p_frequency:
					gui_filter_frequency->set_value(val);
					gui_filter_frequency->queue_draw();
					break;

				case p_resonance:
					gui_filter_resonance->set_value(val);
					gui_filter_resonance->queue_draw();
					break;


				case p_active_one:
					if (val)
					dco1.gui_active->set_toggle(true);
					else { dco1.gui_active->set_toggle(false); }
					break;
				case p_active_two:
					if (val)
					dco2.gui_active->set_toggle(true);
					else { dco2.gui_active->set_toggle(false); }
					break;
				case p_active_three:
					if (val)
					dco3.gui_active->set_toggle(true);
					else { dco3.gui_active->set_toggle(false); }
					break;

				case p_octave_one:
					dco1.gui_octave->set_value(val);
					dco1.gui_octave->queue_draw();
					break;
				case p_octave_two:
					dco2.gui_octave->set_value(val);
					dco2.gui_octave->queue_draw();
					break;
				case p_octave_three:
					dco3.gui_octave->set_value(val);
					dco3.gui_octave->queue_draw();
					break;

				case p_wave_one:
					dco1.gui_wave->set_value(val);
					break;
				case p_wave_two:
					dco2.gui_wave->set_value(val);
					break;
				case p_wave_three:
					dco3.gui_wave->set_value(val);
					break;

				case p_inertia_one:
					dco1.gui_inertia->set_value(val);
					break;
				case p_inertia_two:
					dco2.gui_inertia->set_value(val);
					break;
				case p_inertia_three:
					dco3.gui_inertia->set_value(val);
					break;

				case p_finetune_one:
					dco1.gui_finetune->set_value(val);
					break;
				case p_finetune_two:
					dco2.gui_finetune->set_value(val);
					break;
				case p_finetune_three:
					dco3.gui_finetune->set_value(val);
					break;

				case p_finetune_centre_one:
					dco1.gui_finetune_centre->set_toggle(val);
					break;
				case p_finetune_centre_two:
					dco2.gui_finetune_centre->set_toggle(val);
					break;
				case p_finetune_centre_three:
					dco3.gui_finetune_centre->set_toggle(val);
					break;


				case p_lfo1_speed:
					lfo1.gui_speed->set_value(val);
					lfo1.gui_speed->queue_draw();
					break;
				case p_lfo1_wave:
					lfo1.gui_wave->set_value(val);
					break;
				case p_lfo1_dco1_pitch:
					lfo1.gui_dco1->set_value(val);
					break;
				case p_lfo1_dco2_pitch:
					lfo1.gui_dco2->set_value(val);
					break;
				case p_lfo1_dco3_pitch:
					lfo1.gui_dco3->set_value(val);
					break;

				case p_lfo2_speed:
					lfo2.gui_speed->set_value(val);
					lfo2.gui_speed->queue_draw();
					break;
				case p_lfo2_wave:
					lfo2.gui_wave->set_value(val);
					break;
				case p_lfo2_dco1_pitch:
					lfo2.gui_dco1->set_value(val);
					break;
				case p_lfo2_dco2_pitch:
					lfo2.gui_dco2->set_value(val);
					break;
				case p_lfo2_dco3_pitch:
					lfo2.gui_dco3->set_value(val);
					break;

				case p_attack_one:
					adsr1.gui_attack->set_value(val);
					break;
				case p_decay_one:
					adsr1.gui_decay->set_value(val);
					break;
				case p_sustain_one:
					adsr1.gui_sustain->set_value(val);
					break;
				case p_release_one:
					adsr1.gui_release->set_value(val);
					break;
				case p_adsr1_amp_amount:
					adsr1.gui_amount1->set_value(val);
					break;
				case p_adsr1_osc2_amount:
					adsr1.gui_amount2->set_value(val);
					break;


				case p_attack_two:
					adsr2.gui_attack->set_value(val);
					break;
				case p_decay_two:
					adsr2.gui_decay->set_value(val);
					break;
				case p_sustain_two:
					adsr2.gui_sustain->set_value(val);
					break;
				case p_release_two:
					adsr2.gui_release->set_value(val);
					break;
				case p_adsr2_dcf_amount:
					adsr2.gui_amount1->set_value(val);
					break;
				case p_adsr2_osc3_amount:
					adsr2.gui_amount2->set_value(val);
					break;

			}
		}

//----------------------------------------------------------------------------------------------------------------
// ADD DCA CONTROLS TO GUI

void gui_dca::add (Glib::RefPtr< Gdk::Pixbuf > knob_png,string label,int p_attack,int p_decay,int p_sustain,int p_release,int p_amount1, int p_amount2)

{

    //-------------------------------------------\\
    //  ADSR                                     \\
    //-------------------------------------------\\

    gui_panel =new Fixed();

    Gdk::Color foreground_colour = Gdk::Color("#aaaaaa");
    Gdk::Color background_colour = Gdk::Color("#444466");

    // ATTACK SLIDER

    gui_attack = new VScale(p_ports[p_attack].min,p_ports[p_attack].max,0.01);
    slot<void> slot_attack = compose(bind<0>(mem_fun(*this, &gui_dca::write_control),p_attack),mem_fun(*gui_attack, &VScale::get_value));
    gui_attack->set_value(p_ports[p_attack].default_value);
    gui_attack->set_inverted(true);
    gui_attack->signal_value_changed().connect(slot_attack);
    gui_attack->modify_fg(STATE_NORMAL, foreground_colour);
    gui_attack->modify_bg(STATE_NORMAL, background_colour);
    gui_attack->set_size_request(40,80);

    gui_decay = new VScale(p_ports[p_decay].min,p_ports[p_decay].max,0.01);
    slot<void> slot_decay = compose(bind<0>(mem_fun(*this, &gui_dca::write_control),p_decay),mem_fun(*gui_decay, &VScale::get_value));
    gui_decay->set_value(p_ports[p_decay].default_value);
    gui_decay->set_inverted(true);
    gui_decay->signal_value_changed().connect(slot_decay);
    gui_decay->modify_fg(STATE_NORMAL, foreground_colour);
    gui_decay->modify_bg(STATE_NORMAL, background_colour);
    gui_decay->set_size_request(40,80);

    gui_sustain = new VScale(p_ports[p_sustain].min,p_ports[p_sustain].max,0.01);
    slot<void> slot_sustain = compose(bind<0>(mem_fun(*this, &gui_dca::write_control),p_sustain),mem_fun(*gui_sustain, &VScale::get_value));
    gui_sustain->set_value(p_ports[p_sustain].default_value);
    gui_sustain->set_inverted(true);
    gui_sustain->signal_value_changed().connect(slot_sustain);
    gui_sustain->modify_fg(STATE_NORMAL, foreground_colour);
    gui_sustain->modify_bg(STATE_NORMAL, background_colour);
    gui_sustain->set_size_request(40,80);

    gui_release = new VScale(p_ports[p_release].min,p_ports[p_release].max,0.01);
    slot<void> slot_release = compose(bind<0>(mem_fun(*this, &gui_dca::write_control),p_release),mem_fun(*gui_release, &VScale::get_value));
    gui_release->set_value(p_ports[p_release].default_value);
    gui_release->set_inverted(true);
    gui_release->signal_value_changed().connect(slot_release);
    gui_release->modify_fg(STATE_NORMAL, foreground_colour);
    gui_release->modify_bg(STATE_NORMAL, background_colour);
    gui_release->set_size_request(40,80);

    gui_amount1 = new VScale(p_ports[p_amount1].min,p_ports[p_amount1].max,0.01);
    slot<void> slot_amount1 = compose(bind<0>(mem_fun(*this, &gui_dca::write_control),p_amount1),mem_fun(*gui_amount1, &VScale::get_value));
    gui_amount1->set_value(p_ports[p_amount1].default_value);
    gui_amount1->signal_value_changed().connect(slot_amount1);
    gui_amount1->modify_fg(STATE_NORMAL, foreground_colour);
    gui_amount1->modify_bg(STATE_NORMAL, background_colour);
    gui_amount1->set_size_request(40,80);

    gui_amount2 = new VScale(p_ports[p_amount2].min,p_ports[p_amount2].max,0.01);
    slot<void> slot_amount2 = compose(bind<0>(mem_fun(*this, &gui_dca::write_control),p_amount2),mem_fun(*gui_amount2, &VScale::get_value));
    gui_amount2->set_value(p_ports[p_amount2].default_value);
    gui_amount2->set_inverted(true);
    gui_amount2->signal_value_changed().connect(slot_amount2);
    gui_amount2->modify_fg(STATE_NORMAL, foreground_colour);
    gui_amount2->modify_bg(STATE_NORMAL, background_colour);
    gui_amount2->set_size_request(40,80);

    gui_panel->put(*gui_attack,100,0);
    gui_panel->put(*gui_decay,140,0);
    gui_panel->put(*gui_sustain,180,0);
    gui_panel->put(*gui_release,220,0);

    gui_panel->put(*gui_amount1,280,0);
    gui_panel->put(*gui_amount2,320,0);
}

//---------------------------------------------------------------------------------------------------

void gui_dca::write_control(uint32_t port_index, float value) {
	write_function(controller, port_index, sizeof(float), 0, &value);
}

//----------------------------------------------------------------------------------------------------------------
// ADD DCO CONTROLS TO GUI

void gui_dco::add(Glib::RefPtr< Gdk::Pixbuf > knob_png,string label,int p_active,int p_octave,int p_finetune,int p_finetune_centre,int p_inertia,int p_wave)

{

    Gdk::Color foreground_colour = Gdk::Color("#aaaaaa");
    Gdk::Color background_colour = Gdk::Color("#444466");

    //-------------------------------------------\\
    //  OSCILLATOR                               \\
    //-------------------------------------------\\

    gui_panel = manage(new Table(2,14,true));
    gui_panel->set_border_width(8);
    gui_label = manage(new Label(label));

    // DCO ON / OFF SWITCH

    slot<void> slot_active = compose(bind<0>(mem_fun(*this, &gui_dco::write_control), p_active), mem_fun(*this, &gui_dco::active_cb));
    gui_active = manage(new toggle(slot_active));

    // OCTAVE KNOB

    gui_octave = new Knob();
    slot<void> slot_octave = compose(bind<0>(mem_fun(*this, &gui_dco::write_control),  p_octave),mem_fun(*gui_octave, &Knob::get_value));
    gui_octave->setFrames(knob_png,46,50,36);
    gui_octave->set_range(p_ports[p_octave].min,p_ports[p_octave].max,8);
    gui_octave->adj->signal_value_changed().connect(slot_octave);

    // FINE TUNE SLIDER

    gui_finetune = new HScale(p_ports[p_finetune].min,p_ports[p_finetune].max,0.0001);
    slot<void> slot_finetune = compose(bind<0>(mem_fun(*this, &gui_dco::write_control),  p_finetune),mem_fun(*gui_finetune, &HScale::get_value));
    gui_finetune->set_value(p_ports[p_finetune].default_value);
    gui_finetune->signal_value_changed().connect(slot_finetune);
    gui_finetune->modify_fg(STATE_NORMAL, foreground_colour);
    gui_finetune->modify_bg(STATE_NORMAL, background_colour);
		
    // FINE TUNE CENTRE

    slot<void> slot_finetune_centre  = compose(bind<0>(mem_fun(*this, &gui_dco::write_control), p_finetune_centre), mem_fun(*this, &gui_dco::finetune_centre_cb));
    gui_finetune_centre = manage(new toggle(slot_finetune_centre));

    // WAVE SLIDER

    gui_wave = new HScale(p_ports[p_wave].min,p_ports[p_wave].max+1,1);
    slot<void> slot_wave = compose(bind<0>(mem_fun(*this, &gui_dco::write_control),  p_wave),mem_fun(*gui_wave, &HScale::get_value));
    gui_wave->signal_value_changed().connect(slot_wave);

    // WAVE INERTIA

    gui_inertia = new Knob();
    slot<void> slot_inertia = compose(bind<0>(mem_fun(*this, &gui_dco::write_control),  p_inertia),mem_fun(*gui_inertia, &Knob::get_value));
    gui_inertia->setFrames(knob_png,46,50,36);
    gui_inertia->set_range(p_ports[p_inertia].min,p_ports[p_inertia].max,65535);
    gui_inertia->set_value(p_ports[p_inertia].default_value);
    gui_inertia->adj->signal_value_changed().connect(slot_inertia);

    // ATTATCH TO PANEL 

    gui_panel->attach(*manage(gui_active),1,2,0,1);
    gui_panel->attach(*manage(gui_label),0,1,0,1);
    gui_panel->attach(*manage(gui_finetune),2,8,0,1);
    gui_panel->attach(*manage(gui_wave),2,8,1,2);
    gui_panel->attach(*manage(gui_octave),1,2,1,2);
    gui_panel->attach(*manage(gui_inertia),8,9,1,2);
    gui_panel->attach(*manage(gui_finetune_centre),8,9,0,1);

}



//------------------------------------------------------
// CALLBACKS
//------------------------------------------------------

//------- ACTIVE ----------

float gui_dco::active_cb()
{
	return gui_active->get_value();
}



//------- FINETUNE ----------

float gui_dco::finetune_cb()
{
	if (gui_finetune->get_value()==0)
	{
		gui_finetune_centre->set_toggle(true);
	}
	else	{
			gui_finetune_centre->set_toggle(false);
		}
	return gui_finetune->get_value();
}

//------- FINETUNE CENTRE ----------

float gui_dco::finetune_centre_cb()
{
	if (gui_finetune_centre->get_value()==true)
	{
		gui_finetune->set_value(0);
	}
	//gui_finetune_centre->set_toggle(true);
	return gui_finetune_centre->get_value();
}

//-------- WAVE --------------

float gui_dco::wave_cb()
{
	return gui_wave->get_value();
}

//-------- INERTIA --------------

float gui_dco::inertia_cb()
{
	return gui_inertia->get_value();
}



void gui_dco::write_control(uint32_t port_index, float value) {
	write_function(controller, port_index, sizeof(float), 0, &value);
}

//-------------------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------
// ADD LFO CONTROLS TO GUI

void gui_lfo::add(Glib::RefPtr< Gdk::Pixbuf > knob_png,int p_wave,int p_speed, int p_dco1,int p_dco2, int p_dco3, int p_dcf)

{

	Gdk::Color foreground_colour = Gdk::Color("#aaaaaa");
	Gdk::Color background_colour = Gdk::Color("#444466");

	gui_panel = new Fixed();

	// WAVE SLIDER

	gui_wave = new HScale(p_ports[p_wave].min,p_ports[p_wave].max+1,1);
	slot<void> slot_wave = compose(bind<0>(mem_fun(*this, &gui_lfo::write_control),  p_wave),mem_fun(*gui_wave, &HScale::get_value));
	gui_wave->signal_value_changed().connect(slot_wave);
	gui_wave->set_size_request(120,40);

	// SPEED KNOB

	gui_speed = new Knob();
	slot<void> slot_speed = compose(bind<0>(mem_fun(*this, &gui_lfo::write_control),  p_speed),mem_fun(*gui_speed, &Knob::get_value));
	gui_speed->setFrames(knob_png,46,50,36);
	gui_speed->set_range(p_ports[p_speed].min,p_ports[p_speed].max,65535);
	gui_speed->adj->signal_value_changed().connect(slot_speed);
	
	// AMOUNT OF LFO TO OSCILLATOR 1

	gui_dco1 = new VScale(p_ports[p_dco1].min,p_ports[p_dco1].max,0.01);
	slot<void> slot_dco1 = compose(bind<0>(mem_fun(*this, &gui_lfo::write_control),p_dco1),mem_fun(*gui_dco1, &VScale::get_value));
	gui_dco1->set_value(p_ports[p_dco1].default_value);
	gui_dco1->set_inverted(true);
	gui_dco1->signal_value_changed().connect(slot_dco1);
	gui_dco1->modify_fg(STATE_NORMAL, foreground_colour);
	gui_dco1->modify_bg(STATE_NORMAL, background_colour);
	gui_dco1->set_size_request(40,80);

	// AMOUNT OF LFO TO OSCILLATOR 2

	gui_dco2 = new VScale(p_ports[p_dco2].min,p_ports[p_dco2].max,0.01);
	slot<void> slot_dco2 = compose(bind<0>(mem_fun(*this, &gui_lfo::write_control),p_dco2),mem_fun(*gui_dco2, &VScale::get_value));
	gui_dco2->set_value(p_ports[p_dco2].default_value);
	gui_dco2->set_inverted(true);
	gui_dco2->signal_value_changed().connect(slot_dco2);
	gui_dco2->modify_fg(STATE_NORMAL, foreground_colour);
	gui_dco2->modify_bg(STATE_NORMAL, background_colour);
	gui_dco2->set_size_request(40,80);

	// AMOUNT OF LFO TO OSCILLATOR 3

	gui_dco3 = new VScale(p_ports[p_dco3].min,p_ports[p_dco3].max,0.01);
	slot<void> slot_dco3 = compose(bind<0>(mem_fun(*this, &gui_lfo::write_control),p_dco3),mem_fun(*gui_dco3, &VScale::get_value));
	gui_dco3->set_value(p_ports[p_dco3].default_value);
	gui_dco3->set_inverted(true);
	gui_dco3->signal_value_changed().connect(slot_dco3);
	gui_dco3->modify_fg(STATE_NORMAL, foreground_colour);
	gui_dco3->modify_bg(STATE_NORMAL, background_colour);
	gui_dco3->set_size_request(40,80);

	// AMOUNT OF LFO TO FILTER

	gui_dcf = new VScale(p_ports[p_dcf].min,p_ports[p_dcf].max,0.01);
	slot<void> slot_dcf = compose(bind<0>(mem_fun(*this, &gui_lfo::write_control),p_dcf),mem_fun(*gui_dcf, &VScale::get_value));
	gui_dcf->set_value(p_ports[p_dcf].default_value);
	gui_dcf->set_inverted(true);
	gui_dcf->signal_value_changed().connect(slot_dcf);
	gui_dcf->modify_fg(STATE_NORMAL, foreground_colour);
	gui_dcf->modify_bg(STATE_NORMAL, background_colour);
	gui_dcf->set_size_request(40,80);

	gui_panel->put(*gui_speed,0,0);
	gui_panel->put(*gui_wave,50,0);
	gui_panel->put(*gui_dco1,20,60);
	gui_panel->put(*gui_dco2,60,60);
	gui_panel->put(*gui_dco3,100,60);
	gui_panel->put(*gui_dcf,140,60);

}

//---------------------------------------------------------------------------------------------------

void gui_lfo::write_control(uint32_t port_index, float value) {
	write_function(controller, port_index, sizeof(float), 0, &value);
}


//-------------------------------------------------------------------------------------------------------------------------------


int main_window::load_patch (string fname) {

    string str;
    ifstream myfile;
    myfile.open (fname.c_str());
    getline(myfile,str);
    float value=0;


    if (str=="minaton1")
    {

        //----------------------------

        getline(myfile,str);

        if (str=="FLAGS")
        {
            getline(myfile,str); // legato
            istringstream ( str ) >> value;
	    write_control(p_legato,value);

            getline(myfile,str); // sync
            istringstream ( str ) >> value;
	    write_control(p_sync,value);

            getline(myfile,str); // volume
            istringstream ( str ) >> value;
	    write_control(p_master_volume,value);

            getline(myfile,str); // midi channel
            istringstream ( str ) >> value;
	    write_control(p_midi_channel,value);

        }

        //-----------------------------

        getline(myfile,str);
        getline(myfile,str);
        if (str=="FILTER")
        {
            getline(myfile,str); // filter frequency
            istringstream ( str ) >> value;
	    write_control(p_frequency,value*9);
	
            getline(myfile,str); // filter resonance
            istringstream ( str ) >> value;
	    write_control(p_resonance,value*4);
        }

        //-----------------------------

        getline(myfile,str);
        getline(myfile,str);

        if (str=="OSCILLATOR ONE")
        {

            getline(myfile,str); // oscillator active or not
            istringstream ( str ) >> value;
	    write_control(p_active_one,value);

            getline(myfile,str); // octave
            istringstream ( str ) >> value;
	    write_control(p_octave_one,value);

            getline(myfile,str); // pitch
            istringstream ( str ) >> value;
            write_control(p_finetune_one,value);;

            getline(myfile,str); // wave
            istringstream ( str ) >> value;
            write_control(p_wave_one,value);
        }


        //-----------------------------

        getline(myfile,str);
        getline(myfile,str);

        if (str=="OSCILLATOR TWO")
        {
            getline(myfile,str); // oscillator active or not
            istringstream ( str ) >> value;
	    write_control(p_active_two,value);

            getline(myfile,str); // octave
            istringstream ( str ) >> value;
            write_control(p_octave_two,value);

            getline(myfile,str); // pitch
            istringstream ( str ) >> value;
            write_control(p_finetune_two,value);

            getline(myfile,str); // wave
            istringstream ( str ) >> value;
	    write_control(p_wave_two,value);

        }


        //-----------------------------

        getline(myfile,str);
        getline(myfile,str);

        if (str=="OSCILLATOR THREE")
        {
            getline(myfile,str); // oscillator active or not
            istringstream ( str ) >> value;
            write_control(p_active_three,value);  

            getline(myfile,str); // octave
            istringstream ( str ) >> value;
            write_control(p_octave_three,value);

            getline(myfile,str); // pitch
            istringstream ( str ) >> value;
            write_control(p_finetune_three,value);

            getline(myfile,str); // wave
            istringstream ( str ) >> value;
	    write_control(p_wave_three,value);
        }


        //-----------------------------

        getline(myfile,str);
        getline(myfile,str);

        if (str=="LFO ONE")
        {
            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo1_dco1_pitch,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo1_dco2_pitch,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo1_dco3_pitch,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo1_dcf,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo1_wave,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo1_speed,value);

        }

        //-----------------------------

        getline(myfile,str);
        getline(myfile,str);

        if (str=="LFO TWO")
        {
            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo2_dco1_pitch,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo2_dco2_pitch,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo2_dco3_pitch,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo2_dcf,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo2_wave,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_lfo2_speed,value);

        }

        //-----------------------------

        getline(myfile,str);
        getline(myfile,str);

        if (str=="ADSR ONE")
        {
            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_attack_one,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_decay_one,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_sustain_one,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_release_one,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
            write_control(p_adsr1_amp_amount,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
            write_control(p_adsr1_osc2_amount,value);
        }

        //-----------------------------

        getline(myfile,str);
        getline(myfile,str);

        if (str=="ADSR TWO")
        {
            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_attack_two,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_decay_two,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_sustain_two,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
	    write_control(p_release_two,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
            write_control(p_adsr2_dcf_amount,value);

            getline(myfile,str);
            istringstream ( str ) >> value;
            write_control(p_adsr2_osc3_amount,value);
        }

    }

        getline(myfile,str);
        getline(myfile,str);

        if (str=="INERTIA")
        {
            getline(myfile,str);
            istringstream ( str ) >> value;
            write_control(p_inertia_one,value);
            getline(myfile,str);
            istringstream ( str ) >> value;
            write_control(p_inertia_two,value);
            getline(myfile,str);
            istringstream ( str ) >> value;
            write_control(p_inertia_three,value);
	}

//--------------------------------


    myfile.close();

    return 0;
}

//--------------------------------------------------------------------


int main_window::save_patch (string patch_name) {

    stringstream file_name;

    if (patch_name==".temp")
    {
	file_name.str("/tmp/minaton.temp");
	cout << "saving.." << file_name.str() << endl;
    }
    else
    {
	transform(patch_name.begin(), patch_name.end(),patch_name.begin(), ::toupper);
	file_name << plugin_path << "patches/" << patch_name << ".minaton";
	cout << "saving.." << file_name.str() << endl;
    }

    ofstream myfile;
    myfile.open (file_name.str().c_str());
    myfile << "minaton1" << endl;

    // SAVE VARIOUS GLOBALS

    myfile << "FLAGS" << endl;
    myfile << gui_legato->get_value() << endl;
    myfile << gui_sync->get_value() << endl;
    myfile << gui_volume->get_value() << endl;
    myfile << 1 << endl;
    myfile << endl;

    // SAVE FILTER

    myfile << "FILTER" << endl;
    myfile << gui_filter_frequency->get_value()/9 << endl;
    myfile << gui_filter_resonance->get_value()/4 << endl;
    myfile << endl;

    // SAVE OSCILLATOR ONE

    myfile << "OSCILLATOR ONE" << endl;
    myfile << dco1.gui_active->get_value() << endl;
    myfile << dco1.gui_octave->get_value() << endl;
    myfile << dco1.gui_finetune->get_value() << endl;
    myfile << dco1.gui_wave->get_value() << endl;
    myfile << endl;

    // SAVE OSCILLATOR TWO

    myfile << "OSCILLATOR TWO" << endl;
    myfile << dco2.gui_active->get_value() << endl;
    myfile << dco2.gui_octave->get_value() << endl;
    myfile << dco2.gui_finetune->get_value() << endl;
    myfile << dco2.gui_wave->get_value() << endl;
    myfile << endl;

    // SAVE OSCILLATOR THREE

    myfile << "OSCILLATOR THREE" << endl;
    myfile << dco3.gui_active->get_value() << endl;
    myfile << dco3.gui_octave->get_value() << endl;
    myfile << dco3.gui_finetune->get_value() << endl;
    myfile << dco3.gui_wave->get_value() << endl;
    myfile << endl;

    // SAVE LFO ONE

    myfile << "LFO ONE" << endl;
    myfile << lfo1.gui_dco1->get_value() << endl;
    myfile << lfo1.gui_dco2->get_value() << endl;
    myfile << lfo1.gui_dco3->get_value() << endl;
    myfile << lfo1.gui_dcf->get_value() << endl;
    myfile << lfo1.gui_wave->get_value() << endl;
    myfile << lfo1.gui_speed->get_value() << endl;
    myfile << endl;

    // SAVE LFO TWO

    myfile << "LFO TWO" << endl;
    myfile << lfo2.gui_dco1->get_value() << endl;
    myfile << lfo2.gui_dco2->get_value() << endl;
    myfile << lfo2.gui_dco3->get_value() << endl;
    myfile << lfo2.gui_dcf->get_value() << endl;
    myfile << lfo2.gui_wave->get_value() << endl;
    myfile << lfo2.gui_speed->get_value() << endl;
    myfile << endl;

    // SAVE AMP ADSR

    myfile << "ADSR ONE" << endl;
    myfile << adsr1.gui_attack->get_value() << endl;
    myfile << adsr1.gui_decay->get_value() << endl;
    myfile << adsr1.gui_sustain->get_value() << endl;
    myfile << adsr1.gui_release->get_value() << endl;
    myfile << adsr1.gui_amount1->get_value() << endl;
    myfile << adsr1.gui_amount2->get_value() << endl;
    myfile << endl;

    // SAVE FILTER ADSR

    myfile << "ADSR TWO" << endl;
    myfile << adsr2.gui_attack->get_value() << endl;
    myfile << adsr2.gui_decay->get_value() << endl;
    myfile << adsr2.gui_sustain->get_value() << endl;
    myfile << adsr2.gui_release->get_value() << endl;
    myfile << adsr2.gui_amount1->get_value() << endl;
    myfile << adsr2.gui_amount2->get_value() << endl;
    myfile << endl;

    // DCO INERTIA

    myfile << "INERTIA" << endl;
    myfile << dco1.gui_inertia->get_value() << endl;
    myfile << dco2.gui_inertia->get_value() << endl;
    myfile << dco3.gui_inertia->get_value() << endl;

    myfile.close();
    return 0;
}


