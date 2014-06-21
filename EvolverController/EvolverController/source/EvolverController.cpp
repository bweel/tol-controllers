#include <utility>

#include "EvolverController.h"
#include "BestTwoParentSelection.h"
#include "BinaryTournamentParentSelection.h"
#include "RandomSelection.h"



bool EvolverController::checkEvolutionEnd()
{
    for (id_t i = 1; i <= NUMBER_OF_MODULES; i++)
    {
        Node * root = getFromDef(MODULE_DEF_BASE_NAME + TO_STR(i));
        if (root)
        {
            Field * controller = root->getField("controller");
            if (controller->getSFString().compare("void") != 0 && controller->getSFString().compare("DeathController"))
            {
                return false;
            }
        }
    }
    return true;
}


CppnGenome EvolverController::createRandomGenome()
{
    for (int i = 0; i < 100; i++)
    {
        CppnGenome newGenome = genomeManager->createGenome(std::vector<CppnGenome>());
        bool empty = checkEmptyPlan(newGenome);
        if (!empty)
        {
            return newGenome;
        }
    }
    return genomeManager->createGenome(std::vector<CppnGenome>());;
}


bool EvolverController::checkEmptyPlan(CppnGenome genome)
{
    std::auto_ptr<BuildPlan> buildPlan = builder->translateGenome(genome);
    if (buildPlan->size() > 1)
    {
        return false;
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

void EvolverController::checkEndEvolution(double currentTime) {
    if (currentTime - lastEvolutionEndCheck > CHECK_EVOLUTION_END_INTERVAL)
    {
        if (checkEvolutionEnd())
        {
            // set all prevoius organisms to DEAD state (should not be necessary though)
            for (int i = 0; i < organismsList.size(); i++)
            {
                organismsList[i].setState(Organism::DEAD);
            }
            initialization = true;          // restart initialization procedure
            initPopulationWaitingTime = 0;  // immediately
        }
        lastEvolutionEndCheck = getTime();
    }
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
    boost::filesystem::path filePath = RESULTS_PATH + simulationDateAndTime + "/parents.txt";
    if(!boost::filesystem::exists(filePath)){
        parentsFile.open(filePath.string(), ios::app);
        parentsFile << "#time id size fitness offspring state" << std::endl;
    }else{
        parentsFile.open(filePath.string(), ios::app);
    }
    for (int i = 0; i < organismsList.size(); i++)
    {
        Organism org = organismsList[i];
        if (org.getState() == Organism::ADULT)
        {
            parentsFile << currentTime << " " << org.getId() << " " << org.getSize() << " " << org.getFitness() << " " << org.getOffspring() << " " << org.getState() << std::endl;
        }
    }
    parentsFile.close();
}

void EvolverController::storePopulationSizesOnFile(double currentTime)
{
    ofstream parentsFile;
    boost::filesystem::path filePath = RESULTS_PATH + simulationDateAndTime + "/populations.txt";
    if(!boost::filesystem::exists(filePath)){
        parentsFile.open(filePath.string(), ios::app);
        parentsFile << "#time infants adults total" << std::endl;
    }else{
        parentsFile.open(filePath.string(), ios::app);
    }
    int infants = 0;
    int adults = 0;
    for (int i = 0; i < organismsList.size(); i++)
    {
        Organism org = organismsList[i];
        if (org.getState() == Organism::INFANT){
            infants++;
        }else if (org.getState() == Organism::ADULT)
        {
            adults++;
        }
    }
    parentsFile << currentTime << " " << infants << " " << adults << " " << (infants+adults) << std::endl;
    parentsFile.close();
}


void EvolverController::logListProblem(std::string event, std::string message, std::string fields)
{
    ofstream file;
    file.open(RESULTS_PATH + simulationDateAndTime + "/list_problems.txt", ios::app);
    file << "TIME: " << getTime() << std::endl;
    file << "EVENT: " << event << std::endl;
    file << "MESSAGE:\n" << message << std::endl;
    file << "FIELDS:\n" << fields;
    file << "LIST:\n" << getOrganismsListAsString() << std::endl;
    file << std::endl;
    file.close();
}


int EvolverController::getRandomWait()
{
    int noise = (rand() % NOISE_GENOMES_INITIALIZATION) - (NOISE_GENOMES_INITIALIZATION/2);
    return WAITING_INTERVAL_GENOMES_INITIALIZATION + noise;
}


std::string EvolverController::getOrganismsListAsString()
{
    std::string text = "";
    for (int i = 0; i < organismsList.size(); i++)
    {
        Organism org = organismsList[i];
        text = text + std::to_string(i) + ") " + "ID: " + std::to_string(org.getId()) + "NAME: " + org.getName() + "STATE: " + std::to_string(org.getState()) + "\n";
    }
    return text;
}

////////////////////////////////////////////////////////////////
////////////////////// MESSAGE FUNCTIONS ////?//////////////////
////////////////////////////////////////////////////////////////

void EvolverController::deathMessage(std::string message, double currentTime) {
    id_t organimsID = std::atoi(MessagesManager::get(message, "ID").c_str());
    int index = searchForOrganism(organimsID);
    
    if (index >= 0)
    {
        if (organismsList[index].getState() != Organism::DEAD)
        {
            organismsList[index].setState(Organism::DEAD);
            
            std::cout << "organism_" << organimsID << " died: REMOVED FROM LIST" << std::endl;
            
            std::string log = std::to_string(getTime()) + " DEATH " + std::to_string(organimsID) + " organismsListSize " + std::to_string(organismsList.size());
            storeEventOnFile(log);
        }
    }
    else
    {
        std::string fields = " ID: " + std::to_string(organimsID) + "\n";
        std::string event = " DEATH_ANNOUNCEMENT_MESSAGE";
        logListProblem(event, message, fields);
    }
}

void EvolverController::birthMessage(std::string message, double currentTime) {
    id_t parent1 = std::atoi(MessagesManager::get(message, "PARENT1").c_str());
    id_t parent2 = std::atoi(MessagesManager::get(message, "PARENT2").c_str());
    id_t organismId = std::atoi(MessagesManager::get(message, "ORGANISM_ID").c_str());
    unsigned int size = std::atoi(MessagesManager::get(message, "SIZE").c_str());
    std::string genome = MessagesManager::get(message, "GENOME");
    std::string mind = MessagesManager::get(message, "MIND");
    
    if(parent1 > 0){
        int index = searchForOrganism(parent1);
        if (index >= 0)
        {
            organismsList[index].setOffspring(organismsList[index].getOffspring()+1);
        }
        else
        {
            std::string event = " ORGANISM_BUILT_MESSAGE (parent1)";
            std::string fields = " PARENT1: " + std::to_string(parent1) + "\n" +
            " PARENT2: " + std::to_string(parent2) + "\n" +
            " ORGANISM_ID: " + std::to_string(organismId) + "\n" +
            " SIZE: " + std::to_string(size) + "\n" +
            " GENOME: " + genome + "\n" +
            " MIND: " + mind + "\n";
            logListProblem(event, message, fields);
        }
    }
    
    if(parent2 > 0){
        int index = searchForOrganism(parent2);
        if (index >= 0)
        {
            organismsList[index].setOffspring(organismsList[index].getOffspring()+1);
        }
        else
        {
            std::string event = " ORGANISM_BUILT_MESSAGE (parent2)";
            std::string fields = " PARENT1: " + std::to_string(parent1) + "\n" +
            " PARENT2: " + std::to_string(parent2) + "\n" +
            " ORGANISM_ID: " + std::to_string(organismId) + "\n" +
            " SIZE: " + std::to_string(size) + "\n" +
            " GENOME: " + genome + "\n" +
            " MIND: " + mind + "\n";
            logListProblem(event, message, fields);
        }
        
    }
    
    std::vector<id_t> parents;
    parents.push_back(parent1);
    parents.push_back(parent2);
    
    Organism newOrganism = Organism(genome, mind, organismId, 0, size, 0, parents, Organism::INFANT);
    int idx = searchForOrganism(organismId);
    if (idx >= 0)
    {
        std::string before = getOrganismsListAsString();
        
        organismsList.erase(organismsList.begin()+idx);
        
        std::string after = getOrganismsListAsString();
        
        ofstream file;
        file.open(RESULTS_PATH + simulationDateAndTime + "/already_there.txt", ios::app);
        file << std::endl;
        file << "organism " << organismId << " was already inside the list, in position " << idx << std::endl;
        file << "list before:" << std::endl << before << std::endl;
        file << "list after:" << std::endl << after << std::endl;
        file.close();

    }
    organismsList.push_back(newOrganism);
    
    ofstream file2;
    file2.open(RESULTS_PATH + simulationDateAndTime + "/added.txt", ios::app);
    file2 << std::endl;
    file2 << "organism " << organismId << " added to list " << std::endl;
    file2 << "list:" << std::endl << getOrganismsListAsString() << std::endl;
    file2.close();
    
    std::string log = std::to_string(getTime()) + " PROCREATE " + std::to_string(parent1) + " and "  + std::to_string(parent2) + " successfully had child " + std::to_string(organismId);
    storeEventOnFile(log);
}

void EvolverController::adultMessage(std::string message, double currentTime) {
    id_t organismId = atoi(MessagesManager::get(message, "ID").c_str());
    int idx = searchForOrganism(organismId);
    if (idx >= 0)
    {
        organismsList[searchForOrganism(organismId)].setState(Organism::ADULT);
    }
    else
    {
        std::string event = " ADULT_ANNOUNCEMENT";
        std::string fields = " ID: " + std::to_string(organismId) + "\n";
        logListProblem(event, message, fields);
    }
}

void EvolverController::fitnessUpdateMessage(std::string message, double currentTime) {
    id_t organismId = atoi(MessagesManager::get(message, "ID").c_str());
    double fitness = atof(MessagesManager::get(message, "FITNESS").c_str());
    int idx = searchForOrganism(organismId);
    if (idx >= 0)
    {
        organismsList[searchForOrganism(organismId)].setFitness(fitness);
    }
    else
    {
        std::string event = " FITNESS_UPDATE";
        std::string fields = " ID: " + std::to_string(organismId) + "\n" + "FITNESS: " + std::to_string(fitness) + "\n";
        logListProblem(event, message, fields);
    }
}

void EvolverController::coupleMessage(std::string message,double currentTime) {
    try {
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
        
        bool empty = true;
        for (int i = 0; i < 100 && empty; i ++)
        {
            CppnGenome newGenome = genomeManager->createGenome(parentsGenomes);
            empty = checkEmptyPlan(newGenome);
            
            if(!empty) {
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
                
                // send new genome to birth clinic
                sendGenomeToBirthClinic(genomeManager->genomeToString(newGenome), newMind->toString(), id1, id2, fitness1, fitness2);
                
                // stop initialization
                initialization = false;
            }
        }
    }catch(LocatedException &e){
        std::cout << "Mating failed, genomeManager threw a located exception: " << e.what();
    }catch(std::exception &e){
        std::cout << "Mating failed, genomeManager threw a normal exception: " << e.what();
    }
}

void EvolverController::genomeSpreadMessage(std::string message, double currentTime) {
    id_t organismId;
    double fitness;
    std::string genomeStr;
    std::string mindStr;
    readFitnessMessage(& organismId, & fitness, &genomeStr, &mindStr, message);
    
    int idx = searchForOrganism(organismId);
    if (idx >= 0)
    {
        // update fitness and state
        organismsList[idx].setFitness(fitness);
        organismsList[idx].setState(Organism::ADULT);   // redundant
        organismsList[idx].setMind(mindStr);            // useful only for the first message from organisms not created from parents
        
        std::string log = std::to_string(getTime()) + " MESSAGE_FROM " + std::to_string(organismId)  + " organismsListSize " + std::to_string(organismsList.size());
        storeEventOnFile(log);
    }
    else
    {
        std::string event = " GENOME_SPREAD_MESSAGE";
        std::string fields = " ID: " + std::to_string(organismId) + "\n" +
        " FITNESS: " + std::to_string(fitness) + "\n" +
        " GENOME: " + genomeStr + "\n" +
        " MIND: " + mindStr + "\n";
        logListProblem(event, message, fields);
    }
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
    } else if (PARENT_SELECTION == "RANDOM") {
        parentSelectionMechanism = new RandomSelection();
    } else {
        std::cerr << "Unknown Parent Selection Mechanism: " << std::endl;
    }
    
    if(MATING_SELECTION == "EVOLVER") {
        matingType = MATING_SELECTION_BY_EVOLVER;
    } else if(MATING_SELECTION == "ORGANISMS"){
        matingType = MATING_SELECTION_BY_ORGANISMS;
    } else {
        std::cerr << "Unknown Mating Selection Mechanism: " << MATING_SELECTION << std::endl;
    }
    
    if(DEATH_SELECTION == "EVOLVER") {
        deathType = DEATH_SELECTION_BY_EVOLVER;
    } else if(DEATH_SELECTION == "TIME_TO_LIVE"){
        deathType = DEATH_SELECTION_BY_TIME_TO_LIVE;
    } else {
        std::cerr << "Unknown Death Selection Mechanism: " << DEATH_SELECTION << std::endl;
    }
    
    emitter = getEmitter(EMITTER_NAME);
    emitter->setChannel(CLINIC_CHANNEL);
    receiver = getReceiver(RECEIVER_NAME);
    receiver->setChannel(EVOLVER_CHANNEL);
    
    builder = new Builder();
}


EvolverController::~EvolverController()
{
    
}


void EvolverController::run()
{
    double TIME_STEP = getBasicTimeStep();
    
    receiver->enable(TIME_STEP);
    while (receiver->getQueueLength() > 0)
    {
        receiver->nextPacket();
    }
    
    
    /****************************************
     ***** WAIT UNTIL ENVIRONMENT IS OK *****
     ****************************************/
    
    // wait unitil EnvironmentModifierController sais the environment is ok
    bool environmentOk = false;
    while (step(TIME_STEP) != -1 && !environmentOk)
    {
        while(receiver->getQueueLength() > 0)
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
    
    
    /***************************************
     ******* CREATE THE FIRST GENOME *******
     ***************************************/
    
    CppnGenome genome = createRandomGenome();
    sendGenomeToBirthClinic(genomeManager->genomeToString(genome), "", 0, 0, 0, 0);
    
    
    // MAIN CYCLE
    
    initialization = true;
    initPopulationWaitingTime = getRandomWait();
    lastGeneratedTime = 0;
    currentTime = 0;
    lastMating = 0;      // FOR CENTRALIZED REPRODUCTION BY THE EVOLVER
    lastDeath = 0;       // FOR CENTRALIZED DEATH BY THE EVOLVER
    lastEvolutionEndCheck = 0;
    lastOffspringLoggingTime = 30;
    
    while (step(TIME_STEP) != -1)
    {
        
        currentTime = getTime();
        
        
        /***********************************
         ******* CHECK END EVOLUTION *******
         ***********************************/
        
        checkEndEvolution(currentTime);
        
        
        /*************************************
         ***** POPULATION INITIALIZATION *****
         *************************************/
        
        if (initialization && (currentTime - lastGeneratedTime > initPopulationWaitingTime))
        {
            CppnGenome newGenome = createRandomGenome();
            sendGenomeToBirthClinic(genomeManager->genomeToString(newGenome), "", 0, 0, 0, 0);
            lastGeneratedTime = getTime();
            initPopulationWaitingTime = getRandomWait();
        }
        
        
        /***************************************************************
         ************************** MANAGE MESSAGES ********************
         ***************************************************************/
        while(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            
            /**************************************
             ****** UPDATE BECAUSE OF DEATH  ******
             **************************************/
            if (message.substr(0,28).compare("[DEATH_ANNOUNCEMENT_MESSAGE]") == 0)
            {
                deathMessage(message,currentTime);
            }
            
            /*************************************
             ****** UPDATE BECAUSE OF BIRTH ******
             *************************************/
            else if (message.substr(0,24).compare("[ORGANISM_BUILT_MESSAGE]") == 0)
            {
                birthMessage(message,currentTime);
            }
            
            /*************************************
             ****** UPDATE BECAUSE OF ADULT ******
             *************************************/
            else if (message.substr(0,20).compare("[ADULT_ANNOUNCEMENT]") == 0)
            {
                adultMessage(message,currentTime);
            }
            
            /******************************
             ******* UPDATE FITNESS *******
             ******************************/
            // should be useful only for distributed
            else if (message.substr(0,16).compare("[FITNESS_UPDATE]") == 0)
            {
                fitnessUpdateMessage(message,currentTime);
            }
            
            /**************************************************************
             ******* CREATE NEW GENOME AFTER SELECTION BY ORGANISMS *******
             **************************************************************/
            else if (matingType == MATING_SELECTION_BY_ORGANISMS)
            {
                
                if (message.substr(0,16).compare("[COUPLE_MESSAGE]") == 0)
                {
                    coupleMessage(message,currentTime);
                }
            }
            
            /*************************************************
             ******* GET GENOMES FOR MATING BY EVOLVER *******
             *************************************************/
            else if (matingType == MATING_SELECTION_BY_EVOLVER)
            {
            
                if (message.substr(0,23).compare("[GENOME_SPREAD_MESSAGE]") == 0)
                {
                    genomeSpreadMessage(message,currentTime);
                }
            }
            
            receiver->nextPacket();
        }
        
        /***************************************************************
         ********************* INTERVAL OPERATIONS *********************
         ***************************************************************/
        
        /************************************************************
         ******* CREATE NEW GENOME USING SELECTION BY EVOLVER *******
         ************************************************************/
        if (matingType == MATING_SELECTION_BY_EVOLVER)
        {
        
            if(currentTime - lastMating > MATING_TIME)
            {
                std::cout << "time to mate" << std::endl;
                
                std::vector<id_t> forMating = selectForMating();
                
                if (forMating.size() == 2)
                {
                    try {
                        std::vector<CppnGenome> parentsGenomes = std::vector<CppnGenome>();
                        std::stringstream stream1(organismsList[searchForOrganism(forMating[0])].getGenome());
                        std::stringstream stream2(organismsList[searchForOrganism(forMating[1])].getGenome());
                        
                        parentsGenomes.push_back(CppnGenome(stream1));
                        parentsGenomes.push_back(CppnGenome(stream2));
                        
                        bool empty = true;
                        for (int i = 0; i < 100 && empty; i++)
                        {
                            CppnGenome newGenome = genomeManager->createGenome(parentsGenomes);
                            empty = checkEmptyPlan(newGenome);
                        
                            if(!empty) {
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
                                
                                initialization = false;
                            }
                        }
                    }catch(LocatedException &e){
                        std::cout << "Mating failed, genomeManager threw a located exception: " << e.what();
                    }catch(std::exception &e){
                        std::cout << "Mating failed, genomeManager threw a normal exception: " << e.what();
                    }
                }
                if (forMating.size() == 1)
                {
                    try{
                        std::vector<CppnGenome> parentsGenomes = std::vector<CppnGenome>();
                        std::stringstream stream1(organismsList[searchForOrganism(forMating[0])].getGenome());
                        
                        parentsGenomes.push_back(CppnGenome(stream1));
                        
                        bool empty = true;
                        for (int i = 0; i < 100 && empty; i++)
                        {
                            CppnGenome newGenome = genomeManager->createGenome(parentsGenomes);
                            empty = checkEmptyPlan(newGenome);
                        
                            if(!empty){
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
                                
                                initialization = false;
                            }
                        }
                    }catch(LocatedException &e){
                        std::cout << "Mating failed, genomeManager threw a located exception: " << e.what();
                    }catch(std::exception &e){
                        std::cout << "Mating failed, genomeManager threw a normal exception: " << e.what();
                    }
                }
                if (forMating.size() == 0)
                {
                    std::cout << "couple of parents not found" << std::endl;
                    
                    std::string log = std::to_string(getTime()) + " couple of parents not found";
                    storeEventOnFile(log);
                }
                
                lastMating = getTime();
            }
        }
        
        /***********************************
         ********** LOG OFFSPRING **********
         ***********************************/
        if(currentTime - lastOffspringLoggingTime > MATING_TIME)
        {
            storeParentsOnFile(currentTime);
            lastOffspringLoggingTime = getTime();
        }
        
        /******************************************
         ******* DEATH BY EVOLVER SELECTION *******
         ******************************************/
        if (deathType == DEATH_SELECTION_BY_EVOLVER)
        {
            if(currentTime - lastDeath > DYING_TIME)
            {
                std::vector<id_t> forDying = selectForDying();
                
                for (int i = 0; i < forDying.size(); i++)
                {
                    sendDeathMessage(forDying[i]);
                    
                    std::cout << "organism_" << forDying[i] << " SELECTED FOR DEATH" << std::endl;
                }
                
                lastDeath = getTime();
            }
        }
        
    }
}