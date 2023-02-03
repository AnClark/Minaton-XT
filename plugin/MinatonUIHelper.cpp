#include "MinatonUI.h"

START_NAMESPACE_DISTRHO

void MinatonUI::_createKnob(ScopedPointer<ImageKnob>& knob, MinatonParamId paramId, uint absolutePosX, uint absolutePosY, uint rotationAngle)
{
    Image& knob_image = (paramId == PARAM_MASTER_VOLUME) ? fVolumeKnobImage : fKnobImage;

    knob = new ImageKnob(this, knob_image, ImageKnob::Vertical);
    knob->setId(paramId);
    knob->setAbsolutePos(absolutePosX, absolutePosY);
    knob->setRange(MinatonParams::paramMinValue(paramId), MinatonParams::paramMaxValue(paramId));
    knob->setDefault(MinatonParams::paramDefaultValue(paramId));
    knob->setValue(MinatonParams::paramDefaultValue(paramId));
    knob->setRotationAngle(rotationAngle);
    knob->setCallback(this);
}

void MinatonUI::_createSlider(ScopedPointer<ImageSlider>& slider, MinatonParamId paramId, uint startPosX, uint startPosY, uint endPosX, uint endPosY, float step, bool inverted)
{
    slider = new ImageSlider(this, fSliderImage);
    slider->setId(paramId);
    slider->setStartPos(startPosX, startPosY);
    slider->setEndPos(endPosX, endPosY);
    slider->setRange(MinatonParams::paramMinValue(paramId), MinatonParams::paramMaxValue(paramId));
    slider->setStep(step);
    slider->setValue(MinatonParams::paramDefaultValue(paramId));
    slider->setInverted(inverted);
    slider->setCallback(this);
}

void MinatonUI::_createSwitchButton(ScopedPointer<ImageSwitch>& switchButton, MinatonParamId paramId, uint absolutePosX, uint absolutePosY, bool withLight)
{
    switchButton = new ImageSwitch(this, withLight ? fSwitchButtonImage_OFF : fSwitchNoLightImage_OFF, withLight ? fSwitchButtonImage_ON : fSwitchNoLightImage_ON);
    switchButton->setId(paramId);
    switchButton->setAbsolutePos(absolutePosX, absolutePosY);
    switchButton->setCallback(this);
}

END_NAMESPACE_DISTRHO
