//
//  main.cpp
//  ScreenshotController
//
//  Created by Berend Weel on 14/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "ScreenshotController.h"

int main(int argc, char **argv)
{
    ScreenshotController* controller = new ScreenshotController();
    controller->run();
    delete controller;
    return 0;
}

