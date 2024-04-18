/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestTake2AudioProcessor::TestTake2AudioProcessor()
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
chorusEffect(),  // Log2 of 1024
fft(10),
fftWindow(1024),
fftBuffer(1024),
window(1024),
fftSize(1024)
{
    // Create a Hann window
    for (int i = 0; i < fftSize; ++i) {
        window[i] = 0.5 * (1 - std::cos(2.0 * juce::MathConstants<double>::pi * i / (fftSize - 1)));
    }
}

TestTake2AudioProcessor::~TestTake2AudioProcessor() {
    
}


//==============================================================================
const juce::String TestTake2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TestTake2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TestTake2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TestTake2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TestTake2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TestTake2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TestTake2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void TestTake2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TestTake2AudioProcessor::getProgramName (int index)
{
    return {};
}

void TestTake2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TestTake2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = samplesPerBlock;
        spec.numChannels = getTotalNumInputChannels();

        chorusEffect.prepare(spec);
        chorusEffect.setCentreDelay(27.0f);
        chorusEffect.setMix(0.8f);
        chorusEffect.setFeedback(0.8f);
        chorusEffect.setDepth(0.8f);
}


void TestTake2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TestTake2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void TestTake2AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();

    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);

        // Temporary buffer to hold FFT data and a buffer for the results
        std::vector<std::complex<float>> fftData(fftSize);
        std::vector<std::complex<float>> fftOutput(fftSize);

        // Apply FFT window and compute FFT
        for (int i = 0; i < buffer.getNumSamples(); i += hopSize) {
            // Windowing
            for (int j = 0; j < fftSize; ++j) {
                if (i + j < buffer.getNumSamples()) {
                    fftData[j] = std::complex<float>(window[j] * channelData[i + j], 0.0f);
                }
            }

            // Forward FFT
            fft.perform(fftData.data(), fftOutput.data(), false); // Perform FFT and store results in fftOutput

            // Modify frequencies by shifting them in the frequency domain
            std::rotate(fftOutput.begin(), fftOutput.begin() + static_cast<int>(pitchShiftFactor * hopSize), fftOutput.end());

            // Inverse FFT
            fft.perform(fftOutput.data(), fftData.data(), true); // Perform inverse FFT and store results back in fftData

            // Overlap-add back into the buffer
            for (int j = 0; j < fftSize; ++j) {
                if (i + j < buffer.getNumSamples()) {
                    channelData[i + j] += fftData[j].real(); // Use only the real part of the complex result
                }
            }
        }
    }
}

//==============================================================================
bool TestTake2AudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* TestTake2AudioProcessor::createEditor() { return new TestTake2AudioProcessorEditor(*this); }

void TestTake2AudioProcessor::getStateInformation (juce::MemoryBlock& destData) {}
void TestTake2AudioProcessor::setStateInformation (const void* data, int sizeInBytes) {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new TestTake2AudioProcessor(); }
