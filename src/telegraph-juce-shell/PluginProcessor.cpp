/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cassert>
#include <algae.h>

using telegraph::DisplayName;
using telegraph::TokenName;
using telegraph::ModDestination;
using telegraph::NonModulatedParameter;

static juce::AudioProcessorValueTreeState::ParameterLayout constructParameters(){
    using telegraph::getParameterRange;
    juce::AudioProcessorValueTreeState::ParameterLayout paramList = {
                        
                        std::unique_ptr<juce::AudioParameterChoice> (
                            new juce::AudioParameterChoice(
                                TokenName<NonModulatedParameter>(NonModulatedParameter::EXCITER_TYPE), 
                                DisplayName<NonModulatedParameter>(NonModulatedParameter::EXCITER_TYPE),
                                {"SINE", "SAW", "SQUARE"}, 
                                2
                            )
                        ),
                        
                        std::unique_ptr<juce::AudioParameterChoice> (
                            new juce::AudioParameterChoice(
                                TokenName<NonModulatedParameter>(NonModulatedParameter::RESONATOR_TYPE), 
                                DisplayName<NonModulatedParameter>(NonModulatedParameter::RESONATOR_TYPE),
                                {"COS", "WRAP", "TANH", "CLIP", "LOWERED_BELL"}, 
                                2
                            )
                        ),
                        
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_ATTACK), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::AMP_ATTACK),
                            0.0f,
                            1.0f,
                            0.2f
                        ),
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_DECAY), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::AMP_DECAY),
                            0.0f,
                            1.0f,
                            0.2f
                        ),
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_SUSTAIN), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::AMP_SUSTAIN),
                            0.0f,
                            1.0f,
                            1.0f
                        ),
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_RELEASE), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::AMP_RELEASE),
                            0.0f,
                            1.0f,
                            0.7f
                        ),                        
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_ATTACK), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_ATTACK),
                            0.0f,
                            1.0f,
                            0.2f
                        ),
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_DECAY), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_DECAY),
                            0.0f,
                            1.0f,
                            0.2f
                        ),
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_SUSTAIN), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_SUSTAIN),
                            0.0f,
                            1.0f,
                            1.0f
                        ),
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_RELEASE), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_RELEASE),
                            0.0f,
                            1.0f,
                            0.7f
                        ),                        
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_ATTACK), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_ATTACK),
                            0.0f,
                            1.0f,
                            0.2f
                        ),
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_DECAY), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_DECAY),
                            0.0f,
                            1.0f,
                            0.2f
                        ),
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_SUSTAIN), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_SUSTAIN),
                            0.0f,
                            1.0f,
                            1.0f
                        ),
                        std::make_unique<juce::AudioParameterFloat> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_RELEASE), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_RELEASE),
                            0.0f,
                            1.0f,
                            0.7f
                        ),
                        
                        std::make_unique<juce::AudioParameterInt> (
                            TokenName<NonModulatedParameter>(NonModulatedParameter::UNISON_AMOUNT), 
                            DisplayName<NonModulatedParameter>(NonModulatedParameter::UNISON_AMOUNT),
                            1,
                            16,
                            1
                        ),
                        
                      };
    for(size_t mod_dest_idx=0; mod_dest_idx<telegraph::Size<telegraph::ModDestination>(); mod_dest_idx++){
        // auto scaleType = std::get<3>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx)));
        // switch (scaleType)
        // {
        // case telegraph::ScalingType::DB:
        //     paramList.add( 
        //         std::make_unique<juce::AudioParameterFloat> (
        //             telegraph::TokenName<ModDestination>(telegraph::ModDestination(mod_dest_idx)), // parameterID
        //             telegraph::DisplayName<ModDestination>(telegraph::ModDestination(mod_dest_idx)), // parameter name
        //             juce::NormalisableRange<float>(
        //                 std::get<0>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))),   // minimum value
        //                 std::get<1>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))),   // maximum value
        //                 [](float rangeStart, float rangeEnd, float val)->float{return algae::dsp::core::units::rmstodb(val);}, //from 0-1
        //                 [](float rangeStart, float rangeEnd, float val)->float{return algae::dsp::core::units::dbtorms(val);}  //to 0-1
        //             ),
        //             std::get<2>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))) // default value
        //         )   
        //     ); 
        //     break;
        // case telegraph::ScalingType::FREQ:
        //     paramList.add( 
        //         std::make_unique<juce::AudioParameterFloat> (
        //             telegraph::TokenName<ModDestination>(telegraph::ModDestination(mod_dest_idx)), // parameterID
        //             telegraph::DisplayName<ModDestination>(telegraph::ModDestination(mod_dest_idx)), // parameter name
        //             juce::NormalisableRange<float>(
        //                 std::get<0>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))),   // minimum value
        //                 std::get<1>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))),   // maximum value
        //                 [](float min, float max, float val)->float{return telegraph::scale_parameter_from_norm_exp(val,min,max);}, //from 0-1
        //                 [](float min, float max, float val)->float{return telegraph::scale_parameter_to_norm_exp(val,min,max);}  //to 0-1
        //             ),
        //             std::get<2>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))) // default value
        //         )   
        //     ); 
        //     break;
        // case telegraph::ScalingType::EXP:
        //     paramList.add( 
        //         std::make_unique<juce::AudioParameterFloat> (
        //             telegraph::TokenName<ModDestination>(telegraph::ModDestination(mod_dest_idx)), // parameterID
        //             telegraph::DisplayName<ModDestination>(telegraph::ModDestination(mod_dest_idx)), // parameter name
        //             juce::NormalisableRange<float>(
        //                 std::get<0>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))),   // minimum value
        //                 std::get<1>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))),   // maximum value
        //                 [](float min, float max, float val)->float{return telegraph::scale_parameter_from_norm_exp(val,min,max);}, //from 0-1
        //                 [](float min, float max, float val)->float{return telegraph::scale_parameter_to_norm_exp(val,min,max);}  //to 0-1
        //             ),
        //             std::get<2>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))) // default value
        //         )   
        //     ); 
        //     break;
        // case telegraph::ScalingType::QSEMITONES:
        //     paramList.add( 
        //         std::make_unique<juce::AudioParameterFloat> (
        //             telegraph::TokenName<ModDestination>(telegraph::ModDestination(mod_dest_idx)), // parameterID
        //             telegraph::DisplayName<ModDestination>(telegraph::ModDestination(mod_dest_idx)), // parameter name
        //             juce::NormalisableRange<float>(
        //                 std::get<0>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))),   // minimum value
        //                 std::get<1>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))),   // maximum value
        //                 [](float min, float max, float val)->float{return telegraph::scale_parameter_from_norm(val,min,max);}, //from 0-1
        //                 [](float min, float max, float val)->float{return telegraph::scale_parameter_to_norm(val,min,max);},  //to 0-1
        //                 [](float min, float max, float val)->float{return floor(val);}
        //             ),
        //             std::get<2>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))) // default value
        //         )   
        //     ); 
        //     break;
        // default:
            paramList.add( 
                std::make_unique<juce::AudioParameterFloat> (
                    telegraph::TokenName<ModDestination>(telegraph::ModDestination(mod_dest_idx)), // parameterID
                    telegraph::DisplayName<ModDestination>(telegraph::ModDestination(mod_dest_idx)), // parameter name
                    std::get<0>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))),   // minimum value
                    std::get<1>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))),   // maximum value
                    std::get<2>(telegraph::getParameterRange<float>(telegraph::ModDestination(mod_dest_idx))) // default value
                )   
            ); 
        //     break;

        // }
        
    }
    for(size_t mod_source_idx=0; mod_source_idx<telegraph::Size<telegraph::ModSource>(); mod_source_idx++){
        for(size_t mod_dest_idx=0; mod_dest_idx<telegraph::Size<telegraph::ModDestination>(); mod_dest_idx++){
            std::string parameter_id = std::string(TokenName<telegraph::ModSource>(telegraph::ModSource(mod_source_idx)) )
                                        + "_to_" 
                                        + std::string(telegraph::TokenName<telegraph::ModDestination>(telegraph::ModDestination(mod_dest_idx)));
            std::string parameter_name = std::string(telegraph::DisplayName<telegraph::ModSource>(telegraph::ModSource(mod_source_idx)))
                                        + " to "
                                        + std::string(telegraph::DisplayName<telegraph::ModDestination>(telegraph::ModDestination(mod_dest_idx)));
            paramList.add( 
                std::make_unique<juce::AudioParameterFloat> (
                    parameter_id, // parameterID
                    parameter_name, // parameter name
                    -1.0f,   // minimum value
                    1.0f,   // maximum value
                    0.0f // default value
                )   
            ); 
        }
    }

    return paramList;
}

