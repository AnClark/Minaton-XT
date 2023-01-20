

#include "dca.h"
#include "dcf.h"
#include "dco.h"
#include "gui.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <math.h>
#include <sstream>
using namespace std;

int save_patch(string patch_name)
{

    transform(patch_name.begin(), patch_name.end(), patch_name.begin(), ::toupper);

    stringstream file_name;

    file_name << "patches/" << patch_name << ".minatron";

    ofstream myfile;
    myfile.open(file_name.str().c_str());
    myfile << "minaton1" << endl;

    // SAVE VARIOUS GLOBALS

    myfile << "FLAGS" << endl;
    myfile << get_legato() << endl;
    myfile << get_sync_mode() << endl;
    myfile << master_volume->value() << endl;
    myfile << midi_channel->value() << endl;
    myfile << endl;

    // SAVE FILTER

    myfile << "FILTER" << endl;
    myfile << freq->value() << endl;
    myfile << res->value() << endl;
    myfile << endl;

    // SAVE OSCILLATOR ONE

    myfile << "OSCILLATOR ONE" << endl;
    myfile << get_dco_state(0) << endl;
    myfile << octave1->value() << endl;
    myfile << pitch1->value() << endl;
    myfile << wave1->value() << endl;
    myfile << endl;

    // SAVE OSCILLATOR TWO

    myfile << "OSCILLATOR TWO" << endl;
    myfile << get_dco_state(1) << endl;
    myfile << octave2->value() << endl;
    myfile << pitch2->value() << endl;
    myfile << wave2->value() << endl;
    myfile << endl;

    // SAVE OSCILLATOR THREE

    myfile << "OSCILLATOR THREE" << endl;
    myfile << get_dco_state(2) << endl;
    myfile << octave3->value() << endl;
    myfile << pitch3->value() << endl;
    myfile << wave3->value() << endl;
    myfile << endl;

    // SAVE LFO ONE

    myfile << "LFO ONE" << endl;
    myfile << lfo1_dco1_pitch->value() << endl;
    myfile << lfo1_dco2_pitch->value() << endl;
    myfile << lfo1_dco3_pitch->value() << endl;
    myfile << lfo1_dcf->value() << endl;
    myfile << wave4->value() << endl;
    myfile << lfo1speed->value() << endl;
    myfile << endl;

    // SAVE LFO TWO

    myfile << "LFO TWO" << endl;
    myfile << lfo2_dco1_pitch->value() << endl;
    myfile << lfo2_dco2_pitch->value() << endl;
    myfile << lfo2_dco3_pitch->value() << endl;
    myfile << lfo2_dcf->value() << endl;
    myfile << wave5->value() << endl;
    myfile << lfo2speed->value() << endl;
    myfile << endl;

    // SAVE AMP ADSR

    myfile << "ADSR ONE" << endl;
    myfile << attack1->value() << endl;
    myfile << decay1->value() << endl;
    myfile << sustain1->value() << endl;
    myfile << release1->value() << endl;
    myfile << adsr_amp_amount1->value() << endl;
    myfile << adsr_osc2_amount1->value() << endl;

    myfile << endl;

    // SAVE FILTER ADSR

    myfile << "ADSR TWO" << endl;
    myfile << attack2->value() << endl;
    myfile << decay2->value() << endl;
    myfile << sustain2->value() << endl;
    myfile << release2->value() << endl;
    myfile << adsr_filter_amount2->value() << endl;
    myfile << adsr_osc3_amount2->value() << endl;
    myfile << endl;

    myfile.close();
    return 0;
}

//---------------------------------------------------------------------------------

