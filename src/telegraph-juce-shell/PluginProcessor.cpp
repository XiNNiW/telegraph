/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cassert>
#include <algae.h>
using algae::dsp::core::oscillator::sine_t;


//==============================================================================
TelegraphAudioProcessor::TelegraphAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter (exciter_pitch = new juce::AudioParameterChoice ("exciter_pitch", // parameterID
                                                    "Exciter Pitch", // parameter name
                                                    {"-12", "-7", "0", "7", "+12"},
                                                    2  // default index
                                                    )
    );
    addParameter (exciter_vibrato_speed = new juce::AudioParameterFloat ("exciter_vib_speed", // parameterID
                                                    "Exciter Vibrato Speed", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.5f)
    ); // default value
    addParameter (exciter_vibrato_amount = new juce::AudioParameterFloat ("exciter_vib_amount", // parameterID
                                                    "Exciter Vibrato Amount", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0)
    ); // default value
    addParameter (exciter_waveform = new juce::AudioParameterChoice ("exciter_waveform", // parameterID
                                                    "Exciter Waveform", // parameter name
                                                    {"SINE", "SAW", "SQUARE"},
                                                    0  // default index
                                                    )
    ); 
    addParameter (exciter_gain = new juce::AudioParameterFloat ("exciter_gain", // parameterID
                                                    "Exciter Gain", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.5f)
    ); // default value
    addParameter (resonator_pitch = new juce::AudioParameterChoice ("resonator_pitch", // parameterID
                                                    "Resonator Pitch", // parameter name
                                                    {"-12", "-7", "0", "7", "+12"},
                                                    2  // default index
                                                    )
    );
    addParameter (resonator_q = new juce::AudioParameterFloat ("resonator_q", // parameterID
                                                    "Resonator Q", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.5f)
    ); // default value
    addParameter (resonator_type = new juce::AudioParameterChoice ("resonator_type", // parameterID
                                                    "Resonator Chaos Type", // parameter name
                                                    {"COS", "WRAP", "TANH", "CLIP", "LOWERED_BELL"},
                                                    0  // default index
                                                    )
    ); 
    addParameter (resonator_chaos_character = new juce::AudioParameterFloat ("resonator_chaos_character", // parameterID
                                                    "Resonator Chaos Character", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    1.0f)
    ); // default value
    addParameter (resonator_chaos_amount = new juce::AudioParameterFloat ("resonator_chaos_amount", // parameterID
                                                    "Resonator Chaos Amount", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.0f)
    ); // default value
    addParameter (attack = new juce::AudioParameterFloat ("attack", // parameterID
                                                    "Attack", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.2f)
    ); // default value
    addParameter (decay = new juce::AudioParameterFloat ("decay", // parameterID
                                                    "Decay", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.2f)
    ); // default value
    addParameter (sustain = new juce::AudioParameterFloat ("sustain", // parameterID
                                                    "Sustain", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    1.0f)
    ); // default value
    addParameter (release = new juce::AudioParameterFloat ("release", // parameterID
                                                    "Release", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.333f)
    ); // default value



    addParameter (env_one_attack = new juce::AudioParameterFloat ("env_one_attack", // parameterID
                                                    "env 1: Attack", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.2f)
    ); // default value
    addParameter (env_one_decay = new juce::AudioParameterFloat ("env_one_decay", // parameterID
                                                    "env 1: Decay", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.2f)
    ); // default value
    addParameter (env_one_sustain = new juce::AudioParameterFloat ("env_one_sustain", // parameterID
                                                    "env 1: Sustain", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    1.0f)
    ); // default value
    addParameter (env_one_release = new juce::AudioParameterFloat ("env_one_release", // parameterID
                                                    "env 1: Release", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.333f)
    ); // default value

    addParameter (env_two_attack = new juce::AudioParameterFloat ("env_two_attack", // parameterID
                                                    "env 2: Attack", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.2f)
    ); // default value
    addParameter (env_two_decay = new juce::AudioParameterFloat ("env_two_decay", // parameterID
                                                    "env 2: Decay", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.2f)
    ); // default value
    addParameter (env_two_sustain = new juce::AudioParameterFloat ("env_two_sustain", // parameterID
                                                    "env 2: Sustain", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    1.0f)
    ); // default value
    addParameter (env_two_release = new juce::AudioParameterFloat ("env_two_release", // parameterID
                                                    "env 2: Release", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.333f)
    ); // default value

    addParameter (lfo_one_speed = new juce::AudioParameterFloat ("lfo_one_speed", // parameterID
                                                    "LFO Speed", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.5f)
    ); // default value
    addParameter (lfo_two_speed = new juce::AudioParameterFloat ("lfo_two_speed", // parameterID
                                                    "LFO Speed", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.5f)
    ); // default value
    addParameter (lowpass_cutoff = new juce::AudioParameterFloat ("lowpass_cutoff", // parameterID
                                                    "Lowpass Cutoff", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    1.0f)
    ); // default value
    addParameter (lowpass_q = new juce::AudioParameterFloat ("lowpass_q", // parameterID
                                                    "Lowpass Q", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.2f)
    ); // default value
    addParameter (highpass_cutoff = new juce::AudioParameterFloat ("highpass_cutoff", // parameterID
                                                    "Highpass Cutoff", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.0f)
    ); // default value  
    addParameter (unison = new juce::AudioParameterInt ("unison_amt", // parameterID
                                            "Unison", // parameter name
                                            1,   // minimum value
                                            16,   // maximum value
                                            1)
    ); // default value      
    addParameter (unison_detune = new juce::AudioParameterFloat ("unison_detune", // parameterID
                                            "Unison Detune", // parameter name
                                            0,   // minimum value
                                            1,   // maximum value
                                            0.01)
    ); // default value                                                                                       
    addParameter (stereo_width = new juce::AudioParameterFloat ("stereo_width", // parameterID
                                                "Stereo Width", // parameter name
                                                0.0f,   // minimum value
                                                1.0f,   // maximum value
                                                1.0f)
    ); // default value
    addParameter (gain = new juce::AudioParameterFloat ("gain", // parameterID
                                                    "Gain", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.75f)
    ); // default value
    for(size_t mod_source_idx=0; mod_source_idx<telegraph::Size<telegraph::ModSource>(); mod_source_idx++){
        for(size_t mod_dest_idx=0; mod_dest_idx<telegraph::Size<telegraph::ModDestination>(); mod_dest_idx++){
            std::string parameter_name = telegraph::DisplayName<telegraph::ModSource>(telegraph::ModSource(mod_source_idx));
            parameter_name += "to";
            parameter_name += telegraph::DisplayName<telegraph::ModDestination>(telegraph::ModDestination(mod_dest_idx));
            addParameter (mod_matrix[mod_source_idx][mod_dest_idx] = new juce::AudioParameterFloat (parameter_name, // parameterID
                                                    parameter_name, // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.0f)   // default value
            ); 
        }
    }
}

