/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UIComponents.h"

//==============================================================================
/**
*/
class TelegraphAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    enum
    {
        paramControlHeight = 40,
        paramLabelWidth    = 80,
        paramSliderWidth   = 300
    };
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    TelegraphAudioProcessorEditor (TelegraphAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~TelegraphAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TelegraphAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& stateTree;

    TelegraphUIContentComponent ui=TelegraphUIContentComponent();
    std::unique_ptr<ComboBoxAttachment> exciterTypeAttachment;
    std::unique_ptr<SliderAttachment> exciterTuneAttachment;
    std::unique_ptr<SliderAttachment> unisonAmountAttachment;
    std::unique_ptr<SliderAttachment> unisonDetuneAttachment;
    std::unique_ptr<SliderAttachment> vibratoDepthAttachment;
    std::unique_ptr<SliderAttachment> vibratoSpeedAttachment;
    std::unique_ptr<SliderAttachment> exciterGainAttachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TelegraphAudioProcessorEditor)
};
