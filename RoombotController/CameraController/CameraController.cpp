#include "CameraController.h"
#include "MessagesManager.h"
#include <cmath>
#include <boost/lexical_cast.hpp>
#include "Defines.h"

#define EFFICIENCY_FACTOR 1.67

CameraController::CameraController() :
    camera(NULL),
    parameters(initParameters(getControllerArguments()))
{
}

CameraController::~CameraController()
{
}

// read parameters tree
boost::property_tree::ptree * CameraController::initParameters(const std::string & path)
{
    boost::property_tree::ptree * result = new boost::property_tree::ptree();
    boost::property_tree::read_json(path, (* result));
    return result;
}

void CameraController::initialize() {
    std::cout << "Initializing robot: " << getName() << std::endl;
    emitter = getEmitter("Emitter");
    receiver = getReceiver("Receiver");
    
    TIME_STEP = getBasicTimeStep();
    
    type = parameters->get<int>("Robot." + getName() + ".Type");
    robotIndex = parameters->get<std::size_t>("Robot."+ getName() +".Index");
    
    // Battery is rated in joules here for easy conversion from the motor work
    // Battery specifications are usually in mAh (milliamp-hours).
    // To convert you need the nominal voltage.
    //
    // Reminder:
    // 1 Joule = 1 Newton Meter  ( kg * m^2 / s^2 )
    // 1 Watt-hour = 3600 Joules
    //
    // Roombot battery is 1200 mAh @ 12 V (taken from http://biorob.epfl.ch/roombots )
    // so 1200 mAh * 12 V / 1000 = 14.4 watt-hours (12V taken from https://fmcc.faulhaber.com/details/overview/PGR_3877_13818/PGR_13818_13813/en/ )
    // * 3600 = 51840 Joules
    batteryMax = ((1200 * 12) / 1000) * 3600;
    // Multiply by the number of modules to simulate power sharing
    batteryMax = batteryMax * parameters->get<int>("Robot.Modules_#");
    battery = batteryMax;
    
    // initialization for non-root module
    if (!isRoot()) {
        // set non-root module's emitter and receiver using the intex of the root
        emitter->setChannel(1);
        receiver->setChannel(2);
        receiver->enable(TIME_STEP);
        
        return;
    } else {
        // set root module's emitter and receiver as inverse of the ones for non-root modules
        // in order to make possible the communitation between master and slave
        emitter->setChannel(2);
        receiver->setChannel(1);
        receiver->enable(TIME_STEP);
    }
}

std::vector<Motor*> CameraController::initializeMotors() {
    numMotors = parameters->get<int>("Robot.Motor.Number");
    std::cout << getName() << ": Initializing " << numMotors << " motors...";
    std::vector<Motor*> motor;
    for(size_t i=0;i<numMotors;i++){
        std::string motorName("Robot.Motor."+std::to_string(i));
        
        motor.push_back(getMotor(parameters->get<std::string>(motorName)));
        if (!motor[i]) {
            throw std::runtime_error("Device Not Found");
        }
        motor[i]->enableTorqueFeedback(TIME_STEP);
    }

    std::cout << "done." << std::endl;
    return motor;
}

std::vector<PositionSensor*> CameraController::initializePositionSensors() {
    numMotors = parameters->get<int>("Robot.Motor.Number");
    std::vector<PositionSensor*> sensors;
    for(size_t i=0;i<numMotors;i++){
        std::string motorName("Robot.Motor."+std::to_string(i));
        
        sensors.push_back(getPositionSensor("position"+parameters->get<std::string>(motorName)));
        if (!sensors[i]) {
            throw std::runtime_error("Device Not Found");
        }
        
        sensors[i]->enable(TIME_STEP);
    }
    
    return sensors;
}

// check if a module is root
bool CameraController::isRoot() {
    return type == M_ROOT;
}

void CameraController::sendEnergyUsage(double energy) {
    std::string message = "[ENERGY_UPDATE]";
    message = MessagesManager::add(message, "ID", std::to_string(robotIndex));
    message = MessagesManager::add(message, "ENERGY", std::to_string(energy));
    emitter->send(message.c_str(), (int)message.length()+1);
}

void CameraController::run() {
    double averageEnergy = 0;
    double initialised = false;
    
    torque = new double[numMotors];
    previousPosition = new double[numMotors];
    angularDisplacement = new double[numMotors];
    position = new double[numMotors];
    energy = new double[numMotors];
    
    initialize();
    std::vector<Motor*> motor = initializeMotors();
    std::vector<PositionSensor*> positionSensor = initializePositionSensors();
    
    double lastTime = 0;
    while (step(TIME_STEP) != -1)
    {
        if(!initialised){
            for(int i=0;i<numMotors;i++){
                position[i] = 0;
                previousPosition[i] = 0;
            }
        }
        if(battery <= 0){
            std::cout << "We ran out of battery! " << std::endl;
            return;
        }
        double time = getTime();
        
//        camera->getImage();
//        camera->saveImage("camera-"+std::to_string(t)+".png", 100);
        for(int i=0;i<numMotors;i++){
            // Conversion calculation from
            // http://en.wikipedia.org/wiki/Torque#Units
            // E = torque * Angle
            position[i] = positionSensor[i]->getValue();                  // Radians
//            std::cout << getName() << " Current motor position: " << position[i] << " previous position: " << previousPosition[i] << std::endl;
            torque[i] = std::abs(motor[i]->getTorqueFeedback());    // Nm
            
            angularDisplacement[i] = std::abs(previousPosition[i] - position[i]);
//            std:: cout << getName() << " angular displacement: " << angularDisplacement[i] << " ( " << previousPosition[i] - position[i] << " )" << std::endl;
            
            energy[i] = torque[i] * angularDisplacement[i];
//            std:: cout << getName() << " energy cost: " << energy[i] << std::endl;
            
            if(i==0||i==2){
                motor[i]->setPosition(sin(time+i));
            }
            previousPosition[i] = position[i];
        }
        
        
        if(!isRoot()){
//            std::cout << getName() << "Sending energy usage: " << energy[0]+energy[1]+energy[2] << std::endl;
            sendEnergyUsage(energy[0]+energy[1]+energy[2]);
        }else{
            double energyUsage = energy[0]+energy[1]+energy[2];
            while(receiver->getQueueLength() > 0)
            {
                std::string message = (char*)receiver->getData();
//                std::cout << getName() << "Received message: " << message << std::endl;
                
                if (message.substr(0,15).compare("[ENERGY_UPDATE]") == 0)
                {
//                    std::cout << "Received energy usage from: " << MessagesManager::get(message,"ID") << ": " << MessagesManager::get(message,"ENERGY") << std::endl;
                    energyUsage += boost::lexical_cast<double>(MessagesManager::get(message,"ENERGY"));
                }
                
                receiver->nextPacket();
            }
            
            // energy usage in Joules
            battery -= energyUsage * EFFICIENCY_FACTOR;
            averageEnergy += energyUsage * EFFICIENCY_FACTOR;
            if(time - lastTime > 10) {
                std::cout << getName() << " Energy usage: " << averageEnergy / (time-lastTime) << " BatteryLevel: " << battery << " (" << (((double)battery/(double)batteryMax)*100.0) << "%)" << std::endl;
                lastTime = time;
                averageEnergy = 0;
            }
        }
    }

    delete [] torque;
    delete [] previousPosition;
    delete [] angularDisplacement;
    delete [] position;
    delete [] energy;
}