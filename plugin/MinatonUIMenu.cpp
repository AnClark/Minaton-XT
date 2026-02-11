#include "MinatonPresets.hpp"
#include "MinatonUI.h"

enum MinatonMenuId {
    MENU_DEFAULT_PRESET = -1,
    MENU_FIRST_PRESET
};

void MinatonImGuiUI::onImGuiDisplay()
{
    //
    // Toolbar area - resides below the plugin logo
    //
    if (ImGui::Begin("Main Tools", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground)) {
        ImGui::SetWindowPos(ImVec2(20, 504));
        ImGui::SetWindowSize(ImVec2(100, 50));

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 2.0f);

        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0x3A, 0x3A, 0x3A, 0xFF));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0x5A, 0x5A, 0x5A, 0xFF));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0x7A, 0x7A, 0x7A, 0xFF));
        if (ImGui::Button(" Presets ", ImVec2(0, 25))) {
            ImGui::OpenPopup("preset_menu");
        }
        ImGui::PopStyleColor(3);

        if (ImGui::BeginPopup("preset_menu")) {
            ImGui::SeparatorText("Factory Presets");

            for (uint32_t i = 0; i < ui->fPresetManager->getEmbedPresetCount(); i++) {
                const auto& preset = ui->fPresetManager->getEmbedPresetById(i);
                if (ImGui::MenuItem(preset.name.c_str(), NULL, (_selectedPresetId == MENU_FIRST_PRESET + i))) {
                    ui->fPresetManager->loadPatchById(i);
                    _selectedPresetId = MENU_FIRST_PRESET + i;
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Default Patch", NULL, (_selectedPresetId == MENU_DEFAULT_PRESET))) {
                ui->fPresetManager->loadDefaultPatch(true);
                _selectedPresetId = MENU_DEFAULT_PRESET;
            }

            ImGui::EndPopup();
        }

        ImGui::PopStyleVar(2);

        ImGui::End();
    }
}
