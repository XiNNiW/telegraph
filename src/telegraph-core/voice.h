#include <algae.h>
using algae::dsp::shell::dsp_node;
using algae::dsp::core::AudioBlock;
using algae::dsp::core::StereoBlock;
using algae::dsp::core::filter::process;
using algae::dsp::core::filter::process_tanh;
using algae::dsp::core::filter::chaotic_resonator_t;
using algae::dsp::core::filter::onepole_hip_t;
using algae::dsp::core::filter::hip;
using algae::dsp::core::filter::update_coefficients;
using algae::dsp::core::filter::biquad_t;
using algae::dsp::core::filter::lowpass;
using algae::dsp::core::filter::reson_bp_t;
using algae::dsp::core::filter::reson_bp;
using algae::dsp::core::filter::dc_block_t;
using algae::dsp::core::control::ramp_t;
using algae::dsp::core::control::adsr_t;
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
using algae::dsp::core::oscillator::sine_t;
using algae::dsp::core::oscillator::cos_t;
using algae::dsp::core::units::mtof;

#define TWO_PI  2*M_PI 

namespace telegraph{

    template<typename sample_t>
    sample_t scale_parameter(sample_t val, sample_t min, sample_t max){
        
        val = val>=0.0 && val<=1.0 ? val : 0.0;
        return val*(max-min) + min;
    }

    template<typename sample_t>
    sample_t scale_parameter_exp(sample_t val, sample_t min, sample_t max){
        auto m = max<=0?0.0001:log(max);
        auto n = min<=0?0.0001:log(min);
        
        return exp(scale_parameter<sample_t>(val,n,m));
    }

    template<typename sample_t>
    sample_t scale_parameter_as_db(sample_t val){
        if (val<=0) return 0;
        
        val = scale_parameter<sample_t>(val, -48.0, 0.0);
        return pow(10.0, val/20.0);
    }

    template<typename sample_t, int TABLE_SIZE>
    sample_t scale_parameter_from_set(sample_t val,  const sample_t (&options) [TABLE_SIZE]){
        
        val = val>=0 && val<=1 ? val : 0;
        return options[size_t(floor(val*(TABLE_SIZE-1)))];
    }

    template<typename input_t, typename output_t, int TABLE_SIZE>
    output_t scale_parameter_from_set(input_t val,  const output_t (&options) [TABLE_SIZE]){
        val = (val>=0) && (val<=1) ? val : 0;
        return options[size_t(ceil(val*(TABLE_SIZE-1)))];
    }
    template<typename output_t, int TABLE_SIZE>
    output_t lookup_safe(size_t val,  const output_t (&options) [TABLE_SIZE]){
        val = (val>=0) && (val<=(TABLE_SIZE-1)) ? val : 0;
        return options[val];
    }

    enum Wave {
        SINE,
        SAW,
        SQUARE,
        TRI
    };

    enum FeedbackMode {
        COS,
        TANH
    };
    
    template<typename sample_t, typename frequency_t, size_t MAX_UNISON>
    struct alignas(16) voice_t {
        std::array<chaotic_resonator_t<sample_t>,MAX_UNISON> resonator alignas(16);
        //union {
            std::array<stk_blit_saw_t<sample_t>,MAX_UNISON> sawtooth alignas(16);
            std::array<stk_blit_square_t<sample_t>,MAX_UNISON> square alignas(16);
            std::array<sample_t,MAX_UNISON> phase alignas(16);
        //};
        std::array<sample_t,MAX_UNISON> phi alignas(16);
        onepole_hip_t<sample_t> highpass alignas(16);
        biquad_t<sample_t> lowpass alignas(16);

        ramp_t<sample_t> exciter_envelope alignas(16);
        adsr_t<sample_t> amp_envelope alignas(16);
        frequency_t frequency alignas(16);
        sample_t lfo_phase alignas(16);
        sample_t lfo_value alignas(16);
        //audio
        sample_t resonator_frequency alignas(16);
        int note alignas(16);
        bool amp_gate alignas(16)=false;
    };

