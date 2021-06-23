#pragma once
#include <algae.h>

namespace telegraph {

    using algae::dsp::core::control::adsr_t;
    using algae::dsp::core::control::adsr_params_t;
    using algae::dsp::core::oscillator::update_phase;

    enum class ModDestination {
        VIB_AMOUNT,
        VIB_SPEED,
        LFO_ONE_SPEED,
        LFO_TWO_SPEED,
        EXCITER_FREQ,
        EXCITER_GAIN,
        RESONATOR_FREQ,
        RESONATOR_Q,
        CHAOS_AMOUNT,
        CHAOS_CHARACTER,
        DETUNE,
        LOWPASS_CUTOFF,
        LOWPASS_Q,
        HIGHPASS_CUTOFF,
        STEREO_WIDTH,
        GAIN,
        _length_
    };

    enum class ModSource {
        AMP_ENV,
        ENV_ONE,
        ENV_TWO,
        VIB_LFO,
        LFO_ONE,
        LFO_TWO,
        VELOCITY,
        PITCH_BEND,
        MOD_WHEEL,
        _length_
    };

    template<typename T>
    static constexpr size_t Size(){
        return static_cast<size_t>(T::_length_);
    }

    template<typename T>
    inline const std::string DisplayName(const T& value);

    template<>
    inline const std::string DisplayName<ModSource>(const ModSource& value){
        const std::array<std::string,Size<ModSource>()> names = {
            "AMP_ENV",
            "ENV_ONE",
            "ENV_TWO",
            "VIB_LFO",
            "LFO_ONE",
            "LFO_TWO",
            "VELOCITY",
            "PITCH_BEND",
            "MOD_WHEEL",
        };
        return names[static_cast<size_t>(value)];
    }

    template<>
    inline const std::string DisplayName<ModDestination>(const ModDestination& value){
        const std::array<std::string, Size<ModDestination>()> names = {
            "VIB_AMOUNT",
            "VIB_SPEED",
            "LFO_ONE_SPEED",
            "LFO_TWO_SPEED",
            "EXCITER_FREQ",
            "EXCITER_GAIN",
            "RESONATOR_FREQ",
            "RESONATOR_Q",
            "CHAOS_AMOUNT",
            "CHAOS_CHARACTER",
            "DETUNE",
            "LOWPASS_CUTOFF",
            "LOWPASS_Q",
            "HIGHPASS_CUTOFF",
            "STEREO_WIDTH",
            "GAIN",
        };
        return names[static_cast<size_t>(value)];
    }

    template<typename sample_t>
    struct lfo_t{
        sample_t phase alignas(16);
        sample_t value alignas(16);
        static const inline lfo_t<sample_t> process(lfo_t<sample_t> lfo, const sample_t& freq){
            lfo.phase = update_phase<sample_t,sample_t>(lfo.phase, freq);
            return lfo;
        }
    };

    template<typename sample_t>
    struct alignas(16) modulators_t{
        adsr_t<sample_t> amp_envelope alignas(16);
        adsr_t<sample_t> mod_envelope_1 alignas(16);
        adsr_t<sample_t> mod_envelope_2 alignas(16);
        lfo_t<sample_t>  vib_lfo alignas(16);
        lfo_t<sample_t>  mod_lfo_1 alignas(16);
        lfo_t<sample_t>  mod_lfo_2 alignas(16);
    };

    template<typename sample_t>
    using ModulationMatrix = std::array<std::array<sample_t,Size<ModDestination>()>,Size<ModSource>()>;

    enum Wave {
        SINE,
        SAW,
        SQUARE,
        TRI
    };

    enum FeedbackMode {
        COS,
        TANH,
        LOWERED_BELL,
        CLIP,
        WRAP
    };

    template<typename sample_t>
    struct params_t {
        ModulationMatrix<sample_t> mod_matrix;
        std::array<sample_t, Size<ModDestination>()> modulatable_params;
        size_t unison = 2;

        Wave wave_mode=SINE;
        FeedbackMode feedback_mode=COS;

        adsr_params_t<sample_t> amp_env_params;
        adsr_params_t<sample_t> mod_env_one_params;
        adsr_params_t<sample_t> mod_env_two_params;

    };

    template<typename sample_t>
    inline const sample_t getModulatedValue(
        const sample_t& value,
        const ModDestination& dest,
        const modulators_t<sample_t>& mods, 
        const ModulationMatrix<sample_t>& matrix
    ){

        sample_t val = value;

        for(size_t source_idx=0; source_idx<Size<ModSource>(); source_idx++){
            const sample_t mod_amount = matrix[source_idx][static_cast<size_t>(dest)];
            switch(source_idx){
                case static_cast<size_t>(ModSource::AMP_ENV): val+= mods.amp_envelope.env.value*mod_amount; break;
                case static_cast<size_t>(ModSource::ENV_ONE): val+= mods.mod_envelope_1.env.value*mod_amount; break;
                case static_cast<size_t>(ModSource::ENV_TWO): val+= mods.mod_envelope_2.env.value*mod_amount; break;
                case static_cast<size_t>(ModSource::VIB_LFO): val+= mods.mod_lfo_1.value*mod_amount; break;
                case static_cast<size_t>(ModSource::LFO_ONE): val+= mods.mod_lfo_1.value*mod_amount; break;
                case static_cast<size_t>(ModSource::LFO_TWO): val+= mods.mod_lfo_1.value*mod_amount; break;
            }
        }

        return val;

    }

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
        val = val>=0 && val<=1 ? val : 0;
        return options[size_t(floor(val*(TABLE_SIZE-1)))];
    }

    template<typename output_t, int TABLE_SIZE>
    output_t lookup_safe(size_t val,  const output_t (&options) [TABLE_SIZE]){
        val = (val>=0) && (val<=(TABLE_SIZE-1)) ? val : 0;
        return options[val];
    }

}