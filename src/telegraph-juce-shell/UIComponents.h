#pragma once
#include <JuceHeader.h>
#include "telegraph_core.h"
#include <utility>
#include <tuple>

using telegraph::ModDestination;
using telegraph::NonModulatedParameter;

static std::unique_ptr<juce::Slider> makeTelegraphKnob(){
    auto knob=std::make_unique<juce::Slider>();
    knob->setSliderStyle (juce::Slider::SliderStyle::Rotary);
    knob->setTextBoxStyle (juce::Slider::TextBoxAbove, true, 20, 0);
    
    return knob;
}

template<typename T>
static std::unique_ptr<juce::Label> makeTelegraphKnobLabel(juce::Component* knob, T paramId){
    auto label = std::make_unique<juce::Label>();
    label->setText(
        telegraph::DisplayName<T>(paramId),
        juce::dontSendNotification
    );
    label->attachToComponent(knob, false);
    return label;
}


static std::unique_ptr<juce::Label> makeKnobLabel(juce::Component* knob, std::string displayName){
    auto label = std::make_unique<juce::Label>();
    label->setText(
        displayName,
        juce::dontSendNotification
    );
    label->attachToComponent(knob, false);
    return label;
}

static void positionKnobOnPanelGrid(
    juce::Rectangle<int> panelBounds, 
    const size_t& knobsPerRow, 
    const size_t& knobPos, 
    juce::Component& knob
){
    auto knobSize = panelBounds.getWidth()/knobsPerRow;
    auto xLoc = (knobSize * (knobPos % knobsPerRow));
    auto yLoc = (knobSize * (knobPos / knobsPerRow)) + panelBounds.getY();
    auto width = knobSize;
    auto height = knobSize;

    knob.setBounds (
        xLoc,yLoc,width*0.9,height*0.9
    );
                    
}

class TelegraphUIContentComponent : public juce::Component {
    public:
        enum
        {
            paramControlHeight = 80,
            paramLabelWidth    = 80,
            paramSliderWidth   = 300
        };
        TelegraphUIContentComponent()
        : 
          headerPanel(juce::Colours::lightgrey),
          modulePanel()
        {
            addAndMakeVisible(headerPanel);
            addAndMakeVisible(modulePanel);
        }
        void paint (juce::Graphics& g) override
        {
            g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
        }
        void resized() override
        {
            auto bounds = getLocalBounds();

            auto height = bounds.getHeight();

            headerPanel.setBounds  (bounds.removeFromTop  (height/16));
            modulePanel.setBounds  (bounds.removeFromTop  (15*height/16));

        }
        // EXCITER PANEL
        std::unique_ptr<juce::ComboBox>& getExciterTypeSelector(){
            return modulePanel.topRow.exciterPanel.exciterSelector;
        }
        std::unique_ptr<juce::Slider>& getExciterTuneKnob(){
            return modulePanel.topRow.exciterPanel.exciterTuneKnob;
        }
        std::unique_ptr<juce::Slider>& getUnisonAmountKnob(){
            return modulePanel.topRow.exciterPanel.unisonKnob;
        }
        std::unique_ptr<juce::Slider>& getUnisonDetuneKnob(){
            return modulePanel.topRow.exciterPanel.detuneKnob;
        }
        std::unique_ptr<juce::Slider>& getVibratoDepthKnob(){
            return modulePanel.topRow.exciterPanel.vibratoAmountKnob;
        }
        std::unique_ptr<juce::Slider>& getVibratoSpeedKnob(){
            return modulePanel.topRow.exciterPanel.vibratoSpeedKnob;
        }
        std::unique_ptr<juce::Slider>& getExciterGainKnob(){
            return modulePanel.topRow.exciterPanel.exciterGainKnob;
        }
        // RESONATOR PANEL
        std::unique_ptr<juce::ComboBox>& getResontorTypeSelector(){
            return modulePanel.topRow.resonatorPanel.resonatorSelector;
        }
        std::unique_ptr<juce::Slider>& getResonatorTuneKnob(){
            return modulePanel.topRow.resonatorPanel.resonatorTuneKnob;
        }
        std::unique_ptr<juce::Slider>& getResonatorQKnob(){
            return modulePanel.topRow.resonatorPanel.resonatorQKnob;
        }
        std::unique_ptr<juce::Slider>& getChaosAmountKnob(){
            return modulePanel.topRow.resonatorPanel.chaosAmountKnob;
        }
        std::unique_ptr<juce::Slider>& getChaosCharacter(){
            return modulePanel.topRow.resonatorPanel.chaosCharacterKnob;
        }
        std::unique_ptr<juce::Slider>& getHighpassCutoffKnob(){
            return modulePanel.topRow.resonatorPanel.highpassCutoffKnob;
        }
        //AMP PANEL
        std::unique_ptr<juce::Slider>& getLowpasCutoffKnob(){return modulePanel.topRow.ampPanel.lowpassCutoffKnob;}
        std::unique_ptr<juce::Slider>& getLowpassQKnob(){return modulePanel.topRow.ampPanel.lowpassQKnob;}
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
        
