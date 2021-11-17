#pragma once
#include <algae.h>
#include "parameters.h"
using algae::dsp::shell::dsp_node;
using algae::dsp::core::AudioBlock;
using algae::dsp::core::StereoBlock;
using algae::dsp::core::filter::process;
using algae::dsp::core::filter::chaotic_resonator_t;
using algae::dsp::core::filter::onepole_onezero_t;
using algae::dsp::core::filter::hip;
using algae::dsp::core::filter::update_coefficients;
using algae::dsp::core::filter::biquad_t;
using algae::dsp::core::filter::lowpass;
using algae::dsp::core::filter::reson_bp_t;
using algae::dsp::core::filter::reson_bp;
using algae::dsp::core::filter::dc_block_t;
using algae::dsp::core::control::ramp_t;
using algae::dsp::core::control::adsr_t;
using algae::dsp::core::control::adsr_params_t;
using algae::dsp::core::control::update_adsr;
using algae::dsp::core::control::update_ad;
using algae::dsp::core::oscillator::noise;
using algae::dsp::core::oscillator::update_phase;
using algae::dsp::core::oscillator::stk_blit_saw_t;
using algae::dsp::core::oscillator::stk_blit_saw;
using algae::dsp::core::oscillator::stk_blit_square_t;
using algae::dsp::core::oscillator::stk_blit_square;
using algae::dsp::core::oscillator::setFrequency;
using algae::dsp::core::oscillator::process;
using algae::dsp::core::oscillator::sineOsc;
using algae::dsp::core::math::tanh_approx_pade;
using algae::dsp::core::math::lowered_bell;
using algae::dsp::core::math::clip;
using algae::dsp::core::math::lerp;
using algae::dsp::core::oscillator::sine_t;
using algae::dsp::core::oscillator::cos_t;
using algae::dsp::core::units::mtof;
using algae::dsp::core::units::dbtorms;

#define TWO_PI  2*M_PI 

namespace telegraph {

    template<typename sample_t, typename frequency_t, size_t MAX_UNISON>
    struct alignas(16) voice_t {

        std::array<chaotic_resonator_t<sample_t>, MAX_UNISON> resonator alignas(16);

        //union {
            std::array<stk_blit_saw_t<sample_t>, MAX_UNISON> sawtooth alignas(16)=std::array<stk_blit_saw_t<sample_t>, MAX_UNISON>();
            std::array<stk_blit_square_t<sample_t>, MAX_UNISON> square alignas(16)=std::array<stk_blit_square_t<sample_t>, MAX_UNISON>();
            std::array<sample_t, MAX_UNISON> phase alignas(16)=std::array<sample_t, MAX_UNISON>();
            std::array<sample_t, MAX_UNISON> phi alignas(16)=std::array<sample_t, MAX_UNISON>();
        //};
        std::array<sample_t, Size<ModDestination>()> parameter_values;
        std::array<onepole_onezero_t<sample_t>, 2> highpass_state alignas(16);
        std::array<biquad_t<sample_t>, 2> lowpass_state alignas(16);
        modulators_t<sample_t> modulators = modulators_t<sample_t>();
        // frequency_t frequency alignas(16);        //audio
        sample_t resonator_frequency alignas(16);
        int note alignas(16) = 60;
        bool amp_gate alignas(16)=false;
    };

    template<typename sample_t, typename frequency_t, int MAX_UNISON>
    const inline voice_t<sample_t, frequency_t, MAX_UNISON> initVoice(voice_t<sample_t, frequency_t, MAX_UNISON> v, sample_t sampleRate){
        for(size_t unison_idx=0; unison_idx<MAX_UNISON; unison_idx++){
            v.sawtooth[unison_idx] = stk_blit_saw<sample_t,frequency_t>(440.0, sampleRate);
            v.square[unison_idx] = stk_blit_square<sample_t,frequency_t>(440.0, sampleRate);
            v.phase[unison_idx] = 0;
            v.phi[unison_idx] = 0;
        }
        
        return v;
    }

    template<typename sample_t, typename frequency_t, int MAX_UNISON>
    const inline bool isActive(const voice_t<sample_t,frequency_t, MAX_UNISON>& v){
        bool isActive = false;
        if((!v.amp_gate) && (v.modulators.amp_envelope.env.value<0.0001) ){
            isActive = false;
        } else {
            isActive = true;
        }

        return isActive;
    }

