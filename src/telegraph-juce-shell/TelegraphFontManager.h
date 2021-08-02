#pragma once
#include <JuceHeader.h>
#include "fonts/NMCondensed.h"
#include "fonts/FAW.h"
#include "fonts/HPDisplay.h"

class TelegraphFontManager {
    public:
        static TelegraphFontManager& Instance(){
            static TelegraphFontManager instance;
            return instance;
        }
        const Font getDisplayFont(){
            auto font = Font(primaryDisplayFont);
            font.setHeight(48);
            return font;
        }
        const Typeface::Ptr getDisplayTypeface(){
            return primaryDisplayFont;
        }
        const Font getSecondaryHeaderFont(){
            auto font = juce::Font(TelegraphFontManager::Instance().getDisplayTypeface());
            font.setHeight(24);
            return font;
        }
        const Font getLabelFont(){
            auto font = Font(primaryLabelFont);
            font.setHeight(14);
            return font;
        }
        const Typeface::Ptr getLabelTypeface(){
            return primaryLabelFont;
        }
        const Font getIconFont(){
            auto font = Font(iconFont);
            font.setHeight(18);
            return font;
        }
        const Typeface::Ptr getIconTypeface(){
            return iconFont;
        }
       
        TelegraphFontManager(TelegraphFontManager const&)=delete;              
        void operator=(TelegraphFontManager const&)=delete; 

    private:
        
        TelegraphFontManager()
        {
            // primaryLabelFont = Typeface::createSystemTypefaceFor(Eunomia::EunomiaBold_otf, Eunomia::EunomiaBold_otfSize);
            primaryLabelFont = Typeface::createSystemTypefaceFor(NMCondensed::NMCondensedBold_ttf, NMCondensed::NMCondensedBold_ttfSize);
            // primaryLabelFont = Typeface::createSystemTypefaceFor(ORCB::OCRB_ttf, ORCB::OCRB_ttfSize);
            // primaryLabelFont = Typeface::createSystemTypefaceFor(USSRStencil::USSR_STENCIL_WEBFONT_ttf, USSRStencil::USSR_STENCIL_WEBFONT_ttfSize);
            primaryDisplayFont   = Typeface::createSystemTypefaceFor(HPDisplay::HPDisplayRegular_ttf, HPDisplay::HPDisplayRegular_ttfSize);
            // primaryDisplayFont   = Typeface::createSystemTypefaceFor(Eunomia::EunomiaBold_otf, Eunomia::EunomiaBold_otfSize);
            // primaryDisplayFont   = Typeface::createSystemTypefaceFor(USSRStencil::USSR_STENCIL_WEBFONT_ttf, USSRStencil::USSR_STENCIL_WEBFONT_ttfSize     );
            // primaryDisplayFont   = Typeface::createSystemTypefaceFor(ARKES::ARKESDenseRegular_ttf,          ARKES::ARKESDenseRegular_ttfSize         );
            iconFont   = Typeface::createSystemTypefaceFor(FAW::FAIconFont_ttf, FAW::FAIconFont_ttfSize);

        }

    juce::Typeface::Ptr primaryDisplayFont;
    juce::Typeface::Ptr primaryLabelFont;
    juce::Typeface::Ptr iconFont;
      
};