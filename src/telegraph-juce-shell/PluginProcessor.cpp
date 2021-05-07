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
    addParameter (resonator_chaos_character = new juce::AudioParameterFloat ("resonator_chaos_character", // parameterID
                                                    "Resonator Chaos Character", // parameter name
                                                    0.0f,   // minimum value
                                                    1.0f,   // maximum value
                                                    0.0f)); // default value
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
                                                    0.2f)); // default value
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
                                                    0.001f)); // default value                                            
    addParameter (stereo_width = new juce::AudioParameterFloat ("stereo_width", // parameterID
                                                "Stereo Width", // parameter name
                                                0.0f,   // minimum value
                                                1.0f,   // maximum value
                                                0.75f)); // default value
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
        voices[index] = telegraph::initVoice<double,double>(voices[index], sampleRate);
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
    const double SR = this->getSampleRate();
    const int controlRateDivisor = 32;
    const double CR = SR/32;
    

    if(!midiMessages.isEmpty()){
        for (const MidiMessageMetadata it : midiMessages){
            MidiMessage m = it.getMessage();
            if (m.isNoteOn())
            {
                voices = telegraph::playNote<double, double, NUMBER_OF_VOICES>(voices, params, m.getChannel(), m.getNoteNumber(), m.getVelocity(),SR,CR);
            }
            else if (m.isNoteOff())
            {
                voices = telegraph::releaseNote<double, double, NUMBER_OF_VOICES>(voices, params, m.getChannel(), m.getNoteNumber(), m.getVelocity(),SR);
                
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
    // double phase_inc = 2*M_PI*440.0/SR;
    
    for (int sample_index = 0; sample_index < buffer.getNumSamples(); sample_index++)
    {
        auto outL = buffer.getWritePointer(0, sample_index);
        auto outR = buffer.getWritePointer(1, sample_index);


        for(int voiceIndex=0; voiceIndex<NUMBER_OF_VOICES; voiceIndex++){
    
        // ..do something to the data...
        
            
            //if voice is active
            if(telegraph::isActive<double,double>(voices[voiceIndex])){
                voices[voiceIndex] = telegraph::process<double, double, WAVE_TABLE_SIZE>(voices[voiceIndex], params, SR);
                
                if(sample_index%controlRateDivisor==0){
                    updateSynthParams();
                    voices[voiceIndex] = telegraph::process_control<double,double>(voices[voiceIndex], params, SR, CR);
                }
                *outL += voices[voiceIndex].out_left; 
                *outR += voices[voiceIndex].out_right; 

                
                
            }


        }

    }

}

void TelegraphAudioProcessor::updateSynthParams(){
    params.exciter_gain = telegraph::scale_parameter_as_db<float>(exciter_gain->get());
    params.wave_mode = telegraph::scale_parameter_from_set<int, telegraph::Wave>(exciter_waveform->getIndex(),{telegraph::Wave::SINE,telegraph::Wave::SAW,telegraph::Wave::SQUARE});
    params.resonator_q = telegraph::scale_parameter<float>(telegraph::scale_parameter_as_db<float>(resonator_q->get()),1.0, 100.0);
    params.resonator_chaos_character = resonator_chaos_character->get();
   
    params.resonator_chaos_character = telegraph::scale_parameter_exp<float>(resonator_chaos_character->get(),0,100);
    params.resonator_chaos_amount = telegraph::scale_parameter_as_db<float>(resonator_chaos_amount->get());
    params.amp_attack = telegraph::scale_parameter_exp<float>(attack->get(), 4.0, 4800.0);
    params.amp_decay = telegraph::scale_parameter_exp<float>(decay->get(), 4.0, 4800.0);
    params.amp_sustain = sustain->get();
    params.amp_release = telegraph::scale_parameter_exp<float>(release->get(), 4.0, 4800.0);
    params.lowpass_filter_cutoff = telegraph::scale_parameter_exp<float>(lowpass_cutoff->get(),200.0,20000.0);
    params.lowpass_filter_q = lowpass_q->get();
    // params.highpass_filter_cutoff = telegraph::scale_parameter<float>(highpass_cutoff->get(),30.0,1000.0);
    params.stereo_width = stereo_width->get();
    params.gain = telegraph::scale_parameter_as_db<float>(gain->get());
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