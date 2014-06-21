#include "DeathController.h"


DeathController::DeathController():

_parameters(_init_parameters(getControllerArguments())),
numMotors(0),
motorRange(0),
TIME_STEP(getBasicTimeStep()),
_motors(_init_motors(TIME_STEP))
{
    
}

DeathController::~DeathController()
{
    
}

void DeathController::run()
{
    while (true)
    {
        for(int i = 0; i < numMotors; i++)
        {
            _set_motor_position(i, 0);
        }
        if (step(TIME_STEP) == -1)
        {
            return;
        }
    }
}

// read parameters tree
boost::property_tree::ptree * DeathController::_init_parameters(const std::string & path)
{
    boost::property_tree::ptree * result = new boost::property_tree::ptree();
    boost::property_tree::read_json(path, (* result));
    return result;
}

// set new position of motors
void DeathController::_set_motor_position(size_t index, double value)
{
    if (value < -1.0) {
        value = -1.0;
    }
    if (value > 1.0) {
        value = 1.0;
    }
    
    value *= motorRange;
    
    _motors[index]->setPosition(value);
    
#ifdef DEBUG_CONTROLLER
    std::cout << "[" << getTime() << "] " << getName() << ": Set Servo Angle " << value << std::endl;
#endif
}

// initialize motors
Motor** DeathController::_init_motors(double time_step)
{
    numMotors = _parameters->get<unsigned int>("Robot.Motor.Number");
    motorRange = _parameters->get<double>("Robot.Motor.Range");
    
    Motor** motor = new Motor*[numMotors];
    for(size_t i=0;i<numMotors;i++){
        std::string motorName("Robot.Motor."+std::to_string(i));
        
        motor[i] = getMotor(_parameters->get<std::string>(motorName));
        if (!motor[i]) {
            throw std::runtime_error("Device Not Found");
        }
        motor[i]->enablePosition(time_step);
    }
#ifdef DEBUG_CONTROLLER
    std::cout << "[" << getTime() << "] " << getName() << ": Init Servo" << std::endl;
#endif
    return motor;
}