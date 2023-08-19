#include "MinatonPlugin.h"
#include "DistrhoPlugin.hpp"
#include "DistrhoPluginUtils.hpp"
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

    // Here we use DPF AudioMidiSyncHelper to make sure audio and MIDI plays simultaneously.
    // Without sync, audio may have a lag of ~15ms.
    for (AudioMidiSyncHelper amsh(outputs, frames, midiEvents, midiEventCount); amsh.nextEvent();) {
        // Use output ports provided by AudioMidiSyncHelper
        float* const outL = amsh.outputs[0];
        float* const outR = amsh.outputs[1];

        // Process MIDI events
        for (uint32_t i = 0; i < amsh.midiEventCount; ++i) {
            const MidiEvent& ev(amsh.midiEvents[i]);
            _processMidi(ev.data, ev.size);
        }

        //	++control_delay;

        // if (control_delay>20)
        //{
        //		control_delay = 0;

        // Check if no oscillators enabled or volume at zero.
        // Once true, clear ring buffer and return.
        static const uint32_t minimum_volume = MinatonParams::paramMinValue(PARAM_MASTER_VOLUME);
        if (!fSynthesizer->active1 && !fSynthesizer->active2 && !fSynthesizer->active3 || fSynthesizer->master_volume <= minimum_volume) {
            for (int x = 0; x < amsh.frames; x++) {
                outL[x] = 0;
                outR[x] = 0;
            }
            return;
        }

        // Generate and output audio frames.
        //   - Waves are well resampled on the synth side. See minaton_synth::set_freq().
        for (unsigned int x = 0; x < amsh.frames; x++) {
            _processAudioFrame(outL, outR, x);
        }
    }
}

void MinatonPlugin::bufferSizeChanged(int newBufferSize)
{
    if (fBufferSize != newBufferSize) {
        d_stderr("[DSP] Buffer size changed: from %d to %d", fBufferSize, newBufferSize);

        fBufferSize = newBufferSize;
    } else {
        d_stderr("[DSP] Buffer size changed: same as current value, %d", fBufferSize);
    }
}

void MinatonPlugin::sampleRateChanged(double newSampleRate)
{
    if (fSampleRate != newSampleRate) {
        d_stderr("[DSP] Sample rate changed: from %f to %f", fSampleRate, newSampleRate);

        fSampleRate = newSampleRate;
        fSynthesizer->set_samplerate(fSampleRate);
    } else {
        d_stderr("[DSP] Sample rate changed: same as current value, %f", fSampleRate);
    }
}

Plugin* createPlugin()
{
    return new MinatonPlugin();
}

END_NAMESPACE_DISTRHO
