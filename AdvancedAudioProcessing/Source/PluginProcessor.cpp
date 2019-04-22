/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AdvancedAudioProcessingAudioProcessor::AdvancedAudioProcessingAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    //Add Parameter for Stereo Pan
    addParameter(pan = new AudioParameterFloat("pan", "Pan", -1.0f, 1.0f, 0.0f));
    
    
    //Add Parameter for Stereo Width
    addParameter(width = new AudioParameterFloat("width", "Width", 0.0f, 2.0f, 0.5f));

	//Add Parameter for Input Choice
	addParameter(input = new AudioParameterChoice("input", "Input", { "Stereo", "Mid-Side" }, 0));

	//Add Parameter for Output Choice
	addParameter(output = new AudioParameterChoice("output", "Output", { "Stereo", "Mid-Side" }, 0));

}

AdvancedAudioProcessingAudioProcessor::~AdvancedAudioProcessingAudioProcessor()
{
}

//==============================================================================
const String AdvancedAudioProcessingAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AdvancedAudioProcessingAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AdvancedAudioProcessingAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AdvancedAudioProcessingAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AdvancedAudioProcessingAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AdvancedAudioProcessingAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AdvancedAudioProcessingAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AdvancedAudioProcessingAudioProcessor::setCurrentProgram (int index)
{
}

const String AdvancedAudioProcessingAudioProcessor::getProgramName (int index)
{
    return {};
}

void AdvancedAudioProcessingAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AdvancedAudioProcessingAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AdvancedAudioProcessingAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AdvancedAudioProcessingAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void AdvancedAudioProcessingAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
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
        //Declare Left and Right Channels as discrete
        auto* channelDataLeft = buffer.getWritePointer (0);
        auto* channelDataRight = buffer.getWritePointer (1);
        
        //Declare Stereo Pan position and pDash
        float stereoPan = pan->get();
        float pDash = (StereoPan + 1) / 2;
        
        //Declare Stereo Width
        float stereoWidth = width->get();
        
        //Declare Input and Output choice index
        int inputIndex = input->getIndex();
        int outputIndex = output->getIndex();
        
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            //Declare Left and Reight Inputs and store in float
            float inLeft = channelDataLeft[i];
            float inRight = channelDataRight[i];
		}

    }
}

//==============================================================================
bool AdvancedAudioProcessingAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AdvancedAudioProcessingAudioProcessor::createEditor()
{
	return new GenericAudioProcessorEditor(this);
}

//==============================================================================
void AdvancedAudioProcessingAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    
    MemoryOutputStream stream(destData, true);
    stream.writeFloat(*pan);
    stream.writeFloat(*width);
    stream.writeInt(*input);
    stream.writeInt(*output);
    
}

void AdvancedAudioProcessingAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    
    MemoryInputStream stream(data, static_cast<size_t> (sizeInBytes), false);
    pan->setValueNotifyingHost(stream.readFloat());
    width->setValueNotifyingHost(stream.readFloat());
    input->setValueNotifyingHost(stream.readInt());
    output->setValueNotifyingHost(stream.readInt());

    
    
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AdvancedAudioProcessingAudioProcessor();
}
