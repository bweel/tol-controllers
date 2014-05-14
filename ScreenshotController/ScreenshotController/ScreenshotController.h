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

using namespace webots;

class ScreenshotController : public Supervisor
{
    
private:
    static const int SCREENSHOT_INTERVAL = 25; // in seconds

    double lastScreenshot = -1;
public:
    
    ScreenshotController();
    
    ~ScreenshotController();
    
    void run();
    
};


#endif /* defined(__ScreenshotController__ScreenshotController__) */
