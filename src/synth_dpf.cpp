#include "synth_dpf.hpp"
#include "minaton_waves.hpp"
#include "stb_decompress.h"

#include <cstring>

//=========================================================
//-- Embedded data definition
//=========================================================

namespace minaton_waves {

static const unsigned char* wave_440Hz_sineData = decode_compressed_file_array(minaton_wave_440Hz_sine_wav_compressed_data, minaton_wave_440Hz_sine_wav_compressed_size);
static const unsigned char* wave_440Hz_sawData = decode_compressed_file_array(minaton_wave_440Hz_saw_wav_compressed_data, minaton_wave_440Hz_saw_wav_compressed_size);
static const unsigned char* wave_440Hz_squareData = decode_compressed_file_array(minaton_wave_440Hz_square_wav_compressed_data, minaton_wave_440Hz_square_wav_compressed_size);
static const unsigned char* wave_440Hz_triangleData = decode_compressed_file_array(minaton_wave_440Hz_triangle_wav_compressed_data, minaton_wave_440Hz_triangle_wav_compressed_size);

static const unsigned char* wave_1Hz_sineData = decode_compressed_file_array(minaton_wave_1Hz_sine_wav_compressed_data, minaton_wave_1Hz_sine_wav_compressed_size);
static const unsigned char* wave_1Hz_sawData = decode_compressed_file_array(minaton_wave_1Hz_saw_wav_compressed_data, minaton_wave_1Hz_saw_wav_compressed_size);
static const unsigned char* wave_1Hz_squareData = decode_compressed_file_array(minaton_wave_1Hz_square_wav_compressed_data, minaton_wave_1Hz_square_wav_compressed_size);

const unsigned int wave_440Hz_sineDataSize = 2442;
const unsigned int wave_440Hz_sawDataSize = 2444;
const unsigned int wave_440Hz_squareDataSize = 1244;
const unsigned int wave_440Hz_triangleDataSize = 1244;

const unsigned int wave_1Hz_sawDataSize = 88246;
const unsigned int wave_1Hz_sineDataSize = 88246;
const unsigned int wave_1Hz_squareDataSize = 88246;

}

//=========================================================
//-- Synth implementation for DPF
//=========================================================

void minaton_synth_dpf::init()
{
    number_of_waves = 0;

    add_wave("sine", minaton_waves::wave_440Hz_sineData, minaton_waves::wave_440Hz_sineDataSize);
    add_wave("saw", minaton_waves::wave_440Hz_sawData, minaton_waves::wave_440Hz_sawDataSize);
    add_wave("square", minaton_waves::wave_440Hz_squareData, minaton_waves::wave_440Hz_squareDataSize);
    add_wave("triangle", minaton_waves::wave_440Hz_triangleData, minaton_waves::wave_440Hz_triangleDataSize);
    add_wave("triangle", minaton_waves::wave_440Hz_triangleData, minaton_waves::wave_440Hz_triangleDataSize); // dummy wave white noise generator is #4

    add_wave("slow sine", minaton_waves::wave_1Hz_sineData, minaton_waves::wave_1Hz_sineDataSize);
    add_wave("slow saw", minaton_waves::wave_1Hz_sawData, minaton_waves::wave_1Hz_sawDataSize);
    add_wave("slow square", minaton_waves::wave_1Hz_squareData, minaton_waves::wave_1Hz_squareDataSize);

    init_src();

    number_of_dcos = 0;

    add_dco();
    set_dco_wave(0, 1);
    set_dco_frequency(0, 1);

    add_dco();
    set_dco_wave(1, 1);
    set_dco_frequency(1, 1);

    add_dco();
    set_dco_wave(2, 1);
    set_dco_frequency(2, 1);

    add_dco();
    set_dco_wave(3, 5);
    set_dco_frequency(3, 0.0001);

    add_dco();
    set_dco_wave(4, 5);
    set_dco_frequency(4, 0.0001);

    init_dcas();
}

