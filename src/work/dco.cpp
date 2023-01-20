
#include "dco.h"
#include "dca.h"
#include "main.h"
#include <iostream>
#include <math.h>
#include <samplerate.h>
#include <sndfile.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>
using namespace std;

int error_sc;
SRC_STATE* src_test;
SRC_DATA mySampleData;

//---------------------------------------------------------

class wave {
public:
    string name;
    float* sample;
    SF_INFO sfinfo;
};

SNDFILE *infile, *outfile;

vector<wave> waves;

float master_note = 0;
int master_sync = 0;

//---------------- Add new waveform ------------------

int add_wave(string name, string file)
{
    wave new_wave;
    int readcount;

    new_wave.name = name;

    if (!(infile = sf_open(file.c_str(), SFM_READ, &new_wave.sfinfo))) { /* Open failed so print an error message. */
        cout << "Not able to open input file - " << name << endl;
        /* Print the error message fron libsndfile. */
        sf_perror(NULL);
        return 1;
    }

    new_wave.sample = new float[new_wave.sfinfo.frames * new_wave.sfinfo.channels];
    readcount = sf_read_float(infile, new_wave.sample, (new_wave.sfinfo.channels * new_wave.sfinfo.frames));

    /* Close input and output files. */
    sf_close(infile);

    waves.push_back(new_wave);
    cout << "Loaded waveform - " << name << endl;
}

//---------------------------------------------------------------

void init_src()
{
    int channels = 1;
    src_test = src_new(SRC_LINEAR, channels, &error_sc);

    if (src_test) {
        cout << endl
             << "Secret rabbit was successful!" << endl;
    } else {
        cout << endl
             << "Sorry, the secret rabbit has watership downed!" << endl;
    }
}

//---------------------------------------------------------------

class osc {
public:
    int state;
    int delay;
    float frequency;
    float old_frequency;
    float volume;
    float pulsewidth;
    int out_position;
    int out_length;
    int wave;
    float inertia;
    float buffer[65536 * 8];
    float lfo1_amount;
    float lfo2_amount;

    //------------------------------

    void set_freq(float f)
    {

        frequency = f;

        if (lfo1_amount > 0) {
            f = f + (pow(1.059463, (lfo1_amount) * (get_dco_out(3))) - 1);
        }

        if (lfo2_amount > 0) {
            f = f + (pow(1.059463, (lfo2_amount) * (get_dco_out(4))) - 1);
        }

        if (f < 0.1) {
            f = 0.1;
        }

        if (inertia == 0.5) {
            old_frequency = f;
        }

        if (inertia < 0.5) {
            if (old_frequency > f) {
                old_frequency = old_frequency - pow(0.01, inertia);
                if (old_frequency < f)
                    old_frequency = f;
            }

            if (old_frequency < f) {
                old_frequency = old_frequency + pow(0.01, inertia);
                if (old_frequency > f)
                    old_frequency = f;
            }

            f = old_frequency;
        }

        src_reset(src_test);
        mySampleData.data_in = waves[wave].sample;
        mySampleData.input_frames = waves[wave].sfinfo.frames;
        mySampleData.data_out = buffer;
        mySampleData.output_frames = waves[wave].sfinfo.frames / f;
        mySampleData.src_ratio = (float)1 / f;
        src_process(src_test, &mySampleData);
        out_length = mySampleData.output_frames_gen;
    }

    //----------------------

    void set_lfo1_amount(float value)
    {
        lfo1_amount = value;
    }

    //----------------------

    void set_lfo2_amount(float value)
    {
        lfo2_amount = value;
    }
};

osc new_dco;
vector<osc> dcos;

//-------------------------------------------------

void add_dco()
{
    new_dco.wave = 1;
    new_dco.out_position = 0;
    new_dco.state = false; // default state is new oscillator off
    new_dco.lfo1_amount = 0;
    new_dco.lfo2_amount = 0;
    new_dco.inertia = 0.5;
    dcos.push_back(new_dco);
}

//------------------------------

void dco_on(int number)
{
    dcos[number].state = true;
}

