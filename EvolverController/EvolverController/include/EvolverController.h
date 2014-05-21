#ifndef EVOLVER_CONTROLLER_H
#define EVOLVER_CONTROLLER_H

#include "GenomeManager.h"
#include "CppnGenomeManager.h"
#include "ParametersReader.h"

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
    
    std::string SHAPE_ENCODING = ParametersReader::get<std::string>("SHAPE_ENCODING");
    
    
    Receiver * receiver; 
    Emitter * emitter;
    
    GenomeManager * genomeManager;
    
    std::vector<std::tuple<id_t, double, CppnGenome>> organismsList = std::vector<std::tuple<id_t, double, CppnGenome>>();

    
    
    
    void generateInitialGenomes();
    
    std::vector<id_t> selectForMating();
    
    std::vector<id_t> selectForDying();
    
    bool checkEvolutionEnd();
    
    void readFitenessMessage(id_t * id, double * fitness, std::string * genome, std::string message);
    
    void sendGenomeToBirthClinic(CppnGenome genome, id_t parent1, id_t parent2);
    
    void sendDeathMessage(id_t organimsId);
    
    void removeFromOrganismsList(id_t organimsID);
    
    void addToOrganismsList(id_t organismID, double fitness, CppnGenome genome);
    
    int searchForOrganism(id_t organismId);
    
    void storeEventOnFile(std::string log);
    
    
    
public:
    
    EvolverController();
    
    ~EvolverController();
    
    void run();
    
};

#endif