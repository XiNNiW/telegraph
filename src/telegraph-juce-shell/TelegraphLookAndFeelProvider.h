#pragma once
#include <JuceHeader.h>
#include "TelegraphFontManager.h"


struct IconLookAndFeel : public juce::LookAndFeel_V4 {
    
    IconLookAndFeel(): juce::LookAndFeel_V4(){
        setDefaultSansSerifTypeface(TelegraphFontManager::Instance().getIconTypeface());
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& button, bool isMouseOverButton, bool isButtonDown) override
    {
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

struct ModulationKnobLookAndFeel : juce::LookAndFeel_V4 {

    ModulationKnobLookAndFeel(){}
    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                                       const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
    {
        
        auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
        auto fill    = slider.findColour (Slider::rotarySliderFillColourId);

        auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (10);

        auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto midpointAngle =  rotaryStartAngle + 0.5 * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = jmin (8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;

        Path backgroundArc;
        backgroundArc.addCentredArc (bounds.getCentreX(),
                                    bounds.getCentreY(),
                                    arcRadius,
                                    arcRadius,
                                    0.0f,
                                    rotaryStartAngle,
                                    rotaryEndAngle,
                                    true);

        g.setColour (outline);
        g.strokePath (backgroundArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));

        if (slider.isEnabled())
        {
            Path valueArc;
            valueArc.addCentredArc (bounds.getCentreX(),
                                    bounds.getCentreY(),
                                    arcRadius,
                                    arcRadius,
                                    0.0f,
                                    midpointAngle,
                                    toAngle,
                                    true);

            g.setColour (fill);
            g.strokePath (valueArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
        }

        auto thumbWidth = lineW * 2.0f;
        Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - MathConstants<float>::halfPi),
                                bounds.getCentreY() + arcRadius * std::sin (toAngle - MathConstants<float>::halfPi));

        g.setColour (slider.findColour (Slider::thumbColourId));
        g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
    }
};


struct TelegraphKnobLookAndFeel : juce::LookAndFeel_V4 {
    Slider::SliderLayout getSliderLayout (Slider& slider)
    {
        // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints

        int minXSpace = 0;
        int minYSpace = 0;

        auto textBoxPos = slider.getTextBoxPosition();

        if (textBoxPos == Slider::TextBoxLeft || textBoxPos == Slider::TextBoxRight)
            minXSpace = 30;
        else
            minYSpace = 15;

        auto localBounds = slider.getLocalBounds();

        auto textBoxWidth  = jmax (0, jmin (slider.getTextBoxWidth(),  localBounds.getWidth() - minXSpace));
        auto textBoxHeight = jmax (0, jmin (slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));

        Slider::SliderLayout layout;

        // 2. set the textBox bounds

        if (textBoxPos != Slider::NoTextBox)
        {
            if (slider.isBar())
            {
                layout.textBoxBounds = localBounds;
            }
            else
            {
                layout.textBoxBounds.setWidth (textBoxWidth);
                layout.textBoxBounds.setHeight (textBoxHeight);

                if (textBoxPos == Slider::TextBoxLeft)           layout.textBoxBounds.setX (0);
                else if (textBoxPos == Slider::TextBoxRight)     layout.textBoxBounds.setX (localBounds.getWidth() - textBoxWidth);
                else /* above or below -> centre horizontally */ layout.textBoxBounds.setX ((localBounds.getWidth() - textBoxWidth) / 2);

                if (textBoxPos == Slider::TextBoxAbove)          layout.textBoxBounds.setY (0);
                else if (textBoxPos == Slider::TextBoxBelow)     layout.textBoxBounds.setY (localBounds.getHeight() - textBoxHeight);
                else /* left or right -> centre vertically */    layout.textBoxBounds.setY ((localBounds.getHeight() - textBoxHeight) / 2);
            }
        }

        // 3. set the slider bounds

        layout.sliderBounds = localBounds;

        if (slider.isBar())
        {
            layout.sliderBounds.reduce (1, 1);   // bar border
        }
        else
        {
            if (textBoxPos == Slider::TextBoxLeft)       layout.sliderBounds.removeFromLeft (textBoxWidth);
            else if (textBoxPos == Slider::TextBoxRight) layout.sliderBounds.removeFromRight (textBoxWidth);
            else if (textBoxPos == Slider::TextBoxAbove) layout.sliderBounds.removeFromTop (textBoxHeight);
            else if (textBoxPos == Slider::TextBoxBelow) layout.sliderBounds.removeFromBottom (textBoxHeight);

            const int thumbIndent = getSliderThumbRadius (slider);

            if (slider.isHorizontal())    layout.sliderBounds.reduce (thumbIndent, 0);
            else if (slider.isVertical()) layout.sliderBounds.reduce (0, thumbIndent);
        }

        return layout;
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

        juce::LookAndFeel_V4* getModulationKnobLookAndFeel(){
            return &modulationKnobLookAndFeel;
        }
  
        TelegraphLookAndFeelProvider(TelegraphLookAndFeelProvider const&)=delete;              
        void operator=(TelegraphLookAndFeelProvider const&)=delete; 

    private:
        
        TelegraphLookAndFeelProvider()
        {
        }

        IconLookAndFeel iconLookAndFeel;
        ModulationKnobLookAndFeel modulationKnobLookAndFeel;
};