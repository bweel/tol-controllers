//
//  ScreenshotController.h
//  ScreenshotController
//
//  Created by Berend Weel on 14/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef __ScreenshotController__ScreenshotController__
#define __ScreenshotController__ScreenshotController__

#include <webots/Supervisor.hpp>
#include <stack>
#include <limits>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "Defines.h"
#include "ParametersReader.h"
#include "MessagesManager.h"

using namespace webots;

class ScreenshotController : public Supervisor
{
    
private:
    
    int SCREENSHOT_CHANNEL = ParametersReader::get<int>("SCREENSHOT_CHANNEL");
    
    int TIME_STEP = ParametersReader::get<int>("TIME_STEP");
    
    static const int SCREENSHOT_INTERVAL = 5; // in seconds

    double lastScreenshot = -1;
    std::string simulationDateAndTime;
    
    Receiver * receiver;
    
public:
    
    ScreenshotController();
    
    ~ScreenshotController();
    
    void run();
    
};


#endif /* defined(__ScreenshotController__ScreenshotController__) */