    template<typename sample_t, typename frequency_t> 
    const inline chaotic_resonator_t<sample_t> update_resonator(
        chaotic_resonator_t<sample_t> r, 
        const frequency_t& freq,
        const sample_t& q, 
        const sample_t& chaos_character, 
        const sample_t& chaos_amount, 
        const frequency_t& sampleRate
    ){  

        r.resonator = update_coefficients<sample_t,frequency_t>(r.resonator, freq, q, 0.5, sampleRate);
        r.feedback_amt = chaos_character;
        r.chaos_gain = chaos_amount;
        
        return r;
    }

    template<typename sample_t, typename frequency_t, size_t MAX_UNISON>
    const inline modulators_t<sample_t> process_modulators(voice_t<sample_t, frequency_t, MAX_UNISON> v, params_t<sample_t> p, const frequency_t& controlRate) {
        v.modulators.amp_envelope = adsr_t<sample_t>::process(v.modulators.amp_envelope, v.amp_gate, p.amp_env_params);
        v.modulators.mod_envelope_1 = adsr_t<sample_t>::process(v.modulators.mod_envelope_1, v.amp_gate, p.mod_env_one_params);
        v.modulators.mod_envelope_2 = adsr_t<sample_t>::process(v.modulators.mod_envelope_2, v.amp_gate, p.mod_env_two_params);
        v.modulators.vib_lfo = lfo_t<sample_t>::process(v.modulators.vib_lfo, v.parameter_values[static_cast<size_t>(ModDestination::VIB_SPEED)], controlRate);
        v.modulators.mod_lfo_1 = lfo_t<sample_t>::process(v.modulators.mod_lfo_1, v.parameter_values[static_cast<size_t>(ModDestination::LFO_ONE_SPEED)], controlRate);
        v.modulators.mod_lfo_2 = lfo_t<sample_t>::process(v.modulators.mod_lfo_2, v.parameter_values[static_cast<size_t>(ModDestination::LFO_TWO_SPEED)], controlRate);
        
        return v.modulators;
    }

    template<typename sample_t, typename frequency_t, size_t MAX_UNISON>
    std::array<sample_t, Size<ModDestination>()> modulate_params(const voice_t<sample_t, frequency_t, MAX_UNISON>& v, const params_t<sample_t>& p){
        std::array<sample_t, Size<ModDestination>()> modulated_values;

        for(size_t param_idx=0; param_idx<Size<ModDestination>(); param_idx++){
            
            const auto [ min, max, defaultValue, scalingType ] = getParameterRange<sample_t>(ModDestination(param_idx));
            sample_t val;

            switch (ModDestination(param_idx))
            {
            case ModDestination::EXCITER_FREQ:
                val = v.note + p.modulatable_params[param_idx] + p.modulatable_params[static_cast<size_t>(ModDestination::VIB_AMOUNT)]*sine_t<sample_t,0>::lookup(v.modulators.vib_lfo.phase);
                break;
            case ModDestination::RESONATOR_FREQ:
                val = v.note + p.modulatable_params[param_idx] + p.modulatable_params[static_cast<size_t>(ModDestination::VIB_AMOUNT)]*sine_t<sample_t,0>::lookup(v.modulators.vib_lfo.phase);
                break;
            default:
                val = p.modulatable_params[param_idx];
                break;
            }

            val = getModulatedValue<sample_t>(val, ModDestination(param_idx), v.modulators, p.mod_matrix);

            switch (scalingType)
            {
            case ScalingType::DB:
                modulated_values[param_idx] = dbtorms(val);
                break;
            case ScalingType::SEMITONES:
                modulated_values[param_idx] = mtof(val);
                break;
            default:
                modulated_values[param_idx] = val;
                break;
            }
            

        }

        return modulated_values;
    }

