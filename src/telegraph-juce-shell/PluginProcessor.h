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
    juce::AudioProcessorValueTreeState& getStateTree(){
      return parameters;
    }

private:
    void updateSynthParams();
    juce::AudioProcessorValueTreeState parameters;
    std::array<telegraph::voice_t<float, float, MAX_UNISON>,NUMBER_OF_VOICES> voices;
    telegraph::params_t<float> params;
    std::atomic<float>* exciter_pitch;
    std::atomic<float>* exciter_waveform; //
    std::atomic<float>* exciter_gain;
    std::atomic<float>* exciter_vibrato_amount;
    std::atomic<float>* resonator_q;
    std::atomic<float>* resonator_pitch;
    std::atomic<float>* resonator_type; //
    std::atomic<float>* resonator_chaos_character;
    std::atomic<float>* resonator_chaos_amount;
    std::atomic<float>* attack; //
    std::atomic<float>* decay;//
    std::atomic<float>* sustain;//
    std::atomic<float>* release;//
    std::atomic<float>* lowpass_cutoff;
    std::atomic<float>* lowpass_q;
    std::atomic<float>* highpass_cutoff;
    std::atomic<float>* stereo_width;
    std::atomic<float>* unison;//
    std::atomic<float>* unison_detune;
    std::atomic<float>* gain;
    
    std::atomic<float>* env_one_attack;//
    std::atomic<float>* env_one_decay;//
    std::atomic<float>* env_one_sustain;//
    std::atomic<float>* env_one_release;//
    
    std::atomic<float>* env_two_attack;//
    std::atomic<float>* env_two_decay;//
    std::atomic<float>* env_two_sustain;//
    std::atomic<float>* env_two_release;//
   
    std::atomic<float>* exciter_vibrato_speed;
    std::atomic<float>* lfo_one_speed;
    std::atomic<float>* lfo_two_speed;

    std::array<std::array<std::atomic<float>*,telegraph::Size<telegraph::ModDestination>()>,telegraph::Size<telegraph::ModSource>()> mod_matrix;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TelegraphAudioProcessor)
};
