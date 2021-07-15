#pragma once
#include "SystemConstants.h"

static void crashLogger (void* data);


class TelegraphLogger {
    public:
        static TelegraphLogger& Instance(){
            static TelegraphLogger instance;
            
            return instance;
        }
        void logMessage(const juce::String & msg){
            logger.logMessage(msg);
        }

        TelegraphLogger(TelegraphLogger const&)=delete;              
        void operator=(TelegraphLogger const&)=delete; 

    private:
        
        TelegraphLogger()
        :logger(juce::FileLogger(PROJECT_DIR_PATH+juce::File::getSeparatorString()+"telegraph_synth.log","LOG START"))
        {
            juce::SystemStats::setApplicationCrashHandler ( 
                crashLogger
            );
        }
        juce::FileLogger logger;

};

static void crashLogger (void* data) {
    auto& logger = TelegraphLogger::Instance();
    logger.logMessage(juce::SystemStats::getStackBacktrace() );
    logger.logMessage(juce::SystemStats::getStackBacktrace() );
}
