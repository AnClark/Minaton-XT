#include "MinatonUI.h"
#include "MinatonArtwork.hpp"

START_NAMESPACE_DISTRHO

namespace Art = MinatonArtwork;

MinatonUI::MinatonUI()
    : UI(Art::backWidth, Art::backHeight)
    , fImgBackground(Art::backData, Art::backWidth, Art::backHeight, kImageFormatBGRA)
    , fKnobImage(Art::newknobData, Art::newknobWidth, Art::newknobHeight, kImageFormatBGRA)
    , fVolumeKnobImage(Art::volknobData, Art::volknobWidth, Art::volknobWidth, kImageFormatBGRA)
    , fSliderImage(Art::sliderData, Art::sliderWidth, Art::sliderHeight, kImageFormatBGRA)
    , fSwitchButtonImage_ON(Art::switch_onData, Art::switch_onWidth, Art::switch_onHeight, kImageFormatBGRA)
    , fSwitchButtonImage_OFF(Art::switch_offData, Art::switch_offWidth, Art::switch_offHeight, kImageFormatBGRA)
    , fSliderBackgroundDco(Art::slider_background_dcoData, Art::slider_background_dcoWidth, Art::slider_background_dcoHeight, kImageFormatBGRA)
    , fSliderBackgroundEnv(Art::slider_background_envData, Art::slider_background_envWidth, Art::slider_background_envHeight, kImageFormatBGRA)
    , fSliderBackgroundLfo(Art::slider_background_lfoData, Art::slider_background_lfoWidth, Art::slider_background_lfoHeight, kImageFormatBGRA)
{
    // Knob initial angles
    constexpr int KNOB_ANGLE = 287;
    constexpr int KNOB_VOLUME_ANGLE = 305;

    // Master output
    _createKnob(fMasterVolume, PARAM_MASTER_VOLUME, 520, 442, KNOB_VOLUME_ANGLE);
    _createSwitchButton(fSync, PARAM_SYNC, 375, 473);
    _createSwitchButton(fLegato, PARAM_LEGATO, 375, 519);

    // DCO activator
    _createSwitchButton(fDco1Active, PARAM_ACTIVE_ONE, 54, 40);
    _createSwitchButton(fDco2Active, PARAM_ACTIVE_TWO, 54, 150);
    _createSwitchButton(fDco3Active, PARAM_ACTIVE_THREE, 54, 260);

    // DCO waveform
    _createSlider(fDco1Wave, PARAM_WAVE_ONE, 100, 103, 335, 103, 1.0f);
    _createSlider(fDco2Wave, PARAM_WAVE_TWO, 100, 215, 335, 215, 1.0f);
    _createSlider(fDco3Wave, PARAM_WAVE_THREE, 100, 325, 335, 325, 1.0f);

    // LFO waveform
    _createSlider(fLfo1Waveform, PARAM_LFO1_WAVE, 490, 59, 567, 59, 1.0f);
    _createSlider(fLfo2Waveform, PARAM_LFO2_WAVE, 490, 259, 567, 259, 1.0f);

    // DCO finetune
    _createSlider(fFinetune1, PARAM_FINETUNE_ONE, 100, 55, 335, 55, 0.1f);
    _createSlider(fFinetune2, PARAM_FINETUNE_TWO, 100, 172, 335, 172, 0.1f);
    _createSlider(fFinetune3, PARAM_FINETUNE_THREE, 100, 280, 335, 280, 0.1f);

    // DCA1 envelope
    _createSlider(fDca1Attack, PARAM_ATTACK_ONE, 101, 385, 101, 420, 1.0f, true);
    _createSlider(fDca1Decay, PARAM_DECAY_ONE, 141, 385, 141, 420, 1.0f, true);
    _createSlider(fDca1Sustain, PARAM_SUSTAIN_ONE, 181, 385, 181, 420, 0.01f, true);
    _createSlider(fDca1Release, PARAM_RELEASE_ONE, 221, 385, 221, 420, 1.0f, true);

    // DCA1 route
    _createSlider(fDca1RoutedToAmp, PARAM_ADSR1_AMP_AMOUNT, 280, 385, 280, 420, 0.1f);
    _createSlider(fDca1RoutedToOsc2Pitch, PARAM_ADSR1_OSC2_AMOUNT, 321, 385, 321, 420, 0.1f);

    // DCA2 envelope
    _createSlider(fDca2Attack, PARAM_ATTACK_TWO, 101, 480, 101, 515, 1.0f, true);
    _createSlider(fDca2Decay, PARAM_DECAY_TWO, 141, 480, 141, 515, 1.0f, true);
    _createSlider(fDca2Sustain, PARAM_SUSTAIN_TWO, 181, 480, 181, 515, 0.01f, true);
    _createSlider(fDca2Release, PARAM_RELEASE_TWO, 221, 480, 221, 515, 1.0f, true);

    // DCA2 route
    _createSlider(fDca2RoutedToDcf, PARAM_ADSR2_DCF_AMOUNT, 280, 480, 280, 515, 0.1f);
    _createSlider(fDca2RoutedToOsc3Pitch, PARAM_ADSR2_OSC3_AMOUNT, 321, 480, 321, 515, 0.1f);

    // LFO speed
    _createKnob(fLfo1Speed, PARAM_LFO1_SPEED, 440, 47, KNOB_ANGLE);
    _createKnob(fLfo2Speed, PARAM_LFO2_SPEED, 440, 250, KNOB_ANGLE);

    // LFO1 route
    _createSlider(fLfo1RouteToDco1Pitch, PARAM_LFO1_DCO1_PITCH, 458, 119, 458, 159, 1.0f, true);
    _createSlider(fLfo1RouteToDco2Pitch, PARAM_LFO1_DCO2_PITCH, 496, 119, 496, 159, 1.0f, true);
    _createSlider(fLfo1RouteToDco3Pitch, PARAM_LFO1_DCO3_PITCH, 534, 119, 534, 159, 1.0f, true);
    _createSlider(fLfo1RouteToDcf, PARAM_LFO1_DCF, 584, 119, 584, 159, 0.1f, true);

    // LFO2 route
    _createSlider(fLfo2RouteToDco1Pitch, PARAM_LFO2_DCO1_PITCH, 458, 320, 458, 360, 1.0f, true);
    _createSlider(fLfo2RouteToDco2Pitch, PARAM_LFO2_DCO2_PITCH, 496, 320, 496, 360, 1.0f, true);
    _createSlider(fLfo2RouteToDco3Pitch, PARAM_LFO2_DCO3_PITCH, 534, 320, 534, 360, 1.0f, true);
    _createSlider(fLfo2RouteToDcf, PARAM_LFO2_DCF, 584, 320, 584, 360, 0.1f, true);

    // DCO octave
    _createKnob(fDco1Octave, PARAM_OCTAVE_ONE, 54, 88, KNOB_ANGLE);
    _createKnob(fDco2Octave, PARAM_OCTAVE_TWO, 54, 197, KNOB_ANGLE);
    _createKnob(fDco3Octave, PARAM_OCTAVE_THREE, 54, 306, KNOB_ANGLE);

    // Inertia
    _createKnob(fInertia1, PARAM_INERTIA_ONE, 377, 88, KNOB_ANGLE);
    _createKnob(fInertia2, PARAM_INERTIA_TWO, 377, 197, KNOB_ANGLE);
    _createKnob(fInertia3, PARAM_INERTIA_THREE, 377, 306, KNOB_ANGLE);

    // Equalizer params
    _createKnob(fCutOffFrequency, PARAM_FREQUENCY, 377, 368, KNOB_ANGLE);
    _createKnob(fResonance, PARAM_RESONANCE, 377, 422, KNOB_ANGLE);
}

