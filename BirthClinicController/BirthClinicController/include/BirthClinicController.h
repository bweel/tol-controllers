#ifndef BIRTH_CLINIC_H
#define BIRTH_CLINIC_H

#include "Module.h"
#include "Builder.h"
#include "Organism.h"
#include "RoombotModule.h"

#include <webots/Supervisor.hpp>
#include <stack>
#include <limits>

using namespace webots;

class BirthClinicController : public Supervisor
{

    private:
    
    std::string simulationDateAndTime;
    
    Node * platform;
    
    Receiver * receiver;
    Emitter * emitter;
    
    Position position = Position(0,5,0,true);
    
    Builder * builder;
    
    std::map<id_t, Module *> moduleMap;
    std::stack<id_t> availableModules;
    
    id_t nextOrganismId;
    
    
    
    void connectModulesToObjects();
    
    int buildOrganism(CppnGenome genome);
    
    id_t getNextOrganismId();
    
    int countInRadius(Position position, double radius);
    
	bool positionFree(Position position, double size);
    
	Position getFreeRandomPosition(double size);
    
    void sendStartMessage(int channel);
    
    std::string readUpdateAvailableMessage(std::string message);
    
    void readGenomeMessage(std::string message, std::string * genomeStr, id_t * parent1, id_t * parent2);
    
    void addModuleToReserve(std::string moduleDef);
    
    void storePhilogenyOnFile(id_t parent1, id_t parent2, id_t newBorn);
    
    void rotate();
    


    public:
    
    BirthClinicController();
    
    ~BirthClinicController();
    
    void run();
    
};

#endif