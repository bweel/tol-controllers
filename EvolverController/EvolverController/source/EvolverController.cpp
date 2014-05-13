#include "EvolverController.h"


void EvolverController::generateInitialGenomes()
{
    for(id_t i = 0; i < INITIAL_POPULATION; i++)
    {
        CppnGenome newGenome = genomeManager->createGenome(std::vector<CppnGenome>());
        sendGenomeToBirthClinic(newGenome);
    }
}


void EvolverController::sendGenomeToBirthClinic(CppnGenome genome)
{
    emitter->setChannel(CLINIC_CHANNEL);
    std::string message = genomeManager->genomeToString(genome);
    emitter->send(message.c_str(), (int)message.length()+1);
}


void EvolverController::sendDeathMessage(id_t organimsId)
{
    emitter->setChannel(DEATH_CHANNEL);
    std::string message = std::to_string(organimsId);
    emitter->send(message.c_str(), (int)message.length()+1);
}


void EvolverController::readFitenessMessage(id_t * id, double * fitness, std::string * genome, std::string message)
{
    std::string organismStr = message.substr(message.find("NAME")+4, message.find("FITNESS")-4);
    std::string fitnessStr = message.substr(message.find("FITNESS")+7, message.find("GENOME")-(organismStr.length()+11));
    *genome = message.substr(message.find("GENOME")+6, message.length());
    
    if (fitnessStr.compare("nan") == 0)
        *fitness = 0.0;
    else
        *fitness = std::atof(fitnessStr.c_str());
    
    std::string idStr = organismStr.substr(organismStr.find("_")+1, organismStr.length());
    *id = std::atoi(idStr.c_str());
}


std::vector<id_t> EvolverController::selectForMating()
{
    std::vector<id_t> forMating = std::vector<id_t>();
    
    // first
    int bestPos = -1;
    double bestFitness = -1;
    for(int i = 0; i < organismsList.size(); i++)
    {
        if (std::get<1>(organismsList[i]) > bestFitness)
        {
            bestFitness = std::get<1>(organismsList[i]);
            bestPos = i;
        }
    }
    if(bestPos == -1)
    {
        return forMating;
    }
    forMating.push_back(std::get<0>(organismsList[bestPos]));
    
    // second
    std::tuple<id_t, double, CppnGenome> backupBestPair = organismsList[bestPos];
    organismsList.erase(organismsList.begin()+bestPos);
    bestPos = -1;
    bestFitness = -1;
    for(int i = 0; i < organismsList.size(); i++)
    {
        if (std::get<1>(organismsList[i]) > bestFitness)
        {
            bestFitness = std::get<1>(organismsList[i]);
            bestPos = i;
        }
    }
    organismsList.push_back(backupBestPair);
    if(bestPos == -1)
    {
        return forMating;
    }
    forMating.push_back(std::get<0>(organismsList[bestPos]));
    return forMating;
}


std::vector<id_t> EvolverController::selectForDying()
{
    std::vector<id_t> forDying;
    
    if (!organismsList.empty())
    {
        forDying.push_back(std::get<0>(organismsList[rand() % organismsList.size()]));
    }
    return forDying;
}


void EvolverController::removeFromOrganismsList(id_t organimsID)
{
    for(int i = 0; i < organismsList.size(); i++)
    {
        if (std::get<0>(organismsList[i]) == organimsID)
        {
            organismsList.erase(organismsList.begin() + i);
            return;
        }
    }
}


void EvolverController::addToOrganismsList(id_t organimsID, double fitness, CppnGenome genome)
{
    for(int i = 0; i < organismsList.size(); i++)
    {
        if (std::get<0>(organismsList[i]) == organimsID)
        {
            std::get<1>(organismsList[i]) = fitness;
            std::get<2>(organismsList[i]) = genome;
            return;
        }
    }
    organismsList.push_back(std::tuple<id_t, double, CppnGenome>(organimsID, fitness, genome));
}


int EvolverController::searchForOrganism(id_t organismId)
{
    for(int i = 0; i < organismsList.size(); i++)
    {
        if (std::get<0>(organismsList[i]) == organismId)
        {
            return i;
        }
    }
    return -1;
}


////////////////////////////////////////////////////////////////
//////////////////////// MAIN FUNCTIONS ////////////////////////
////////////////////////////////////////////////////////////////


EvolverController::EvolverController() : Supervisor()
{
    srand(static_cast<unsigned int>(time(NULL)));
    
    // setup shape encoding
    switch (SHAPE_ENCODING) {
        case CPPN:
            genomeManager = new CppnGenomeManager();
            break;
            
        default:
            break;
    }
    
    emitter = getEmitter(EMITTER_NAME);
    emitter->setChannel(CLINIC_CHANNEL);
    receiver = getReceiver(RECEIVER_NAME);
    receiver->setChannel(EVOLVER_CHANNEL);
}


EvolverController::~EvolverController()
{
    
}


void EvolverController::run()
{
    double timeStep = getBasicTimeStep();
    
    receiver->enable(timeStep);
    while (receiver->getQueueLength() > 0)
    {
        receiver->nextPacket();
    }
    
    // wait unitil EnvironmentModifierController sais the environment is ok
    bool environmentOk = false;
    while (step(timeStep) != -1 && !environmentOk)
    {
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            if (message.compare("ENVIRONMENT_OK") == 0)
            {
                environmentOk = true;
            }
            receiver->nextPacket();
        }
    }
    
    // generate an initial population of genomes and send them to the BirthClinicController
    generateInitialGenomes();
    
    
    // MAIN CYCLE
    unsigned int currentTime = 1;
    
    while (step(timeStep) != -1)
    {
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            
            id_t organismId;
            double fitness;
            std::string genomeStr;
            readFitenessMessage(& organismId, & fitness, &genomeStr, message);
            std::istringstream stream(genomeStr);
            CppnGenome genome = genomeManager->getGenomeFromStream(stream);
            
            std::cout << "new message received from organism_" << organismId << std::endl;
            
            addToOrganismsList(organismId, fitness, genome);
            
            receiver->nextPacket();
        }
        
        if(currentTime % MATING_TIME == 0)
        {
            std::cout << "time to mate" << std::endl;
            
            std::vector<id_t> forMating = selectForMating();
            
            if (forMating.size() == 2)
            {
                std::cout << "parents found" << std::endl;
                
                std::vector<CppnGenome> parentsGenomes = std::vector<CppnGenome>();
                parentsGenomes.push_back(std::get<2>(organismsList[searchForOrganism(forMating[0])]));
                parentsGenomes.push_back(std::get<2>(organismsList[searchForOrganism(forMating[1])]));
                CppnGenome newGenome = genomeManager->createGenome(parentsGenomes);
                
                std::cout << "NEW GENOME CREATED FROM organism_" << forMating[0] << "and organism_" << forMating[1] << std::endl;
                
                sendGenomeToBirthClinic(newGenome);
            }
        }
        
        if(currentTime % DYING_TIME == 0)
        {
            std::vector<id_t> forDying = selectForDying();
            for (int i = 0; i < forDying.size(); i++)
            {
                sendDeathMessage(forDying[i]);
                removeFromOrganismsList(forDying[i]);
                
                std::cout << "organism_" << forDying[i] << " SELECTED FOR DEATH" << std::endl;
            }
        }
        
        currentTime++;
    }
}