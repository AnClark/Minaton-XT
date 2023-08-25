#pragma once

#include "synth_dpf.hpp"

#include "DistrhoPlugin.hpp"
#include "MinatonParams.h"

#include "config.h"

#include <samplerate.h>

#include <memory>

constexpr size_t MAX_RESAMPLED_BUFFER_SIZE = 65536;

START_NAMESPACE_DISTRHO

class MinatonPlugin : public Plugin {
    double fSampleRate = getSampleRate();
    double fBufferSize = getBufferSize();
    std::unique_ptr<minaton_synth_dpf> fSynthesizer = std::make_unique<minaton_synth_dpf>();

    // Minaton note key data
    unsigned char m_key;
    double m_rate;
    uint32_t m_period;
    uint32_t m_counter;
    int last_note;
    int control_delay;

    // Resampler data
    SRC_STATE* m_srcMaster_L;
    SRC_STATE* m_srcMaster_R;
    SRC_DATA m_srcData_L, m_srcData_R;
    int m_srcErrNo;

    float *buffer_before_resample_l, *buffer_before_resample_r;

    float buffer_after_resample_l[MAX_RESAMPLED_BUFFER_SIZE], buffer_after_resample_r[MAX_RESAMPLED_BUFFER_SIZE];
    uint64_t m_sizeResampled, m_sizeResampled_L, m_sizeResampled_R;
    uint32_t m_resampleBufferReadIndex;

public:
    MinatonPlugin();
    ~MinatonPlugin();

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // Information

    /**
        Get the plugin label.@n
        This label is a short restricted name consisting of only _, a-z, A-Z and 0-9 characters.
    */
    const char* getLabel() const noexcept override
    {
        return DISTRHO_PLUGIN_NAME;
    }

    /**
           Get an extensive comment/description about the plugin.@n
           Optional, returns nothing by default.
         */
    const char* getDescription() const override
    {
        return "A fat sounding mono subtractive software synthesizer";
    }

    /**
           Get the plugin author/maker.
         */
    const char* getMaker() const noexcept override
    {
        return DISTRHO_PLUGIN_BRAND;
    }

    /**
           Get the plugin license (a single line of text or a URL).@n
           For commercial plugins this should return some short copyright information.
         */
    const char* getLicense() const noexcept override
    {
        return "GPLv3";
    }

    /**
        Get the plugin version, in hexadecimal.
        @see d_version()
        */
    uint32_t getVersion() const noexcept override
    {
        return d_version(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    }

    /**
           Get the plugin unique Id.@n
           This value is used by LADSPA, DSSI and VST plugin formats.
           @see d_cconst()
         */
    int64_t getUniqueId() const noexcept override
    {
        return d_cconst('m', 'n', 't', 'n');
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override;

    // ----------------------------------------------------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void setParameterValue(uint32_t index, float value) override;

    // ----------------------------------------------------------------------------------------------------------------
    // Audio/MIDI Processing

    void activate() override;
    void run(const float** inputs, float** outputs, uint32_t frames, const MidiEvent* midiEvents, uint32_t midiEventCount) override;

    // ----------------------------------------------------------------------------------------------------------------
    // Callbacks (optional)

    void bufferSizeChanged(int newBufferSize);
    void sampleRateChanged(double newSampleRate) override;

private:
    // ----------------------------------------------------------------------------------------------------------------
    // Internal helpers

    float _obtainSynthParameter(MinatonParamId index) const;
    void _applySynthParameter(MinatonParamId index, float value);

    void initResampler(uint32_t bufferSize);
    void reinitResampler(uint32_t bufferSize, uint32_t sampleRate);
    void cleanupResampler();

    // ----------------------------------------------------------------------------------------------------------------
    // Processors

    void _processAudioFrame(float* audio_l, float* audio_r, uint32_t frame_index);
    void _processMidi(const uint8_t* data, const uint32_t size);

    // ----------------------------------------------------------------------------------------------------------------

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MinatonPlugin)
};

END_NAMESPACE_DISTRHO
