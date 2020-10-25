/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VenomDistortionAudioProcessor::VenomDistortionAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
treeState (*this, nullptr, "PARAMETER", createParameterLayout()),
lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000, 0.7)),
highPassFilter(juce::dsp::IIR::Coefficients<float>::makeHighPass(44100, 20, 0.7))
#endif
{
//    juce::NormalisableRange<float> cutoffRange (20.0f, 20000.0f);
//
//    treeState.createAndAddParameter(CUTOFF_ID, CUTOFF_NAME, CUTOFF_ID, cutoffRange, 20000.0f, nullptr, nullptr);
//
//    treeState.createAndAddParameter(LOWCUT_ID, LOWCUT_NAME, LOWCUT_ID, cutoffRange, 20.0f, nullptr, nullptr);
    
}

VenomDistortionAudioProcessor::~VenomDistortionAudioProcessor()
{
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout VenomDistortionAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    auto outputParam = std::make_unique<juce::AudioParameterFloat>(OUTPUT_ID, OUTPUT_NAME, -50.0f, 0.0f, 0.0f);
    params.push_back(std::move(outputParam));
    
    auto inputParam = std::make_unique<juce::AudioParameterFloat>(INPUT_ID, INPUT_NAME, -30.0f, 30.0f, 0.0f);
    params.push_back(std::move(inputParam));
    
    auto mixParam = std::make_unique<juce::AudioParameterFloat>(MIX_ID, MIX_NAME, 0.0f, 1.0f, 1.0f);
    params.push_back(std::move(mixParam));
    
    auto driveParam = std::make_unique<juce::AudioParameterFloat>(DRIVE_ID, DRIVE_NAME, 1.f, 25.0f, 1.f);
    params.push_back(std::move(driveParam));
    
    auto normRange = juce::NormalisableRange<float>(20,20000);
    normRange.setSkewForCentre(1000);
    
    auto cutoffParam = std::make_unique<juce::AudioParameterFloat>(CUTOFF_ID, CUTOFF_NAME, normRange, 20000);
    params.push_back(std::move(cutoffParam));
    
    auto lowCutParam = std::make_unique<juce::AudioParameterFloat>(LOWCUT_ID, LOWCUT_NAME, normRange, 20);
    params.push_back(std::move(lowCutParam));
    

    auto hardclipBoolParam = std::make_unique<juce::AudioParameterBool>("hardclip", "Hardclip", false);
    params.push_back(std::move(hardclipBoolParam));
    

    return { params.begin(), params.end() };
}

const juce::String VenomDistortionAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VenomDistortionAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VenomDistortionAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VenomDistortionAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VenomDistortionAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VenomDistortionAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VenomDistortionAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VenomDistortionAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VenomDistortionAudioProcessor::getProgramName (int index)
{
    return {};
}

void VenomDistortionAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VenomDistortionAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    lastSampleRate = sampleRate;
    
        
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = samplesPerBlock;
        spec.numChannels = getTotalNumOutputChannels();
        
     
        lowPassFilter.prepare(spec);
        lowPassFilter.reset();
    
        highPassFilter.prepare(spec);
        highPassFilter.reset();
}

void VenomDistortionAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VenomDistortionAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void VenomDistortionAudioProcessor::updateFilter()
{
    float lpfreq = *treeState.getRawParameterValue("cutoff");
    float hpfreq = *treeState.getRawParameterValue("lowcut");
    
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, lpfreq, 1.0);
    
    *highPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, hpfreq, 1.0);
}

void VenomDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    

    // copy samples for a dry signal, split into L and R instead of doing for(#channels) bc creating a cleanSignal array only works in mono
    float *cleanSignalL = new float[buffer.getNumSamples()];
    float *cleanSignalR = new float[buffer.getNumSamples()];
    
    
        auto* channelDataL = buffer.getWritePointer (0);
        auto* channelDataR = buffer.getWritePointer (1);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            cleanSignalL[sample] = channelDataL[sample];
            cleanSignalR[sample] = channelDataR[sample];
        }
    
    // apply distortion processing to channel data
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto sliderInputValue = treeState.getRawParameterValue (INPUT_ID);
        auto sliderDriveValue = treeState.getRawParameterValue (DRIVE_ID);
        auto sliderOutputValue = treeState.getRawParameterValue (OUTPUT_ID);
        
        auto hardclipBool = treeState.getRawParameterValue("hardclip");
        
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            //set input volume, add 3 bc algorithm makes starting volume slightly lower
            channelData[sample] = channelData[sample] * juce::Decibels::decibelsToGain(sliderInputValue->load() + 3);
            
            // https://www.youtube.com/watch?v=oIChUOV_0w4
            // compression at 23:13
            // bitcrushing at 29:00

            
            //hardclipper
            if (hardclipBool->load() == true)
            {
            algorithm = juce::jlimit (-1.f, 1.f, channelData[sample] * sliderDriveValue->load());
            }
            
            //Arctan
            else if (hardclipBool->load() == false)
            {
                algorithm = (2.0f/juce::float_Pi) * atan(channelData[sample] * sliderDriveValue->load());
            }
            
            
            //rectifier
            //algorithm = std::abs( channelData[sample] * sliderDriveValue->load());
            
            // apply distortion
            channelData[sample] = algorithm * juce::Decibels::decibelsToGain(sliderOutputValue->load());
            
            
        }
    }
    
    // process filtering
    juce::dsp::AudioBlock <float> block (buffer);
    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float> (block));
    highPassFilter.process(juce::dsp::ProcessContextReplacing<float> (block));
    updateFilter();
    
    // mixing bewtween dry signal and processed signal
    auto sliderMixValue = treeState.getRawParameterValue (MIX_ID);
    
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelDataL[sample] = (1.f - sliderMixValue->load()) * cleanSignalL[sample] + sliderMixValue->load() * channelDataL[sample];
            
            channelDataR[sample] = (1.f - sliderMixValue->load()) * cleanSignalR[sample] + sliderMixValue->load() * channelDataR[sample];
        }
    
}

//==============================================================================
bool VenomDistortionAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VenomDistortionAudioProcessor::createEditor()
{
    return new VenomDistortionAudioProcessorEditor (*this);
}

//==============================================================================
void VenomDistortionAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = treeState.copyState();
            std::unique_ptr<juce::XmlElement> xml (state.createXml());
            copyXmlToBinary (*xml, destData);
}

void VenomDistortionAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
     
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (treeState.state.getType()))
            treeState.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VenomDistortionAudioProcessor();
}
