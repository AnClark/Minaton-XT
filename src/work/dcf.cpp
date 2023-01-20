
#include "dco.h"
#include <iostream>
#include <math.h>

using namespace std;

float master_frequency = 0;
float master_resonance = 0;
float lfo1_amount = 0;
float lfo2_amount = 0;

float buf0 = 0, buf1 = 0, out;

float f = 0, p = 0, q = 0; // filter coefficients
float b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0; // filter buffers (beware denormals!)
float t1 = 0, t2 = 0; // temporary buffers

static float selectivity = 90, gain1 = 0, gain2 = 1, ratio = 7, cap = 0;

float dcf(float in, float frequency, float resonance)
{

    frequency = frequency + master_frequency;
    resonance = resonance + master_resonance;

    if (lfo1_amount > 0) {
        frequency = frequency + (pow(1.059463, (lfo1_amount) * (get_dco_out(3) / 4)) - 1);
    }

    if (lfo2_amount > 0) {
        frequency = frequency + (pow(1.059463, (lfo2_amount) * (get_dco_out(4) / 4)) - 1);
    }

    if (frequency > 0.6) {
        frequency = 0.6;
    }
    if (frequency < 0.001) {
        frequency = 0.001;
    }

    gain1 = 1.0 / (selectivity + 1.0);

    cap = (in + cap * selectivity) * gain1;
    in = ((in + cap * ratio) * gain2);

    // Moog 24 dB/oct resonant lowpass VCF
    // References: CSound source code, Stilson/Smith CCRMA paper.
    // Modified by paul.kellett@maxim.abel.co.uk July 2000

    // Set coefficients given frequency & resonance [0.0...1.0]

    q = 1.0f - frequency;
    p = frequency + 0.8f * frequency * q;
    f = p + p - 1.0f;
    q = resonance * (1.0f + 0.5f * q * (1.0f - q + 5.6f * q * q));

    // Filter (in [-1.0...+1.0])

    in -= q * b4; // feedback
    t1 = b1;
    b1 = (in + b0) * p - b1 * f;
    t2 = b2;
    b2 = (b1 + t1) * p - b2 * f;
    t1 = b3;
    b3 = (b2 + t2) * p - b3 * f;
    b4 = (b3 + t1) * p - b4 * f;
    b4 = b4 - b4 * b4 * b4 * 0.166667f; // clipping
    b0 = in;

    // Lowpass  output:  b4
    // Highpass output:  in - b4;
    // Bandpass output:  3.0f * (b3 - b4);

    return b4;
}

//---------------------------------------------------------
//----------- SIMPLE DIGITAL RESONANCE FILTER

float dcf_simple(float in, float f, float q)
{
    // set feedback amount given f and q between 0 and 1
    float fb = q + q / (1.0 - f);

    // for each sample...
    buf0 = buf0 + f * (in - buf0 + fb * (buf0 - buf1));
    buf1 = buf1 + f * (buf0 - buf1);
    out = buf1;

    return out;
}

//---------------------------------------------------------
//----------- SET FILTER FREQUENCY

void set_dcf_frequency(float freq)
{
    master_frequency = freq;
    return;
}

//------------------------------------------------------------
//------------ SET FILTER RESONANCE

void set_dcf_resonance(float res)
{
    master_resonance = res;
    return;
}

//-------------------------------------------------

void set_lfo1_dcf_amount(float value)
{
    lfo1_amount = value;
}

//-------------------------------------------------

void set_lfo2_dcf_amount(float value)
{
    lfo2_amount = value;
}
