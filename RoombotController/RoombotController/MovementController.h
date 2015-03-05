//
//  MovementController.h
//  RoombotController
//
//  Created by Berend Weel on 21/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef __RoombotController__MovementController__
#define __RoombotController__MovementController__

#include <vector>
#include <webots/Robot.hpp>

#include "WorldModel.h"
#include "MessageHandler.h"
#include "Logger.h"
#include "LearningController.h"
#include "Defines.h"

using namespace webots;

typedef std::vector<double> dvector;
typedef std::vector<dvector> doubledvector;

class MovementController {
    
public:
    MovementController(WorldModel &wm, MessageHandler &mh, Robot *robot);
    
    /**
     * Get the current servo angle value normalizing it between [-1;1]
     * @return Servo angle value normalized between [-1;1]
     */
    double getMotorPosition(size_t index);
    
    /**
     * Sets the servo angle normalizing it between [-pi/2;pi/2]
     * @param Servo angle value normalized between [-1;1]
     */
    void setMotorPosition(size_t index, double value);
    
    void step();
    
    void initialise();
    void initialiseMotors();
    void normaliseMotors();
    
    void sendAngles(doubledvector nextPositions);
    void sendAngles(size_t index, dvector currentPositions);

    doubledvector receiveAngles();
    dvector receiveAngles(size_t index);
private:
    WorldModel &worldModel;
    MessageHandler &messageHandler;
    Robot *robot;
    
    log4cpp::Category &logger;
    
    std::unique_ptr<LearningController> learningController;
    
    double tMinusOne;
    double tPlusOne;
    
    doubledvector currentPositions;
    doubledvector nextPositions;
    dvector anglesTMinusOne;
    dvector anglesTPlusOne;
    
    std::vector<Motor*> motors;
//    std::vector<PositionSensor*> sensors;       // motor sensors
    double motorRange;                          // range of motors
    
    void logGPS();
};

#endif /* defined(__RoombotController__MovementController__) */
