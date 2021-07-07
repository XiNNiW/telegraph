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
static std::unique_ptr<juce::Label> makeTelegraphKnobLabel(std::unique_ptr<juce::Slider>& knob, T paramId){
    auto label = std::make_unique<juce::Label>();
    label->setText(
        telegraph::DisplayName<T>(paramId),
        juce::dontSendNotification
    );
    label->attachToComponent(knob.get(), false);
    return label;
}

static void positionKnobOnPanelGrid(
    juce::Rectangle<int> panelBounds, 
    const size_t& knobsPerRow, 
    const size_t& knobPos, 
    std::unique_ptr<juce::Slider>& knob
){
    auto knobSize = panelBounds.getWidth()/knobsPerRow;
    auto xLoc = (knobSize * (knobPos % knobsPerRow));
    auto yLoc = (knobSize * (knobPos / knobsPerRow)) + panelBounds.getY();
    auto width = knobSize;
    auto height = knobSize;
    // knob->setBounds (
    //     xLoc,
    //     yLoc,
    //     width,
    //     height
    // );
    // auto bounds = panelBounds.removeFromTop(yLoc).removeFromRight(xLoc);
    // bounds.setSize(width,height);
    knob->setBounds (
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
        //   exciterPanel(juce::Colours::lightgrey),
        //   resonatorPanel(juce::Colours::lightgrey),
        //   ampPanel(juce::Colours::lightgrey),
        //   modulatorPanel(juce::Colours::lightgrey),
        //   outputPanel(juce::Colours::lightgrey)
        {
            addAndMakeVisible(headerPanel);
            addAndMakeVisible(modulePanel);
        }
        // juce::Component getComponent(telegraph::ModDestination id){}
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

            // mainPanel.setBounds (bounds);
        }
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
                    ,exciterTuneLabel(std::move(makeTelegraphKnobLabel(exciterTuneKnob, ModDestination::EXCITER_FREQ)))
                    ,unisonKnob(std::move(makeTelegraphKnob()))
                    ,unisonLabel(std::move(makeTelegraphKnobLabel(unisonKnob, NonModulatedParameter::UNISON_AMOUNT)))
                    ,detuneKnob(std::move(makeTelegraphKnob()))
                    ,detuneLabel(std::move(makeTelegraphKnobLabel(detuneKnob, ModDestination::DETUNE)))
                    ,vibratoAmountKnob(std::move(makeTelegraphKnob()))
                    ,vibratoAmountLabel(std::move(makeTelegraphKnobLabel(vibratoAmountKnob, ModDestination::VIB_AMOUNT)))
                    ,vibratoSpeedKnob(std::move(makeTelegraphKnob()))
                    ,vibratoSpeedLabel(std::move(makeTelegraphKnobLabel(vibratoSpeedKnob, ModDestination::VIB_SPEED)))
                    ,exciterGainKnob(std::move(makeTelegraphKnob()))
                    ,exciterGainLabel(std::move(makeTelegraphKnobLabel(exciterGainKnob, ModDestination::EXCITER_GAIN)))
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
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,0,exciterTuneKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,1,exciterGainKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,3,unisonKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,4,detuneKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,6,vibratoAmountKnob);
                        positionKnobOnPanelGrid(bounds.withTop(topInset+dropdownHeight+topInset),3,7,vibratoSpeedKnob);

                        
                       
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
                    void paint (juce::Graphics& g) override
                    {
                        g.fillAll (juce::Colours::dimgrey);
                    }
                };
                ResonatorPanel resonatorPanel;
                struct AmpPanel:juce::Component{
                    void paint (juce::Graphics& g) override
                    {
                        g.fillAll (juce::Colours::peachpuff);
                    }
                };
                AmpPanel ampPanel;
                
            };
            TopRow topRow;
            struct BottomRow: public juce::Component{
                BottomRow(){}
                void paint (juce::Graphics& g) override
                {
                    g.fillAll (juce::Colours::transparentBlack);
                }
            };
            BottomRow bottomRow;
        };
        ModulePanel modulePanel;
};