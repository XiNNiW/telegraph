#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

static const juce::String PROJECT_DIR_PATH = juce::String( 
            juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getFullPathName()
            + juce::File::getSeparatorString() 
            + ProjectInfo::projectName 
        );
static const juce::String PRESET_DIR_PATH  = juce::String(PROJECT_DIR_PATH + juce::File::getSeparatorString() + "Presets");
static const juce::String PRESET_FILE_EXT = juce::String(".telegraph_preset");
class PresetFileManager {
    public:
        PresetFileManager(TelegraphAudioProcessor& proc)
        : processor(proc)
        {
            if (!juce::File (PROJECT_DIR_PATH).exists())
            {
                juce::File (PROJECT_DIR_PATH).createDirectory();
            }

            if (!juce::File (PRESET_DIR_PATH).exists())
            {
                juce::File (PRESET_DIR_PATH).createDirectory();
            }
    
            populatePresetList();
        }
        const size_t getNumPresets(){
            return presetList.size();
        }
        void loadPreset(const size_t& presetNumber){
            auto presetFile = presetList[presetNumber];

            juce::MemoryBlock presetBinary;
    
            if (presetFile.loadFileAsData (presetBinary))
            {
                processor.setStateInformation (presetBinary.getData(), (int) presetBinary.getSize());
                currentPreset = &presetFile;
            }
        }
        void saveCurrentPreset(juce::String name){
            File presetFile = File (PRESET_DIR_PATH + juce::File::getSeparatorString() + name + PRESET_FILE_EXT);
    
            if (!presetFile.exists())
            {
                presetFile.create();
            }
            else
            {
                presetFile.deleteFile();
            }
            
            MemoryBlock presetBinaryData;
            processor.getStateInformation (presetBinaryData);
            
            presetFile.appendData (presetBinaryData.getData(), presetBinaryData.getSize());

            presetList.clear();
            populatePresetList();
            
        }

        const juce::String getPresetName(const size_t& presetNumber){
            return presetList[presetNumber].getFileNameWithoutExtension();
        }

        const juce::String getCurrentPresetName(){
            return currentPreset.has_value()?currentPreset.value()->getFileNameWithoutExtension():"Init Patch";
        }

    private:
        void populatePresetList(){
            auto presetFileSearchWildCard = "*" + PRESET_FILE_EXT;
            for (
                DirectoryEntry entry : RangedDirectoryIterator (File (PRESET_DIR_PATH), false , presetFileSearchWildCard, File::findFiles)
            ){
                juce::File preset = entry.getFile();
                presetList.add (preset);
            }
        }
        juce::Array<File> presetList;
        std::optional<File*> currentPreset = std::nullopt;
        TelegraphAudioProcessor& processor;
};