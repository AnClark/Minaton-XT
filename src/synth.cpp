

//=========================================================
//-- DCO ROUTINES
//=========================================================

#include "synth.hpp"

//------------------------------

void minaton_synth::init()
{

    stringstream ss;

    ss.str("");
    ss << bundle_path << "waves/440Hz-sine.wav";
    add_wave(0, "sine", ss.str());
    ss.str("");
    ss << bundle_path << "waves/440Hz-saw.wav";
    add_wave(1, "sine", ss.str());
    ss.str("");
    ss << bundle_path << "waves/440Hz-square.wav";
    add_wave(2, "sine", ss.str());
    ss.str("");
    ss << bundle_path << "waves/440Hz-triangle.wav";
    add_wave(3, "sine", ss.str());
    ss.str("");
    ss << bundle_path << "waves/440Hz-triangle.wav"; // dummy wave white noise generator is #4
    add_wave(4, "sine", ss.str());
    ss.str("");
    ss << bundle_path << "waves/1Hz-sine.wav";
    add_wave(5, "sine", ss.str());
    ss.str("");
    ss << bundle_path << "waves/1Hz-saw.wav";
    add_wave(6, "sine", ss.str());
    ss.str("");
    ss << bundle_path << "waves/1Hz-square.wav";
    add_wave(7, "sine", ss.str());

    init_src();

    add_dco(DCO_ONE);
    set_dco_wave(DCO_ONE, WAVE_SAW);
    set_dco_frequency(DCO_ONE, 1);

    add_dco(DCO_TWO);
    set_dco_wave(DCO_TWO, WAVE_SAW);
    set_dco_frequency(DCO_TWO, 1);

    add_dco(DCO_THREE);
    set_dco_wave(DCO_THREE, WAVE_SAW);
    set_dco_frequency(DCO_THREE, 1);

    add_dco(LFO_ONE);
    set_dco_wave(LFO_ONE, WAVE_SLOW_SINE);
    set_dco_frequency(LFO_ONE, 0.0001);

    add_dco(LFO_TWO);
    set_dco_wave(LFO_TWO, WAVE_SLOW_SINE);
    set_dco_frequency(LFO_TWO, 0.0001);

    init_dcas();
}

//--------------------------------------------------------------------

void minaton_synth::cleanup()
{

    // cout << "freeing wavetables........." << endl;

    for (int x = 0; x < WAVES_COUNT; x++) {
        // cout << waves_name[x] << endl;
        free(waves[x].sample);
    }

    // cout << "freeing the secret rabbit........" << endl;

    src_delete(src_test);
}
//----------------- INIT DCF -----------------------------------------------------------------

void minaton_synth::init_dcf()
{
    float master_frequency_left = 0;
    float master_resonance_left = 0;
    float lfo1_amount_left = 0;
    float lfo2_amount_left = 0;

    float buf0_left = 0, buf1_left = 0, out_left;

    float f_left = 0, p_left = 0, q_left = 0; // filter coefficients
    float b0_left = 0, b1_left = 0, b2_left = 0, b3_left = 0, b4_left = 0; // filter buffers (beware denormals!)
    float t1_left = 0, t2_left = 0; // temporary buffers
}

//----------------------------------------------------------------------------------------------------------------

void minaton_synth::set_freq(int dco_number, float f)
{
    minaton_dco& target_dco = this->dco[dco_number];

    target_dco.frequency = f;

    if (target_dco.lfo1_amount > 0) {
        f = f + (pow(1.059463, (target_dco.lfo1_amount) * (get_dco_out(3))) - 1);
    }

    if (target_dco.lfo2_amount > 0) {
        f = f + (pow(1.059463, (target_dco.lfo2_amount) * (get_dco_out(4))) - 1);
    }

    if (f < 0.1) {
        f = 0.1;
    }

    if (target_dco.inertia == 0.5) {
        target_dco.old_frequency = f;
    }

    if (target_dco.inertia < 0.5) {
        if (target_dco.old_frequency > f) {
            target_dco.old_frequency = target_dco.old_frequency - target_dco.inertia;
            if (target_dco.old_frequency < f)
                target_dco.old_frequency = f;
        }

        if (target_dco.old_frequency < f) {
            target_dco.old_frequency = target_dco.old_frequency + target_dco.inertia;
            if (target_dco.old_frequency > f)
                target_dco.old_frequency = f;
        }

        f = target_dco.old_frequency;
    }

    float* buffer = &dco1_buffer[0];
    if (dco_number == 1)
        buffer = &dco2_buffer[0];
    if (dco_number == 2)
        buffer = &dco3_buffer[0];
    if (dco_number == 3)
        buffer = &lfo1_buffer[0];
    if (dco_number == 4)
        buffer = &lfo2_buffer[0];

    src_reset(src_test);
    mySampleData.data_in = waves[target_dco.wave_id].sample;
    mySampleData.input_frames = waves[target_dco.wave_id].sfinfo.frames;
    mySampleData.data_out = buffer;
    mySampleData.output_frames = waves[target_dco.wave_id].sfinfo.frames / f;
    mySampleData.src_ratio = (float)1 / f;
    src_process(src_test, &mySampleData);
    target_dco.out_length = mySampleData.output_frames_gen;
}

