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

ScreenshotController::ScreenshotController() {
    
}

ScreenshotController::~ScreenshotController() {
    
}

void ScreenshotController::run() {
    double timeStep = getBasicTimeStep();
    
    boost::filesystem::path dirpath(getProjectPath());
    dirpath /= "Results";
    dirpath /= "screenshots";
    
    if(!boost::filesystem::exists(dirpath)){
        boost::filesystem::create_directories(dirpath);
    }
    
    while (step(timeStep) != -1) {
        double now = getTime();
        if(lastScreenshot < 0 || now - lastScreenshot > SCREENSHOT_INTERVAL){
            boost::filesystem::path filepath = dirpath;
            filepath /= boost::lexical_cast<std::string>(now) + ".jpg";
            
            std::cout << "Screenshot taken." << std::endl;
            exportImage(filepath.string(), 100);
            lastScreenshot = now;
        }
    }
}