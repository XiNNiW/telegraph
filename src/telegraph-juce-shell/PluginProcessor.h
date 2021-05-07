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
const int NUMBER_OF_VOICES = 16;
const int WAVE_TABLE_SIZE = 1024;
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
    std::array<telegraph::voice_t<double,double>,NUMBER_OF_VOICES> voices;
    telegraph::params_t<double> params;
    juce::AudioParameterChoice* exciter_pitch;
    juce::AudioParameterChoice* exciter_waveform;
    juce::AudioParameterFloat* exciter_gain;

    juce::AudioParameterFloat* resonator_q;
    juce::AudioParameterChoice* resonator_pitch;
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
    juce::AudioParameterFloat* gain;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TelegraphAudioProcessor)
};
