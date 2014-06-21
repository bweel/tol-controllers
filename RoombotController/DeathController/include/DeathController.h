#ifndef RoombotController_DeathController_h
#define RoombotController_DeathController_h

#include "ParametersReader.h"

#include <webots/Robot.hpp>
#include <boost/property_tree/ptree.hpp>


using namespace webots;


class DeathController : public Robot
{
    
public:
    
    DeathController();
    
    virtual ~DeathController();
    
    void run();
    
private:
    
    double TIME_STEP;
    
    boost::property_tree::ptree * _parameters;
    
    unsigned int numMotors;
    double motorRange;
    Motor ** _motors;
    
    
    
    
    
    boost::property_tree::ptree * _init_parameters(const std::string & path);
        
    void _set_motor_position(size_t index, double value);
    
    Motor** _init_motors(double time_step);
    
};

#endif