//----------------------

void minaton_synth::set_lfo1_amount(int dco_number, float value)
{
    this->dco[dco_number].lfo1_amount = value;
}

//----------------------

void minaton_synth::set_lfo2_amount(int dco_number, float value)
{
    this->dco[dco_number].lfo2_amount = value;
}

//---------------- Add new waveform ------------------

int minaton_synth::add_wave(int number, string name, string file)
{
    minaton_wave& current_wave = this->waves[number];
    int readcount;

    current_wave.name = name;

    if (!(infile = sf_open(file.c_str(), SFM_READ, &current_wave.sfinfo))) {
        cout << "Unable to open input file - " << file.c_str() << " " << sf_strerror(infile) << endl;
        sf_perror(NULL);
        return 1;
    }

    current_wave.sample = new float[current_wave.sfinfo.frames * current_wave.sfinfo.channels];
    readcount = sf_read_float(infile, current_wave.sample, (current_wave.sfinfo.channels * current_wave.sfinfo.frames));

    sf_close(infile);

    // cout << "Loaded waveform - " << name << waves_sfinfo[number_of_waves].frames << endl;

    return readcount;
}

//---------------------------------------------------------------

void minaton_synth::init_src()
{
    int channels = 1;
    src_test = src_new(SRC_LINEAR, channels, &error_sc);

    if (!src_test) {
        cout << endl
             << "Sorry, the secret rabbit has watership downed!" << endl;
    }
}

//-------------------------------------------------

void minaton_synth::add_dco(int number)
{
    minaton_dco& current_dco = this->dco[number];

    current_dco.wave_id = 1;
    current_dco.out_position = 0;
    current_dco.state = false; // default state is new oscillator off
    current_dco.lfo1_amount = 0;
    current_dco.lfo2_amount = 0;
    current_dco.inertia = 0.5;
}

//------------------------------

void minaton_synth::dco_on(int number)
{
    this->dco[number].state = true;
}

//------------------------------

void minaton_synth::dco_off(int number)
{
    this->dco[number].state = false;
}

//--------------------------------
bool minaton_synth::get_dco_state(int number)
{
    return this->dco[number].state;
}

//-------------------------------------------------

float minaton_synth::get_dco_out(int dco_number)
{
    const minaton_dco& target_dco = this->dco[dco_number];

    int pos = target_dco.out_position;

    if (target_dco.wave_id == 4) // it's white noise so output floating point random values in the range -1 to 1
    {
        return ((float)(rand() % 65536) / 32768) - 1;
    }

    if (dco_number == 0)
        return dco1_buffer[pos];
    if (dco_number == 1)
        return dco2_buffer[pos];
    if (dco_number == 2)
        return dco3_buffer[pos];
    if (dco_number == 3)
        return lfo1_buffer[pos];
    if (dco_number == 4)
        return lfo2_buffer[pos];

    return 0;
}

//----------------------------------------------------

float minaton_synth::dco_cycle(int dco_number)
{
    minaton_dco& target_dco = this->dco[dco_number];

    target_dco.out_position++;

    if (dco[0].out_position == 0 && master_sync == 1) {
        dco[1].out_position = 0;
    }

    if (target_dco.out_position >= target_dco.out_length) {

        target_dco.out_position = 0;
        if (dco_number == 0)
            update_dco1_tuning();
        if (dco_number == 1)
            update_dco2_tuning();
        if (dco_number == 2)
            update_dco3_tuning();
    }

    int pos = target_dco.out_position;

    if (target_dco.wave_id == 4) // it's white noise so output floating point random values in the range -1 to 1
    {
        return ((float)(rand() % 65536) / 32768) - 1;
    }

    if (dco_number == 0)
        return dco1_buffer[pos];
    if (dco_number == 1)
        return dco2_buffer[pos];
    if (dco_number == 2)
        return dco3_buffer[pos];
    if (dco_number == 3)
        return lfo1_buffer[pos];
    if (dco_number == 4)
        return lfo2_buffer[pos];

    return 0;
}

