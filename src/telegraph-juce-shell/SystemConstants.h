#pragma once

static const juce::String PROJECT_DIR_PATH = juce::String( 
            juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getFullPathName()
            + juce::File::getSeparatorString() 
            + ProjectInfo::projectName 
        );
static const juce::String PRESET_DIR_PATH  = juce::String(PROJECT_DIR_PATH + juce::File::getSeparatorString() + "Presets");
static const juce::String PRESET_FILE_EXT = juce::String(".telegraph_preset");