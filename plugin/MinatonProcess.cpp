#include "MinatonPlugin.h"

void MinatonPlugin::_processAudioFrame(float* audio_l, float* audio_r, uint32_t frame_index)
{
    float mix1 = 0, mix2 = 0, mix3 = 0, delay;

    fSynthesizer->dca_update();

    delay++;
    if (delay > 20) {
        fSynthesizer->dco_cycle(3);
        fSynthesizer->dco_cycle(4);
        delay = 0;
    }

    if (fParameters[PARAM_ACTIVE_ONE]) {
        mix1 = fSynthesizer->dco_cycle(0);
    }
    if (fParameters[PARAM_ACTIVE_TWO]) {
        mix2 = fSynthesizer->dco_cycle(1);
    }
    if (fParameters[PARAM_ACTIVE_THREE]) {
        mix3 = fSynthesizer->dco_cycle(2);
    }

    float mixer_out_left = (mix1 + mix2) / fSynthesizer->master_volume;
    float mixer_out_right = (mix2 + mix3) / fSynthesizer->master_volume;

    // mixer_out =  1.1f * mixer_out - 0.2f * mixer_out * mixer_out * mixer_out;

    mixer_out_left = fSynthesizer->dcf_left(mixer_out_left, fSynthesizer->envelope2_out(0.1, fSynthesizer->adsr_filter_amount2), 0.1);
    mixer_out_right = fSynthesizer->dcf_right(mixer_out_right, fSynthesizer->envelope2_out(0.1, fSynthesizer->adsr_filter_amount2), 0.1);

    audio_l[frame_index] = fSynthesizer->envelope1_out(mixer_out_left, fSynthesizer->adsr_amp_amount1);
    audio_r[frame_index] = fSynthesizer->envelope1_out(mixer_out_right, fSynthesizer->adsr_amp_amount1);
}

void MinatonPlugin::_processMidi(const uint8_t* data, const uint32_t size)
{
    // Procedure adapted from synthv1 (https://github.com/rncbc/synthv1).

    for (uint32_t i = 0; i < size; ++i) {

        // channel status
        const int channel = (data[i] & 0x0f) + 1;
        const int status = (data[i] & 0xf0);

        // all system common/real-time ignored
        if (status == 0xf0)
            continue;

        // check data size (#1)
        if (++i >= size)
            break;

        const int key = (data[i] & 0x7f);

        // program change
        if (status == 0xc0) {
            if (fParameters[PARAM_ACTIVE_ONE] || fParameters[PARAM_ACTIVE_TWO] || fParameters[PARAM_ACTIVE_THREE])
                d_stderr2("[Unimplemented] program change = %d", key);
            continue;
        }

        // channel aftertouch
        if (status == 0xd0) {
            const float pre = float(key) / 127.0f;
            if (fParameters[PARAM_ACTIVE_ONE] || fParameters[PARAM_ACTIVE_TWO] || fParameters[PARAM_ACTIVE_THREE])
                d_stderr2("[Unimplemented] Channel aftertouch pressure = %d", pre);
            continue;
        }

        // check data size (#2)
        if (++i >= size)
            break;

        // channel value
        const int value = (data[i] & 0x7f);

        d_stderr("[MIDI] channel = 0x%2x, status = 0x%2x, key = %d, channel value = %d", channel, status, key, value);

        // note on
        if (status == 0x90 && value > 0) {
#if 0
            d_stderr("Current key on: %d", key);
#else
            m_key = key;
            m_period = m_rate * 4.0 / 440.0f; // LV2::key2hz(m_key);

            if (last_note == -1 || fParameters[PARAM_LEGATO] == 0) {
                fSynthesizer->trigger_envelope1();
                fSynthesizer->trigger_envelope2();
            }
            fSynthesizer->set_tuning(m_key - 38);
            last_note = m_key;
#endif
        }
        // note off
        else if (status == 0x80 || (status == 0x90 && value == 0)) {
            m_key = -1;
            fSynthesizer->release_envelope1();
            fSynthesizer->release_envelope2();
            last_note = -1;
        }
        // key pressure/poly.aftertouch
        else if (status == 0xa0) {
            const float pre = float(value) / 127.0f;
            d_stderr2("[Unimplemented] Key pressure/poly aftertouch: pressure = %f", pre);
        }
        // control change. `key` is control's ID (CC ID).
        else if (status == 0xb0) {
            d_stderr2("[Unimplemented] Control change: CC#%d", key);
        }
        // pitch bend
        else if (status == 0xe0) {
            const float pitchbend = float(key + (value << 7) - 0x2000) / 8192.0f;
            d_stderr2("[Unimplemented] Pitch bend: bend to %f", pitchbend);
        }
    }
}