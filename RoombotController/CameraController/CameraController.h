#ifndef CAMERA_CONTROLLER_H
#define	CAMERA_CONTROLLER_H

#include <webots/Supervisor.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <vector>

using namespace webots;

class CameraController : public Supervisor
{
    
public:    
    CameraController();             // contructor
    
    virtual ~CameraController();    // deconstructor
    
    void run();                 // robot behavior
    
private:
    boost::property_tree::ptree * initParameters(const std::string & path);
    
    Camera* camera;
    Emitter *emitter;
    Receiver *receiver;
    int numMotors;
    
    double *torque;
    double *previousPosition;
    double *angularDisplacement;
    double *position;
    double *energy;
    
    double TIME_STEP;
    
    boost::property_tree::ptree * parameters;
    
    bool isRoot();
    void sendEnergyUsage(double energy);
    std::vector<Motor*> initializeMotors();
    std::vector<PositionSensor*> initializePositionSensors();
    void initialize();
    
    int type;
    std::size_t robotIndex;
    
    // Number of joules in the battery
    long int batteryMax;
    long int battery;
};

#endif	/* CAMERA_CONTROLLER_H */