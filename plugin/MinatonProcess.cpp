#include "MinatonPlugin.h"

static inline float calculate_velocity_multiply_factor(unsigned char velocity)
{
    // Sanity check. Volume out of range will corrupt the synthesizer!
    // Input volume range: [1, 127].
    if (unlikely(velocity > 127)) {
        velocity = 127;
    } else if (unlikely(velocity < 1)) {
        velocity = 1;
    }

    // Optimize the formula by Reciprocal Multiplication. This highly improves performance.
    // NOTE: Original formula is: vel_multiply_factor = velocity / 127.0f.
    static constexpr float velocity_conv_factor = 1.0f / 127.0f; // Preload multiply factor on compile time
    return (float)velocity * velocity_conv_factor; // Optimized calculation
}

static void mixer_out_sanitize(float& mixer_out)
{
    // Check for illegal sample data value: NaN or out-of-bound.
    // Then replace the error value with a fallback one.
    //
    // This sanitizer is adapted from src/work/main.cpp.

    if (isnan(mixer_out)) {
        // The most common situation is getting an NaN
        mixer_out = -0.984375;
    } else if (mixer_out <= -1.25f) {
        // The second common one is out of left boundary
        mixer_out = -0.984375;
    } else if (mixer_out >= 1.25f) {
        // Out of right boundary.
        mixer_out = 0.984375;
    } else {
        // Seems that the line below is bypassed by Thunderox originally
        // mixer_out = 1.1f * mixer_out - 0.2f * mixer_out * mixer_out * mixer_out;
        return;
    }
}

