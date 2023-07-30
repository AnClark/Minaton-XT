#include "MinatonPresets.hpp"
#include "MinatonUI.h"
#include "stb_decompress.h"

#include <sstream>
#include <string>

static void decode_compressed_file_array(const void* compressed_file_data, const int compressed_file_size, std::string& decompressed_file_data)
{
    const unsigned int buf_decompressed_size = minaton_stb::stb_decompress_length((const unsigned char*)compressed_file_data);
    unsigned char* buf_decompressed_data = (unsigned char*)malloc(buf_decompressed_size);
    minaton_stb::stb_decompress(buf_decompressed_data, (const unsigned char*)compressed_file_data, (unsigned int)compressed_file_size);

    std::stringstream output_builder;
    output_builder << buf_decompressed_data;
    decompressed_file_data = output_builder.str();
}

MinatonPresetManager::MinatonPresetManager(DISTRHO::MinatonUI* uiInstance)
    : fUiInstance(uiInstance)
{
    InitializeEmbedFactoryPresets(this->fEmbedFactoryPresets);
}

MinatonPresetManager::~MinatonPresetManager()
{
}

int MinatonPresetManager::loadPatchById(uint32_t patchId)
{
    std::stringstream builtin_preset_content; // Reader for built-in preset
    std::string decoded_preset_data; // Stores decoded data of built-in preset

    EmbedPresetInfo target_patch;
    if (patchId < 0 || patchId > fEmbedFactoryPresets.size()) {
        d_stderr2("[PRESET] Invalid patch ID");
        return -1;
    }

    target_patch = fEmbedFactoryPresets[patchId];
    decode_compressed_file_array(target_patch.file_array, target_patch.file_array_size, decoded_preset_data);

    // Some patches have partial params, so we load default patch in case it's author's intent.
    loadDefaultPatch();

    d_stderr("[PRESET] Loading preset: id = %d, name = %s", patchId, target_patch.name.c_str());
    _loadPatch(decoded_preset_data);

    return 0;
}

void MinatonPresetManager::loadDefaultPatch()
{
    for (int param = 0; param < PARAM_COUNT; param++) {
        _applyParameter(param, MinatonParams::paramDefaultValue(MinatonParamId(param)));
    }
}

uint32_t MinatonPresetManager::getEmbedPresetCount()
{
    return fEmbedFactoryPresets.size();
}

EmbedPresetInfo MinatonPresetManager::getEmbedPresetById(uint32_t id)
{
    if (id < 0 && id >= fEmbedFactoryPresets.size()) {
        d_stderr2("[PRESET] ERROR: invalid preset index %d", id);
        return EmbedPresetInfo();
    }

    return fEmbedFactoryPresets.at(id);
}

std::vector<EmbedPresetInfo>& MinatonPresetManager::getEmbedPresetBank()
{
    return fEmbedFactoryPresets;
}

