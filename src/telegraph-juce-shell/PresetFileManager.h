#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SystemConstants.h"

class PresetFileManager {
    public:
        static PresetFileManager& Instance(){
            static PresetFileManager instance;
            
            return instance;
        }
        
        const size_t getNumPresets(){
            return presetList.size();
        }
        void loadPreset(const size_t& presetNumber, TelegraphAudioProcessor& processor){
            auto presetFile = presetList[presetNumber];

            juce::MemoryBlock presetBinary;
    
            if (presetFile.loadFileAsData (presetBinary))
            {
                processor.setStateInformation (presetBinary.getData(), (int) presetBinary.getSize());
                currentPresetName = std::make_optional(presetFile.getFileNameWithoutExtension());
                currentPresetIndex = std::make_optional(presetNumber);

            }
        }
        void saveCurrentPreset(juce::String name, TelegraphAudioProcessor& processor){
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
            currentPresetIndex = std::make_optional(presetList.indexOf(presetFile));
            currentPresetName = std::make_optional(name);
            
        }

        const std::optional<juce::String> getPresetName(const size_t& presetNumber){
            return presetNumber<presetList.size()
                    ?std::make_optional(presetList[presetNumber].getFileNameWithoutExtension())
                    :std::nullopt;
        }

        const juce::String getCurrentPresetName(){
            return currentPresetName.has_value()?currentPresetName.value():"Init Patch";
        }

        PresetFileManager(PresetFileManager const&)=delete;              
        void operator=(PresetFileManager const&)=delete; 

    private:
        
        PresetFileManager()
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
        std::optional<size_t> currentPresetIndex = std::nullopt;
        std::optional<juce::String> currentPresetName = std::nullopt;
};