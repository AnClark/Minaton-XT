
#include "synth.hpp"

using namespace LV2;

//---------------------------------------------------------------------------------------

class Minaton_voice : public LV2::Voice {
public:
    minaton_synth new_synth;

    Minaton_voice(double rate)
        : m_key(LV2::INVALID_KEY)
        , m_rate(rate)
        , m_period(10)
        , m_counter(0)
    {

        ifstream myfile;
        myfile.open("/tmp/minaton.conf");
        myfile >> new_synth.bundle_path;
        myfile.close();

        new_synth.init();
        new_synth.init_dcas();
        new_synth.init_dcf();

        new_synth.attack = 0;
        new_synth.decay = 1;
        new_synth.wait = 2;
        new_synth.release = 3;
        new_synth.dormant = 4;
        new_synth.selectivity = 90;
        new_synth.gain1_left = 0;
        new_synth.gain2_left = 1;
        new_synth.ratio_left = 7;
        new_synth.cap_left = 0;
        new_synth.gain1_right = 0;
        new_synth.gain2_right = 1;
        new_synth.ratio_right = 7;
        new_synth.cap_right = 0;

        last_note = -1;
        control_delay = 20;

        new_synth.release_envelope1();
        new_synth.release_envelope2();
        new_synth.envelope1.level = 0;
        new_synth.envelope2.level = 0;
    }

    void on(unsigned char key, unsigned char velocity)
    {
        m_key = key;
        m_period = m_rate * 4.0 / LV2::key2hz(m_key);

        if (last_note == -1 || p(p_legato)[0] == 0) {
            new_synth.trigger_envelope1();
            new_synth.trigger_envelope2();
        }
        new_synth.set_tuning(m_key - 38);
        last_note = m_key;
    }

    void off(unsigned char velocity)
    {
        m_key = LV2::INVALID_KEY;
        new_synth.release_envelope1();
        new_synth.release_envelope2();
        last_note = -1;
    }

    unsigned char get_key() const
    {
        return m_key;
    }

    //---------------------------------------------------------------------------------------------------------------------------------

    ~Minaton_voice()
    {
        new_synth.cleanup();
    }

