#ifndef EnvironmentModifierController_EnvironmentModifierController_h
#define EnvironmentModifierController_EnvironmentModifierController_h

#include "Defines.h"
#include "Position.h"
#include "ParametersReader.h"
#include "MessagesManager.h"

#include <webots/Supervisor.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <iostream>


using namespace webots;


class EnvironmentModifierController : public Supervisor
{
    
private:
    
    int MODIFIER_CHANNEL = ParametersReader::get<int>("MODIFIER_CHANNEL");
    int CLINIC_CHANNEL = ParametersReader::get<int>("CLINIC_CHANNEL");
    int EVOLVER_CHANNEL = ParametersReader::get<int>("EVOLVER_CHANNEL");
    int SCREENSHOT_CHANNEL = ParametersReader::get<int>("SCREENSHOT_CHANNEL");
    
    int NUMBER_OF_MODULES = ParametersReader::get<int>("NUMBER_OF_MODULES");
    int ARENA_SIZE = ParametersReader::get<int>("ARENA_SIZE");
    
    int TIME_STEP = ParametersReader::get<int>("TIME_STEP");
    
    Receiver * receiver;
    Emitter * emitter;
    
    std::string simulationDateAndTime;
    
    
    void putAllModulesToReserve();
    
    void putModuleToReserve(std::string moduleName);
    
    void sendInitializedEnvironmentMessage();
    
    void sendUpdateAvailableMessageToBirthClinic(std::string moduleDef);
        
    
    
public:
    
    EnvironmentModifierController();
    
    ~EnvironmentModifierController();
    
    void run();
    
};

#endif