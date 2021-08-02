#pragma once
#include <JuceHeader.h>
#include "telegraph_core.h"
#include "TelegraphFontManager.h"
#include "TelegraphLookAndFeelProvider.h"
#include "fonts/FAW.h"
#include <utility>
#include <optional>

using telegraph::ModDestination;
using telegraph::NonModulatedParameter;

static std::unique_ptr<juce::Slider> makeTelegraphKnob(){
    
    auto knob=std::make_unique<juce::Slider>();
    knob->setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    // knob->setTextBoxStyle (juce::Slider::TextBoxAbove, true, 20, 0);
    
    knob->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 10);
    knob->setTextBoxIsEditable(true);
    knob->showTextBox();
    knob->setColour(knob->textBoxTextColourId, juce::Colour(0xaa000000));
    knob->setColour(knob->textBoxOutlineColourId, juce::Colours::transparentBlack);
    
    return knob;
}

static void styleTelegraphModulationKnob(juce::Slider* knob){
    knob->setLookAndFeel(TelegraphLookAndFeelProvider::Instance().getModulationKnobLookAndFeel());
    knob->setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    knob->setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 10);
    knob->setTextBoxIsEditable(true);
    knob->showTextBox();
    knob->setColour(knob->textBoxTextColourId, juce::Colours::transparentBlack);
    knob->setColour(knob->textBoxOutlineColourId, juce::Colours::transparentBlack);
    knob->setColour(juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
    knob->setColour(juce::Slider::thumbColourId, juce::Colours::transparentBlack);
    knob->setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::turquoise);
    knob->setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::transparentBlack);
    knob->setTextBoxIsEditable(true);
    knob->toBack();
    knob->setVisible(false);
   
}

static std::unique_ptr<juce::Slider> makeTelegraphModulationAmountKnob(){
    auto knob=std::make_unique<juce::Slider>();
    styleTelegraphModulationKnob(knob.get());
    
    return knob;
}

static std::unique_ptr<juce::TextButton> makeTelegraphModMapButton(){
    auto button = std::make_unique<juce::TextButton>(FAW::RADIO_ICON_UNICODE,"");
    // button->setSize(40,40);
    // button->setColour(juce::TextButton::button)
    
    button->setLookAndFeel(TelegraphLookAndFeelProvider::Instance().getIconLookAndFeel());
    // getLookAndFeel().getTextButtonFont(*button, 18) = TelegraphFontManager::Instance().getIconFont();
    button->setColour(TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    button->setColour(TextButton::buttonColourId, juce::Colours::transparentBlack);

    button->setColour(TextButton::textColourOffId, juce::Colour(0x55000000));
    button->setColour(TextButton::textColourOnId, juce::Colour(0x55000000));
    return button;
}

template<typename T>
static std::unique_ptr<juce::Label> makeTelegraphKnobLabel(juce::Component* knob, T paramId){
    auto label = std::make_unique<juce::Label>();
    label->setColour(juce::Label::backgroundColourId, juce::Colour(0x5528163d ));
    label->setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    // label->setColour(juce::Label::outlineColourId, juce::Colours::gre);
    label->setText(
        juce::String(telegraph::DisplayName<T>(paramId)).toUpperCase(),
        juce::dontSendNotification
    );
    label->setFont(TelegraphFontManager::Instance().getLabelFont());
    label->attachToComponent(knob, false);
    label->setJustificationType(Justification::centred);
    return label;
}


static std::unique_ptr<juce::Label> makeKnobLabel(juce::Component* knob, std::string displayName){
    auto label = std::make_unique<juce::Label>();
    label->setFont(TelegraphFontManager::Instance().getLabelFont());
    // label->setColour(juce::Label::backgroundColourId, juce::Colour(0xff28163d ));
    label->setColour(juce::Label::textColourId, juce::Colours::lightgrey);
    // label->setColour(juce::Label::outlineColourId, juce::Colour(0x5528163d ));
    label->setText(
        displayName,
        juce::dontSendNotification
    );
    label->attachToComponent(knob, false);
    label->setJustificationType(Justification::centred);
    return label;
}

static void positionKnobOnPanelGrid(
    juce::Rectangle<int> panelBounds, 
    const size_t& knobsPerRow, 
    const size_t& knobPos, 
    juce::Component& knob
){
    auto leftInset = 15;
    auto labelHeight = 15;
    panelBounds.removeFromLeft(leftInset);
    auto knobSize = panelBounds.getWidth()/knobsPerRow ;
    auto xLoc = leftInset + (knobSize * (knobPos % knobsPerRow));
    auto yLoc = ((knobSize + labelHeight) * (knobPos / knobsPerRow)) + panelBounds.getY();
    auto width = knobSize;
    auto height = knobSize;

    knob.setBounds (
        xLoc,yLoc,width*0.9,height*0.9
    );
                    
}

struct PresetSaveModalComponent: public juce::Component {
    PresetSaveModalComponent()
    :dialogTitle(juce::Label())
    ,presetNameTextBox(std::make_unique<juce::Label>())
    ,cancelButton(std::make_unique<juce::TextButton>("Cancel"))
    ,saveButton(std::make_unique<juce::TextButton>("Save"))
    {
        presetNameTextBox->setEditable(true, false, false);
        presetNameTextBox->setColour(juce::Label::backgroundColourId, juce::Colours::darkslateblue);
        presetNameTextBox->setFont(TelegraphFontManager::Instance().getLabelFont());
        dialogTitle.setText("Save As Preset?",juce::dontSendNotification);
        dialogTitle.setColour(juce::Label::backgroundColourId, juce::Colours::black);
        
        addAndMakeVisible(dialogTitle);
        addAndMakeVisible(*presetNameTextBox);
        addAndMakeVisible(*cancelButton);
        addAndMakeVisible(*saveButton);
    }
    void resized() override {
        auto bounds = getLocalBounds();
        dialogTitle.setBounds(bounds.removeFromTop(bounds.getHeight()/6));
        auto dialogWidth = bounds.getWidth();
        auto dialogHeight = bounds.getHeight();
        auto topInset = bounds.getHeight()/16;
        auto elementHeight = dialogHeight/2;
        auto editBoxBounds = bounds.removeFromTop(elementHeight+topInset).withSizeKeepingCentre( dialogWidth*0.9, elementHeight*0.9);
        presetNameTextBox->setBounds(editBoxBounds);
        auto buttonWidth = bounds.getWidth()/2;
        cancelButton->setBounds(bounds.removeFromRight(buttonWidth).withSizeKeepingCentre( buttonWidth*0.8, elementHeight*0.8));
        saveButton->setBounds(bounds.withSizeKeepingCentre(buttonWidth*0.8, elementHeight*0.8));
    }
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::darkgrey);
    }
    juce::Label dialogTitle;
    std::unique_ptr<juce::Label>  presetNameTextBox;
    std::unique_ptr<juce::Button> cancelButton;
    std::unique_ptr<juce::Button> saveButton;
};