    //---------------------------------------------------------------------------------------------------------------------------------
    void render(uint32_t from, uint32_t to)
    {

        if (new_synth.envelope1.state == new_synth.dormant) {
            return;
        }

        if ((!p(p_active_one)[0] && !p(p_active_two)[0] && !p(p_active_three)[0]) || new_synth.master_volume == 100) {
            for (unsigned int x = from; x < to; ++x) {
                p(p_left_output)[x] = 0;
                p(p_right_output)[x] = 0;
            }
            return; // No oscillators enabled or volume at zero, clear ring buffer and return
        }

        float mix1 = 0, mix2 = 0, mix3 = 0, delay;

        //	++control_delay;

        // if (control_delay>20)
        //{
        //		control_delay = 0;

        new_synth.master_volume = p(p_master_volume)[0];
        new_synth.set_legato(p(p_legato)[0]);
        new_synth.set_sync_mode(p(p_sync)[0]);

        new_synth.dco_wave[0] = p(p_wave_one)[0];
        new_synth.dco_inertia[0] = p(p_inertia_one)[0];

        new_synth.dco_wave[1] = p(p_wave_two)[0];
        new_synth.dco_inertia[1] = p(p_inertia_two)[0];

        new_synth.dco_wave[2] = p(p_wave_three)[0];
        new_synth.dco_inertia[2] = p(p_inertia_three)[0];

        new_synth.set_freq(3, p(p_lfo1_speed)[0]);
        new_synth.dco_wave[3] = p(p_lfo1_wave)[0];

        new_synth.set_freq(4, p(p_lfo2_speed)[0]);
        new_synth.dco_wave[4] = p(p_lfo2_wave)[0];

        new_synth.dco_lfo1_amount[0] = p(p_lfo1_dco1_pitch)[0];
        new_synth.dco_lfo1_amount[1] = p(p_lfo1_dco2_pitch)[0];
        new_synth.dco_lfo1_amount[2] = p(p_lfo1_dco3_pitch)[0];
        new_synth.lfo1_amount = p(p_lfo1_dcf)[0];

        new_synth.dco_lfo2_amount[0] = p(p_lfo2_dco1_pitch)[0];
        new_synth.dco_lfo2_amount[1] = p(p_lfo2_dco2_pitch)[0];
        new_synth.dco_lfo2_amount[2] = p(p_lfo2_dco3_pitch)[0];
        new_synth.lfo2_amount = p(p_lfo2_dcf)[0];

        new_synth.envelope1.attack = p(p_attack_one)[0];
        new_synth.envelope1.decay = p(p_decay_one)[0];
        new_synth.envelope1.sustain = p(p_sustain_one)[0];
        new_synth.envelope1.release = p(p_release_one)[0];
        new_synth.adsr_amp_amount1 = p(p_adsr1_amp_amount)[0];
        new_synth.adsr_osc2_amount1 = p(p_adsr1_osc2_amount)[0];

        new_synth.envelope2.attack = p(p_attack_two)[0];
        new_synth.envelope2.decay = p(p_decay_two)[0];
        new_synth.envelope2.sustain = p(p_sustain_two)[0];
        new_synth.envelope2.release = p(p_release_two)[0];
        new_synth.adsr_filter_amount2 = p(p_adsr2_dcf_amount)[0];
        new_synth.adsr_osc3_amount2 = p(p_adsr2_osc3_amount)[0];
        //	}

        new_synth.master_frequency = p(p_frequency)[0] / 9;
        new_synth.master_resonance = p(p_resonance)[0] / 4;

        if (p(p_active_one)[0]) {
            new_synth.octave1 = p(p_octave_one)[0];
            new_synth.pitch1 = p(p_finetune_one)[0];
        }

        if (p(p_active_two)[0]) {
            new_synth.pitch2 = p(p_finetune_two)[0];
            new_synth.octave2 = p(p_octave_two)[0];
        }

        if (p(p_active_three)[0]) {
            new_synth.octave3 = p(p_octave_three)[0];
            new_synth.pitch3 = p(p_finetune_three)[0];
        }

        for (unsigned int x = from; x < to; x++) {
            new_synth.dca_update();

            delay++;
            if (delay > 20) {
                new_synth.dco_cycle(3);
                new_synth.dco_cycle(4);
                delay = 0;
            }

            if (p(p_active_one)[0]) {
                mix1 = new_synth.dco_cycle(0);
            }
            if (p(p_active_two)[0]) {
                mix2 = new_synth.dco_cycle(1);
            }
            if (p(p_active_three)[0]) {
                mix3 = new_synth.dco_cycle(2);
            }

            float mixer_out_left = (mix1 + mix2) / new_synth.master_volume;
            float mixer_out_right = (mix2 + mix3) / new_synth.master_volume;

            // mixer_out =  1.1f * mixer_out - 0.2f * mixer_out * mixer_out * mixer_out;

            mixer_out_left = new_synth.dcf_left(mixer_out_left, new_synth.envelope2_out(0.1, new_synth.adsr_filter_amount2), 0.1);
            mixer_out_right = new_synth.dcf_right(mixer_out_right, new_synth.envelope2_out(0.1, new_synth.adsr_filter_amount2), 0.1);

            p(p_left_output)[x] = new_synth.envelope1_out(mixer_out_left, new_synth.adsr_amp_amount1);
            p(p_right_output)[x] = new_synth.envelope1_out(mixer_out_right, new_synth.adsr_amp_amount1);
        }
    }

private:
    unsigned char m_key;
    double m_rate;
    uint32_t m_period;
    uint32_t m_counter;
    int last_note;
    int control_delay;
};

//---------------------------------------------------------------------------------------------------------------------------------
class Minaton : public LV2::Synth<Minaton_voice, Minaton> {
public:
    Minaton(double rate)
        : LV2::Synth<Minaton_voice, Minaton>(p_n_ports, p_midi_in)
    {

        add_voices(new Minaton_voice(rate));
        add_audio_outputs(p_left_output, p_right_output);
    }

    //---------------------------------------------------------------------------------------------------------------------

    ~Minaton()
    {
    }

    //--------------------------------------------------------------------------------------------------------------------

    void post_process(uint32_t from, uint32_t to)
    {
        // for (uint32_t i = from; i < to; ++i) {
        // p(AUDIO_PORT)[i] = m_filterstate * 0.9 + p(AUDIO_PORT)[i] * 0.1;
        // m_filterstate = p(AUDIO_PORT)[i];
        //}
    }
};

//---------------------------------------------------------------------------------------------------------------------------------

static int _ = Minaton::register_class("http://nickbailey/minaton");
