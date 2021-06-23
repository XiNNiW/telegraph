#include <gtest/gtest.h>
#include "../../src/telegraph-core/voice.h"
#include <iostream>

TEST(DSP_Test, ProcessDoesNotExplode) { 
    constexpr size_t BLOCKSIZE = 64;
    constexpr size_t TABLESIZE = 1024;
    constexpr size_t MAXUNISON = 2;
    constexpr float SR = 48000;
    constexpr float CR = SR/BLOCKSIZE;

    telegraph::params_t<float> p;

    telegraph::voice_t<float,float,2> v = telegraph::initVoice<float,float,2>(telegraph::voice_t<float,float,2>(),SR);
    
    std::array<AudioBlock<float,BLOCKSIZE>,2> blocks;

    v = telegraph::process_control<float, float>(v, p, SR, CR);

    std::tie(v,blocks) = telegraph::process<float,float,TABLESIZE,BLOCKSIZE,MAXUNISON>(v,p,SR);

    for(size_t idx=0; idx<BLOCKSIZE; idx++){
        ASSERT_LT(blocks[0][idx], 1.1);
        ASSERT_GT(blocks[0][idx], -1.1);
        ASSERT_LT(blocks[1][idx], 1.1);
        ASSERT_GT(blocks[1][idx], -1.1);
    }

    v = telegraph::noteOn<float,float,2>(v,p,60.0,100,SR,CR);
    v = telegraph::process_control<float, float>(v, p, SR, CR);

    std::tie(v,blocks) = telegraph::process<float,float,1024,BLOCKSIZE,2>(v,p, SR);
    for(size_t idx=0; idx<BLOCKSIZE; idx++){
        ASSERT_LT(blocks[0][idx], 1.1);
        ASSERT_GT(blocks[0][idx], -1.1);
        ASSERT_LT(blocks[1][idx], 1.1);
        ASSERT_GT(blocks[1][idx], -1.1);
    }

    v = telegraph::noteOff<float,float,2>(v);
    v = telegraph::process_control<float, float>(v, p, SR, CR);

    std::tie(v,blocks) = telegraph::process<float,float,1024,BLOCKSIZE,2>(v,p, SR);
    for(size_t idx=0; idx<BLOCKSIZE; idx++){
        ASSERT_LT(blocks[0][idx], 1.1);
        ASSERT_GT(blocks[0][idx], -1.1);
        ASSERT_LT(blocks[1][idx], 1.1);
        ASSERT_GT(blocks[1][idx], -1.1);
    }

}

TEST(DSP_Test, ProcessDoesNotExplode_polyphonic) { 
    constexpr size_t VOICES = 16;
    constexpr size_t MAX_UNISON = 2;
    constexpr size_t TABLESIZE = 1024;
    constexpr size_t BLOCKSIZE = 64;
    constexpr float SR = 48000;
    constexpr float CR = SR/BLOCKSIZE;

    telegraph::params_t<float> p;
    std::array<telegraph::voice_t<float,float,MAX_UNISON>,VOICES> voices;
    for(size_t voice_idx=0;voice_idx<VOICES; voice_idx++){
        voices[voice_idx] = telegraph::initVoice<float,float,MAX_UNISON>(voices[voice_idx],SR);
    }

    std::array<AudioBlock<float,BLOCKSIZE>,2> blocks;
    
    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);
    for(size_t idx=0; idx<BLOCKSIZE; idx++){
        ASSERT_LT(blocks[0][idx], 1.1);
        ASSERT_GT(blocks[0][idx], -1.1);
        ASSERT_LT(blocks[1][idx], 1.1);
        ASSERT_GT(blocks[1][idx], -1.1);
    }

    
    voices = telegraph::playNote<float, float, VOICES>(voices, p, 1, 60, 100 ,SR, CR);
    voices = telegraph::playNote<float, float, VOICES>(voices, p, 1, 64, 100 ,SR, CR);
    voices = telegraph::playNote<float, float, VOICES>(voices, p, 1, 67, 100 ,SR, CR);
    
    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);
    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);
    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);
    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);
    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);

    for(size_t idx=0; idx<BLOCKSIZE; idx++){
        ASSERT_LT(blocks[0][idx], 1.1);
        ASSERT_GT(blocks[0][idx], -1.1);
        ASSERT_LT(blocks[1][idx], 1.1);
        ASSERT_GT(blocks[1][idx], -1.1);
    }

    voices = telegraph::releaseNote<float, float, VOICES>(voices, p, 1, 60, 0, SR);
    voices = telegraph::releaseNote<float, float, VOICES>(voices, p, 1, 64, 0, SR);
    voices = telegraph::releaseNote<float, float, VOICES>(voices, p, 1, 67, 0, SR);

    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);
    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);
    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);
    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);
    std::tie(voices,blocks) = telegraph::process<float,float,MAX_UNISON,VOICES,TABLESIZE,BLOCKSIZE>(voices ,p, SR);

    for(size_t idx=0; idx<BLOCKSIZE; idx++){
        ASSERT_LT(blocks[0][idx], 1.1);
        ASSERT_GT(blocks[0][idx], -1.1);
        ASSERT_LT(blocks[1][idx], 1.1);
        ASSERT_GT(blocks[1][idx], -1.1);
    }

}