    template<typename sample_t>
    struct params_t {
        size_t unison = 2;
        sample_t exciter_gain=1.0;
        Wave wave_mode=SINE;
        FeedbackMode feedback_mode=COS;
        sample_t resonator_q=1.0;
        sample_t resonator_feedback=0.01;
        sample_t resonator_cross_feedback=0.01;
        sample_t resonator_chaos_character=0.01;
        sample_t resonator_chaos_amount=0;
        sample_t detune=0.01;
        int amp_attack=48;
        int amp_decay=4800;
        sample_t amp_sustain=0.9;
        int amp_release=2400;
        sample_t exciter_ratio=1;
        sample_t resonater_ratio=1;
        sample_t vibrato_speed=0.0;
        sample_t vibrato_depth=0.0;
        sample_t gain=0.5;
        sample_t lowpass_filter_cutoff = 20000.0;
        sample_t lowpass_filter_q = 0.05;
        sample_t highpass_filter_cutoff = 30.0;
        sample_t stereo_width = 1;
    };

    template<typename sample_t, typename frequency_t, int MAX_UNISON>
    const inline voice_t<sample_t, frequency_t, MAX_UNISON> initVoice(voice_t<sample_t, frequency_t, MAX_UNISON> v, sample_t sampleRate){
        for(size_t unison_idx=0; unison_idx<MAX_UNISON; unison_idx++){
            v.sawtooth[unison_idx] = stk_blit_saw<sample_t,frequency_t>(440.0, sampleRate);
            v.square[unison_idx] = stk_blit_square<sample_t,frequency_t>(440.0, sampleRate);
        }
        
        return v;
    }

    template<typename sample_t, typename frequency_t, int MAX_UNISON>
    const inline voice_t<sample_t, frequency_t, MAX_UNISON> resetVoice(voice_t<sample_t,frequency_t, MAX_UNISON> v){
        v.amp_gate = false;
        v.exciter_envelope = ramp_t<sample_t>();
        v.amp_envelope = adsr_t<sample_t>();
        return v;
    }

    template<typename sample_t, typename frequency_t, int MAX_UNISON>
    const inline bool isActive(const voice_t<sample_t,frequency_t, MAX_UNISON>& v){
        bool isActive = false;
        if((!v.amp_gate) && (v.amp_envelope.env.value<0.0001) ){
            isActive = false;
        } else {
            isActive = true;
        }

        return isActive;
    }

    template<typename sample_t>
    const inline sample_t lerp(sample_t origin, sample_t dest, sample_t amt){
        amt = (amt>1)?1:(amt<0)?0:amt;
        return origin + amt*(dest-origin);
    }

    template<typename sample_t, typename frequency_t, size_t MAX_UNISON> 
    const inline voice_t<sample_t, frequency_t, MAX_UNISON> process_control(voice_t<sample_t,frequency_t, MAX_UNISON> v, const params_t<sample_t>& p, const frequency_t& sampleRate, const frequency_t& controlRate){
        
        v.lfo_phase = update_phase<sample_t,frequency_t>(v.lfo_phase,p.vibrato_speed/controlRate,1);
        v.lfo_value = v.amp_envelope.env.value*(mtof<frequency_t>(1)*p.vibrato_depth)*sine_t<sample_t, 1024>::lookup(v.lfo_phase);
        v.lfo_phase += (0.99999-v.amp_envelope.env.value)*v.lfo_value;

        frequency_t resonator_frequency = v.frequency*p.resonater_ratio;
        resonator_frequency = resonator_frequency==0?1:resonator_frequency;
        frequency_t exciter_freq = v.frequency*p.exciter_ratio;
        
        for(size_t unison_idx=0; unison_idx<MAX_UNISON; unison_idx++){
            const sample_t sign = (unison_idx%2)>0 ? -1 : 1;
            const sample_t detune_amt = sign*p.detune*sample_t(unison_idx)/sample_t(MAX_UNISON);
            const sample_t pitch_mod = v.lfo_value+detune_amt;
            switch(p.wave_mode){
                case SAW: v.sawtooth[unison_idx] = setFrequency<sample_t, frequency_t>(v.sawtooth[unison_idx], exciter_freq+pitch_mod, sampleRate); break;
                case SQUARE: v.square[unison_idx] = setFrequency<sample_t, frequency_t>(v.square[unison_idx], exciter_freq+pitch_mod, sampleRate); break;
                default: v.phi[unison_idx] = (exciter_freq+pitch_mod)/sampleRate; break;
            }
            v.resonator[unison_idx].resonator = update_coefficients<sample_t,frequency_t>(v.resonator[unison_idx].resonator, resonator_frequency+pitch_mod, p.resonator_q, 0.5, sampleRate);
            v.resonator[unison_idx].feedback_amt = p.resonator_chaos_character; 
            v.resonator[unison_idx].chaos_gain = p.resonator_chaos_amount; 
        }
        v.lowpass = lowpass<sample_t,frequency_t>(v.lowpass, p.lowpass_filter_cutoff, p.lowpass_filter_q, sampleRate);
        v.highpass = hip(v.highpass, p.highpass_filter_cutoff, sampleRate);
        

        v.amp_envelope = update_adsr<sample_t>(v.amp_envelope, v.amp_gate, p.amp_attack, p.amp_decay, p.amp_sustain, p.amp_release);
    
        return v;
    }