void MinatonPresetManager::_loadPatch(const std::string& patchFileContent)
{
    std::string str;
    std::stringstream myfile(patchFileContent);
    getline(myfile, str);
    float value = 0;

    std::istringstream input_str;

    if (str == "minaton1") {

        //----------------------------

        getline(myfile, str);

        if (str == "FLAGS") {
            getline(myfile, str); // legato
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LEGATO, value);

            getline(myfile, str); // sync
            std::istringstream(str) >> value;
            _applyParameter(PARAM_SYNC, value);

            getline(myfile, str); // volume
            std::istringstream(str) >> value;
            _applyParameter(PARAM_MASTER_VOLUME, value);

            getline(myfile, str); // midi channel
            std::istringstream(str) >> value;
            _applyParameter(PARAM_MIDI_CHANNEL, value);
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);
        if (str == "FILTER") {
            getline(myfile, str); // filter frequency
            std::istringstream(str) >> value;
            _applyParameter(PARAM_FREQUENCY, value * 9);

            getline(myfile, str); // filter resonance
            std::istringstream(str) >> value;
            _applyParameter(PARAM_RESONANCE, value * 4);
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "OSCILLATOR ONE") {

            getline(myfile, str); // oscillator active or not
            std::istringstream(str) >> value;
            _applyParameter(PARAM_ACTIVE_ONE, value);

            getline(myfile, str); // octave
            std::istringstream(str) >> value;
            _applyParameter(PARAM_OCTAVE_ONE, value);

            getline(myfile, str); // pitch
            std::istringstream(str) >> value;
            _applyParameter(PARAM_FINETUNE_ONE, value);

            getline(myfile, str); // wave
            std::istringstream(str) >> value;
            _applyParameter(PARAM_WAVE_ONE, value);
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "OSCILLATOR TWO") {
            getline(myfile, str); // oscillator active or not
            std::istringstream(str) >> value;
            _applyParameter(PARAM_ACTIVE_TWO, value);

            getline(myfile, str); // octave
            std::istringstream(str) >> value;
            _applyParameter(PARAM_OCTAVE_TWO, value);

            getline(myfile, str); // pitch
            std::istringstream(str) >> value;
            _applyParameter(PARAM_FINETUNE_TWO, value);

            getline(myfile, str); // wave
            std::istringstream(str) >> value;
            _applyParameter(PARAM_WAVE_TWO, value);
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "OSCILLATOR THREE") {
            getline(myfile, str); // oscillator active or not
            std::istringstream(str) >> value;
            _applyParameter(PARAM_ACTIVE_THREE, value);

            getline(myfile, str); // octave
            std::istringstream(str) >> value;
            _applyParameter(PARAM_OCTAVE_THREE, value);

            getline(myfile, str); // pitch
            std::istringstream(str) >> value;
            _applyParameter(PARAM_FINETUNE_THREE, value);

            getline(myfile, str); // wave
            std::istringstream(str) >> value;
            _applyParameter(PARAM_WAVE_THREE, value);
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "LFO ONE") {
            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO1_DCO1_PITCH, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO1_DCO2_PITCH, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO1_DCO3_PITCH, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO1_DCF, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO1_WAVE, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO1_SPEED, value);
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "LFO TWO") {
            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO2_DCO1_PITCH, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO2_DCO2_PITCH, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO2_DCO3_PITCH, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO2_DCF, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO2_WAVE, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_LFO2_SPEED, value);
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "ADSR ONE") {
            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_ATTACK_ONE, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_DECAY_ONE, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_SUSTAIN_ONE, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_RELEASE_ONE, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_ADSR1_AMP_AMOUNT, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_ADSR1_OSC2_AMOUNT, value);
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "ADSR TWO") {
            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_ATTACK_TWO, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_DECAY_TWO, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_SUSTAIN_TWO, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_RELEASE_TWO, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_ADSR2_DCF_AMOUNT, value);

            getline(myfile, str);
            std::istringstream(str) >> value;
            _applyParameter(PARAM_ADSR2_OSC3_AMOUNT, value);
        }
    }

    getline(myfile, str);
    getline(myfile, str);

    if (str == "INERTIA") {
        getline(myfile, str);
        std::istringstream(str) >> value;
        _applyParameter(PARAM_INERTIA_ONE, value);

        getline(myfile, str);
        std::istringstream(str) >> value;
        _applyParameter(PARAM_INERTIA_TWO, value);

        getline(myfile, str);
        std::istringstream(str) >> value;
        _applyParameter(PARAM_INERTIA_THREE, value);
    }

    //--------------------------------
}

void MinatonPresetManager::_applyParameter(uint32_t index, float value)
{
    // Set parameter from UI side.
    fUiInstance->setParameterValue(index, value);

    // Update UI.
    // Notice that setParameterValue() does not invoke parameterChanged(), so the UI remains as-is.
    // We need to invoke parameterChanged() manually.
    fUiInstance->parameterChanged(index, value);
}
