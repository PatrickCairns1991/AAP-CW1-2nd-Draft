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
	
	//Add Parameter for Channel Flip Toggle
	addParameter(flip = new AudioParameterBool("flip", "Flip", false, "Stereo Channel Flip (post-pan)"));

	//Add Parameter for Channel Polarity Invert Toggle
	addParameter(invert = new AudioParameterBool("invert", "Invert", false, "Stereo Polarity Invert (post-pan)"));

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

		auto* channelDataLeft = buffer.getWritePointer(0);
		auto* channelDataRight = buffer.getWritePointer(1);
        
        //Declare Stereo Pan position and pDash
        float stereoPan = pan->get();
        float pDash = 3.14159265359 * (stereoPan + 1) / 4;
		float leftPanGain = cos(pDash);
		float rightPanGain = sin(pDash);
        
        //Declare Stereo Width and related constants
        float stereoWidth = width->get();
        
        //Declare Input and Output choice index
        int inputIndex = input->getIndex();
        int outputIndex = output->getIndex();

		//Declare Input Channel Flip Toggle
		bool flipToggle = flip->get();

		//Declare Input Polarity Invert Toggle
		bool invertToggle = invert->get();
        
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            //Stereo In Stereo Out Flip Off Invert Off
            if(inputIndex == 0 && outputIndex == 0 && flipToggle == false && invertToggle == false)
            {
                //Inputs
                float inLeft = channelDataLeft[i];
                float inRight = channelDataRight[i];
                
                //Convert to MS for Width modulation
                float mid = (2 - stereoWidth) * 0.5 * (inLeft + inRight);
                float side = stereoWidth * 0.5 * (inLeft - inRight);
                
                //Convert Back to Stereo
                float left = (mid + side); 
                float right = (mid - side);

				//Pan
				float outLeft = left * leftPanGain;
				float outRight = right * rightPanGain;
                
                //Output
                channelDataLeft[i] = outLeft;
                channelDataRight[i] = outRight;  
            }

			//Stereo In Stereo Out Flip Off Invert On
			else if (inputIndex == 0 && outputIndex == 0 && flipToggle == false && invertToggle == true)
			{
				//Inputs
				float inLeft = channelDataLeft[i];
				float inRight = channelDataRight[i];

				//Convert to MS for Width modulation
				float mid = (2 - stereoWidth) * 0.5 * (inLeft + inRight);
				float side = stereoWidth * 0.5 * (inLeft - inRight);

				//Convert Back to Stereo
				float left = (mid + side);
				float right = (mid - side);

				//Pan
				float outLeft = left * leftPanGain;
				float outRight = right * rightPanGain;

				//Invert
				float outLeftInvert = outLeft * (-1.0);
				float outRightInvert = outRight * (-1.0);

				//Output and
				channelDataLeft[i] = outLeftInvert;
				channelDataRight[i] = outRightInvert;
			}

			//Stereo In Stereo Out Flip On Invert Off
			else if (inputIndex == 0 && outputIndex == 0 && flipToggle == true && invertToggle == false)
			{
				//Inputs
				float inLeft = channelDataLeft[i];
				float inRight = channelDataRight[i];

				//Convert to MS for Width modulation
				float mid = (2 - stereoWidth) * 0.5 * (inLeft + inRight);
				float side = stereoWidth * 0.5 * (inLeft - inRight);

				//Convert Back to Stereo
				float left = (mid + side);
				float right = (mid - side);

				//Pan
				float outLeft = left * leftPanGain;
				float outRight = right * rightPanGain;

				//Flip
				float outLeftFlipped = outRight;
				float outRightFlipped = outLeft;

				//Output
				channelDataLeft[i] = outLeftFlipped;
				channelDataRight[i] = outRightFlipped;
			}

			//Stereo In Stereo Out Flip On Invert On
			else if (inputIndex == 0 && outputIndex == 0 && flipToggle == true && invertToggle == true)
			{
				//Inputs
				float inLeft = channelDataLeft[i];
				float inRight = channelDataRight[i];

				//Convert to MS for Width modulation
				float mid = (2 - stereoWidth) * 0.5 * (inLeft + inRight);
				float side = stereoWidth * 0.5 * (inLeft - inRight);

				//Convert Back to Stereo
				float left = (mid + side);
				float right = (mid - side);

				//Pan
				float outLeft = left * leftPanGain;
				float outRight = right * rightPanGain;

				//Flip and Invert
				float outLeftFlippedAndInverted = outRight * (-1.0);
				float outRightFlippedAndInverted = outLeft * (-1.0);

				//Output
				channelDataLeft[i] = outLeftFlippedAndInverted;
				channelDataRight[i] = outRightFlippedAndInverted;
			}
            
            //Stereo in MS out Flip Off Invert Off
            else if(inputIndex == 0 && outputIndex == 1 && flipToggle == false && invertToggle == false)
            {
                //Inputs
                float inLeft = channelDataLeft[i];
                float inRight = channelDataRight[i];
                
                //Convert to MS for Width modulation
                float mid = (2 - stereoWidth) * 0.5 * (inLeft + inRight);
                float side = stereoWidth * 0.5 * (inLeft - inRight);

				//Convert Back to Stereo
				float left = mid + side;
				float right = mid - side;

				//Pan
				float leftPan = left * leftPanGain;
				float rightPan = right * rightPanGain;

				//Convert Back to MS
				float midOut = 0.5 * (leftPan + rightPan);
				float sideOut = 0.5 * (leftPan - rightPan);

                //Output
                channelDataLeft[i] = midOut;
                channelDataRight[i] = sideOut;
            }

			//Stereo in MS out Flip Off Invert On
			else if (inputIndex == 0 && outputIndex == 1 && flipToggle == false && invertToggle == true)
			{
				//Inputs
				float inLeft = channelDataLeft[i];
				float inRight = channelDataRight[i];

				//Convert to MS for Width modulation
				float mid = (2 - stereoWidth) * 0.5 * (inLeft + inRight);
				float side = stereoWidth * 0.5 * (inLeft - inRight);

				//Convert Back to Stereo
				float left = mid + side;
				float right = mid - side;

				//Pan and invert
				float leftPan = (-1.0) * left * leftPanGain;
				float rightPan = (-1.0) * right * rightPanGain;

				//Convert Back to MS
				float midOut = 0.5 * (leftPan + rightPan);
				float sideOut = 0.5 * (leftPan - rightPan);

				//Output
				channelDataLeft[i] = midOut;
				channelDataRight[i] = sideOut;
			}

			//Stereo in MS out Flip On Invert Off
			else if (inputIndex == 0 && outputIndex == 1 && flipToggle == true && invertToggle == false)
			{
				//Inputs
				float inLeft = channelDataLeft[i];
				float inRight = channelDataRight[i];

				//Convert to MS for Width modulation
				float mid = (2 - stereoWidth) * 0.5 * (inLeft + inRight);
				float side = stereoWidth * 0.5 * (inLeft - inRight);

				//Convert Back to Stereo
				float left = mid + side;
				float right = mid - side;

				//Pan
				float leftPan = left * leftPanGain;
				float rightPan = right * rightPanGain;

				//Flip Channels and Convert Back to MS
				float midOut = 0.5 * (rightPan + leftPan);
				float sideOut = 0.5 * (rightPan - leftPan);

				//Output
				channelDataLeft[i] = midOut;
				channelDataRight[i] = sideOut;
			}

			//Stereo in MS out Flip On Invert On
			else if (inputIndex == 0 && outputIndex == 1 && flipToggle == true && invertToggle == true)
			{
				//Inputs
				float inLeft = channelDataLeft[i];
				float inRight = channelDataRight[i];

				//Convert to MS for Width modulation
				float mid = (2 - stereoWidth) * 0.5 * (inLeft + inRight);
				float side = stereoWidth * 0.5 * (inLeft - inRight);

				//Convert Back to Stereo
				float left = mid + side;
				float right = mid - side;

				//Pan and invert
				float leftPan = (-1.0) * left * leftPanGain;
				float rightPan = (-1.0) * right * rightPanGain;

				//Flip Channels and Convert Back to MS
				float midOut = 0.5 * (rightPan + leftPan);
				float sideOut = 0.5 * (rightPan - leftPan);

				//Output
				channelDataLeft[i] = midOut;
				channelDataRight[i] = sideOut;
			}
            
            //MS in Stereo out Flip Off Invert Off
            else if(inputIndex == 1 && outputIndex == 0 && flipToggle == false && invertToggle == false)
            {
                //Inputs
                float mid = channelDataLeft[i];
                float side = channelDataRight[i];

				//Apply stereo width
				float midWide = (2 - stereoWidth) * mid;
				float sideWide = stereoWidth * side;

				//Convert to stereo and pan
				float outLeft = (midWide + sideWide) * leftPanGain;
				float outRight = (midWide - sideWide) * rightPanGain;
                
                //Output
                channelDataLeft[i] = outLeft;
                channelDataRight[i] = outRight;
            }

			//MS in Stereo out Flip Off Invert On
			else if (inputIndex == 1 && outputIndex == 0 && flipToggle == false && invertToggle == true)
			{
				//Inputs
				float mid = channelDataLeft[i];
				float side = channelDataRight[i];

				//Apply stereo width
				float midWide = (2 - stereoWidth) * mid;
				float sideWide = stereoWidth * side;

				//Convert to stereo, pan, and invert
				float outLeft = (midWide + sideWide) * leftPanGain * (-1.0);
				float outRight = (midWide - sideWide) * rightPanGain * (-1.0);

				//Output
				channelDataLeft[i] = outLeft;
				channelDataRight[i] = outRight;
			}

			//MS in Stereo out Flip On Invert Off
			else if (inputIndex == 1 && outputIndex == 0 && flipToggle == true && invertToggle == false)
			{
				//Inputs
				float mid = channelDataLeft[i];
				float side = channelDataRight[i];

				//Apply stereo width
				float midWide = (2 - stereoWidth) * mid;
				float sideWide = stereoWidth * side;

				//Convert to stereo and pan
				float outLeft = (midWide + sideWide) * leftPanGain;
				float outRight = (midWide - sideWide) * rightPanGain;

				//Flip and Output
				channelDataLeft[i] = outRight;
				channelDataRight[i] = outLeft;
			}

			//MS in Stereo out Flip On Invert On
			else if (inputIndex == 1 && outputIndex == 0 && flipToggle == true && invertToggle == true)
			{
				//Inputs
				float mid = channelDataLeft[i];
				float side = channelDataRight[i];

				//Apply stereo width
				float midWide = (2 - stereoWidth) * mid;
				float sideWide = stereoWidth * side;

				//Convert back to stereo, pan and invert
				float outLeft = (midWide + sideWide) * leftPanGain * (-1.0);
				float outRight = (midWide - sideWide) * rightPanGain * (-1.0);

				//Flip and Output
				channelDataLeft[i] = outRight;
				channelDataRight[i] = outLeft;
			}
            
            //MS in MS out Flip Off Invert Off
			else if (inputIndex == 1 && outputIndex == 1 && flipToggle == false && invertToggle == false)
			{
				//Inputs
				float mid = channelDataLeft[i];
				float side = channelDataRight[i];

				//Apply Width
				float midWide = (2 - stereoWidth) * mid;
				float sideWide = stereoWidth * side;

				//Convert to Stereo and Pan
				float left = (midWide + sideWide) * leftPanGain;
				float right = (midWide - sideWide) * rightPanGain;

				//Convert back to MS for output
				float outMid = 0.5 * (left + right);
				float outSide = 0.5 * (left - right);

				//Output
				channelDataLeft[i] = outMid;
				channelDataRight[i] = outSide;
			}

			//MS in MS out Flip Off Invert On
			else if (inputIndex == 1 && outputIndex == 1 && flipToggle == false && invertToggle == true)
			{
				//Inputs
				float mid = channelDataLeft[i];
				float side = channelDataRight[i];

				//Apply Width
				float midWide = (2 - stereoWidth) * mid;
				float sideWide = stereoWidth * side;

				//Convert to Stereo, Pan and Invert
				float left = (midWide + sideWide) * leftPanGain * (-1.0);
				float right = (midWide - sideWide) * rightPanGain * (-1.0);

				//Convert back to MS for output
				float outMid = 0.5 * (left + right);
				float outSide = 0.5 * (left - right);

				//Output
				channelDataLeft[i] = outMid;
				channelDataRight[i] = outSide;
			}

			//MS in MS out Flip On Invert Off
			else if (inputIndex == 1 && outputIndex == 1 && flipToggle == true && invertToggle == false)
			{
				//Inputs
				float mid = channelDataLeft[i];
				float side = channelDataRight[i];

				//Apply Width
				float midWide = (2 - stereoWidth) * mid;
				float sideWide = stereoWidth * side;

				//Convert to Stereo and Pan
				float left = (midWide + sideWide) * leftPanGain;
				float right = (midWide - sideWide) * rightPanGain;

				//Flip Channels and Convert back to MS for output
				float outMid = 0.5 * (right + left);
				float outSide = 0.5 * (right - left);

				//Output
				channelDataLeft[i] = outMid;
				channelDataRight[i] = outSide;
			}

			//MS in MS out Flip On Invert On
			else if (inputIndex == 1 && outputIndex == 1 && flipToggle == true && invertToggle == true)
			{
				//Inputs
				float mid = channelDataLeft[i];
				float side = channelDataRight[i];

				//Apply Width
				float midWide = (2 - stereoWidth) * mid;
				float sideWide = stereoWidth * side;

				//Convert to Stereo, Invert and Pan
				float left = (midWide + sideWide) * leftPanGain * (-1.0);
				float right = (midWide - sideWide) * rightPanGain * (-1.0);

				//Flip Channels and Convert back to MS for output
				float outMid = 0.5 * (right + left);
				float outSide = 0.5 * (right - left);

				//Output
				channelDataLeft[i] = outMid;
				channelDataRight[i] = outSide;
			}
                
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
	stream.writeBool(*flip);
	stream.writeBool(*invert);
    
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
	flip->setValueNotifyingHost(stream.readBool());
	invert->setValueNotifyingHost(stream.readBool());

    
    
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AdvancedAudioProcessingAudioProcessor();
}
