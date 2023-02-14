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

    // Initialize resampler buffer
    m_out_before_resample_l = nullptr;
    m_out_before_resample_r = nullptr;
    bufferSizeChanged(getBufferSize());

    // Initialize resampler
    m_resampler_state = src_new(SRC_LINEAR, 2, &m_resampler_error);
    if (!m_resampler_state) {
        d_stderr2(">>> ERROR: Cannot load resampler. Will not resample when samplerate != 44100.0f.");
        m_resampler_loaded = false;
    } else {
        d_stderr(">>> Resampler loaded.");
        m_resampler_loaded = true;
    }
}

MinatonPlugin::~MinatonPlugin()
{
    fSynthesizer->cleanup();

    src_delete(m_resampler_state);

    free(m_out_before_resample_l);
    free(m_out_before_resample_r);
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

    // Resample
    if (fSampleRate == 44100.0f || !m_resampler_loaded) { // Default sample rate = 44100, no need to resample
        for (unsigned int x = 0; x < frames; x++) {
            _processAudioFrame(outputs[0], outputs[1], x);
        }
    } else { // If not, resample it
        const double PROCESS_SAMPLE_COUNT = getExpectedInputSize(44100.0f, fSampleRate, frames);

        for (unsigned int x = 0; x < PROCESS_SAMPLE_COUNT; x++) {
            _processAudioFrame(m_out_before_resample_l, m_out_before_resample_r, x);
        }

        src_reset(m_resampler_state);
        m_resampler_data.data_in = m_out_before_resample_l;
        m_resampler_data.input_frames = PROCESS_SAMPLE_COUNT;
        m_resampler_data.data_out = outputs[0];
        m_resampler_data.output_frames = (long)round(PROCESS_SAMPLE_COUNT * (double)fSampleRate / (double)44100.0f);
        m_resampler_data.src_ratio = (float)fSampleRate / 44100.0f;
        src_process(m_resampler_state, &m_resampler_data);

        src_reset(m_resampler_state);
        m_resampler_data.data_in = m_out_before_resample_r;
        m_resampler_data.input_frames = PROCESS_SAMPLE_COUNT;
        m_resampler_data.data_out = outputs[1];
        m_resampler_data.output_frames = (long)round(PROCESS_SAMPLE_COUNT * (double)fSampleRate / (double)44100.0f);
        m_resampler_data.src_ratio = (float)fSampleRate / 44100.0f;
        src_process(m_resampler_state, &m_resampler_data);
    }
}

void MinatonPlugin::sampleRateChanged(double newSampleRate)
{
    fSampleRate = newSampleRate;
}

void MinatonPlugin::bufferSizeChanged(uint32_t newBufferSize)
{
    fBufferSize = newBufferSize;

    m_out_before_resample_l = (float*)realloc(m_out_before_resample_l, fBufferSize * sizeof(float));
    m_out_before_resample_r = (float*)realloc(m_out_before_resample_r, fBufferSize * sizeof(float));
}

Plugin* createPlugin()
{
    return new MinatonPlugin();
}

END_NAMESPACE_DISTRHO
