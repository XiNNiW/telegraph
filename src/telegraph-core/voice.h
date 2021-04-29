#include <algae.h>
using algae::dsp::shell::dsp_node;
using algae::dsp::core::filter::vcf_t;
using algae::dsp::core::filter::process;
using algae::dsp::core::filter::onepole_t;
using algae::dsp::core::filter::update_onepole_hip;
using algae::dsp::core::filter::bp_t;
using algae::dsp::core::filter::bp;
using algae::dsp::core::filter::bandpass_t;
using algae::dsp::core::filter::process;
using algae::dsp::core::filter::update_coefficients;
using algae::dsp::core::filter::update_bp;
using algae::dsp::core::filter::update_biquad;
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
using algae::dsp::core::oscillator::phasor_t;
using algae::dsp::core::oscillator::update_phasor;
using algae::dsp::core::oscillator::update_phase_custom_period;
using algae::dsp::core::oscillator::stk_blit_saw_t;
using algae::dsp::core::oscillator::stk_blit_saw;
using algae::dsp::core::oscillator::stk_blit_square_t;
using algae::dsp::core::oscillator::stk_blit_square;
using algae::dsp::core::oscillator::setFrequency;
using algae::dsp::core::oscillator::process;
using algae::dsp::core::math::tanh_approx_pade;
using algae::dsp::core::oscillator::cos_approx_taylor_4;
using algae::dsp::core::oscillator::sine_approx_taylor_4;
using algae::dsp::core::oscillator::sine_t;
using algae::dsp::core::oscillator::cos_t;
using algae::dsp::core::units::mtof;
using algae::dsp::core::filter::moog_t;
using algae::dsp::core::filter::update_moog;
using algae::dsp::core::filter::setFilterParameters;

#define TWO_PI  2*M_PI 



namespace telegraph{
    enum Wave {
        SAW,
        SQUARE,
        SINE
    };
    
    template<typename sample_t, typename frequency_t>
    struct voice_t {
        //control
        int note;
        bool amp_gate;
        frequency_t frequency;
        ramp_t<sample_t> exciter_envelope;
        adsr_t<sample_t> amp_envelope;
        sample_t lfo_phase;
        sample_t lfo_value;
        //audio
        //union {
            stk_blit_saw_t<sample_t, frequency_t> sawtooth;
            stk_blit_square_t<sample_t, frequency_t> square;
            sample_t phase;
        //};
        frequency_t phi;
        frequency_t shaper_phi;
        sample_t shaper_phase_left;
        sample_t shaper_phase_right;
        sample_t resonator_frequency;
        reson_bp_t<sample_t> resonator_left;
        reson_bp_t<sample_t> resonator_right;
        onepole_t<sample_t, frequency_t> highpass_left;
        onepole_t<sample_t, frequency_t> highpass_right;
        biquad_t<sample_t,frequency_t> filter_left;
        biquad_t<sample_t,frequency_t> filter_right;
        // moog_t<sample_t,frequency_t> filter_left;
        // moog_t<sample_t,frequency_t> filter_right;
        sample_t out_left;
        sample_t out_right;
    };

    template<typename sample_t>
    struct params_t {
        sample_t exciter_gain=1.0;
        sample_t resonator_q=1.0;
        sample_t resonator_feedback=0.01;
        sample_t resonator_cross_feedback=0.01;
        sample_t resonator_detune=0.01;
        int amp_attack=48;
        int amp_decay=4800;
        sample_t amp_sustain=0.9;
        int amp_release=48000;
        int feedback_attack=0;
        int feedback_decay=48000;
        int noise_attack=48;
        int noise_decay=4800;
        sample_t osc_tune=1;
        sample_t resonater_tune_L=1;
        sample_t resonater_tune_R=1;
        sample_t shaper_tune=1;
        sample_t vibrato_speed=0;
        sample_t vibrato_depth=0;
        sample_t shaper_amount=1;
        Wave wave_mode=SQUARE;
        sample_t q_scaling = 1;
        sample_t gain=0.5;
        sample_t filter_cutoff = 10000;
        sample_t filter_q = 0.05;
    };

