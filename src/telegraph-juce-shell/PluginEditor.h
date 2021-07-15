/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TelegraphUIContentComponent.h"
#include "ModMapButtonListener.h"
#include "PresetDisplayButtonListener.h"
#include "PresetFileManager.h"
#include "PresetSaveAsModalLauncher.h"
#include "TelegraphLogger.h"

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
    std::unique_ptr<ComboBoxAttachment> resonatorTypeAttachment;
    std::unique_ptr<SliderAttachment>   resonatorTuneAttachment;
    std::unique_ptr<SliderAttachment>   resonatorQAttachment;
    std::unique_ptr<SliderAttachment>   chaosAmountAttachment;
    std::unique_ptr<SliderAttachment>   chaosCharacterAttachment;
    std::unique_ptr<SliderAttachment>   highPassCutoffAttachment;
    std::unique_ptr<SliderAttachment> lowpasCutoffKnobAttachment;
    std::unique_ptr<SliderAttachment> lowpassQKnobAttachment;
    std::unique_ptr<SliderAttachment> ampAttackKnobAttachment;
    std::unique_ptr<SliderAttachment> ampDecayKnobAttachment;
    std::unique_ptr<SliderAttachment> ampSustainKnobAttachment;
    std::unique_ptr<SliderAttachment> ampReleaseKnobAttachment;
    std::unique_ptr<SliderAttachment> mainGainKnobAttachment;
    std::unique_ptr<SliderAttachment> modEnv1AttackKnob;
    std::unique_ptr<SliderAttachment> modEnv1DecayKnob;
    std::unique_ptr<SliderAttachment> modEnv1SustainKnob;
    std::unique_ptr<SliderAttachment> modEnv1ReleaseKnob;
    std::unique_ptr<SliderAttachment> modEnv2AttackKnob;
    std::unique_ptr<SliderAttachment> modEnv2DecayKnob;
    std::unique_ptr<SliderAttachment> modEnv2SustainKnob;
    std::unique_ptr<SliderAttachment> modEnv2ReleaseKnob;
    std::unique_ptr<SliderAttachment> modLFO1SpeedKnob;
    std::unique_ptr<SliderAttachment> modLFO2SpeedKnob;
    // std::map<telegraph::ModDestination,std::unique_ptr<SliderAttachment>> modulationKnobs;
    std::map<telegraph::ModSource,std::map<telegraph::ModDestination,std::unique_ptr<SliderAttachment>>> modulationMatrixKnobs;

    ModMapButtonListener modulationButtonListener;
    PresetDisplayButtonListener presetButtonListener;
    PresetSaveAsModalLauncher presetSaveAsModalLauncher;
    TelegraphLogger& logger = TelegraphLogger::Instance();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TelegraphAudioProcessorEditor)
};
