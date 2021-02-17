#include <algae/dsp.h>
using algae::dsp::shell::dsp_node;
using algae::dsp::core::filter::vcf_t;
using algae::dsp::core::filter::update_vcf;
using algae::dsp::core::filter::onepole_t;
using algae::dsp::core::filter::update_onepole_hip;
using algae::dsp::core::filter::bp_t;
using algae::dsp::core::filter::bp;
using algae::dsp::core::filter::update_bp;
using algae::dsp::core::filter::update_biquad;
using algae::dsp::core::control::ramp_t;
using algae::dsp::core::control::update_adsr;
using algae::dsp::core::control::update_ad;
using algae::dsp::core::oscillator::noise;

namespace telegraph{
    template<typename sample_t>
    struct voice_t {
        bool is_active;
        ramp_t<sample_t> exciter_envelope;
        int exciter_attack;
        int exciter_decay;
        vcf_t<sample_t> resonator;
        sample_t resonator_frequency;
        sample_t resonator_q;
        sample_t resonator_feedback;
        bool amp_gate;
        int amp_attack;
        int amp_decay;
        sample_t amp_sustain;
        int amp_release;
        ramp_t<sample_t> amp_envelope;
        sample_t output;

    };

    template<typename sample_t>
    voice_t process(voice_t v){
        sample_t exciter = noise<sammple_t>();
        v.exciter_envelope = update_ad<sample_t>(v.exciter_envelope, v.exciter_attack, v.exciter_decay);
        exciter *= v.exciter_envelope.value;
        exciter *= v.exciter_envelope.value;

        sample_t resonator_input = exciter + cos(v.resonator_feedback * v.resonator.re);
        sample_t resonator_input *= 0.5;
        v.resonator = update_vcf<sammple_t>(resonator_input, v.resonator_frequency, v.resonator_q);

        v.amp_envelope = update_adsr<sample_t>(v.amp_envelope, v.amp_gate, v.amp_attack, v.amp_decay, v.amp_sustain, v.amp_release);
        v.output = tanh(v.resonator.re * v.amp_envelope.value);
        
    }

    template<typename sample_t>
    voice_t<sample_t> noteOn(voice_t<sample_t> v, sample_t note){
        v.resonator_frequency = mtof<sample_t>(note);
        v.amp_gate = true;
        v.is_active = true;
        return v;
    }

    template<typename sample_t>
    voice_t<sample_t> noteOff(voice_t<sample_t> v){
        return voice_t<sample_t>();
    }

    template<typename sample_t>
    voice_t<sample_t> setFeedback(voice_t<sample_t> v, sample_t resonator_feedback){
        v.resonator_feedback = resonator_feedback;
        return v;
    }

    template<typename sample_t>
    voice_t<sample_t> setQ(voice_t<sample_t> v, sample_t resonator_q){
        v.resonator_q = resonator_q;
        return v;
    }

    template<typename sample_t>
    voice_t<sample_t> setAmpAttack(voice_t<sample_t> v, sample_t amp_attack){
        v.amp_attack = amp_attack;
        return v;
    }

    template<typename sample_t>
    voice_t<sample_t> setAmpDecay(voice_t<sample_t> v, sample_t amp_decay){
        v.amp_decay = amp_decay;
        return v;
    }

    template<typename sample_t>
    voice_t<sample_t> setAmpSustain(voice_t<sample_t> v, sample_t amp_sustain){
        v.amp_sustain = amp_sustain;
        return v;
    }

    template<typename sample_t>
    voice_t<sample_t> setAmpRelease(voice_t<sample_t> v, sample_t amp_release){
        v.amp_release = amp_release;
        return v;
    }

    template<typename sample_t>
    voice_t<sample_t> setExciterAttack(voice_t<sample_t> v, sample_t exciter_attack){
        v.exciter_attack = exciter_attack;
        return v;
    }

    template<typename sample_t>
    voice_t<sample_t> setExciterDecay(voice_t<sample_t> v, sample_t exciter_decay){
        v.exciter_decay = exciter_decay;
        return v;
    }

}