//==============================================================================
TelegraphAudioProcessor::TelegraphAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    :AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
#else
:
#endif
parameters (*this, nullptr, juce::Identifier ("Telegraph"), constructParameters())
{
    using telegraph::NonModulatedParameter;
    using telegraph::ModDestination;
    using telegraph::TokenName;
    exciter_pitch = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::EXCITER_FREQ));
    exciter_waveform = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::EXCITER_TYPE)); //
    exciter_gain = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::EXCITER_GAIN));
    exciter_vibrato_amount = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::VIB_AMOUNT));
    resonator_q = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::RESONATOR_Q));
    resonator_pitch = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::RESONATOR_FREQ));
    resonator_type = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::RESONATOR_TYPE)); //
    resonator_chaos_character = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::CHAOS_CHARACTER));
    resonator_chaos_amount = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::CHAOS_AMOUNT));
    attack = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_ATTACK)); //
    decay = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_DECAY));//
    sustain = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_SUSTAIN));//
    release = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::AMP_RELEASE));//
    lowpass_cutoff = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::LOWPASS_CUTOFF));
    lowpass_q = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::LOWPASS_Q));
    highpass_cutoff = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::HIGHPASS_CUTOFF));
    stereo_width = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::STEREO_WIDTH));
    unison = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::UNISON_AMOUNT));//
    unison_detune = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::DETUNE));
    gain = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::GAIN));
    
    env_one_attack = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_ATTACK));//
    env_one_decay = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_DECAY));//
    env_one_sustain = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_SUSTAIN));//
    env_one_release = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_ONE_RELEASE));//
    
    env_two_attack = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_ATTACK));//
    env_two_decay = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_DECAY));//
    env_two_sustain = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_SUSTAIN));//
    env_two_release = parameters.getRawParameterValue(TokenName<NonModulatedParameter>(NonModulatedParameter::ENV_TWO_RELEASE));//
   
    exciter_vibrato_speed = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::VIB_SPEED));
    lfo_one_speed = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::LFO_ONE_SPEED));
    lfo_two_speed = parameters.getRawParameterValue(TokenName<ModDestination>(ModDestination::LFO_TWO_SPEED));
    for(size_t source_idx=0; source_idx<telegraph::Size<telegraph::ModSource>(); source_idx++){
        for(size_t dest_idx=0; dest_idx<telegraph::Size<telegraph::ModDestination>(); dest_idx++){
            std::string parameter_id = std::string(TokenName<telegraph::ModSource>(telegraph::ModSource(source_idx))) 
                                        + "_to_" 
                                        + std::string(telegraph::TokenName<telegraph::ModDestination>(telegraph::ModDestination(dest_idx)));
            mod_matrix[source_idx][dest_idx] = parameters.getRawParameterValue(parameter_id);
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

    using telegraph::NonModulatedParameter;
    using algae::dsp::core::units::dbtorms;
    params.wave_mode = telegraph::lookup_safe<telegraph::Wave>(*exciter_waveform, {telegraph::Wave::SINE,telegraph::Wave::SAW,telegraph::Wave::SQUARE});

    params.feedback_mode = telegraph::lookup_safe<telegraph::FeedbackMode>(*resonator_type, {telegraph::FeedbackMode::COS, telegraph::FeedbackMode::WRAP, telegraph::FeedbackMode::TANH, telegraph::FeedbackMode::CLIP, telegraph::FeedbackMode::LOWERED_BELL});

    // using telegraph::ModDestination;
    // for(size_t mod_dest_idx=0; mod_dest_idx<telegraph::Size<ModDestination>(); mod_dest_idx++){
    //     params.modulatable_params[mod_dest_idx] = 
    // }
    params.modulatable_params[static_cast<size_t>(ModDestination::EXCITER_GAIN)] = *exciter_gain;
    params.modulatable_params[static_cast<size_t>(ModDestination::EXCITER_FREQ)] = *exciter_pitch;
    params.modulatable_params[static_cast<size_t>(ModDestination::RESONATOR_Q)] = *resonator_q;
    params.modulatable_params[static_cast<size_t>(ModDestination::VIB_AMOUNT)] = *exciter_vibrato_amount;
    params.modulatable_params[static_cast<size_t>(ModDestination::RESONATOR_FREQ)] = *resonator_pitch;
    params.modulatable_params[static_cast<size_t>(ModDestination::CHAOS_CHARACTER)] = *resonator_chaos_character;
    params.modulatable_params[static_cast<size_t>(ModDestination::CHAOS_AMOUNT)] = *resonator_chaos_amount;
    params.modulatable_params[static_cast<size_t>(ModDestination::LOWPASS_CUTOFF)] = *lowpass_cutoff;
    params.modulatable_params[static_cast<size_t>(ModDestination::LOWPASS_Q)] = *lowpass_q;
    params.modulatable_params[static_cast<size_t>(ModDestination::HIGHPASS_CUTOFF)] = *highpass_cutoff;
    params.modulatable_params[static_cast<size_t>(ModDestination::DETUNE)] = *unison_detune;
    params.modulatable_params[static_cast<size_t>(ModDestination::STEREO_WIDTH)] = *stereo_width;
    params.modulatable_params[static_cast<size_t>(ModDestination::GAIN)] = *gain;

    params.amp_env_params.attack = telegraph::scale_parameter_from_norm_exp<float>(*attack, 4.0, 4800.0);
    params.amp_env_params.decay = telegraph::scale_parameter_from_norm_exp<float>(*decay, 4.0, 4800.0);
    params.amp_env_params.sustain = *sustain;
    params.amp_env_params.release = telegraph::scale_parameter_from_norm_exp<float>(*release, 4.0, 4800.0);

    params.mod_env_one_params.attack = telegraph::scale_parameter_from_norm_exp<float>(*env_one_attack, 4.0, 4800.0);
    params.mod_env_one_params.decay = telegraph::scale_parameter_from_norm_exp<float>(*env_one_decay, 4.0, 4800.0);
    params.mod_env_one_params.sustain = *env_one_sustain;
    params.mod_env_one_params.release = telegraph::scale_parameter_from_norm_exp<float>(*env_one_release, 4.0, 4800.0);
    
    params.mod_env_two_params.attack = telegraph::scale_parameter_from_norm_exp<float>(*env_two_attack, 4.0, 4800.0);
    params.mod_env_two_params.decay = telegraph::scale_parameter_from_norm_exp<float>(*env_two_decay, 4.0, 4800.0);
    params.mod_env_two_params.sustain = *env_one_sustain;
    params.mod_env_two_params.release = telegraph::scale_parameter_from_norm_exp<float>(*env_two_release, 4.0, 4800.0);
   
    params.modulatable_params[static_cast<size_t>(ModDestination::VIB_SPEED)]     = telegraph::scale_parameter_from_norm<float>(*exciter_vibrato_speed,0,8);
    params.modulatable_params[static_cast<size_t>(ModDestination::LFO_ONE_SPEED)] = telegraph::scale_parameter_from_norm<float>(*lfo_one_speed,0,8);
    params.modulatable_params[static_cast<size_t>(ModDestination::LFO_TWO_SPEED)] = telegraph::scale_parameter_from_norm<float>(*lfo_two_speed,0,8);

    params.unison = *unison;

    for(size_t mod_source_idx=0; mod_source_idx<telegraph::Size<telegraph::ModSource>(); mod_source_idx++){
        for(size_t mod_dest_idx=0; mod_dest_idx<telegraph::Size<telegraph::ModDestination>(); mod_dest_idx++){
            params.mod_matrix[mod_source_idx][mod_dest_idx] = *(mod_matrix[mod_source_idx][mod_dest_idx]);
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
    // return new GenericAudioProcessorEditor(*this);//new TelegraphAudioProcessorEditor (*this);
    return new TelegraphAudioProcessorEditor(*this, parameters);//new TelegraphAudioProcessorEditor (*this);
}

//==============================================================================
void TelegraphAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void TelegraphAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TelegraphAudioProcessor();
}