// TEST(DSP_Test, DenormalizeParameter) { 
//     double val;

//     val = telegraph::denormalize<double>(0.0,50.0,100.0);
//     ASSERT_FLOAT_EQ(50.0, val);
//     val = telegraph::denormalize<double>(1.0,50.0,100.0);
//     ASSERT_FLOAT_EQ(100.0, val);
//     val = telegraph::denormalize<double>(0.5,50.0,100.0);
//     ASSERT_FLOAT_EQ(75.0, val);

// }

// TEST(DSP_Test, SetResonatorQ) { 
//     telegraph::params_t<double> initial = telegraph::params_t<double>();
//     initial.resonator_q= 100;
    
//     telegraph::params_t<double> final = telegraph::setQ<double>(initial, 0.532);

//     ASSERT_FLOAT_EQ(0.532, final.resonator_q);
// }

// TEST(DSP_Test, SetResonatorFeedback) { 
//     telegraph::params_t<double> initial = telegraph::params_t<double>();
//     initial.resonator_feedback= 100;
    
//     telegraph::params_t<double> final = telegraph::setFeedback<double>(initial, 0.532);

//     ASSERT_FLOAT_EQ(0.532, final.resonator_feedback);
// }

// TEST(DSP_Test, SetAmpAttack) { 
//     telegraph::params_t<double> initial = telegraph::params_t<double>();
//     initial.amp_attack = 100;
//     const double sample_rate = 1;
    
//     telegraph::params_t<double> final = telegraph::setAmpAttack<double>(initial, 3, sample_rate);

//     ASSERT_FLOAT_EQ(3000.0, final.amp_attack);
// }

// TEST(DSP_Test, SetAmpDecay) { 
//     telegraph::params_t<double> initial = telegraph::params_t<double>();
//     initial.amp_decay= 100;
//     const double sample_rate = 1;
    
//     telegraph::params_t<double> final = telegraph::setAmpDecay<double>(initial, 4, sample_rate);

//     ASSERT_FLOAT_EQ(4000.0, final.amp_decay);
// }

// TEST(DSP_Test, SetAmpSustain) { 
//     telegraph::params_t<double> initial = telegraph::params_t<double>();
//     initial.amp_sustain= 100;
//     const double sample_rate = 1;
    
//     telegraph::params_t<double> final = telegraph::setAmpSustain<double>(initial, 0.5);

//     ASSERT_FLOAT_EQ(0.5, final.amp_sustain);
// }

// TEST(DSP_Test, SetAmpRelease) { 
//     telegraph::params_t<double> initial = telegraph::params_t<double>();
//     initial.amp_release= 100;
//     const double sample_rate = 1;
    
//     telegraph::params_t<double> final = telegraph::setAmpRelease<double>(initial, 5, sample_rate);

//     ASSERT_FLOAT_EQ(5000.0, final.amp_release);
// }

// // TEST(DSP_Test, SetExciterAttack) { 
// //     telegraph::params_t<double> initial = telegraph::params_t<double>();
// //     initial.exciter_attack= 100;
// //     const double sample_rate = 1;
    
// //     telegraph::params_t<double> final = telegraph::setExciterAttack<double>(initial, 6, sample_rate);

// //     ASSERT_FLOAT_EQ(6000.0, final.exciter_attack);
// // }

// // TEST(DSP_Test, SetExciterDecay) { 
// //     telegraph::params_t<double> initial = telegraph::params_t<double>();
// //     initial.exciter_decay= 100;
// //     const double sample_rate = 1;
    
// //     telegraph::params_t<double> final = telegraph::setExciterDecay<double>(initial, 7, sample_rate);

// //     ASSERT_FLOAT_EQ(7000.0, final.exciter_decay);
// // }



