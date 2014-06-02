//
//  main.cpp
//  TimingController
//
//  Created by Berend Weel on 27/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "TimingController.h"

int main(int argc, const char * argv[])
{
    TimingController* controller = new TimingController();
    controller->run();
    delete controller;
    return 0;
}

