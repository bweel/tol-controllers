//
//  TimingController.h
//  ScreenshotController
//
//  Created by Berend Weel on 27/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef __ScreenshotController__TimingController__
#define __ScreenshotController__TimingController__

#include <webots/Supervisor.hpp>
#include "ParametersReader.h"

using namespace webots;

class TimingController : public Supervisor {
private:
    double experimentLength = ParametersReader::get<double>("EXPERIMENT_TIME");
    int TIME_STEP = ParametersReader::get<int>("TIME_STEP");
    
public:
    void run ();
};

#endif /* defined(__ScreenshotController__TimingController__) */
