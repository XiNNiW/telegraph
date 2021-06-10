/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


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
                                                    ));
    addParameter (exciter_vibrato_speed = new juce::AudioParameterFloat ("exciter_vib_speed", // parameterID
                                                    "Exciter Vibrato Speed", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.5f)); // default value
    addParameter (exciter_vibrato_amount = new juce::AudioParameterFloat ("exciter_vib_amount", // parameterID
                                                    "Exciter Vibrato Amount", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0)); // default value
    addParameter (exciter_waveform = new juce::AudioParameterChoice ("exciter_waveform", // parameterID
                                                    "Exciter Waveform", // parameter name
                                                    {"SINE", "SAW", "SQUARE"},
                                                    0  // default index
                                                    )); 
    addParameter (exciter_gain = new juce::AudioParameterFloat ("exciter_gain", // parameterID
                                                    "Exciter Gain", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.5f)); // default value
    addParameter (resonator_pitch = new juce::AudioParameterChoice ("resonator_pitch", // parameterID
                                                    "Resonator Pitch", // parameter name
                                                    {"-12", "-7", "0", "7", "+12"},
                                                    2  // default index
                                                    ));
    addParameter (resonator_q = new juce::AudioParameterFloat ("resonator_q", // parameterID
                                                    "Resonator Q", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.5f)); // default value
    addParameter (resonator_type = new juce::AudioParameterChoice ("resonator_type", // parameterID
                                                    "Resonator Chaos Type", // parameter name
                                                    {"COS", "TANH"},
                                                    1  // default index
                                                    )); 
    addParameter (resonator_chaos_character = new juce::AudioParameterFloat ("resonator_chaos_character", // parameterID
                                                    "Resonator Chaos Character", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    1.0f)); // default value
    addParameter (resonator_chaos_amount = new juce::AudioParameterFloat ("resonator_chaos_amount", // parameterID
                                                    "Resonator Chaos Amount", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.0f)); // default value
    addParameter (attack = new juce::AudioParameterFloat ("attack", // parameterID
                                                    "Attack", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.2f)); // default value
    addParameter (decay = new juce::AudioParameterFloat ("decay", // parameterID
                                                    "Decay", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.2f)); // default value
    addParameter (sustain = new juce::AudioParameterFloat ("sustain", // parameterID
                                                    "Sustain", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    1.0f)); // default value
    addParameter (release = new juce::AudioParameterFloat ("release", // parameterID
                                                    "Release", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.333f)); // default value
    addParameter (lowpass_cutoff = new juce::AudioParameterFloat ("lowpass_cutoff", // parameterID
                                                    "Lowpass Cutoff", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    1.0f)); // default value
    addParameter (lowpass_q = new juce::AudioParameterFloat ("lowpass_q", // parameterID
                                                    "Lowpass Q", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.2f)); // default value
    addParameter (highpass_cutoff = new juce::AudioParameterFloat ("highpass_cutoff", // parameterID
                                                    "Highpass Cutoff", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.0f)); // default value  
    addParameter (unison = new juce::AudioParameterInt ("unison_amt", // parameterID
                                            "Unison", // parameter name
                                            1,   // minimum value
                                            16,   // maximum value
                                            1)); // default value                                                                                          
    addParameter (stereo_width = new juce::AudioParameterFloat ("stereo_width", // parameterID
                                                "Stereo Width", // parameter name
                                                0.0f,   // minimum value
                                                1.0f,   // maximum value
                                                1.0f)); // default value
    addParameter (gain = new juce::AudioParameterFloat ("gain", // parameterID
                                                    "Gain", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.75f)); // default value
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
    for(int index=0; index<NUMBER_OF_VOICES; index++){
        voices[index] = telegraph::initVoice<SAMPLE, SAMPLE, MAX_UNISON>(voices[index], sampleRate);
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


    if(!midiMessages.isEmpty()){
        for (const MidiMessageMetadata it : midiMessages){
            MidiMessage m = it.getMessage();
            if (m.isNoteOn())
            {
                voices = telegraph::playNote<float, float, NUMBER_OF_VOICES>(voices, params, m.getChannel(), m.getNoteNumber(), m.getVelocity(),SR,CR);
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

    // for(size_t block_idx=0; block_idx < buffer.getNumSamples(); block_idx+=BLOCKSIZE){
    //     std::array<AudioBlock<SAMPLE,BLOCKSIZE>,2> block;
    //     std::tie(voices,block) = process<SAMPLE,SAMPLE,MAX_UNISON,NUMBER_OF_VOICES,WAVE_TABLE_SIZE,BLOCKSIZE>(voices,params,SR);
        
    //     for (size_t sample_index = block_idx; sample_index < block_idx+BLOCKSIZE; sample_index++)
    //     {
    //         if(sample_index<buffer.getNumSamples()){
    //             auto outL = buffer.getWritePointer(0, sample_index);
    //             auto outR = buffer.getWritePointer(1, sample_index);
    //             *outL += block[0][sample_index-block_idx]; 
    //             *outR += block[1][sample_index-block_idx]; 
    //         }
            
    //     }
    // }

    for(size_t samp_idx=0; samp_idx < buffer.getNumSamples(); samp_idx++){
        bool shouldUpdateControl = (samp_idx%BLOCKSIZE)==0;
        std::array<SAMPLE,2> block;
        std::tie(voices,block) = process<SAMPLE,SAMPLE,MAX_UNISON,NUMBER_OF_VOICES,WAVE_TABLE_SIZE,BLOCKSIZE>(voices,params,SR,shouldUpdateControl);
        auto outL = buffer.getWritePointer(0, samp_idx);
        auto outR = buffer.getWritePointer(1, samp_idx);
        *outL += block[0]; 
        *outR += block[1]; 
    }

}

void TelegraphAudioProcessor::updateSynthParams(){
    params.exciter_gain = telegraph::scale_parameter_as_db<SAMPLE>(exciter_gain->get());
    params.exciter_ratio = telegraph::lookup_safe<SAMPLE>(exciter_pitch->getIndex(),{0.5, 2.0/3.0, 1, 3.0/2.0, 2});
    params.wave_mode = telegraph::lookup_safe<telegraph::Wave>(exciter_waveform->getIndex(), {telegraph::Wave::SINE,telegraph::Wave::SAW,telegraph::Wave::SQUARE});
    params.resonator_q = telegraph::scale_parameter<SAMPLE>(telegraph::scale_parameter_as_db<SAMPLE>(resonator_q->get()),1.0, 100.0);
    params.vibrato_speed = telegraph::scale_parameter<SAMPLE>(exciter_vibrato_speed->get(),0,8);
    params.vibrato_depth = telegraph::scale_parameter<SAMPLE>(exciter_vibrato_amount->get(),0,2);
    // params.feedback_mode = telegraph::FeedbackMode::TANH;
    params.feedback_mode = telegraph::lookup_safe<telegraph::FeedbackMode>(resonator_type->getIndex(), {telegraph::FeedbackMode::COS,telegraph::FeedbackMode::TANH});
    
    params.resonator_chaos_character = resonator_chaos_character->get();
    auto character = params.resonator_chaos_character;
    params.resonator_feedback = telegraph::scale_parameter_exp<SAMPLE>(character,0.01,100);
    params.resonater_ratio = telegraph::lookup_safe<SAMPLE>(resonator_pitch->getIndex(),{0.5, 2.0/3.0, 1, 3.0/2.0, 2});

    params.resonator_chaos_character = telegraph::scale_parameter_exp<SAMPLE>(resonator_chaos_character->get(),0,100);
    params.resonator_chaos_amount = telegraph::scale_parameter_as_db<SAMPLE>(resonator_chaos_amount->get());
    params.amp_attack = telegraph::scale_parameter_exp<SAMPLE>(attack->get(), 4.0, 4800.0);
    params.amp_decay = telegraph::scale_parameter_exp<SAMPLE>(decay->get(), 4.0, 4800.0);
    params.amp_sustain = sustain->get();
    params.amp_release = telegraph::scale_parameter_exp<SAMPLE>(release->get(), 4.0, 4800.0);
    params.lowpass_filter_cutoff = telegraph::scale_parameter_exp<SAMPLE>(lowpass_cutoff->get(),200.0,20000.0);
    params.lowpass_filter_q = lowpass_q->get();
    params.highpass_filter_cutoff = telegraph::scale_parameter<SAMPLE>(highpass_cutoff->get(),30.0,1000.0);
    params.stereo_width = stereo_width->get();
    params.unison = unison->get();
    params.gain = telegraph::scale_parameter_as_db<SAMPLE>(gain->get());//SAMPLE(params.unison);
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
