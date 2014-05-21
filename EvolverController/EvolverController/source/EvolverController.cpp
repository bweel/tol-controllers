#include "EvolverController.h"


void EvolverController::generateInitialGenomes()
{
    double timeStep = getBasicTimeStep();
    
    for(id_t i = 0; i < INITIAL_POPULATION; i++)
    {
        CppnGenome newGenome = genomeManager->createGenome(std::vector<CppnGenome>());
        sendGenomeToBirthClinic(newGenome, 0, 0);
        
        double startingTime = getTime();
        int noise = (rand() % 30) - 15;
        while (getTime() - startingTime < 60 + noise)
        {
            step(timeStep);
        }
    }
}


bool EvolverController::checkEvolutionEnd()
{
    for (id_t i = 1; i <= NUMBER_OF_MODULES; i++)
    {
        Node * root = getFromDef(MODULE_DEF_BASE_NAME + TO_STR(i));
        if (root)
        {
            Field * controller = root->getField("controller");
            if (controller->getSFString().compare("void") != 0)
            {
                return false;
            }
        }
    }
    return true;
}


void EvolverController::sendGenomeToBirthClinic(CppnGenome genome, id_t parent1, id_t parent2)
{
    emitter->setChannel(CLINIC_CHANNEL);
    
    std::string fitness1, fitness2;
    if (parent1 != 0)
        fitness1 = std::to_string(std::get<1>(organismsList[searchForOrganism(parent1)]));
    else
        fitness1 = "/";
    if (parent2 != 0)
        fitness2 = std::to_string(std::get<1>(organismsList[searchForOrganism(parent2)]));
    else
        fitness2 = "/";
    
    std::string message = "GENOME" + genomeManager->genomeToString(genome) + "PARENTS" + std::to_string(parent1) + "-" + std::to_string(parent2) + "PARENTS_FITNESS" + fitness1 + "-" + fitness2;
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


void EvolverController::storeEventOnFile(std::string log)
{
    ofstream organismsListsFile;
    organismsListsFile.open(RESULTS_PATH + simulationDateAndTime + "/evolver_organisms_list.txt", ios::app);
    organismsListsFile << log + "\n";
    organismsListsFile.close();
}



////////////////////////////////////////////////////////////////
//////////////////////// MAIN FUNCTIONS ////////////////////////
////////////////////////////////////////////////////////////////


EvolverController::EvolverController() : Supervisor()
{   
    srand(static_cast<unsigned int>(time(NULL)));
    
    // setup shape encoding
    if (SHAPE_ENCODING == "CPPN")
    {
        genomeManager = new CppnGenomeManager();
    }
    else
    {
        // set genomeManager for other kind of genome
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
    double lastMating = 0;
    //double lastDeath = 0;    // UNCOMMENT FOR CENTRALIZED DEATH BY THE EVOLVER
    double lastEvolutionEndCheck = 0;
    
    while (step(timeStep) != -1)
    {
        currentTime = getTime();
        
        if (currentTime - lastEvolutionEndCheck > CHECK_EVOLUTION_END_INTERVAL)
        {
            if (checkEvolutionEnd())
            {
                generateInitialGenomes();
            }
        }
        
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            
            if (message.substr(0,12).compare("SOMEONE_DIED") == 0)
            {
                id_t organimsID = std::atoi(message.substr(12,message.length()).c_str());
                removeFromOrganismsList(organimsID);
                
                std::cout << "organism_" << organimsID << " died: REMOVED FROM LIST" << std::endl;
                
                std::string log = std::to_string(getTime()) + " DEATH " + std::to_string(organimsID) + std::to_string(std::get<1>(organismsList[searchForOrganism(organimsID)])) + std::to_string(organismsList.size());
                storeEventOnFile(log);
                
                receiver->nextPacket();
            }
            else
            {
                id_t organismId;
                double fitness;
                std::string genomeStr;
                readFitenessMessage(& organismId, & fitness, &genomeStr, message);
                std::istringstream stream(genomeStr);
                CppnGenome genome = genomeManager->getGenomeFromStream(stream);
                
                std::cout << "new message received from organism_" << organismId << std::endl;
                
                addToOrganismsList(organismId, fitness, genome);
                
                std::string log = std::to_string(getTime()) + " MESSAGE_FROM " + std::to_string(organismId) + std::to_string(std::get<1>(organismsList[searchForOrganism(organismId)])) + std::to_string(organismsList.size());
                storeEventOnFile(log);
                
                receiver->nextPacket();
            }
        }
        
        if(currentTime - lastMating > MATING_TIME)
        {
            std::cout << "time to mate" << std::endl;
            
            std::vector<id_t> forMating = selectForMating();
            
            if (forMating.size() == 2)
            {
                std::vector<CppnGenome> parentsGenomes = std::vector<CppnGenome>();
                parentsGenomes.push_back(std::get<2>(organismsList[searchForOrganism(forMating[0])]));
                parentsGenomes.push_back(std::get<2>(organismsList[searchForOrganism(forMating[1])]));
                CppnGenome newGenome = genomeManager->createGenome(parentsGenomes);
                
                std::cout << "NEW GENOME CREATED FROM organism_" << forMating[0] << " and organism_" << forMating[1] << std::endl;
                
                std::string log = std::to_string(getTime()) + " NEW GENOME CREATED FROM " + std::to_string(forMating[0]) + " and " + std::to_string(forMating[1]);
                storeEventOnFile(log);
                
                sendGenomeToBirthClinic(newGenome, forMating[0], forMating[1]);
            }
            if (forMating.size() == 1)
            {
                std::vector<CppnGenome> parentsGenomes = std::vector<CppnGenome>();
                parentsGenomes.push_back(std::get<2>(organismsList[searchForOrganism(forMating[0])]));
                CppnGenome newGenome = genomeManager->createGenome(parentsGenomes);
                
                std::cout << "NEW GENOME CREATED FROM SINGLE PARENT organism_" << forMating[0] << std::endl;
                
                std::string log = std::to_string(getTime()) + " NEW GENOME CREATED FROM " + std::to_string(forMating[0]);
                storeEventOnFile(log);
                
                sendGenomeToBirthClinic(newGenome, forMating[0], 0);
            }
            if (forMating.size() == 0)
            {
                std::cout << "couple of parents not found" << std::endl;
                
                std::string log = std::to_string(getTime()) + " couple of parents not found";
                storeEventOnFile(log);
            }
            
            lastMating = getTime();
        }
        
        /****************** UNCOMMENT FOR CENTRALIZED DEATH BY THE EVOLVER ******************
        
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
        ************************************************************************************/
    }
}