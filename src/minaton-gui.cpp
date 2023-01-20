
#include "main_window.hpp"
#include <fstream>
#include <gtkmm/main.h>
#include <lv2.h>

static LV2UI_Descriptor* minaton_guiDescriptor = NULL;

static LV2_Handle instantiateMinaton_gui(const _LV2UI_Descriptor* descriptor, const char* plugin_uri, const char* bundle_path, LV2UI_Write_Function write_function, LV2UI_Controller controller, LV2UI_Widget* widget, const LV2_Feature* const* features)
{

    Gtk::Main::init_gtkmm_internals();
    ofstream myfile;
    myfile.open("/tmp/minaton.conf");
    myfile << bundle_path << endl;
    myfile.close();
    main_window* gui_data = new main_window(write_function, controller, bundle_path);
    *widget = gui_data->gobj();
    return (LV2_Handle)gui_data;
}

static void cleanupminaton_gui(LV2UI_Handle instance)
{
    static_cast<main_window*>(instance)->save_patch(".temp");
    delete static_cast<main_window*>(instance);
}

static void portEvent_minaton_gui(LV2UI_Handle ui, uint32_t port, uint32_t buffer_size, uint32_t format, const void* buffer)
{
    main_window* gui = (main_window*)ui;
    gui->gui_port_event(ui, port, buffer_size, format, buffer);
}

static void init_gui()
{

    minaton_guiDescriptor = (LV2UI_Descriptor*)malloc(sizeof(LV2UI_Descriptor));
    minaton_guiDescriptor->URI = "http://nickbailey/minaton/gui";
    minaton_guiDescriptor->instantiate = instantiateMinaton_gui;
    minaton_guiDescriptor->cleanup = cleanupminaton_gui;
    minaton_guiDescriptor->port_event = portEvent_minaton_gui;
    minaton_guiDescriptor->extension_data = NULL;
}

// LV2_SYMBOL_EXPORT
const LV2UI_Descriptor* lv2ui_descriptor(uint32_t index)
{

    if (!minaton_guiDescriptor) {
        init_gui();
    }

    switch (index) {
    case 0:
        return minaton_guiDescriptor;
    default:
        return NULL;
    }
}
