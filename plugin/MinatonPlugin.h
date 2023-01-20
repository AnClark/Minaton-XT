#pragma once

#include "synth.hpp"

#include "DistrhoPlugin.hpp"
#include "MinatonParams.h"

#include <memory>

START_NAMESPACE_DISTRHO

class MinatonPlugin : public Plugin {
    double fSampleRate = getSampleRate();
    std::unique_ptr<minaton_synth> fSynthesizer = std::make_unique<minaton_synth>();

    float fParameters[MinatonParamId::PARAM_COUNT];

    // Minaton note key data
    unsigned char m_key;
    double m_rate;
    uint32_t m_period;
    uint32_t m_counter;
    int last_note;
    int control_delay;

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
        return "Minaton";
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
        return "Thunderox, AnClark Liu";
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
        return d_version(0, 0, 3);
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

    void sampleRateChanged(double newSampleRate) override;

private:
    // ----------------------------------------------------------------------------------------------------------------
    // Internal helpers

    void _applySynthParameters();
    void _processMidi(const uint8_t* data, const uint32_t size);

    // ----------------------------------------------------------------------------------------------------------------

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MinatonPlugin)
};

END_NAMESPACE_DISTRHO