    template<typename sample_t, typename frequency_t, size_t MAX_UNISON> 
    const inline voice_t<sample_t, frequency_t, MAX_UNISON> process_control(voice_t<sample_t,frequency_t, MAX_UNISON> v, const params_t<sample_t>& p, const frequency_t& sampleRate, const frequency_t& controlRate){
 
        v.parameter_values = modulate_params<sample_t,frequency_t,MAX_UNISON>(v,p);

        frequency_t resonator_frequency = v.parameter_values[static_cast<size_t>(ModDestination::RESONATOR_FREQ)];

        frequency_t exciter_freq = v.parameter_values[static_cast<size_t>(ModDestination::EXCITER_FREQ)];

        
        for(size_t unison_idx=0; unison_idx<MAX_UNISON; unison_idx++){
            const sample_t sign = (unison_idx%2)>0 ? -1 : 1;
            const sample_t max_detune = (mtof(v.note+sign)-mtof(v.note));
            const sample_t detune_amt = max_detune*v.parameter_values[static_cast<size_t>(ModDestination::DETUNE)]*sample_t(unison_idx)/sample_t(MAX_UNISON);
            const sample_t pitch_mod = detune_amt;

            switch (p.wave_mode) {
                case Wave::SAW: v.sawtooth[unison_idx] = setFrequency<sample_t, frequency_t>(v.sawtooth[unison_idx], exciter_freq+pitch_mod, sampleRate); break;
                case Wave::SQUARE: v.square[unison_idx] = setFrequency<sample_t, frequency_t>(v.square[unison_idx], exciter_freq+pitch_mod, sampleRate); break;
                default: v.phi[unison_idx] = (exciter_freq+pitch_mod)/sampleRate; break;
            }

            auto f = resonator_frequency+pitch_mod;
            f = f>0?f:1;
            const auto q = v.parameter_values[static_cast<size_t>(ModDestination::RESONATOR_Q)];
            const auto c = v.parameter_values[static_cast<size_t>(ModDestination::CHAOS_CHARACTER)];
            const auto a = v.parameter_values[static_cast<size_t>(ModDestination::CHAOS_AMOUNT)];
            v.resonator[unison_idx] = update_resonator<sample_t, frequency_t>(v.resonator[unison_idx], f, q, c, a, sampleRate);

        }

        for(size_t stereo_idx=0; stereo_idx<2; stereo_idx++){
            v.lowpass_state[stereo_idx] = lowpass<sample_t,frequency_t>(v.lowpass_state[stereo_idx], v.parameter_values[static_cast<size_t>(ModDestination::LOWPASS_CUTOFF)], v.parameter_values[static_cast<size_t>(ModDestination::LOWPASS_Q)], sampleRate);
            v.highpass_state[stereo_idx] = hip(v.highpass_state[stereo_idx],v.parameter_values[static_cast<size_t>(ModDestination::HIGHPASS_CUTOFF)], sampleRate);
        }
        v.modulators = process_modulators<sample_t, frequency_t, MAX_UNISON>(v, p, controlRate);
    
        return v;
    }

    template<typename sample_t, typename frequency_t, size_t TABLE_SIZE, size_t BLOCK_SIZE, size_t MAX_UNISON>
    const inline std::array<AudioBlock<sample_t, BLOCK_SIZE>,2> pan_unison(const AudioBlock<sample_t, BLOCK_SIZE>& block, const voice_t<sample_t, frequency_t, MAX_UNISON>& v, const params_t<sample_t>& p, const size_t& unison_index){

        // const sample_t one_minus_width = (1-v.parameter_values[static_cast<size_t>(ModDestination::STEREO_WIDTH)]);
        const sample_t center = 0.125;
        sample_t power_left =  p.unison>1?0.25*sample_t(unison_index)/sample_t(p.unison-1):center;

        power_left = lerp<sample_t>(power_left,center,0);

        sample_t power_right = power_left + 0.75;
        return {
            block*cos_t<sample_t, TABLE_SIZE>::lookup(power_left),
            block*cos_t<sample_t, TABLE_SIZE>::lookup(power_right)
        };
    }