void MinatonUI::parameterChanged(uint32_t index, float value)
{
    fParameter[index] = value;

    switch (index) {
    // OSC activation
    case PARAM_ACTIVE_ONE:
        fDco1Active->setDown(value > 0.0f ? true : false);
        break;
    case PARAM_ACTIVE_TWO:
        fDco2Active->setDown(value > 0.0f ? true : false);
        break;
    case PARAM_ACTIVE_THREE:
        fDco3Active->setDown(value > 0.0f ? true : false);
        break;

    // Master output
    case PARAM_MASTER_VOLUME:
        fMasterVolume->setValue(value);
        break;
    case PARAM_SYNC:
        fSync->setDown(value > 0.0f ? true : false);
        break;
    case PARAM_LEGATO:
        fLegato->setDown(value > 0.0f ? true : false);
        break;

    // DCO waveform
    case PARAM_WAVE_ONE:
        fDco1Wave->setValue(value);
        break;
    case PARAM_WAVE_TWO:
        fDco2Wave->setValue(value);
        break;
    case PARAM_WAVE_THREE:
        fDco3Wave->setValue(value);
        break;

    // DCO finetune
    case PARAM_FINETUNE_ONE:
        fFinetune1->setValue(value);
        break;
    case PARAM_FINETUNE_TWO:
        fFinetune2->setValue(value);
        break;
    case PARAM_FINETUNE_THREE:
        fFinetune3->setValue(value);
        break;

    // DCO Octaves
    case PARAM_OCTAVE_ONE:
        fDco1Octave->setValue(value);
        break;
    case PARAM_OCTAVE_TWO:
        fDco2Octave->setValue(value);
        break;
    case PARAM_OCTAVE_THREE:
        fDco3Octave->setValue(value);
        break;

    // Inertia
    case PARAM_INERTIA_ONE:
        fInertia1->setValue(value);
        break;
    case PARAM_INERTIA_TWO:
        fInertia2->setValue(value);
        break;
    case PARAM_INERTIA_THREE:
        fInertia3->setValue(value);
        break;

    // DCA1 envelope
    case PARAM_ATTACK_ONE:
        fDca1Attack->setValue(value);
        break;
    case PARAM_DECAY_ONE:
        fDca1Decay->setValue(value);
        break;
    case PARAM_SUSTAIN_ONE:
        fDca1Sustain->setValue(value);
        break;
    case PARAM_RELEASE_ONE:
        fDca1Release->setValue(value);
        break;

    // DCA1 route
    case PARAM_ADSR1_AMP_AMOUNT:
        fDca1RoutedToAmp->setValue(value);
        break;
    case PARAM_ADSR1_OSC2_AMOUNT:
        fDca1RoutedToOsc2Pitch->setValue(value);
        break;

    // DCA2 envelope
    case PARAM_ATTACK_TWO:
        fDca2Attack->setValue(value);
        break;
    case PARAM_DECAY_TWO:
        fDca2Decay->setValue(value);
        break;
    case PARAM_SUSTAIN_TWO:
        fDca2Sustain->setValue(value);
        break;
    case PARAM_RELEASE_TWO:
        fDca2Release->setValue(value);
        break;

    // DCA2 route
    case PARAM_ADSR2_DCF_AMOUNT:
        fDca2RoutedToDcf->setValue(value);
        break;
    case PARAM_ADSR2_OSC3_AMOUNT:
        fDca2RoutedToOsc3Pitch->setValue(value);
        break;

    // LFO speed
    case PARAM_LFO1_SPEED:
        fLfo1Speed->setValue(value);
        break;
    case PARAM_LFO2_SPEED:
        fLfo2Speed->setValue(value);
        break;

    // LFO waveform
    case PARAM_LFO1_WAVE:
        fLfo1Waveform->setValue(value);
        break;
    case PARAM_LFO2_WAVE:
        fLfo2Waveform->setValue(value);
        break;

    // LFO1 route
    case PARAM_LFO1_DCO1_PITCH:
        fLfo1RouteToDco1Pitch->setValue(value);
        break;
    case PARAM_LFO1_DCO2_PITCH:
        fLfo1RouteToDco2Pitch->setValue(value);
        break;
    case PARAM_LFO1_DCO3_PITCH:
        fLfo1RouteToDco3Pitch->setValue(value);
        break;
    case PARAM_LFO1_DCF:
        fLfo1RouteToDcf->setValue(value);
        break;

    // LFO2 route
    case PARAM_LFO2_DCO1_PITCH:
        fLfo2RouteToDco1Pitch->setValue(value);
        break;
    case PARAM_LFO2_DCO2_PITCH:
        fLfo2RouteToDco2Pitch->setValue(value);
        break;
    case PARAM_LFO2_DCO3_PITCH:
        fLfo2RouteToDco3Pitch->setValue(value);
        break;
    case PARAM_LFO2_DCF:
        fLfo2RouteToDcf->setValue(value);
        break;

    // Equalizer params
    case PARAM_FREQUENCY:
        fCutOffFrequency->setValue(value);
        break;
    case PARAM_RESONANCE:
        fResonance->setValue(value);
        break;
    }
}

