#pragma once
#include <map>
#include <JuceHeader.h>
#include "telegraph_core.h"


class ModMapButtonListener : public juce::Button::Listener {
    public:
        ModMapButtonListener(
            TelegraphUIContentComponent& uiComponent
        )
        :ui(uiComponent)
        {}
        void attach(telegraph::ModSource source, juce::Button& b){
            b.addListener(this);
        }
        void buttonClicked (juce::Button* buttonbutton) override{
            using telegraph::ModDestination;
            using telegraph::Size;
            ui.currentEditMode = ui.currentEditMode == TelegraphUIContentComponent::Mode::MODULATION_EDIT
                            ?TelegraphUIContentComponent::Mode::SYNTH_EDIT
                            :TelegraphUIContentComponent::Mode::MODULATION_EDIT;
            ui.resized();


        }

    private:
        TelegraphUIContentComponent& ui;
        // juce::DialogWindow::LaunchOptions launcher;
        std::optional<DialogWindow*> modulationDialog = std::nullopt;
        std::map<telegraph::ModSource, juce::Button&> modButtons= std::map<telegraph::ModSource, juce::Button&>();
        
};