    template<typename sample_t, typename frequency_t, size_t TABLE_SIZE, size_t BLOCK_SIZE, size_t MAX_UNISON>
    const inline std::pair<voice_t<sample_t,frequency_t, MAX_UNISON>,std::array<AudioBlock<sample_t, BLOCK_SIZE>, 2>> process(
        voice_t<sample_t,frequency_t, MAX_UNISON> v, 
        const params_t<sample_t>& p, 
        const frequency_t& sampleRate
    ){
        const sample_t amp_env_value = v.modulators.amp_envelope.env.value;
        const sample_t sq_amp_env_value = amp_env_value*amp_env_value;
      
        StereoBlock<sample_t, BLOCK_SIZE> output = {
            AudioBlock<sample_t, BLOCK_SIZE>::empty(),
            AudioBlock<sample_t, BLOCK_SIZE>::empty()
        };

        for(size_t unison_idx=0; unison_idx < p.unison; unison_idx++){
            AudioBlock<sample_t, BLOCK_SIZE> exciter;
            AudioBlock<sample_t, BLOCK_SIZE> block;

            switch(p.wave_mode){
                case Wave::SAW:  std::tie(v.sawtooth[unison_idx], exciter) = process<sample_t, BLOCK_SIZE>(v.sawtooth[unison_idx]); break;
                case Wave::SQUARE: std::tie(v.square[unison_idx], exciter) = process<sample_t, BLOCK_SIZE>(v.square[unison_idx]); break;
                default: std::tie(v.phase[unison_idx], exciter) = sineOsc<sample_t, TABLE_SIZE, BLOCK_SIZE>::process(v.phase[unison_idx], v.phi[unison_idx]); break;
            }

            exciter *= v.parameter_values[static_cast<size_t>(ModDestination::EXCITER_GAIN)];
            exciter *= amp_env_value;
            block = exciter;

            switch(p.feedback_mode){
                case FeedbackMode::TANH: std::tie(v.resonator[unison_idx],block) = process<sample_t, tanh_approx_pade<sample_t>, BLOCK_SIZE>(v.resonator[unison_idx],exciter);break;
                // case FeedbackMode::LOWERED_BELL: std::tie(v.resonator[unison_idx],block) = process<sample_t, lowered_bell<sample_t>, BLOCK_SIZE>(v.resonator[unison_idx],exciter);break;
                case FeedbackMode::WRAP: std::tie(v.resonator[unison_idx],block) = process<sample_t, algae::dsp::core::math::wrap<sample_t>, BLOCK_SIZE>(v.resonator[unison_idx],exciter);break;
                // case FeedbackMode::CLIP: std::tie(v.resonator[unison_idx],block) = process<sample_t, clip<sample_t>, BLOCK_SIZE>(v.resonator[unison_idx],exciter);break;
                case FeedbackMode::COS: std::tie(v.resonator[unison_idx],block) = process<sample_t, cos_t<sample_t,TABLE_SIZE>::lookup, BLOCK_SIZE>(v.resonator[unison_idx],exciter);break;
                //default: std::tie(v.resonator[unison_idx],block) = process<sample_t, cos_t<sample_t,TABLE_SIZE>::lookup, BLOCK_SIZE>(v.resonator[unison_idx],exciter);break;
            }

            block *= exciter*(-1) + 1;

            StereoBlock<sample_t, BLOCK_SIZE> panned_block = pan_unison<sample_t, frequency_t, TABLE_SIZE, BLOCK_SIZE, MAX_UNISON>(block, v, p, unison_idx);
            
            output[0] += panned_block[0];
            output[1] += panned_block[1];
        }


        for(size_t stereo_idx=0; stereo_idx<2; stereo_idx++){

            output[stereo_idx] *= v.modulators.velocity*1.2;
 
            output[stereo_idx] = tanh_approx_pade<sample_t>(output[stereo_idx]);
            
            output[stereo_idx] *= /*0.25**/v.parameter_values[static_cast<size_t>(ModDestination::GAIN)];

            std::tie(v.highpass_state[stereo_idx], output[stereo_idx]) = process<sample_t>(v.highpass_state[stereo_idx], output[stereo_idx]);
            
            std::tie(v.lowpass_state[stereo_idx], output[stereo_idx]) = process<sample_t>(v.lowpass_state[stereo_idx], output[stereo_idx]);

            output[stereo_idx] *= sq_amp_env_value;
        }

        return std::pair(v, output);
        
    }

