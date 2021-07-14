#pragma once
#include <JuceHeader.h>
#include "telegraph_core.h"
#include "PresetFileManager.h"

class PresetDisplayButtonListener : public juce::Button::Listener, public ModalComponentManager::Callback {
    public:
        PresetDisplayButtonListener(TelegraphUIContentComponent& uiComp, PresetFileManager& presetFileManager)
        :ui(uiComp)
        ,presetManager(presetFileManager)
        {
            ui.getPresetDisplay()->addListener(this);
            ui.getPresetDisplay()->setName(presetManager.getCurrentPresetName());
        }
        void buttonClicked(Button* button) override {
            juce::PopupMenu m;
            for(size_t prog_idx=0; prog_idx<static_cast<size_t>(presetManager.getNumPresets()); prog_idx++){
                m.addItem (prog_idx, presetManager.getPresetName(prog_idx));
            }
            //m.showAt(button,0,0,0,0,this);
            int result = m.showAt(button);
            if(result>0) {
                presetManager.loadPreset(result);
            }

        }
        void modalStateFinished (int returnValue) override {
            //presetManager.loadPreset(returnValue);
        }
    private:
        TelegraphUIContentComponent& ui;
        PresetFileManager& presetManager;
};