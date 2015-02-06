//
//  MovementController.cpp
//  RoombotController
//
//  Created by Berend Weel on 21/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "MovementController.h"

MovementController::MovementController(WorldModel &wm, MessageHandler &mh, Robot *robot) :
    worldModel(wm),
    messageHandler(mh),
    robot(robot),
    logger(Logger::getInstance("MovementController"))
{
    logger.debugStream() << worldModel.now << " " << worldModel.robotName << " Movement Controller Created";
}

void MovementController::initialise() {
    initialiseMotors();
    
    if(worldModel.iAmRoot()){
        learningController = std::unique_ptr<LearningController>(new LearningController(worldModel));
        learningController->initialise();
        
        // initialize motors' angles (notice that root modules are 1 timestep further than the others)
        anglesIn = doubledvector(worldModel.organismSize,std::vector<double>(motors.size(),0.0));     // input angles
        anglesOut = doubledvector(worldModel.organismSize,std::vector<double>(motors.size(),0.0));    // output angles
        anglesTMinusOne = dvector(motors.size(),0.0);   // angles for root at time t-1
        anglesTPlusOne = dvector(motors.size(),0.0);    // angles for root at time t+1
    }
    
    for(size_t i=0;i<motors.size();i++)
    {
        setMotorPosition(i,0.0);
    }
    
    tMinusOne = 0;
    tPlusOne = 0;
}

void MovementController::step() {
    if(worldModel.iAmRoot()) {
        anglesIn = receiveAngles();     // read current angles
        
        // Set the angles for the root module with the ones at time T-1
        for(size_t i=0;i<motors.size();i++) {
            anglesIn[worldModel.robotIndex][i] = anglesTMinusOne[i];
            anglesTMinusOne[i] = getMotorPosition(i);    // update anglesTMinusOne for the next iteration
            tMinusOne = worldModel.now;                          // update tMinusOne for the next iteration
        }
        
        anglesOut = learningController->computeAngles(anglesIn);  // compute new angles
        sendAngles(anglesOut);                  // send them usign the emitter
        
        for(size_t i=0;i<motors.size();i++) {
            setMotorPosition(i,anglesTPlusOne[i]);   // root is one timestep further
            anglesTPlusOne[i] = anglesOut[worldModel.robotIndex][i]; // update anglesTPlusOne for the next iteration
            tPlusOne = worldModel.now;                       // update tPlusOne for the next iteration
        }
        
        learningController->step();
    } else {
        dvector anglesOut = dvector(motors.size(),0.0);
        for(size_t i=0;i<motors.size();i++) {
            anglesOut[i] = getMotorPosition(i);
        }
        sendAngles(worldModel.robotIndex,anglesOut);         // send angles using emitter
        
        dvector nextAngles = receiveAngles(worldModel.robotIndex);
        for(size_t i=0;i<motors.size();i++) {
            setMotorPosition(i,nextAngles[i]);
        }

    }
}

double MovementController::getMotorPosition(size_t index) {
    double value = sensors[index]->getValue() / motorRange;
    return value;
}

// set new position of motors
void MovementController::setMotorPosition(size_t index, double value) {
    value = value < -1.0 ? -1.0 : value;
    value = value >  1.0 ?  1.0 : value;
    
    value *= motorRange;
    
    motors[index]->setPosition(value);
}

// initialize motors
void MovementController::initialiseMotors() {
    int numMotors = worldModel.parameters.get<unsigned int>("Robot.Motor.Number");
    motorRange = worldModel.parameters.get<double>("Robot.Motor.Range");
    
    for(size_t i=0;i<numMotors;i++) {
        std::string motorParameterName = "Robot.Motor."+std::to_string(i);
        std::string motorName = worldModel.parameters.get<std::string>(motorParameterName);
        logger.debugStream() << worldModel.now << " " << worldModel.robotName << ": Initialising Motor: " << motorName;
        
        motors.push_back(robot->getMotor(motorName));
        if (!motors[i]) {
            throw std::runtime_error("Device Not Found: "+to_string(motorName));
        }
        
        std::string sensorName = "position"+worldModel.parameters.get<std::string>(motorParameterName);
        logger.debugStream() << worldModel.now << " " << worldModel.robotName << ": Initialising position sensor: " << sensorName;
        
        sensors.push_back(robot->getPositionSensor(sensorName));
        if (!sensors[i]) {
            throw std::runtime_error("Device Not Found: "+to_string(motorName));
        }
        sensors[i]->enable(worldModel.TIME_STEP);
    }
}

