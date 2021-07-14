#pragma once
#include <JuceHeader.h>
#include "TelegraphUIContentComponent.h"

class PresetSaveAsModalLauncher : public juce::Button::Listener {
    public:
        PresetSaveAsModalLauncher(TelegraphUIContentComponent& uiComp, PresetFileManager& presetFileManager)
        :ui(uiComp)
        ,presetManager(presetFileManager)
        ,modalComponent(std::make_unique<PresetSaveModalComponent>())
        ,launchOptions(juce::DialogWindow::LaunchOptions())
        {
            launchOptions.dialogTitle = "Save Preset";
            launchOptions.componentToCentreAround = &ui;
            launchOptions.content = OptionalScopedPointer<Component>(modalComponent.get(),false);
            launchOptions.resizable = false;

            ui.getPresetSaveButton()->addListener(this);
            modalComponent->saveButton->addListener(this);
            modalComponent->cancelButton->addListener(this);
        }
        void buttonClicked(Button* button) override {

            if(button == ui.getPresetSaveButton().get()){
                modalComponent->setBounds(
                    ui.getBounds().withSizeKeepingCentre(ui.getBounds().getWidth()/2,ui.getBounds().getWidth()/8)
                );
                modalComponent->presetNameTextBox->setText(presetManager.getCurrentPresetName(), juce::dontSendNotification);
                dialog = std::make_optional(launchOptions.launchAsync());
            }
            else if (button == modalComponent->cancelButton.get()) {
                if(dialog.has_value()) dialog.value()->exitModalState(1234);
            }
            else if( button == modalComponent->saveButton.get()) {
                if(dialog.has_value()) {
                    presetManager.saveCurrentPreset(modalComponent->presetNameTextBox->getText());
                    ui.getPresetDisplay()->setName(presetManager.getCurrentPresetName());
                    dialog.value()->exitModalState(1234);
                }
            }
                
            
        }
    private:
        TelegraphUIContentComponent& ui;
        PresetFileManager& presetManager;
        std::unique_ptr<PresetSaveModalComponent> modalComponent; 
        std::optional<juce::DialogWindow*> dialog = std::nullopt;
        juce::DialogWindow::LaunchOptions launchOptions;
};