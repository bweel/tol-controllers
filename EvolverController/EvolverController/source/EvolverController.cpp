#include <utility>

#include "EvolverController.h"
#include "BestTwoParentSelection.h"
#include "BinaryTournamentParentSelection.h"



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


void EvolverController::sendGenomeToBirthClinic(std::string genome, std::string newMind, id_t parent1, id_t parent2, double fitness1, double fitness2)
{
    emitter->setChannel(CLINIC_CHANNEL);
    
    std::string fitness1Str, fitness2Str;
    if (parent1 != 0)
        fitness1Str = std::to_string(fitness1);
    else
        fitness1Str = "/";
    if (parent2 != 0)
        fitness2Str = std::to_string(fitness2);
    else
        fitness2Str = "/";
    
    std::string message = "[GENOME_TO_CLINIC_MESSAGE]";
    message = MessagesManager::add(message, "GENOME", genome);
    message = MessagesManager::add(message, "MIND", newMind);
    message = MessagesManager::add(message, "PARENTS", std::to_string(parent1) + "-" + std::to_string(parent2));
    message = MessagesManager::add(message, "PARENTS_FITNESS", fitness1Str + "-" + fitness2Str);
    
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
    *id = std::atoi(MessagesManager::get(message, "ID").c_str());
    std::string fitnessStr = MessagesManager::get(message, "FITNESS");
    if (fitnessStr.compare("nan") == 0)
        *fitness = 0.0;
    else
        *fitness = std::atof(fitnessStr.c_str());
    *genome = MessagesManager::get(message, "GENOME");
    *mind = MessagesManager::get(message, "MIND");
}


void EvolverController::readCoupleMessage(std::string message, id_t * id1, double * fitness1, std::string * genome1, std::string * mind1, id_t * id2, double * fitness2, std::string * genome2, std::string * mind2)
{
    * id1 = std::atoi(MessagesManager::get(message, "ID1").c_str());
    * fitness1 = std::atof(MessagesManager::get(message, "FITNESS1").c_str());
    * genome1 = MessagesManager::get(message, "GENOME1");
    * mind1 = MessagesManager::get(message, "MIND1");
    * id2 = std::atoi(MessagesManager::get(message, "ID2").c_str());
    * fitness2 = std::atof(MessagesManager::get(message, "FITNESS2").c_str());
    * genome2 = MessagesManager::get(message, "GENOME2");
    * mind2 = MessagesManager::get(message, "MIND2");
}


