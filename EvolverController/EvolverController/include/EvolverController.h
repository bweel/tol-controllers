#ifndef EVOLVER_CONTROLLER_H
#define EVOLVER_CONTROLLER_H

#include "GenomeManager.h"
#include "CppnGenomeManager.h"
#include "ParametersReader.h"
#include "MessagesManager.h"
#include "MatrixGenomeManager.h"
#include "Organism.h"
#include "ParentSelectionMechanism.h"

#include <webots/Supervisor.hpp>


using namespace webots;

class EvolverController : public Supervisor
{
    
private:
    
    std::string simulationDateAndTime;
    
    int CLINIC_CHANNEL = ParametersReader::get<int>("CLINIC_CHANNEL");
    int EVOLVER_CHANNEL = ParametersReader::get<int>("EVOLVER_CHANNEL");
    int DEATH_CHANNEL = ParametersReader::get<int>("DEATH_CHANNEL");
    
    int NUMBER_OF_MODULES = ParametersReader::get<int>("NUMBER_OF_MODULES");
    int INITIAL_POPULATION = ParametersReader::get<int>("INITIAL_POPULATION");
    
    int MATING_TIME = ParametersReader::get<int>("MATING_TIME");
    int DYING_TIME = ParametersReader::get<int>("DYING_TIME");
    int CHECK_EVOLUTION_END_INTERVAL = ParametersReader::get<int>("CHECK_EVOLUTION_END_INTERVAL");
    
    int TIME_STEP = ParametersReader::get<int>("TIME_STEP");
    
    std::string SHAPE_ENCODING = ParametersReader::get<std::string>("SHAPE_ENCODING");
    std::string MIND_ENCODING = ParametersReader::get<std::string>("MIND_ENCODING");
    std::string PARENT_SELECTION = ParametersReader::get<std::string>("PARENT_SELECTION");
    
    Receiver * receiver; 
    Emitter * emitter;
    
    GenomeManager * genomeManager;
    MindGenomeManager * mindGenomeManager;
    ParentSelectionMechanism * parentSelectionMechanism;
    
    std::vector<Organism> organismsList;

    
    void generateInitialGenomes();
    
    std::vector<id_t> selectForMating();
    
    std::vector<id_t> selectForDying();
    
    bool checkEvolutionEnd();
    
    void readFitnessMessage(id_t * id, double * fitness, std::string * genome, std::string * mind, std::string message);
    
    void readCoupleMessage(std::string message, id_t * id1, double * fitness1, std::string * genome1, std::string * mind1, id_t * id2, double * fitness2, std::string * genome2, std::string * mind2);
    
    void sendGenomeToBirthClinic(std::string genome, std::string newMind, id_t parent1, id_t parent2, double fitness1, double fitness2);
    
    void sendDeathMessage(id_t organimsId);
    
    void removeFromOrganismsList(id_t organimsID);
    
    void addToOrganismsList(id_t organimsID, double fitness, std::string genome, std::string mindGenome);
    
    int searchForOrganism(id_t organismId);
    
    void storeEventOnFile(std::string log);
    
    
    
public:
    
    EvolverController();
    
    ~EvolverController();
    
    void run();
    
};

#endif