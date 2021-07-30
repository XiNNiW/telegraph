#pragma once
#include <JuceHeader.h>
#include "TelegraphFontManager.h"


struct IconLookAndFeel : public juce::LookAndFeel_V4 {
    
    IconLookAndFeel(): juce::LookAndFeel_V4(){
        // setColour (juce::TextButton::textColourOffId, juce::Colours::red);
        // setColour (juce::TextButton::textColourOnId, juce::Colours::red);
        setDefaultSansSerifTypeface(TelegraphFontManager::Instance().getIconTypeface());
        
        // set(TelegraphFontManager::Instance().getIconTypeface());
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
        // auto font = getTextButtonFont (button, button.getHeight());
        auto font = TelegraphFontManager::Instance().getIconFont();
        g.setFont (font);
        g.setColour (button.findColour (button.getToggleState() ? juce::TextButton::textColourOnId
                                                                : juce::TextButton::textColourOffId)
                        .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
    
        auto yIndent = juce::jmin (4, button.proportionOfHeight (0.3f));
        auto cornerSize = juce::jmin (button.getHeight(), button.getWidth()) / 2;
    
        auto fontHeight = juce::roundToInt (font.getHeight() * 0.6f);
        auto leftIndent  = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft()  ? 4 : 2));
        auto rightIndent = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        auto textWidth = button.getWidth() - leftIndent - rightIndent;
    
        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                            leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                            juce::Justification::centred, 2);
    }

    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        const int width = button.getWidth();
        const int height = button.getHeight();

        const float indent = 2.0f;
        const int cornerSize = jmin (roundToInt ((float) width * 0.4f),
                                    roundToInt ((float) height * 0.4f));

        Path p;
        p.addRoundedRectangle (indent, indent,
                            (float) width - indent * 2.0f,
                            (float) height - indent * 2.0f,
                            (float) cornerSize);

        Colour bc (backgroundColour.withMultipliedSaturation (0.9f));

        if (shouldDrawButtonAsHighlighted)
        {
            if (shouldDrawButtonAsDown)
                bc = bc.brighter();
            else if (bc.getBrightness() > 0.5f)
                bc = bc.darker (0.1f);
            else
                bc = bc.brighter (0.1f);
        }

        g.setColour (bc);
        g.fillPath (p);

        g.setColour (bc.contrasting().withAlpha ((shouldDrawButtonAsHighlighted) ? 0.6f : 0.4f));
        g.strokePath (p, PathStrokeType ((shouldDrawButtonAsHighlighted) ? 2.0f : 1.4f));

    }

};

class TelegraphLookAndFeelProvider {
    public:
        static TelegraphLookAndFeelProvider& Instance(){
            static TelegraphLookAndFeelProvider instance;
            return instance;
        }

        juce::LookAndFeel_V4* getIconLookAndFeel(){
            return &iconLookAndFeel;
        }
  
        TelegraphLookAndFeelProvider(TelegraphLookAndFeelProvider const&)=delete;              
        void operator=(TelegraphLookAndFeelProvider const&)=delete; 

    private:
        
        TelegraphLookAndFeelProvider()
        {
        }

        IconLookAndFeel iconLookAndFeel;
};