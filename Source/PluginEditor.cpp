/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VenomDistortionAudioProcessorEditor::VenomDistortionAudioProcessorEditor (VenomDistortionAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    setSize (200, 200);
     
        // these define the parameters of our slider object
        outputVolume.setSliderStyle (juce::Slider::LinearBarVertical);
        outputVolume.setRange (0.0, 127.0, 1.0);
        outputVolume.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
        outputVolume.setPopupDisplayEnabled (true, false, this);
        outputVolume.setTextValueSuffix (" Volume");
        outputVolume.setValue(1.0);
     
        // this function adds the slider to the editor
        addAndMakeVisible (outputVolume);
}

VenomDistortionAudioProcessorEditor::~VenomDistortionAudioProcessorEditor()
{
    // sets the position and size of the slider with arguments (x, y, width, height)
    outputVolume.setBounds (40, 30, 20, getHeight() - 60);
}

//==============================================================================
void VenomDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    
    // fill the whole window white
    g.fillAll (juce::Colours::white);
     
    // set the current drawing colour to black
    g.setColour (juce::Colours::black);
     
    // set the font size and draw text to the screen
    g.setFont (15.0f);
     
    g.drawFittedText ("Midi Volume", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void VenomDistortionAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