int load_patch(string fname)
{

    ifstream myfile;
    stringstream strm;
    // strm << str("");
    strm << "patches/" << fname;
    string str;
    myfile.open(strm.str().c_str());
    getline(myfile, str);
    float value = 0;
    if (str == "minaton1") {

        //----------------------------

        getline(myfile, str);

        if (str == "FLAGS") {
            getline(myfile, str); // legato
            istringstream(str) >> value;
            set_legato(value);
            legato_button->value(value);

            getline(myfile, str); // sync
            istringstream(str) >> value;
            set_sync_mode(value);
            sync_button->value(value);

            getline(myfile, str); // volume
            istringstream(str) >> value;
            master_volume->value(value);

            getline(myfile, str); // midi channel
            istringstream(str) >> value;
            midi_channel->value(value);
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);
        if (str == "FILTER") {
            getline(myfile, str); // filter frequency
            istringstream(str) >> value;
            freq->value(value);

            set_dcf_frequency(freq->value());

            getline(myfile, str); // filter resonance
            istringstream(str) >> value;
            res->value(value);

            set_dcf_resonance(res->value());
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "OSCILLATOR ONE") {

            getline(myfile, str); // oscillator active or not
            istringstream(str) >> value;
            active1->value(value);

            if (value)
                dco_on(0);
            else
                dco_off(0);

            getline(myfile, str); // octave
            istringstream(str) >> value;
            octave1->value(value);

            getline(myfile, str); // pitch
            istringstream(str) >> value;
            pitch1->value(value);

            getline(myfile, str); // wave
            istringstream(str) >> value;
            wave1->value(value);

            set_dco_wave(0, wave1->value());
            set_dco_frequency(0, pow(1.059463, 12 * octave1->value() + (pitch1->value())));

            dco1_panel->hide();
            dco1_panel->show();
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "OSCILLATOR TWO") {
            getline(myfile, str); // oscillator active or not
            istringstream(str) >> value;
            active2->value(value);

            if (value)
                dco_on(1);
            else
                dco_off(1);

            getline(myfile, str); // octave
            istringstream(str) >> value;
            octave2->value(value);

            getline(myfile, str); // pitch
            istringstream(str) >> value;
            pitch2->value(value);

            getline(myfile, str); // wave
            istringstream(str) >> value;
            wave2->value(value);

            set_dco_wave(1, wave2->value());
            set_dco_frequency(1, pow(1.059463, 12 * octave2->value() + (pitch2->value())));

            dco2_panel->hide();
            dco2_panel->show();
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "OSCILLATOR THREE") {
            getline(myfile, str); // oscillator active or not
            istringstream(str) >> value;
            active3->value(value);

            if (value)
                dco_on(2);
            else
                dco_off(2);

            getline(myfile, str); // octave
            istringstream(str) >> value;
            octave3->value(value);

            getline(myfile, str); // pitch
            istringstream(str) >> value;
            pitch3->value(value);

            getline(myfile, str); // wave
            istringstream(str) >> value;
            wave3->value(value);

            set_dco_wave(2, wave3->value());
            set_dco_frequency(2, pow(1.059463, 12 * octave3->value() + (pitch3->value())));

            dco3_panel->hide();
            dco3_panel->show();
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "LFO ONE") {
            getline(myfile, str);
            istringstream(str) >> value;
            lfo1_dco1_pitch->value(value);

            set_dco_lfo1_amount(0, lfo1_dco1_pitch->value());

            getline(myfile, str);
            istringstream(str) >> value;
            lfo1_dco2_pitch->value(value);

            set_dco_lfo1_amount(1, lfo1_dco2_pitch->value());

            getline(myfile, str);
            istringstream(str) >> value;
            lfo1_dco3_pitch->value(value);

            set_dco_lfo1_amount(2, lfo1_dco3_pitch->value());

            getline(myfile, str);
            istringstream(str) >> value;
            lfo1_dcf->value(value);

            set_lfo1_dcf_amount(lfo1_dcf->value());

            getline(myfile, str);
            istringstream(str) >> value;
            wave4->value(value);

            set_dco_wave(3, wave4->value());

            getline(myfile, str);
            istringstream(str) >> value;
            lfo1speed->value(value);

            set_dco_frequency(3, lfo1speed->value());

            lfo1_panel->hide();
            lfo1_panel->show();
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "LFO TWO") {
            getline(myfile, str);
            istringstream(str) >> value;
            lfo2_dco1_pitch->value(value);

            set_dco_lfo2_amount(0, lfo2_dco1_pitch->value());

            getline(myfile, str);
            istringstream(str) >> value;
            lfo2_dco2_pitch->value(value);

            set_dco_lfo2_amount(1, lfo2_dco2_pitch->value());

            getline(myfile, str);
            istringstream(str) >> value;
            lfo2_dco3_pitch->value(value);

            set_dco_lfo2_amount(2, lfo2_dco3_pitch->value());

            getline(myfile, str);
            istringstream(str) >> value;
            lfo2_dcf->value(value);

            set_lfo2_dcf_amount(lfo2_dcf->value());

            getline(myfile, str);
            istringstream(str) >> value;
            wave5->value(value);

            set_dco_wave(4, wave5->value());

            getline(myfile, str);
            istringstream(str) >> value;
            lfo2speed->value(value);

            set_dco_frequency(4, lfo2speed->value());

            lfo2_panel->hide();
            lfo2_panel->show();
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "ADSR ONE") {
            getline(myfile, str);
            istringstream(str) >> value;
            attack1->value(value);

            set_envelope1_attack(attack1->value());

            getline(myfile, str);
            istringstream(str) >> value;
            decay1->value(value);

            set_envelope1_decay(decay1->value());

            getline(myfile, str);
            istringstream(str) >> value;
            sustain1->value(value);

            set_envelope1_sustain(sustain1->value());

            getline(myfile, str);
            istringstream(str) >> value;
            release1->value(value);

            set_envelope1_release(release1->value());

            getline(myfile, str);
            istringstream(str) >> value;
            adsr_amp_amount1->value(value);

            getline(myfile, str);
            istringstream(str) >> value;
            adsr_osc2_amount1->value(value);
        }

        //-----------------------------

        getline(myfile, str);
        getline(myfile, str);

        if (str == "ADSR TWO") {
            getline(myfile, str);
            istringstream(str) >> value;
            attack2->value(value);

            set_envelope2_attack(attack2->value());

            getline(myfile, str);
            istringstream(str) >> value;
            decay2->value(value);

            set_envelope2_decay(decay2->value());

            getline(myfile, str);
            istringstream(str) >> value;
            sustain2->value(value);

            set_envelope2_sustain(sustain2->value());

            getline(myfile, str);
            istringstream(str) >> value;
            release2->value(value);

            set_envelope2_release(release2->value());

            getline(myfile, str);
            istringstream(str) >> value;
            adsr_filter_amount2->value(value);

            getline(myfile, str);
            istringstream(str) >> value;
            adsr_osc3_amount2->value(value);
        }
    }
    //--------------------------------

    myfile.close();

    return 0;
}
