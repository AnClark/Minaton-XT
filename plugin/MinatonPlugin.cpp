#include "MinatonPlugin.h"
#include "DistrhoPlugin.hpp"
#include "DistrhoPluginUtils.hpp"
#include "MinatonParams.h"

START_NAMESPACE_DISTRHO

MinatonPlugin::MinatonPlugin()
    : Plugin(MinatonParamId::PARAM_COUNT, 0, 0) // parameters, programs, states
#if 0 /* Now we prefer fixed size of buffer */
    , buffer_before_resample_l(nullptr)
    , buffer_before_resample_r(nullptr)
#endif
    , m_srcMaster_L(nullptr)
    , m_srcMaster_R(nullptr)
    , m_resampleBufferReadIndex(0)
    , m_sizeResampled(0)
    , m_volumeReciprocalDivFactor(0.0f)
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

    // Sanity check for host buffer size.
    // I can't prevents some hosts to give Minaton extremely large buffers :-0
    // TODO: Show a warning on UI side.
    if (unlikely(frames > MAX_HOST_BUFFER_SIZE)) {
        d_stderr2("[DSP] [FATAL!] Your host gives Minaton too large buffer (size: %d)! Max accepted size should be %d", frames, MAX_HOST_BUFFER_SIZE);

        memset(outL, 0, sizeof(float) * frames);
        memset(outR, 0, sizeof(float) * frames);
        return;
    }

    // Check if no oscillators enabled or volume at zero.
    // Once true, clear ring buffer and return.
    if (!fSynthesizer->active1 && !fSynthesizer->active2 && !fSynthesizer->active3 || fSynthesizer->master_volume <= MINIMUM_VOLUME) {
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

#if 0 /* Now we prefer fixed size of buffer */
        // Check if buffer is available. Buffer may be unavailable when being reallocated
        if (!buffer_before_resample_l || !buffer_before_resample_r) {
            memset(outL, 0, sizeof(float) * frames);
            memset(outR, 0, sizeof(float) * frames);
            return;
        }
#endif

        // Play remaining frames of resampled buffer
        while (frame_index < frames && m_resampleBufferReadIndex < m_sizeResampled) {
            outL[frame_index] = buffer_after_resample_l[m_resampleBufferReadIndex];
            outR[frame_index] = buffer_after_resample_r[m_resampleBufferReadIndex];

            frame_index++;
            m_resampleBufferReadIndex++;
        }

        // If all resampled buffers played, and there's still space left in output buffer, let's require new samples
        while (frame_index < frames) {
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
            m_srcData_L.data_in = buffer_before_resample_l;
            m_srcData_L.input_frames = frames;
            m_srcData_L.data_out = buffer_after_resample_l;
            m_srcData_L.output_frames = MAX_RESAMPLED_BUFFER_SIZE;
            m_srcData_L.src_ratio = (float)fSampleRate / 44100.0f;
            src_process(m_srcMaster_L, &m_srcData_L);
            m_sizeResampled_L = m_srcData_L.output_frames_gen;

            m_srcData_R.data_in = buffer_before_resample_r;
            m_srcData_R.input_frames = frames;
            m_srcData_R.data_out = buffer_after_resample_r;
            m_srcData_R.output_frames = MAX_RESAMPLED_BUFFER_SIZE;
            m_srcData_R.src_ratio = (float)fSampleRate / 44100.0f;
            src_process(m_srcMaster_R, &m_srcData_R);
            m_sizeResampled_R = m_srcData_R.output_frames_gen;

            m_sizeResampled = m_sizeResampled_L;

            // Reset resample buffer read pointer
            m_resampleBufferReadIndex = 0;

            // If the host buffer still has space, play our newly generated frames
            while (frame_index < frames && m_resampleBufferReadIndex < m_sizeResampled) {
                outL[frame_index] = buffer_after_resample_l[m_resampleBufferReadIndex];
                outR[frame_index] = buffer_after_resample_r[m_resampleBufferReadIndex];

                frame_index++;
                m_resampleBufferReadIndex++;
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
    m_srcMaster_L = src_new(SRC_LINEAR, 1, &m_srcErrNo);
    m_srcMaster_R = src_new(SRC_LINEAR, 1, &m_srcErrNo);

#if 0 /* Now we prefer fixed size of buffer */
    buffer_before_resample_l = (float*)malloc(sizeof(float) * bufferSize);
    buffer_before_resample_r = (float*)malloc(sizeof(float) * bufferSize);
#else
    // Set resampler input buffer to zero in case unexpected noise generates
    memset(buffer_before_resample_l, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);
    memset(buffer_before_resample_r, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);
#endif

    // Must set resampler output buffer to zero, otherwise you may encounter noises and overflow waves!
    memset(buffer_after_resample_l, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);
    memset(buffer_after_resample_r, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);
}

void MinatonPlugin::reinitResampler(uint32_t bufferSize, uint32_t sampleRate)
{
#if 0 /* Now we prefer fixed size of buffer */
    free(buffer_before_resample_l);
    free(buffer_before_resample_r);

    buffer_before_resample_l = (float*)malloc(sizeof(float) * bufferSize);
    buffer_before_resample_r = (float*)malloc(sizeof(float) * bufferSize);
#else
    // Set resampler input buffer to zero in case unexpected noise generates
    memset(buffer_before_resample_l, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);
    memset(buffer_before_resample_r, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);
#endif

    // Must set resampler output buffer to zero, otherwise you may encounter noises and overflow waves!
    memset(buffer_after_resample_l, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);
    memset(buffer_after_resample_r, 0, sizeof(float) * MAX_RESAMPLED_BUFFER_SIZE);

    src_reset(m_srcMaster_L);
    src_reset(m_srcMaster_R);
}

void MinatonPlugin::cleanupResampler()
{
#if 0 /* Now we prefer fixed size of buffer */
    free(buffer_before_resample_l);
    free(buffer_before_resample_r);

    buffer_before_resample_l = nullptr;
    buffer_before_resample_r = nullptr;
#endif

    src_delete(m_srcMaster_L);
    src_delete(m_srcMaster_R);

    m_srcMaster_L = nullptr;
    m_srcMaster_R = nullptr;
}

Plugin* createPlugin()
{
    return new MinatonPlugin();
}

END_NAMESPACE_DISTRHO
