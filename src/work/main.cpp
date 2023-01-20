

#include "dca.h"
#include "dcf.h"
#include "dco.h"
#include "files.h"
#include "jack.cpp"
#include <dirent.h>

#include <iostream>
#include <jack/jack.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

#include <samplerate.h>
#include <sndfile.h>

jack_nframes_t mySamplePos = 0;
jack_nframes_t jackPos = 0;
int delay;
int last_note = -1;

Fl_Double_Window* pw;

// ------------------- jack stuff ----------------------------

jack_port_t* input_port;
jack_port_t* output_port;
jack_port_t* midi_in_port;

int process(jack_nframes_t nframes, void* arg)

{
    //--------- AUDIO PART

    jack_default_audio_sample_t* out = (jack_default_audio_sample_t*)
        jack_port_get_buffer(output_port, nframes);

    if (!get_dco_state(0) && !get_dco_state(1) && !get_dco_state(2) || master_volume == 500) {
        for (int x = 0; x < nframes; x++) {
            out[x] = 0;
        }
        return 0; // No oscillators enabled or volume at zero, clear ring buffer and return
    }

    float mix1 = 0, mix2 = 0, mix3 = 0, lfo1 = 0, lfo2 = 0, delay;

    for (int x = 0; x < nframes; x++) {
        dca_update();

        delay++;
        if (delay > 20) {
            dco_cycle(3);
            dco_cycle(4);
            delay = 0;
        }

        if (get_dco_state(0)) {
            mix1 = get_dco_out(0);
            dco_cycle(0);
        }
        if (get_dco_state(1)) {
            mix2 = get_dco_out(1);
            dco_cycle(1);
        }
        if (get_dco_state(2)) {
            mix3 = get_dco_out(2);
            dco_cycle(2);
        }
        float mixer_out = (mix1 + mix2 + mix3) / master_volume;

        if (mixer_out <= -1.25f) {
            mixer_out = -0.984375;
        } else if (mixer_out >= 1.25f) {
            mixer_out = 0.984375;
        } else {
            mixer_out = 1.1f * mixer_out - 0.2f * mixer_out * mixer_out * mixer_out;
        }

        mixer_out = dcf(mixer_out, envelope2_out(0.1, adsr_filter_amount2->value()), 0.1);
        out[x] = envelope1_out(mixer_out, adsr_amp_amount1->value());
    }

    //------------ MIDI PART

    jack_midi_event_t in_event;
    jack_nframes_t event_index = 0;
    jack_position_t positionQuery;
    jack_transport_state_t transportQuery;
    void* port_buf = jack_port_get_buffer(midi_in_port, nframes);

    jack_nframes_t event_count = jack_midi_get_event_count(port_buf);

    if (event_count > 0) {
        for (int i = 0; i < event_count; i++) {
            jack_midi_event_get(&in_event, port_buf, i);

            int value1 = in_event.buffer[0];
            int value2 = in_event.buffer[1];
            int value3 = in_event.buffer[2];

            if (value1 == 143 + midi_channel->value()) {
                if (last_note == -1 || get_legato() == 0) {
                    trigger_envelope1();
                    trigger_envelope2();
                }
                set_tuning(value2 - 38);
                last_note = value2;
            }

            if (value1 == 127 + midi_channel->value() && value2 == last_note) {
                release_envelope1();
                release_envelope2();
                last_note = -1;
            }

            if (value1 == 176 && value2 == 1) {
                float cutoff = ((float)(pow(1.6, value3 * 0.002000))) - 1;
                set_dcf_frequency(cutoff);
            }
        }
    }
    return 0;
}

//------------------------------------------------------------------------
// JACK SAMPLE RATE CHANGED

int srate(jack_nframes_t nframes, void* arg)

{
    printf("the sample rate is now %lu/sec\n", nframes);
    return 0;
}

//------------------------------------------------------------------------
// JACK ERROR CHANGED

void error(const char* desc)
{
    fprintf(stderr, "JACK error: %s\n", desc);
}

//------------------------------------------------------------------------
// JACK SHUTDOWN

void jack_shutdown(void* arg)
{
    return;
}

//------------------------------------------------------------------------
// INITIALISE GUI JACK AND OSCILLATORS ETC

int main(int argc, char* argv[])
{
    Fl_Double_Window* mw;
    mw = make_window();
    mw->show();

    pw = patch_window();

    init_src();

    add_wave("sine", "waves/440Hz-sine.wav");
    add_wave("saw", "waves/440Hz-saw.wav");
    add_wave("square", "waves/440Hz-square.wav");
    add_wave("triangle", "waves/440Hz-triangle.wav");
    add_wave("triangle", "waves/440Hz-triangle.wav"); // dummy wave white noise generator is #4

    add_wave("slow sine", "waves/1Hz-sine.wav");
    add_wave("slow saw", "waves/1Hz-saw.wav");
    add_wave("slow square", "waves/1Hz-square.wav");

    add_dco();
    set_dco_wave(0, 1);
    set_dco_frequency(0, 1);

    add_dco();
    set_dco_wave(1, 1);
    set_dco_frequency(1, 1);

    add_dco();
    set_dco_wave(2, 1);
    set_dco_frequency(2, 1);

    add_dco();
    set_dco_wave(3, 5);
    set_dco_frequency(3, 0.0001);

    add_dco();
    set_dco_wave(4, 5);
    set_dco_frequency(4, 0.0001);

    init_dcas();

    //------------ jack stuff -------------------------------------

    jack_client_t* client;

    const char** ports;

    jack_set_error_function(error);

    if ((client = jack_client_open("Minaton", JackNullOption, NULL)) == 0) {
        fprintf(stderr, "jack server not running?\n");
        return 1;
    }

    jack_set_process_callback(client, process, 0);
    jack_set_sample_rate_callback(client, srate, 0);
    jack_on_shutdown(client, jack_shutdown, 0);
    printf("engine sample rate: %lu\n", jack_get_sample_rate(client));

    output_port = jack_port_register(client, "output",
        JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

    midi_in_port = jack_port_register(client, "midi in", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);

    if (jack_activate(client)) {
        fprintf(stderr, "cannot activate client");
        return 1;
    }

    if ((ports = jack_get_ports(client, NULL, NULL,
             JackPortIsPhysical | JackPortIsInput))
        == NULL) {
        fprintf(stderr, "Cannot find any physical playback ports\n");
        return 1;
    }

    int i = 0;
    while (ports[i] != NULL) {
        if (jack_connect(client, jack_port_name(output_port), ports[i])) {
            fprintf(stderr, "cannot connect output ports\n");
        }
        i++;
    }

    free(ports);

    Fl::run();

    jack_client_close(client);
    return 0;
}

//--------------------------------------------------

void show_patch_window()
{
    patch_browser->clear();

    DIR* dir;
    struct dirent* ent;
    dir = opendir("patches/");

    if (dir != NULL) {

        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL) {
            // printf ("%s\n", ent->d_name);
            string str = ent->d_name;
            if (str != "." && str != "..") {
                patch_browser->add(str.c_str());
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        perror("");
        //  return EXIT_FAILURE;
    }

    pw->show();
}
