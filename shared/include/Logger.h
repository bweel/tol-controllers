//
//  Logger.h
//  shared
//
//  Created by Berend Weel on 03/07/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef shared_Logger_h
#define shared_Logger_h

#define LOG4CPP_FIX_ERROR_COLLISION 1

#include <log4cpp/Category.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>

class Logger {
public:
    static void initialise(){
        if(!initialised){
            std::string initFileName = "../../log4cpp.properties";
            log4cpp::PropertyConfigurator::configure(initFileName);
            initialised = true;
        }
    }
    
    static void shutdown() {
        log4cpp::Category::shutdown();
        initialised = false;
    }
    
    static log4cpp::Category& getInstance(const std::string &name) {
        if(!initialised){
            initialise();
        }
        return log4cpp::Category::getInstance(name);
    }
    
    static log4cpp::Category& getRoot(){
        if(!initialised){
            initialise();
        }
        return log4cpp::Category::getRoot();
    }
    
private:
    static bool initialised;
};

#endif
