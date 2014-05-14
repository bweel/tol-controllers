#ifndef EnvironmentModifierController_EnvironmentModifierController_h
#define EnvironmentModifierController_EnvironmentModifierController_h

#include "Defines.h"
#include "Position.h"

#include <webots/Supervisor.hpp>
#include <iostream>


using namespace webots;


class EnvironmentModifierController : public Supervisor
{
    
private:
    
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