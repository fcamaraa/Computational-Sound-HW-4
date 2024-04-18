/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestTake2AudioProcessorEditor::TestTake2AudioProcessorEditor (TestTake2AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 50, 20);
    gainSlider.setRange(0.0f, 1.0f, 0.01f);
    gainSlider.setValue(0.5f);
    gainSlider.addListener(this);
    addAndMakeVisible(gainSlider);

    setSize(400, 300);
    
    // Setup pitch shift slider
        pitchSlider.setSliderStyle(juce::Slider::LinearVertical);
        pitchSlider.setTextBoxStyle(juce::Slider::TextBoxAbove, true, 50, 20);
        pitchSlider.setRange(0.5f, 2.0f, 0.01f);  // Range suitable for pitch shifting (0.5x to 2x)
        pitchSlider.setValue(1.0f);               // Default value (no pitch shift)
        pitchSlider.addListener(this);
        addAndMakeVisible(pitchSlider);

        setSize(400, 300);
}

TestTake2AudioProcessorEditor::~TestTake2AudioProcessorEditor()
{
}

//==============================================================================
void TestTake2AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Gain Shift", getLocalBounds(), juce::Justification::centred, 1);
    gainSlider.setBounds(50, getHeight() / 2 - 100, 100, 200);
}

void TestTake2AudioProcessorEditor::resized()
{
    gainSlider.setBounds(50, getHeight() / 2 - 100, 100, 200);
        pitchSlider.setBounds(200, getHeight() / 2 - 100, 100, 200);  // Position next to the gain slider
}

void TestTake2AudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    if (slider == &gainSlider) {
        audioProcessor.rawVolume = gainSlider.getValue();
    } else if (slider == &pitchSlider) {
        audioProcessor.setPitchShift(pitchSlider.getValue());
    }
}