TelegraphAudioProcessor::~TelegraphAudioProcessor()
{
}

//==============================================================================
const juce::String TelegraphAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TelegraphAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TelegraphAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TelegraphAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TelegraphAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TelegraphAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TelegraphAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TelegraphAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TelegraphAudioProcessor::getProgramName (int index)
{
    return {};
}

void TelegraphAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TelegraphAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    for(size_t index=0; index<NUMBER_OF_VOICES; index++){
        voices[index] = telegraph::initVoice<float, float,MAX_UNISON>(voices[index], sampleRate);
    }

}

void TelegraphAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TelegraphAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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

void TelegraphAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const float SR = this->getSampleRate();
    const float CR = SR/BLOCKSIZE;

    if(!midiMessages.isEmpty()) {
        for (const MidiMessageMetadata it : midiMessages){
            MidiMessage m = it.getMessage();
            if (m.isNoteOn())
            {
                voices = telegraph::playNote<float, float, NUMBER_OF_VOICES>(voices, params, m.getChannel(), m.getNoteNumber(), m.getVelocity(), SR, CR);
            }
            else if (m.isNoteOff())
            {
                voices = telegraph::releaseNote<float, float, NUMBER_OF_VOICES>(voices, params, m.getChannel(), m.getNoteNumber(), m.getVelocity(),SR);
                
            }
            else if (m.isChannelPressure())
            {
            }
            else if (m.isAftertouch())
            {
            }
            else if (m.isPitchWheel())
            {
            }
            else if (m.isController())
            {
            }
            else if (m.isProgramChange())
            {
            }
            else
            {
                // std::cout << "Ignoring midi message " << std::endl;
            }
        }
        // apply the messages to free voices or find the quietest voice and replace it
    }

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    updateSynthParams();
    size_t num_samples_in_buffer = buffer.getNumSamples();
    size_t num_blocks = num_samples_in_buffer/BLOCKSIZE; 

    for(size_t block_idx=0; block_idx < num_blocks; block_idx++) {
        std::array<AudioBlock<float,BLOCKSIZE>,2> block;

        std::tie(voices, block) = process<float, float, MAX_UNISON, NUMBER_OF_VOICES, WAVE_TABLE_SIZE, BLOCKSIZE>(voices,params,SR);
        size_t num_processed = block_idx*BLOCKSIZE;
        for (size_t sample_index = num_processed; sample_index < num_processed+BLOCKSIZE; sample_index++)
        {
            if(sample_index < num_samples_in_buffer) {
                auto outL = buffer.getWritePointer(0, sample_index);
                auto outR = buffer.getWritePointer(1, sample_index);
                *outL += block[0][sample_index-num_processed]; 
                *outR += block[1][sample_index-num_processed]; 
            }
            
        }
    }

    size_t rem_samples = num_samples_in_buffer % BLOCKSIZE; 
    // assert(rem_samples == 0);

    if(rem_samples > 0) {
        size_t num_processed = num_blocks*BLOCKSIZE;
        for(size_t sample_idx = num_processed; sample_idx<num_samples_in_buffer; sample_idx++){
            std::array<AudioBlock<float, 1>, 2> block;
            std::tie(voices,block) = process<float, float, MAX_UNISON, NUMBER_OF_VOICES, WAVE_TABLE_SIZE, 1>(voices,params,SR);
            
            if(sample_idx < num_samples_in_buffer) {
                auto outL = buffer.getWritePointer(0, sample_idx);
                auto outR = buffer.getWritePointer(1, sample_idx);
                *outL += block[0][sample_idx-num_processed]; 
                *outR += block[1][sample_idx-num_processed]; 
            }
        }
    }


}

