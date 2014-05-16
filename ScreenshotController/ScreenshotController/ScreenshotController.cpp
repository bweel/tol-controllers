//
//  ScreenshotController.cpp
//  ScreenshotController
//
//  Created by Berend Weel on 14/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "ScreenshotController.h"
#include "Defines.h"

ScreenshotController::ScreenshotController() : Supervisor() {
    receiver = getReceiver(RECEIVER_NAME);
    receiver->setChannel(SCREENSHOT_CHANNEL);
}

ScreenshotController::~ScreenshotController() {
    
}

void ScreenshotController::run() {
    double timeStep = getBasicTimeStep();
    
    receiver->enable(timeStep);
    
    // wait unitil EnvironmentModifierController sais the environment is ok
    bool environmentOk = false;
    while (step(timeStep) != -1 && !environmentOk)
    {
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            if (message.substr(0,14).compare("ENVIRONMENT_OK") == 0)
            {
                environmentOk = true;
                simulationDateAndTime = message.substr(14,message.length());
            }
            receiver->nextPacket();
        }
    }
    
    boost::filesystem::path dirpath(getProjectPath());
    dirpath /= "Results";
    dirpath /= simulationDateAndTime;
    dirpath /= "screenshots";
    
    if(!boost::filesystem::exists(dirpath)){
        boost::filesystem::create_directories(dirpath);
    }
    
    while (step(timeStep) != -1) {
        double now = getTime();
        if(lastScreenshot < 0 || now - lastScreenshot > SCREENSHOT_INTERVAL){
            boost::filesystem::path filepath = dirpath;
            filepath /= boost::lexical_cast<std::string>(now) + ".jpg";
            
//            std::cout << "Screenshot taken." << std::endl;
            exportImage(filepath.string(), 100);
            lastScreenshot = now;
        }
    }
}