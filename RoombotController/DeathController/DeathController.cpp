#include "DeathController.h"


DeathController::DeathController() {
}

DeathController::~DeathController() {
    
}

void DeathController::run() {
    double timeStep = getBasicTimeStep();

    initialiseParameters(getControllerArguments());
    initialiseMotors(timeStep);
    
    while (step(timeStep)) {
        for(int i = 0; i < motors.size(); i++) {
            motors[i]->setPosition(0);
        }
    }
}

// read parameters tree
void DeathController::initialiseParameters(const std::string & path) {
    boost::property_tree::read_json(path, parameters);
}

// initialize motors
void DeathController::initialiseMotors(double timeStep) {
    int numMotors = parameters.get<unsigned int>("Robot.Motor.Number");
    
    for(size_t i=0;i<numMotors;i++){
        std::string motorName("Robot.Motor."+std::to_string(i));
        
        Motor *motor = getMotor(parameters.get<std::string>(motorName));
        if (!motor) {
            throw std::runtime_error("Device Not Found");
        }
        
        motors.push_back(motor);
    }
}