//-------------------------------------------------

float minaton_synth::get_dco_frequency(int number)
{
    return this->dco[number].frequency;
}

//-------------------------------------------------

void minaton_synth::set_dco_frequency(int number, float frequency)
{
    set_freq(number, frequency);
}

//-------------------------------------------------

float minaton_synth::get_dco_volume(int number)
{
    return this->dco[number].volume;
}

//-------------------------------------------------

void minaton_synth::set_dco_volume(int number, float volume)
{
    this->dco[number].volume = volume;
}

//-------------------------------------------------

int minaton_synth::get_dco_wave(int number)
{
    return this->dco[number].wave_id;
}

//-------------------------------------------------

void minaton_synth::set_dco_wave(int number, int wave)
{
    this->dco[number].wave_id = wave;
}

//-------------------------------------------------

float minaton_synth::get_dco_inertia(int number)
{
    return this->dco[number].inertia;
}

//-------------------------------------------------

void minaton_synth::set_dco_inertia(int number, float value)
{
    this->dco[number].inertia = value;
}

//-------------------------------------------------

void minaton_synth::set_dco_lfo1_amount(int number, float value)
{
    set_lfo1_amount(number, value);
}

//-------------------------------------------------

void minaton_synth::set_dco_lfo2_amount(int number, float value)
{
    set_lfo2_amount(number, value);
}

//-------------------------------------------------
// SET GLOBAL TUNING

void minaton_synth::set_tuning(float frequency)
{
    master_note = frequency;
    update_dco1_tuning();
    update_dco2_tuning();
    update_dco3_tuning();
}

//------------------------------------------------
// TOGGLE SYNC MODE

void minaton_synth::toggle_sync()
{
    master_sync = 1 - master_sync;
}

//------------------------------------------------
// SET SYNC MODE

void minaton_synth::set_sync_mode(int value)
{
    master_sync = value;
}

//------------------------------------------------
// RETURN SYNC MODE

int minaton_synth::get_sync_mode()
{
    return master_sync;
}

//------------------------------------------------
// GET OSCILLATOR LFO1 AMOUNT

float minaton_synth::get_dco_lfo1_amount(int number)
{
    return this->dco[number].lfo1_amount;
}

//------------------------------------------------
// GET OSCILLATOR LFO2 AMOUNT

float minaton_synth::get_dco_lfo2_amount(int number)
{
    return this->dco[number].lfo2_amount;
}

//------------------------------------------------

void minaton_synth::update_dco1_tuning()
{
    set_dco_frequency(0, pow(1.059463, 12 * octave1 + (pitch1 + master_note)));

    if (master_sync && dco[1].frequency < dco[0].frequency) {
        set_dco_frequency(1, pow(1.059463, 12 * octave2 + (pitch2 + master_note)));
    }
}

//------------------------------------------------

void minaton_synth::update_dco2_tuning()
{
    float env1_amount = 0;

    if (adsr_osc2_amount1 > 0) {
        env1_amount = envelope1_out(adsr_osc2_amount1, 1);
    }

    set_dco_frequency(1, pow(1.059463, 12 * octave2 + (pitch2 + master_note + env1_amount)));
}

//------------------------------------------------

void minaton_synth::update_dco3_tuning()
{
    float env2_amount = 0;

    if (adsr_osc3_amount2 > 0) {
        env2_amount = envelope2_out(adsr_osc3_amount2, 1);
    }
    set_dco_frequency(2, pow(1.059463, 12 * octave3 + (pitch3 + master_note + env2_amount)));
}

//=========================================================
//-- DCA ROUTINES
//=========================================================

void minaton_synth::init_dcas()
{
    envelope1.attack = 0;
    envelope1.decay = 0;
    envelope1.sustain = 0;
    envelope1.release = 0;
    envelope1.state = ENV_STATE_DORMANT;
    envelope1.level = 0;

    envelope2.attack = 0;
    envelope2.decay = 0;
    envelope2.sustain = 0;
    envelope2.release = 0;
    envelope2.state = ENV_STATE_DORMANT;
    envelope2.level = 0;
}

//---------------------------------------------------------

float minaton_synth::envelope1_out(float input, float amount)
{
    input = input * (envelope1.level / amount);
    return input;
}

//---------------------------------------------------------