class TelegraphUIContentComponent : public juce::Component {
    
    public:
        enum
        {
            paramControlHeight = 80,
            paramLabelWidth    = 80,
            paramSliderWidth   = 300
        };
        enum class Mode {
            SYNTH_EDIT,
            MODULATION_EDIT,
            // PRESET_SAVE
        };
        TelegraphUIContentComponent()
        :presetSaveDialog()
        ,overlayPanel()
        ,headerPanel()
        ,modulePanel()
        {
            using telegraph::ModSource;
            using telegraph::ModDestination;
            using telegraph::NonModulatedParameter;
            using telegraph::Size;
            addAndMakeVisible(headerPanel);
            addAndMakeVisible(modulePanel);
            addAndMakeVisible(overlayPanel);
            addAndMakeVisible(presetSaveDialog);
            overlayPanel.setVisible(false);
            presetSaveDialog.setVisible(false);

            for(size_t source_idx=0; source_idx<Size<ModSource>(); source_idx++){
                for(size_t dest_idx=0; dest_idx<Size<ModDestination>(); dest_idx++){
                    modulationAmountKnobs[ModSource(source_idx)][ModDestination(dest_idx)] = makeTelegraphModulationAmountKnob();
                    addAndMakeVisible(*modulationAmountKnobs[ModSource(source_idx)][ModDestination(dest_idx)]);
                }
            }
            modulationGlow.setGlowProperties(2.2, juce::Colour(0x88ffffff));
        }
        void paint (juce::Graphics& g) override
        {
            g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
        }
        void resized() override
        {
            using telegraph::Size;
            using telegraph::ModDestination;
            using telegraph::ModSource;
            auto bounds = getLocalBounds();
            overlayPanel.setBounds(bounds);

            auto height = bounds.getHeight();

            headerPanel.setBounds  (bounds.removeFromTop  (height/16));
            modulePanel.setBounds  (bounds.removeFromTop  (15*height/16));
            
            for(size_t idx=0; idx<Size<ModDestination>(); idx++){
                auto maybeComponent = getModulationDestinationComponent(ModDestination(idx));
                if(maybeComponent){

                    auto targetKnob = maybeComponent.value();
              
                    if(currentEditMode==Mode::MODULATION_EDIT){

                        for(size_t source_idx=0; source_idx<Size<ModSource>(); source_idx++){
                            modulationAmountKnobs[ModSource(source_idx)][ModDestination(idx)]->setVisible(false);
                            modulationAmountKnobs[ModSource(source_idx)][ModDestination(idx)]->toBack();
                            targetKnob->setComponentEffect (nullptr);
                        }
                        
                        auto targetBounds = modulationAmountKnobs[currentModSource][ModDestination(idx)]->getParentComponent()->getLocalArea (targetKnob, targetKnob->getLocalBounds());
                        modulationAmountKnobs[currentModSource][ModDestination(idx)]->setBounds(targetBounds);
                        modulationAmountKnobs[currentModSource][ModDestination(idx)]->setVisible(true);
                        modulationAmountKnobs[currentModSource][ModDestination(idx)]->toFront(false);
                        
                        targetKnob->setComponentEffect (&modulationGlow);
                    }
                    else {
                        for(size_t source_idx=0; source_idx<Size<ModSource>(); source_idx++){
                            modulationAmountKnobs[ModSource(source_idx)][ModDestination(idx)]->setVisible(false);
                            modulationAmountKnobs[ModSource(source_idx)][ModDestination(idx)]->toBack();
                            targetKnob->setComponentEffect (nullptr);
                        }
                        
                    }
                    
                }
            }

        }
        // EXCITER PANEL
        std::unique_ptr<juce::ComboBox>& getExciterTypeSelector(){return modulePanel.topRow.exciterPanel.exciterSelector;}
        std::unique_ptr<juce::Slider>& getExciterTuneKnob(){return modulePanel.topRow.exciterPanel.exciterTuneKnob;}
        std::unique_ptr<juce::Slider>& getUnisonAmountKnob(){return modulePanel.topRow.exciterPanel.unisonKnob;}
        std::unique_ptr<juce::Slider>& getUnisonDetuneKnob(){return modulePanel.topRow.exciterPanel.detuneKnob;}
        std::unique_ptr<juce::Slider>& getVibratoDepthKnob(){return modulePanel.topRow.exciterPanel.vibratoAmountKnob;}
        std::unique_ptr<juce::Slider>& getVibratoSpeedKnob(){return modulePanel.topRow.exciterPanel.vibratoSpeedKnob;}
        std::unique_ptr<juce::Slider>& getExciterGainKnob(){return modulePanel.topRow.exciterPanel.exciterGainKnob;}
        // RESONATOR PANEL
        std::unique_ptr<juce::ComboBox>& getResontorTypeSelector(){return modulePanel.topRow.resonatorPanel.resonatorSelector;}
        std::unique_ptr<juce::Slider>& getResonatorTuneKnob(){return modulePanel.topRow.resonatorPanel.resonatorTuneKnob;}
        std::unique_ptr<juce::Slider>& getResonatorQKnob(){return modulePanel.topRow.resonatorPanel.resonatorQKnob;}
        std::unique_ptr<juce::Slider>& getChaosAmountKnob(){return modulePanel.topRow.resonatorPanel.chaosAmountKnob;}
        std::unique_ptr<juce::Slider>& getChaosCharacterKnob(){return modulePanel.topRow.resonatorPanel.chaosCharacterKnob;}
        //AMP PANEL
        std::unique_ptr<juce::Slider>& getLowpasCutoffKnob(){return modulePanel.topRow.ampPanel.lowpassCutoffKnob;}
        std::unique_ptr<juce::Slider>& getLowpassQKnob(){return modulePanel.topRow.ampPanel.lowpassQKnob;}
        std::unique_ptr<juce::Slider>& getHighpassCutoffKnob(){return modulePanel.topRow.ampPanel.highpassCutoffKnob;}
        std::unique_ptr<juce::TextButton>& getAmpMapButton(){return modulePanel.topRow.ampPanel.ampMapBtn;}
        std::unique_ptr<juce::Slider>& getAmpAttackKnob(){return modulePanel.topRow.ampPanel.ampAttackKnob;}
        std::unique_ptr<juce::Slider>& getAmpDecayKnob(){return modulePanel.topRow.ampPanel.ampDecayKnob;}
        std::unique_ptr<juce::Slider>& getAmpSustainKnob(){return modulePanel.topRow.ampPanel.ampSustainKnob;}
        std::unique_ptr<juce::Slider>& getAmpReleaseKnob(){return modulePanel.topRow.ampPanel.ampReleaseKnob;}
        std::unique_ptr<juce::Slider>& getMainGainKnob(){return modulePanel.topRow.ampPanel.finalGainKnob;}
        //MODULATION PANL
        std::unique_ptr<juce::TextButton>& getModEnv1MapButton(){return modulePanel.bottomRow.modEnv1MapBtn;}
        std::unique_ptr<juce::Slider>& getModEnv1AttackKnob(){return modulePanel.bottomRow.modEnv1AttackKnob;}
        std::unique_ptr<juce::Slider>& getModEnv1DecayKnob(){return modulePanel.bottomRow.modEnv1DecayKnob;}
        std::unique_ptr<juce::Slider>& getModEnv1SustainKnob(){return modulePanel.bottomRow.modEnv1SustainKnob;}
        std::unique_ptr<juce::Slider>& getModEnv1ReleaseKnob(){return modulePanel.bottomRow.modEnv1ReleaseKnob;}
        std::unique_ptr<juce::TextButton>& getModEnv2MapButton(){return modulePanel.bottomRow.modEnv2MapBtn;}
        std::unique_ptr<juce::Slider>& getModEnv2AttackKnob(){return modulePanel.bottomRow.modEnv2AttackKnob;}
        std::unique_ptr<juce::Slider>& getModEnv2DecayKnob(){return modulePanel.bottomRow.modEnv2DecayKnob;}
        std::unique_ptr<juce::Slider>& getModEnv2SustainKnob(){return modulePanel.bottomRow.modEnv2SustainKnob;}
        std::unique_ptr<juce::Slider>& getModEnv2ReleaseKnob(){return modulePanel.bottomRow.modEnv2ReleaseKnob;}
        std::unique_ptr<juce::TextButton>& getModLFO1MapButton(){return modulePanel.bottomRow.modLFO1MapBtn;}
        std::unique_ptr<juce::Slider>& getModLFO1SpeedKnob(){return modulePanel.bottomRow.modLFO1SpeedKnob;}
        std::unique_ptr<juce::TextButton>& getModLFO2MapButton(){return modulePanel.bottomRow.modLFO2MapBtn;}
        std::unique_ptr<juce::Slider>& getModLFO2SpeedKnob(){return modulePanel.bottomRow.modLFO2SpeedKnob;}
        std::unique_ptr<juce::TextButton>& getPresetDisplay(){return headerPanel.presetDisplay;}
        std::unique_ptr<juce::TextButton>& getPresetSaveButton(){return headerPanel.presetSaveButton;}
        PresetSaveModalComponent& getPresetSaveDialog(){
            return presetSaveDialog;
        }
        void showModalOverlay(){
            overlayPanel.setVisible(true);
            overlayPanel.toFront(false);
        }
        void hideModalOverlay(){
            overlayPanel.setVisible(false);
        }
        void showPresetSaveDialog(){
            showModalOverlay();
            presetSaveDialog.setVisible(true);
            presetSaveDialog.toFront(true);
        }
        void hidePresetSaveDialog(){
            hideModalOverlay();
            presetSaveDialog.setVisible(false);
            presetSaveDialog.toBack();
        }
        const std::optional<juce::Slider*> getModulationDestinationComponent(ModDestination paramId){
            switch (paramId)
            {
            case ModDestination::VIB_AMOUNT:
                return std::optional{getVibratoDepthKnob().get()};
                break;
            case ModDestination::VIB_SPEED:
                return std::optional{getVibratoSpeedKnob().get()};
                break;
            case ModDestination::LFO_ONE_SPEED:
                return std::optional{getModLFO1SpeedKnob().get()};
                break;
            case ModDestination::LFO_TWO_SPEED:
                return std::optional{getModLFO2SpeedKnob().get()};
                break;
            case ModDestination::EXCITER_FREQ:
                return std::optional{getExciterTuneKnob().get()};
                break;
            case ModDestination::EXCITER_GAIN:
                return std::optional{getExciterGainKnob().get()};
                break;
            case ModDestination::RESONATOR_FREQ:
                return std::optional{getResonatorTuneKnob().get()};
                break;
            case ModDestination::RESONATOR_Q:
                return std::optional{getResonatorQKnob().get()};
                break;
            case ModDestination::CHAOS_AMOUNT:
                return std::optional{getChaosAmountKnob().get()};
                break;
            case ModDestination::CHAOS_CHARACTER:
                return std::optional{getChaosCharacterKnob().get()};
                break;
            case ModDestination::DETUNE:
                return std::optional{getUnisonDetuneKnob().get()};
                break;
            case ModDestination::LOWPASS_CUTOFF:
                return std::optional{getLowpasCutoffKnob().get()};
                break;
            case ModDestination::LOWPASS_Q:
                return std::optional{getLowpassQKnob().get()};
                break;
            case ModDestination::HIGHPASS_CUTOFF:
                return std::optional{getHighpassCutoffKnob().get()};
                break;
            case ModDestination::STEREO_WIDTH:
                return std::nullopt;
                break;
            case ModDestination::GAIN:
                return std::optional{getMainGainKnob().get()};
                break;
            
            default:
                return std::nullopt;
                break;
            }
            
        }