int minaton_synth_dpf::add_wave(string name, const unsigned char* data, size_t size)
{
    // If the file is already opened, first close it
    // if (infile)
    //    sf_close(infile);

    // Prepare the memory I/O structure
    SF_VIRTUAL_IO io;
    io.get_filelen = &Memory::getLength;
    io.read = &Memory::read;
    io.seek = &Memory::seek;
    io.tell = &Memory::tell;

    // Initialize the memory data
    m_memory.begin = static_cast<const unsigned char*>(data);
    m_memory.current = m_memory.begin;
    m_memory.size = size;

    int readcount;

    waves_name[number_of_waves] = name;

    if (!(infile = sf_open_virtual(&io, SFM_READ, &waves_sfinfo[number_of_waves], &m_memory))) {
        cout << "Unable to open wave from memory - " << sf_strerror(infile) << endl;
        sf_perror(NULL);
        return 1;
    }

    waves_sample[number_of_waves] = new float[waves_sfinfo[number_of_waves].frames * waves_sfinfo[number_of_waves].channels];
    readcount = sf_read_float(infile, waves_sample[number_of_waves], (waves_sfinfo[number_of_waves].channels * waves_sfinfo[number_of_waves].frames));

    sf_close(infile);

    // cout << "Loaded waveform - " << name << waves_sfinfo[number_of_waves].frames << endl;

    ++number_of_waves;

    return number_of_waves - 1;
}

void minaton_synth_dpf::panic()
{
    // Setting state to "dormant" can immediately turn all sounds off.
    //
    // "dormant" is the state when envelope ceases.
    // See minaton_synth::dca_update(): part `if (envelope1.state == release)`.

    envelope1.state = ENV_STATE_DORMANT;
    envelope1.level = 0;
    envelope2.state = ENV_STATE_DORMANT;
    envelope2.level = 0;

    // Remember to reset DCO position as well
    reset_dco_out_position();
}

void minaton_synth_dpf::reset_dco_out_position()
{
    // Reset DCO position in order to reset phases of DCOs and LFOs
    // This can avoid weird stereo spectrum among the 3 DCOs (e.g. when after resetting LFO sends to 0)

    for (uint8_t i = 0; i < 5; i++)
        dco_out_position[i] = 0;
}

void minaton_synth_dpf::set_dco_output_channel(int dco_number, minaton_channel_mode channel)
{
    switch (dco_number) {
    case 0: // DCO1
        dco1_output_channel = channel;
        break;
    case 1: // DCO2
        dco2_output_channel = channel;
        break;
    case 2: // DCO3
        dco3_output_channel = channel;
        break;
    }
}

minaton_channel_mode minaton_synth_dpf::get_dco_output_channel(int dco_number)
{
    switch (dco_number) {
    case 0: // DCO1
        return dco1_output_channel;
        break;
    case 1: // DCO2
        return dco2_output_channel;
        break;
    case 2: // DCO3
        return dco3_output_channel;
        break;
    }

    return CHANNEL_L_AND_R;
}

//=========================================================
//-- Accessor for embedded waves
//=========================================================

sf_count_t Memory::getLength(void* user)
{
    Memory* memory = static_cast<Memory*>(user);

    return memory->size;
}

sf_count_t Memory::read(void* ptr, sf_count_t count, void* user)
{
    Memory* memory = static_cast<Memory*>(user);

    sf_count_t position = tell(user);
    if (position + count >= memory->size)
        count = memory->size - position;

    std::memcpy(ptr, memory->current, static_cast<std::size_t>(count));
    memory->current += count;
    return count;
}

sf_count_t Memory::seek(sf_count_t offset, int whence, void* user)
{
    Memory* memory = static_cast<Memory*>(user);
    sf_count_t position = 0;
    switch (whence) {
    case SEEK_SET:
        position = offset;
        break;
    case SEEK_CUR:
        position = memory->current - memory->begin + offset;
        break;
    case SEEK_END:
        position = memory->size - offset;
        break;
    default:
        position = 0;
        break;
    }

    if (position >= memory->size)
        position = memory->size - 1;
    else if (position < 0)
        position = 0;

    memory->current = memory->begin + position;
    return position;
}

sf_count_t Memory::tell(void* user)
{
    Memory* memory = static_cast<Memory*>(user);
    return memory->current - memory->begin;
}
