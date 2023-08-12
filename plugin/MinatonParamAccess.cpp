#include "MinatonPlugin.h"

float MinatonPlugin::_obtainSynthParameter(MinatonParamId index) const
{
    switch (index) {
    // OSC activation
    case PARAM_ACTIVE_ONE:
        return fSynthesizer->active1;
    case PARAM_ACTIVE_TWO:
        return fSynthesizer->active2;
    case PARAM_ACTIVE_THREE:
        return fSynthesizer->active3;

    // Master output
    case PARAM_MASTER_VOLUME:
        return fSynthesizer->master_volume;
    case PARAM_LEGATO:
        return fSynthesizer->get_legato();
    case PARAM_SYNC:
        return fSynthesizer->get_sync_mode();

    // DCO Wave 1
    case PARAM_WAVE_ONE:
        return fSynthesizer->get_dco_wave(DCO_ONE);
    case PARAM_INERTIA_ONE:
        return fSynthesizer->get_dco_inertia(DCO_ONE);

    // DCO Wave 2
    case PARAM_WAVE_TWO:
        return fSynthesizer->get_dco_wave(DCO_TWO);
    case PARAM_INERTIA_TWO:
        return fSynthesizer->get_dco_inertia(DCO_TWO);

    // DCO Wave 3
    case PARAM_WAVE_THREE:
        return fSynthesizer->get_dco_wave(DCO_THREE);
    case PARAM_INERTIA_THREE:
        return fSynthesizer->get_dco_inertia(DCO_THREE);

    // LFO1 Speed
    case PARAM_LFO1_SPEED:
        return fSynthesizer->get_dco_frequency(LFO_ONE);
    case PARAM_LFO1_WAVE:
        return fSynthesizer->get_dco_wave(LFO_ONE);

    // LFO2 Speed
    case PARAM_LFO2_SPEED:
        return fSynthesizer->get_dco_frequency(LFO_TWO);
    case PARAM_LFO2_WAVE:
        return fSynthesizer->get_dco_wave(LFO_TWO);

    // LFO1 to DCOs
    case PARAM_LFO1_DCO1_PITCH:
        return fSynthesizer->get_dco_lfo1_amount(DCO_ONE);
    case PARAM_LFO1_DCO2_PITCH:
        return fSynthesizer->get_dco_lfo1_amount(DCO_TWO);
    case PARAM_LFO1_DCO3_PITCH:
        return fSynthesizer->get_dco_lfo1_amount(DCO_THREE);
    case PARAM_LFO1_DCF:
        return fSynthesizer->lfo1_amount;

    // LFO2 to DCOs
    case PARAM_LFO2_DCO1_PITCH:
        return fSynthesizer->get_dco_lfo2_amount(DCO_ONE);
    case PARAM_LFO2_DCO2_PITCH:
        return fSynthesizer->get_dco_lfo2_amount(DCO_TWO);
    case PARAM_LFO2_DCO3_PITCH:
        return fSynthesizer->get_dco_lfo2_amount(DCO_THREE);
    case PARAM_LFO2_DCF:
        return fSynthesizer->lfo2_amount;

    // ADSR envelope 1
    case PARAM_ATTACK_ONE:
        return fSynthesizer->envelope1.attack;
    case PARAM_DECAY_ONE:
        return fSynthesizer->envelope1.decay;
    case PARAM_SUSTAIN_ONE:
        return fSynthesizer->envelope1.sustain;
    case PARAM_RELEASE_ONE:
        return fSynthesizer->envelope1.release;
    case PARAM_ADSR1_AMP_AMOUNT:
        return fSynthesizer->adsr_amp_amount1;
    case PARAM_ADSR1_OSC2_AMOUNT:
        return fSynthesizer->adsr_osc2_amount1;

    // ADSR envelope 2
    case PARAM_ATTACK_TWO:
        return fSynthesizer->envelope2.attack;
    case PARAM_DECAY_TWO:
        return fSynthesizer->envelope2.decay;
    case PARAM_SUSTAIN_TWO:
        return fSynthesizer->envelope2.sustain;
    case PARAM_RELEASE_TWO:
        return fSynthesizer->envelope2.release;
    case PARAM_ADSR2_DCF_AMOUNT:
        return fSynthesizer->adsr_filter_amount2;
    case PARAM_ADSR2_OSC3_AMOUNT:
        return fSynthesizer->adsr_osc3_amount2;

    // Master frequency / resonance
    case PARAM_FREQUENCY:
        return fSynthesizer->master_frequency * 9;
    case PARAM_RESONANCE:
        return fSynthesizer->master_resonance * 4;

    // OSC1 octave / pitch
    case PARAM_OCTAVE_ONE:
        return fSynthesizer->octave1;
    case PARAM_FINETUNE_ONE:
        return fSynthesizer->pitch1;

    // OSC2 octave / pitch
    case PARAM_OCTAVE_TWO:
        return fSynthesizer->octave2;
    case PARAM_FINETUNE_TWO:
        return fSynthesizer->pitch2;

    // OSC3 octave / pitch
    case PARAM_OCTAVE_THREE:
        return fSynthesizer->octave3;
    case PARAM_FINETUNE_THREE:
        return fSynthesizer->pitch3;

    // Output mode & channel routes
    case PARAM_OUTPUT_MODE:
        return fSynthesizer->get_output_mode();
    case PARAM_DCO1_OUTPUT_CHANNEL:
        return fSynthesizer->get_dco_output_channel(DCO_ONE);
    case PARAM_DCO2_OUTPUT_CHANNEL:
        return fSynthesizer->get_dco_output_channel(DCO_TWO);
    case PARAM_DCO3_OUTPUT_CHANNEL:
        return fSynthesizer->get_dco_output_channel(DCO_THREE);

    // Unused parameters
    case PARAM_FINETUNE_CENTRE_ONE:
    case PARAM_FINETUNE_CENTRE_TWO:
    case PARAM_FINETUNE_CENTRE_THREE:
    case PARAM_PATCHES:
    case PARAM_MIDI_CHANNEL:
    case PARAM_COUNT:
        break;
    }

    return 0.0f;
}

