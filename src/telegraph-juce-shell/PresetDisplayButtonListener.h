#pragma once
#include <JuceHeader.h>
#include "telegraph_core.h"
#include "PresetFileManager.h"

class PresetDisplayButtonListener : public juce::Button::Listener {
    public:
        PresetDisplayButtonListener(TelegraphUIContentComponent& uiComp, TelegraphAudioProcessor& proc)
        :ui(uiComp)
        ,processor(proc)
        ,menu(std::make_unique<juce::PopupMenu>())
        {
            ui.getPresetDisplay()->addListener(this);
            ui.getPresetDisplay()->setButtonText(PresetFileManager::Instance().getCurrentPresetName());
        }
        void buttonClicked(Button* button) override {

            menu->clear();

            menu->addSectionHeader ("SELECT PRESET");
            menu->addSeparator();
            for(size_t prog_idx=0; prog_idx<static_cast<size_t>(PresetFileManager::Instance().getNumPresets()); prog_idx++){
                auto presetName = PresetFileManager::Instance().getPresetName(prog_idx);
                bool isCurrentPreset = presetName == PresetFileManager::Instance().getCurrentPresetName();
                auto maybePresetName = PresetFileManager::Instance().getPresetName(prog_idx);
                if(maybePresetName.has_value()) menu->addItem (prog_idx+1, maybePresetName.value(), true, isCurrentPreset);
            }

            menu->showMenuAsync(
                juce::PopupMenu::Options()
                    .withTargetComponent(button)
                    .withMinimumWidth(button->getBounds().getWidth()), 
                popupCallback
            );

            // int result = menu->show();
            // if(result>0){
            //     PresetFileManager::Instance().loadPreset(result-1, processor);
            //     ui.getPresetDisplay()->setButtonText(PresetFileManager::Instance().getCurrentPresetName());
            // }

        }

    private:
        TelegraphUIContentComponent& ui;
        TelegraphAudioProcessor& processor;
        std::unique_ptr<juce::PopupMenu> menu;
        std::function<void (int)> popupCallback = std::function<void (int)>([&](int result){
            if(result>0){
                PresetFileManager::Instance().loadPreset(result-1, processor);
                ui.getPresetDisplay()->setButtonText(PresetFileManager::Instance().getCurrentPresetName());
                
            }
            
        });
};