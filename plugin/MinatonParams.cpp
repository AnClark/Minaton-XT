#include "MinatonParams.h"

static struct MinatonParamDefinition minaton_params[MinatonParamId::PARAM_COUNT] = {
    // symbol, frequency, min, max, def, type
    { "frequency", "Frequency", -0.1, 0.5, 0, PARAM_FLOAT },
    { "resonance", "Resonance", 0, 3.6, 1.6, PARAM_FLOAT },
    { "active_one", "DCO Active 1", 0, 1, 1, PARAM_BOOL },
    { "octave_one", "DCO Octave 1", -4, 4, 0, PARAM_INT },
    { "finetune_one", "Finetune 1", -7, 7, 0, PARAM_FLOAT },
    { "finetune_centre_one", "Finetune centre 1", -4, 4, 0, PARAM_FLOAT },
    { "inertia_one", "Inertia 1", 0.001, 1, 1, PARAM_FLOAT },
    { "wave_one", "DCO Waveform 1", 0, 4, 1, PARAM_INT },
    { "active_two", "DCO Active 2", 0, 1, 1, PARAM_BOOL },
    { "octave_two", "DCO Octave 2", -4, 4, 0, PARAM_INT },
    { "finetune_two", "Finetune 2", -7, 7, 0, PARAM_FLOAT },
    { "finetune_centre_two", "Finetune centre 2", -4, 4, 0, PARAM_FLOAT },
    { "inertia_two", "Inertia 2", 0.01, 1, 1, PARAM_FLOAT },
    { "wave_two", "DCO Wave 2", 0, 4, 1, PARAM_INT },
    { "active_three", "DCO Active 3", 0, 1, 1, PARAM_BOOL },
    { "octave_three", "DCO Octave 3", -4, 4, 0, PARAM_INT },
    { "finetune_three", "Finetune 3", -7, 7, 0, PARAM_FLOAT },
    { "finetune_centre_three", "Finetune centre 3", -4, 4, 0, PARAM_FLOAT },
    { "inertia_three", "Inertia 3", 0.01, 1, 1, PARAM_FLOAT },
    { "wave_three", "DCO Waveform 3", 0, 4, 1, PARAM_INT },
    { "patches", "Patches", 0, 1024, 1, PARAM_INT },
    { "legato", "Legato", 0, 1, 0, PARAM_BOOL },
    { "sync", "Sync", 0, 1, 0, PARAM_BOOL },
    { "master_volume", "Master volume", 5, 100, 80, PARAM_INT },
    { "midi_channel", "MIDI channel", 1, 16, 1, PARAM_INT },
    { "lfo1_dco1_pitch", "Route LFO1 to DCO1 Pitch", 0, 48, 0, PARAM_FLOAT },
    { "lfo1_dco2_pitch", "Route LFO1 to DCO2 Pitch", 0, 48, 0, PARAM_FLOAT },
    { "lfo1_dco3_pitch", "Route LFO1 to DCO3 Pitch", 0, 48, 0, PARAM_FLOAT },
    { "lfo1_dcf", "Route LFO1 to DCF", 0, 8, 0, PARAM_FLOAT },
    { "lfo1_wave", "LFO one waveform", 4, 7, 5, PARAM_INT },
    { "lfo1_speed", "LFO one speed", 2, 256, 0, PARAM_FLOAT },
    { "lfo2_dco1_pitch", "Route LFO2 to DCO1 Pitch", 0, 48, 0, PARAM_FLOAT },
    { "lfo2_dco2_pitch", "Route LFO2 to DCO2 Pitch", 0, 48, 0, PARAM_FLOAT },
    { "lfo2_dco3_pitch", "Route LFO2 to DCO3 Pitch", 0, 48, 0, PARAM_FLOAT },
    { "lfo2_dcf", "Route LFO2 to DCF", 0, 8, 0, PARAM_FLOAT },
    { "lfo2_wave", "LFO two waveform", 4, 7, 5, PARAM_INT },
    { "lfo2_speed", "LFO two speed", 2, 256, 0, PARAM_FLOAT },
    { "attack_one", "DCA envelope attack 1", 0, 70, 40, PARAM_FLOAT },
    { "decay_one", "DCA envelope decay 1", 30, 70, 50, PARAM_FLOAT },
    { "sustain_one", "DCA envelope sustain 1", 0, 1, 0, PARAM_FLOAT },
    { "release_one", "DCA envelope release 1", 30, 70, 50, PARAM_FLOAT },
    { "adsr1_amp_amount", "DCA1 routed to amp", 0.5, 16, 0.5, PARAM_FLOAT },
    { "adsr1_osc2_amount", "DCA1 routed to oscillator2 pitch", 0, 16, 0, PARAM_FLOAT },
    { "attack_two", "DCA1 envelope attack", 0, 70, 0, PARAM_FLOAT },
    { "decay_two", "DCA2 envelope decay", 30, 70, 40, PARAM_FLOAT },
    { "sustain_two", "DCA2 envelope sustain", 0, 1, 0, PARAM_FLOAT },
    { "release_two", "DCA2 envelope release", 30, 70, 30, PARAM_FLOAT },
    { "adsr2_dcf_amount", "DCA2 routed to DCF", 1, 16, 1, PARAM_FLOAT },
    { "adsr2_osc3_amount", "DCA2 routed to oscillator3 pitch", 0, 16, 0, PARAM_FLOAT },
    { "output_mode", "Master output mode", 0, 1, 0, PARAM_BOOL },
    { "dco1_output_channel", "DCO1 output channel", 0, 2, 0, PARAM_INT },
    { "dco2_output_channel", "DCO2 output channel", 0, 2, 2, PARAM_INT },
    { "dco3_output_channel", "DCO3 output channel", 0, 2, 1, PARAM_INT }
};

const char* MinatonParams::paramName(MinatonParamId index)
{
    return minaton_params[index].name;
}

const char* MinatonParams::paramSymbol(MinatonParamId index)
{
    return minaton_params[index].symbol;
}

float MinatonParams::paramDefaultValue(MinatonParamId index)
{
    return minaton_params[index].def;
}

float MinatonParams::paramMinValue(MinatonParamId index)
{
    return minaton_params[index].min;
}

float MinatonParams::paramMaxValue(MinatonParamId index)
{
    return minaton_params[index].max;
}

bool MinatonParams::paramFloat(MinatonParamId index)
{
    return (minaton_params[index].type == PARAM_FLOAT);
}

bool MinatonParams::paramBool(MinatonParamId index)
{
    return (minaton_params[index].type == PARAM_BOOL);
}

bool MinatonParams::paramInt(MinatonParamId index)
{
    return (minaton_params[index].type == PARAM_INT);
}