        std::map<telegraph::ModSource, std::map<telegraph::ModDestination, std::unique_ptr<juce::Slider>>> modulationAmountKnobs = std::map<telegraph::ModSource,std::map<telegraph::ModDestination, std::unique_ptr<juce::Slider>>>();
        Mode currentEditMode=Mode::SYNTH_EDIT; 
        telegraph::ModSource currentModSource;
        GlowEffect modulationGlow = GlowEffect();
        
        
        
    private:
        PresetSaveModalComponent presetSaveDialog;
        struct OverlayPanel : public juce::Component {
            OverlayPanel(){}
            void paint(juce::Graphics& g) override {
                g.fillAll(
                    juce::Colour(0x99222222)
                );
            }
            void resized() override {}
        };
        OverlayPanel overlayPanel;
        struct HeaderPanel: public juce::Component{
            HeaderPanel()
            :pluginTitle(juce::Label())
            ,presetDisplay(std::make_unique<juce::TextButton>())
            ,presetSaveButton(std::make_unique<juce::TextButton>(FAW::SAVE_ICON_UNICODE))
            {
                pluginTitle.setText("Telegraph", juce::dontSendNotification);
                pluginTitle.setFont(TelegraphFontManager::Instance().getDisplayFont());
                presetSaveButton->setLookAndFeel(TelegraphLookAndFeelProvider::Instance().getIconLookAndFeel());

                addAndMakeVisible(pluginTitle);

                presetDisplay->setName("Init Patch");
                presetDisplay->setColour(juce::TextButton::buttonColourId, juce::Colours::darkslateblue);
                presetDisplay->setColour(juce::TextButton::buttonOnColourId, juce::Colours::darkslateblue);
                presetDisplay->setColour(juce::TextButton::textColourOffId, juce::Colours::cyan);
                presetDisplay->setColour(juce::TextButton::textColourOnId, juce::Colours::cyan);
                addAndMakeVisible(*presetDisplay);

                addAndMakeVisible(*presetSaveButton);
            }
            void paint (juce::Graphics& g) override
            {
                g.fillAll (backgroundColour);
            }
            void resized() override {
                auto bounds = getLocalBounds();
                auto titleWidth = bounds.getWidth()/3;
                auto titleBounds = bounds.removeFromLeft(titleWidth);
                pluginTitle.setBounds(titleBounds);

                presetDisplay->setBounds(bounds.removeFromLeft(titleWidth).withSizeKeepingCentre(titleWidth*0.8, bounds.getHeight()*0.75));
                auto buttonBounds = bounds.removeFromLeft(bounds.getWidth()/4);
                presetSaveButton->setBounds(buttonBounds.withSizeKeepingCentre(buttonBounds.getWidth()*0.7, buttonBounds.getHeight()*0.7));
            }
            juce::Label pluginTitle;
            std::unique_ptr<juce::TextButton> presetDisplay;
            std::unique_ptr<juce::TextButton> presetSaveButton;
            juce::Colour backgroundColour=juce::Colours::transparentBlack;
        };
        HeaderPanel headerPanel;
        struct ModulePanel: public juce::Component{
            ModulePanel():
             topRow(),
             bottomRow()
            {
                addAndMakeVisible(topRow);
                addAndMakeVisible(bottomRow);
            }
            void resized() override
            {
                auto bounds = getLocalBounds();

                auto height = bounds.getHeight();

                topRow.setBounds  (bounds.removeFromTop  (2*height/3));
                bottomRow.setBounds  (bounds.removeFromTop  (height/3));

            }
            void paint (juce::Graphics& g) override
            {
                g.fillAll (juce::Colours::transparentBlack);
            }
            struct TopRow: public juce::Component{
                TopRow():
                    exciterPanel(),
                    resonatorPanel(),
                    ampPanel()
                {
                    addAndMakeVisible(exciterPanel);
                    addAndMakeVisible(resonatorPanel);
                    addAndMakeVisible(ampPanel);
                }
                void resized() override
                {
                    auto bounds = getLocalBounds();

                    auto width = bounds.getWidth();

                    exciterPanel.setBounds  (bounds.removeFromLeft  (width/3));
                    resonatorPanel.setBounds  (bounds.removeFromLeft  (width/3));
                    ampPanel.setBounds  (bounds.removeFromLeft  (width/3));

                }
                void paint (juce::Graphics& g) override
                {
                    g.fillAll (juce::Colours::transparentBlack);
                }
                struct ExciterPanel:juce::Component{
                    ExciterPanel()
                    :panelTitle(juce::Label("Exciter"))
                    ,exciterSelector(std::make_unique<juce::ComboBox>())
                    ,exciterTuneKnob(std::move(makeTelegraphKnob()))
                    ,exciterTuneLabel(std::move(makeTelegraphKnobLabel(exciterTuneKnob.get(), ModDestination::EXCITER_FREQ)))
                    ,unisonKnob(std::move(makeTelegraphKnob()))
                    ,unisonLabel(std::move(makeTelegraphKnobLabel(unisonKnob.get(), NonModulatedParameter::UNISON_AMOUNT)))
                    ,detuneKnob(std::move(makeTelegraphKnob()))
                    ,detuneLabel(std::move(makeTelegraphKnobLabel(detuneKnob.get(), ModDestination::DETUNE)))
                    ,vibratoAmountKnob(std::move(makeTelegraphKnob()))
                    ,vibratoAmountLabel(std::move(makeTelegraphKnobLabel(vibratoAmountKnob.get(), ModDestination::VIB_AMOUNT)))
                    ,vibratoSpeedKnob(std::move(makeTelegraphKnob()))
                    ,vibratoSpeedLabel(std::move(makeTelegraphKnobLabel(vibratoSpeedKnob.get(), ModDestination::VIB_SPEED)))
                    ,exciterGainKnob(std::move(makeTelegraphKnob()))
                    ,exciterGainLabel(std::move(makeTelegraphKnobLabel(exciterGainKnob.get(), ModDestination::EXCITER_GAIN)))
                    {
                        panelTitle.setText("EXCITER", juce::dontSendNotification);
                        panelTitle.setFont(TelegraphFontManager::Instance().getSecondaryHeaderFont());
                        panelTitle.setJustificationType(Justification::centred);
                        exciterSelector->addItem("SINE",1);
                        exciterSelector->addItem("SAW",2);
                        exciterSelector->addItem("SQUARE",3);
                        addAndMakeVisible(panelTitle);
                        addAndMakeVisible(*exciterSelector);
                        addAndMakeVisible (*exciterTuneKnob);
                        addAndMakeVisible (*exciterTuneLabel);
                        addAndMakeVisible (*unisonKnob);
                        addAndMakeVisible (*unisonLabel);
                        addAndMakeVisible (*detuneKnob);
                        addAndMakeVisible (*detuneLabel);
                        addAndMakeVisible (*vibratoAmountKnob);
                        addAndMakeVisible (*vibratoAmountLabel);
                        addAndMakeVisible (*vibratoSpeedKnob);
                        addAndMakeVisible (*vibratoSpeedLabel);
                        addAndMakeVisible (*exciterGainKnob);
                        addAndMakeVisible (*exciterGainLabel);

                    }
                    void resized() override {
                        auto bounds = getLocalBounds();
                        const auto titleHeight = bounds.getHeight()/10;
                        panelTitle.setBounds(bounds.removeFromTop(titleHeight));
                        auto dropdownWidth = 2*bounds.getWidth() / 3;
                        auto dropdownHeight = bounds.getWidth() / 4;
                        auto topInset =  titleHeight; //bounds.getHeight()/16 +
                        exciterSelector->setBounds ( 
                            (bounds.getWidth()-dropdownWidth)/2,
                            topInset,
                            dropdownWidth,
                            dropdownHeight
                        );
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,0,*exciterTuneKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,1,*exciterGainKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,3,*unisonKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,4,*detuneKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,6,*vibratoAmountKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,7,*vibratoSpeedKnob);

                    }
                    void paint (juce::Graphics& g) override
                    {
                        auto bounds = getLocalBounds();
                        // g.fillAll (juce::Colours::black);
                        g.fillAll (juce::Colour(0x99000000));
                        g.setColour(juce::Colours::darkgrey);
                        auto trimmedBounds = bounds.withTrimmedLeft(2).withTrimmedRight(2).withTrimmedTop(2).withTrimmedBottom(2);
                        g.fillRect(trimmedBounds);
                    }
                    juce::Label panelTitle;
                    std::unique_ptr<juce::ComboBox> exciterSelector;
                    std::unique_ptr<juce::Slider> exciterTuneKnob;
                    std::unique_ptr<juce::Label> exciterTuneLabel;
                    std::unique_ptr<juce::Slider> unisonKnob;
                    std::unique_ptr<juce::Label> unisonLabel;
                    std::unique_ptr<juce::Slider> detuneKnob;
                    std::unique_ptr<juce::Label> detuneLabel;
                    std::unique_ptr<juce::Slider> vibratoAmountKnob;
                    std::unique_ptr<juce::Label> vibratoAmountLabel;
                    std::unique_ptr<juce::Slider> vibratoSpeedKnob;
                    std::unique_ptr<juce::Label> vibratoSpeedLabel;
                    std::unique_ptr<juce::Slider> exciterGainKnob;
                    std::unique_ptr<juce::Label> exciterGainLabel;

                };
                ExciterPanel exciterPanel;
                struct ResonatorPanel:juce::Component{
                    ResonatorPanel()
                    :panelTitle(juce::Label("Resonator"))
                    ,resonatorSelector(std::make_unique<juce::ComboBox>())
                    ,resonatorTuneKnob(std::move(makeTelegraphKnob()))
                    ,resonatorTuneLabel(std::move(makeTelegraphKnobLabel(resonatorTuneKnob.get(),ModDestination::RESONATOR_FREQ)))
                    ,resonatorQKnob(std::move(makeTelegraphKnob()))
                    ,resonatorQLabel(std::move(makeTelegraphKnobLabel(resonatorQKnob.get(),ModDestination::RESONATOR_Q)))
                    ,chaosAmountKnob(std::move(makeTelegraphKnob()))
                    ,chaosAmountLabel(std::move(makeTelegraphKnobLabel(chaosAmountKnob.get(),ModDestination::CHAOS_AMOUNT)))
                    ,chaosCharacterKnob(std::move(makeTelegraphKnob()))
                    ,chaosCharacterLabel(std::move(makeTelegraphKnobLabel(chaosCharacterKnob.get(),ModDestination::CHAOS_CHARACTER)))
                    {
                        panelTitle.setText("RESONATOR", juce::dontSendNotification);
                        panelTitle.setFont(TelegraphFontManager::Instance().getSecondaryHeaderFont());
                        panelTitle.setJustificationType(Justification::centred);
                        //{"COS", "WRAP", "TANH", "CLIP", "LOWERED_BELL"}
                        resonatorSelector->addItem("COS",1);
                        resonatorSelector->addItem("WRAP",2);
                        resonatorSelector->addItem("TANH",3);
                        // resonatorSelector->addItem("CLIP",4);
                        // resonatorSelector->addItem("LOWERED BELL",5);
                        addAndMakeVisible(panelTitle);
                        addAndMakeVisible(*resonatorSelector);
                        addAndMakeVisible(*resonatorTuneKnob);
                        addAndMakeVisible(*resonatorTuneLabel);
                        addAndMakeVisible(*resonatorQKnob);
                        addAndMakeVisible(*resonatorQLabel);
                        addAndMakeVisible(*chaosAmountKnob);
                        addAndMakeVisible(*chaosAmountLabel);
                        addAndMakeVisible(*chaosCharacterKnob);
                        addAndMakeVisible(*chaosCharacterLabel);
                        
                    }
                    void resized() override {
                        auto bounds = getLocalBounds();
                        const auto titleHeight = bounds.getHeight()/10;
                        panelTitle.setBounds(bounds.removeFromTop(titleHeight));

                        auto dropdownWidth = 2*bounds.getWidth() / 3;
                        auto dropdownHeight = bounds.getWidth() / 4;
                        auto topInset = titleHeight;//bounds.getHeight()/16;
                        resonatorSelector->setBounds ( 
                            (bounds.getWidth()-dropdownWidth)/2,
                            topInset,
                            dropdownWidth,
                            dropdownHeight
                        );
                        
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,0,*resonatorTuneKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,1,*resonatorQKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,3,*chaosAmountKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,4,*chaosCharacterKnob);
                        // positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,6,*highpassCutoffKnob);

                    }
                    void paint (juce::Graphics& g) override
                    {
                        auto bounds = getLocalBounds();
                        // g.fillAll (juce::Colours::black);
                        g.fillAll (juce::Colour(0x99000000));

                        g.setColour(juce::Colours::dimgrey);
                        auto trimmedBounds = bounds.withTrimmedTop(2);
                        trimmedBounds = trimmedBounds.withTrimmedBottom(2);
                        g.fillRect(trimmedBounds);
                    }
                    juce::Label panelTitle;
                    std::unique_ptr<juce::ComboBox> resonatorSelector;
                    std::unique_ptr<juce::Slider> resonatorTuneKnob;
                    std::unique_ptr<juce::Label> resonatorTuneLabel;
                    std::unique_ptr<juce::Slider> resonatorQKnob;
                    std::unique_ptr<juce::Label> resonatorQLabel;
                    std::unique_ptr<juce::Slider> chaosAmountKnob;
                    std::unique_ptr<juce::Label> chaosAmountLabel;
                    std::unique_ptr<juce::Slider> chaosCharacterKnob;
                    std::unique_ptr<juce::Label> chaosCharacterLabel;
                    
                };
                ResonatorPanel resonatorPanel;
                struct AmpPanel:juce::Component{
                    AmpPanel()
                    :panelTitle(juce::Label("Amplifier"))
                    ,lowpassCutoffKnob(std::move(makeTelegraphKnob()))
                    ,lowpassCutoffLabel(std::move(makeTelegraphKnobLabel(lowpassCutoffKnob.get(),ModDestination::LOWPASS_CUTOFF)))
                    ,lowpassQKnob(std::move(makeTelegraphKnob()))
                    ,lowpassQLabel(std::move(makeTelegraphKnobLabel(lowpassQKnob.get(),ModDestination::LOWPASS_Q)))
                    ,highpassCutoffKnob(std::move(makeTelegraphKnob()))
                    ,highpassCutoffLabel(std::move(makeTelegraphKnobLabel(highpassCutoffKnob.get(),ModDestination::HIGHPASS_CUTOFF)))
                    ,ampMapBtn(std::move(makeTelegraphModMapButton()))
                    ,ampMapLabel(std::move(makeKnobLabel(ampMapBtn.get(),"Amp Env:")))
                    ,ampAttackKnob(std::move(makeTelegraphKnob()))
                    ,ampAttackLabel(std::move(makeTelegraphKnobLabel(ampAttackKnob.get(),NonModulatedParameter::AMP_ATTACK)))
                    ,ampDecayKnob(std::move(makeTelegraphKnob()))
                    ,ampDecayLabel(std::move(makeTelegraphKnobLabel(ampDecayKnob.get(),NonModulatedParameter::AMP_DECAY)))
                    ,ampSustainKnob(std::move(makeTelegraphKnob()))
                    ,ampSustainLabel(std::move(makeTelegraphKnobLabel(ampSustainKnob.get(),NonModulatedParameter::AMP_SUSTAIN)))
                    ,ampReleaseKnob(std::move(makeTelegraphKnob()))
                    ,ampReleaseLabel(std::move(makeTelegraphKnobLabel(ampReleaseKnob.get(),NonModulatedParameter::AMP_RELEASE)))
                    ,finalGainKnob(std::move(makeTelegraphKnob()))
                    ,finalGainLabel(std::move(makeTelegraphKnobLabel(finalGainKnob.get(),ModDestination::GAIN)))
                    {
                        panelTitle.setText("AMPLIFIER", juce::dontSendNotification);
                        panelTitle.setFont(TelegraphFontManager::Instance().getSecondaryHeaderFont());
                        panelTitle.setJustificationType(Justification::centred);
                        addAndMakeVisible(panelTitle);
                        addAndMakeVisible(*lowpassCutoffKnob);
                        addAndMakeVisible(*lowpassCutoffLabel);
                        addAndMakeVisible(*lowpassQKnob);
                        addAndMakeVisible(*lowpassQLabel);
                        addAndMakeVisible(*highpassCutoffKnob);
                        addAndMakeVisible(*highpassCutoffLabel);
                        addAndMakeVisible(*ampMapBtn);
                        addAndMakeVisible(*ampAttackKnob);
                        addAndMakeVisible(*ampAttackLabel);
                        addAndMakeVisible(*ampDecayKnob);
                        addAndMakeVisible(*ampDecayLabel);
                        addAndMakeVisible(*ampSustainKnob);
                        addAndMakeVisible(*ampSustainLabel);
                        addAndMakeVisible(*ampReleaseKnob);
                        addAndMakeVisible(*ampReleaseLabel);
                        addAndMakeVisible(*finalGainKnob);
                        addAndMakeVisible(*finalGainLabel);
                    }
                    void resized() override {
                        auto bounds = getLocalBounds();
                        const auto titleHeight = bounds.getHeight()/10;
                        panelTitle.setBounds(bounds.removeFromTop(titleHeight));
                        bounds = bounds.withTrimmedTop(titleHeight);

                        // auto topInset = bounds.getHeight()/16; //titleHeight;
                        positionKnobOnPanelGrid(bounds,3,0,*highpassCutoffKnob);
                        positionKnobOnPanelGrid(bounds,3,1,*lowpassCutoffKnob);
                        positionKnobOnPanelGrid(bounds,3,2,*lowpassQKnob);
                        auto filterKnobHeight = bounds.getWidth()/3;
                        bounds = bounds.withTrimmedTop(filterKnobHeight+titleHeight);
                        positionKnobOnPanelGrid(bounds,5,0,*ampMapBtn);
                        positionKnobOnPanelGrid(bounds,5,1,*ampAttackKnob);
                        positionKnobOnPanelGrid(bounds,5,2,*ampDecayKnob);
                        positionKnobOnPanelGrid(bounds,5,3,*ampSustainKnob);
                        positionKnobOnPanelGrid(bounds,5,4,*ampReleaseKnob);
                        auto envKnobHeight = bounds.getWidth()/5;
                        bounds = bounds.withTrimmedTop(envKnobHeight+titleHeight);
                        positionKnobOnPanelGrid(bounds,2,1,*finalGainKnob);

                    }
                    void paint (juce::Graphics& g) override
                    {
                        auto bounds = getLocalBounds();
                        g.fillAll (juce::Colour(0x99000000));
                        g.setColour(juce::Colours::darkgrey);
                        auto trimmedBounds = bounds.withTrimmedLeft(2).withTrimmedRight(2).withTrimmedTop(2).withTrimmedBottom(2);

                        g.fillRect(trimmedBounds);
                    }
                    juce::Label panelTitle;
                    std::unique_ptr<juce::Slider>     lowpassCutoffKnob;
                    std::unique_ptr<juce::Label>      lowpassCutoffLabel;
                    std::unique_ptr<juce::Slider>     lowpassQKnob;
                    std::unique_ptr<juce::Label>      lowpassQLabel;
                    std::unique_ptr<juce::Slider> highpassCutoffKnob;
                    std::unique_ptr<juce::Label> highpassCutoffLabel;
                    std::unique_ptr<juce::TextButton> ampMapBtn;
                    std::unique_ptr<juce::Label>      ampMapLabel;
                    std::unique_ptr<juce::Slider>     ampAttackKnob;
                    std::unique_ptr<juce::Label>      ampAttackLabel;
                    std::unique_ptr<juce::Slider>     ampDecayKnob;
                    std::unique_ptr<juce::Label>      ampDecayLabel;
                    std::unique_ptr<juce::Slider>     ampSustainKnob;
                    std::unique_ptr<juce::Label>      ampSustainLabel;
                    std::unique_ptr<juce::Slider>     ampReleaseKnob;
                    std::unique_ptr<juce::Label>      ampReleaseLabel;
                    std::unique_ptr<juce::Slider>     finalGainKnob;
                    std::unique_ptr<juce::Label>      finalGainLabel;
                };
                
