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
lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 20000.0f, 0.1))
#endif
{
    juce::NormalisableRange<float> cutoffRange (100.0f, 20000.0f);
       
       treeState.createAndAddParameter("cutoff", "Cutoff", "cutoff", cutoffRange, 100.0f, nullptr, nullptr);
}

VenomDistortionAudioProcessor::~VenomDistortionAudioProcessor()
{
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout VenomDistortionAudioProcessor::createParameterLayout()
{
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    auto outputParam = std::make_unique<juce::AudioParameterFloat>(OUTPUT_ID, OUTPUT_NAME, -50.0f, 0.0f, 0.5f);
    params.push_back(std::move(outputParam));
    
    auto inputParam = std::make_unique<juce::AudioParameterFloat>(INPUT_ID, INPUT_NAME, -30.0f, 30.0f, 0.5f);
    params.push_back(std::move(inputParam));
    
    auto mixParam = std::make_unique<juce::AudioParameterFloat>(MIX_ID, MIX_NAME, 0.0f, 1.0f, 1.0f);
    params.push_back(std::move(mixParam));
    
    auto driveParam = std::make_unique<juce::AudioParameterFloat>(DRIVE_ID, DRIVE_NAME, 1.f, 25.0f, 0.05f);
    params.push_back(std::move(driveParam));
    
    //auto cutoffParam = std::make_unique<juce::AudioParameterFloat>(CUTOFF_ID, CUTOFF_NAME, 100.f, 20000.0f, 100.0f);
    //params.push_back(std::move(cutoffParam));
    

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
    float freq = *treeState.getRawParameterValue("cutoff");
    
    *lowPassFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, freq, 0.1);
}

void VenomDistortionAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        auto sliderOutputValue = treeState.getRawParameterValue (OUTPUT_ID);
        auto sliderInputValue = treeState.getRawParameterValue (INPUT_ID);
        auto sliderDriveValue = treeState.getRawParameterValue (DRIVE_ID);
        auto sliderMixValue = treeState.getRawParameterValue (MIX_ID);
        // ..do something to the data...
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            auto dry = channelData[sample];
            
            //set input volume
            auto wet = channelData[sample] * juce::Decibels::decibelsToGain(sliderInputValue->load());
            
            // https://www.youtube.com/watch?v=oIChUOV_0w4
            // compression at 23:13
            // bitcrushing at 29:00
            
            //algorithms
            float softcliparctan = (2.0f/juce::float_Pi) * atan(wet * sliderDriveValue->load());
            //float hardclip = juce::jlimit(-1.0f,1.0f,channelData[sample]) * drive;
            //float tanhwaveshaper = tanh(channelData[sample] * drive);
            //float sinefoldover = sin(channelData[sample]) * drive;
            //float softclipcubic = channelData[sample]-(1/3)*pow(channelData[sample], 3) * drive;
            
            // set drive and output and mix
            channelData[sample] = (((softcliparctan * sliderMixValue->load()) * (juce::Decibels::decibelsToGain(sliderOutputValue->load()))) + ((dry * juce::Decibels::decibelsToGain(sliderOutputValue->load())) * (1-sliderMixValue->load())));
            
            // set distortion and output
            //channelData[sample] = ((softcliparctan) * (juce::Decibels::decibelsToGain(output)));
            
            
        }
    }
    juce::dsp::AudioBlock <float> block (buffer);
        updateFilter();
        lowPassFilter.process(juce::dsp::ProcessContextReplacing<float> (block));
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
