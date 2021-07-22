#include <gtest/gtest.h>
#include "../../src/telegraph-core/voice.h"
#include <iostream>

using telegraph::ModDestination;
using telegraph::Size;
using telegraph::DisplayName;
using telegraph::TokenName;


TEST(Parameters_Test, ModulatableParams_size) { 
    EXPECT_EQ(16, Size<ModDestination>());
}

TEST(Parameters_Test, ModulatableParams_token_name_unique){
    for(size_t left=0; left<Size<ModDestination>(); left++){
        for(size_t right=0; right<Size<ModDestination>(); right++){
            if(left!=right){
                auto leftName =TokenName<ModDestination>(ModDestination(left));
                auto rightName =TokenName<ModDestination>(ModDestination(right));
                EXPECT_NE(leftName,rightName);
            }
        }
    }
}

using telegraph::NonModulatedParameter;

TEST(Parameters_Test, NonModulatableParams_size) { 
    EXPECT_EQ(15, Size<NonModulatedParameter>());
}

TEST(Parameters_Test, NonModulatableParams_token_name_unique){
    for(size_t left=0; left<Size<NonModulatedParameter>(); left++){
        for(size_t right=0; right<Size<NonModulatedParameter>(); right++){
            if(left!=right){
                auto leftName =TokenName<NonModulatedParameter>(NonModulatedParameter(left));
                auto rightName =TokenName<NonModulatedParameter>(NonModulatedParameter(right));
                EXPECT_NE(leftName,rightName);
            }
        }
    }
}

using telegraph::ModSource; 
TEST(Parameters_Test, ModSource_size) { 
    EXPECT_EQ(9, Size<ModSource>());
}

TEST(Parameters_Test, ModSource_token_name_unique){
    for(size_t left=0; left<Size<ModSource>(); left++){
        for(size_t right=0; right<Size<ModSource>(); right++){
            if(left!=right){
                auto leftName =TokenName<ModSource>(ModSource(left));
                auto rightName =TokenName<ModSource>(ModSource(right));
                EXPECT_NE(leftName,rightName);
            }
        }
    }
}

using telegraph::lfo_t;
TEST(Parameters_Test, LFO){
    lfo_t<float> lfo;
    lfo.phase=0;

    EXPECT_EQ(0,lfo.phase);

    lfo = lfo_t<float>::process(lfo,1,64);

    EXPECT_EQ(0.015625, lfo.phase);
}

using telegraph::getParameterRange;
using telegraph::convertBipolarSignalValueToParameterRange;
using telegraph::ScalingType;
TEST(Parameters_Test, convertBipolarSignalValueToParameterRange_db){
    const auto[min,max,defaultVal,scalingType] = getParameterRange<float>(ModDestination::CHAOS_AMOUNT);
    ASSERT_EQ(0,min);
    ASSERT_EQ(120,max);
    ASSERT_EQ(0,defaultVal);
    ASSERT_EQ(ScalingType::DB,scalingType);

    EXPECT_EQ(-60, convertBipolarSignalValueToParameterRange(-1, ModDestination::CHAOS_AMOUNT));
    EXPECT_EQ(0, convertBipolarSignalValueToParameterRange(0, ModDestination::CHAOS_AMOUNT));
    EXPECT_EQ(60, convertBipolarSignalValueToParameterRange(1, ModDestination::CHAOS_AMOUNT));

}

TEST(Parameters_Test, convertBipolarSignalValueToParameterRange_freq){
    const auto[min,max,defaultVal,scalingType] = getParameterRange<float>(ModDestination::EXCITER_FREQ);
    ASSERT_EQ(-12,min);
    ASSERT_EQ(12,max);
    ASSERT_EQ(0, defaultVal);
    ASSERT_EQ(ScalingType::SEMITONES, scalingType);

    EXPECT_EQ(-12, convertBipolarSignalValueToParameterRange(-1, ModDestination::EXCITER_FREQ));
    EXPECT_EQ(0, convertBipolarSignalValueToParameterRange(0, ModDestination::EXCITER_FREQ));
    EXPECT_EQ(12, convertBipolarSignalValueToParameterRange(1, ModDestination::EXCITER_FREQ));

}


using telegraph::modulators_t; 
using telegraph::ModulationMatrix;
using telegraph::getModulatedValue;
TEST(Parameters_Test, getModulatedValue){

    const auto[min,max,defaultVal,scalingType] = getParameterRange<float>(ModDestination::CHAOS_AMOUNT);
    ASSERT_EQ(0,min);
    ASSERT_EQ(120,max);
    ASSERT_EQ(0,defaultVal);
    ASSERT_EQ(ScalingType::DB,scalingType);

    modulators_t<float> mods;
    ModulationMatrix<float> mod_matrix = ModulationMatrix<float>();
    
    mod_matrix[static_cast<size_t>(ModSource::LFO_ONE)][static_cast<size_t>(ModDestination::CHAOS_AMOUNT)] = 1;
    mod_matrix[static_cast<size_t>(ModSource::LFO_ONE)][static_cast<size_t>(ModDestination::CHAOS_CHARACTER)] = 0.5;
    mod_matrix[static_cast<size_t>(ModSource::LFO_TWO)][static_cast<size_t>(ModDestination::CHAOS_CHARACTER)] = 1;
    mod_matrix[static_cast<size_t>(ModSource::LFO_ONE)][static_cast<size_t>(ModDestination::DETUNE)] = 1;
    mod_matrix[static_cast<size_t>(ModSource::LFO_TWO)][static_cast<size_t>(ModDestination::DETUNE)] = -1;

    mods.mod_lfo_1.phase = 0.25;
    auto lfo1value = sine_t<float, 0>::lookup(mods.mod_lfo_1.phase);
    ASSERT_EQ(1.0f, lfo1value);
    EXPECT_EQ(85, getModulatedValue<float>(25,ModDestination::CHAOS_AMOUNT, mods, mod_matrix));

    mods.mod_lfo_1.phase = 0.125;
    lfo1value = sine_t<float, 0>::lookup(mods.mod_lfo_1.phase);
    ASSERT_NEAR(0.707107f, lfo1value,0.0001);

    EXPECT_NEAR(67.4264, getModulatedValue<float>(25,ModDestination::CHAOS_AMOUNT, mods, mod_matrix),0.01);

    mods.mod_lfo_1.phase = 0.75;
    lfo1value = sine_t<float, 0>::lookup(mods.mod_lfo_1.phase);
    ASSERT_NEAR(-1.0f, lfo1value,0.0001);

    EXPECT_EQ(-35, getModulatedValue<float>(25,ModDestination::CHAOS_AMOUNT, mods, mod_matrix));
    
}