float minaton_synth::envelope2_out(float input, float amount)
{
    input = input * (envelope2.level / amount);
    return input;
}

//--------------------------------------------------------

void minaton_synth::dca_update()
{

    //-----------------------------------------------
    // PROCESS ENVELOPE 1

    if (envelope1.state == ENV_STATE_ATTACK) {
        if (envelope1.level < 1) {
            if (envelope1.attack == 0) {
                envelope1.state = ENV_STATE_DECAY;
            }
            envelope1.level += pow(0.8, envelope1.attack);
        } else {
            envelope1.state = ENV_STATE_DECAY;
        }
    }

    if (envelope1.state == ENV_STATE_DECAY) {
        if (envelope1.level > envelope1.sustain) {
            envelope1.level -= pow(0.8, envelope1.decay);
        } else {
            envelope1.state = ENV_STATE_WAIT;
            envelope1.level = envelope1.sustain;
        }
    }

    if (envelope1.state == ENV_STATE_RELEASE) {
        if (envelope1.level > 0) {
            envelope1.level -= pow(0.8, envelope1.release);
        } else {
            envelope1.state = ENV_STATE_DORMANT;
            envelope1.level = 0;
        }
    }

    //-----------------------------------------------
    // PROCESS ENVELOPE 2

    if (envelope2.state == ENV_STATE_ATTACK) {
        if (envelope2.level < 1) {
            if (envelope2.attack == 0) {
                envelope2.level = 1;
            }
            envelope2.level += pow(0.8, envelope2.attack);
        } else {
            envelope2.state = ENV_STATE_DECAY;
        }
    }

    if (envelope2.state == ENV_STATE_DECAY) {
        if (envelope2.level > envelope2.sustain) {
            envelope2.level -= pow(0.8, envelope2.decay);
        } else {
            envelope2.state = ENV_STATE_WAIT;
            envelope2.level = envelope2.sustain;
        }
    }

    if (envelope2.state == ENV_STATE_RELEASE) {
        if (envelope2.level > 0) {
            envelope2.level -= pow(0.8, envelope2.release);
        } else {
            envelope2.state = ENV_STATE_DORMANT;
            envelope2.level = 0;
        }
    }
}

//---------------------------------------------------------
// SET ENVELOPE 1 - attack

void minaton_synth::set_envelope1_attack(float value)
{
    envelope1.attack = value;
}

//--------------------------------------------------------
// SET ENVELOPE 1 - decay

void minaton_synth::set_envelope1_decay(float value)
{
    envelope1.decay = value;
}

//---------------------------------------------------------
// SET ENVELOPE 1 - sustain

void minaton_synth::set_envelope1_sustain(float value)
{
    envelope1.sustain = value;
}

//--------------------------------------------------------
// SET ENVELOPE 1 - release

void minaton_synth::set_envelope1_release(float value)
{
    envelope1.release = value;
}

//--------------------------------------------------------
// TRIGGER ADSR 1 - RESET COUNTER

void minaton_synth::trigger_envelope1()
{
    envelope1.state = ENV_STATE_ATTACK;
    // envelope1.level = 0; // meant to set the level back to zero to retrigger on legato mode, but causes clicks, need to fade it instead, work for later
}

//--------------------------------------------------------
// RELEASE ADSR 1 - RESET COUNTER

void minaton_synth::release_envelope1()
{
    envelope1.state = ENV_STATE_RELEASE;
}

//---------------------------------------------------------
// SET ENVELOPE 2 - attack

void minaton_synth::set_envelope2_attack(float value)
{
    envelope2.attack = value;
    // envelope2.level = 0;
}

//--------------------------------------------------------
// SET ENVELOPE 2 - decay

void minaton_synth::set_envelope2_decay(float value)
{
    envelope2.decay = value;
}

//---------------------------------------------------------
// SET ENVELOPE 2 - sustain

void minaton_synth::set_envelope2_sustain(float value)
{
    envelope2.sustain = value;
}

//--------------------------------------------------------
// SET ENVELOPE 2 - release

void minaton_synth::set_envelope2_release(float value)
{
    envelope2.release = value;
}

//--------------------------------------------------------
// TRIGGER ADSR 2 - RESET COUNTER

void minaton_synth::trigger_envelope2()
{
    envelope2.state = 0;
    envelope2.level = 0;
}

//--------------------------------------------------------
// RELEASE ADSR 2 - RESET COUNTER

void minaton_synth::release_envelope2()
{
    envelope2.state = ENV_STATE_RELEASE;
}

