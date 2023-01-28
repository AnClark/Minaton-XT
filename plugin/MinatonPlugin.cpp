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
    fSynthesizer->cleanup();
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
    return _obtainSynthParameter(MinatonParamId(index));
}

void MinatonPlugin::setParameterValue(uint32_t index, float value)
{
    _applySynthParameter(MinatonParamId(index), value);
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

    // Check if no oscillators enabled or volume at zero.
    // Once true, clear ring buffer and return.
    static const uint32_t minimum_volume = MinatonParams::paramMinValue(PARAM_MASTER_VOLUME);
    if (!fSynthesizer->active1 && !fSynthesizer->active2 && !fSynthesizer->active3 || fSynthesizer->master_volume <= minimum_volume) {
        for (int x = 0; x < frames; x++) {
            outputs[0][x] = 0;
            outputs[1][x] = 0;
        }
        return;
    }

    for (unsigned int x = 0; x < frames; x++) {
        _processAudioFrame(outputs[0], outputs[1], x);
    }
}

void MinatonPlugin::sampleRateChanged(double newSampleRate)
{
}

float MinatonPlugin::_obtainSynthParameter(MinatonParamId index) const
{
    switch (index) {
    // OSC activation
    case PARAM_ACTIVE_ONE:
        return fSynthesizer->active1;
    case PARAM_ACTIVE_TWO:
        return fSynthesizer->active2;
    case PARAM_ACTIVE_THREE:
        return fSynthesizer->active3;

    // Master output
    case PARAM_MASTER_VOLUME:
        return fSynthesizer->master_volume;
    case PARAM_LEGATO:
        return fSynthesizer->get_legato();
    case PARAM_SYNC:
        return fSynthesizer->get_sync_mode();

    // DCO Wave 1
    case PARAM_WAVE_ONE:
        return fSynthesizer->dco_wave[0];
    case PARAM_INERTIA_ONE:
        return fSynthesizer->dco_inertia[0];

    // DCO Wave 2
    case PARAM_WAVE_TWO:
        return fSynthesizer->dco_wave[1];
    case PARAM_INERTIA_TWO:
        return fSynthesizer->dco_inertia[1];

    // DCO Wave 3
    case PARAM_WAVE_THREE:
        return fSynthesizer->dco_wave[2];
    case PARAM_INERTIA_THREE:
        return fSynthesizer->dco_inertia[2];

    // LFO1 Speed
    case PARAM_LFO1_SPEED:
        return fSynthesizer->dco_frequency[3];
    case PARAM_LFO1_WAVE:
        return fSynthesizer->dco_wave[3];

    // LFO2 Speed
    case PARAM_LFO2_SPEED:
        return fSynthesizer->dco_frequency[4];
    case PARAM_LFO2_WAVE:
        return fSynthesizer->dco_wave[4];

    // LFO1 to DCOs
    case PARAM_LFO1_DCO1_PITCH:
        return fSynthesizer->dco_lfo1_amount[0];
    case PARAM_LFO1_DCO2_PITCH:
        return fSynthesizer->dco_lfo1_amount[1];
    case PARAM_LFO1_DCO3_PITCH:
        return fSynthesizer->dco_lfo1_amount[2];
    case PARAM_LFO1_DCF:
        return fSynthesizer->lfo1_amount;

    // LFO2 to DCOs
    case PARAM_LFO2_DCO1_PITCH:
        return fSynthesizer->dco_lfo2_amount[0];
    case PARAM_LFO2_DCO2_PITCH:
        return fSynthesizer->dco_lfo2_amount[1];
    case PARAM_LFO2_DCO3_PITCH:
        return fSynthesizer->dco_lfo2_amount[2];
    case PARAM_LFO2_DCF:
        return fSynthesizer->lfo2_amount;

    // ADSR envelope 1
    case PARAM_ATTACK_ONE:
        return fSynthesizer->envelope1.attack;
    case PARAM_DECAY_ONE:
        return fSynthesizer->envelope1.decay;
    case PARAM_SUSTAIN_ONE:
        return fSynthesizer->envelope1.sustain;
    case PARAM_RELEASE_ONE:
        return fSynthesizer->envelope1.release;
    case PARAM_ADSR1_AMP_AMOUNT:
        return fSynthesizer->adsr_amp_amount1;
    case PARAM_ADSR1_OSC2_AMOUNT:
        return fSynthesizer->adsr_osc2_amount1;

    // ADSR envelope 2
    case PARAM_ATTACK_TWO:
        return fSynthesizer->envelope2.attack;
    case PARAM_DECAY_TWO:
        return fSynthesizer->envelope2.decay;
    case PARAM_SUSTAIN_TWO:
        return fSynthesizer->envelope2.sustain;
    case PARAM_RELEASE_TWO:
        return fSynthesizer->envelope2.release;
    case PARAM_ADSR2_DCF_AMOUNT:
        return fSynthesizer->adsr_filter_amount2;
    case PARAM_ADSR2_OSC3_AMOUNT:
        return fSynthesizer->adsr_osc3_amount2;

    // Master frequency / resonance
    case PARAM_FREQUENCY:
        return fSynthesizer->master_frequency * 9;
    case PARAM_RESONANCE:
        return fSynthesizer->master_resonance * 4;

    // OSC1 octave / pitch
    case PARAM_OCTAVE_ONE:
        return fSynthesizer->octave1;
    case PARAM_FINETUNE_ONE:
        return fSynthesizer->pitch1;

    // OSC2 octave / pitch
    case PARAM_OCTAVE_TWO:
        return fSynthesizer->octave2;
    case PARAM_FINETUNE_TWO:
        return fSynthesizer->pitch2;

    // OSC3 octave / pitch
    case PARAM_OCTAVE_THREE:
        return fSynthesizer->octave3;
    case PARAM_FINETUNE_THREE:
        return fSynthesizer->pitch3;

    // Unused parameters
    case PARAM_FINETUNE_CENTRE_ONE:
    case PARAM_FINETUNE_CENTRE_TWO:
    case PARAM_FINETUNE_CENTRE_THREE:
    case PARAM_PATCHES:
    case PARAM_MIDI_CHANNEL:
    case PARAM_COUNT:
        break;
    }

    return 0.0f;
}

