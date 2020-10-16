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
    
    setSize (600, 200);
     
    // OUTPUT
    // these define the parameters of our slider object
    outputVolume.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    outputVolume.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    outputVolume.setRange (-48.0f, 0.0f, 1.0f);
    outputVolume.setValue(0.0f);
    // this = this class
    outputVolume.addListener(this);
    // slider customization
    outputVolume.getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::red);
    outputVolume.getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred);
     
    // this function adds the slider to the editor
    addAndMakeVisible (&outputVolume);
    
    
    // DISTORTION
    distortionDrive.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    distortionDrive.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    distortionDrive.setRange (1.0f, 60.0f, 0.5f);
    distortionDrive.setValue(1.0f);
    distortionDrive.addListener(this);

    distortionDrive.getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::red);
    distortionDrive.getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred);
     
    addAndMakeVisible (&distortionDrive);
    
    
    // mix
    distortionMix.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    distortionMix.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    distortionMix.setRange (0.001f, 1.0f, 0.001f);
    distortionMix.setValue(1.0f);
    distortionMix.addListener(this);

    distortionMix.getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::red);
    distortionMix.getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred);
     
    addAndMakeVisible (&distortionMix);
}

VenomDistortionAudioProcessorEditor::~VenomDistortionAudioProcessorEditor()
{
    
}

//==============================================================================
void VenomDistortionAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    
    // fill the whole window white
    g.fillAll (juce::Colours::black);
     
    // set the current drawing colour to black
    g.setColour (juce::Colours::white);
     
    // set the font size and draw text to the screen
    g.setFont (20.0f);
     
    g.drawFittedText ("Venom Distortion v2", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void VenomDistortionAudioProcessorEditor::resized()
{
    
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // sets the position and size of the slider with arguments (x, y, width, height)
    //outputVolume.setBounds (40, 30, 20, getHeight() - 60);
    
    distortionDrive.setBounds(getWidth()/6, getHeight()/4, 130, 130);
    distortionMix.setBounds((getWidth()/2)-25, getHeight()/4, 130, 130);
    outputVolume.setBounds((getWidth()/2)+100, getHeight()/4, 130, 130);
}

void VenomDistortionAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &outputVolume)
    {
        audioProcessor.output = outputVolume.getValue();
    }
    else if (slider == &distortionDrive)
    {
        audioProcessor.drive = distortionDrive.getValue();
    }
    else if (slider == &distortionMix)
    {
        audioProcessor.mix = distortionMix.getValue();
    }
}

