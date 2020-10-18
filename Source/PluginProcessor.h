/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define OUTPUT_ID "output"
#define OUTPUT_NAME "Output"

#define INPUT_ID "input"
#define INPUT_NAME "Input"

#define DRIVE_ID "drive"
#define DRIVE_NAME "Drive"

#define MIX_ID "mix"
#define MIX_NAME "Mix"

#define CUTOFF_ID "cutoff"
#define CUTOFF_NAME "Cutoff"

//==============================================================================
/**
*/
class VenomDistortionAudioProcessor  : public juce::AudioProcessor
{
public:
    
    
    //==============================================================================
    VenomDistortionAudioProcessor();
    ~VenomDistortionAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
//    float input {0.0f};
//    
//    float output {0.0f};
//    
//    float drive {1.f};
//    
//    float mix {0.0f};
    
    void updateFilter();
    
    juce::AudioProcessorValueTreeState treeState;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    
    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> lowPassFilter;
       
       float lastSampleRate;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VenomDistortionAudioProcessor)
};
