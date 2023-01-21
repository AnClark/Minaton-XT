#include "MinatonPlugin.h"
#include "DistrhoPlugin.hpp"
#include "MinatonParams.h"

START_NAMESPACE_DISTRHO

MinatonPlugin::MinatonPlugin()
    : Plugin(MinatonParamId::PARAM_COUNT, 0, 0) // parameters, programs, states
{
    // TODO: Integrate waves as internal resources
    fSynthesizer->set_bundle_path("/home/anclark/Sources/minaton/src/");

    fSynthesizer->init();
    fSynthesizer->init_dcas();
    fSynthesizer->init_dcf();

    fSynthesizer->attack = 0;
    fSynthesizer->decay = 1;
    fSynthesizer->wait = 2;
    fSynthesizer->release = 3;
    fSynthesizer->dormant = 4;
    fSynthesizer->selectivity = 90;
    fSynthesizer->gain1_left = 0;
    fSynthesizer->gain2_left = 1;
    fSynthesizer->ratio_left = 7;
    fSynthesizer->cap_left = 0;
    fSynthesizer->gain1_right = 0;
    fSynthesizer->gain2_right = 1;
    fSynthesizer->ratio_right = 7;
    fSynthesizer->cap_right = 0;

    fSynthesizer->release_envelope1();
    fSynthesizer->release_envelope2();
    fSynthesizer->envelope1.level = 0;
    fSynthesizer->envelope2.level = 0;
}

MinatonPlugin::~MinatonPlugin()
{
}

void MinatonPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    parameter.hints = kParameterIsAutomatable;

    parameter.name = MinatonParams::paramName(MinatonParamId(index));
    parameter.shortName = MinatonParams::paramSymbol(MinatonParamId(index));
    parameter.symbol = MinatonParams::paramSymbol(MinatonParamId(index));
    parameter.ranges.min = MinatonParams::paramMinValue(MinatonParamId(index));
    parameter.ranges.max = MinatonParams::paramMaxValue(MinatonParamId(index));
    parameter.ranges.def = MinatonParams::paramDefaultValue(MinatonParamId(index));

    if (MinatonParams::paramInt(MinatonParamId(index)))
        parameter.hints |= kParameterIsInteger;
    else if (MinatonParams::paramBool(MinatonParamId(index)))
        parameter.hints |= kParameterIsBoolean;

    setParameterValue(index, parameter.ranges.def);
}

float MinatonPlugin::getParameterValue(uint32_t index) const
{
    return fParameters[index];
}

void MinatonPlugin::setParameterValue(uint32_t index, float value)
{
    fParameters[index] = value;
    _applySynthParameters();
}

void MinatonPlugin::activate()
{
}

void MinatonPlugin::run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount)
{
    uint32_t event_index;

    for (event_index = 0; event_index < midiEventCount; ++event_index) {
        _processMidi(midiEvents[event_index].data, midiEvents[event_index].size);
    }

    //	++control_delay;

    // if (control_delay>20)
    //{
    //		control_delay = 0;

    for (unsigned int x = 0; x < frames; x++) {
        _processAudioFrame(outputs[0], outputs[1], x);
    }
}

void MinatonPlugin::sampleRateChanged(double newSampleRate)
{
}

