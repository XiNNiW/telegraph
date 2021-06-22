/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <telegraph_core.h>

//==============================================================================
/**
*/
constexpr size_t NUMBER_OF_VOICES = 16;
constexpr size_t MAX_UNISON = 16;
constexpr size_t WAVE_TABLE_SIZE = 1024;
constexpr size_t BLOCKSIZE = 32;
class TelegraphAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    TelegraphAudioProcessor();
    ~TelegraphAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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

private:
    void updateSynthParams();
    std::array<telegraph::voice_t<float,float, MAX_UNISON>,NUMBER_OF_VOICES> voices;
    telegraph::params_t<float> params;
    juce::AudioParameterChoice* exciter_pitch;
    juce::AudioParameterChoice* exciter_waveform;
    juce::AudioParameterFloat* exciter_gain;
    juce::AudioParameterFloat* exciter_vibrato_amount;
    juce::AudioParameterFloat* resonator_q;
    juce::AudioParameterChoice* resonator_pitch;
    juce::AudioParameterChoice* resonator_type;
    juce::AudioParameterFloat* resonator_chaos_character;
    juce::AudioParameterFloat* resonator_chaos_amount;
    juce::AudioParameterFloat* attack;
    juce::AudioParameterFloat* decay;
    juce::AudioParameterFloat* sustain;
    juce::AudioParameterFloat* release;
    juce::AudioParameterFloat* lowpass_cutoff;
    juce::AudioParameterFloat* lowpass_q;
    juce::AudioParameterFloat* highpass_cutoff;
    juce::AudioParameterFloat* stereo_width;
    juce::AudioParameterInt* unison;
    juce::AudioParameterFloat* unison_detune;
    juce::AudioParameterFloat* gain;
    
    juce::AudioParameterFloat* env_one_attack;
    juce::AudioParameterFloat* env_one_decay;
    juce::AudioParameterFloat* env_one_sustain;
    juce::AudioParameterFloat* env_one_release;
    
    juce::AudioParameterFloat* env_two_attack;
    juce::AudioParameterFloat* env_two_decay;
    juce::AudioParameterFloat* env_two_sustain;
    juce::AudioParameterFloat* env_two_release;
   
    juce::AudioParameterFloat* exciter_vibrato_speed;
    juce::AudioParameterFloat* lfo_one_speed;
    juce::AudioParameterFloat* lfo_two_speed;

    juce::AudioParameterFloat* mod_matrix[telegraph::Size<telegraph::ModSource>()][telegraph::Size<telegraph::ModDestination>()];

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TelegraphAudioProcessor)
};
