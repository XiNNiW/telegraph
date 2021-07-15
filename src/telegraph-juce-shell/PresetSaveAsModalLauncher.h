#pragma once
#include <JuceHeader.h>
#include "TelegraphUIContentComponent.h"

class PresetSaveAsModalLauncher : public juce::Button::Listener {
    public:
        PresetSaveAsModalLauncher(TelegraphUIContentComponent& uiComp, TelegraphAudioProcessor& proc)
        :ui(uiComp)
        ,processor(proc)
        {
            ui.getPresetSaveButton()->addListener(this);
            ui.getPresetSaveDialog().saveButton->addListener(this);
            ui.getPresetSaveDialog().cancelButton->addListener(this);
        }
        void buttonClicked(Button* button) override {

            if(button == ui.getPresetSaveButton().get()){
                ui.getPresetSaveDialog().setBounds(
                    ui.getBounds().withSizeKeepingCentre(ui.getBounds().getWidth()/2,ui.getBounds().getWidth()/6)
                );
                ui.getPresetSaveDialog().presetNameTextBox->setText(PresetFileManager::Instance().getCurrentPresetName(), juce::dontSendNotification);
                ui.showPresetSaveDialog();
                ui.resized();
            }
            else if (button == ui.getPresetSaveDialog().cancelButton.get()) {
                ui.hidePresetSaveDialog();
                ui.resized();
            }
            else if( button == ui.getPresetSaveDialog().saveButton.get()) {
                auto presetName = ui.getPresetSaveDialog().presetNameTextBox->getText();
                PresetFileManager::Instance().saveCurrentPreset(presetName, processor);
                ui.getPresetDisplay()->setButtonText(presetName);
                ui.hidePresetSaveDialog();
                ui.resized();
            }
                
            
        }
    private:
        TelegraphUIContentComponent& ui;
        TelegraphAudioProcessor& processor;

};