    template<typename sample_t, size_t TABLE_SIZE, size_t BLOCK_SIZE, size_t MAX_UNISON>
    const inline std::array<AudioBlock<sample_t, BLOCK_SIZE>,2> pan_unison_block(const AudioBlock<sample_t, BLOCK_SIZE>& block, const params_t<sample_t>& p, const size_t& unison_index){
        StereoBlock<sample_t,BLOCK_SIZE> output = {
            AudioBlock<sample_t, BLOCK_SIZE>::empty(),
            AudioBlock<sample_t, BLOCK_SIZE>::empty()
        };

        const sample_t one_minus_width = (1-p.stereo_width);
        const sample_t center = 0.125;
        sample_t power_left =  p.unison>1?0.25*sample_t(unison_index)/sample_t(p.unison-1):center;
        // sample_t power_left =  center;
        power_left = lerp(power_left,center,one_minus_width);

        sample_t power_right = power_left + 0.75;
        output[0] = block*cos_t<sample_t,TABLE_SIZE>::lookup(power_left);
        output[1] = block*cos_t<sample_t,TABLE_SIZE>::lookup(power_right);
        return output;
    }


    template<typename sample_t, typename frequency_t, size_t TABLE_SIZE, size_t BLOCK_SIZE, size_t MAX_UNISON>
    const inline std::pair<voice_t<sample_t,frequency_t, MAX_UNISON>,std::array<AudioBlock<sample_t, BLOCK_SIZE>, 2>> process(
        voice_t<sample_t,frequency_t, MAX_UNISON> v, 
        const params_t<sample_t>& p, 
        const frequency_t& sampleRate
    ){
       
        StereoBlock<sample_t,BLOCK_SIZE> output = {
            AudioBlock<sample_t, BLOCK_SIZE>::empty(),
            AudioBlock<sample_t, BLOCK_SIZE>::empty()
        };

        for(size_t unison_idx=0; unison_idx<p.unison; unison_idx++){
            AudioBlock<sample_t, BLOCK_SIZE> exciter;
            AudioBlock<sample_t, BLOCK_SIZE> block;

            switch(p.wave_mode){
                case SAW:  std::tie(v.sawtooth[unison_idx], exciter) = process<sample_t, BLOCK_SIZE>(v.sawtooth[unison_idx]); break;
                case SQUARE: std::tie(v.square[unison_idx], exciter) = process<sample_t, BLOCK_SIZE>(v.square[unison_idx]); break;
                default: std::tie(v.phase[unison_idx], exciter) = sineOsc<sample_t, TABLE_SIZE, BLOCK_SIZE>::process(v.phase[unison_idx], v.phi[unison_idx]); break;
            }

            exciter *= p.exciter_gain;
            exciter = exciter*v.amp_envelope.env.value;
            block = exciter;
            // sample_t fb1 = p.resonator_feedback;

            // for(size_t idx=0;idx<BLOCK_SIZE;idx++){
            //     sample_t input;
            //     switch(p.feedback_mode){
            //         case TANH: input = tanh_approx_pade<sample_t>(-fb1*v.resonator[unison_idx].resonator.y1);
            //         default: input = cos_t<sample_t, TABLE_SIZE>::lookup(-fb1*v.resonator[unison_idx].resonator.y1);
            //     }
            //     input = exciter[idx] + p.resonator_chaos_amount*input;
            //     input *= 0.5;
            //     v.resonator[unison_idx].resonator = process<sample_t>(v.resonator[unison_idx].resonator, input);
            //     block[idx] = v.resonator[unison_idx].resonator.y1;
            // }


            switch(p.feedback_mode){
                case TANH: std::tie(v.resonator[unison_idx],block) = process_tanh<sample_t, BLOCK_SIZE>(v.resonator[unison_idx],exciter);
                default: std::tie(v.resonator[unison_idx],block) = process<sample_t, cos_t<sample_t,TABLE_SIZE>::lookup, BLOCK_SIZE>(v.resonator[unison_idx],exciter);
            }

            block *= exciter*-1 + 1;
            StereoBlock<sample_t,BLOCK_SIZE> panned_block=pan_unison_block<sample_t, TABLE_SIZE, BLOCK_SIZE, MAX_UNISON>(block,p,unison_idx);
            output[0] += panned_block[0];
            output[1] += panned_block[1];
        } 

        for(size_t stereo_idx=0; stereo_idx<2; stereo_idx++){
            output[stereo_idx] = tanh_approx_pade<sample_t>(output[stereo_idx]);
            
            output[stereo_idx] *= 0.25*p.gain;

            // std::tie(v.highpass, output[stereo_idx]) = process<sample_t, frequency_t>(v.highpass, output[stereo_idx]);

            // std::tie(v.lowpass, output[stereo_idx]) = process<sample_t>(v.lowpass, output[stereo_idx]);

            output[stereo_idx] *= v.amp_envelope.env.value*v.amp_envelope.env.value;
        }
            

            // const sample_t one_minus_width = (1-p.stereo_width);
            // const sample_t center = 0.125;
            // sample_t power_left =  p.unison>1?0.25*sample_t(unison_idx)/sample_t(p.unison-1):center;
            // // sample_t power_left =  center;
            // power_left = lerp(power_left,center,one_minus_width);

            // sample_t power_right = power_left + 0.75;
            // output[0] += block*cos_t<sample_t,TABLE_SIZE>::lookup(power_left);
            // output[1] += block*cos_t<sample_t,TABLE_SIZE>::lookup(power_right);
        

        return std::pair(v,output);
        
    }