                AmpPanel ampPanel;
                
            };
            TopRow topRow;
            struct BottomRow: public juce::Component{
                BottomRow()
                :panelTitle(juce::Label())
                ,modEnv1MapBtn(std::move(makeTelegraphModMapButton()))
                ,modEnv1MapLabel(std::move(makeKnobLabel(modEnv1MapBtn.get(),"Env 1:")))
                ,modEnv1AttackKnob(std::move(makeTelegraphKnob()))
                ,modEnv1AttackLabel(std::move(makeTelegraphKnobLabel(modEnv1AttackKnob.get(), NonModulatedParameter::ENV_ONE_ATTACK)))
                ,modEnv1DecayKnob(std::move(makeTelegraphKnob()))
                ,modEnv1DecayLabel(std::move(makeTelegraphKnobLabel(modEnv1DecayKnob.get(), NonModulatedParameter::ENV_ONE_DECAY)))
                ,modEnv1SustainKnob(std::move(makeTelegraphKnob()))
                ,modEnv1SustainLabel(std::move(makeTelegraphKnobLabel(modEnv1SustainKnob.get(), NonModulatedParameter::ENV_ONE_SUSTAIN)))
                ,modEnv1ReleaseKnob(std::move(makeTelegraphKnob()))
                ,modEnv1ReleaseLabel(std::move(makeTelegraphKnobLabel(modEnv1ReleaseKnob.get(), NonModulatedParameter::ENV_ONE_RELEASE)))
                ,modEnv2MapBtn(std::move(makeTelegraphModMapButton()))
                ,modEnv2MapLabel(std::move(makeKnobLabel(modEnv2MapBtn.get(),"Env 2:")))
                ,modEnv2AttackKnob(std::move(makeTelegraphKnob()))
                ,modEnv2AttackLabel(std::move(makeTelegraphKnobLabel(modEnv2AttackKnob.get(), NonModulatedParameter::ENV_TWO_ATTACK)))
                ,modEnv2DecayKnob(std::move(makeTelegraphKnob()))
                ,modEnv2DecayLabel(std::move(makeTelegraphKnobLabel(modEnv2DecayKnob.get(), NonModulatedParameter::ENV_TWO_DECAY)))
                ,modEnv2SustainKnob(std::move(makeTelegraphKnob()))
                ,modEnv2SustainLabel(std::move(makeTelegraphKnobLabel(modEnv2SustainKnob.get(), NonModulatedParameter::ENV_TWO_SUSTAIN)))
                ,modEnv2ReleaseKnob(std::move(makeTelegraphKnob()))
                ,modEnv2ReleaseLabel(std::move(makeTelegraphKnobLabel(modEnv2ReleaseKnob.get(), NonModulatedParameter::ENV_TWO_RELEASE)))
                ,modLFO1MapBtn(std::move(makeTelegraphModMapButton()))
                ,modLFO1MapLabel(std::move(makeKnobLabel(modLFO1MapBtn.get(),"LFO 1:")))
                ,modLFO1SpeedKnob(std::move(makeTelegraphKnob()))
                ,modLFO1SpeedLabel(std::move(makeTelegraphKnobLabel(modLFO1SpeedKnob.get(), ModDestination::LFO_ONE_SPEED)))
                ,modLFO2MapBtn(std::move(makeTelegraphModMapButton()))
                ,modLFO2MapLabel(std::move(makeKnobLabel(modLFO2MapBtn.get(),"LFO 2:")))
                ,modLFO2SpeedKnob(std::move(makeTelegraphKnob()))
                ,modLFO2SpeedLabel(std::move(makeTelegraphKnobLabel(modLFO2SpeedKnob.get(), ModDestination::LFO_TWO_SPEED)))
                {
                    panelTitle.setText("MODULATORS", juce::dontSendNotification);
                    panelTitle.setFont(TelegraphFontManager::Instance().getSecondaryHeaderFont());
                    addAndMakeVisible(panelTitle);
                    addAndMakeVisible(*modEnv1MapBtn);
                    addAndMakeVisible(*modEnv1MapLabel);
                    addAndMakeVisible(*modEnv1AttackKnob);
                    addAndMakeVisible(*modEnv1AttackLabel);
                    addAndMakeVisible(*modEnv1DecayKnob);
                    addAndMakeVisible(*modEnv1DecayLabel);
                    addAndMakeVisible(*modEnv1SustainKnob);
                    addAndMakeVisible(*modEnv1SustainLabel);
                    addAndMakeVisible(*modEnv1ReleaseKnob);
                    addAndMakeVisible(*modEnv1ReleaseLabel);
                    addAndMakeVisible(*modEnv2MapBtn);
                    addAndMakeVisible(*modEnv2MapLabel);
                    addAndMakeVisible(*modEnv2AttackKnob);
                    addAndMakeVisible(*modEnv2AttackLabel);
                    addAndMakeVisible(*modEnv2DecayKnob);
                    addAndMakeVisible(*modEnv2DecayLabel);
                    addAndMakeVisible(*modEnv2SustainKnob);
                    addAndMakeVisible(*modEnv2SustainLabel);
                    addAndMakeVisible(*modEnv2ReleaseKnob);
                    addAndMakeVisible(*modEnv2ReleaseLabel);
                    addAndMakeVisible(*modLFO1MapBtn);
                    addAndMakeVisible(*modLFO1MapLabel);
                    addAndMakeVisible(*modLFO1SpeedKnob);
                    addAndMakeVisible(*modLFO1SpeedLabel);
                    addAndMakeVisible(*modLFO2MapBtn);
                    addAndMakeVisible(*modLFO2MapLabel);
                    addAndMakeVisible(*modLFO2SpeedKnob);
                    addAndMakeVisible(*modLFO2SpeedLabel);
                }
                void resized() override {
                        auto bounds = getLocalBounds();
                        const auto titleHeight = bounds.getHeight()/10;
                        panelTitle.setBounds(bounds.removeFromTop(titleHeight));
                        bounds = bounds.withTrimmedTop(titleHeight);
                        auto topInset = titleHeight;//bounds.getHeight()/8;
                        auto rowOneBounds = bounds;//bounds.withTop(topInset);//.withRight(bounds.getRight()-bounds.getWidth()/3);
                        positionKnobOnPanelGrid(rowOneBounds,10,0,*modEnv1MapBtn);
                        positionKnobOnPanelGrid(rowOneBounds,10,1,*modEnv1AttackKnob);
                        positionKnobOnPanelGrid(rowOneBounds,10,2,*modEnv1DecayKnob);
                        positionKnobOnPanelGrid(rowOneBounds,10,3,*modEnv1SustainKnob);
                        positionKnobOnPanelGrid(rowOneBounds,10,4,*modEnv1ReleaseKnob);
                        positionKnobOnPanelGrid(rowOneBounds,10,5,*modEnv2MapBtn);
                        positionKnobOnPanelGrid(rowOneBounds,10,6,*modEnv2AttackKnob);
                        positionKnobOnPanelGrid(rowOneBounds,10,7,*modEnv2DecayKnob);
                        positionKnobOnPanelGrid(rowOneBounds,10,8,*modEnv2SustainKnob);
                        positionKnobOnPanelGrid(rowOneBounds,10,9,*modEnv2ReleaseKnob);

                        auto rowTwoBounds = rowOneBounds.withTop(rowOneBounds.getY()+topInset);
                        positionKnobOnPanelGrid(rowTwoBounds,10,10,*modLFO1MapBtn);
                        positionKnobOnPanelGrid(rowTwoBounds,10,11,*modLFO1SpeedKnob);
                        positionKnobOnPanelGrid(rowTwoBounds,10,15,*modLFO2MapBtn);
                        positionKnobOnPanelGrid(rowTwoBounds,10,16,*modLFO2SpeedKnob);
                    }
                void paint (juce::Graphics& g) override
                {
                    auto bounds = getLocalBounds();
                    g.fillAll (juce::Colour(0x99000000));
                    g.setColour(juce::Colour(0xff444444 ));
                    auto trimmedBounds = bounds.withTrimmedLeft(2);
                    trimmedBounds = trimmedBounds.withTrimmedRight(2);
                    trimmedBounds = trimmedBounds.withTrimmedBottom(2);
                    g.fillRect(trimmedBounds);

                }
                juce::Label panelTitle;
                std::unique_ptr<juce::TextButton> modEnv1MapBtn;
                std::unique_ptr<juce::Label>      modEnv1MapLabel;
                std::unique_ptr<juce::Slider>     modEnv1AttackKnob;
                std::unique_ptr<juce::Label>      modEnv1AttackLabel;
                std::unique_ptr<juce::Slider>     modEnv1DecayKnob;
                std::unique_ptr<juce::Label>      modEnv1DecayLabel;
                std::unique_ptr<juce::Slider>     modEnv1SustainKnob;
                std::unique_ptr<juce::Label>      modEnv1SustainLabel;
                std::unique_ptr<juce::Slider>     modEnv1ReleaseKnob;
                std::unique_ptr<juce::Label>      modEnv1ReleaseLabel;
                std::unique_ptr<juce::TextButton> modEnv2MapBtn;
                std::unique_ptr<juce::Label>      modEnv2MapLabel;
                std::unique_ptr<juce::Slider>     modEnv2AttackKnob;
                std::unique_ptr<juce::Label>      modEnv2AttackLabel;
                std::unique_ptr<juce::Slider>     modEnv2DecayKnob;
                std::unique_ptr<juce::Label>      modEnv2DecayLabel;
                std::unique_ptr<juce::Slider>     modEnv2SustainKnob;
                std::unique_ptr<juce::Label>      modEnv2SustainLabel;
                std::unique_ptr<juce::Slider>     modEnv2ReleaseKnob;
                std::unique_ptr<juce::Label>      modEnv2ReleaseLabel;
                std::unique_ptr<juce::TextButton> modLFO1MapBtn;
                std::unique_ptr<juce::Label>      modLFO1MapLabel;
                std::unique_ptr<juce::Slider>     modLFO1SpeedKnob;
                std::unique_ptr<juce::Label>      modLFO1SpeedLabel;
                std::unique_ptr<juce::TextButton> modLFO2MapBtn;
                std::unique_ptr<juce::Label>      modLFO2MapLabel;
                std::unique_ptr<juce::Slider>     modLFO2SpeedKnob;
                std::unique_ptr<juce::Label>      modLFO2SpeedLabel;

            };
            BottomRow bottomRow;
        };
        ModulePanel modulePanel;
};