// -------------------------------------------------------------------
// Widget Callbacks

void MinatonUI::imageButtonClicked(ImageButton* button, int)
{
}

void MinatonUI::imageSwitchClicked(ImageSwitch* button, bool down)
{
    setParameterValue(button->getId(), down);
}

void MinatonUI::imageKnobDragStarted(ImageKnob* knob)
{
    editParameter(knob->getId(), true);
}

void MinatonUI::imageKnobDragFinished(ImageKnob* knob)
{
    editParameter(knob->getId(), false);
}

void MinatonUI::imageKnobValueChanged(ImageKnob* knob, float value)
{
    setParameterValue(knob->getId(), value);
}

void MinatonUI::imageSliderDragStarted(ImageSlider* slider)
{
    editParameter(slider->getId(), true);
}

void MinatonUI::imageSliderDragFinished(ImageSlider* slider)
{
    editParameter(slider->getId(), false);
}

void MinatonUI::imageSliderValueChanged(ImageSlider* slider, float value)
{
    setParameterValue(slider->getId(), value);
}

void MinatonUI::onDisplay()
{
    const GraphicsContext& context(getGraphicsContext());

    fImgBackground.draw(context);

    // DCO long vertical sliders
    fSliderBackgroundDco.drawAt(context, 100, 60); // DCO1 Finetune
    fSliderBackgroundDco.drawAt(context, 100, 107); // DCO1 Wave
    fSliderBackgroundDco.drawAt(context, 100, 177); // DCO2 Finetune
    fSliderBackgroundDco.drawAt(context, 100, 221); // DCO2 Wave
    fSliderBackgroundDco.drawAt(context, 100, 285); // DCO3 Finetune
    fSliderBackgroundDco.drawAt(context, 100, 329); // DCO3 Wave

    fSliderBackgroundEnv.drawAt(context, 117, 394);
    fSliderBackgroundEnv.drawAt(context, 157, 394);
    fSliderBackgroundEnv.drawAt(context, 197, 394);
    fSliderBackgroundEnv.drawAt(context, 237, 394);
    fSliderBackgroundEnv.drawAt(context, 237 + 60, 394);
    fSliderBackgroundEnv.drawAt(context, 237 + 100, 394);

    fSliderBackgroundEnv.drawAt(context, 117, 491);
    fSliderBackgroundEnv.drawAt(context, 157, 491);
    fSliderBackgroundEnv.drawAt(context, 197, 491);
    fSliderBackgroundEnv.drawAt(context, 237, 491);
    fSliderBackgroundEnv.drawAt(context, 237 + 60, 491);
    fSliderBackgroundEnv.drawAt(context, 237 + 100, 491);

    fSliderBackgroundEnv.drawAt(context, 474, 132);
    fSliderBackgroundEnv.drawAt(context, 512, 132);
    fSliderBackgroundEnv.drawAt(context, 550, 132);
    fSliderBackgroundEnv.drawAt(context, 600, 132);

    fSliderBackgroundEnv.drawAt(context, 474, 335);
    fSliderBackgroundEnv.drawAt(context, 512, 335);
    fSliderBackgroundEnv.drawAt(context, 550, 335);
    fSliderBackgroundEnv.drawAt(context, 600, 335);

    // LFO waveform sliders
    fSliderBackgroundLfo.drawAt(context, 494, 50); // LFO1
    fSliderBackgroundLfo.drawAt(context, 494, 250); // LFO2
}

void MinatonUI::idleCallback() { }

// -----------------------------------------------------------------------

UI* createUI()
{
    return new MinatonUI();
}

END_NAMESPACE_DISTRHO
