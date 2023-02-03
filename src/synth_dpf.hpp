// synth_dpf.hpp: Special implementiaon of Minaton synth for DPF

#pragma once

#include "synth.hpp"

//////////////////////// REFERENCES ////////////////////////
// - sf_virtual_io usages is refered from Stickman-project
//   (https://github.com/TheLusitanianKing/Stickman-project)
////////////////////////////////////////////////////////////

//=========================================================
//-- Constants
//=========================================================

enum minaton_channel_mode {
    CHANNEL_LEFT,
    CHANNEL_RIGHT,
    CHANNEL_L_AND_R
};

//=========================================================
//-- Accessor for embedded waves
//
//   Required by libsndfile API sf_open_virtual().
//=========================================================

struct Memory {
    const unsigned char* begin;
    const unsigned char* current;
    sf_count_t size;

    static sf_count_t getLength(void* user);
    static sf_count_t read(void* ptr, sf_count_t count, void* user);
    static sf_count_t seek(sf_count_t offset, int whence, void* user);
    static sf_count_t tell(void* user);
};

//=========================================================
//-- Synth implementation for DPF
//
//   Aiming at accessing embedded wave files.
//=========================================================

class minaton_synth_dpf : public minaton_synth {
public:
    void init();
    int add_wave(string, const unsigned char*, size_t size);

    void set_output_mode(bool mode) { output_mode = mode; }
    bool get_output_mode() { return output_mode; }

    void set_dco_output_channel(int dco_number, minaton_channel_mode channel);
    minaton_channel_mode get_dco_output_channel(int dco_number);

private:
    Memory m_memory;

    bool output_mode; // true: stereo; false: mono
    minaton_channel_mode dco1_output_channel;
    minaton_channel_mode dco2_output_channel;
    minaton_channel_mode dco3_output_channel;
};
