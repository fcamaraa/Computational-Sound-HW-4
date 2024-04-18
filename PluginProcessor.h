/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <vector>
#include <complex>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
/**
*/
class TestTake2AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TestTake2AudioProcessor();
    ~TestTake2AudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    //void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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

    double rawVolume{ 0.5 };
    void setPitchShift(float newPitch) {
        pitchShiftFactor = newPitch;
    }

private:
    juce::dsp::Chorus <float> chorusEffect;

    double gainSmoothed{0.5};
    float lastSample = 0;
    
    juce::dsp::FFT fft;
        std::vector<std::complex<float>> fftWindow;
        std::vector<std::complex<float>> fftBuffer;
        std::vector<float> window;
        int fftSize = 1024;
        int hopSize = 512;
        float pitchShiftFactor = 1.5; // Example: 1.5 times the original pitch
    float currentGain = 0.0f; // Current gain applied
        float targetGain = 0.5f; 
        
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestTake2AudioProcessor)
};