void MinatonPlugin::_applySynthParameter(MinatonParamId index, float value)
{
    switch (index) {
    // OSC activation
    case PARAM_ACTIVE_ONE:
        fSynthesizer->active1 = value;
        break;
    case PARAM_ACTIVE_TWO:
        fSynthesizer->active2 = value;
        break;
    case PARAM_ACTIVE_THREE:
        fSynthesizer->active3 = value;
        break;

    // Master output
    case PARAM_MASTER_VOLUME:
        fSynthesizer->master_volume = value;
        break;
    case PARAM_LEGATO:
        fSynthesizer->set_legato(value);
        break;
    case PARAM_SYNC:
        fSynthesizer->set_sync_mode(value);
        break;

    // DCO Wave 1
    case PARAM_WAVE_ONE:
        fSynthesizer->dco_wave[0] = value;
        break;
    case PARAM_INERTIA_ONE:
        fSynthesizer->dco_inertia[0] = value;
        break;

    // DCO Wave 2
    case PARAM_WAVE_TWO:
        fSynthesizer->dco_wave[1] = value;
        break;
    case PARAM_INERTIA_TWO:
        fSynthesizer->dco_inertia[1] = value;
        break;

    // DCO Wave 3
    case PARAM_WAVE_THREE:
        fSynthesizer->dco_wave[2] = value;
        break;
    case PARAM_INERTIA_THREE:
        fSynthesizer->dco_inertia[2] = value;
        break;

    // LFO1 Speed
    case PARAM_LFO1_SPEED:
        fSynthesizer->set_freq(3, value);
        break;
    case PARAM_LFO1_WAVE:
        fSynthesizer->dco_wave[3] = value;
        break;

    // LFO2 Speed
    case PARAM_LFO2_SPEED:
        fSynthesizer->set_freq(4, value);
        break;
    case PARAM_LFO2_WAVE:
        fSynthesizer->dco_wave[4] = value;
        break;

    // LFO1 to DCOs
    case PARAM_LFO1_DCO1_PITCH:
        fSynthesizer->dco_lfo1_amount[0] = value;
        break;
    case PARAM_LFO1_DCO2_PITCH:
        fSynthesizer->dco_lfo1_amount[1] = value;
        break;
    case PARAM_LFO1_DCO3_PITCH:
        fSynthesizer->dco_lfo1_amount[2] = value;
        break;
    case PARAM_LFO1_DCF:
        fSynthesizer->lfo1_amount = value;
        break;

    // LFO2 to DCOs
    case PARAM_LFO2_DCO1_PITCH:
        fSynthesizer->dco_lfo2_amount[0] = value;
        break;
    case PARAM_LFO2_DCO2_PITCH:
        fSynthesizer->dco_lfo2_amount[1] = value;
        break;
    case PARAM_LFO2_DCO3_PITCH:
        fSynthesizer->dco_lfo2_amount[2] = value;
        break;
    case PARAM_LFO2_DCF:
        fSynthesizer->lfo2_amount = value;
        break;

    // ADSR envelope 1
    case PARAM_ATTACK_ONE:
        fSynthesizer->envelope1.attack = value;
        break;
    case PARAM_DECAY_ONE:
        fSynthesizer->envelope1.decay = value;
        break;
    case PARAM_SUSTAIN_ONE:
        fSynthesizer->envelope1.sustain = value;
        break;
    case PARAM_RELEASE_ONE:
        fSynthesizer->envelope1.release = value;
        break;
    case PARAM_ADSR1_AMP_AMOUNT:
        fSynthesizer->adsr_amp_amount1 = value;
        break;
    case PARAM_ADSR1_OSC2_AMOUNT:
        fSynthesizer->adsr_osc2_amount1 = value;
        break;

    // ADSR envelope 2
    case PARAM_ATTACK_TWO:
        fSynthesizer->envelope2.attack = value;
        break;
    case PARAM_DECAY_TWO:
        fSynthesizer->envelope2.decay = value;
        break;
    case PARAM_SUSTAIN_TWO:
        fSynthesizer->envelope2.sustain = value;
        break;
    case PARAM_RELEASE_TWO:
        fSynthesizer->envelope2.release = value;
        break;
    case PARAM_ADSR2_DCF_AMOUNT:
        fSynthesizer->adsr_filter_amount2 = value;
        break;
    case PARAM_ADSR2_OSC3_AMOUNT:
        fSynthesizer->adsr_osc3_amount2 = value;
        break;

    // Master frequency / resonance
    case PARAM_FREQUENCY:
        fSynthesizer->master_frequency = value / 9;
        break;
    case PARAM_RESONANCE:
        fSynthesizer->master_resonance = value / 4;
        break;

    // OSC1 octave / pitch
    case PARAM_OCTAVE_ONE:
        fSynthesizer->octave1 = value;
        break;
    case PARAM_FINETUNE_ONE:
        fSynthesizer->pitch1 = value;
        break;

    // OSC2 octave / pitch
    case PARAM_OCTAVE_TWO:
        fSynthesizer->octave2 = value;
        break;
    case PARAM_FINETUNE_TWO:
        fSynthesizer->pitch2 = value;
        break;

    // OSC3 octave / pitch
    case PARAM_OCTAVE_THREE:
        fSynthesizer->octave3 = value;
        break;
    case PARAM_FINETUNE_THREE:
        fSynthesizer->pitch3 = value;
        break;

    // Unused parameters
    case PARAM_FINETUNE_CENTRE_ONE:
    case PARAM_FINETUNE_CENTRE_TWO:
    case PARAM_FINETUNE_CENTRE_THREE:
    case PARAM_PATCHES:
    case PARAM_MIDI_CHANNEL:
    case PARAM_COUNT:
        break;
    }
}

Plugin* createPlugin()
{
    return new MinatonPlugin();
}

END_NAMESPACE_DISTRHO
