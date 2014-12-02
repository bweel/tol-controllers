#ifndef RoombotController_DeathController_h
#define RoombotController_DeathController_h

#include "ParametersReader.h"

#include <webots/Robot.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>


using namespace webots;


class DeathController : public Robot
{
    
public:
    
    DeathController();
    
    virtual ~DeathController();
    
    void run();
    
private:
    
    void initialiseParameters(const std::string & path);
    
    void initialiseMotors(double timeStep);
    
    std::vector<Motor*> motors;
    boost::property_tree::ptree parameters;
};

#endif