void MinatonPlugin::_processAudioFrame(float* audio_l, float* audio_r, uint32_t frame_index)
{
    float mix1 = 0, mix2 = 0, mix3 = 0;
    volatile float delay; // LFO delay before every cycle
                          // May be optimized out when building with -O3.
                          // Defining with volatile can prevent unexpected optimization.

    fSynthesizer->dca_update();

    delay++;
    if (delay > 20) {
        fSynthesizer->dco_cycle(3);
        fSynthesizer->dco_cycle(4);
        delay = 0;
    }

    if (fSynthesizer->active1) {
        mix1 = fSynthesizer->dco_cycle(0);
    }
    if (fSynthesizer->active2) {
        mix2 = fSynthesizer->dco_cycle(1);
    }
    if (fSynthesizer->active3) {
        mix3 = fSynthesizer->dco_cycle(2);
    }

    const float& volume_reciprocal_div_factor = m_volumeReciprocalDivFactor;
    float velocity_mul_factor = calculate_velocity_multiply_factor(m_velocity);

    if (fSynthesizer->get_output_mode() == OUTPUT_STEREO) { // Stereo output
        float mixer_out_left = ((fSynthesizer->is_dco_out_to_L(0) ? mix1 : 0.0f) + (fSynthesizer->is_dco_out_to_L(1) ? mix2 : 0.0f) + (fSynthesizer->is_dco_out_to_L(2) ? mix3 : 0.0f)) * volume_reciprocal_div_factor * velocity_mul_factor;
        float mixer_out_right = ((fSynthesizer->is_dco_out_to_R(0) ? mix1 : 0.0f) + (fSynthesizer->is_dco_out_to_R(1) ? mix2 : 0.0f) + (fSynthesizer->is_dco_out_to_R(2) ? mix3 : 0.0f)) * volume_reciprocal_div_factor * velocity_mul_factor;

        // Must sanitize mixer outputs, since NaN or out-of-bound samples appear occasionally.
        // Even a illeagal value still corrupts the synthesizer!
        mixer_out_sanitize(mixer_out_left);
        mixer_out_sanitize(mixer_out_right);

        // mixer_out =  1.1f * mixer_out - 0.2f * mixer_out * mixer_out * mixer_out;

        // Apply DCF (Moog resonant filter)
        //
        // If there is only ONE channel routed to DCO, then DCF will make Minaton overload:
        // it consumes more than 5.0% CPU on REAPER, with "Release" CMake profile.
        //
        // For example:
        //   - DCO1 -> L, DCO2 -> L/R, DCO3 -> R, but only DCO1 is active.
        //   - All DCOs route to L, and active at least one DCO.
        //
        // To solve this issue, I have a workaround. Only apply DCF for channel, when:
        //   - At least one DCO is routed there, and
        //   - that DCO is active as well.
        //
        // NOTICE: Make sure you don't forget the outer parentheses of IS_DCO_OUT_TO_(L|R)!
        if ((fSynthesizer->is_dco_out_to_L(0) && fSynthesizer->active1) || (fSynthesizer->is_dco_out_to_L(1) && fSynthesizer->active2) || (fSynthesizer->is_dco_out_to_L(2) && fSynthesizer->active3))
            mixer_out_left = fSynthesizer->dcf_left(mixer_out_left, fSynthesizer->envelope2_out(0.1, fSynthesizer->adsr_filter_amount2), 0.1);
        if ((fSynthesizer->is_dco_out_to_R(0) && fSynthesizer->active1) || (fSynthesizer->is_dco_out_to_R(1) && fSynthesizer->active2) || (fSynthesizer->is_dco_out_to_R(2) && fSynthesizer->active3))
            mixer_out_right = fSynthesizer->dcf_right(mixer_out_right, fSynthesizer->envelope2_out(0.1, fSynthesizer->adsr_filter_amount2), 0.1);

        audio_l[frame_index] = fSynthesizer->envelope1_out(mixer_out_left, fSynthesizer->adsr_amp_amount1);
        audio_r[frame_index] = fSynthesizer->envelope1_out(mixer_out_right, fSynthesizer->adsr_amp_amount1);
    } else { // Mono output
        float mixer_out = (mix1 + mix2 + mix3) * volume_reciprocal_div_factor * velocity_mul_factor;

        mixer_out_sanitize(mixer_out);

        // dcf_left() and dcf_right() has the same output on mono mix-down
        mixer_out = fSynthesizer->dcf_left(mixer_out, fSynthesizer->envelope2_out(0.1, fSynthesizer->adsr_filter_amount2), 0.1);

        audio_l[frame_index] = fSynthesizer->envelope1_out(mixer_out, fSynthesizer->adsr_amp_amount1);
        audio_r[frame_index] = fSynthesizer->envelope1_out(mixer_out, fSynthesizer->adsr_amp_amount1);
    }
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
            if (fSynthesizer->active1 || fSynthesizer->active2 || fSynthesizer->active3)
                d_stderr2("[Unimplemented] program change = %d", key);
            continue;
        }

        // channel aftertouch
        if (status == 0xd0) {
            const float pre = float(key) / 127.0f;
            if (fSynthesizer->active1 || fSynthesizer->active2 || fSynthesizer->active3)
                d_stderr2("[Unimplemented] Channel aftertouch pressure = %d", pre);
            continue;
        }

        // check data size (#2)
        if (++i >= size)
            break;

        // channel value (velocity)
        const int value = (data[i] & 0x7f);

        d_stderr("[MIDI] channel = 0x%2x, status = 0x%2x, key = %d, channel value = %d", channel, status, key, value);

        // note on
        if (status == 0x90 && value > 0) {
            // Reset DCO audio frame position to avoid unexpected phase distortion
            fSynthesizer->reset_dco_out_position();

#if 0
            d_stderr("Current key on: %d", key);
#else
            m_key = key;
            m_period = m_rate * 4.0 / 440.0f; // LV2::key2hz(m_key);
            m_velocity = (unsigned char)value;

            if (last_note == -1 || fSynthesizer->get_legato() == 0) {
                fSynthesizer->trigger_envelope1();
                fSynthesizer->trigger_envelope2();
            }
            fSynthesizer->set_tuning(m_key - 38);
            last_note = m_key;
#endif
        }
        // note off
        else if (status == 0x80 || (status == 0x90 && value == 0)) {
            m_key = key;

            if (m_key == last_note) {
                fSynthesizer->release_envelope1();
                fSynthesizer->release_envelope2();

                last_note = -1;
            }
        }
        // key pressure/poly.aftertouch
        else if (status == 0xa0) {
            const float pre = float(value) / 127.0f;
            d_stderr2("[Unimplemented] Key pressure/poly aftertouch: pressure = %f", pre);
        }
        // control change. `key` is control's ID (CC ID).
        else if (status == 0xb0) {
            if (key == 0x78) { // Process All Notes Off (0x78)
                d_stderr("[MIDI] All Notes Off");
                fSynthesizer->panic();

                last_note = -1;
            } else if (key == 0x7b) { // Process All Sounds Off (0x7b)
                d_stderr("[MIDI] All Sounds Off");
                fSynthesizer->panic();

                last_note = -1;
            } else {
                d_stderr2("[Unimplemented] Control change: CC#%d", key);
            }
        }
        // pitch bend
        // FIXME: Bend value +8192 (raw data: 0xe0 0x7f 0x7f) acts as same as -8192.
        //        Input MIDI event in Plugin::run() has wrong values, even though REAPER passes the right data to Minaton.
        else if (status == 0xe0) {
            const float pitchbend = float((data[1] & 0x7f) + (value << 7) - 0x2000) / 8192.0f;
            fSynthesizer->set_tuning(m_key - 38 + pitchbend);
        }
    }
}
