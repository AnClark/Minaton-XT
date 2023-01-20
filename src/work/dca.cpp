
#include "main.h"
#include <iostream>
#include <math.h>
using namespace std;

static int attack = 0;
static int decay = 1;
static int wait = 2;
static int release = 3;

class adsr {
public:
    float attack;
    float decay;
    float sustain;
    float release;

    int state;
    float level;
};

adsr dca1;
adsr envelope1;
adsr envelope2;

int master_legato = 0;

//---------------------------------------------------------
void init_dcas()
{
    envelope1.attack = 0;
    envelope1.decay = 0;
    envelope1.sustain = 0;
    envelope1.release = 0;
    envelope1.state = wait;
    envelope1.level = 0;

    envelope2.attack = 0;
    envelope2.decay = 0;
    envelope2.sustain = 0;
    envelope2.release = 0;
    envelope2.state = wait;
    envelope2.level = 0;
}

//---------------------------------------------------------

float envelope1_out(float input, float amount)
{
    input = input * (envelope1.level / amount);
    return input;
}

//---------------------------------------------------------

float envelope2_out(float input, float amount)
{
    input = input * (envelope2.level / amount);
    return input;
}

//--------------------------------------------------------

void dca_update()
{

    //-----------------------------------------------
    // PROCESS ENVELOPE 1

    if (envelope1.state == attack) {
        if (envelope1.level < 1) {
            if (attack1 == 0) {
                envelope1.level = decay;
            }
            envelope1.level += pow(0.8, attack1);
        } else {
            envelope1.state = decay;
        }
    }

    if (envelope1.state == decay) {
        if (envelope1.level > sustain1) {
            envelope1.level -= pow(0.8, decay1);
        } else {
            envelope1.state = wait;
            envelope1.level = envelope1.sustain;
        }
    }

    if (envelope1.state == release) {
        if (envelope1.level > 0) {
            envelope1.level -= pow(0.8, release1);
        } else {
            envelope1.state = wait;
            envelope1.level = 0;
        }
    }

    //-----------------------------------------------
    // PROCESS ENVELOPE 2

    if (envelope2.state == attack) {
        if (envelope2.level < 1) {
            if (attack2 == 0) {
                envelope2.level = 1;
            }
            envelope2.level += pow(0.8, attack2);
        } else {
            envelope2.state = decay;
        }
    }

    if (envelope2.state == decay) {
        if (envelope2.level > sustain2) {
            envelope2.level -= pow(0.8, decay2);
        } else {
            envelope2.state = wait;
            envelope2.level = envelope2.sustain;
        }
    }

    if (envelope2.state == release) {
        if (envelope2.level > 0) {
            envelope2.level -= pow(0.8, release2);
        } else {
            envelope2.state = wait;
            envelope2.level = 0;
        }
    }
}

//---------------------------------------------------------
// SET ENVELOPE 1 - attack

void set_envelope1_attack(float value)
{
    envelope1.attack = value;
}

//--------------------------------------------------------
// SET ENVELOPE 1 - decay

void set_envelope1_decay(float value)
{
    envelope1.decay = value;
}

//---------------------------------------------------------
// SET ENVELOPE 1 - sustain

void set_envelope1_sustain(float value)
{
    envelope1.sustain = value;
}

//--------------------------------------------------------
// SET ENVELOPE 1 - release

void set_envelope1_release(float value)
{
    envelope1.release = value;
}

//--------------------------------------------------------
// TRIGGER ADSR 1 - RESET COUNTER

void trigger_envelope1()
{
    envelope1.state = 0;
    // envelope1.level = 0; // meant to set the level back to zero to retrigger on legato mode, but causes clicks, need to fade it instead, work for later
}

//--------------------------------------------------------
// RELEASE ADSR 1 - RESET COUNTER

void release_envelope1()
{
    envelope1.state = release;
}

//---------------------------------------------------------
// SET ENVELOPE 2 - attack

void set_envelope2_attack(float value)
{
    envelope2.attack = value;
}

//--------------------------------------------------------
// SET ENVELOPE 2 - decay

void set_envelope2_decay(float value)
{
    envelope2.decay = value;
}

//---------------------------------------------------------
// SET ENVELOPE 2 - sustain

void set_envelope2_sustain(float value)
{
    envelope2.sustain = value;
}

//--------------------------------------------------------
// SET ENVELOPE 2 - release

void set_envelope2_release(float value)
{
    envelope2.release = value;
}

//--------------------------------------------------------
// TRIGGER ADSR 2 - RESET COUNTER

void trigger_envelope2()
{
    envelope2.state = 0;
    envelope2.level = 0;
}

//--------------------------------------------------------
// RELEASE ADSR 2 - RESET COUNTER

void release_envelope2()
{
    envelope2.state = release;
}

//---------------------------------------------------------
// TOGGLE LEGATO MODE

void toggle_legato()
{
    master_legato = 1 - master_legato;
}

//--------------------------------------------------------
// SET LEGATO MODE
void set_legato(int value)
{
    master_legato = value;
}

//--------------------------------------------------------
// RETURN LEGATO MODE
int get_legato()
{
    return master_legato;
}
