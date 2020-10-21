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
    
    setSize (700, 300);
     
    // OUTPUT
    // these define the parameters of our slider object
    outputSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    outputSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    outputSlider.setRange (-50.0f, 0.0f, 0.5f);
    outputSlider.setValue(0.0f);
    // this = this class
    outputSlider.addListener(this);
    // slider customization
    outputSlider.getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::red);
    outputSlider.getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred);
     
    // this function adds the slider to the editor
    addAndMakeVisible (&outputSlider);
    
    outputValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, OUTPUT_ID, outputSlider);
    
    
    // Drive
    driveSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    driveSlider.setRange (1.f, 25.0f, 0.05f);
    driveSlider.setValue(1.f);
    driveSlider.addListener(this);

    driveSlider.getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::red);
    driveSlider.getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred);
     
    addAndMakeVisible (&driveSlider);
    
    driveValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, DRIVE_ID, driveSlider);
    
    
    // mix
    mixSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    mixSlider.setRange (0.0f, 1.0f, 0.001f);
    mixSlider.setValue(1.0f);
    mixSlider.addListener(this);

    mixSlider.getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::red);
    mixSlider.getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred);
     
    addAndMakeVisible (&mixSlider);
    
    mixValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, MIX_ID, mixSlider);
    
    // input
    inputSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    inputSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    inputSlider.setRange (-30.0f, 30.0f, 0.5f);
    inputSlider.setValue(0.0f);
    inputSlider.addListener(this);

    inputSlider.getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::red);
    inputSlider.getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred);
     
    addAndMakeVisible (&inputSlider);
    
    inputValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, INPUT_ID, inputSlider);
    
    // cutoff
    cutoffSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    cutoffSlider.setRange (100.0f, 20000.0f, 100.f);
    cutoffSlider.setValue(20000.0f);
    cutoffSlider.addListener(this);

    cutoffSlider.getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::red);
    cutoffSlider.getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkred);
     
    addAndMakeVisible (&cutoffSlider);
    
    cutoffValue = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, CUTOFF_ID, cutoffSlider);
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
     
    g.drawFittedText ("Venom Distortion", 0, 40, getWidth(), 30, juce::Justification::centred, 1);
    
    g.drawFittedText ("Input", -250, 100, getWidth(), 30, juce::Justification::centred, 1);
    g.drawFittedText ("Drive", -125, 100, getWidth(), 30, juce::Justification::centred, 1);
    g.drawFittedText ("Filter", 0, 100, getWidth(), 30, juce::Justification::centred, 1);
    g.drawFittedText ("Mix", 125, 100, getWidth(), 30, juce::Justification::centred, 1);
    g.drawFittedText ("Output", 250, 100, getWidth(), 30, juce::Justification::centred, 1);
}

void VenomDistortionAudioProcessorEditor::resized()
{
    
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    // sets the position and size of the slider with arguments (x, y, width, height)
    //outputVolume.setBounds (40, 30, 20, getHeight() - 60);
    
    inputSlider.setBounds(30, getHeight()/4+60, 130, 130);
    driveSlider.setBounds(160, getHeight()/4+60, 130, 130);
    cutoffSlider.setBounds(285, getHeight()/4+60, 130, 130);
    mixSlider.setBounds(410, getHeight()/4+60, 130, 130);
    outputSlider.setBounds(535, getHeight()/4+60, 130, 130);
}

void VenomDistortionAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
//    if (slider == &outputSlider)
//    {
//        audioProcessor.output = outputSlider.getValue();
//    }
//    else if (slider == &driveSlider)
//    {
//        audioProcessor.drive = driveSlider.getValue();
//    }
//    else if (slider == &mixSlider)
//    {
//        audioProcessor.mix = mixSlider.getValue();
//    }
//    else if (slider ==&inputSlider)
//    {
//        audioProcessor.input = inputSlider.getValue();
//    }
}

