#ifndef EnvironmentModifierController_EnvironmentModifierController_h
#define EnvironmentModifierController_EnvironmentModifierController_h

#include "Defines.h"
#include "Position.h"
#include "ParametersReader.h"

#include <webots/Supervisor.hpp>
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
    
    Receiver * receiver;
    Emitter * emitter;
    
    std::string simulationDateAndTime;
    
    
    void putAllModulesToReserve();
    
    void putModuleToReserve(std::string moduleName);
    
    void sendInitializedEnvironmentMessage();
    
    void sendUpdateAvailableMessageToBirthClinic(std::string moduleDef);
    
    std::string readToReserveMessage(std::string message);
    
    
    
public:
    
    EnvironmentModifierController();
    
    ~EnvironmentModifierController();
    
    void run();
    
};

#endif