void MovementController::normaliseMotors() {
    for(int i = 0; i < motors.size(); i++) {
        setMotorPosition(i, 0);
    }
}

// use the emitter to send motors' angles
void MovementController::sendAngles(doubledvector anglesOut){
    if(anglesOut.size() > 0){
        using namespace boost::property_tree;
        ptree data;
        data.put("timestamp",worldModel.now);
        for(int i=0;i<anglesOut.size();i++){
            ptree sub;
            for(int j=0;j<anglesOut[i].size();j++){
                sub.put(std::to_string(j),anglesOut[i][j]);
            }
            data.add_child(std::to_string(i), sub);
        }
        
        std::ostringstream datastring;
        boost::property_tree::json_parser::write_json(datastring,data,false);

        messageHandler.send(datastring.str());
    }
}

// send angles using emitter
void MovementController::sendAngles(size_t index, dvector anglesOut){
    if(anglesOut.size() > 0){
        using namespace boost::property_tree;
        ptree data;
        data.put("index",std::to_string(index));
        data.put("timestamp",worldModel.now);
        for(int j=0;j<anglesOut.size();j++){
            data.put(std::to_string(j),anglesOut[j]);
        }
        
        std::ostringstream datastring;
        boost::property_tree::json_parser::write_json(datastring,data,false);
        //        std::cout << "[" << getTime() << "] sendAngles(index) " << getName() << " sending data: " << datastring.str() << std::endl;
        messageHandler.send(datastring.str());
    }
}

// get the angles for each motors of each module of the organisms
doubledvector MovementController::receiveAngles()
{
    doubledvector result(worldModel.organismSize,dvector(worldModel.numMotors,0.0));
    
//    std::cout << "[" << getTime() << "] receiveAngles " << getName() << ": queue length: " << _receiver->getQueueLength()  << std::endl;
    while(messageHandler.hasMessage()){
//        std::cout << "[" << getTime() << "] receiveAngles " << getName() << ": received data of size: " << _receiver->getDataSize() << " data: " << (char*)_receiver->getData() << std::endl;
        
        using namespace boost::property_tree;
        ptree data;
        std::istringstream stream(messageHandler.receive());
        boost::property_tree::json_parser::read_json(stream, data);
        
        int index = data.get<double>("index");
        for(int j=0;j<worldModel.numMotors;j++)
        {
            result[index][j] = data.get<double>(std::to_string(j));
        }
        
//        std::cout << "[" << getTime() << "] " << getName() << " received vector with: " << result.size() << " elements" << std::endl;
//        std::cout << "[" << getTime() << "] " << getName() << " received angles of timestep: " << data.get<double>("timestamp") << std::endl;
        
        messageHandler.next();
    }
    
    return result;
}

// receive angles using receiver
dvector MovementController::receiveAngles(size_t index){
    dvector result(worldModel.numMotors,0.0);
    
//    std::cout << "[" << getTime() << "] receiveAngles(index) " << getName() << ": queue length: " << _receiver->getQueueLength()  << std::endl;
    if(messageHandler.hasMessage()){
//        std::cout << "[" << getTime() << "] receiveAngles(index) " << getName() << ": received data of size: " << _receiver->getDataSize() << " data: " << (char*)_receiver->getData() << std::endl;
        
        using namespace boost::property_tree;
        std::istringstream stream(messageHandler.receive());
        ptree data;
        boost::property_tree::json_parser::read_json(stream, data);
        
        ptree sub = data.get_child(std::to_string(index));
        for(int j=0;j<worldModel.numMotors;j++){
            result[j] = sub.get<double>(std::to_string(j));
        }
        
//        std::cout << "Received vector with: " << result.size() << " elements:" << std::endl;
//        for(int j=0;j<numMotors;j++){
//            std::cout << result[j] << ", ";
//        }
//        std::cout << std::endl;
//        std::cout << "[" << getTime() << "] " << getName() << " got angles with timestamp: " << data.get<double>("timestamp") << std::endl;
        
        messageHandler.next();
    } else {
//        std::cerr << "[" << getTime() << "] " << getName() << " did not receive any angle updates from root!" << std::endl;
    }
    
    return result;
}