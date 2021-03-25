#include <gtest/gtest.h>
#include "../../src/telegraph-core/voice.h"
#include <iostream>




TEST(DSP_Test, ProcessDoesNotExplode) { 
    // params_t {
    //     sample_t resonator_q;
    //     sample_t resonator_feedback;
    //     int amp_attack;
    //     int amp_decay;
    //     sample_t amp_sustain;
    //     int amp_release;
    //     int exciter_attack;
    //     int exciter_decay;
    // };
    telegraph::params_t<double> p = telegraph::params_t<double>{
        0.9,10.5,44100,44100,1.0,44100,44100
    };
    telegraph::voice_t<double> v = telegraph::initVoice<double>(telegraph::voice_t<double>(),44100);
    v = telegraph::process<double,double>(v,p, 44100);
    ASSERT_LT(v.output, 1.1);
    ASSERT_GT(v.output, -1.1);

    v = telegraph::process<double,double>(v, p, 44100.0);
    v = telegraph::process<double,double>(v, p, 44100.0);
    v = telegraph::process<double,double>(v, p, 44100.0);
    ASSERT_LT(v.output, 1.1);
    ASSERT_GT(v.output, -1.1);

    v = telegraph::noteOn<double>(v,p,60.0,44100.0);

    v = telegraph::process<double,double>(v, p, 44100.0);
    v = telegraph::process<double,double>(v, p, 44100.0);
    v = telegraph::process<double,double>(v, p, 44100.0);
    ASSERT_LT(v.output, 1.1);
    ASSERT_GT(v.output, -1.1);

}


TEST(DSP_Test, DenormalizeParameter) { 
    double val;

    val = telegraph::denormalize<double>(0.0,50.0,100.0);
    ASSERT_FLOAT_EQ(50.0, val);
    val = telegraph::denormalize<double>(1.0,50.0,100.0);
    ASSERT_FLOAT_EQ(100.0, val);
    val = telegraph::denormalize<double>(0.5,50.0,100.0);
    ASSERT_FLOAT_EQ(75.0, val);

}

TEST(DSP_Test, SetResonatorQ) { 
    telegraph::params_t<double> initial = telegraph::params_t<double>();
    initial.resonator_q= 100;
    
    telegraph::params_t<double> final = telegraph::setQ<double>(initial, 0.532);

    ASSERT_FLOAT_EQ(0.532, final.resonator_q);
}

TEST(DSP_Test, SetResonatorFeedback) { 
    telegraph::params_t<double> initial = telegraph::params_t<double>();
    initial.resonator_feedback= 100;
    
    telegraph::params_t<double> final = telegraph::setFeedback<double>(initial, 0.532);

    ASSERT_FLOAT_EQ(0.532, final.resonator_feedback);
}

TEST(DSP_Test, SetAmpAttack) { 
    telegraph::params_t<double> initial = telegraph::params_t<double>();
    initial.amp_attack = 100;
    const double sample_rate = 1;
    
    telegraph::params_t<double> final = telegraph::setAmpAttack<double>(initial, 3, sample_rate);

    ASSERT_FLOAT_EQ(3000.0, final.amp_attack);
}

TEST(DSP_Test, SetAmpDecay) { 
    telegraph::params_t<double> initial = telegraph::params_t<double>();
    initial.amp_decay= 100;
    const double sample_rate = 1;
    
    telegraph::params_t<double> final = telegraph::setAmpDecay<double>(initial, 4, sample_rate);

    ASSERT_FLOAT_EQ(4000.0, final.amp_decay);
}

TEST(DSP_Test, SetAmpSustain) { 
    telegraph::params_t<double> initial = telegraph::params_t<double>();
    initial.amp_sustain= 100;
    const double sample_rate = 1;
    
    telegraph::params_t<double> final = telegraph::setAmpSustain<double>(initial, 0.5);

    ASSERT_FLOAT_EQ(0.5, final.amp_sustain);
}

TEST(DSP_Test, SetAmpRelease) { 
    telegraph::params_t<double> initial = telegraph::params_t<double>();
    initial.amp_release= 100;
    const double sample_rate = 1;
    
    telegraph::params_t<double> final = telegraph::setAmpRelease<double>(initial, 5, sample_rate);

    ASSERT_FLOAT_EQ(5000.0, final.amp_release);
}

TEST(DSP_Test, SetExciterAttack) { 
    telegraph::params_t<double> initial = telegraph::params_t<double>();
    initial.exciter_attack= 100;
    const double sample_rate = 1;
    
    telegraph::params_t<double> final = telegraph::setExciterAttack<double>(initial, 6, sample_rate);

    ASSERT_FLOAT_EQ(6000.0, final.exciter_attack);
}

TEST(DSP_Test, SetExciterDecay) { 
    telegraph::params_t<double> initial = telegraph::params_t<double>();
    initial.exciter_decay= 100;
    const double sample_rate = 1;
    
    telegraph::params_t<double> final = telegraph::setExciterDecay<double>(initial, 7, sample_rate);

    ASSERT_FLOAT_EQ(7000.0, final.exciter_decay);
}



