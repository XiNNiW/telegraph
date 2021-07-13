#pragma once
#include <map>
#include <JuceHeader.h>
#include "telegraph_core.h"


class ModMapButtonListener : public juce::Button::Listener, public juce::KeyListener {
    public:
        typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
        ModMapButtonListener(
            TelegraphUIContentComponent& uiComponent,
            juce::AudioProcessorValueTreeState& stateValueTree,
            std::map<telegraph::ModSource, std::map<telegraph::ModDestination,std::unique_ptr<SliderAttachment>>>& knobAttachments
        )
        :ui(uiComponent)
        ,stateTree(stateValueTree)
        ,modKnobAttachments(knobAttachments)
        {
            ui.addKeyListener(this);
        }
        void attach(telegraph::ModSource source, juce::Button* b){
            modButtons[source] = b;
            b->addListener(this);
        }
        void buttonClicked (juce::Button* button) override{
            using telegraph::ModDestination;
            using telegraph::ModSource;
            using telegraph::Size;
            using telegraph::TokenName;

            bool buttonFound=false;
            ModSource source;
            for(auto& it:modButtons){
                if (it.second == button){
                    buttonFound = true;
                    source = it.first;
                    break;
                }
            }


            if(buttonFound){


                bool shouldResetSynthEditMode = ui.currentEditMode == TelegraphUIContentComponent::Mode::MODULATION_EDIT 
                                                && previousSource.has_value() 
                                                && source == previousSource.value();
                if(!shouldResetSynthEditMode) ui.currentModSource = source;
                ui.currentEditMode = shouldResetSynthEditMode
                            ?TelegraphUIContentComponent::Mode::SYNTH_EDIT
                            :TelegraphUIContentComponent::Mode::MODULATION_EDIT;
                
                ui.resized();

                previousSource = std::make_optional(source);
            }
            
            

        }

        bool keyPressed ( 	
            const juce::KeyPress&  	key,
		    juce::Component*  	originatingComponent 
	    ) override {
            if(ui.currentEditMode==TelegraphUIContentComponent::Mode::MODULATION_EDIT && key.getKeyCode() == key.escapeKey ){
                ui.currentEditMode = TelegraphUIContentComponent::Mode::SYNTH_EDIT;
                ui.resized();
                return true;
            } else {
                return false;
            }
        }

    private:
        TelegraphUIContentComponent& ui;
        juce::AudioProcessorValueTreeState& stateTree;
        std::map<telegraph::ModSource, std::map<telegraph::ModDestination,std::unique_ptr<SliderAttachment>>>& modKnobAttachments;
        std::optional<telegraph::ModSource> previousSource = std::nullopt;
        std::optional<DialogWindow*> modulationDialog = std::nullopt;
        std::map<telegraph::ModSource, juce::Button*> modButtons= std::map<telegraph::ModSource, juce::Button*>();
        std::optional<telegraph::ModSource> activeSource = std::nullopt;
        
};