    template<typename sample_t, typename frequency_t>
    voice_t<sample_t,frequency_t> initVoice(voice_t<sample_t,frequency_t> v, sample_t sampleRate){
        v.amp_gate = false;
        v.sawtooth = stk_blit_saw<sample_t,frequency_t>(440.0, sampleRate);
        v.square = stk_blit_square<sample_t,frequency_t>(440.0, sampleRate);
        
        // v.exciter_envelope = ramp_t<sample_t>();
        // v.amp_envelope = adsr_t<sample_t>();
        return v;
    }

    template<typename sample_t, typename frequency_t>
    voice_t<sample_t,frequency_t> resetVoice(voice_t<sample_t,frequency_t> v){
        v.amp_gate = false;
        v.exciter_envelope = ramp_t<sample_t>();
        v.amp_envelope = adsr_t<sample_t>();
        return v;
    }

    template<typename sample_t, typename frequency_t>
    const inline bool isActive(const voice_t<sample_t,frequency_t>& v,const params_t<sample_t>& p){
        bool isActive = false;
        if(v.amp_envelope.env.value<0.0001 && v.amp_envelope.env.index > p.amp_attack){
            isActive = false;
        } else {
            isActive = true;
        }

        return isActive;
    }


    template<typename sample_t, typename frequency_t, int TABLE_SIZE>
    const inline voice_t<sample_t,frequency_t> process(voice_t<sample_t,frequency_t> v, params_t<sample_t> p, frequency_t sampleRate){
        
        sample_t exciter;
        switch(p.wave_mode){
            case SAW:  exciter = v.sawtooth.state; v.sawtooth = process<sample_t,frequency_t>(v.sawtooth, sampleRate); break;
            case SQUARE: exciter = v.square.state; v.square = process<sample_t,frequency_t>(v.square, sampleRate); break;
            default: exciter = sine_t<sample_t,TABLE_SIZE>::lookup(v.phase); v.phase = update_phase_custom_period<sample_t,frequency_t>(v.phase, v.phi,1.0); break;
        }

        exciter = p.exciter_gain * exciter;

        sample_t sq_env = v.amp_envelope.env.value;
        sample_t shaper_left = sq_env*(p.shaper_amount*sine_t<sample_t,TABLE_SIZE>::lookup(v.shaper_phase_left));
        sample_t shaper_right = sq_env*(p.shaper_amount*cos_t<sample_t,TABLE_SIZE>::lookup(v.shaper_phase_right));
        sample_t feedback_signal_left = -cos_t<sample_t,TABLE_SIZE>::lookup((p.resonator_feedback*v.resonator_left.y1 + p.resonator_cross_feedback*v.resonator_right.y1 - shaper_left)); //+ sq_env*sin(v.phase)
        sample_t feedback_signal_right = cos_t<sample_t,TABLE_SIZE>::lookup((p.resonator_feedback*v.resonator_right.y1 - p.resonator_cross_feedback*v.resonator_left.y1 + shaper_right )); //+ sq_env*sin(v.phase)
        sample_t resonator_input_left = exciter + feedback_signal_left;
        sample_t resonator_input_right = exciter + feedback_signal_right;
        resonator_input_left *= 0.5;
        resonator_input_right *= 0.5;
        v.resonator_left = process<sample_t, frequency_t>(v.resonator_left, resonator_input_left);
        v.resonator_right = process<sample_t, frequency_t>(v.resonator_right, resonator_input_right);

        v.highpass_left = update_onepole_hip<sample_t, frequency_t>(v.highpass_left,v.resonator_left.y1,5.0,sampleRate);
        v.highpass_right = update_onepole_hip<sample_t, frequency_t>(v.highpass_right,v.resonator_right.y1,5.0,sampleRate);

        sample_t mod_left = v.highpass_left.y*(1-exciter);
        sample_t mod_right = v.highpass_right.y*(1-exciter);

        v.out_left = (mod_left)*p.gain;
        v.out_left = tanh_approx_pade<sample_t>(v.out_left);
        v.filter_left = update_biquad<sample_t,frequency_t>(v.filter_left, v.out_left);
        v.out_left = v.filter_left.y1;
        v.out_left *= v.amp_envelope.env.value;
        v.out_left *= v.amp_envelope.env.value;
        v.out_left *= 0.5;

        v.out_right = (mod_right)*p.gain;
        v.out_right = tanh_approx_pade<sample_t>(v.out_right);
        v.filter_right = update_biquad<sample_t,frequency_t>(v.filter_right, v.out_right);
        v.out_right = v.filter_right.y1;
        v.out_right *= v.amp_envelope.env.value;
        v.out_right *= v.amp_envelope.env.value;
        v.out_right *= 0.5;

        v.shaper_phase_left = update_phase_custom_period<sample_t,frequency_t>(v.shaper_phase_left, v.shaper_phi, 1.0);
        v.shaper_phase_right = update_phase_custom_period<sample_t,frequency_t>(v.shaper_phase_right, v.shaper_phi, 1.0);
      
        return v;
        
    }


