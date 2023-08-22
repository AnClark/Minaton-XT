#include "MinatonPlugin.h"
#include "DistrhoPlugin.hpp"
#include "DistrhoPluginUtils.hpp"
#include "MinatonParams.h"

START_NAMESPACE_DISTRHO

MinatonPlugin::MinatonPlugin()
    : Plugin(MinatonParamId::PARAM_COUNT, 0, 0) // parameters, programs, states
    , buffer_before_resample_l(nullptr)
    , buffer_before_resample_r(nullptr)
    , m_src_master_resample_l(nullptr)
    , m_src_master_resample_r(nullptr)
    , resample_buffer_read_index(0)
    , resampled_size(0)
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

    // Initialize resample buffer
    initResampler(getBufferSize());
}

MinatonPlugin::~MinatonPlugin()
{
    fSynthesizer->cleanup();

    cleanupResampler();
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
    uint32_t frame_index = 0;

    // Use output ports provided by AudioMidiSyncHelper
    float* const outL = outputs[0];
    float* const outR = outputs[1];

    // Check if no oscillators enabled or volume at zero.
    // Once true, clear ring buffer and return.
    static const uint32_t minimum_volume = MinatonParams::paramMinValue(PARAM_MASTER_VOLUME);
    if (!fSynthesizer->active1 && !fSynthesizer->active2 && !fSynthesizer->active3 || fSynthesizer->master_volume <= minimum_volume) {
        for (int x = 0; x < frames; x++) {
            outL[x] = 0;
            outR[x] = 0;
        }
        return;
    }

    // Process MIDI events
    for (uint32_t i = 0; i < midiEventCount; ++i) {
        const MidiEvent& ev(midiEvents[i]);
        _processMidi(ev.data, ev.size);
    }

    //	++control_delay;

    // if (control_delay>20)
    //{
    //		control_delay = 0;

    // Generate and output audio frames.
    //   - If sample rate == 44100.0f (default sample rate), output frames as-is.
    //   - If not, perform a resample on final mix.
    if (fSampleRate == 44100.0f) {
        for (unsigned int x = 0; x < frames; x++) {
            _processAudioFrame(outL, outR, x);
        }
    } else {
        /**
         * Resample principle: Fill in the audio buffer with resampled frames.
         * 1. Check if a resampled buffer is fully played or not. If not, send the
         *   remaining frames to host buffers until:
         *   - the buffer is full, or
         *   - all samples played.
         * 2. If all resampled frames played, generate new samples, then resample it.
         *    Input sample counts is same as host buffer size (equals param "frames").
         * 3. At this time, if the host buffer is still not fully filled, redo the
         *    first step.
         * Resampler uses libsamplerate for higher quality.
         */

        // Check if buffer is available. Buffer may be unavailable when being reallocated
        if (!buffer_before_resample_l || !buffer_before_resample_r)
            return;

        // Play remaining frames of resampled buffer
        while (frame_index < frames && resample_buffer_read_index < resampled_size) {
            outL[frame_index] = buffer_after_resample_l[resample_buffer_read_index];
            outR[frame_index] = buffer_after_resample_r[resample_buffer_read_index];

            frame_index++;
            resample_buffer_read_index++;
        }

        // If all resampled buffers played, require new samples
        if (resample_buffer_read_index >= resampled_size) {
            for (uint32_t x = 0; x < frames; x++) {
                _processAudioFrame(buffer_before_resample_l, buffer_before_resample_r, x);
            }

            // Process each channel respectively.
            // Notes:
            // - The resampler functions are originally designed for interleaved WAV files.
            // - resampled_size_l and resampled_size_r are actually the same value, since the input frame sizes are equal.
            //   Only resampled_size_l is used.
            // - Do NOT change the last index of resampled buffer (by modifying resampled_size_l), otherwise you will
            //   encounter unexpected loud noise blowing up your DAW!
            m_resampler_data_l.data_in = buffer_before_resample_l;
            m_resampler_data_l.input_frames = frames;
            m_resampler_data_l.data_out = buffer_after_resample_l;
            m_resampler_data_l.output_frames = MAX_RESAMPLED_BUFFER_SIZE;
            m_resampler_data_l.src_ratio = (float)fSampleRate / 44100.0f;
            src_process(m_src_master_resample_l, &m_resampler_data_l);
            resampled_size_l = m_resampler_data_l.output_frames_gen;

            m_resampler_data_r.data_in = buffer_before_resample_r;
            m_resampler_data_r.input_frames = frames;
            m_resampler_data_r.data_out = buffer_after_resample_r;
            m_resampler_data_r.output_frames = MAX_RESAMPLED_BUFFER_SIZE;
            m_resampler_data_r.src_ratio = (float)fSampleRate / 44100.0f;
            src_process(m_src_master_resample_r, &m_resampler_data_r);
            resampled_size_r = m_resampler_data_r.output_frames_gen;

            resampled_size = resampled_size_l;

            // Reset resample buffer read pointer
            resample_buffer_read_index = 0;

            // If the host buffer still has space, play our newly generated frames
            while (frame_index < frames && resample_buffer_read_index < resampled_size) {
                outL[frame_index] = buffer_after_resample_l[resample_buffer_read_index];
                outR[frame_index] = buffer_after_resample_r[resample_buffer_read_index];

                frame_index++;
                resample_buffer_read_index++;
            }
        }
    }
}

