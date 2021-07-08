/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <telegraph_core.h>

using telegraph::DisplayName;
using telegraph::TokenName;
using telegraph::ModDestination;
using telegraph::NonModulatedParameter;

//==============================================================================
TelegraphAudioProcessorEditor::TelegraphAudioProcessorEditor (TelegraphAudioProcessor& p, juce::AudioProcessorValueTreeState& t)
    : AudioProcessorEditor (&p), audioProcessor (p), stateTree(t)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    // //GAIN
    // mainGainLabel.setText (DisplayName<ModDestination>(ModDestination::GAIN), juce::dontSendNotification);
    // addAndMakeVisible (mainGainLabel);

    // addAndMakeVisible (mainGainKnob);
    // mainGainAttachment.reset (new SliderAttachment (stateTree, TokenName<ModDestination>(ModDestination::GAIN), mainGainKnob));
    // // END GAINd
    exciterTypeAttachment.reset (
      new ComboBoxAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::EXCITER_TYPE), 
        *ui.getExciterTypeSelector()
      )
    );
    exciterTuneAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::EXCITER_FREQ), 
        *ui.getUnisonAmountKnob()
      )
    );
    unisonAmountAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::UNISON_AMOUNT), 
        *ui.getUnisonAmountKnob()
      )
    );
    unisonDetuneAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::DETUNE), 
        *ui.getUnisonDetuneKnob()
      )
    );
    vibratoDepthAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::VIB_AMOUNT), 
        *ui.getVibratoDepthKnob()
      )
    );
    vibratoSpeedAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::VIB_SPEED), 
        *ui.getVibratoSpeedKnob()
      )
    );
    exciterGainAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::EXCITER_GAIN), 
        *ui.getExciterGainKnob()
      )
    );
    resonatorTypeAttachment.reset (
      new ComboBoxAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::RESONATOR_TYPE), 
        *ui.getResontorTypeSelector()
      )
    );
    
    resonatorTuneAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::RESONATOR_FREQ), 
        *ui.getResonatorTuneKnob()
      )
    );
    resonatorQAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::RESONATOR_Q), 
        *ui.getResonatorQKnob()
      )
    );
    chaosAmountAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::CHAOS_AMOUNT), 
        *ui.getChaosAmountKnob()
      )
    );
    chaosCharacterAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::CHAOS_CHARACTER), 
        *ui.getChaosCharacter()
      )
    );
    highPassCutoffAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::HIGHPASS_CUTOFF), 
        *ui.getHighpassCutoffKnob()
      )
    );

    lowpasCutoffKnobAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::LOWPASS_CUTOFF), 
        *ui.getLowpasCutoffKnob()
      )
    );
    lowpassQKnobAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::LOWPASS_Q), 
        *ui.getLowpassQKnob()
      )
    );
    // ampMapButtonKnobAttachment.reset (
    //   new ButtonAttachment (
    //     stateTree, 
    //     TokenName<ModDestination>(ModDestination::HIGHPASS_CUTOFF), 
    //     *ui.getHighpassCutoffKnob()
    //   )
    // );
    ampAttackKnobAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_ATTACK), 
        *ui.getAmpAttackKnob()
      )
    );
    ampDecayKnobAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_DECAY), 
        *ui.getAmpDecayKnob()
      )
    );
    ampSustainKnobAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_SUSTAIN), 
        *ui.getAmpSustainKnob()
      )
    );
    ampReleaseKnobAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_RELEASE), 
        *ui.getAmpReleaseKnob()
      )
    );
    mainGainKnobAttachment.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::GAIN), 
        *ui.getMainGainKnob()
      )
    );
    // modEnv1MapButton.reset (
    //   new SliderAttachment (
    //     stateTree, 
    //     TokenName<ModDestination>(ModDestination::GAIN), 
    //     *ui.getMainGainKnob()
    //   )
    // );
    modEnv1AttackKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_ATTACK), 
        *ui.getModEnv1AttackKnob()
      )
    );
    modEnv1DecayKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_DECAY), 
        *ui.getModEnv1DecayKnob()
      )
    );
    modEnv1SustainKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_SUSTAIN), 
        *ui.getModEnv1SustainKnob()
      )
    );
    modEnv1ReleaseKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_RELEASE), 
        *ui.getModEnv1ReleaseKnob()
      )
    );
    // modEnv2MapButton.reset (
    //   new SliderAttachment (
    //     stateTree, 
    //     TokenName<ModDestination>(ModDestination::GAIN), 
    //     *ui.getMainGainKnob()
    //   )
    // );
    modEnv2AttackKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_ATTACK), 
        *ui.getModEnv2AttackKnob()
      )
    );
    modEnv2DecayKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_DECAY), 
        *ui.getModEnv2DecayKnob()
      )
    );
    modEnv2SustainKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_SUSTAIN), 
        *ui.getModEnv2SustainKnob()
      )
    );
    modEnv2ReleaseKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_RELEASE), 
        *ui.getModEnv2ReleaseKnob()
      )
    );
    // modLFO1MapButton.reset (
    //   new SliderAttachment (
    //     stateTree, 
    //     TokenName<ModDestination>(ModDestination::GAIN), 
    //     *ui.getMainGainKnob()
    //   )
    // );
    modLFO1SpeedKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::LFO_ONE_SPEED), 
        *ui.getModLFO1SpeedKnob()
      )
    );
    // modLFO2MapButton.reset (
    //   new SliderAttachment (
    //     stateTree, 
    //     TokenName<ModDestination>(ModDestination::LFO_ONE_SPEED), 
    //     *ui.getModLFO1SpeedKnob()
    //   )
    // );
    modLFO2SpeedKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::LFO_TWO_SPEED), 
        *ui.getModLFO2SpeedKnob()
      )
    );
  

    addAndMakeVisible(ui);

    setSize (1000, 900);
    // setSize (paramSliderWidth + paramLabelWidth, juce::jmax (100, paramControlHeight * 2));
}

TelegraphAudioProcessorEditor::~TelegraphAudioProcessorEditor()
{
}

//==============================================================================
void TelegraphAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    // g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // g.setColour (juce::Colours::white);
    // g.setFont (15.0f);
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void TelegraphAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // ui.resized();
    auto r = getLocalBounds();
    ui.setBounds(r);

    // auto gainRect = r.removeFromTop (paramControlHeight);
    // mainGainLabel.setBounds (gainRect.removeFromLeft (paramLabelWidth));
    // mainGainKnob.setBounds (gainRect);


}
