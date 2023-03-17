// synth_dpf.hpp: Special implementiaon of Minaton synth for DPF

#pragma once

#include "synth.hpp"

//////////////////////// REFERENCES ////////////////////////
// - sf_virtual_io usages is refered from Stickman-project
//   (https://github.com/TheLusitanianKing/Stickman-project)
////////////////////////////////////////////////////////////

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

    void panic();
    void reset_dco_out_position();

private:
    Memory m_memory;
};
