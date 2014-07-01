#ifndef BIRTH_CLINIC_H
#define BIRTH_CLINIC_H

#include "Module.h"
#include "Builder.h"
#include "Organism.h"
#include "RoombotModule.h"
#include "ParametersReader.h"
#include "MessagesManager.h"
#include "MindGenome.h"

#include <webots/Supervisor.hpp>
#include <stack>
#include <limits>

using namespace webots;

class BirthClinicController : public Supervisor
{

    private:
    
    std::string simulationDateAndTime;
    
    int CLINIC_CHANNEL = ParametersReader::get<int>("CLINIC_CHANNEL");
    int EVOLVER_CHANNEL = ParametersReader::get<int>("EVOLVER_CHANNEL");
    int SCREENSHOT_CHANNEL = ParametersReader::get<int>("SCREENSHOT_CHANNEL");
    
    int ARENA_SIZE = ParametersReader::get<int>("ARENA_SIZE");
    int NUMBER_OF_MODULES = ParametersReader::get<int>("NUMBER_OF_MODULES");
    
    std::string SHAPE_ENCODING = ParametersReader::get<std::string>("SHAPE_ENCODING");
    
    
    Node * platform;
    
    Receiver * receiver;
    Emitter * emitter;
    
    Position position = Position(0,5,0,true);
    
    Builder * builder;
    
    std::map<id_t, Module *> moduleMap;
    std::stack<id_t> availableModules;
    
    id_t nextOrganismId;
    
    
    
    void connectModulesToObjects();
    
    int buildOrganism(CppnGenome genome, std::string mindGenome, id_t forcedId);
    
    id_t getNextOrganismId();
    
    bool isClinicFree();
    
    int countInRadius(Position position, double radius);
    
	bool positionFree(Position position, double size);
    
	Position getFreeRandomPosition(double size);
        
    void readGenomeMessage(std::string message, std::string * genomeStr, std::string * mindStr, id_t * parent1, id_t * parent2, std::string * fitness1, std::string * fitness2);
    
    void readRebuildMessage(std::string message, id_t * organismId, std::string * genomeStr, std::string * mindStr);
    
    void addModuleToReserve(std::string moduleDef);
    
    void storePhilogenyOnFile(id_t parent1, id_t parent2, id_t newBorn, std::string fitness1, std::string fitness2);
    
    void storeGenomeOnFile(id_t organismId, std::string genomeStr);
    
    void storeMindGenomeOnFile(id_t organismId, std::string mindGenomeStr);
    
    void logModuleCount(std::string message, int modules);
    
    void rotate();
    
    void sendOrganismBuiltMessage(id_t parent1, id_t parent2, id_t organism, unsigned int size, std::string genome, std::string mind);

    public:
    
    BirthClinicController();
    
    ~BirthClinicController();
    
    void run();
    
};

#endif