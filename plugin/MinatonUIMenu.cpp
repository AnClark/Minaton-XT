#include "MinatonPresets.hpp"
#include "MinatonUI.h"
#include "config.h" // For version info

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
        ImGui::SetWindowPos(ImVec2(20 + 2, 504));
        ImGui::SetWindowSize(ImVec2(100, 50 + 20));

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 2.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 2.0f);

        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(0x3A, 0x3A, 0x3A, 0xFF));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(0x5A, 0x5A, 0x5A, 0xFF));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(0x7A, 0x7A, 0x7A, 0xFF));
        if (ImGui::Button("Presets", ImVec2(60 - 2, 25 - 2))) {
            ImGui::OpenPopup("preset_menu");
        }
        if (ImGui::Button("Help", ImVec2(60 - 2, 25 - 2))) {
            _isAboutWindowOpen = !_isAboutWindowOpen;
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

    //
    // Help Window
    //
    {
        ImGui::SetNextWindowPos(ImVec2(150 - 50, 100), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(500 + 50, 400), ImGuiCond_Once);

        if (_isAboutWindowOpen) {
            if (ImGui::Begin("Help & About", &_isAboutWindowOpen, ImGuiWindowFlags_NoSavedSettings)) {
                if (ImGui::BeginTabBar("HelpTabs")) {
                    if (ImGui::BeginTabItem("Manual")) {
                        ImGui::SeparatorText("Minaton XT Manual");
                        if (ImGui::CollapsingHeader("Overview")) {
                            ImGui::TextWrapped("Minaton XT is a virtual analog synthesizer plugin inspired by classic hardware synths.");
                            ImGui::TextWrapped("It features 3 DCOs, 2 LFOs, dual envelopes for rich sound design possibilities.");
                            ImGui::TextWrapped("A Moog-style filter brings legendary analogue sounding, catching your ears.");

                            ImGui::Dummy(ImVec2(0, 2));
                            ImGui::Text(" ------------- Quick start guide -------------");
                            ImGui::BulletText("Use the Presets button to load factory presets or reset to the default patch.");
                            ImGui::BulletText("Adjust parameters using the knobs and sliders on the main interface.");
                            ImGui::Dummy(ImVec2(0, 2));

                            ImGui::TextWrapped("Enjoy creating sounds with Minaton XT!");
                        }

                        if (ImGui::CollapsingHeader("Controls Explanation")) {
                            ImGui::TextWrapped("Minaton XT features a variety of controls for sound design. Below is a breakdown of the main components:");

                            ImGui::BulletText("DCOs (Digitally Controlled Oscillators):");
                            ImGui::Indent();
                            ImGui::TextWrapped("Generate the raw sound waveforms. You can select from various wave types (e.g., sine, square, sawtooth) and adjust pitch, volume, and other parameters for each of the 3 oscillators.");
                            ImGui::Unindent();

                            ImGui::BulletText("LFOs (Low Frequency Oscillators):");
                            ImGui::Indent();
                            ImGui::TextWrapped("Modulate parameters like pitch, filter cutoff, or amplitude for effects such as vibrato, tremolo, or filter sweeps. There are 2 LFOs available with adjustable speed and depth.");
                            ImGui::Unindent();

                            ImGui::BulletText("DCF (Digitally Controlled Filter):");
                            ImGui::Indent();
                            ImGui::TextWrapped("A Moog-style 24 dB/octave low-pass filter that shapes the tonal quality by adjusting cutoff frequency and resonance. It can be modulated by LFOs and Envelope 2 for dynamic filtering effects.");
                            ImGui::Unindent();

                            ImGui::BulletText("Envelopes:");
                            ImGui::Indent();
                            ImGui::TextWrapped("Control the amplitude and modulation envelopes using ADSR (Attack, Decay, Sustain, Release) parameters. Dual envelopes (Envelope 1 for amplitude and pitch modulation, Envelope 2 for filter and pitch modulation) enable complex sound shaping.");
                            ImGui::Unindent();

                            ImGui::TextWrapped("Experiment with these controls to create unique sounds. Refer to the main interface for visual knobs and sliders.");
                        }

                        if (ImGui::CollapsingHeader("DCOs")) {
                            ImGui::TextWrapped("DCOs (Digitally Controlled Oscillators) are the primary sound generators in Minaton XT. The DCO section resides in the upper-left area of the interface.");
                            ImGui::TextWrapped("There are 3 DCOs, each with controls for waveform, inertia, octave, fine tune, and activation. They form the foundation of the synthesizer's sound.");

                            ImGui::BulletText("DCO Parameters:");
                            ImGui::Indent();
                            if (ImGui::BeginTable("DCOParametersTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                                ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                                ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableHeadersRow();

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextWrapped("Waveform Selection Slider");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Select the waveform type for each DCO. Available options include various shapes that affect the harmonic content and timbre.\nEach DCO supports 5 waveforms: Sine, Triangle, Sawtooth, Square, and Noise.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextWrapped("Inertia");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Adjusts the inertia parameter, which influences how smoothly the DCO responds to changes, adding a sense of momentum or smoothing to the sound.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextWrapped("Octave");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Shifts the pitch by octaves.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextWrapped("Fine Tune");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Provides precise pitch adjustment.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextWrapped("Activation Switches");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Enable or disable each DCO to control which oscillators contribute to the output.");

                                ImGui::EndTable();
                            }
                            ImGui::Unindent();

                            ImGui::BulletText("Using DCOs:");
                            ImGui::Indent();
                            ImGui::TextWrapped("Start by activating DCOs and selecting waveforms. Adjust octaves and fine tunes for harmony. Use inertia for expressive control. Combine with LFOs and envelopes for dynamic sounds.");
                            ImGui::Unindent();

                            ImGui::TextWrapped("Tip: DCO 1 is typically the main oscillator; use DCOs 2 and 3 for layering or subtle variations.");
                        }

                        if (ImGui::CollapsingHeader("Envelopes")) {

                            ImGui::TextWrapped("Envelopes shape the amplitude and other parameters over time, defining the attack, decay, sustain, and release (ADSR) of sounds. Minaton XT features dual envelopes for flexible sound design.");
                            ImGui::TextWrapped("Envelope 1 primarily controls amplitude, while Envelope 2 can modulate filter cutoff and other parameters. Each envelope has independent ADSR controls.");

                            ImGui::BulletText("ADSR Parameters:");
                            ImGui::Indent();
                            if (ImGui::BeginTable("ADSRTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                                ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                                ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableHeadersRow();

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Attack");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Time for the envelope to reach its peak level when a note is triggered.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Decay");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Time for the envelope to fall from peak to sustain level.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Sustain");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Level maintained while a note is held.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Release");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Time for the envelope to fall to zero after a note is released.");

                                ImGui::EndTable();
                            }
                            ImGui::Unindent();

                            ImGui::BulletText("Envelope Amounts:");
                            ImGui::Indent();
                            ImGui::TextWrapped("Control how much each envelope affects amplitude (for Envelope 1) or filter/pitch (for Envelope 2).");
                            if (ImGui::BeginTable("EnvelopeAmountsTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                                ImGui::TableSetupColumn("Slider", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                                ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, 110.0f);
                                ImGui::TableSetupColumn("Envelope", ImGuiTableColumnFlags_WidthFixed, 60.0f);
                                ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableHeadersRow();

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("AMP");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("ADS Amp Amount");
                                ImGui::TableSetColumnIndex(2);
                                ImGui::Text("Env. 1");
                                ImGui::TableSetColumnIndex(3);
                                ImGui::Text("Modulates overall volume.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("DCO2");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("ADS Osc2 Amount");
                                ImGui::TableSetColumnIndex(2);
                                ImGui::Text("Env. 1");
                                ImGui::TableSetColumnIndex(3);
                                ImGui::Text("Modulates DCO2 pitch.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("CUTOFF");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("ADS Filter Amount");
                                ImGui::TableSetColumnIndex(2);
                                ImGui::Text("Env. 2");
                                ImGui::TableSetColumnIndex(3);
                                ImGui::Text("Modulates filter cutoff.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("DCO3");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("ADS Osc3 Amount");
                                ImGui::TableSetColumnIndex(2);
                                ImGui::Text("Env. 2");
                                ImGui::TableSetColumnIndex(3);
                                ImGui::Text("Modulates DCO3 pitch.");

                                ImGui::EndTable();
                            }
                            ImGui::Unindent();

                            ImGui::BulletText("Using Envelopes:");
                            ImGui::Indent();
                            ImGui::TextWrapped("Adjust ADSR for different sound characteristics: fast attack for percussive sounds, slow decay for pads. Use amounts to apply modulation. Experiment with legato mode for smoother transitions.");
                            ImGui::Unindent();

                            ImGui::TextWrapped("Tip: Envelope 1 is triggered on note-on, Envelope 2 can be independently controlled for complex modulations.");
                        }

                        if (ImGui::CollapsingHeader("LFO")) {
                            ImGui::TextWrapped("LFOs (Low Frequency Oscillators) modulate parameters like pitch, filter cutoff, or amplitude to create effects such as vibrato, tremolo, or filter sweeps. Minaton XT includes 2 LFOs (LFO1 and LFO2), each with adjustable speed and waveform, and can be routed to modulate DCO pitches and DCF cutoff.");
                            ImGui::TextWrapped("LFOs are implemented as additional DCOs (DCO4 for LFO1, DCO5 for LFO2) with dedicated controls for modulation depth per destination.");

                            ImGui::BulletText("LFO Parameters:");
                            ImGui::Indent();
                            if (ImGui::BeginTable("LFOParametersTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                                ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, 80.0f);
                                ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableHeadersRow();

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Waveform");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Select the LFO waveform type (same options as DCOs: Sine, Triangle, Sawtooth, Square, Noise).");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Speed");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Adjust the oscillation rate of the LFO.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextWrapped("Modulation Amounts");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Control the depth of modulation applied to specific destinations.");

                                ImGui::EndTable();
                            }
                            ImGui::BulletText("Modulation Amount Controls:");
                            ImGui::Indent();
                            if (ImGui::BeginTable("LFOModulationTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                                ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                                ImGui::TableSetupColumn("Slider Label", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                                ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableHeadersRow();

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("DCO1/2/3 Pitch");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("DCO1/2/3");
                                ImGui::TableSetColumnIndex(2);
                                ImGui::Text("Modulates the pitch of each DCO.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("DCF Cutoff");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::Text("CUTOFF");
                                ImGui::TableSetColumnIndex(2);
                                ImGui::Text("Modulates the filter cutoff frequency.");

                                ImGui::EndTable();
                            }
                            ImGui::Unindent();
                            ImGui::Unindent();

                            ImGui::BulletText("Using LFOs:");
                            ImGui::Indent();
                            ImGui::TextWrapped("Select waveforms and set speeds for desired modulation effects. Adjust individual amounts to route LFO modulation to DCO pitches or DCF cutoff. Combine with envelopes for evolving sound textures.");
                            ImGui::Unindent();

                            ImGui::TextWrapped("Tip: Experiment with different LFO settings to add movement and interest to your sounds.");
                        }

                        if (ImGui::CollapsingHeader("DCF (Filter)")) {
                            ImGui::TextWrapped("The DCF (Digitally Controlled Filter) in Minaton XT is a Moog-style 24 dB/octave low-pass filter that shapes the sound's tonal quality. It attenuates frequencies above the cutoff point while allowing lower frequencies to pass through.");
                            ImGui::TextWrapped("The filter's cutoff frequency and resonance can be modulated by LFOs (LFO1 and LFO2) and Envelope 2 for dynamic, evolving timbres.");

                            ImGui::BulletText("Filter Parameters:");
                            ImGui::Indent();
                            if (ImGui::BeginTable("DCFFilterTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                                ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                                ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableHeadersRow();

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Cutoff");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Controls the filter's cutoff frequency point.\nHigher values allow more high frequencies to pass.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Resonance");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Adds emphasis around the cutoff frequency, creating a peak that can lead to self-oscillation at high settings.");

                                ImGui::EndTable();
                            }
                            ImGui::Unindent();

                            ImGui::BulletText("Modulation:");
                            ImGui::Indent();
                            ImGui::TextWrapped("LFO1 and LFO2 can modulate the cutoff frequency based on their respective amounts (lfo1_amount and lfo2_amount), using the LFO output scaled by 1/4. Envelope 2 can also modulate the cutoff via the ADS Filter Amount (adsr_filter_amount2).");
                            ImGui::Unindent();

                            ImGui::BulletText("Using the DCF:");
                            ImGui::Indent();
                            ImGui::TextWrapped("Adjust cutoff and resonance to sculpt the basic tone. Apply modulation from LFOs for rhythmic effects or Envelope 2 for envelope-controlled sweeps. The filter processes left and right channels separately with identical coefficients for stereo consistency.");
                            ImGui::Unindent();

                            ImGui::TextWrapped("Tip: High resonance combined with modulation can produce classic analog filter behaviors, including feedback and harmonic enhancement.");
                        }

                        if (ImGui::CollapsingHeader("Output Control")) {
                            ImGui::TextWrapped("The Output Control section manages the final sound output, including volume, play modes, and mixing options for the synthesizer.");

                            ImGui::BulletText("Output control parameters:");
                            ImGui::Indent();
                            if (ImGui::BeginTable("OutputControlTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                                ImGui::TableSetupColumn("Parameter", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                                ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
                                ImGui::TableHeadersRow();

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Master Volume");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Controls the overall output volume of the synthesizer. The volume parameter ranges from 5 to 100, mapped to a logarithmic dB scale (-39.6 dB to -14.0 dB) for natural perception, ensuring balanced attenuation without clipping.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Legato Mode");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("When enabled, legato mode allows for smooth transitions between notes without retriggering the envelopes, ideal for monophonic lead lines and expressive playing.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::Text("Master Sync");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("When activated, Master Sync ensures that DCO2's frequency does not fall below DCO1's frequency, maintaining harmonic relationships and preventing dissonant intervals in layered sounds.");

                                ImGui::TableNextRow();
                                ImGui::TableSetColumnIndex(0);
                                ImGui::TextWrapped("Mix Mode\n(Output Mode)");
                                ImGui::TableSetColumnIndex(1);
                                ImGui::TextWrapped("Determines the output mixing configuration, such as mono or stereo modes, affecting how the DCOs are combined and routed to the left and right channels for spatial sound design.");

                                ImGui::EndTable();
                            }
                            ImGui::Unindent();

                            ImGui::BulletText("Using Output Controls:");
                            ImGui::Indent();
                            ImGui::TextWrapped("Set the master volume to an appropriate level to avoid clipping. Enable legato mode for smooth note transitions. Use master sync to maintain harmonic integrity when layering DCOs. Choose the desired mix mode for your output configuration.");
                            ImGui::Unindent();

                            ImGui::BulletText("About Mix Mode:");
                            ImGui::Indent();
                            ImGui::TextWrapped("In Stereo mode, each DCO (DCO1, DCO2, DCO3) can be independently routed to the left and/or right output channels based on user settings. The output for each channel is the sum of the routed DCOs multiplied by volume and velocity factors.");
                            ImGui::TextWrapped("In Mono mode, all active DCOs are summed together and sent equally to both left and right channels.");
                            ImGui::TextWrapped("You can control DCO output routing via \"dco1/2/3_output_channel\" parameters in Generic UI.");
                            ImGui::Unindent();

                            ImGui::TextWrapped("Tip: Start with moderate volume levels and adjust as needed to fit within your mix.");
                        }

                        ImGui::EndTabItem();
                    }

                    if (ImGui::BeginTabItem("About")) {
                        String name_with_version = String("Minaton XT ") + String(VERSION_MAJOR) + "." + String(VERSION_MINOR) + "." + String(VERSION_PATCH);
                        ImGui::SeparatorText(name_with_version.buffer());
                        ImGui::TextWrapped("Minaton XT is a virtual monophonic analog synthesizer plugin inspired by classic hardware synths.");
                        ImGui::BulletText("Copyright (c) 2010-2012 ThunderOx");
                        ImGui::BulletText("Copyright (c) 2022-2026 AnClark Liu");

                        ImGui::SeparatorText("Authors");
                        ImGui::BulletText("ThunderOx - Original Minaton LV2 Plugin");
                        ImGui::BulletText("AnClark Liu - Minaton XT");

                        ImGui::SeparatorText("License");
                        ImGui::TextWrapped("This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.");
                        ImGui::TextWrapped("VST is a trademark of Steinberg GmbH.");

                        ImGui::EndTabItem();
                    }
                    ImGui::EndTabBar();
                }
            }

            ImGui::End();
        }
    }
}
