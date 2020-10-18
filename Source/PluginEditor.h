/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class VenomDistortionAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                             public juce::Slider::Listener
{
public:
    VenomDistortionAudioProcessorEditor (VenomDistortionAudioProcessor&);
    ~VenomDistortionAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    // override is for overwrtiting already implemented function by JUCE
    void sliderValueChanged (juce::Slider* sliderGain) override;

private:
    
    juce::Slider driveSlider;
    juce::Slider mixSlider;
    juce::Slider outputSlider;
    juce::Slider inputSlider;
    
//    juce::AudioProcessorValueTreeState::SliderAttachment drive;
//    juce::AudioProcessorValueTreeState::SliderAttachment mix;
//    juce::AudioProcessorValueTreeState::SliderAttachment output;
    
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VenomDistortionAudioProcessor& audioProcessor;
    
public:
    std::unique_ptr <juce::AudioProcessorValueTreeState::SliderAttachment> outputValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VenomDistortionAudioProcessorEditor)
};
