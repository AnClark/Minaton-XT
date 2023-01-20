#pragma once

enum MinatonParamId {
    PARAM_FREQUENCY,
    PARAM_RESONANCE,
    PARAM_ACTIVE_ONE,
    PARAM_OCTAVE_ONE,
    PARAM_FINETUNE_ONE,
    PARAM_FINETUNE_CENTRE_ONE,
    PARAM_INERTIA_ONE,
    PARAM_WAVE_ONE,
    PARAM_ACTIVE_TWO,
    PARAM_OCTAVE_TWO,
    PARAM_FINETUNE_TWO,
    PARAM_FINETUNE_CENTRE_TWO,
    PARAM_INERTIA_TWO,
    PARAM_WAVE_TWO,
    PARAM_ACTIVE_THREE,
    PARAM_OCTAVE_THREE,
    PARAM_FINETUNE_THREE,
    PARAM_FINETUNE_CENTRE_THREE,
    PARAM_INERTIA_THREE,
    PARAM_WAVE_THREE,
    PARAM_PATCHES,
    PARAM_LEGATO,
    PARAM_SYNC,
    PARAM_MASTER_VOLUME,
    PARAM_MIDI_CHANNEL,
    PARAM_LFO1_DCO1_PITCH,
    PARAM_LFO1_DCO2_PITCH,
    PARAM_LFO1_DCO3_PITCH,
    PARAM_LFO1_DCF,
    PARAM_LFO1_WAVE,
    PARAM_LFO1_SPEED,
    PARAM_LFO2_DCO1_PITCH,
    PARAM_LFO2_DCO2_PITCH,
    PARAM_LFO2_DCO3_PITCH,
    PARAM_LFO2_DCF,
    PARAM_LFO2_WAVE,
    PARAM_LFO2_SPEED,
    PARAM_ATTACK_ONE,
    PARAM_DECAY_ONE,
    PARAM_SUSTAIN_ONE,
    PARAM_RELEASE_ONE,
    PARAM_ADSR1_AMP_AMOUNT,
    PARAM_ADSR1_OSC2_AMOUNT,
    PARAM_ATTACK_TWO,
    PARAM_DECAY_TWO,
    PARAM_SUSTAIN_TWO,
    PARAM_RELEASE_TWO,
    PARAM_ADSR2_DCF_AMOUNT,
    PARAM_ADSR2_OSC3_AMOUNT,
    PARAM_COUNT
};

enum MinatonParamType {
    PARAM_FLOAT,
    PARAM_INT,
    PARAM_BOOL
};

struct MinatonParamDefinition {
    const char* symbol;
    const char* name;
    float min;
    float max;
    float def;
    char type;
};

namespace MinatonParams {

const char* paramName(MinatonParamId index);
const char* paramSymbol(MinatonParamId index);

float paramDefaultValue(MinatonParamId index);
float paramMinValue(MinatonParamId index);
float paramMaxValue(MinatonParamId index);

bool paramFloat(MinatonParamId index);
bool paramBool(MinatonParamId index);
bool paramInt(MinatonParamId index);
}
