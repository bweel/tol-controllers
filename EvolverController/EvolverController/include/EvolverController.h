#ifndef EVOLVER_CONTROLLER_H
#define EVOLVER_CONTROLLER_H

#include "GenomeManager.h"
#include "CppnGenomeManager.h"

#include <webots/Supervisor.hpp>


using namespace webots;

class EvolverController : public Supervisor
{
    
private:
    
    std::string simulationDateAndTime;
    
    Receiver * receiver;
    Emitter * emitter;
    
    GenomeManager * genomeManager;
    
    std::vector<std::tuple<id_t, double, CppnGenome>> organismsList = std::vector<std::tuple<id_t, double, CppnGenome>>();

    
    
    
    void generateInitialGenomes();
    
    void readFitenessMessage(id_t * id, double * fitness, std::string * genome, std::string message);
    
    std::vector<id_t> selectForMating();
    
    std::vector<id_t> selectForDying();
    
    void sendGenomeToBirthClinic(CppnGenome genome, id_t parent1, id_t parent2);
    
    void sendDeathMessage(id_t organimsId);
    
    void removeFromOrganismsList(id_t organimsID);
    
    void addToOrganismsList(id_t organismID, double fitness, CppnGenome genome);
    
    int searchForOrganism(id_t organismId);
    
    
public:
    
    EvolverController();
    
    ~EvolverController();
    
    void run();
    
};

#endif