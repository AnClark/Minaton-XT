
#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <samplerate.h>
#include <sndfile.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>

using namespace std;

class adsr {
public:
    float attack;
    float decay;
    float sustain;
    float release;

    int state;
    float level;
};

//----------------------------------------------------------------------------

class minaton_synth {
public:
    void on(unsigned char, unsigned char);
    void init();
    void cleanup();
    string bundle_path;
    float master_volume;
    float adsr_osc2_amount1;
    float adsr_osc3_amount2;
    float adsr_filter_amount1;
    float adsr_filter_amount2;
    float adsr_amp_amount1;
    float adsr_amp_amount2;
    float pitch1;
    float octave1;
    float pitch2;
    float octave2;
    float pitch3;
    float octave3;
    float attack1;
    float decay1;
    float sustain1;
    float release1;
    float attack2;
    float decay2;
    float sustain2;
    float release2;
    int attack;
    int decay;
    int wait;
    int release;
    int dormant;
    float selectivity, gain1_left, gain1_right, gain2_left, gain2_right, ratio_left, ratio_right, cap_left, cap_right;

    //=========================================================
    //-- DCA ROUTINES
    //=========================================================

    adsr envelope1;
    adsr envelope2;
    int master_legato;
    void dca_update();
    void init_dcas();
    float envelope1_out(float, float);
    void trigger_envelope1();
    void release_envelope1();
    void set_envelope1_attack(float);
    void set_envelope1_decay(float);
    void set_envelope1_sustain(float);
    void set_envelope1_release(float);
    float envelope2_out(float, float);
    void trigger_envelope2();
    void release_envelope2();
    void set_envelope2_attack(float);
    void set_envelope2_decay(float);
    void set_envelope2_sustain(float);
    void set_envelope2_release(float);
    void toggle_legato();
    void set_legato(int);
    int get_legato();
    int legato_button;
    int sync_button;
    float freq;
    float res;
    bool active1;
    bool active2;
    bool active3;
    float lfo1_dcf;
    float lfo2_dcf;
    int wave1;
    int wave2;
    int wave3;
    int wave4;
    int wave5;
    float lfo1speed;
    float lfo2speed;
    float lfo1_dco1_pitch;
    float lfo1_dco2_pitch;
    float lfo1_dco3_pitch;
    float lfo2_dco1_pitch;
    float lfo2_dco2_pitch;
    float lfo2_dco3_pitch;

    //=========================================================
    //-- DCF ROUTINES
    //=========================================================

    void init_dcf();

    float master_frequency;
    float master_resonance;
    float lfo1_amount;
    float lfo2_amount;

    float buf0_left, buf1_left, out_left;

    float f_left, p_left, q_left; // filter coefficients
    float b0_left, b1_left, b2_left, b3_left, b4_left; // filter buffers (beware denormals!)
    float t1_left, t2_left; // temporary buffers

    float f_right, p_right, q_right; // filter coefficients
    float b0_right, b1_right, b2_right, b3_right, b4_right; // filter buffers (beware denormals!)
    float t1_right, t2_right;

    void set_dcf_frequency(float);

    void set_lfo1_dcf_amount(float);
    void set_lfo2_dcf_amount(float);

    float dcf_left(float, float, float);
    float dcf_right(float, float, float);
    void set_dcf_resonance(float);

    //=========================================================
    //-- DCO ROUTINES
    //=========================================================

    int error_sc;
    SRC_STATE* src_test;
    SRC_DATA mySampleData;
    int number_of_dcos;

    bool dco_state[5];
    int dco_delay[5];
    float dco_frequency[5];
    float dco_old_frequency[5];
    float dco_volume[5];
    float dco_pulsewidth[5];
    int dco_out_position[5];
    int dco_out_length[5];
    int dco_wave[5];
    float dco_inertia[5];
    float dco_lfo1_amount[5];
    float dco_lfo2_amount[5];

    float dco1_buffer[4096];
    float dco2_buffer[4096];
    float dco3_buffer[4096];
    float lfo1_buffer[65536 * 8];
    float lfo2_buffer[65536 * 8];

    void set_lfo1_amount(int, float);
    void set_lfo2_amount(int, float);

    void set_freq(int, float);

    int number_of_waves;
    float* waves_sample[16];
    string waves_name[16];
    SF_INFO waves_sfinfo[16];

    SNDFILE *infile, *outfile;

    float master_note;
    int master_sync;

    void init_src();
    void dco_on(int);
    void dco_off(int);
    bool get_dco_state(int);
    int add_wave(string, string);
    void add_dco();
    float get_dco_out(int);
    float dco_cycle(int);
    float get_dco_frequency(int);
    void set_dco_frequency(int, float);
    float get_dco_volume(int);
    void set_dco_volume(int, float);
    int get_dco_wave(int);
    void set_dco_wave(int, int);
    void set_dco_lfo1_amount(int, float);
    void set_dco_lfo2_amount(int, float);

    void set_tuning(float);
    void toggle_sync();
    void set_sync_mode(int);
    int get_sync_mode();

    float get_dco_lfo1_amount(int);
    float get_dco_lfo2_amount(int);

    float get_dco_inertia(int);
    void set_dco_inertia(int, float);

    void update_dco1_tuning();
    void update_dco2_tuning();
    void update_dco3_tuning();

    //=========================================================
    //-- FILE ROUTINES
    //=========================================================

    int save_patch(string);
    int load_patch(string);

    void set_bundle_path(const char*);

protected:
};