    private:
        struct HeaderPanel: public juce::Component{
            HeaderPanel(juce::Colour c):backgroundColour(c){}
            void paint (juce::Graphics& g) override
            {
                g.fillAll (backgroundColour);
            }
            juce::Colour backgroundColour;
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
                    :exciterSelector(std::make_unique<juce::ComboBox>())
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

                        exciterSelector->addItem("SINE",1);
                        exciterSelector->addItem("SAW",2);
                        exciterSelector->addItem("SQUARE",3);
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
                        auto dropdownWidth = 2*bounds.getWidth() / 3;
                        auto dropdownHeight = bounds.getWidth() / 4;
                        auto topInset = bounds.getHeight()/16;
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
                        g.fillAll (juce::Colours::blue);
                    }
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
                    :resonatorSelector(std::make_unique<juce::ComboBox>())
                    ,resonatorTuneKnob(std::move(makeTelegraphKnob()))
                    ,resonatorTuneLabel(std::move(makeTelegraphKnobLabel(resonatorTuneKnob.get(),ModDestination::RESONATOR_FREQ)))
                    ,resonatorQKnob(std::move(makeTelegraphKnob()))
                    ,resonatorQLabel(std::move(makeTelegraphKnobLabel(resonatorQKnob.get(),ModDestination::RESONATOR_Q)))
                    ,chaosAmountKnob(std::move(makeTelegraphKnob()))
                    ,chaosAmountLabel(std::move(makeTelegraphKnobLabel(chaosAmountKnob.get(),ModDestination::CHAOS_AMOUNT)))
                    ,chaosCharacterKnob(std::move(makeTelegraphKnob()))
                    ,chaosCharacterLabel(std::move(makeTelegraphKnobLabel(chaosCharacterKnob.get(),ModDestination::CHAOS_CHARACTER)))
                    ,highpassCutoffKnob(std::move(makeTelegraphKnob()))
                    ,highpassCutoffLabel(std::move(makeTelegraphKnobLabel(highpassCutoffKnob.get(),ModDestination::HIGHPASS_CUTOFF)))
                    {
                        resonatorSelector->addItem("COS",1);
                        resonatorSelector->addItem("TANH",2);
                        resonatorSelector->addItem("WRAP",3);
                        resonatorSelector->addItem("LOWERED BELL",4);
                        addAndMakeVisible(*resonatorSelector);
                        addAndMakeVisible(*resonatorTuneKnob);
                        addAndMakeVisible(*resonatorTuneLabel);
                        addAndMakeVisible(*resonatorQKnob);
                        addAndMakeVisible(*resonatorQLabel);
                        addAndMakeVisible(*chaosAmountKnob);
                        addAndMakeVisible(*chaosAmountLabel);
                        addAndMakeVisible(*chaosCharacterKnob);
                        addAndMakeVisible(*chaosCharacterLabel);
                        addAndMakeVisible(*highpassCutoffKnob);
                        addAndMakeVisible(*highpassCutoffLabel);
                    }
                    void resized() override {
                        auto bounds = getLocalBounds();
                        auto dropdownWidth = 2*bounds.getWidth() / 3;
                        auto dropdownHeight = bounds.getWidth() / 4;
                        auto topInset = bounds.getHeight()/16;
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
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,6,*highpassCutoffKnob);

                    }
                    void paint (juce::Graphics& g) override
                    {
                        g.fillAll (juce::Colours::dimgrey);
                    }
                    std::unique_ptr<juce::ComboBox> resonatorSelector;
                    std::unique_ptr<juce::Slider> resonatorTuneKnob;
                    std::unique_ptr<juce::Label> resonatorTuneLabel;
                    std::unique_ptr<juce::Slider> resonatorQKnob;
                    std::unique_ptr<juce::Label> resonatorQLabel;
                    std::unique_ptr<juce::Slider> chaosAmountKnob;
                    std::unique_ptr<juce::Label> chaosAmountLabel;
                    std::unique_ptr<juce::Slider> chaosCharacterKnob;
                    std::unique_ptr<juce::Label> chaosCharacterLabel;
                    std::unique_ptr<juce::Slider> highpassCutoffKnob;
                    std::unique_ptr<juce::Label> highpassCutoffLabel;
                };
                ResonatorPanel resonatorPanel;
                struct AmpPanel:juce::Component{
                    AmpPanel()
                    :lowpassCutoffKnob(std::move(makeTelegraphKnob()))
                    ,lowpassCutoffLabel(std::move(makeTelegraphKnobLabel(lowpassCutoffKnob.get(),ModDestination::LOWPASS_CUTOFF)))
                    ,lowpassQKnob(std::move(makeTelegraphKnob()))
                    ,lowpassQLabel(std::move(makeTelegraphKnobLabel(lowpassQKnob.get(),ModDestination::LOWPASS_Q)))
                    ,ampMapBtn(std::make_unique<juce::TextButton>("map"))
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
                        addAndMakeVisible(*lowpassCutoffKnob);
                        addAndMakeVisible(*lowpassCutoffLabel);
                        addAndMakeVisible(*lowpassQKnob);
                        addAndMakeVisible(*lowpassQLabel);
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
                        auto topInset = bounds.getHeight()/16;
                        positionKnobOnPanelGrid(bounds.withTop(topInset*2),3,0,*lowpassCutoffKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset*2),3,1,*lowpassQKnob);
                        auto filterKnobHeight = bounds.getWidth()/3;
                        positionKnobOnPanelGrid(bounds.withTop(topInset*3+filterKnobHeight),5,0,*ampMapBtn);
                        positionKnobOnPanelGrid(bounds.withTop(topInset*3+filterKnobHeight),5,1,*ampAttackKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset*3+filterKnobHeight),5,2,*ampDecayKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset*3+filterKnobHeight),5,3,*ampSustainKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset*3+filterKnobHeight),5,4,*ampReleaseKnob);
                        auto envKnobHeight = bounds.getWidth()/5;
                        positionKnobOnPanelGrid(bounds.withTop(topInset*5+filterKnobHeight+envKnobHeight*2),2,1,*finalGainKnob);

                    }
                    void paint (juce::Graphics& g) override
                    {
                        g.fillAll (juce::Colours::darkgrey);
                    }
                    std::unique_ptr<juce::Slider>     lowpassCutoffKnob;
                    std::unique_ptr<juce::Label>      lowpassCutoffLabel;
                    std::unique_ptr<juce::Slider>     lowpassQKnob;
                    std::unique_ptr<juce::Label>      lowpassQLabel;
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
                :modEnv1MapBtn(std::make_unique<juce::TextButton>("map"))
                ,modEnv1MapLabel(std::move(makeKnobLabel(modEnv1MapBtn.get(),"Env 1:")))
                ,modEnv1AttackKnob(std::move(makeTelegraphKnob()))
                ,modEnv1AttackLabel(std::move(makeTelegraphKnobLabel(modEnv1AttackKnob.get(), NonModulatedParameter::ENV_ONE_ATTACK)))
                ,modEnv1DecayKnob(std::move(makeTelegraphKnob()))
                ,modEnv1DecayLabel(std::move(makeTelegraphKnobLabel(modEnv1DecayKnob.get(), NonModulatedParameter::ENV_ONE_DECAY)))
                ,modEnv1SustainKnob(std::move(makeTelegraphKnob()))
                ,modEnv1SustainLabel(std::move(makeTelegraphKnobLabel(modEnv1SustainKnob.get(), NonModulatedParameter::ENV_ONE_SUSTAIN)))
                ,modEnv1ReleaseKnob(std::move(makeTelegraphKnob()))
                ,modEnv1ReleaseLabel(std::move(makeTelegraphKnobLabel(modEnv1ReleaseKnob.get(), NonModulatedParameter::ENV_ONE_RELEASE)))
                ,modEnv2MapBtn(std::make_unique<juce::TextButton>("map"))
                ,modEnv2MapLabel(std::move(makeKnobLabel(modEnv2MapBtn.get(),"Env 2:")))
                ,modEnv2AttackKnob(std::move(makeTelegraphKnob()))
                ,modEnv2AttackLabel(std::move(makeTelegraphKnobLabel(modEnv2AttackKnob.get(), NonModulatedParameter::ENV_TWO_ATTACK)))
                ,modEnv2DecayKnob(std::move(makeTelegraphKnob()))
                ,modEnv2DecayLabel(std::move(makeTelegraphKnobLabel(modEnv2DecayKnob.get(), NonModulatedParameter::ENV_TWO_DECAY)))
                ,modEnv2SustainKnob(std::move(makeTelegraphKnob()))
                ,modEnv2SustainLabel(std::move(makeTelegraphKnobLabel(modEnv2SustainKnob.get(), NonModulatedParameter::ENV_TWO_SUSTAIN)))
                ,modEnv2ReleaseKnob(std::move(makeTelegraphKnob()))
                ,modEnv2ReleaseLabel(std::move(makeTelegraphKnobLabel(modEnv2ReleaseKnob.get(), NonModulatedParameter::ENV_TWO_RELEASE)))
                ,modLFO1MapBtn(std::make_unique<juce::TextButton>("map"))
                ,modLFO1MapLabel(std::move(makeKnobLabel(modLFO1MapBtn.get(),"LFO 1:")))
                ,modLFO1SpeedKnob(std::move(makeTelegraphKnob()))
                ,modLFO1SpeedLabel(std::move(makeTelegraphKnobLabel(modLFO1SpeedKnob.get(), ModDestination::LFO_ONE_SPEED)))
                ,modLFO2MapBtn(std::make_unique<juce::TextButton>("map"))
                ,modLFO2MapLabel(std::move(makeKnobLabel(modLFO2MapBtn.get(),"LFO 2:")))
                ,modLFO2SpeedKnob(std::move(makeTelegraphKnob()))
                ,modLFO2SpeedLabel(std::move(makeTelegraphKnobLabel(modLFO2SpeedKnob.get(), ModDestination::LFO_TWO_SPEED)))
                {
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
                        auto topInset = bounds.getHeight()/8;
                        auto rowOneBounds = bounds.withTop(topInset);//.withRight(bounds.getRight()-bounds.getWidth()/3);
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
                    g.fillAll (juce::Colours::transparentBlack);
                }
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