    template<typename sample_t, typename frequency_t> 
    const inline voice_t<sample_t, frequency_t> process_control(voice_t<sample_t,frequency_t> v, const params_t<sample_t>& p, const frequency_t& sampleRate, const frequency_t& controlRate){
        
        v.lfo_phase = update_phase_custom_period<sample_t,frequency_t>(v.lfo_phase,p.vibrato_speed/controlRate,1);
        v.lfo_value = v.amp_envelope.env.value*(mtof<frequency_t>(1)*p.vibrato_depth)*sine_t<sample_t, 1024>::lookup(v.lfo_phase);
        v.lfo_phase += (0.99999-v.amp_envelope.env.value)*v.lfo_value;

        switch(p.wave_mode){
            case SAW: v.sawtooth = setFrequency<sample_t, frequency_t>(v.sawtooth, v.frequency*p.osc_tune+v.lfo_value, sampleRate); break;
            case SQUARE: v.square = setFrequency<sample_t, frequency_t>(v.square, v.frequency*p.osc_tune+v.lfo_value, sampleRate); break;
            default: v.phi = (v.frequency*p.osc_tune+v.lfo_value)/sampleRate; break;
        }

        frequency_t resonator_frequency_L = v.frequency*p.resonater_tune_L;
        frequency_t resonator_frequency_R = v.frequency*p.resonater_tune_L;

        resonator_frequency_L = resonator_frequency_L==0?1:resonator_frequency_L;
        resonator_frequency_R = resonator_frequency_R==0?1:resonator_frequency_R;

        v.resonator_left = update_coefficients<sample_t,frequency_t>(v.resonator_left, resonator_frequency_L/*+p.resonator_detune*/, p.resonator_q, 0.5, sampleRate);
        v.resonator_right = update_coefficients<sample_t,frequency_t>(v.resonator_right, resonator_frequency_R/*-p.resonator_detune*/, p.resonator_q, 0.5, sampleRate);

        v.filter_left = lowpass<sample_t,frequency_t>(v.filter_left,p.filter_cutoff,p.filter_q,sampleRate);
        v.filter_right = lowpass<sample_t,frequency_t>(v.filter_right,p.filter_cutoff,p.filter_q,sampleRate);

        // v.filter_left = setFilterParameters(v.filter_left,p.filter_cutoff,p.filter_q,sampleRate);
        // v.filter_right = setFilterParameters(v.filter_right,p.filter_cutoff,p.filter_q,sampleRate);

        v.amp_envelope = update_adsr<sample_t>(v.amp_envelope, v.amp_gate, p.amp_attack, p.amp_decay, p.amp_sustain, p.amp_release);
        v.exciter_envelope = update_ad<sample_t>(v.exciter_envelope, p.feedback_attack, p.feedback_decay);

        return v;
    }

    template<typename sample_t, typename frequency_t>
    const inline voice_t<sample_t,frequency_t> noteOn(voice_t<sample_t,frequency_t> v, const params_t<sample_t>& p, const sample_t& note, const frequency_t& sampleRate, const frequency_t& controlRate){
        v.amp_gate = true;
        v.amp_envelope = adsr_t<sample_t>();
        v.exciter_envelope = ramp_t<sample_t>();
        
        v.note = note;
        frequency_t freq = mtof<frequency_t>(note);
        v.frequency = freq;
        v.shaper_phi = p.shaper_tune*freq/sampleRate;
        
        switch(p.wave_mode){
            case SAW: v.sawtooth = setFrequency(v.sawtooth, freq*p.osc_tune, sampleRate); break;
            case SQUARE: v.square = setFrequency(v.square, freq*p.osc_tune, sampleRate); break;
            default: v.phi = freq*p.osc_tune/sampleRate; break;
        }

        return process_control(v, p, sampleRate, controlRate);
    }