    template<typename sample_t, typename frequency_t, size_t MAX_UNISON, size_t VOICES, size_t WAVE_TABLE_SIZE, size_t BLOCK_SIZE>
    const inline std::pair<std::array<voice_t<sample_t,frequency_t,MAX_UNISON>, VOICES>, StereoBlock<sample_t,BLOCK_SIZE>> process(
        std::array<voice_t<sample_t,frequency_t,MAX_UNISON>,VOICES> voices, 
        const params_t<sample_t>& params, 
        const frequency_t& sampleRate){

        const frequency_t SR = sampleRate;
        const frequency_t CR = SR/BLOCK_SIZE;
        
        StereoBlock<sample_t,BLOCK_SIZE> block = {
            AudioBlock<sample_t,BLOCK_SIZE>::empty(),
            AudioBlock<sample_t,BLOCK_SIZE>::empty()
        };

        for(size_t voiceIndex=0; voiceIndex<VOICES; voiceIndex++){

            if(telegraph::isActive<sample_t, frequency_t,MAX_UNISON>(voices[voiceIndex])){
                voices[voiceIndex] = process_control<sample_t, frequency_t>(voices[voiceIndex], params, SR, CR);
                StereoBlock<sample_t,BLOCK_SIZE> samples;
                std::tie(voices[voiceIndex],samples) = process<sample_t, frequency_t, WAVE_TABLE_SIZE, BLOCK_SIZE>(voices[voiceIndex], params, SR);
                block[0] +=samples[0];
                block[1] +=samples[1];
            }

        }

        return std::pair(voices,block);
    }


    template<typename sample_t, typename frequency_t, size_t MAX_UNISON> 
    const inline voice_t<sample_t,frequency_t,MAX_UNISON> noteOn(voice_t<sample_t,frequency_t,MAX_UNISON> v, const params_t<sample_t>& p, const sample_t& note, const frequency_t& sampleRate, const frequency_t& controlRate){
        v.amp_gate = true;
        v.amp_envelope = adsr_t<sample_t>();
        v.exciter_envelope = ramp_t<sample_t>();
        
        v.note = note;
        frequency_t freq = mtof<frequency_t>(note);
        v.frequency = freq;

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
            sample_t value = voices[index].amp_envelope.env.value;
            if (!isActive<sample_t,frequency_t,MAX_UNISON>(voices[index])){
                indexOfQuietestVoice = index;
                break;
            }
            if(value<minValue){
                minValue = value;
                indexOfQuietestVoice = index;
            }
        }

        voices[indexOfQuietestVoice] = noteOn<sample_t,frequency_t>(voices[indexOfQuietestVoice], params, noteNumber, sampleRate, controlRate);

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