void MinatonPlugin::_applySynthParameter(MinatonParamId index, float value)
{
    switch (index) {
    // OSC activation
    case PARAM_ACTIVE_ONE:
        fSynthesizer->active1 = value;
        break;
    case PARAM_ACTIVE_TWO:
        fSynthesizer->active2 = value;
        break;
    case PARAM_ACTIVE_THREE:
        fSynthesizer->active3 = value;
        break;

    // Master output
    case PARAM_MASTER_VOLUME:
        fSynthesizer->master_volume = value;
        break;
    case PARAM_LEGATO:
        fSynthesizer->set_legato(value);
        break;
    case PARAM_SYNC:
        fSynthesizer->set_sync_mode(value);
        break;

    // DCO Wave 1
    case PARAM_WAVE_ONE:
        fSynthesizer->set_dco_wave(DCO_ONE, wave_id(value));
        break;
    case PARAM_INERTIA_ONE:
        fSynthesizer->set_dco_inertia(DCO_ONE, (int)value);
        break;

    // DCO Wave 2
    case PARAM_WAVE_TWO:
        fSynthesizer->set_dco_wave(DCO_TWO, wave_id(value));
        break;
    case PARAM_INERTIA_TWO:
        fSynthesizer->set_dco_inertia(DCO_TWO, (int)value);
        break;

    // DCO Wave 3
    case PARAM_WAVE_THREE:
        fSynthesizer->set_dco_wave(DCO_THREE, wave_id(value));
        break;
    case PARAM_INERTIA_THREE:
        fSynthesizer->set_dco_inertia(DCO_THREE, (int)value);
        break;

    // LFO1 Speed
    case PARAM_LFO1_SPEED:
        fSynthesizer->set_freq(LFO_ONE, value);
        break;
    case PARAM_LFO1_WAVE:
        fSynthesizer->set_dco_wave(LFO_ONE, wave_id(value));
        fSynthesizer->set_freq(LFO_ONE, fSynthesizer->get_dco_frequency(LFO_ONE));
        break;

    // LFO2 Speed
    case PARAM_LFO2_SPEED:
        fSynthesizer->set_freq(LFO_TWO, value);
        break;
    case PARAM_LFO2_WAVE:
        fSynthesizer->set_dco_wave(LFO_TWO, wave_id(value));
        fSynthesizer->set_freq(4, fSynthesizer->get_dco_frequency(LFO_TWO));
        break;

    // LFO1 to DCOs
    case PARAM_LFO1_DCO1_PITCH:
        fSynthesizer->set_dco_lfo1_amount(DCO_ONE, value);
        break;
    case PARAM_LFO1_DCO2_PITCH:
        fSynthesizer->set_dco_lfo1_amount(DCO_TWO, value);
        break;
    case PARAM_LFO1_DCO3_PITCH:
        fSynthesizer->set_dco_lfo1_amount(DCO_THREE, value);
        break;
    case PARAM_LFO1_DCF:
        fSynthesizer->lfo1_amount = value;
        break;

    // LFO2 to DCOs
    case PARAM_LFO2_DCO1_PITCH:
        fSynthesizer->set_dco_lfo2_amount(DCO_ONE, value);
        break;
    case PARAM_LFO2_DCO2_PITCH:
        fSynthesizer->set_dco_lfo2_amount(DCO_TWO, value);
        break;
    case PARAM_LFO2_DCO3_PITCH:
        fSynthesizer->set_dco_lfo2_amount(DCO_THREE, value);
        break;
    case PARAM_LFO2_DCF:
        fSynthesizer->lfo2_amount = value;
        break;

    // ADSR envelope 1
    case PARAM_ATTACK_ONE:
        fSynthesizer->envelope1.attack = value;
        break;
    case PARAM_DECAY_ONE:
        fSynthesizer->envelope1.decay = value;
        break;
    case PARAM_SUSTAIN_ONE:
        fSynthesizer->envelope1.sustain = value;
        break;
    case PARAM_RELEASE_ONE:
        fSynthesizer->envelope1.release = value;
        break;
    case PARAM_ADSR1_AMP_AMOUNT:
        fSynthesizer->adsr_amp_amount1 = value;
        break;
    case PARAM_ADSR1_OSC2_AMOUNT:
        fSynthesizer->adsr_osc2_amount1 = value;
        break;

    // ADSR envelope 2
    case PARAM_ATTACK_TWO:
        fSynthesizer->envelope2.attack = value;
        break;
    case PARAM_DECAY_TWO:
        fSynthesizer->envelope2.decay = value;
        break;
    case PARAM_SUSTAIN_TWO:
        fSynthesizer->envelope2.sustain = value;
        break;
    case PARAM_RELEASE_TWO:
        fSynthesizer->envelope2.release = value;
        break;
    case PARAM_ADSR2_DCF_AMOUNT:
        fSynthesizer->adsr_filter_amount2 = value;
        break;
    case PARAM_ADSR2_OSC3_AMOUNT:
        fSynthesizer->adsr_osc3_amount2 = value;
        break;

    // Master frequency / resonance
    case PARAM_FREQUENCY:
        fSynthesizer->master_frequency = value / 9;
        break;
    case PARAM_RESONANCE:
        fSynthesizer->master_resonance = value / 4;
        break;

    // OSC1 octave / pitch
    case PARAM_OCTAVE_ONE:
        fSynthesizer->octave1 = value;
        break;
    case PARAM_FINETUNE_ONE:
        fSynthesizer->pitch1 = value;
        break;

    // OSC2 octave / pitch
    case PARAM_OCTAVE_TWO:
        fSynthesizer->octave2 = value;
        break;
    case PARAM_FINETUNE_TWO:
        fSynthesizer->pitch2 = value;
        break;

    // OSC3 octave / pitch
    case PARAM_OCTAVE_THREE:
        fSynthesizer->octave3 = value;
        break;
    case PARAM_FINETUNE_THREE:
        fSynthesizer->pitch3 = value;
        break;

    // Output mode & channel routes
    case PARAM_OUTPUT_MODE:
        fSynthesizer->set_output_mode(value);
        break;
    case PARAM_DCO1_OUTPUT_CHANNEL:
        fSynthesizer->set_dco_output_channel(DCO_ONE, minaton_channel_mode(value));
        break;
    case PARAM_DCO2_OUTPUT_CHANNEL:
        fSynthesizer->set_dco_output_channel(DCO_TWO, minaton_channel_mode(value));
        break;
    case PARAM_DCO3_OUTPUT_CHANNEL:
        fSynthesizer->set_dco_output_channel(DCO_THREE, minaton_channel_mode(value));
        break;

    // Unused parameters
    case PARAM_FINETUNE_CENTRE_ONE:
    case PARAM_FINETUNE_CENTRE_TWO:
    case PARAM_FINETUNE_CENTRE_THREE:
    case PARAM_PATCHES:
    case PARAM_MIDI_CHANNEL:
    case PARAM_COUNT:
        break;
    }
}
