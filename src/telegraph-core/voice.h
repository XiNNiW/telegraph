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

    template<typename sample_t, int SIZE>
    sample_t scale_parameter_from_set(sample_t val,  const sample_t (&options) [SIZE]){
        
        val = val>=0 && val<=1 ? val : 0;
        return options[size_t(floor(val*(SIZE-1)))];
    }

    template<typename input_t, typename output_t, int SIZE>
    output_t scale_parameter_from_set(input_t val,  const output_t (&options) [SIZE]){
        val = val>=0 && val<=1 ? val : 0;
        return options[size_t(floor(val*(SIZE-1)))];
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
    
    template<typename sample_t, typename frequency_t, int UNISON>
    struct alignas(16) voice_t {
        //control
        int note;
        bool amp_gate=false;
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
        sample_t resonator_frequency;
        reson_bp_t<sample_t> resonator[UNISON];
        onepole_t<sample_t, frequency_t> highpass[UNISON];
        biquad_t<sample_t,frequency_t> filter[UNISON];
        sample_t output[2];
    };

    template<typename sample_t>
    struct params_t {
        sample_t exciter_gain=1.0;
        Wave wave_mode=SINE;
        FeedbackMode feedback_mode=COS;
        sample_t resonator_q=1.0;
        sample_t resonator_feedback=0.01;
        sample_t resonator_cross_feedback=0.01;
        sample_t resonator_chaos_character=0.01;
        sample_t resonator_chaos_amount=0;
        sample_t resonator_detune=0.01;
        int amp_attack=48;
        int amp_decay=4800;
        sample_t amp_sustain=0.9;
        int amp_release=2400;
        int feedback_attack=0;
        int feedback_decay=2400;
        int noise_attack=48;
        int noise_decay=2400;
        sample_t exciter_ratio=1;
        sample_t resonater_ratio=1;
        sample_t resonater_tune_R=1;
        sample_t shaper_tune=1;
        sample_t vibrato_speed=0.0;
        sample_t vibrato_depth=0.0;
        sample_t shaper_amount=1.0;
        sample_t q_scaling = 1.0;
        sample_t gain=0.5;
        sample_t lowpass_filter_cutoff = 20000.0;
        sample_t lowpass_filter_q = 0.05;
        sample_t highpass_filter_cutoff = 30.0;
        sample_t stereo_width = 1;
    };

    template<typename sample_t, typename frequency_t, int UNISON>
    voice_t<sample_t, frequency_t, UNISON> initVoice(voice_t<sample_t, frequency_t, UNISON> v, sample_t sampleRate){
        v.sawtooth = stk_blit_saw<sample_t,frequency_t>(440.0, sampleRate);
        v.square = stk_blit_square<sample_t,frequency_t>(440.0, sampleRate);

        return v;
    }

    template<typename sample_t, typename frequency_t, int UNISON>
    voice_t<sample_t, frequency_t, UNISON> resetVoice(voice_t<sample_t,frequency_t, UNISON> v){
        v.amp_gate = false;
        v.exciter_envelope = ramp_t<sample_t>();
        v.amp_envelope = adsr_t<sample_t>();
        return v;
    }

    template<typename sample_t, typename frequency_t, int UNISON>
    const inline bool isActive(const voice_t<sample_t,frequency_t, UNISON>& v){
        bool isActive = false;
        if((!v.amp_gate) && (v.amp_envelope.env.value<0.0001) ){
            isActive = false;
        } else {
            isActive = true;
        }

        return isActive;
    }

    template<typename sample_t>
    sample_t lerp(sample_t origin, sample_t dest, sample_t amt){
        amt = (amt>1)?1:(amt<0)?0:amt;
        return origin + amt*(dest-origin);
    }




    template<typename sample_t, typename frequency_t, int TABLE_SIZE, int UNISON>
    const inline voice_t<sample_t,frequency_t, UNISON> process(voice_t<sample_t,frequency_t, UNISON> v, const params_t<sample_t>& p, const frequency_t& sampleRate){
        v.output[0] = 0;
        v.output[1] = 0;

        sample_t exciter;
        switch(p.wave_mode){
            case SAW:  exciter = v.sawtooth.state; v.sawtooth = process<sample_t,frequency_t>(v.sawtooth, sampleRate); break;
            case SQUARE: exciter = v.square.state; v.square = process<sample_t,frequency_t>(v.square, sampleRate); break;
            default: exciter = sine_t<sample_t, TABLE_SIZE>::lookup(v.phase); v.phase = update_phase_custom_period<sample_t,frequency_t>(v.phase, v.phi,1.0); break;
        }

        exciter = p.exciter_gain * exciter;
        exciter *= v.amp_envelope.env.value;

        for(size_t index=0; index<UNISON; index++){
            sample_t sample;
            sample_t fb1 = p.resonator_feedback;
            switch(p.feedback_mode){
                case TANH: sample = tanh_approx_pade<sample_t>(-fb1*v.resonator[index].y1);
                default: sample = cos_t<sample_t, TABLE_SIZE>::lookup(-fb1*v.resonator[index].y1);
            }
             
            sample = exciter + p.resonator_chaos_amount*sample;
            sample *= 0.5;
            v.resonator[index] = process<sample_t, frequency_t>(v.resonator[index], sample);
            sample = tanh_approx_pade<sample_t>(v.resonator[index].y1);
            sample *= (1-exciter);
            sample *= 0.25*p.gain;
            v.highpass[index] = update_onepole_hip<sample_t, frequency_t>(v.highpass[index], sample, p.highpass_filter_cutoff, sampleRate);
            sample = v.highpass[index].y;
            v.filter[index] = update_biquad<sample_t,frequency_t>(v.filter[index], sample);
            sample = v.filter[index].y1;
            sample *= v.amp_envelope.env.value;
            sample *= v.amp_envelope.env.value;
            
            sample_t one_minus_width = (1-p.stereo_width);
            const sample_t center = 0.125;
            sample_t power_left =  UNISON>1?0.25*sample_t(index)/sample_t(UNISON-1):center;
            power_left = lerp(power_left,center,one_minus_width);
            sample_t power_right = power_left + 0.75;
            v.output[0] += sample*cos_t<sample_t,TABLE_SIZE>::lookup(power_left);
            v.output[1] += sample*cos_t<sample_t,TABLE_SIZE>::lookup(power_right);
        }

        return v;
        
    }


    template<typename sample_t, typename frequency_t, int UNISON> 
    const inline voice_t<sample_t, frequency_t, UNISON> process_control(voice_t<sample_t,frequency_t, UNISON> v, const params_t<sample_t>& p, const frequency_t& sampleRate, const frequency_t& controlRate){
        
        v.lfo_phase = update_phase_custom_period<sample_t,frequency_t>(v.lfo_phase,p.vibrato_speed/controlRate,1);
        v.lfo_value = v.amp_envelope.env.value*(mtof<frequency_t>(1)*p.vibrato_depth)*sine_t<sample_t, 1024>::lookup(v.lfo_phase);
        v.lfo_phase += (0.99999-v.amp_envelope.env.value)*v.lfo_value;

        switch(p.wave_mode){
            case SAW: v.sawtooth = setFrequency<sample_t, frequency_t>(v.sawtooth, v.frequency*p.exciter_ratio+v.lfo_value, sampleRate); break;
            case SQUARE: v.square = setFrequency<sample_t, frequency_t>(v.square, v.frequency*p.exciter_ratio+v.lfo_value, sampleRate); break;
            default: v.phi = (v.frequency*p.exciter_ratio+v.lfo_value)/sampleRate; break;
        }

        frequency_t resonator_frequency = v.frequency*p.resonater_ratio;
        resonator_frequency = resonator_frequency==0?1:resonator_frequency;

        for(size_t index; index<UNISON; index++){
            v.resonator[index] = update_coefficients<sample_t,frequency_t>(v.resonator[index], resonator_frequency/*+p.resonator_detune*/, p.resonator_q, 0.5, sampleRate);
            v.filter[index] = lowpass<sample_t,frequency_t>(v.filter[index],p.lowpass_filter_cutoff,p.lowpass_filter_q,sampleRate);
        }

        v.amp_envelope = update_adsr<sample_t>(v.amp_envelope, v.amp_gate, p.amp_attack, p.amp_decay, p.amp_sustain, p.amp_release);
        v.exciter_envelope = update_ad<sample_t>(v.exciter_envelope, p.feedback_attack, p.feedback_decay);
        
        return v;
    }

    template<typename sample_t, typename frequency_t, int UNISON>
    const inline voice_t<sample_t,frequency_t,UNISON> noteOn(voice_t<sample_t,frequency_t,UNISON> v, const params_t<sample_t>& p, const sample_t& note, const frequency_t& sampleRate, const frequency_t& controlRate){
        v.amp_gate = true;
        v.amp_envelope = adsr_t<sample_t>();
        v.exciter_envelope = ramp_t<sample_t>();
        
        v.note = note;
        frequency_t freq = mtof<frequency_t>(note);
        v.frequency = freq;
        
        switch(p.wave_mode){
            case SAW: v.sawtooth = setFrequency(v.sawtooth, freq*p.exciter_ratio, sampleRate); break;
            case SQUARE: v.square = setFrequency(v.square, freq*p.exciter_ratio, sampleRate); break;
            default: v.phi = freq*p.exciter_ratio/sampleRate; break;
        }

        return process_control(v, p, sampleRate, controlRate);
    }

    template<typename sample_t, typename frequency_t, int UNISON>
    const inline voice_t<sample_t, frequency_t, UNISON> noteOff(voice_t<sample_t, frequency_t, UNISON> v){
        
        v.amp_gate = false;
        return v;
    }

    template<typename sample_t, typename frequency_t, size_t NUM_VOICES, int UNISON>
    const inline std::array<voice_t<sample_t,frequency_t,UNISON>, NUM_VOICES> playNote(
        std::array<voice_t<sample_t,frequency_t,UNISON>, NUM_VOICES> voices,
        const params_t<sample_t> params,
        const int channel,
        const int noteNumber,
        const int velocity,
        const frequency_t sampleRate,
        const frequency_t controlRate
    ){
        size_t indexOfQuietestVoice = 0;
        sample_t minValue = 10000;
        for(size_t index=0; index<NUM_VOICES; index++){
            sample_t value = voices[index].amp_envelope.env.value;
            if (!isActive(voices[index])){
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

    template<typename sample_t, typename frequency_t, size_t NUM_VOICES, int UNISON>
    const inline std::array<voice_t<sample_t,frequency_t, UNISON>, NUM_VOICES> releaseNote(
        std::array<voice_t<sample_t,frequency_t, UNISON>, NUM_VOICES> voices,
        const params_t<sample_t> params,
        const int channel,
        const int noteNumber,
        const int velocity,
        const frequency_t sampleRate
    ){
        for(size_t index=0; index<NUM_VOICES; index++){
            if(isActive(voices[index]) && voices[index].note==noteNumber){
                voices[index] = noteOff<sample_t,frequency_t>(voices[index]);
            }
        }
        return voices;
    }



}
