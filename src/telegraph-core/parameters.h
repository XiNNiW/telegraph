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

    enum class NonModulatedParameter {
        EXCITER_TYPE,
        RESONATOR_TYPE,
        AMP_ATTACK,
        AMP_DECAY,
        AMP_SUSTAIN,
        AMP_RELEASE,
        ENV_ONE_ATTACK,
        ENV_ONE_DECAY,
        ENV_ONE_SUSTAIN,
        ENV_ONE_RELEASE,
        ENV_TWO_ATTACK,
        ENV_TWO_DECAY,
        ENV_TWO_SUSTAIN,
        ENV_TWO_RELEASE,
        UNISON_AMOUNT,
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
    constexpr const char* DisplayName(const T& value);

    template<typename T>
    constexpr const char* TokenName(const T& value);

    template<>
    constexpr const char* DisplayName<ModSource>(const ModSource& value){
        const std::array<const char*,Size<ModSource>()> names = {
            "Amp Env",
            "Mod Env 1",
            "Mod Env 2",
            "Vibrato LFO",
            "Mod LFO 1",
            "Mod LFO 2",
            "velocity",
            "pitch bend",
            "mod wheel"
        };
        return names[static_cast<size_t>(value)];
    }

    template<>
    constexpr const char* TokenName<ModSource>(const ModSource& value){
        const std::array<const char*,Size<ModSource>()> names = {
            "AMP_ENV",
            "ENV_ONE",
            "ENV_TWO",
            "VIB_LFO",
            "LFO_ONE",
            "LFO_TWO",
            "VELOCITY",
            "PITCH_BEND",
            "MOD_WHEEL"
        };
        return names[static_cast<size_t>(value)];
    }

    template<>
    constexpr const char* DisplayName<ModDestination>(const ModDestination& value){
        const std::array<const char*, Size<ModDestination>()> names = {
            "Vibrato Amount",
            "Vibrato Speed",
            "Speed",
            "Speed",
            "Exciter Tune",
            "Exciter Gain",
            "Resonator Tune",
            "Resonator Q",
            "Chaos Amount",
            "Chaos Character",
            "Unison Detune",
            "Lowpass Cutoff",
            "Lowpass Q",
            "Highpass Cutoff",
            "Stereo Width",
            "Gain"
        };
        return names[static_cast<size_t>(value)];
    }

    template<>
    constexpr const char* TokenName<ModDestination>(const ModDestination& value){
        const std::array<const char*, Size<ModDestination>()> names = {
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

    template<>
    constexpr const char* DisplayName<NonModulatedParameter>(const NonModulatedParameter& value){
        const std::array<const char*, Size<NonModulatedParameter>()> names = {
            "Waveform type",
            "Chaos Type",
            "A",
            "D",
            "S",
            "R",
            "A",
            "D",
            "S",
            "R",
            "A",
            "D",
            "S",
            "R",
            "Unison"
        };
        return names[static_cast<size_t>(value)];
    }

    template<>
    constexpr const char* TokenName<NonModulatedParameter>(const NonModulatedParameter& value){
        const std::array<const char*, Size<NonModulatedParameter>()> names = {
            "EXCITER_TYPE",
            "RESONATOR_TYPE",
            "AMP_ATTACK",
            "AMP_DECAY",
            "AMP_SUSTAIN",
            "AMP_RELEASE",
            "ENV_ONE_ATTACK",
            "ENV_ONE_DECAY",
            "ENV_ONE_SUSTAIN",
            "ENV_ONE_RELEASE",
            "ENV_TWO_ATTACK",
            "ENV_TWO_DECAY",
            "ENV_TWO_SUSTAIN",
            "ENV_TWO_RELEASE",
            "UNISON_AMOUNT"
        };
        return names[static_cast<size_t>(value)];
    }

    template<typename sample_t>
    struct lfo_t{
        sample_t phase alignas(16)=0;
        static const inline lfo_t<sample_t> process(lfo_t<sample_t> lfo, const sample_t& freq, const sample_t& controlRate){
            auto increment = freq/controlRate;
            lfo.phase = update_phase<sample_t,sample_t>(lfo.phase, increment);
            return lfo;
        }
    };

    template<typename sample_t>
    struct alignas(16) modulators_t{
        adsr_t<sample_t> amp_envelope alignas(16) = adsr_t<sample_t>();
        adsr_t<sample_t> mod_envelope_1 alignas(16) = adsr_t<sample_t>();
        adsr_t<sample_t> mod_envelope_2 alignas(16) = adsr_t<sample_t>();
        lfo_t<sample_t>  vib_lfo alignas(16) = lfo_t<sample_t>();
        lfo_t<sample_t>  mod_lfo_1 alignas(16) = lfo_t<sample_t>();
        lfo_t<sample_t>  mod_lfo_2 alignas(16) = lfo_t<sample_t>();
        sample_t velocity = 0.75;
        sample_t mod_wheel = 0;
        sample_t pitch_bend = 0.5;
    };

    template<typename sample_t>
    using ModulationMatrix = std::array<std::array<sample_t,Size<ModDestination>()>,Size<ModSource>()>;

    enum class Wave {
        SINE,
        SAW,
        SQUARE,
        TRI
    };

    enum class FeedbackMode {
        COS,
        TANH,
        LOWERED_BELL,
        CLIP,
        WRAP
    };

    enum class ScalingType {
        LINEAR,
        DB,
        FREQ,
        EXP,
        SEMITONES,
        QSEMITONES,
        _length_
    };

    template<typename sample_t>
    constexpr const std::tuple<sample_t,sample_t,sample_t, ScalingType> getParameterRange(const ModDestination& d){
            constexpr sample_t neg_inf = 0;
            constexpr sample_t minus_24_db = 100-24;
            constexpr sample_t minus_ten_db =100-10;
            constexpr sample_t twenty_db = 120;
            switch (d)
            {
            case ModDestination::VIB_AMOUNT:        return  {0,         2,           0,              ScalingType::SEMITONES  }; break;
            case ModDestination::VIB_SPEED:         return  {0,         8,           1.5,            ScalingType::FREQ       }; break;
            case ModDestination::LFO_ONE_SPEED:     return  {0,         8,           0.5,            ScalingType::FREQ       }; break;
            case ModDestination::LFO_TWO_SPEED:     return  {0,         8,           0.5,            ScalingType::FREQ       }; break;
            case ModDestination::EXCITER_FREQ:      return  {-12,       12,          0,              ScalingType::SEMITONES  }; break;
            case ModDestination::EXCITER_GAIN:      return  {neg_inf,   minus_ten_db,minus_24_db,    ScalingType::DB         }; break;
            case ModDestination::RESONATOR_FREQ:    return  {-12,       12,          0,              ScalingType::SEMITONES  }; break;
            case ModDestination::RESONATOR_Q:       return  {0.1,       50,          1,              ScalingType::EXP        }; break;
            case ModDestination::CHAOS_AMOUNT:      return  {neg_inf,   twenty_db,   neg_inf,        ScalingType::DB         }; break;
            case ModDestination::CHAOS_CHARACTER:   return  {0,         100,         100,            ScalingType::EXP        }; break;
            case ModDestination::DETUNE:            return  {0,         1,           0.5,            ScalingType::LINEAR     }; break;
            case ModDestination::LOWPASS_CUTOFF:    return  {200,       20000,       20000,          ScalingType::FREQ       }; break;
            case ModDestination::LOWPASS_Q:         return  {0,         10,          0.1,            ScalingType::EXP        }; break;
            case ModDestination::HIGHPASS_CUTOFF:   return  {30,        1000,        30,             ScalingType::LINEAR     }; break;
            case ModDestination::STEREO_WIDTH:      return  {0,         1,           1,              ScalingType::LINEAR     }; break;
            case ModDestination::GAIN:              return  {neg_inf,   minus_ten_db,minus_24_db,    ScalingType::DB         }; break;
            default:
                return {0,1,0,ScalingType::LINEAR}; 
                break;
            }

    }



    template<typename sample_t>
    struct params_t {
        ModulationMatrix<sample_t> mod_matrix = ModulationMatrix<sample_t>();;
        std::array<sample_t, Size<ModDestination>()> modulatable_params=std::array<sample_t, Size<ModDestination>()>();
        size_t unison = 2;

        Wave wave_mode=Wave::SINE;
        FeedbackMode feedback_mode=FeedbackMode::COS;

        adsr_params_t<sample_t> amp_env_params=adsr_params_t<sample_t>();
        adsr_params_t<sample_t> mod_env_one_params=adsr_params_t<sample_t>();
        adsr_params_t<sample_t> mod_env_two_params=adsr_params_t<sample_t>();

    };

    template<typename sample_t>
    inline const sample_t getModulatedValue(
        const sample_t& value,
        const ModDestination& dest,
        const modulators_t<sample_t>& mods, 
        const ModulationMatrix<sample_t>& matrix
    ){
        using algae::dsp::core::math::lerp;
        using algae::dsp::core::oscillator::sine_t;
        sample_t val = value;

        auto vib_lfo_value  = sine_t<sample_t,0>::lookup( mods.vib_lfo.phase   );
        auto mod_lfo1_value = sine_t<sample_t,0>::lookup( mods.mod_lfo_1.phase );
        auto mod_lfo2_value = sine_t<sample_t,0>::lookup( mods.mod_lfo_2.phase );

        for(size_t source_idx=0; source_idx<Size<ModSource>(); source_idx++){
            const sample_t mod_amount = matrix[source_idx][static_cast<size_t>(dest)];
            if (fabs(mod_amount)>0) {
                switch(source_idx){
                    case static_cast<size_t>(ModSource::AMP_ENV) : val+= convertBipolarSignalValueToParameterRange(mods.amp_envelope.env.value*mod_amount  , dest); break;
                    case static_cast<size_t>(ModSource::ENV_ONE) : val+= convertBipolarSignalValueToParameterRange(mods.mod_envelope_1.env.value*mod_amount, dest); break;
                    case static_cast<size_t>(ModSource::ENV_TWO) : val+= convertBipolarSignalValueToParameterRange(mods.mod_envelope_2.env.value*mod_amount, dest); break;
                    case static_cast<size_t>(ModSource::VIB_LFO) : val+= convertBipolarSignalValueToParameterRange(vib_lfo_value  * mod_amount, dest); break;
                    case static_cast<size_t>(ModSource::LFO_ONE) : val+= convertBipolarSignalValueToParameterRange(mod_lfo1_value * mod_amount, dest); break;
                    case static_cast<size_t>(ModSource::LFO_TWO) : val+= convertBipolarSignalValueToParameterRange(mod_lfo2_value * mod_amount, dest); break;
                    case static_cast<size_t>(ModSource::VELOCITY): val = lerp<sample_t>(val, val*mods.velocity, mod_amount); break;
                }
            }

        }


        return val;
        
    }

    template<typename sample_t>
    sample_t scale_parameter_from_norm(sample_t val, sample_t min, sample_t max){
        
        val = val>=0.0 && val<=1.0 ? val : 0.0;
        return val*(max-min) + min;
    }

    template<typename sample_t>
    sample_t scale_parameter_to_norm(sample_t val, sample_t min, sample_t max){
        
        val = (val-min)/(max-min);
        val = val>=0.0 && val<=1.0 ? val : 0.0;
        return val;
    }

    template<typename sample_t>
    sample_t scale_parameter_from_norm_exp(sample_t val, sample_t min, sample_t max){
        auto m = max<=0?0.0001:log(max);
        auto n = min<=0?0.0001:log(min);
        
        return exp(scale_parameter_from_norm<sample_t>(val,n,m));
    }

    template<typename sample_t>
    sample_t scale_parameter_to_norm_exp(sample_t val, sample_t min, sample_t max){
        auto m = max<=0?0.0001:log(max);
        auto n = min<=0?0.0001:log(min);
        
        return scale_parameter_to_norm<sample_t>(log(val),n,m);
    }

    template<typename sample_t>
    sample_t scale_parameter_as_db(sample_t val){
        if (val<=0) return 0;
        
        val = scale_parameter_from_norm<sample_t>(val, -48.0, 0.0);
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

    template<typename sample_t>
    const inline sample_t convertBipolarSignalValueToParameterRange(const sample_t& signalValue, const ModDestination& param){
        using algae::dsp::core::units::mtof;
        const auto [min,max,defaultValue,scalingType] = getParameterRange<sample_t>(param);
        const sample_t scaled = signalValue*(max-min)*0.5;

        return scaled;

    }


    


}