    template<typename sample_t, typename frequency_t>
    const inline voice_t<sample_t, frequency_t> noteOff(voice_t<sample_t, frequency_t> v){
        
        v.amp_gate = false;
        return v;
    }

    template<typename sample_t, typename frequency_t, size_t NUM_VOICES>
    const inline std::array<voice_t<sample_t,frequency_t>, NUM_VOICES> playNote(
        std::array<voice_t<sample_t,frequency_t>, NUM_VOICES> voices,
        const params_t<sample_t> params,
        const int channel,
        const int noteNumber,
        const int velocity,
        const frequency_t sampleRate,
        const frequency_t controlRate
    ){
        bool voiceWasAssigned = false;
        
        for(size_t index=0; index<NUM_VOICES; index++){
            if (!isActive(voices[index], params)){
                voices[index] = noteOn<sample_t,frequency_t>(voices[index], params, noteNumber, sampleRate, controlRate);
                voiceWasAssigned = true;
                break;
            }
        }

        if(!voiceWasAssigned){
            size_t indexOfQuietestVoice = 0;
            sample_t minValue = 10000;
            for(size_t index=0; index<NUM_VOICES; index++){
                sample_t value = voices[index].amp_envelope.env.value;
                if(value<minValue){
                    minValue = value;
                    indexOfQuietestVoice = index;
                }
            }

            voices[indexOfQuietestVoice] = noteOn<sample_t,frequency_t>(voices[indexOfQuietestVoice], params, noteNumber, sampleRate, controlRate);

        }

        return voices;
    }

    template<typename sample_t, typename frequency_t, size_t NUM_VOICES>
    const inline std::array<voice_t<sample_t,frequency_t>, NUM_VOICES> releaseNote(
        std::array<voice_t<sample_t,frequency_t>, NUM_VOICES> voices,
        const params_t<sample_t> params,
        const int channel,
        const int noteNumber,
        const int velocity,
        const frequency_t sampleRate
    ){
        for(size_t index=0; index<NUM_VOICES; index++){
            if(isActive(voices[index],params) && voices[index].note==noteNumber){
                voices[index] = noteOff<sample_t,frequency_t>(voices[index]);
            }
        }
        return voices;
    }

    template<typename sample_t>
    sample_t denormalize(sample_t val, sample_t min, sample_t max){
        
        val = val>=0.0 && val<=1.0 ? val : 0.0;
        return val*(max-min) + min;
    }

    template<typename sample_t>
    sample_t denormalize_exp(sample_t val, sample_t min, sample_t max, sample_t base){
        
        val = val>0.0 && val<=1.0 ? val : 0.0;
        val = val>0.0 && base!=1?pow(base,val)/(base-1):val;
        return val*(max-min) + min;
    }

    template<typename sample_t, int SIZE>
    sample_t denormalize_set(sample_t val,  const sample_t (&options) [SIZE]){
        
        val = val>=0 && val<=1 ? val : 0;
        return options[size_t(floor(val*(SIZE-1)))];
    }

    template<typename input_t, typename output_t, int SIZE>
    output_t denormalize_set(input_t val,  const output_t (&options) [SIZE]){
        val = val>=0 && val<=1 ? val : 0;
        return options[size_t(floor(val*(SIZE-1)))];
    }

    template<typename sample_t>
    params_t<sample_t> setFeedback(params_t<sample_t> v, sample_t resonator_feedback){
        v.resonator_feedback = resonator_feedback;
        return v;
    }
    template<typename sample_t>
    params_t<sample_t> setFeedback(params_t<sample_t> v, sample_t resonator_feedback, sample_t min, sample_t max){
        resonator_feedback = denormalize<sample_t>(resonator_feedback,min,max);
        return setFeedback(v, resonator_feedback);
    }

    template<typename sample_t>
    params_t<sample_t> setQ(params_t<sample_t> v, sample_t resonator_q){
        v.resonator_q = resonator_q;
        return v;
    }

    template<typename sample_t>
    params_t<sample_t> setQ(params_t<sample_t> v, sample_t resonator_q, sample_t min, sample_t max){
        resonator_q = denormalize<sample_t>(resonator_q,min,max);
        return setQ(v, resonator_q);
    }

