#include <algae.h>
using algae::dsp::shell::dsp_node;
using algae::dsp::core::filter::vcf_t;
using algae::dsp::core::filter::update_vcf;
using algae::dsp::core::filter::onepole_t;
using algae::dsp::core::filter::update_onepole_hip;
using algae::dsp::core::filter::bp_t;
using algae::dsp::core::filter::bp;
using algae::dsp::core::filter::bandpass_t;
using algae::dsp::core::filter::process_bandpass;
using algae::dsp::core::filter::update_coefficients;
using algae::dsp::core::filter::update_bp;
using algae::dsp::core::filter::update_biquad;
using algae::dsp::core::control::ramp_t;
using algae::dsp::core::control::update_adsr;
using algae::dsp::core::control::update_ad;
using algae::dsp::core::oscillator::noise;
using algae::dsp::core::oscillator::phasor_t;
using algae::dsp::core::oscillator::update_phasor;
using algae::dsp::core::oscillator::sinOsc;
using algae::dsp::core::oscillator::fm_bl_saw_t;
using algae::dsp::core::oscillator::process_fm_bl_saw;
using algae::dsp::core::oscillator::max_bl_modulation_index;
using algae::dsp::core::units::mtof;

namespace telegraph{
    template<typename sample_t>
    struct voice_t {
        bool is_active;
        fm_bl_saw_t<sample_t, sample_t> exciter;
        ramp_t<sample_t> exciter_envelope;
        bandpass_t<sample_t> resonator;
        sample_t resonator_frequency;
        bool amp_gate;
        ramp_t<sample_t> amp_envelope;
        sample_t output;
    };

    template<typename sample_t>
    struct params_t {
        sample_t resonator_q;
        sample_t resonator_feedback;
        int amp_attack;
        int amp_decay;
        sample_t amp_sustain;
        int amp_release;
        int exciter_attack;
        int exciter_decay;
    };

    template<typename sample_t>
    voice_t<sample_t> initVoice(voice_t<sample_t> v, sample_t sampleRate){
        v.is_active = false;
        v.amp_gate = false;
        v.exciter = algae::dsp::core::oscillator::fm_bl_saw<sample_t,sample_t>(v.exciter, 0, sampleRate);
        v.exciter_envelope = ramp_t<sample_t>();
        v.amp_envelope = ramp_t<sample_t>();
        v.resonator = bandpass_t<sample_t>();
        // v.resonator = update_coefficients<sample_t,sample_t>(bandpass_t<sample_t>(), 440.0, 10.0 , sampleRate);
        return v;
    }

    template<typename sample_t>
    voice_t<sample_t> resetVoice(voice_t<sample_t> v){
        v.is_active = false;
        v.amp_gate = false;
        v.exciter_envelope = ramp_t<sample_t>();
        v.amp_envelope = ramp_t<sample_t>();
        v.resonator = bandpass_t<sample_t>();
        // v.resonator = update_coefficients<sample_t,sample_t>(bandpass_t<sample_t>(), 440.0, 10.0 , sampleRate);
        return v;
    }