void TelegraphAudioProcessor::updateSynthParams(){
    // params.exciter_gain = telegraph::scale_parameter_as_db<float>(exciter_gain->get());
    // params.exciter_ratio = telegraph::lookup_safe<float>(exciter_pitch->getIndex(),{0.5, 2.0/3.0, 1, 3.0/2.0, 2});
    params.wave_mode = telegraph::lookup_safe<telegraph::Wave>(exciter_waveform->getIndex(), {telegraph::Wave::SINE,telegraph::Wave::SAW,telegraph::Wave::SQUARE});
    // params.resonator_q = telegraph::scale_parameter<float>(telegraph::scale_parameter_as_db<float>(resonator_q->get()),1.0, 100.0);
    // params.vibrato_speed = telegraph::scale_parameter<float>(exciter_vibrato_speed->get(),0,8);
    // params.vibrato_depth = telegraph::scale_parameter<float>(exciter_vibrato_amount->get(),0,2);
    params.feedback_mode = telegraph::lookup_safe<telegraph::FeedbackMode>(resonator_type->getIndex(), {telegraph::FeedbackMode::COS, telegraph::FeedbackMode::WRAP, telegraph::FeedbackMode::TANH, telegraph::FeedbackMode::CLIP, telegraph::FeedbackMode::LOWERED_BELL});

    // params.resonator_feedback = telegraph::scale_parameter_exp<float>(resonator_chaos_character->get(),0.01,100);
    // params.resonater_ratio = telegraph::lookup_safe<float>(resonator_pitch->getIndex(), {0.5, 2.0/3.0, 1, 3.0/2.0, 2});

    // params.resonator_chaos_character = telegraph::scale_parameter_exp<float>(resonator_chaos_character->get(),0,100);
    // params.resonator_chaos_amount = telegraph::scale_parameter_as_db<float>(resonator_chaos_amount->get());
    // params.amp_attack = telegraph::scale_parameter_exp<float>(attack->get(), 4.0, 4800.0);
    // params.amp_decay = telegraph::scale_parameter_exp<float>(decay->get(), 4.0, 4800.0);
    // params.amp_sustain = sustain->get();
    // params.amp_release = telegraph::scale_parameter_exp<float>(release->get(), 4.0, 4800.0);
    // params.lowpass_filter_cutoff = telegraph::scale_parameter_exp<float>(lowpass_cutoff->get(),200.0,20000.0);
    // params.lowpass_filter_q = lowpass_q->get();
    // params.highpass_filter_cutoff = telegraph::scale_parameter<float>(highpass_cutoff->get(),30.0,1000.0);
    // params.stereo_width = stereo_width->get();
    // params.unison = unison->get();
    // params.unison_detune = unison_detune->get();
    // params.gain = telegraph::scale_parameter_as_db<float>(gain->get());
    
    using telegraph::ModDestination;
    params.modulatable_params[static_cast<size_t>(ModDestination::EXCITER_GAIN)] = telegraph::scale_parameter_as_db<float>(exciter_gain->get());
    params.modulatable_params[static_cast<size_t>(ModDestination::EXCITER_FREQ)] = telegraph::lookup_safe<float>(exciter_pitch->getIndex(),{0.5, 2.0/3.0, 1, 3.0/2.0, 2});
    params.modulatable_params[static_cast<size_t>(ModDestination::RESONATOR_Q)] = telegraph::scale_parameter<float>(telegraph::scale_parameter_as_db<float>(resonator_q->get()),1.0, 100.0);
    params.modulatable_params[static_cast<size_t>(ModDestination::VIB_AMOUNT)] = telegraph::scale_parameter<float>(exciter_vibrato_amount->get(),0,2);
    params.modulatable_params[static_cast<size_t>(ModDestination::RESONATOR_FREQ)] = telegraph::lookup_safe<float>(resonator_pitch->getIndex(), {0.5, 2.0/3.0, 1, 3.0/2.0, 2});
    params.modulatable_params[static_cast<size_t>(ModDestination::CHAOS_CHARACTER)] = telegraph::scale_parameter_exp<float>(resonator_chaos_character->get(),0,100);
    params.modulatable_params[static_cast<size_t>(ModDestination::CHAOS_AMOUNT)] = telegraph::scale_parameter_as_db<float>(resonator_chaos_amount->get());
    params.modulatable_params[static_cast<size_t>(ModDestination::LOWPASS_CUTOFF)] = telegraph::scale_parameter_exp<float>(lowpass_cutoff->get(),200.0,20000.0);
    params.modulatable_params[static_cast<size_t>(ModDestination::LOWPASS_Q)] = lowpass_q->get();
    params.modulatable_params[static_cast<size_t>(ModDestination::HIGHPASS_CUTOFF)] = telegraph::scale_parameter<float>(highpass_cutoff->get(),30.0,1000.0);
    params.modulatable_params[static_cast<size_t>(ModDestination::DETUNE)] = unison_detune->get();
    params.modulatable_params[static_cast<size_t>(ModDestination::STEREO_WIDTH)] = stereo_width->get();
    params.modulatable_params[static_cast<size_t>(ModDestination::GAIN)] = telegraph::scale_parameter_as_db<float>(gain->get());

    params.amp_env_params.attack = telegraph::scale_parameter_exp<float>(attack->get(), 4.0, 4800.0);
    params.amp_env_params.decay = telegraph::scale_parameter_exp<float>(decay->get(), 4.0, 4800.0);
    params.amp_env_params.sustain = sustain->get();
    params.amp_env_params.release = telegraph::scale_parameter_exp<float>(release->get(), 4.0, 4800.0);

    params.mod_env_one_params.attack = telegraph::scale_parameter_exp<float>(env_one_attack->get(), 4.0, 4800.0);
    params.mod_env_one_params.decay = telegraph::scale_parameter_exp<float>(env_one_decay->get(), 4.0, 4800.0);
    params.mod_env_one_params.sustain = env_one_sustain->get();
    params.mod_env_one_params.release = telegraph::scale_parameter_exp<float>(env_one_release->get(), 4.0, 4800.0);
    
    params.mod_env_two_params.attack = telegraph::scale_parameter_exp<float>(env_two_attack->get(), 4.0, 4800.0);
    params.mod_env_two_params.decay = telegraph::scale_parameter_exp<float>(env_two_decay->get(), 4.0, 4800.0);
    params.mod_env_two_params.sustain = env_one_sustain->get();
    params.mod_env_two_params.release = telegraph::scale_parameter_exp<float>(env_two_release->get(), 4.0, 4800.0);
   
    params.modulatable_params[static_cast<size_t>(ModDestination::VIB_SPEED)]     = telegraph::scale_parameter<float>(exciter_vibrato_speed->get(),0,8);
    params.modulatable_params[static_cast<size_t>(ModDestination::LFO_ONE_SPEED)] = telegraph::scale_parameter<float>(lfo_one_speed->get(),0,8);
    params.modulatable_params[static_cast<size_t>(ModDestination::LFO_TWO_SPEED)] = telegraph::scale_parameter<float>(lfo_two_speed->get(),0,8);

    params.unison = unison->get();

    for(size_t mod_source_idx=0; mod_source_idx<telegraph::Size<telegraph::ModSource>(); mod_source_idx++){
        for(size_t mod_dest_idx=0; mod_dest_idx<telegraph::Size<telegraph::ModDestination>(); mod_dest_idx++){
            params.mod_matrix[mod_source_idx][mod_dest_idx] = mod_matrix[mod_source_idx][mod_dest_idx]->get();
        }
    }
}

//==============================================================================
bool TelegraphAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TelegraphAudioProcessor::createEditor()
{
    return new GenericAudioProcessorEditor(*this);//new TelegraphAudioProcessorEditor (*this);
}

//==============================================================================
void TelegraphAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TelegraphAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TelegraphAudioProcessor();
}
