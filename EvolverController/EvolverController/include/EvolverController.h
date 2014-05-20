#ifndef EVOLVER_CONTROLLER_H
#define EVOLVER_CONTROLLER_H

#include "GenomeManager.h"
#include "CppnGenomeManager.h"
#include "MatrixGenomeManager.h"
#include "Organism.h"

#include <webots/Supervisor.hpp>


using namespace webots;

class EvolverController : public Supervisor
{
    
private:
    
    std::string simulationDateAndTime;
    
    Receiver * receiver; 
    Emitter * emitter;
    
    double lastMating;
    double lastDeath;
    
    GenomeManager * genomeManager;
    MindGenomeManager * mindGenomeManager;
    
    std::vector<Organism> organismsList;

    
    void generateInitialGenomes();
    
    void readFitnessMessage(id_t * id, double * fitness, std::string * genome, std::string * mind, std::string message);
    
    std::vector<id_t> selectForMating();
    
    std::vector<id_t> selectForDying();
    
    void sendGenomeToBirthClinic(std::string genome, std::string newMind, id_t parent1, id_t parent2);
    
    void sendDeathMessage(id_t organimsId);
    
    void removeFromOrganismsList(id_t organimsID);
    
    void addToOrganismsList(id_t organimsID, double fitness, std::string genome, std::string mindGenome);
    
    int searchForOrganism(id_t organismId);
    
    
public:
    
    EvolverController();
    
    ~EvolverController();
    
    void run();
    
};

#endif