    template<typename sample_t, typename frequency_t>
    voice_t<sample_t> process(voice_t<sample_t> v, params_t<sample_t> params, frequency_t sampleRate){

        // sample_t noise = algae::dsp::core::oscillator::noise<sample_t>();
        // noise *= v.exciter_envelope.value;
        // noise *= v.exciter_envelope.value;

        // v.exciter = process_fm_bl_saw<sample_t>(v.exciter, sampleRate);
        // // sample_t exciter = v.exciter.output;
        // sample_t exciter = algae::dsp::core::oscillator::sinOsc<sample_t,sample_t>(v.exciter.phasor.phase);
        // exciter *= v.amp_envelope.value;

        // sample_t resonator_input = noise + exciter + params.resonator_feedback*cos( params.resonator_feedback*M_2_PI * v.resonator.y1);
        
        // v.resonator = process_bandpass<sample_t>(v.resonator, resonator_input);

        // // resonator_input += v.resonator.y1 + exciter + params.resonator_feedback*cos(params.resonator_feedback*  M_2_PI * v.resonator3.y1);
        // // v.resonator2 = process_bandpass<sample_t>(v.resonator2, resonator_input);

        // // resonator_input +=  v.resonator2.y1 + exciter + params.resonator_feedback*tanh( params.resonator_feedback* M_2_PI * v.resonator3.y1);
        // // v.resonator3 = process_bandpass<sample_t>(v.resonator3, resonator_input);
        // //wrap this in a karplus strong voice

        // // chain filters in series... then in paralell
        // // fm the filters together
        // v.exciter_envelope = update_ad<sample_t>(v.exciter_envelope, params.exciter_attack, params.exciter_decay);
        // v.amp_envelope = update_adsr<sample_t>(v.amp_envelope, v.amp_gate, params.amp_attack, params.amp_decay, params.amp_sustain, params.amp_release);
        
        // v.output = tanh(v.resonator.y1 * v.amp_envelope.value);
        v.exciter.phasor = update_phasor<double>(v.exciter.phasor, v.resonator_frequency, sampleRate);

        v.output = algae::dsp::core::oscillator::sinOsc<sample_t>(v.exciter.phasor.phase);
        // v.output *= v.amp_envelope.value;
        return v;
        
    }

    template<typename sample_t>
    voice_t<sample_t> noteOn(voice_t<sample_t> v, params_t<sample_t> p, sample_t note, sample_t sampleRate){
        v.resonator_frequency = mtof<sample_t>(note);
        v.exciter = algae::dsp::core::oscillator::setFrequency(v.exciter,v.resonator_frequency,sampleRate);
        v.amp_gate = true;
        v.is_active = true;
        // v.resonator = update_coefficients<sample_t,sample_t>(v.resonator, 440.0, 10.0 , sampleRate);
        sample_t q = p.resonator_q;
        // q = 1000.0;
        v.resonator = update_coefficients<sample_t,sample_t>(v.resonator,v.resonator_frequency, q , sampleRate);
        // v.resonator2 = update_coefficients<sample_t,sample_t>(v.resonator2,v.resonator_frequency, q , sampleRate);
        // v.resonator2 = update_coefficients<sample_t,sample_t>(v.resonator3,v.resonator_frequency, q , sampleRate);

        return v;
    }

    template<typename sample_t>
    voice_t<sample_t> noteOff(voice_t<sample_t> v){
        v.amp_gate = false;
        return v;
    }

    template<typename sample_t>
    sample_t denormalize(sample_t val, sample_t min, sample_t max){
        val = val>=0.0 && val<=1.0 ? val : 0.0;
        return val*(max-min) + min;
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

    template<typename sample_t>
    params_t<sample_t> setExciterAttack(params_t<sample_t> v, sample_t exciter_attack, sample_t sampleRate){
        v.exciter_attack = msToSamples<sample_t,sample_t>(exciter_attack, sampleRate);
        return v;
    }

    template<typename sample_t>
    params_t<sample_t> setExciterAttack(params_t<sample_t> v, sample_t exciter_attack, sample_t min, sample_t max, sample_t sampleRate){
        exciter_attack = denormalize<sample_t>(exciter_attack, min, max);
        return setExciterAttack(v, exciter_attack, sampleRate);
    }

    template<typename sample_t>
    params_t<sample_t> setExciterDecay(params_t<sample_t> v, sample_t exciter_decay, sample_t sampleRate){
        v.exciter_decay = msToSamples<sample_t,sample_t>(exciter_decay, sampleRate);
        return v;
    }

    template<typename sample_t>
    params_t<sample_t> setExciterDecay(params_t<sample_t> v, sample_t exciter_decay, sample_t min, sample_t max, sample_t sampleRate){
        exciter_decay = denormalize<sample_t>(exciter_decay,min,max);
        return setExciterDecay(v, exciter_decay, sampleRate);
    }

}
