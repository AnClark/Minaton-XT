#pragma once

#include "DistrhoUI.hpp"
#include "ImageWidgets.hpp"

#include "MinatonParams.h"

using DGL_NAMESPACE::ImageAboutWindow;
using DGL_NAMESPACE::ImageButton;
using DGL_NAMESPACE::ImageKnob;
using DGL_NAMESPACE::ImageSlider;
using DGL_NAMESPACE::ImageSwitch;

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class MinatonUI : public UI,
                  public ImageButton::Callback,
                  public ImageKnob::Callback,
                  public ImageSlider::Callback,
                  public ImageSwitch::Callback,
                  public IdleCallback {
public:
    MinatonUI();

protected:
    // -------------------------------------------------------------------
    // DSP Callbacks

    void parameterChanged(uint32_t index, float value) override;

    // -------------------------------------------------------------------
    // Widget Callbacks

    void imageButtonClicked(ImageButton* button, int) override;
    void imageSwitchClicked(ImageSwitch* button, bool) override;
    void imageKnobDragStarted(ImageKnob* knob) override;
    void imageKnobDragFinished(ImageKnob* knob) override;
    void imageKnobValueChanged(ImageKnob* knob, float value) override;
    void imageSliderDragStarted(ImageSlider* slider) override;
    void imageSliderDragFinished(ImageSlider* slider) override;
    void imageSliderValueChanged(ImageSlider* slider, float value) override;

    void onDisplay() override;

    // -------------------------------------------------------------------
    // Other Callbacks

    void idleCallback() override;

private:
    // -------------------------------------------------------------------
    // Parameter storage
    float fParameter[MinatonParamId::PARAM_COUNT];

    // -------------------------------------------------------------------
    // Image resources

    Image fImgBackground;
    Image fKnobImage, fVolumeKnobImage;
    Image fSliderImage, fSliderBackgroundDco, fSliderBackgroundEnv, fSliderBackgroundLfo;
    Image fSwitchButtonImage_ON, fSwitchButtonImage_OFF;
    Image fSwitchNoLightImage_ON, fSwitchNoLightImage_OFF;
    Image fImgLabelMixMode, fImgLabelMonoStereo;
    //  ImageAboutWindow fAboutWindow;

    // -------------------------------------------------------------------
    // Widgets

    ScopedPointer<ImageSwitch> fDco1Active, fDco2Active, fDco3Active;
    ScopedPointer<ImageSlider> fDco1Wave, fDco2Wave, fDco3Wave;
    ScopedPointer<ImageKnob> fDco1Octave, fDco2Octave, fDco3Octave;

    ScopedPointer<ImageSlider> fFinetune1, fFinetune2, fFinetune3;
    ScopedPointer<ImageButton> fFinetune1Center, fFinetune2Center, fFinetune3Center;
    ScopedPointer<ImageKnob> fInertia1, fInertia2, fInertia3;

    ScopedPointer<ImageSwitch> fLegato, fSync, fOutputMode;

    ScopedPointer<ImageKnob> fCutOffFrequency, fResonance;

    ScopedPointer<ImageSlider> fDca1Attack, fDca1Decay, fDca1Sustain, fDca1Release;
    ScopedPointer<ImageSlider> fDca1RoutedToAmp, fDca1RoutedToOsc2Pitch;

    ScopedPointer<ImageSlider> fDca2Attack, fDca2Decay, fDca2Sustain, fDca2Release;
    ScopedPointer<ImageSlider> fDca2RoutedToDcf, fDca2RoutedToOsc3Pitch;

    ScopedPointer<ImageKnob> fLfo1Speed, fLfo2Speed;
    ScopedPointer<ImageSlider> fLfo1Waveform, fLfo2Waveform;
    ScopedPointer<ImageSlider> fLfo1RouteToDco1Pitch, fLfo1RouteToDco2Pitch, fLfo1RouteToDco3Pitch, fLfo1RouteToDcf;
    ScopedPointer<ImageSlider> fLfo2RouteToDco1Pitch, fLfo2RouteToDco2Pitch, fLfo2RouteToDco3Pitch, fLfo2RouteToDcf;

    ScopedPointer<ImageKnob> fMasterVolume;

    // -------------------------------------------------------------------
    // Helpers

    void _createKnob(ScopedPointer<ImageKnob>& knob, MinatonParamId paramId, uint absolutePosX, uint absolutePosY, uint rotationAngle);
    void _createSlider(ScopedPointer<ImageSlider>& slider, MinatonParamId paramId, uint startPosX, uint startPosY, uint endPosX, uint endPosY, float step, bool inverted = false);
    void _createSwitchButton(ScopedPointer<ImageSwitch>& switchButton, MinatonParamId paramId, uint absolutePosX, uint absolutePosY, bool withLight = true);

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MinatonUI)
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
