#include <utility>

#include "EvolverController.h"


void EvolverController::generateInitialGenomes()
{
    for(id_t i = 0; i < INITIAL_POPULATION; i++)
    {
        CppnGenome newGenome = genomeManager->createGenome(std::vector<CppnGenome>());
        sendGenomeToBirthClinic(genomeManager->genomeToString(newGenome), "", 0, 0);
    }
}


void EvolverController::sendGenomeToBirthClinic(std::string genome, std::string newMind, id_t parent1, id_t parent2)
{
    emitter->setChannel(CLINIC_CHANNEL);
    std::string message = "GENOME" + genome + "MIND" + newMind + "PARENTS" + std::to_string(parent1) + "-" + std::to_string(parent2);
    emitter->send(message.c_str(), (int)message.length()+1);
}


void EvolverController::sendDeathMessage(id_t organimsId)
{
    emitter->setChannel(DEATH_CHANNEL);
    std::string message = std::to_string(organimsId);
    emitter->send(message.c_str(), (int)message.length()+1);
}


void EvolverController::readFitnessMessage(id_t * id, double * fitness, std::string * genome, std::string * mind, std::string message)
{
    std::string organismStr = message.substr(message.find("NAME")+4, message.find("FITNESS")-4);
    std::string fitnessStr = message.substr(message.find("FITNESS")+7, message.find("MIND")-(organismStr.length()+11));
    *mind = message.substr(message.find("MIND")+4, message.find("GENOME")-(organismStr.length()+fitnessStr.length()+11));
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
        if (organismsList[i].getFitness() > bestFitness)
        {
            bestFitness = organismsList[i].getFitness();
            bestPos = i;
        }
    }
    if(bestPos == -1)
    {
        return forMating;
    }
    forMating.push_back(organismsList[bestPos].getId());
    
    // second
    Organism backupBestPair = organismsList[bestPos];
    organismsList.erase(organismsList.begin()+bestPos);
    bestPos = -1;
    bestFitness = -1;
    for(int i = 0; i < organismsList.size(); i++)
    {
        if (organismsList[i].getFitness() > bestFitness)
        {
            bestFitness = organismsList[i].getFitness();
            bestPos = i;
        }
    }
    organismsList.push_back(backupBestPair);
    if(bestPos == -1)
    {
        return forMating;
    }
    forMating.push_back(organismsList[bestPos].getId());
    return forMating;
}


std::vector<id_t> EvolverController::selectForDying()
{
    std::vector<id_t> forDying;
    
    if (!organismsList.empty())
    {
        forDying.push_back(organismsList[rand() % organismsList.size()].getId());
    }
    return forDying;
}


void EvolverController::removeFromOrganismsList(id_t organimsID)
{
    for(int i = 0; i < organismsList.size(); i++)
    {
        if (organismsList[i].getId() == organimsID)
        {
            organismsList.erase(organismsList.begin() + i);
            return;
        }
    }
}


void EvolverController::addToOrganismsList(id_t organismID, double fitness, std::string genome, std::string mindGenome)
{
    for(int i = 0; i < organismsList.size(); i++)
    {
        if (organismsList[i].getId() == organismID)
        {
            organismsList[i].setFitness(fitness);
            organismsList[i].setGenome(genome);
            organismsList[i].setMind(mindGenome);
            return;
        }
    }
    Organism organism(genome, mindGenome, organismID, fitness);
    organismsList.push_back(organism);
}


int EvolverController::searchForOrganism(id_t organismId)
{
    for(int i = 0; i < organismsList.size(); i++)
    {
        if (organismsList[i].getId() == organismId)
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

    switch (MIND_ENCODING) {
        case A_POWER:
            mindGenomeManager = new MatrixGenomeManager();
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
            if (message.substr(0,14).compare("ENVIRONMENT_OK") == 0)
            {
                environmentOk = true;
                simulationDateAndTime = message.substr(14,message.length());
            }
            receiver->nextPacket();
        }
    }
    
    // generate an initial population of genomes and send them to the BirthClinicController
    generateInitialGenomes();
    
    
    // MAIN CYCLE
    double currentTime = 0;
    lastMating = 0;
    lastDeath = 0;
    
    while (step(timeStep) != -1)
    {
        currentTime = getTime();
        
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            
            id_t organismId;
            double fitness;
            std::string genomeStr;
            std::string mindStr;
            readFitnessMessage(& organismId, & fitness, &genomeStr, &mindStr, message);

//            std::istringstream stream(genomeStr);
//            CppnGenome genome = genomeManager->getGenomeFromStream(stream);
            
//            std::istringstream mindStream(mindStr);
//            boost::shared_ptr<Mind::MindGenome> mind = mindGenomeManager->getGenomeFromStream(mindStream);
            
            std::cout << "new message received from organism_" << organismId << std::endl;
            
            addToOrganismsList(organismId, fitness, genomeStr, mindStr);
            
            receiver->nextPacket();
        }
        
        if(currentTime - lastMating > MATING_TIME)
        {
            std::cout << "time to mate" << std::endl;
            
            std::vector<id_t> forMating = selectForMating();
            
            if (forMating.size() == 2)
            {
                std::vector<CppnGenome> parentsGenomes = std::vector<CppnGenome>();
                std::stringstream stream1(organismsList[searchForOrganism(forMating[0])].getGenome());
                std::stringstream stream2(organismsList[searchForOrganism(forMating[1])].getGenome());
                
                parentsGenomes.push_back(CppnGenome(stream1));
                parentsGenomes.push_back(CppnGenome(stream2));
                
                CppnGenome newGenome = genomeManager->createGenome(parentsGenomes);
                
                std::vector<boost::shared_ptr<MindGenome> > parentMindGenomes;
                std::stringstream mind1(organismsList[searchForOrganism(forMating[0])].getMind());
                std::stringstream mind2(organismsList[searchForOrganism(forMating[0])].getMind());
                
                boost::shared_ptr<MindGenome> mindGenome1 = mindGenomeManager->getGenomeFromStream(mind1);
                boost::shared_ptr<MindGenome> mindGenome2 = mindGenomeManager->getGenomeFromStream(mind2);
                
                parentMindGenomes.push_back(mindGenome1);
                parentMindGenomes.push_back(mindGenome2);
                boost::shared_ptr<MindGenome> newMind = mindGenomeManager->createGenome(parentMindGenomes);
                
                
                std::cout << "NEW GENOME CREATED FROM organism_" << forMating[0] << " and organism_" << forMating[1] << std::endl;
                
                sendGenomeToBirthClinic(genomeManager->genomeToString(newGenome), newMind->toString(), forMating[0], forMating[1]);
            }
            else
            {
                std::cout << "couple of parents not found" << std::endl;
            }
            
            lastMating = getTime();
        }
        
        if(currentTime - lastDeath > DYING_TIME)
        {
            std::vector<id_t> forDying = selectForDying();
            
            for (int i = 0; i < forDying.size(); i++)
            {
                sendDeathMessage(forDying[i]);
                removeFromOrganismsList(forDying[i]);
                
                std::cout << "organism_" << forDying[i] << " SELECTED FOR DEATH" << std::endl;
            }
            
            lastDeath = getTime();
        }
    }
}