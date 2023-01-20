#include "MinatonPlugin.h"
#include "DistrhoPlugin.hpp"
#include "MinatonParams.h"

START_NAMESPACE_DISTRHO

MinatonPlugin::MinatonPlugin()
    : Plugin(MinatonParamId::PARAM_COUNT, 0, 0) // parameters, programs, states
{
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
    float mix1 = 0, mix2 = 0, mix3 = 0, delay;
    uint32_t event_index;

    for (event_index = 0; event_index < midiEventCount; ++event_index) {
        _processMidi(midiEvents[event_index].data, midiEvents[event_index].size);
    }

    //	++control_delay;

    // if (control_delay>20)
    //{
    //		control_delay = 0;

    for (unsigned int x = 0; x < frames; x++) {
        fSynthesizer->dca_update();

        delay++;
        if (delay > 20) {
            fSynthesizer->dco_cycle(3);
            fSynthesizer->dco_cycle(4);
            delay = 0;
        }

        if (fParameters[PARAM_ACTIVE_ONE]) {
            mix1 = fSynthesizer->dco_cycle(0);
        }
        if (fParameters[PARAM_ACTIVE_TWO]) {
            mix2 = fSynthesizer->dco_cycle(1);
        }
        if (fParameters[PARAM_ACTIVE_THREE]) {
            mix3 = fSynthesizer->dco_cycle(2);
        }

        float mixer_out_left = (mix1 + mix2) / fSynthesizer->master_volume;
        float mixer_out_right = (mix2 + mix3) / fSynthesizer->master_volume;

        // mixer_out =  1.1f * mixer_out - 0.2f * mixer_out * mixer_out * mixer_out;

        mixer_out_left = fSynthesizer->dcf_left(mixer_out_left, fSynthesizer->envelope2_out(0.1, fSynthesizer->adsr_filter_amount2), 0.1);
        mixer_out_right = fSynthesizer->dcf_right(mixer_out_right, fSynthesizer->envelope2_out(0.1, fSynthesizer->adsr_filter_amount2), 0.1);

        outputs[0][x] = fSynthesizer->envelope1_out(mixer_out_left, fSynthesizer->adsr_amp_amount1);
        outputs[1][x] = fSynthesizer->envelope1_out(mixer_out_right, fSynthesizer->adsr_amp_amount1);
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

void MinatonPlugin::_processMidi(const uint8_t* data, const uint32_t size)
{
    // Procedure adapted from synthv1 (https://github.com/rncbc/synthv1).

    for (uint32_t i = 0; i < size; ++i) {

        // channel status
        const int channel = (data[i] & 0x0f) + 1;
        const int status = (data[i] & 0xf0);

        // all system common/real-time ignored
        if (status == 0xf0)
            continue;

        // check data size (#1)
        if (++i >= size)
            break;

        const int key = (data[i] & 0x7f);

        // program change
        if (status == 0xc0) {
            if (fParameters[PARAM_ACTIVE_ONE] || fParameters[PARAM_ACTIVE_TWO] || fParameters[PARAM_ACTIVE_THREE])
                d_stderr2("[Unimplemented] program change = %d", key);
            continue;
        }

        // channel aftertouch
        if (status == 0xd0) {
            const float pre = float(key) / 127.0f;
            if (fParameters[PARAM_ACTIVE_ONE] || fParameters[PARAM_ACTIVE_TWO] || fParameters[PARAM_ACTIVE_THREE])
                d_stderr2("[Unimplemented] Channel aftertouch pressure = %d", pre);
            continue;
        }

        // check data size (#2)
        if (++i >= size)
            break;

        // channel value
        const int value = (data[i] & 0x7f);

        d_stderr("[MIDI] channel = 0x%2x, status = 0x%2x, key = %d, channel value = %d", channel, status, key, value);

        // note on
        if (status == 0x90 && value > 0) {
#if 0
            d_stderr("Current key on: %d", key);
#else
            m_key = key;
            m_period = m_rate * 4.0 / 440.0f; // LV2::key2hz(m_key);

            if (last_note == -1 || fParameters[PARAM_LEGATO] == 0) {
                fSynthesizer->trigger_envelope1();
                fSynthesizer->trigger_envelope2();
            }
            fSynthesizer->set_tuning(m_key - 38);
            last_note = m_key;
#endif
        }
        // note off
        else if (status == 0x80 || (status == 0x90 && value == 0)) {
            m_key = -1;
            fSynthesizer->release_envelope1();
            fSynthesizer->release_envelope2();
            last_note = -1;
        }
        // key pressure/poly.aftertouch
        else if (status == 0xa0) {
            const float pre = float(value) / 127.0f;
            d_stderr2("[Unimplemented] Key pressure/poly aftertouch: pressure = %f", pre);
        }
        // control change. `key` is control's ID (CC ID).
        else if (status == 0xb0) {
            d_stderr2("[Unimplemented] Control change: CC#%d", key);
        }
        // pitch bend
        else if (status == 0xe0) {
            const float pitchbend = float(key + (value << 7) - 0x2000) / 8192.0f;
            d_stderr2("[Unimplemented] Pitch bend: bend to %f", pitchbend);
        }
    }
}

Plugin* createPlugin()
{
    return new MinatonPlugin();
}

END_NAMESPACE_DISTRHO