//------------------------------

void dco_off(int number)
{
    dcos[number].state = false;
}

//--------------------------------
int get_dco_state(int number)
{
    return dcos[number].state;
}

//-------------------------------------------------

float get_dco_out(int number)
{
    int pos = dcos[number].out_position;

    if (dcos[number].wave == 4) // it's white noise so output floating point random values in the range -1 to 1
    {
        return 0; //((float)(rand() % 65536)/32768)-1;
    }

    return dcos[number].buffer[pos];
}

//----------------------------------------------------

void dco_cycle(int number)
{
    dcos[number].out_position++;

    if (dcos[0].out_position == 0 && master_sync == 1) {
        dcos[1].out_position = 0;
    }

    if (dcos[number].out_position >= dcos[number].out_length) {

        dcos[number].out_position = 0;
        if (number == 0)
            update_dco1_tuning();
        if (number == 1)
            update_dco2_tuning();
        if (number == 2)
            update_dco3_tuning();
    }

    return;
}

//-------------------------------------------------

float get_dco_frequency(int number)
{
    return dcos[number].frequency;
}

//-------------------------------------------------

void set_dco_frequency(int number, float frequency)
{
    dcos[number].set_freq(frequency);
}

//-------------------------------------------------

float get_dco_volume(int number)
{
    return dcos[number].volume;
}

//-------------------------------------------------

void set_dco_volume(int number, float volume)
{
    dcos[number].volume = volume;
}

//-------------------------------------------------

int get_dco_wave(int number)
{
    return dcos[number].wave;
}

//-------------------------------------------------

void set_dco_wave(int number, int wave)
{
    dcos[number].wave = wave;
}

//-------------------------------------------------

float get_dco_inertia(int number)
{
    return dcos[number].inertia;
}

//-------------------------------------------------

void set_dco_inertia(int number, float value)
{
    dcos[number].inertia = value;
}

//-------------------------------------------------

void set_dco_lfo1_amount(int number, float value)
{
    dcos[number].set_lfo1_amount(value);
}

//-------------------------------------------------

void set_dco_lfo2_amount(int number, float value)
{
    dcos[number].set_lfo2_amount(value);
}

//-------------------------------------------------
// SET GLOBAL TUNING

void set_tuning(float frequency)
{
    master_note = frequency;
    update_dco1_tuning();
    update_dco2_tuning();
    update_dco3_tuning();
}

//------------------------------------------------
// TOGGLE SYNC MODE

void toggle_sync()
{
    master_sync = 1 - master_sync;
    cout << master_sync << endl;
}

//------------------------------------------------
// SET SYNC MODE

void set_sync_mode(int value)
{
    master_sync = value;
}

//------------------------------------------------
// RETURN SYNC MODE

int get_sync_mode()
{
    return master_sync;
}

//------------------------------------------------
// GET OSCILLATOR LFO1 AMOUNT

float get_dco_lfo1_amount(int number)
{
    return dcos[number].lfo1_amount;
}

//------------------------------------------------
// GET OSCILLATOR LFO2 AMOUNT

float get_dco_lfo2_amount(int number)
{
    return dcos[number].lfo2_amount;
}

//------------------------------------------------

void update_dco1_tuning()
{
    set_dco_frequency(0, pow(1.059463, 12 * octave1 + (pitch1 + master_note)));
}

//------------------------------------------------

void update_dco2_tuning()
{
    float env1_amount = 0;

    if (adsr_osc2_amount1 > 0) {
        env1_amount = envelope1_out(adsr_osc2_amount1, 1);
    }

    set_dco_frequency(1, pow(1.059463, 12 * octave2 + (pitch2 + master_note + env1_amount)));
}

//------------------------------------------------

void update_dco3_tuning()
{
    float env2_amount = 0;

    if (adsr_osc3_amount2 > 0) {
        env2_amount = envelope2_out(adsr_osc3_amount2, 1);
    }
    set_dco_frequency(2, pow(1.059463, 12 * octave3 + (pitch3 + master_note + env2_amount)));
}
