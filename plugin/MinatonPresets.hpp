#pragma once

#include <cstdint>
#include <string>
#include <vector>

// Forward decls.
namespace DISTRHO {
class MinatonUI;
}

struct EmbedPresetInfo {
    std::string name;
    std::string file_path;
    bool read_only;

    const unsigned int* file_array = NULL;
    int file_array_size = 0;
};

void InitializeEmbedFactoryPresets(std::vector<EmbedPresetInfo>& factory_presets_list);

class MinatonPresetManager {
    std::vector<EmbedPresetInfo> fEmbedFactoryPresets;
    DISTRHO::MinatonUI* fUiInstance;

public:
    MinatonPresetManager(DISTRHO::MinatonUI*);
    ~MinatonPresetManager();

    // -------------------------------------------------------------------
    // Load patch

    int loadPatchById(uint32_t);
    void loadDefaultPatch(bool writeLog = false);

    // -------------------------------------------------------------------
    // Query built-in preset

    uint32_t getEmbedPresetCount();
    EmbedPresetInfo getEmbedPresetById(uint32_t);
    std::vector<EmbedPresetInfo>& getEmbedPresetBank();

private:
    // -------------------------------------------------------------------
    // Internal helpers

    // Worker for parsing and applying actual patch file
    void _loadPatch(const std::string&);

    // Set parameter, then update UI
    void _applyParameter(uint32_t, float);
};