//---------------------------------------------------------
// TOGGLE LEGATO MODE

void minaton_synth::toggle_legato()
{
    master_legato = 1 - master_legato;
}

//--------------------------------------------------------
// SET LEGATO MODE
void minaton_synth::set_legato(int value)
{
    master_legato = value;
}

//--------------------------------------------------------
// RETURN LEGATO MODE
int minaton_synth::get_legato()
{
    return master_legato;
}

//=========================================================
//-- DCF ROUTINES
//=========================================================

float minaton_synth::dcf_left(float in, float frequency, float resonance)
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

    gain1_left = 1.0 / (selectivity + 1.0);

    cap_left = (in + cap_left * selectivity) * gain1_left;
    in = ((in + cap_left * ratio_left) * gain2_left);

    // Moog 24 dB/oct resonant lowpass VCF
    // References: CSound source code, Stilson/Smith CCRMA paper.
    // Modified by paul.kellett@maxim.abel.co.uk July 2000

    // Set coefficients given frequency & resonance [0.0...1.0]

    q_left = 1.0f - frequency;
    p_left = frequency + 0.8f * frequency * q_left;
    f_left = p_left + p_left - 1.0f;
    q_left = resonance * (1.0f + 0.5f * q_left * (1.0f - q_left + 5.6f * q_left * q_left));

    // Filter (in [-1.0...+1.0])

    in -= q_left * b4_left; // feedback
    t1_left = b1_left;
    b1_left = (in + b0_left) * p_left - b1_left * f_left;
    t2_left = b2_left;
    b2_left = (b1_left + t1_left) * p_left - b2_left * f_left;
    t1_left = b3_left;
    b3_left = (b2_left + t2_left) * p_left - b3_left * f_left;
    b4_left = (b3_left + t1_left) * p_left - b4_left * f_left;
    b4_left = b4_left - b4_left * b4_left * b4_left * 0.166667f; // clipping
    b0_left = in;

    // Lowpass  output:  b4
    // Highpass output:  in - b4;
    // Bandpass output:  3.0f * (b3 - b4);

    return b4_left;
}

//------------------- FILTER RIGHT

float minaton_synth::dcf_right(float in, float frequency, float resonance)
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

    gain1_right = 1.0 / (selectivity + 1.0);

    cap_right = (in + cap_right * selectivity) * gain1_right;
    in = ((in + cap_right * ratio_right) * gain2_right);

    // Moog 24 dB/oct resonant lowpass VCF
    // References: CSound source code, Stilson/Smith CCRMA paper.
    // Modified by paul.kellett@maxim.abel.co.uk July 2000

    // Set coefficients given frequency & resonance [0.0...1.0]

    q_right = 1.0f - frequency;
    p_right = frequency + 0.8f * frequency * q_right;
    f_right = p_right + p_right - 1.0f;
    q_right = resonance * (1.0f + 0.5f * q_right * (1.0f - q_right + 5.6f * q_right * q_right));

    // Filter (in [-1.0...+1.0])

    in -= q_right * b4_right; // feedback
    t1_right = b1_right;
    b1_right = (in + b0_right) * p_right - b1_right * f_right;
    t2_right = b2_right;
    b2_right = (b1_right + t1_right) * p_right - b2_right * f_right;
    t1_right = b3_right;
    b3_right = (b2_right + t2_right) * p_right - b3_right * f_right;
    b4_right = (b3_right + t1_right) * p_right - b4_right * f_right;
    b4_right = b4_right - b4_right * b4_right * b4_right * 0.166667f; // clipping
    b0_right = in;

    // Lowpass  output:  b4
    // Highpass output:  in - b4;
    // Bandpass output:  3.0f * (b3 - b4);

    return b4_right;
}

//---------------------------------------------------------
//----------- SET FILTER FREQUENCY

void minaton_synth::set_dcf_frequency(float freq)
{
    master_frequency = freq;
    return;
}

//------------------------------------------------------------
//------------ SET FILTER RESONANCE

void minaton_synth::set_dcf_resonance(float res)
{
    master_resonance = res;
    return;
}

//-------------------------------------------------

void minaton_synth::set_lfo1_dcf_amount(float value)
{
    lfo1_amount = value;
}

//-------------------------------------------------

void minaton_synth::set_lfo2_dcf_amount(float value)
{
    lfo2_amount = value;
}

//=========================================================
//-- FILE ROUTINES
//=========================================================

void minaton_synth::set_bundle_path(const char* new_bundle_path)
{
    this->bundle_path = new_bundle_path;
}