std::vector<id_t> EvolverController::selectForMating()
{
    std::vector<Organism> candidates = std::vector<Organism>();
    for(int i=0;i<organismsList.size();i++){
        if(organismsList[i].getState()==Organism::ADULT){
            candidates.push_back(organismsList[i]);
        }
    }
    return parentSelectionMechanism->selectParents(candidates);
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


/*void EvolverController::updateOrganismsList(id_t organismID, double fitness, std::string genome, std::string mindGenome,
                                           unsigned int size, unsigned int offspring, std::vector<id_t> parents, Organism::State state)
{
    for(int i = 0; i < organismsList.size(); i++)
    {
        if (organismsList[i].getId() == organismID)
        {
            organismsList[i].setFitness(fitness);
            organismsList[i].setGenome(genome); // should never change
            organismsList[i].setSize(size); // should never change
            organismsList[i].setOffspring(offspring);
            organismsList[i].setParents(parents); // should never change
            organismsList[i].setSize(state);
            return;
        }
    }
    Organism organism(genome, mindGenome, organismID, fitness, size, offspring, parents, state);
    organismsList.push_back(organism);
}*/


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


void EvolverController::storeEventOnFile(std::string log)
{
    ofstream organismsListsFile;
    organismsListsFile.open(RESULTS_PATH + simulationDateAndTime + "/evolver_organisms_list.txt", ios::app);
    organismsListsFile << log << std::endl;
    organismsListsFile.close();
}


void EvolverController::storeParentsOnFile(double currentTime)
{
    ofstream parentsFile;
    parentsFile.open(RESULTS_PATH + simulationDateAndTime + "/parents.txt", ios::app);
    for (int i = 0; i < organismsList.size(); i++)
    {
        Organism org = organismsList[i];
        parentsFile << currentTime << " " << org.getId() << " " << org.getSize() << " " << org.getFitness() << " " << org.getOffspring() << std::endl;
    }
    parentsFile.close();
}


int EvolverController::getRandomWait()
{
    int noise = (rand() % NOISE_GENOMES_INITIALIZATION) - (NOISE_GENOMES_INITIALIZATION/2);
    return WAITING_INTERVAL_GENOMES_INITIALIZATION + noise;
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
        std::cerr << "Unknown Shape Encoding: " << SHAPE_ENCODING << std::endl;
    }
    
    if (MIND_ENCODING == "RLPOWER") {
        mindGenomeManager = new MatrixGenomeManager();
    }
    else
    {
        std::cerr << "Unknown Mind Encoding: " << MIND_ENCODING << std::endl;
    }
    
    if(PARENT_SELECTION == "BESTTWO") {
        parentSelectionMechanism = new BestTwoParentSelection();
    } else if(PARENT_SELECTION == "BINARY_TOURNAMENT"){
        parentSelectionMechanism = new BinaryTournamentParentSelection();
    } else {
        std::cerr << "Unknown Parent Selection Mechanism: " << MIND_ENCODING << std::endl;
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
    receiver->enable(TIME_STEP);
    while (receiver->getQueueLength() > 0)
    {
        receiver->nextPacket();
    }
    
    // wait unitil EnvironmentModifierController sais the environment is ok
    bool environmentOk = false;
    while (step(TIME_STEP) != -1 && !environmentOk)
    {
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            if (message.substr(0,24).compare("[ENVIRONMENT_OK_MESSAGE]") == 0)
            {
                environmentOk = true;
                simulationDateAndTime = MessagesManager::get(message, "SDAT");
            }
            receiver->nextPacket();
        }
    }
    
    // create a first random genome
    CppnGenome newGenome = genomeManager->createGenome(std::vector<CppnGenome>());
    sendGenomeToBirthClinic(genomeManager->genomeToString(newGenome), "", 0, 0, 0, 0);
    
    
    // MAIN CYCLE
    
    int generated = 1;
    int initPopulationWaitingTime = getRandomWait();
    double lastGeneratedTime = 0;
    
    double currentTime = 0;
    double lastMating = 0;    // UNCOMMENT FOR CENTRALIZED REPRODUCTION BY THE EVOLVER
    //double lastDeath = 0;     // UNCOMMENT FOR CENTRALIZED DEATH BY THE EVOLVER
    double lastEvolutionEndCheck = 0;
    double lastOffspringLoggingTime = 30;
    
    while (step(TIME_STEP) != -1)
    {
        
        currentTime = getTime();
        
        
        /***********************************
         ******* CHECK END EVOLUTION *******
         ***********************************/
        
        if (currentTime - lastEvolutionEndCheck > CHECK_EVOLUTION_END_INTERVAL)
        {
            if (checkEvolutionEnd())
            {
                generated = 0;
                initPopulationWaitingTime = 0;
            }
            lastEvolutionEndCheck = getTime();
        }
        
        
        /***********************************
         ******* CHECK END EVOLUTION *******
         ***********************************/
        
        if (generated < INITIAL_POPULATION and currentTime - lastGeneratedTime > initPopulationWaitingTime)
        {
            CppnGenome newGenome = genomeManager->createGenome(std::vector<CppnGenome>());
            sendGenomeToBirthClinic(genomeManager->genomeToString(newGenome), "", 0, 0, 0, 0);
            generated++;
            lastGeneratedTime = getTime();
            initPopulationWaitingTime = getRandomWait();
        }
        
        
        /***********************************
         ********* MANAGE MESSAGES *********
         ***********************************/
        
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            
            if (message.substr(0,28).compare("[DEATH_ANNOUNCEMENT_MESSAGE]") == 0)
            {
                id_t organimsID = std::atoi(MessagesManager::get(message, "ID").c_str());
//                removeFromOrganismsList(organimsID);
                int index = searchForOrganism(organimsID);
                if(index >= 0){
                    organismsList[searchForOrganism(organimsID)].setState(Organism::DEAD);
                
                    std::cout << "organism_" << organimsID << " died: REMOVED FROM LIST" << std::endl;
                
                    std::string log = std::to_string(getTime()) + " DEATH " + std::to_string(organimsID) + " organismsListSize " + std::to_string(organismsList.size());
                    storeEventOnFile(log);
                }
            }
            
            if (message.substr(0,24).compare("[ORGANISM_BUILT_MESSAGE]") == 0)
            {
                id_t parent1 = std::atoi(MessagesManager::get(message, "PARENT1").c_str());
                id_t parent2 = std::atoi(MessagesManager::get(message, "PARENT2").c_str());
                id_t organismId = std::atoi(MessagesManager::get(message, "ORGANISM_ID").c_str());
                unsigned int size = std::atoi(MessagesManager::get(message, "SIZE").c_str());
                std::string genome = MessagesManager::get(message, "GENOME");
                std::string mind = MessagesManager::get(message, "MIND");
                
                if(parent1 > 0){
                    int index = searchForOrganism(parent1);
                    assert(index >= 0);
                    organismsList[index].setOffspring(organismsList[index].getOffspring()+1);
                }
                
                if(parent2 > 0){
                    int index = searchForOrganism(parent2);
                    assert(index >= 0);
                    organismsList[index].setOffspring(organismsList[index].getOffspring()+1);
                }
                
                std::vector<id_t> parents;
                parents.push_back(parent1);
                parents.push_back(parent2);
                
                Organism newOrganism = Organism(genome, mind, organismId, 0, size, 0, parents, Organism::INFANT);
                int idx = searchForOrganism(organismId);
                if (idx >= 0)
                {
                    organismsList.erase(organismsList.begin()+idx);
                }
                organismsList.push_back(newOrganism);
                
               
                std::string log = std::to_string(getTime()) + " PROCREATE " + std::to_string(parent1) + " and "  + std::to_string(parent2) + " succesfully had a child";
                storeEventOnFile(log);
            }
            
            /****************************************************** DISTRIBUTED REPRODUCTION ******************************************************/
            
            /*if (message.substr(0,16).compare("[COUPLE_MESSAGE]") == 0)
            {
                // read message with couple
                id_t id1, id2;
                double fitness1, fitness2;
                std::string genome1, genome2;
                std::string mind1, mind2;
                readCoupleMessage(message, & id1, & fitness1, & genome1, & mind1, & id2, & fitness2, & genome2, & mind2);
                
                // recombine genomes
                std::vector<CppnGenome> parentsGenomes = std::vector<CppnGenome>();
                std::stringstream genomeAsStream1(genome1);
                std::stringstream genomeAsStream2(genome2);
                parentsGenomes.push_back(CppnGenome(genomeAsStream1));
                parentsGenomes.push_back(CppnGenome(genomeAsStream2));
                CppnGenome newGenome = genomeManager->createGenome(parentsGenomes);
                
                // recombine minds
                std::vector<boost::shared_ptr<MindGenome> > parentMindGenomes;
                std::stringstream mindAsStream1(mind1);
                std::stringstream mindAsStream2(mind2);
                parentMindGenomes.push_back(mindGenomeManager->getGenomeFromStream(mindAsStream1));
                parentMindGenomes.push_back(mindGenomeManager->getGenomeFromStream(mindAsStream2));
                boost::shared_ptr<MindGenome> newMind = mindGenomeManager->createGenome(parentMindGenomes);
                
                std::cout << "NEW GENOME CREATED FROM organism_" << id1 << " and organism_" << id2 << std::endl;
                
                // store event into file
                std::string log = std::to_string(getTime()) + " NEW GENOME CREATED FROM " + std::to_string(id1) + " and " + std::to_string(id2);
                storeEventOnFile(log);
                
                sendGenomeToBirthClinic(genomeManager->genomeToString(newGenome), newMind->toString(), id1, id2, fitness1, fitness2);
            }*/
            
            /*************************************************************************************************************************************/
            
            /****************************************************** CENTRALIZED REPRODUCTION ******************************************************/
             
            if (message.substr(0,23).compare("[GENOME_SPREAD_MESSAGE]") == 0)
            {
                id_t organismId;
                double fitness;
                std::string genomeStr;
                std::string mindStr;
                readFitnessMessage(& organismId, & fitness, &genomeStr, &mindStr, message);
             
                // update fitness and state
                organismsList[searchForOrganism(organismId)].setFitness(fitness);
                organismsList[searchForOrganism(organismId)].setState(Organism::ADULT); // useful only for the first message time
                organismsList[searchForOrganism(organismId)].setMind(mindStr);          // useful only for the first message from organisms not created from parents
                
                std::string log = std::to_string(getTime()) + " MESSAGE_FROM " + std::to_string(organismId)  + " organismsListSize " + std::to_string(organismsList.size());
                storeEventOnFile(log);
            }
            
            /*************************************************************************************************************************************/
            
            receiver->nextPacket();
        }
        
        
        /***********************************
         ******* INTERVAL OPERATIONS *******
         ***********************************/
        
        /****************************************************** CENTRALIZED REPRODUCTION ******************************************************/
        
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
                std::stringstream mind2(organismsList[searchForOrganism(forMating[1])].getMind());
                
                boost::shared_ptr<MindGenome> mindGenome1 = mindGenomeManager->getGenomeFromStream(mind1);
                boost::shared_ptr<MindGenome> mindGenome2 = mindGenomeManager->getGenomeFromStream(mind2);
                
                parentMindGenomes.push_back(mindGenome1);
                parentMindGenomes.push_back(mindGenome2);
                boost::shared_ptr<MindGenome> newMind = mindGenomeManager->createGenome(parentMindGenomes);
                
                std::cout << "NEW GENOME CREATED FROM organism_" << forMating[0] << " and organism_" << forMating[1] << std::endl;
                
                std::string log = std::to_string(getTime()) + " NEW GENOME CREATED FROM " + std::to_string(forMating[0]) + " and " + std::to_string(forMating[1]);
                storeEventOnFile(log);
                
                double fitness1 = organismsList[searchForOrganism(forMating[0])].getFitness();
                double fitness2 = organismsList[searchForOrganism(forMating[1])].getFitness();
                sendGenomeToBirthClinic(genomeManager->genomeToString(newGenome), newMind->toString(), forMating[0], forMating[1], fitness1, fitness2);
            }
            if (forMating.size() == 1)
            {
                std::vector<CppnGenome> parentsGenomes = std::vector<CppnGenome>();
                std::stringstream stream1(organismsList[searchForOrganism(forMating[0])].getGenome());
                
                parentsGenomes.push_back(CppnGenome(stream1));
                
                CppnGenome newGenome = genomeManager->createGenome(parentsGenomes);
                
                std::vector<boost::shared_ptr<MindGenome> > parentMindGenomes;
                std::stringstream mind1(organismsList[searchForOrganism(forMating[0])].getMind());

                boost::shared_ptr<MindGenome> mindGenome1 = mindGenomeManager->getGenomeFromStream(mind1);
                
                parentMindGenomes.push_back(mindGenome1);
                boost::shared_ptr<MindGenome> newMind = mindGenomeManager->createGenome(parentMindGenomes);
                
                std::cout << "NEW GENOME CREATED FROM SINGLE PARENT organism_" << forMating[0] << std::endl;
                
                std::string log = std::to_string(getTime()) + " NEW GENOME CREATED FROM " + std::to_string(forMating[0]);
                storeEventOnFile(log);
                
                double fitness = organismsList[searchForOrganism(forMating[0])].getFitness();
                sendGenomeToBirthClinic(genomeManager->genomeToString(newGenome), newMind->toString(), forMating[0], 0, fitness, -1);
            }
            if (forMating.size() == 0)
            {
                std::cout << "couple of parents not found" << std::endl;
                
                std::string log = std::to_string(getTime()) + " couple of parents not found";
                storeEventOnFile(log);
            }
            
            lastMating = getTime();
        }
        
        if(currentTime - lastOffspringLoggingTime > MATING_TIME)
        {
            // LOG EVERY ADULT ORGANISM
            storeParentsOnFile(currentTime);
            lastOffspringLoggingTime = getTime();
        }
        
        /*************************************************************************************************************************************/
        
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