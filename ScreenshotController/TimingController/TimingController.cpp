//
//  TimingController.cpp
//  ScreenshotController
//
//  Created by Berend Weel on 27/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "Defines.h"
#include "TimingController.h"

void TimingController::run() {

    double TIME_STEP = getBasicTimeStep();
    
    double startTime = getTime();
    
    std::cout << BOLDBLUE << "Starting Timing Supervisor, running experiment untill " << experimentLength << " seconds have passed, reverting when done" << RESET << std::endl;
    
    while (step(TIME_STEP) != -1) {
        double now = getTime();
        
        if(now - startTime >= experimentLength){
            simulationRevert();
        }
    }
}