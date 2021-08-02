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
    : AudioProcessorEditor (&p)
    , audioProcessor (p)
    , stateTree(t)
    , modulationButtonListener(ModMapButtonListener(ui, t, modulationMatrixKnobs))
    , presetButtonListener(PresetDisplayButtonListener(ui, p))
    , presetSaveAsModalLauncher(ui, p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    using telegraph::ModSource;
    modulationButtonListener.attach(ModSource::AMP_ENV,ui.getAmpMapButton().get());
    modulationButtonListener.attach(ModSource::ENV_ONE,ui.getModEnv1MapButton().get());
    modulationButtonListener.attach(ModSource::ENV_TWO,ui.getModEnv2MapButton().get());
    modulationButtonListener.attach(ModSource::LFO_ONE,ui.getModLFO1MapButton().get());
    modulationButtonListener.attach(ModSource::LFO_TWO,ui.getModLFO2MapButton().get());
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
        *ui.getExciterTuneKnob()
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
        *ui.getChaosCharacterKnob()
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

    modLFO1SpeedKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::LFO_ONE_SPEED), 
        *ui.getModLFO1SpeedKnob()
      )
    );

    modLFO2SpeedKnob.reset (
      new SliderAttachment (
        stateTree, 
        TokenName<ModDestination>(ModDestination::LFO_TWO_SPEED), 
        *ui.getModLFO2SpeedKnob()
      )
    );

    using telegraph::ModSource;
    using telegraph::ModDestination;
    using telegraph::Size;
    for(size_t source_idx=0; source_idx<Size<ModSource>(); source_idx++){
      for(size_t dest_idx=0; dest_idx<Size<ModDestination>(); dest_idx++){

        
        std::string parameter_id = std::string(TokenName<ModSource>(ModSource(source_idx)) )
            + "_to_" 
            + std::string(telegraph::TokenName<ModDestination>(ModDestination(dest_idx)));

        modulationMatrixKnobs[ModSource(source_idx)][ModDestination(dest_idx)].reset(
            new SliderAttachment (
                stateTree, 
                parameter_id, 
                *ui.modulationAmountKnobs[ModSource(source_idx)][ModDestination(dest_idx)]
            )
        );

      }
    }
  
    addAndMakeVisible(ui);
    // auto screenBounds = getScreenBounds()
    setSize (1000, 950);


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
