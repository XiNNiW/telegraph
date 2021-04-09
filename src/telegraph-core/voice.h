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
using algae::dsp::core::units::mtof;

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
        sample_t out_left;
        sample_t out_right;
    };

    template<typename sample_t>
    struct params_t {
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
        sample_t shaper_discord=0;
        sample_t shaper_amount=1;
        Wave wave_mode=SQUARE;
        sample_t q_scaling = 1;
        sample_t gain=0.75;
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
    voice_t<sample_t,frequency_t> process(voice_t<sample_t,frequency_t> v, params_t<sample_t> p, frequency_t sampleRate){
        
        sample_t exciter;
        switch(p.wave_mode){
            case SAW:  exciter = v.sawtooth.state; v.sawtooth = process(v.sawtooth, sampleRate); break;
            case SQUARE: exciter = v.square.state; v.square = process(v.square, sampleRate); break;
            default: exciter = sin(v.phase); v.phase = update_phase_custom_period(v.phase, v.phi); break;
        }

        sample_t sq_env = v.exciter_envelope.value;
        sample_t shaper_left = sq_env*p.shaper_amount*sin(v.shaper_phase_left);
        sample_t shaper_right = sq_env*p.shaper_amount*cos(v.shaper_phase_right);
        sample_t feedback_signal_left = -sq_env*cos(TWO_PI*(p.resonator_feedback*v.resonator_left.y1 + p.resonator_cross_feedback*v.resonator_right.y1 - shaper_left)); //+ sq_env*sin(v.phase)
        sample_t feedback_signal_right = sq_env*cos(TWO_PI*(p.resonator_feedback*v.resonator_right.y1 - p.resonator_cross_feedback*v.resonator_left.y1 + shaper_right )); //+ sq_env*sin(v.phase)
        sample_t resonator_input_left =  exciter + feedback_signal_left;
        sample_t resonator_input_right =  exciter + feedback_signal_right;
        resonator_input_left *= 0.5;
        resonator_input_right *= 0.5;
        v.resonator_left = process<sample_t, frequency_t>(v.resonator_left, resonator_input_left);
        v.resonator_right = process<sample_t, frequency_t>(v.resonator_right, resonator_input_right);

        v.highpass_left = update_onepole_hip<sample_t, frequency_t>(v.highpass_left,v.resonator_left.y1,5.0,sampleRate);
        v.highpass_right = update_onepole_hip<sample_t, frequency_t>(v.highpass_right,v.resonator_right.y1,5.0,sampleRate);

        v.out_left = v.highpass_left.y*p.gain;
        v.out_left = tanh(v.out_left);
        v.out_left *= v.amp_envelope.env.value;
        v.out_left *= 0.5;

        v.out_right = v.highpass_right.y*p.gain;
        v.out_right = tanh(v.out_right);//v.exciter.integrator.y1);//exciter_output);
        v.out_right *= v.amp_envelope.env.value;
        v.out_right *= 0.5;

        v.shaper_phase_left = update_phase_custom_period<double,double>(v.shaper_phase_left, v.shaper_phi);
        v.shaper_phase_left += p.shaper_discord*v.resonator_right.y1*TWO_PI;
        v.shaper_phase_right = update_phase_custom_period<double,double>(v.shaper_phase_right, v.shaper_phi);
        v.shaper_phase_right += p.shaper_discord*v.resonator_right.y1*TWO_PI;
        v.amp_envelope = update_adsr<sample_t>(v.amp_envelope, v.amp_gate, p.amp_attack, p.amp_decay, p.amp_sustain, p.amp_release);
        v.exciter_envelope = update_ad<sample_t>(v.exciter_envelope, p.feedback_attack, p.feedback_decay);
        
        return v;
        
    }

    template<typename sample_t, typename frequency_t> 
    voice_t<sample_t, frequency_t> process_control(voice_t<sample_t,frequency_t> v, params_t<sample_t> p){
        v.amp_envelope = update_adsr<sample_t>(v.amp_envelope, v.amp_gate, p.amp_attack, p.amp_decay, p.amp_sustain, p.amp_release);
        v.exciter_envelope = update_ad<sample_t>(v.exciter_envelope, p.feedback_attack, p.feedback_decay);
        return v;
    }

    template<typename sample_t, typename frequency_t>
    voice_t<sample_t,frequency_t> noteOn(voice_t<sample_t,frequency_t> v, params_t<sample_t> p, sample_t note, sample_t sampleRate){
        v.amp_gate = true;
        v.amp_envelope = adsr_t<sample_t>();
        v.exciter_envelope = ramp_t<sample_t>();
        
        v.note = note;
        frequency_t freq = mtof<frequency_t>(note);
        v.shaper_phi = p.shaper_tune*freq/sampleRate;

        frequency_t resonator_frequency_L = freq*p.resonater_tune_L;
        frequency_t resonator_frequency_R = freq*p.resonater_tune_R;
        v.resonator_left = update_coefficients<sample_t,sample_t>(v.resonator_left, resonator_frequency_L/*+p.resonator_detune*/, p.resonator_q, 0.5, sampleRate);
        v.resonator_right = update_coefficients<sample_t,sample_t>(v.resonator_right, resonator_frequency_R/*-p.resonator_detune*/, p.resonator_q, 0.5, sampleRate);

        p.q_scaling = p.resonator_q>1?p.resonator_q:1;
        
        switch(p.wave_mode){
            case SAW: v.sawtooth = setFrequency(v.sawtooth, freq*p.osc_tune, sampleRate); break;
            case SQUARE: v.square = setFrequency(v.square, freq*p.osc_tune, sampleRate); break;
            default: v.phi = freq*p.osc_tune/sampleRate; break;
        }

        return v;
    }

    template<typename sample_t, typename frequency_t>
    voice_t<sample_t, frequency_t> noteOff(voice_t<sample_t, frequency_t> v){
        
        v.amp_gate = false;
        return v;
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