    template<typename sample_t, typename frequency_t, size_t MAX_UNISON, size_t VOICES, size_t WAVE_TABLE_SIZE, size_t BLOCK_SIZE>
    const inline std::pair<std::array<voice_t<sample_t,frequency_t,MAX_UNISON>, VOICES>, StereoBlock<sample_t,BLOCK_SIZE>> process(
        std::array<voice_t<sample_t,frequency_t,MAX_UNISON>,VOICES> voices, 
        const params_t<sample_t>& params, 
        const frequency_t& sampleRate
    ){

        const frequency_t SR = sampleRate;
        const frequency_t CR = SR/BLOCK_SIZE;
        
        StereoBlock<sample_t,BLOCK_SIZE> block = {
            AudioBlock<sample_t,BLOCK_SIZE>::empty(),
            AudioBlock<sample_t,BLOCK_SIZE>::empty()
        };

        for(size_t voiceIndex=0; voiceIndex<VOICES; voiceIndex++){

            if(telegraph::isActive<sample_t, frequency_t,MAX_UNISON>(voices[voiceIndex])){
                voices[voiceIndex] = process_control<sample_t, frequency_t>(voices[voiceIndex], params, SR, CR);
                StereoBlock<sample_t, BLOCK_SIZE> samples;
                std::tie(voices[voiceIndex], samples) = process<sample_t, frequency_t, WAVE_TABLE_SIZE, BLOCK_SIZE>(voices[voiceIndex], params, SR);
                block[0] +=samples[0];
                block[1] +=samples[1];
            }

        }

        return std::pair(voices, block);
    }


    template<typename sample_t, typename frequency_t, size_t MAX_UNISON> 
    const inline voice_t<sample_t,frequency_t,MAX_UNISON> noteOn(voice_t<sample_t,frequency_t,MAX_UNISON> v, const params_t<sample_t>& p, const sample_t& note, const sample_t& velocity, const frequency_t& sampleRate, const frequency_t& controlRate){
        v.amp_gate = true;
        v.modulators.amp_envelope = adsr_t<sample_t>();
        v.modulators.mod_envelope_1 = adsr_t<sample_t>();
        v.modulators.mod_envelope_2 = adsr_t<sample_t>();
        
        v.note = note;
        v.modulators.velocity = velocity/127.0;

        return process_control(v, p, sampleRate, controlRate);
    }

    template<typename sample_t, typename frequency_t, size_t MAX_UNISON>
    const inline voice_t<sample_t, frequency_t, MAX_UNISON> noteOff(voice_t<sample_t, frequency_t, MAX_UNISON> v){
        
        v.amp_gate = false;
        return v;
    }

    template<typename sample_t, typename frequency_t, size_t NUM_VOICES, size_t MAX_UNISON>
    const inline std::array<voice_t<sample_t,frequency_t,MAX_UNISON>, NUM_VOICES> playNote(
        std::array<voice_t<sample_t,frequency_t,MAX_UNISON>, NUM_VOICES> voices,
        const params_t<sample_t>& params,
        const int& channel,
        const int& noteNumber,
        const int& velocity,
        const frequency_t& sampleRate,
        const frequency_t& controlRate
    ){
        size_t indexOfQuietestVoice = 0;
        sample_t minValue = 10000;
        for(size_t index=0; index<NUM_VOICES; index++){
            sample_t value = voices[index].modulators.amp_envelope.env.value;
            if (!isActive<sample_t,frequency_t,MAX_UNISON>(voices[index])){
                indexOfQuietestVoice = index;
                break;
            }
            if(value<minValue){
                minValue = value;
                indexOfQuietestVoice = index;
            }
        }

        voices[indexOfQuietestVoice] = noteOn<sample_t,frequency_t>(voices[indexOfQuietestVoice], params, noteNumber, velocity, sampleRate, controlRate);

        return voices;
    }

    template<typename sample_t, typename frequency_t, size_t NUM_VOICES, size_t MAX_UNISON>
    const inline std::array<voice_t<sample_t,frequency_t, MAX_UNISON>, NUM_VOICES> releaseNote(
        std::array<voice_t<sample_t,frequency_t, MAX_UNISON>, NUM_VOICES> voices,
        const params_t<sample_t>& params,
        const int& channel,
        const int& noteNumber,
        const int& velocity,
        const frequency_t& sampleRate
    ){
        for(size_t index=0; index<NUM_VOICES; index++){
            if(isActive<sample_t,frequency_t,MAX_UNISON>(voices[index]) && voices[index].note==noteNumber){
                voices[index] = noteOff<sample_t,frequency_t>(voices[index]);
            }
        }
        return voices;
    }



}
