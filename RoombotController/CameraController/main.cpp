//
//  main.cpp
//  CameraController
//
//  Created by Berend Weel on 13/10/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "CameraController.h"

int main(int arg_c, char ** arg_v)
{
    CameraController * robot = new CameraController();
    robot->run();
    delete(robot);
}