    using algae::dsp::core::units::msToSamples;

    template<typename sample_t>
    sample_t convertTimeParameter(sample_t param, sample_t sampleRate){
        sample_t converted = msToSamples<sample_t,sample_t>(param, sampleRate);
        return converted;
    }

    template<typename sample_t>
    sample_t convertTimeParameter(sample_t amp_attack, sample_t min, sample_t max, sample_t sampleRate){
        amp_attack = denormalize<sample_t>(amp_attack, min, max);
        return convertTimeParameter(amp_attack, sampleRate);
    }

    template<typename sample_t>
    params_t<sample_t> setAmpAttack(params_t<sample_t> v, sample_t amp_attack, sample_t sampleRate){
        v.amp_attack = msToSamples<sample_t,sample_t>(amp_attack, sampleRate);
        return v;
    }

    template<typename sample_t>
    params_t<sample_t> setAmpAttack(params_t<sample_t> v, sample_t amp_attack, sample_t min, sample_t max, sample_t sampleRate){
        amp_attack = denormalize<sample_t>(amp_attack, min, max);
        return setAmpAttack(v, amp_attack, sampleRate);
    }

    template<typename sample_t>
    params_t<sample_t> setAmpDecay(params_t<sample_t> v, sample_t amp_decay, sample_t sampleRate){
        v.amp_decay = msToSamples<sample_t,sample_t>(amp_decay, sampleRate);
        return v;
    }

    template<typename sample_t>
    params_t<sample_t> setAmpDecay(params_t<sample_t> v, sample_t amp_decay, sample_t min, sample_t max, sample_t sampleRate){
        amp_decay = denormalize<sample_t>(amp_decay,min,max);
        return setAmpDecay(v, amp_decay, sampleRate);
    }

    template<typename sample_t>
    params_t<sample_t> setAmpSustain(params_t<sample_t> v, sample_t amp_sustain){
        v.amp_sustain = amp_sustain;
        return v;
    }

    template<typename sample_t>
    params_t<sample_t> setAmpSustain(params_t<sample_t> v, sample_t amp_sustain, sample_t min, sample_t max){
        amp_sustain = denormalize<sample_t>(amp_sustain,min,max);
        return setAmpSustain(v, amp_sustain);
    }

    template<typename sample_t>
    params_t<sample_t> setAmpRelease(params_t<sample_t> v, sample_t amp_release, sample_t sampleRate){
        v.amp_release = msToSamples<sample_t,sample_t>(amp_release, sampleRate);
        return v;
    }

    template<typename sample_t>
    params_t<sample_t> setAmpRelease(params_t<sample_t> v, sample_t amp_release, sample_t min, sample_t max, sample_t sampleRate){
        amp_release = denormalize<sample_t>(amp_release,min,max);
        return setAmpRelease(v, amp_release, sampleRate);
    }

    // template<typename sample_t>
    // params_t<sample_t> setExciterAttack(params_t<sample_t> v, sample_t exciter_attack, sample_t sampleRate){
    //     v.exciter_attack = msToSamples<sample_t,sample_t>(exciter_attack, sampleRate);
    //     return v;
    // }

    // template<typename sample_t>
    // params_t<sample_t> setExciterAttack(params_t<sample_t> v, sample_t exciter_attack, sample_t min, sample_t max, sample_t sampleRate){
    //     exciter_attack = denormalize<sample_t>(exciter_attack, min, max);
    //     return setExciterAttack(v, exciter_attack, sampleRate);
    // }

    // template<typename sample_t>
    // params_t<sample_t> setExciterDecay(params_t<sample_t> v, sample_t exciter_decay, sample_t sampleRate){
    //     v.exciter_decay = msToSamples<sample_t,sample_t>(exciter_decay, sampleRate);
    //     return v;
    // }

    // template<typename sample_t>
    // params_t<sample_t> setExciterDecay(params_t<sample_t> v, sample_t exciter_decay, sample_t min, sample_t max, sample_t sampleRate){
    //     exciter_decay = denormalize<sample_t>(exciter_decay,min,max);
    //     return setExciterDecay(v, exciter_decay, sampleRate);
    // }

}
