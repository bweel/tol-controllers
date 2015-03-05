#include "EnvironmentModifierController.h"



void EnvironmentModifierController::putAllModulesToReserve()
{
    // connect real modules to objects and store them into map and stack
    for (id_t i = 1; i <= NUMBER_OF_MODULES; i++)
    {
        Node * root = getFromDef(MODULE_DEF_BASE_NAME + std::to_string(i));
        if (root)
        {
            Field * translation = root->getField("translation");
            Field * rotation = root->getField("rotation");
            
            Position position = Position(
                                 (ARENA_SIZE/2) + SPARE_DISTANCE,
                                 GROUND_HEIGHT,
                                 i);
            
            translation->setSFVec3f(position.getTransaltion());
            rotation->setSFRotation(position.getRotation());
        }
        else
        {
            std::cout << MODULE_DEF_BASE_NAME + std::to_string(i) + " does not exist." << std::endl;
        }
    }
}


void EnvironmentModifierController::putModuleToReserve(std::string moduleName)
{
    for (int i = 1; i <= NUMBER_OF_MODULES; i++)
    {
        Node * root = getFromDef(MODULE_DEF_BASE_NAME + std::to_string(i));
        if (root)
        {
            Field * name = root->getField("name");
            
            std::string toRemoveSubStr = moduleName.substr(0,moduleName.find(":"));
            std::string toCheckSubStr = name->getSFString().substr(0,name->getSFString().find(":"));
            
            if (toRemoveSubStr.compare(toCheckSubStr) == 0)
            {
                Field * controller = root->getField("controller");
                controller->setSFString("void");
                
                //Field * controllerArgs = root->getField("controllerArgs");
                //controllerArgs->setSFString("");
                
                Field * CB1XLocked = root->getField("CB1XLocked");
                Field * CB1YLocked = root->getField("CB1YLocked");
                Field * CB1ZLocked = root->getField("CB1ZLocked");
                Field * CW1XLocked = root->getField("CW1XLocked");
                Field * CW1ZLocked = root->getField("CW1ZLocked");
                Field * CB2XLocked = root->getField("CB2XLocked");
                Field * CB2ZLocked = root->getField("CB2ZLocked");
                Field * CW2XLocked = root->getField("CW2XLocked");
                Field * CW2YLocked = root->getField("CW2YLocked");
                Field * CW2ZLocked = root->getField("CW2ZLocked");
                
                CB1XLocked->setSFBool(false);
                CB1YLocked->setSFBool(false);
                CB1ZLocked->setSFBool(false);
                CW1XLocked->setSFBool(false);
                CW1ZLocked->setSFBool(false);
                CB2XLocked->setSFBool(false);
                CB2ZLocked->setSFBool(false);
                CW2XLocked->setSFBool(false);
                CW2YLocked->setSFBool(false);
                CW2ZLocked->setSFBool(false);
                
                Field * translation = root->getField("translation");
                Field * rotation = root->getField("rotation");
                
                Position position = Position(
                                             (ARENA_SIZE/2) + SPARE_DISTANCE,
                                             GROUND_HEIGHT,
                                             i);
                
                translation->setSFVec3f(position.getTransaltion());
                rotation->setSFRotation(position.getRotation());
                
                std::cout << moduleName << " to reserve" << std::endl;
                
                sendUpdateAvailableMessageToBirthClinic(MODULE_DEF_BASE_NAME + std::to_string(i));
            }
        }
        else
        {
            std::cout << MODULE_DEF_BASE_NAME + std::to_string(i) + " does not exist." << std::endl;
        }
    }
}


void EnvironmentModifierController::sendInitializedEnvironmentMessage()
{
    std::string message = "[ENVIRONMENT_OK_MESSAGE]";
    message = MessagesManager::add(message, "SDAT", simulationDateAndTime);
    emitter->setChannel(EVOLVER_CHANNEL);
    emitter->send(message.c_str(), (int)message.length()+1);
    emitter->setChannel(CLINIC_CHANNEL);
    emitter->send(message.c_str(), (int)message.length()+1);
    emitter->setChannel(SCREENSHOT_CHANNEL);
    emitter->send(message.c_str(), (int)message.length()+1);
}


void EnvironmentModifierController::sendUpdateAvailableMessageToBirthClinic(std::string moduleDef)
{
    emitter->setChannel(CLINIC_CHANNEL);
    std::string message = "[UPDATE_AVAILABLE_MESSAGE]";
    message = MessagesManager::add(message, "DEF", moduleDef);
    emitter->send(message.c_str(), (int)message.length()+1);
}


//////////////////////////////////////////////////////////////////
///////////////////////// MAIN FUNCTIONS /////////////////////////
//////////////////////////////////////////////////////////////////


EnvironmentModifierController::EnvironmentModifierController()
{
    std::time_t rawtime;
    std::tm* timeinfo;
    char buffer [80];
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    std::strftime(buffer,80,"%Y-%m-%d-%H.%M.%S",timeinfo);
    simulationDateAndTime = buffer;
    
    std::string argumentsDirectory = CONTROLLER_ARGS_PATH + simulationDateAndTime;
    boost::filesystem::path argsPath(argumentsDirectory);
    if (boost::filesystem::exists(argsPath) && boost::filesystem::is_directory(argsPath)) {
        throw std::runtime_error("Directory Already Existing");
    }
    boost::filesystem::create_directories(argsPath);
    
    std::string resultsDirectory = RESULTS_PATH + simulationDateAndTime;
    boost::filesystem::path resultsPath(resultsDirectory);
    if (boost::filesystem::exists(resultsPath) && boost::filesystem::is_directory(resultsPath)) {
        throw std::runtime_error("Directory Already Existing");
    }
    boost::filesystem::create_directories(resultsPath);
    
    emitter = getEmitter(EMITTER_NAME);
    emitter->setChannel(EVOLVER_CHANNEL);
    receiver = getReceiver(RECEIVER_NAME);
    receiver->setChannel(MODIFIER_CHANNEL);
}


EnvironmentModifierController::~EnvironmentModifierController()
{
    
}


void EnvironmentModifierController::run()
{
    double TIME_STEP = getBasicTimeStep();
    
    receiver->enable(TIME_STEP);
    while (receiver->getQueueLength() > 0)
    {
        receiver->nextPacket();
    }
    
    putAllModulesToReserve();
    
    sendInitializedEnvironmentMessage();
    
    while (step(TIME_STEP) != -1)
    {
        while(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            
            if (message.substr(0,20).compare("[TO_RESERVE_MESSAGE]") == 0)
            {
                std::string moduleName = MessagesManager::get(message, "NAME");
                putModuleToReserve(moduleName);
            }
            
            receiver->nextPacket();
        }
    }
}