void MinatonPlugin::_applySynthParameters()
{
    fSynthesizer->master_volume = fParameters[PARAM_MASTER_VOLUME]; // p(p_master_volume)[0];
    fSynthesizer->set_legato(fParameters[PARAM_LEGATO]);
    fSynthesizer->set_sync_mode(fParameters[PARAM_SYNC]);

    fSynthesizer->dco_wave[0] = fParameters[PARAM_WAVE_ONE];
    fSynthesizer->dco_inertia[0] = fParameters[PARAM_INERTIA_ONE];

    fSynthesizer->dco_wave[1] = fParameters[PARAM_WAVE_TWO];
    fSynthesizer->dco_inertia[1] = fParameters[PARAM_INERTIA_TWO];

    fSynthesizer->dco_wave[2] = fParameters[PARAM_WAVE_THREE];
    fSynthesizer->dco_inertia[2] = fParameters[PARAM_INERTIA_THREE];

    fSynthesizer->set_freq(3, fParameters[PARAM_LFO1_SPEED]);
    fSynthesizer->dco_wave[3] = fParameters[PARAM_LFO1_WAVE];

    fSynthesizer->set_freq(4, fParameters[PARAM_LFO2_SPEED]);
    fSynthesizer->dco_wave[4] = fParameters[PARAM_LFO2_WAVE];

    fSynthesizer->dco_lfo1_amount[0] = fParameters[PARAM_LFO1_DCO1_PITCH];
    fSynthesizer->dco_lfo1_amount[1] = fParameters[PARAM_LFO1_DCO2_PITCH];
    fSynthesizer->dco_lfo1_amount[2] = fParameters[PARAM_LFO1_DCO3_PITCH];
    fSynthesizer->lfo1_amount = fParameters[PARAM_LFO1_DCF];

    fSynthesizer->dco_lfo2_amount[0] = fParameters[PARAM_LFO2_DCO1_PITCH];
    fSynthesizer->dco_lfo2_amount[1] = fParameters[PARAM_LFO2_DCO2_PITCH];
    fSynthesizer->dco_lfo2_amount[2] = fParameters[PARAM_LFO2_DCO3_PITCH];
    fSynthesizer->lfo2_amount = fParameters[PARAM_LFO2_DCF];

    fSynthesizer->envelope1.attack = fParameters[PARAM_ATTACK_ONE];
    fSynthesizer->envelope1.decay = fParameters[PARAM_DECAY_ONE];
    fSynthesizer->envelope1.sustain = fParameters[PARAM_SUSTAIN_ONE];
    fSynthesizer->envelope1.release = fParameters[PARAM_RELEASE_ONE];
    fSynthesizer->adsr_amp_amount1 = fParameters[PARAM_ADSR1_AMP_AMOUNT];
    fSynthesizer->adsr_osc2_amount1 = fParameters[PARAM_ADSR1_OSC2_AMOUNT];

    fSynthesizer->envelope2.attack = fParameters[PARAM_ATTACK_TWO];
    fSynthesizer->envelope2.decay = fParameters[PARAM_DECAY_TWO];
    fSynthesizer->envelope2.sustain = fParameters[PARAM_SUSTAIN_TWO];
    fSynthesizer->envelope2.release = fParameters[PARAM_RELEASE_TWO];
    fSynthesizer->adsr_filter_amount2 = fParameters[PARAM_ADSR2_DCF_AMOUNT];
    fSynthesizer->adsr_osc3_amount2 = fParameters[PARAM_ADSR2_OSC3_AMOUNT];

    fSynthesizer->master_frequency = fParameters[PARAM_FREQUENCY] / 9;
    fSynthesizer->master_resonance = fParameters[PARAM_RESONANCE] / 4;

    if (fParameters[PARAM_ACTIVE_ONE]) {
        fSynthesizer->octave1 = fParameters[PARAM_OCTAVE_ONE];
        fSynthesizer->pitch1 = fParameters[PARAM_FINETUNE_ONE];
    }

    if (fParameters[PARAM_ACTIVE_TWO]) {
        fSynthesizer->pitch2 = fParameters[PARAM_FINETUNE_TWO];
        fSynthesizer->octave2 = fParameters[PARAM_OCTAVE_TWO];
    }

    if (fParameters[PARAM_ACTIVE_THREE]) {
        fSynthesizer->octave3 = fParameters[PARAM_OCTAVE_THREE];
        fSynthesizer->pitch3 = fParameters[PARAM_FINETUNE_THREE];
    }
}

Plugin* createPlugin()
{
    return new MinatonPlugin();
}

END_NAMESPACE_DISTRHO