void MinatonPlugin::bufferSizeChanged(int newBufferSize)
{
    if (fBufferSize != newBufferSize) {
        d_stderr("[DSP] Buffer size changed: from %d to %d", fBufferSize, newBufferSize);

        fBufferSize = newBufferSize;
        reinitResampler(fBufferSize, fSampleRate);
    } else {
        d_stderr("[DSP] Buffer size changed: same as current value, %d", fBufferSize);
    }
}

void MinatonPlugin::sampleRateChanged(double newSampleRate)
{
    if (fSampleRate != newSampleRate) {
        d_stderr("[DSP] Sample rate changed: from %f to %f", fSampleRate, newSampleRate);

        fSampleRate = newSampleRate;
        reinitResampler(fBufferSize, fSampleRate);
    } else {
        d_stderr("[DSP] Sample rate changed: same as current value, %f", fSampleRate);
    }
}

void MinatonPlugin::initResampler(uint32_t bufferSize)
{
    m_src_master_resample_l = src_new(SRC_LINEAR, 1, &m_src_errno);
    m_src_master_resample_r = src_new(SRC_LINEAR, 1, &m_src_errno);

    buffer_before_resample_l = (float*)malloc(sizeof(float) * bufferSize);
    buffer_before_resample_r = (float*)malloc(sizeof(float) * bufferSize);

    // Must set resampler output buffer to zero, otherwise you may encounter noises and overflow waves!
    memset(buffer_after_resample_l, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);
    memset(buffer_after_resample_r, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);
}

void MinatonPlugin::reinitResampler(uint32_t bufferSize, uint32_t sampleRate)
{
    free(buffer_before_resample_l);
    free(buffer_before_resample_r);

    buffer_before_resample_l = (float*)malloc(sizeof(float) * bufferSize);
    buffer_before_resample_r = (float*)malloc(sizeof(float) * bufferSize);

    // Must set resampler output buffer to zero, otherwise you may encounter noises and overflow waves!
    memset(buffer_after_resample_l, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);
    memset(buffer_after_resample_r, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);

    src_reset(m_src_master_resample_l);
    src_reset(m_src_master_resample_r);
}

void MinatonPlugin::cleanupResampler()
{
    free(buffer_before_resample_l);
    free(buffer_before_resample_r);

    buffer_before_resample_l = nullptr;
    buffer_before_resample_r = nullptr;

    src_delete(m_src_master_resample_l);
    src_delete(m_src_master_resample_r);
    m_src_master_resample_l = nullptr;
    m_src_master_resample_r = nullptr;
}

Plugin* createPlugin()
{
    return new MinatonPlugin();
}

END_NAMESPACE_DISTRHO
